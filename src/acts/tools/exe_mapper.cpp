#include <includes.hpp>
#include <games/cod/asset_names.hpp>
#include <games/bo4/scriptinstance.hpp>
#include <utils/io_utils.hpp>
#include <hook/module_mapper.hpp>
#include <hook/error.hpp>
#include <hook/memory.hpp>
#include <utils/decrypt.hpp>
#include <DbgHelp.h>
#pragma comment(lib, "imagehlp.lib")
#include "tools/gsc.hpp"
#include "tools/pool.hpp"
#include "tools/bo6/bo6.hpp"
#include <core/memory_allocator.hpp>
#include <decryptutils.hpp>

namespace {
	using namespace tool::gsc::opcode;

	int exe_mapper(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 1)) return tool::BAD_USAGE;


		const char* exe{ argv[2] };

		hook::module_mapper::Module mod{ true };

		LOG_INFO("Loading module {}", exe);
		if (!mod.Load(exe) || !mod) {
			LOG_ERROR("Can't load module");
			return tool::BASIC_ERROR;
		}

		LOG_INFO("Loaded");

		LOG_INFO("{}", (void*)mod->GetNTHeader());
		LOG_INFO("{:x}", mod->GetOptHeader()->ImageBase);
		LOG_INFO("{}", *mod);
		LOG_INFO("{}", mod->Get<void>(0ull));

		
		utils::OutFileCE outVC{ "imports.txt" };
		LOG_INFO("imports.txt");

		hook::library::Library lib{ *mod };

		IMAGE_DATA_DIRECTORY& dir{ lib.GetOptHeader()->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT] };
		if (!dir.VirtualAddress || dir.Size <= 0) return 0; // nothing to patch

		PIMAGE_IMPORT_DESCRIPTOR imports{ (PIMAGE_IMPORT_DESCRIPTOR)(lib[dir.VirtualAddress]) };

		LOG_TRACE("Loading imports 0x{:x}/0x{:x}", dir.VirtualAddress, dir.Size);

		std::ostringstream osnames{};

		Process proc{ L"acts.exe" };

		while (imports->Name) {
			IMAGE_IMPORT_DESCRIPTOR& imp{ *imports++ };

			const char* name{ lib.Get<const char>(imp.Name) };

			if (core::logs::getlevel() <= core::logs::LVL_TRACE) osnames << " " << name;

			hook::library::Library dep{ name, false, DONT_RESOLVE_DLL_REFERENCES | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS };

			outVC << dep << ":\n";

			if (!dep) {
				osnames << "<missing>";
				LOG_TRACE("Can't load IAT lib {} to patch {}", name, *lib);
				continue;
			}

			//lib.Get<IMAGE_THUNK_DATA>(imp.)

			IMAGE_THUNK_DATA* thunks{ lib.Get<IMAGE_THUNK_DATA>(imp.FirstThunk) };
			IMAGE_THUNK_DATA* originalThunks{ lib.Get<IMAGE_THUNK_DATA>(imp.OriginalFirstThunk) };

			while (originalThunks->u1.Function) {
				IMAGE_THUNK_DATA& thunk{ *thunks++ };
				IMAGE_THUNK_DATA& originalThunk{ *originalThunks++ };

				if (originalThunk.u1.Ordinal & IMAGE_ORDINAL_FLAG64) {
					outVC << std::hex << "0x" << originalThunk.u1.Ordinal << "-> " << hook::library::CodePointer{&thunk} << "\n";
				}
				else {
					outVC << lib.Get<const char>(originalThunk.u1.Function + 2) << "-> " << hook::library::CodePointer{ &thunk } << "\n";
				}
			}
		}
		//tool::pool::WriteHex(std::cout, 0, mod->GetOptHeader(), sizeof(*mod->GetOptHeader()));

		// TEST BO4 Dump data

		/*
		struct T8BuiltInFunc {
			uint32_t name;
			uint32_t minArgs;
			uint32_t maxArgs;
			uint32_t pad01;
			uintptr_t function;
			uint32_t type;
			uint32_t pad02;
		};

		const T8BuiltInFunc* defs = mod->Get<T8BuiltInFunc>(0x4f437c0);
		constexpr size_t len = 370;

		for (size_t i = 0; i < len; i++) {
			LOG_INFO("{} -> {}", hashutils::ExtractTmp("function", defs[i].name), mod->RlocBased(defs[i].function));
		}
		*/

		/*
		// Dump bo6 pool names
		auto* pools{ mod->Get<uintptr_t>(0x8FFBE40) };

		size_t idx{};
		while (pools[idx]) {
			const char* cc = mod->Rebase<const char>(pools[idx]);
			char* c = utils::CloneString(cc);
			LOG_INFO("    T10R_ASSET_{} = 0x{:x},", utils::UpperCase(c), idx);
			idx++;
		}
		*/

		LOG_INFO("Done");

		return tool::OK;
	}
	int exe_pool_dumper(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 3)) {
			LOG_ERROR("Different values (first, last):");
			LOG_ERROR("BO4 ...... \"physpreset\" \"assetlist\" ");
			LOG_ERROR("BOCW ..... \"zone\" \"\" ");
			LOG_ERROR("BO7 ...... \"physicssfxeventasset\" \"string\" ");
			LOG_ERROR("IW games . \"physicslibrary\" \"string\" ");
			return tool::BAD_USAGE;
		}
		if (!argv[3][0]) {
			LOG_ERROR("Can't search empty first element");
			return tool::BAD_USAGE;
		}

		std::filesystem::path exe{ argv[2] };

		hook::module_mapper::Module mod{ true };

		LOG_INFO("Loading module {}", exe.string());
		if (!mod.Load(exe) || !mod) {
			LOG_ERROR("Can't load module");
			return tool::BASIC_ERROR;
		}

		const char* first{ argv[3] };
		const char* last{ argv[4][0] ? argv[4] : nullptr };
		std::filesystem::path dir{ tool::NotEnoughParam(argc, 4) ? exe.parent_path() : argv[5] };

		games::cod::asset_names::AssetNames assets{ first, last };
		games::cod::asset_names::AssetDumpFileOptions opts{};
		opts.logLevel = core::logs::LVL_INFO;

		if (!tool::NotEnoughParam(argc, 5)) {
			opts.assetPrefix = argv[6];
		}

		assets.InitMap(*mod);
		assets.DumpFiles(dir, &opts);

		return tool::OK;
	}

	int sp24_data_dump(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 1)) return tool::BAD_USAGE;


		std::filesystem::path exe{ argv[2] };

		hook::module_mapper::Module mod{ true };

		LOG_INFO("Loading module {}", exe.string());
		if (!mod.Load(exe) || !mod) {
			LOG_ERROR("Can't load module");
			return tool::BASIC_ERROR;
		}

		LOG_INFO("Loaded");
		LOG_INFO("base: {:x}", mod->GetOptHeader()->ImageBase);

		// Dump bo6 pool names
		auto* pools{ mod->Get<uintptr_t>(0x8DFCC90) };
		{
			std::filesystem::path pf{ exe.parent_path() / "pools.csv" };
			std::ofstream osf{ pf };
			if (!osf) {
				LOG_ERROR("Can't open {}", pf.string());
			}
			else {
				utils::CloseEnd osfce{ osf };
				osf << "id,name";
				for (size_t idx = 0; idx < bo6::T10R_ASSET_COUNT; idx++) {
					const char* cc = mod->Rebase<const char>(pools[idx]);
					osf << "\n" << std::dec << idx << "," << cc;
				}
			}
			LOG_INFO("Created {}", pf.string());
		}

		return tool::OK;
	}

	int scripts_decrypt(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 4)) return tool::BAD_USAGE;

		const char* exe{ argv[2] };
		VmInfo* nfo{};
		VMId type{ VMOf(argv[3]) };
		const char* scripts{ argv[4] };
		const char* output{ argv[5] };

		if (!type || !IsValidVm(type, nfo)) {
			LOG_ERROR("Can't find type {}", argv[3]);
			return tool::BAD_USAGE;
		}
		std::vector<std::filesystem::path> paths{};

		utils::GetFileRecurse(scripts, paths, [](const std::filesystem::path& p) {
			auto s = p.string();
			return s.ends_with(".gscc") || s.ends_with(".cscc") || s.ends_with(".gscbin") || s.ends_with(".cscbin");
			});

		if (paths.empty()) {
			LOG_ERROR("Not gsc files found in {}", scripts);
			return tool::BASIC_ERROR;
		}

		LOG_INFO("Loading module {}", exe);

		if (!acts::decryptutils::LoadDecrypt(exe)) {
			return tool::BASIC_ERROR;
		}

		LOG_INFO("Module loaded");


		std::function<char* (char*)> DecryptString = [](char* s) -> char* {
			char* sd = acts::decryptutils::DecryptString(s);

			// save the decrypted string at the same location
			if (sd != s) {
				memmove(s, sd, std::strlen(sd) + 1);
			}

			return s;
		};

		hashutils::ReadDefaultFile();

		std::filesystem::path outdir{ output };

		std::string buffer{};
		size_t c{};
		for (const std::filesystem::path& path : paths) {
			if (!utils::ReadFile(path, buffer) || buffer.size() < sizeof(uint64_t)) {
				LOG_ERROR("Can't read {}", path.string());
				continue; // ignore
			}

			uint64_t magic{ *(uint64_t*)buffer.data() };

			VmInfo* nfo{};

			if (!IsValidVmMagic(magic, nfo)) {
				LOG_ERROR("Can't read {}: Unknown magic {:x}", path.string(), magic);
				continue; // ignore
			}

			auto* hbuilder{ tool::gsc::GetGscReader(nfo->vmMagic) };

			if (!hbuilder) {
				LOG_ERROR("Can't read {}: No gsc handler builder for vm {}", path.string(), nfo->name);
				continue;
			}
			auto handler{ (*hbuilder)((byte*)buffer.data(), buffer.size()) };

			if (!handler->IsValidHeader(buffer.size())) {
				LOG_ERROR("Can't read {}: Invalid header", path.string());
				continue;
			}

			size_t strscount{ handler->GetStringsCount() };
			size_t animtcount{ handler->GetAnimTreeDoubleCount() };
			size_t animtucount{ handler->GetAnimTreeSingleCount() };

			LOG_DEBUG("Decrypting {} ({}) ({}/{}/{})...", path.string(), hashutils::ExtractTmpScript(handler->GetName()), strscount, animtcount, animtucount);

			// strings table

			if (strscount) {
				tool::gsc::T8GSCString* strs{ handler->Ptr<tool::gsc::T8GSCString>(handler->GetStringsOffset()) };
				size_t i = 0;
				for (; i < strscount; i++) {
					size_t rloc{ (size_t)((byte*)strs - handler->Ptr()) };


					if (rloc + sizeof(*strs) + sizeof(uint32_t) * strs->num_address > buffer.size() || strs->string >= buffer.size()) {
						LOG_ERROR("Can't decrypt {}: String too far 0x{:x} >= {:x}", path.string(), strs->string, buffer.size());
						break;
					}
					DecryptString(handler->Ptr<char>(strs->string));

					strs = (tool::gsc::T8GSCString*)&((uint32_t*)(&strs[1]))[strs->num_address];
				}
				if (i != strscount) {
					continue;
				}
				LOG_TRACE("{} string(s) decrypted", i);
			}

			// animtrees tables


			if (animtcount) {
				tool::gsc::GSC_ANIMTREE_ITEM* animt{ handler->Ptr<tool::gsc::GSC_ANIMTREE_ITEM>(handler->GetAnimTreeDoubleOffset()) };
				size_t j = 0;
				for (; j < animtcount; j++) {
					size_t rloc{ (size_t)((byte*)animt - handler->Ptr()) };
					if (rloc + sizeof(*animt) + sizeof(uint32_t) * animt->num_address > buffer.size() || animt->address_str1 >= buffer.size() || animt->address_str2 >= buffer.size()) {
						LOG_ERROR("Can't decrypt {}: Anim2 too far", path.string());
						break;
					}
					DecryptString(handler->Ptr<char>(animt->address_str1));
					DecryptString(handler->Ptr<char>(animt->address_str2));

					animt = reinterpret_cast<tool::gsc::GSC_ANIMTREE_ITEM*>(reinterpret_cast<uint32_t*>(&animt[1]) + animt->num_address);
				}
				if (j != animtcount) {
					continue;
				}
				LOG_TRACE("{} anim2(s) decrypted", j);
			}

			if (animtucount) {
				tool::gsc::GSC_USEANIMTREE_ITEM* animtu{ handler->Ptr<tool::gsc::GSC_USEANIMTREE_ITEM>(handler->GetAnimTreeSingleOffset()) };
				size_t k = 0;
				for (; k < animtucount; k++) {
					size_t rloc{ (size_t)((byte*)animtu - handler->Ptr()) };
					if (rloc + sizeof(*animtu) + sizeof(uint32_t) * animtu->num_address > buffer.size() || animtu->address >= buffer.size()) {
						LOG_ERROR("Can't decrypt {}: Anim1 too far", path.string());
						break;
					}

					DecryptString(handler->Ptr<char>(animtu->address));

					animtu = reinterpret_cast<tool::gsc::GSC_USEANIMTREE_ITEM*>(reinterpret_cast<uint32_t*>(&animtu[1]) + animtu->num_address);
				}
				if (k != animtucount) {
					continue;
				}
				LOG_TRACE("{} anim1(s) decrypted", k);
			}

			std::filesystem::path outdirvm{ outdir / tool::gsc::opcode::VMIdFancyName(nfo->vmMagic) };
			std::filesystem::create_directories(outdirvm);
			std::filesystem::path outfile{ outdirvm / utils::va("script_%llx.gscc", handler->GetName()) };

			if (!utils::WriteFile(outfile, buffer)) {
				LOG_ERROR("Can't write to '{}'", outfile.string());
				continue;
			}

			size_t total{ strscount + animtucount + animtcount };
			LOG_INFO("Decrypted {} ({}) into {} / {} (0x{:x}) string(s)", path.string(), hashutils::ExtractTmpScript(handler->GetName()), outfile.string(), total, total);
			c++;
		}
		LOG_INFO("{} (0x{:x}) script(s) decrypted", c, c);

		return tool::OK;
	}

	constexpr bool IsValidChar(char c) {
		return (c >= 0x20 && c < 0x7F) || c == '\n' || c == '\r' || c == '\t';
	}

	int read_strings(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 2)) {
			return tool::BAD_USAGE;
		}

		std::filesystem::path exe{ argv[2] };
		std::filesystem::path output{ argv[3] };
		int64_t minSizeOpt{ tool::NotEnoughParam(argc, 3) ? 4 : utils::ParseFormatInt(argv[4]) };

		if (minSizeOpt < 1) {
			LOG_ERROR("Invalid min size {}", minSizeOpt);
			return tool::BASIC_ERROR;
		}
		size_t minSize{ (size_t)minSizeOpt };

		LOG_INFO("Loading {} -> {} with min size {}", exe.string(), output.string(), minSize);

		std::string file{};

		if (!utils::ReadFile(exe, file)) {
			LOG_ERROR("Can't load file {}", exe.string());
			return tool::BASIC_ERROR;
		}
		{
			std::ofstream os{ output };
			if (!os) {
				LOG_ERROR("Can't open output {}", output.string());
				return tool::BASIC_ERROR;
			}
			utils::CloseEnd osce{ os };

			char* start{ file.data() };
			char* ptr{ file.data() + file.size() - 1 };

			while (ptr > start) {
				if (*ptr) {
					ptr--;
					continue;
				}

				size_t len{};

				while (ptr > start && IsValidChar(ptr[-1])) {
					ptr--;
					len++;
				}

				if (len >= minSize) {
					os << ptr << "\n";
				}

				ptr--;
			}
		}
		LOG_INFO("Dump into {}", output.string());

		return tool::OK;
	}

	int exe_scan(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 2)) return tool::BAD_USAGE;

		const char* exe{ argv[2] };
		const char* pattern{ argv[3] };

		hook::module_mapper::Module mod{ true };

		if (!mod.Load(exe, false)) {
			LOG_ERROR("no load");
			return tool::BASIC_ERROR;
		}

		for (auto& patt : mod->Scan(pattern)) {
			LOG_INFO("{} -> {}", pattern, (void*)patt.location);
		}

		return tool::OK;
	}


	ADD_TOOL(exe_mapper, "dev", "[exe]", "Map exe in memory", exe_mapper);
	ADD_TOOL(exe_scan, "dev", "[exe] [pattern]", "Scan exe", exe_scan);
	ADD_TOOL(read_strings, "dev", "[file] [output] (min size=4)", "Dump file strings", read_strings);
	ADD_TOOL(exe_pool_dumper, "common", "[exe] [start] [end] (outfile) (prefix)", "Dump pool names", exe_pool_dumper);
	ADD_TOOL(sp24_data_dump, "bo6", "[exe]", "Dump common data from an exe dump", sp24_data_dump);
	ADD_TOOL(scripts_decrypt, "gsc", "[exe] [type] [scripts] [ouput]", "Map exe in memory and use it to decrypt GSC scripts", scripts_decrypt);


}