#include <includes.hpp>
#include <cli/cli_options.hpp>
#include <core/memory_allocator.hpp>
#include <tools/gsc.hpp>
#include <tools/gsc_opcodes.hpp>
#include <tools/gsc_decompiler.hpp>

namespace {
	using namespace tool::gsc::opcode;
	using namespace tool::gsc;

	struct GscData {
		std::filesystem::path origin;
		std::vector<byte> buffer{};
		std::shared_ptr<GSCOBJHandler> reader{};
		std::unordered_map<uint64_t, std::unordered_set<uint32_t>> opcodesLocs{};
	};

	struct GscExportData {
		NameLocated nl;
		uint32_t start;
		uint32_t size{};
	};

	int gsct(int argc, const char* argv[]) {
		cli::options::CliOptions opts{};

		struct {
			bool help{};
			bool sameVm{};
			const char* testDecompDir{};
			const char* pltName{};
			bool exportConv{};
			opcode::Platform plt{ PLATFORM_PC };
		} opt;

		opts
			.addOption(&opt.help, "Show help", "--help", "", "-h")
			.addOption(&opt.sameVm, "Use same vm for new", "--sameVm", "", "-s")
			.addOption(&opt.exportConv, "Use exports", "--export", "", "-e")
			.addOption(&opt.testDecompDir, "Start decompiler with new opcodes", "--decomp", "", "-d")
			.addOption(&opt.pltName, "Input platform name", "--plt", " [plt]", "-t")
			;

		if (!opts.ComputeOptions(2, argc, argv) || opt.help || opts.ParamsCount() < 3) {
			LOG_INFO("{} {} [input] [out] [opcodes out]", argv[0], argv[1]);
			opts.PrintOptions();
			return opts.ParamsCount() < 3 ?  tool::BAD_USAGE : tool::OK;
		}

		if (opt.pltName) {
			opt.plt = opcode::PlatformOf(opt.pltName);
			if (!opt.plt) {
				LOG_ERROR("Invalid platform name '{}'", opt.pltName);
				return tool::BASIC_ERROR;
			}
		}

		RegisterOpCodesMap();

		auto LoadVmMap = [](std::unordered_map<uint64_t, GscData>& map, const char* type, const std::filesystem::path& origin, VmInfo*& vm) -> void {
			LOG_INFO("Loading {} path...", type);
			std::vector<std::filesystem::path> paths{};
			utils::GetFileRecurseExt(origin, paths, ".gscc\0.cscc\0");

			for (const std::filesystem::path& path : paths) {
				GscData data{ path };
				if (!utils::ReadFile(path, data.buffer) || data.buffer.size() < 8) {
					LOG_ERROR("Can't read {}", path.string());
					continue;
				}
				uint64_t magic{ *(uint64_t*)data.buffer.data() };

				if (!vm) {
					if (!IsValidVmMagic(magic, vm)) {
						LOG_ERROR("Invalid old vm 0x{:x}", magic);
						continue;
					}
				}
				else if (magic != vm->vmMagic) {
					throw std::runtime_error(std::format("Multiple magic detected 0x{:x} and 0x{:x}", magic, vm->vmMagic));
				}

				std::function<std::shared_ptr<GSCOBJHandler>(byte*, size_t)>* readerbuilder{ tool::gsc::GetGscReader(magic) };

				if (!readerbuilder) {
					LOG_ERROR("Missing reader builder");
					continue;
				}

				data.reader = (*readerbuilder)(data.buffer.data(), data.buffer.size());

				if (!data.reader->IsValidHeader(data.buffer.size())) {
					continue;
				}

				map[data.reader->GetName()] = std::move(data);
			}
			LOG_INFO("{} scripts loaded", map.size());
		};

		VmInfo* oldVm{};
		VmInfo* newVm{};
		std::unordered_map<uint64_t, GscData> oldScriptsMap{};
		std::unordered_map<uint64_t, GscData> newScriptsMap{};
		LoadVmMap(oldScriptsMap, "old", opts[0], oldVm);
		LoadVmMap(newScriptsMap, "new", opts[1], newVm);

		if (!oldVm || !newVm) {
			LOG_ERROR("Missing vm");
			return tool::BASIC_ERROR;
		}

		if (oldVm->HasFlag(VmFlags::VMF_OPCODE_U16) != newVm->HasFlag(VmFlags::VMF_OPCODE_U16)) {
			LOG_ERROR("Can't map vm with different op sizes");
			return tool::BASIC_ERROR;
		}

		bool sameFieldHash = oldVm->HashField("hello") == newVm->HashField("hello");
		bool sameFileHash = oldVm->HashPath("scripts/hello.gsc") == newVm->HashPath("scripts/hello.gsc");

		hashutils::ReadDefaultFile();

		std::unordered_map<const OPCodeInfo*, std::unordered_set<uint16_t>> opcodeMapping{};

		std::unique_ptr<GSCExportReader> oer{ tool::gsc::CreateExportReader(oldVm) };
		std::unique_ptr<GSCExportReader> ner{ tool::gsc::CreateExportReader(newVm) };

		for (auto& [_name, olddata] : oldScriptsMap) {

			uint64_t name;
			// map hash file names
			if (!sameFileHash) {
				const char* mappedName{ hashutils::ExtractPtr(_name) };
				if (!mappedName) {
					LOG_WARNING("No hash lookup for name {:x}, vm with different hashes", _name);
					continue;
				}
				name = newVm->HashPath(mappedName);
			}
			else {
				name = _name;
			}

			auto it{ newScriptsMap.find(name) };
			if (it == newScriptsMap.end()) {
				LOG_TRACE("ignore script {}", hashutils::ExtractTmpScript(name));
				continue;
			}

			GscData& newdata{ it->second };

			LOG_INFO("Translating {}...", hashutils::ExtractTmpScript(name));

			if (!olddata.reader->GetExportsCount() || !newdata.reader->GetExportsCount()) {
				LOG_INFO("No exports");
				continue;
			}

			GscDecompilerGlobalContext gdctx{};

			// disable output file
			gdctx.noDump = true;
			// the decompiler will put everything in this map
			gdctx.opcodesLocs = &newdata.opcodesLocs;


			if (olddata.reader->GetCSEGSize() == newdata.reader->GetCSEGSize()) {
				LOG_INFO("Find similar CSEG");

				int ret{ DecompileGsc(olddata.buffer.data(), olddata.buffer.size(), olddata.origin, gdctx) };

				if (ret) {
					LOG_ERROR("Decompiler output 0x{:x}", ret);
					continue;
				}

				LOG_INFO("Adding {} candidates", newdata.opcodesLocs[name].size());
				uint32_t csegStartOld{ olddata.reader->GetCSEGOffset() };
				uint32_t csegStartNew{ newdata.reader->GetCSEGOffset() };
				for (uint32_t loc : newdata.opcodesLocs[name]) {
					void* opbase{ &olddata.buffer[loc] };
					void* nopbase{ &newdata.buffer[(size_t)loc - csegStartOld + csegStartNew] };

					uint16_t op;
					uint16_t nop;
					if (oldVm->HasFlag(VmFlags::VMF_OPCODE_U16)) {
						op = *(uint16_t*)opbase;
						nop = *(uint16_t*)nopbase;
					}
					else {
						op = (uint16_t)(*(byte*)opbase);
						nop = (uint16_t)(*(byte*)nopbase);
					}


					const OPCodeInfo* oldNfo{ LookupOpCode(oldVm->vmMagic, opt.plt, op) };

					if (!oldNfo || oldNfo->m_id == OPCODE_Undefined) {
						continue; // ignore bad ids
					}


					opcodeMapping[oldNfo].insert(nop);
				}

				continue;
			}

			if (opt.exportConv) {
				// not working, maybe we can translate checking first same CSEG and then the export codes while ignoring strange results
				std::unordered_map<NameLocated, GscExportData*, NameLocatedHash, NameLocatedEquals> oldExportsMap{};
				std::unordered_map<NameLocated, GscExportData*, NameLocatedHash, NameLocatedEquals> newExportsMap{};
				std::vector<GscExportData> oldExports{};
				std::vector<GscExportData> newExports{};

				auto LoadExports = [](GSCOBJHandler& reader, GSCExportReader& exreader, std::vector<GscExportData>& exports, std::unordered_map<NameLocated, GscExportData*, NameLocatedHash, NameLocatedEquals>& map, VmInfo* vmInfo, bool sameFieldHash) -> void {
					uint16_t exportsCount{ reader.GetExportsCount() };
					for (size_t i = 0; i < exportsCount; i++) {
						exreader.SetHandle(reader.Ptr(reader.GetExportsOffset() + i * reader.GetExportSize()));

						GscExportData ged{};

						ged.nl.name = exreader.GetName();
						ged.nl.name_space = exreader.GetNamespace();
						ged.start = exreader.GetAddress();

						// map hash field names
						if (!sameFieldHash) {
							const char* mappedName{ hashutils::ExtractPtr(ged.nl.name) };
							const char* mappedNamespace{ hashutils::ExtractPtr(ged.nl.name_space) };
							if (!mappedName || !mappedNamespace) {
								LOG_WARNING("No hash lookup for name {}::{}, vm with different hashes", hashutils::ExtractTmp("namespace", ged.nl.name_space), hashutils::ExtractTmp("function", ged.nl.name));
								ged.nl.name = 0;
								ged.nl.name_space = 0;
							}
							else {
								ged.nl.name = vmInfo->HashField(mappedName);
								ged.nl.name_space = vmInfo->HashField(mappedNamespace);
							}
						}

						exports.emplace_back(std::move(ged));

					}
					// write end of the cseg to get the size of the last func
					exports.emplace_back(NameLocated{ 0, 0 }, reader.GetCSEGOffset() + reader.GetCSEGSize(), 0);

					// sort, shouldn't be required for now, but still good
					std::sort(exports.begin(), exports.end(), [](GscExportData& a, GscExportData& b) -> bool { return a.start < b.start; });
					for (size_t i = 0; i < exports.size() - 1; i++) {
						// compute the size using the delta with the next func
						exports[i].size = exports[i + 1].start - exports[i].start;

						if (exports[i].nl.name && exports[i].nl.name_space) {
							// add the export to the map
							map[exports[i].nl] = &exports[i];
						}
					}
				};

				LoadExports(*olddata.reader, *oer, oldExports, oldExportsMap, oldVm, sameFieldHash);
				LoadExports(*newdata.reader, *ner, newExports, newExportsMap, newVm, true);

				bool isDecompiled{};
				bool anySimilar{};
				for (auto& [nl, oged] : oldExportsMap) {
					auto ite{ newExportsMap.find(nl) };

					if (ite == newExportsMap.end()) {
						LOG_TRACE("Can't find {}::{} in new dump", hashutils::ExtractTmp("namespace", oged->nl.name_space), hashutils::ExtractTmp("function", oged->nl.name));
						continue;
					}

					GscExportData* nged{ ite->second };

					if (oged->size != nged->size) {
						LOG_TRACE("Not the same size for {}::{} in new dump", hashutils::ExtractTmp("namespace", oged->nl.name_space), hashutils::ExtractTmp("function", oged->nl.name));
						continue;
					}


					if (!isDecompiled) {
						// decompile the script
						int ret{ DecompileGsc(olddata.buffer.data(), olddata.buffer.size(), olddata.origin, gdctx) };

						if (ret) {
							LOG_ERROR("Decompiler output 0x{:x}", ret);
							break;
						}
						isDecompiled = true;
					}
					anySimilar = true;

					for (uint32_t loc : newdata.opcodesLocs[name]) {
						if (loc < oged->start || loc >= oged->start + oged->size) {
							continue; // not inside the current export
						}

						void* opbase{ &olddata.buffer[loc] };
						void* nopbase{ &newdata.buffer[(size_t)loc - oged->start + nged->start] };

						uint16_t op;
						uint16_t nop;
						if (oldVm->HasFlag(VmFlags::VMF_OPCODE_U16)) {
							op = *(uint16_t*)opbase;
							nop = *(uint16_t*)nopbase;
						}
						else {
							op = (uint16_t)(*(byte*)opbase);
							nop = (uint16_t)(*(byte*)nopbase);
						}


						const OPCodeInfo* oldNfo{ LookupOpCode(oldVm->vmMagic, opt.plt, op) };

						if (!oldNfo || oldNfo->m_id == OPCODE_Undefined) {
							continue; // ignore bad ids
						}


						opcodeMapping[oldNfo].insert(nop);
					}
				}

				if (anySimilar) continue;
			}
			

			// compute exports list
			LOG_WARNING("No similar data to exploit for {}", hashutils::ExtractTmpScript(name));
		}

		{
			LOG_INFO("Dump into {}", opts[2]);
			utils::OutFileCE os{ opts[2] };

			if (!os) {
				LOG_ERROR("Can't open {}", opts[2]);
				return tool::BASIC_ERROR;
			}

			for (auto& [oldNfo, newVals] : opcodeMapping) {
				os << oldNfo->m_name << " ->";

				if (!oldVm->HasFlag(VmFlags::VMF_OPCODE_U16) && newVals.size() != 1) {
					LOG_WARNING("Invalid mapping for opcode {}, {} results", oldNfo->m_name, newVals.size());
				}

				for (uint16_t v : newVals) {
					os << " 0x" << std::hex << (int)v;
				}

				os << "\n";
			}
		}

		if (opt.testDecompDir) {
			// register opcodes
			newVm->AddPlatform(PLATFORM_ACTS_TEST);
			newVm->ClearPlatformOpCode(PLATFORM_ACTS_TEST);

			LOG_INFO("Registering new opcodes...");
			for (auto& [oldNfo, newVals] : opcodeMapping) {
				for (uint16_t v : newVals) {
					newVm->RegisterOpCode(PLATFORM_ACTS_TEST, oldNfo->m_id, v);
				}
			}

			LOG_INFO("Decompiling...");
			GscDecompilerGlobalContext outdctx{};

			outdctx.opt.m_platform = PLATFORM_ACTS_TEST;
			outdctx.opt.m_outputDir = opt.testDecompDir;
			outdctx.opt.m_dcomp = true;
			outdctx.opt.m_splitByVm = true;

			
			int r{ tool::OK };
			for (auto& [k, v] : newScriptsMap) {

				int ores{ tool::gsc::DecompileGsc(v.buffer.data(), v.buffer.size(), v.origin, outdctx) };

				if (ores) {
					LOG_ERROR("Error when decompiling {}", hashutils::ExtractTmpScript(k));
					r = tool::BASIC_ERROR;
				}
			}

			LOG_INFO("Decompiled test dump into {}", opt.testDecompDir);
			return r;
		}

		return tool::OK;
	}

	ADD_TOOL(gsct, "gsc", "[old] [new] [output]", "Translate gsc opcodes from one VM to another", gsct);
}