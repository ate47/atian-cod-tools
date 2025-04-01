#include <includes.hpp>
#include <tools/gsc.hpp>
#include <tools/gsc_decompiler.hpp>
#include <decryptutils.hpp>
#include <core/memory_allocator.hpp>

namespace {
	int searcher_gsc(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 2)) return tool::BAD_USAGE;

		LOG_INFO("Load common scripts");
		hashutils::ReadDefaultFile();
		std::vector<std::filesystem::path> scripts{};
		utils::GetFileRecurseExt(argv[2], scripts, ".gscc\0.cscc\0");

		if (scripts.empty()) {
			LOG_ERROR("Can't find any script");
			return tool::BASIC_ERROR;
		}

		tool::gsc::GscDecompilerGlobalContext ctx{};
		ctx.noDump = true;
		ctx.opt.m_header = true;
		std::unordered_set<uint64_t> hashes{};
		std::vector<byte> scriptData{};
		std::vector<const char*> scriptStrings{};
		std::vector<const char*> scriptPaths{};
		core::memory_allocator::MemoryAllocator alloc{};
		{
			hashutils::SetExtracted(&hashes);
			utils::CloseEnd ceex{ []() { hashutils::SetExtracted(nullptr); } };

			int res{ tool::OK };
			for (const std::filesystem::path& path : scripts) {
				void* bufferAlign;
				size_t bufferSizeAlign;
				if (!utils::ReadFileAlign(path, scriptData, bufferAlign, bufferSizeAlign) || bufferSizeAlign < 8) {
					LOG_ERROR("Can't read {}", path.string());
					continue;
				}

				int r{ tool::gsc::DecompileGsc((byte*)bufferAlign, bufferSizeAlign, path, ctx) };
				if (r) {
					res = r;
					LOG_WARNING("Error when decompiling {}", path.string());
					continue;
				}

				uint64_t magic{ *(uint64_t*)bufferAlign };
				auto* builder{ tool::gsc::GetGscReader(magic) };
				if (!builder) {
					LOG_WARNING("No reader for magic {:x}", magic);
					continue;
				}

				std::shared_ptr<tool::gsc::GSCOBJHandler> reader{ (*builder)((byte*)bufferAlign, bufferSizeAlign) };
				const char* extName{ hashutils::ExtractPtr(reader->GetName()) };
				if (extName) {
					scriptPaths.emplace_back(extName);
					scriptStrings.emplace_back(extName);
				}

				size_t stringsCount{ reader->GetStringsCount() };
				if (stringsCount) {
					tool::gsc::T8GSCString* strings{ reader->Ptr<tool::gsc::T8GSCString>(reader->GetStringsOffset()) };

					for (size_t i = 0; i < stringsCount; i++) {
						if (magic == tool::gsc::opcode::VMI_T8_36) {
							scriptStrings.emplace_back(alloc.CloneStr(acts::decryptutils::DecryptStringT8(reader->Ptr<char>(strings->string))));
						}
						else {
							scriptStrings.emplace_back(alloc.CloneStr(acts::decryptutils::DecryptString(reader->Ptr<char>(strings->string))));
						}

						// skip addresses
						strings = (tool::gsc::T8GSCString*)((uint32_t*)(strings + 1) + strings->num_address);
					}
				}

				size_t animDoubleCount{ reader->GetAnimTreeDoubleCount() };

				if (animDoubleCount) {
					tool::gsc::GSC_ANIMTREE_ITEM* anims{ reader->Ptr<tool::gsc::GSC_ANIMTREE_ITEM>(reader->GetAnimTreeDoubleOffset()) };

					for (size_t i = 0; i < animDoubleCount; i++) {
						scriptStrings.emplace_back(alloc.CloneStr(acts::decryptutils::DecryptString(reader->Ptr<char>(anims->address_str1))));
						scriptStrings.emplace_back(alloc.CloneStr(acts::decryptutils::DecryptString(reader->Ptr<char>(anims->address_str2))));

						// skip addresses
						anims = (tool::gsc::GSC_ANIMTREE_ITEM*)((uint32_t*)(anims + 1) + anims->num_address);
					}
				}

				size_t animSingleCount{ reader->GetAnimTreeSingleCount() };

				if (animSingleCount) {
					tool::gsc::GSC_USEANIMTREE_ITEM* anims{ reader->Ptr<tool::gsc::GSC_USEANIMTREE_ITEM>(reader->GetAnimTreeSingleOffset()) };

					for (size_t i = 0; i < animSingleCount; i++) {
						scriptStrings.emplace_back(alloc.CloneStr(acts::decryptutils::DecryptString(reader->Ptr<char>(anims->address))));

						// skip addresses
						anims = (tool::gsc::GSC_USEANIMTREE_ITEM*)((uint32_t*)(anims + 1) + anims->num_address);
					}
				}

			}
			if (res) return res;
		}
		// cleanup known hashes
		auto it = hashes.begin();

		while (it != hashes.end()) {
			const char* ptr{ hashutils::ExtractPtr(*it) };
			if (ptr) {
				scriptStrings.emplace_back(alloc.CloneStr(ptr));
				it = hashes.erase(it);
			}
			else {
				it++;
			}
		}

		LOG_INFO("{} string(s) loaded", scriptStrings.size());
		LOG_INFO("{} script path(s) loaded", scriptPaths.size());
		LOG_INFO("{} hash(es) loaded", hashes.size());
		utils::OutFileCE os{ argv[3] };
		if (!os) {
			LOG_ERROR("Can't open {}", argv[3]);
			return tool::BASIC_ERROR;
		}

		auto CheckHash = [&hashes, &os](uint64_t val, const char* str) -> bool {
			auto it{ hashes.find(val & hashutils::MASK62) };
			if (it != hashes.end()) {
				// remove from searched list
				hashes.erase(it);

				// display
				os << std::hex << val << "," << str << "\n";
				LOG_INFO("{:x},{}", val, str);
				return true;
			}
			return false;
		};
		auto CheckHashes = [&CheckHash](const char* str) -> bool {
			bool r{};
			r |= CheckHash(hash::Hash64A(str), str);
			r |= CheckHash(hash::HashIWRes(str), str);
			r |= CheckHash(hash::HashIWDVar(str), str);
			r |= CheckHash(hash::HashJupScr(str), str);
			r |= CheckHash(hash::HashT10Scr(str), str);
			r |= CheckHash(hash::HashT10ScrSP(str), str);
			return r;
		};

		std::unordered_map<uint64_t, const char*> paths{};

		for (const char* scriptPath : scriptPaths) {
			std::string_view sw{ scriptPath };
			size_t idx{ sw.find_last_of('/') };

			if (idx == std::string::npos) continue; // invalid path

			char* w{ utils::CloneString(scriptPath) };

			w[idx + 1] = 0; // keep only the path

			const char*& path{ paths[hash::Hash64(w)] };

			if (!path) {
				path = alloc.CloneStr(w);
			}
		}
		LOG_INFO("{} path(s) loaded", paths.size());
		auto CheckAllHashes = [&paths, &CheckHashes](const char* str) -> void {
			static char buff[0x200]{};
			sprintf_s(buff, "%s", str);

			CheckHashes(buff);

			for (auto& [h, path] : paths) {
				if (CheckHashes(utils::va("%s%s.gsc", path, buff))) {
					// search csc if we found a result
					CheckHashes(utils::va("%s%s.csc", path, buff));
				}
			}
		};

		for (const char* loadedString : scriptStrings) {
			char* w;

			w = utils::CloneString(loadedString);

			size_t len{ std::strlen(w) };

			// check by reducing the string
			for (size_t i = len; i; i--) {
				w[i] = 0;
				CheckAllHashes(w);
			}

			// check by reducing the string on the other side
			for (size_t i = 1; i < len; i++) {
				CheckAllHashes(loadedString + i);
			}

			// filename search
			w = utils::CloneString(loadedString);
			char* l{ w };
			while (*l) {
				char* next{ l };
				while (*next && (*next != '_' && *next != '/' && *next != '\\')) {
					next++;
				}
				if (!*next) break; // already tested

				*next = 0;

				CheckAllHashes(l);

				l = next + 1;
			}


			w = utils::CloneString(loadedString);

			std::string_view sw{ loadedString };
			size_t idx{ sw.find_last_of('.') };
			if (idx == std::string::npos) {
				idx = sw.length();
			}

			w[idx] = 0;

			size_t idx2{ sw.find_last_of('/') };
			if (idx2 == std::string::npos) {
				CheckAllHashes(w);
			}
			else {
				CheckAllHashes(w + idx2);
			}
		}
		LOG_INFO("done");
		return tool::OK;
	}

	ADD_TOOL(searcher_gsc, "hash", " (dump) (output)", "Extrapolate hashes using GSC strings", searcher_gsc);
}