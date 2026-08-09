#include <includes.hpp>
#include <tools/fastfile/compressors/compressor_t789_abstract.hpp>
#include <tools/fastfile/fastfile_flexible.hpp>
#include <tools/fastfile/linkers/linker_cw.hpp>

namespace {
    using namespace fastfile;
    using namespace fastfile::compressor::t789;

    class T9Header {
      public:
        uint32_t version{};
        fastfile::flexible::PFFBuildData build{};
        fastfile::flexible::PFFFastFileInfo ff{};
        fastfile::flexible::PFFSizeData size{};
        fastfile::flexible::PFPlatformData platform{};
        uint64_t blockSizes[fastfile::linker::cw::XFILE_BLOCK_COUNT]{};
        fastfile::flexible::FlexibleFastFileWriter hwriter;
        T9Header(std::vector<byte>& out) : hwriter(out, fastfile::flexible::MAGIC) {
            hwriter.AddBlock(fastfile::flexible::ST_VERSION, &version, sizeof(version));
            hwriter.AddBlock(fastfile::flexible::ST_PLATFORM_DATA, &platform, sizeof(platform));
            hwriter.AddBlock(fastfile::flexible::ST_BUILD_DATA, &build, sizeof(build));
            hwriter.AddBlock(fastfile::flexible::ST_BLOCK_SIZES, &blockSizes, sizeof(blockSizes));
            hwriter.AddBlock(fastfile::flexible::ST_SIZE_DATA, &size, sizeof(size));
            hwriter.AddBlock(fastfile::flexible::ST_FASTFILE_INFO, &ff, sizeof(ff));
        }
    };

    class FFCompressorBOCW
        : public AbstractCompressorT789<
              T789Version::T789V_CW, XFILE_COMPRESSION_COUNT, KeyVersion::VER_BO4, std::unique_ptr<T9Header>> {
      public:
        FFCompressorBOCW() : AbstractCompressorT789("CW", "Black ops Cold War fast file compressor") {}

        void AllocateHeader(
            FastFileLinkerContext& ctx, std::vector<byte>& output, std::unique_ptr<T9Header>& header
        ) override {
            header = std::make_unique<T9Header>(output);
        }
        void WriteHeaderData(
            FastFileLinkerContext& ctx, std::vector<byte>& output, fastfile::FastFile& ff,
            FastFileCompression compression, FFCompressorSecureInfo& secure, std::unique_ptr<T9Header>& header
        ) override {
            // write header data
            header->version = 0x64;
            header->platform.platform = ctx.opt.platform;
            header->platform.server = ctx.opt.server;
            header->build.timestamp = utils::GetTimestamp() / 1000;
            header->platform.encrypted = false;
            header->size.size = ff.linkedData.size();
            header->platform.compression = compression;

            LoadArchiveChecksums<T789V_CW>(header->build.archiveChecksum);

            // build data
            platform::GetComputerInfoName(header->build.builderName, sizeof(header->build.builderName));
            snprintf(header->ff.fastfileName, sizeof(header->ff.fastfileName), "%s", ff.ffname);

            // blocks load data
            for (size_t i = 0; i < fastfile::linker::cw::XFILE_BLOCK_COUNT; i++) {
                header->blockSizes[i] = (uint64_t)ff.blockSizes[i];
            }

            // todo: write other data

            header->hwriter.WriteEnd();
        }
    };

    utils::ArrayAdder<FFCompressorBOCW, FFCompressor> impl{ GetCompressors() };
} // namespace