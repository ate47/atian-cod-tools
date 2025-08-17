#include <includes.hpp>
#include <tools/ff/linkers/linker_bo4.hpp>

namespace fastfile::linker::bo4 {
	class RawFileWorker : public LinkerWorker {
	public:
		RawFileWorker() : LinkerWorker("RawFile") {}

		void Compute(BO4LinkContext& ctx) override {
			for (fastfile::zone::AssetData& assval : ctx.linkCtx.zone.assets["rawfile"]) {
				std::filesystem::path rfpath{ assval.value };
				assval.handled = true;
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
					const char* buffer;
				}; static_assert(sizeof(RawFile) == 0x20);

				ctx.mainFF.data.AddAsset(games::bo4::pool::ASSET_TYPE_RAWFILE, fastfile::linker::data::POINTER_NEXT);

				ctx.mainFF.data.PushStream(XFILE_BLOCK_TEMP);
				RawFile rf{};

				rf.name.name = ctx.HashPathName(rfpath);
				rf.buffer = (const char*)fastfile::linker::data::POINTER_NEXT;
				rf.len = (uint32_t)buffer.size();
				ctx.mainFF.data.WriteData(rf);

				ctx.mainFF.data.PushStream(XFILE_BLOCK_VIRTUAL);
				ctx.mainFF.data.Align(0x10);
				ctx.mainFF.data.WriteData(buffer.data(), buffer.length() + 1); // add \0
				ctx.mainFF.data.PopStream();

				ctx.mainFF.data.PopStream();

				LOG_INFO("Added asset rawfile {} (hash_{:x})", rfpath.string(), rf.name.name);
			}
		}
	};

	utils::ArrayAdder<RawFileWorker, LinkerWorker> impl{ GetWorkers() };
}