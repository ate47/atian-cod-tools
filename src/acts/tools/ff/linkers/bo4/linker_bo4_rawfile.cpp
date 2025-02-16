#include <includes.hpp>
#include <tools/ff/linkers/linker_bo4.hpp>

namespace fastfile::linker::bo4 {
	class RawFileWorker : public LinkerWorker {
	public:
		RawFileWorker() : LinkerWorker("RawFile") {}

		void Compute(BO4LinkContext& ctx) override {
			std::vector<std::filesystem::path> files{};
			utils::GetFileRecurseExt(ctx.linkCtx.input, files,
				// all the extensions to read into the rawfile assets
				".cfg\0"
				".txt\0"
				".vision\0"
				".graph\0"
				".baseline\0"
				// extension used in the dump
				".raw\0" 
				, true);

			for (const std::filesystem::path& rfpath : files) {
				std::filesystem::path path{ ctx.linkCtx.input / rfpath };
				std::string buffer{};
				if (!utils::ReadFile(path, buffer)) {
					LOG_ERROR("Can't read {}", path.string());
					ctx.error = true;
					continue;
				}

				struct RawFile {
					XHash name;
					int32_t len;
					uintptr_t buffer;
				}; static_assert(sizeof(RawFile) == 0x20);

				RawFile& rf{ utils::Allocate<RawFile>(ctx.assetData) };

				uint64_t hash = HashPathName(rfpath);
				rf.name.hash = hash;
				rf.buffer = fastfile::ALLOC_PTR;
				rf.len = (uint32_t)buffer.size();

				// write header
				utils::WriteValue(ctx.assetData, buffer.data(), buffer.length() + 1); // add \0
				ctx.assets.emplace_back(games::bo4::pool::ASSET_TYPE_RAWFILE, fastfile::ALLOC_PTR);
				LOG_INFO("Added asset rawfile {} (hash_{:x})", path.string(), hash);
			}
		}
	};

	utils::ArrayAdder<RawFileWorker, LinkerWorker> impl{ GetWorkers() };
}