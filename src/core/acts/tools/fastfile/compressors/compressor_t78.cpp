#include <includes.hpp>
#include <tools/fastfile/compressors/compressor_t789_abstract.hpp>
#include <tools/fastfile/fastfile_flexible.hpp>
#include <tools/fastfile/linkers/linker_bo3.hpp>
#include <tools/fastfile/linkers/linker_bo4.hpp>

namespace {
    using namespace fastfile;
    using namespace fastfile::compressor::t789;

    template<typename XFile, T789Version version, size_t compressionMax, KeyVersion keyVersion>
    class FFCompressorT78 : public AbstractCompressorT789<version, compressionMax, keyVersion, void*> {
      public:
        using AbstractCompressorT789<version, compressionMax, keyVersion, void*>::AbstractCompressorT789;

        void AllocateHeader(FastFileLinkerContext& ctx, std::vector<byte>& output, void*& _h) override {
            utils::Allocate<XFile>(output);
        }
        void WriteHeaderData(
            FastFileLinkerContext& ctx, std::vector<byte>& output, fastfile::FastFile& ff,
            FastFileCompression compression, FFCompressorSecureInfo& secure, void*& _h
        ) override {
            // write header data
            XFile& header{ *(XFile*)output.data() };
            *(uint64_t*)&header.magic[0] = 0x3030303066664154;
            header.version = version;
            header.platform = ctx.opt.platform;
            header.server = ctx.opt.server;
            header.timestamp = utils::GetTimestamp() / 1000;
            header.size = ff.linkedData.size();
            header.compression = compression;
            header.encrypted = secure.aesKey != nullptr;
            std::memcpy(header.aesIV, secure.aesIV, sizeof(secure.aesIV));

            LoadArchiveChecksums<version>(header.archiveChecksum);

            platform::GetComputerInfoName(header.builder, sizeof(header.builder));
            snprintf(header.fastfileName, sizeof(header.fastfileName), "%s", ff.ffname);

            // blocks load data
            for (size_t i = 0; i < ACTS_ARRAYSIZE(header.blockSize); i++) {
                header.blockSize[i] = (uint64_t)ff.blockSizes[i];
            }

            // todo: write other data
        }
    };

    utils::ArrayAdder<
        FFCompressorT78<XFileBO3, T789V_BO3, XFILE_COMPRESSION_COUNT_T7, KeyVersion::VER_BO3>, FFCompressor>
        implt7{ GetCompressors(), "BO3", "Black ops 3 fast file compressor" };
    utils::ArrayAdder<
        FFCompressorT78<XFileBO4_0x27F, T789V_BO4, XFILE_COMPRESSION_COUNT, KeyVersion::VER_BO4>, FFCompressor>
        implt8{ GetCompressors(), "BO4", "Black ops 4 fast file compressor" };

} // namespace