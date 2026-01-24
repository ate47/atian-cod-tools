#include <includes.hpp>
#include <cli/cli_options.hpp>
#include <tools/gsc/gsc.hpp>
#include <tools/gsc/decompiler/gsc_decompiler_object.hpp>
#include <decryptutils.hpp>
#include <core/memory_allocator.hpp>

namespace {
	int searcher_gsc(int argc, const char* argv[]) {
		struct {
			bool help{};
			const char* splt{};
			tool::gsc::opcode::Platform plt{ tool::gsc::opcode::PLATFORM_PC };
		} opt{};
		cli::options::CliOptions opts{};

		opts
			.addOption(&opt.help, "show help", "--help", "", "-h")
			.addOption(&opt.splt, "platform", "--plt", " [p]", "-p")
			;

		if (!opts.ComputeOptions(2, argc, argv) || opt.help) {
			opts.PrintOptions();
			return tool::OK;
		}

		if (opts.NotEnoughParam(2)) {
			return tool::BAD_USAGE;
		}

		if (opt.splt) {
			opt.plt = tool::gsc::opcode::PlatformOf(opt.splt);
			if (!opt.splt) {
				LOG_ERROR("Invalid platform: {}", opt.splt);
				return tool::BAD_USAGE;
			}
		}

		LOG_INFO("Load common scripts");
		hashutils::ReadDefaultFile();
		std::vector<std::filesystem::path> scripts{};
		utils::GetFileRecurseExt(opts[0], scripts, ".gscc\0.cscc\0");

		if (scripts.empty()) {
			LOG_ERROR("Can't find any script");
			return tool::BASIC_ERROR;
		}

		tool::gsc::GscDecompilerGlobalContext ctx{};
		ctx.noDump = true;
		ctx.opt.m_header = true;
		ctx.opt.m_platform = opt.plt;
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
				std::vector<const char*> gscstr{};
				ctx.stringsLoc = &gscstr;

				int r{ tool::gsc::DecompileGsc((byte*)bufferAlign, bufferSizeAlign, path, ctx) };
				if (r) {
					res = r;
					LOG_WARNING("Error when decompiling {}", path.string());
					continue;
				}

				for (const char* str : gscstr) {
					scriptStrings.emplace_back(alloc.CloneStr(str));
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
		utils::OutFileCE os{ opts[1], false, std::ios::app | std::ios::out };
		if (!os) {
			LOG_ERROR("Can't open {}", opts[1]);
			return tool::BASIC_ERROR;
		}

		auto CheckHash = [&hashes, &os](uint64_t val, const char* str) -> bool {
			auto it{ hashes.find(val & hashutils::MASK60) };
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
			r |= CheckHash(hash::HashIWAsset(str), str);
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

		size_t count{ scriptStrings.size() };
		size_t steps{ std::max<size_t>(count / 100, 1) };
		size_t id{};
		for (const char* loadedString : scriptStrings) {
			if ((id++ % steps) == 0) {
				LOG_TRACE("completed {}% ({}/{})", 100 * id / count, id, count);
			}

			char* w{ utils::CloneString(loadedString) };

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