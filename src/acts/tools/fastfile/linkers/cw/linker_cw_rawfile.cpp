#include <includes.hpp>
#include <tools/fastfile/linkers/linker_cw.hpp>

namespace fastfile::linker::cw {
	class RawFileWorker : public LinkerWorker {
	public:
		RawFileWorker() : LinkerWorker("RawFile") {}

		void Compute(BOCWLinkContext& ctx) override {
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
					uint64_t name;
					int32_t len;
					const char* buffer;
				}; static_assert(sizeof(RawFile) == 0x18);

				ctx.data.AddAsset(ASSET_TYPE_RAWFILE, fastfile::linker::data::POINTER_NEXT);

				ctx.data.PushStream(XFILE_BLOCK_TEMP);
				RawFile rf{};

				rf.name = ctx.HashPathName(rfpath);
				rf.buffer = (const char*)fastfile::linker::data::POINTER_NEXT;
				rf.len = (uint32_t)buffer.size();
				ctx.data.WriteData(rf);

				ctx.data.PushStream(XFILE_BLOCK_VIRTUAL);
				ctx.data.Align(0x10);
				ctx.data.WriteData(buffer.data(), buffer.length() + 1); // add \0
				ctx.data.PopStream();

				ctx.data.PopStream();

				LOG_INFO("Added asset rawfile {} (hash_{:x})", rfpath.string(), rf.name);
			}
		}
	};

	utils::ArrayAdder<RawFileWorker, LinkerWorker> impl{ GetWorkers() };
}