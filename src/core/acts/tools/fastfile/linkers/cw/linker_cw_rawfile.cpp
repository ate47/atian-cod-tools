#include <includes.hpp>
#include <tools/fastfile/linkers/linker_cw.hpp>

namespace {
    using namespace fastfile::linker::cw;
    class XAssetLinkerImpl : public XAssetLinker {
      public:
        using XAssetLinker::XAssetLinker;

        void Compute(BOCWLinkContext& ctx, const char* id, BOCWFFContext& ff) override {
            std::filesystem::path rfpath{ id };
            std::filesystem::path path{ ctx.linkCtx.input / rfpath };
            std::string buffer{};
            if (!utils::ReadFile(path, buffer)) {
                LOG_ERROR("Can't read rawfile {}", path.string());
                ctx.error = true;
                return;
            }

            struct RawFile {
                CWXHash name;
                int32_t len;
                const char* buffer;
            };
            static_assert(sizeof(RawFile) == 0x18);

            ff.data.PushStream(XFILE_BLOCK_TEMP);
            RawFile& rf{ ff.data.AllocStreamRef<RawFile>() };

            rf.name = ctx.HashPathName(rfpath);
            rf.buffer = (const char*)fastfile::linker::memory::POINTER_NEXT;
            rf.len = (uint32_t)buffer.size();

            ff.data.PushStream(XFILE_BLOCK_VIRTUAL);
            ff.data.Align(0x10);
            ff.data.WriteStream(buffer.data(), buffer.length() + 1); // add \0
            ff.data.PopStream();

            ff.data.PopStream();

            LOG_INFO("Added asset rawfile {} (hash_{:x})", rfpath.string(), rf.name);
        }
    };

    utils::MapAdder<XAssetLinkerImpl, XAssetType, XAssetLinker> impl{ GetWorkers(), XAssetType::ASSET_TYPE_RAWFILE };
} // namespace