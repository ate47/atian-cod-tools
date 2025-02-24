#include <includes.hpp>
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
	};

	int gsct(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 3)) return tool::BAD_USAGE;

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

				auto readerbuilder{ tool::gsc::GetGscReader(magic) };

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
		LoadVmMap(oldScriptsMap, "old", argv[2], oldVm);
		LoadVmMap(newScriptsMap, "new", argv[3], newVm);

		hashutils::ReadDefaultFile();

		std::unordered_map<byte, std::unordered_set<byte>> opcodeMapping{};

		for (auto& [name, olddata] : oldScriptsMap) {
			auto it{ newScriptsMap.find(name) };
			if (it == newScriptsMap.end()) {
				LOG_TRACE("ignore script {}", hashutils::ExtractTmpScript(name));
				continue;
			}

			GscData& newdata{ it->second };

			LOG_INFO("Translating {}...", hashutils::ExtractTmpScript(name));

			if (olddata.reader->GetCSEGOffset() == newdata.reader->GetCSEGOffset()
				&& olddata.reader->GetCSEGSize() == newdata.reader->GetCSEGSize()) {
				LOG_INFO("Find similar CSEG");


				GscDecompilerGlobalContext gdctx{};
				std::unordered_map<uint64_t, std::unordered_set<uint32_t>> opcodesLocs{};

				// disable output file
				gdctx.noDump = true;
				// the decompiler will put everything in this map
				gdctx.opcodesLocs = &opcodesLocs;

				int ret{ DecompileGsc(olddata.buffer.data(), olddata.buffer.size(), olddata.origin, gdctx) };

				if (ret) {
					LOG_ERROR("Decompiler output 0x{:x}", ret);
					continue;
				}

				LOG_INFO("Adding {} candidates", opcodesLocs[name].size());

				for (uint32_t loc : opcodesLocs[name]) {
					opcodeMapping[*(byte*)(&olddata.buffer[loc])].insert(*(byte*)(&newdata.buffer[loc]));
				}

				continue;
			}

			LOG_WARNING("No similar data to exploit for {}", hashutils::ExtractTmpScript(name));
		}

		{

			LOG_INFO("Dump into {}", argv[4]);
			utils::OutFileCE os{ argv[4] };

			if (!os) {
				LOG_ERROR("Can't open {}", argv[4]);
				return tool::BASIC_ERROR;
			}

			for (auto& [oldVal, newVals] : opcodeMapping) {

				const OPCodeInfo* oldNfo{ LookupOpCode(oldVm->vmMagic, tool::gsc::opcode::PLATFORM_PC, oldVal) };

				if (!oldNfo || oldNfo->m_id == OPCODE_Undefined) {
					continue; // ignore bad ids
				}

				os << oldNfo->m_name << " ->";

				for (byte v : newVals) {
					os << " 0x" << std::hex << (int)v;
				}

				os << "\n";
			}
		}

		return tool::OK;
	}

	ADD_TOOL(gsct, "gsc", " [old] [new] [output]", "Translate gsc opcodes from one VM to another", gsct);
}