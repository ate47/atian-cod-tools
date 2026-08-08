#include <includes.hpp>
#include <unit_test.hpp>
#include <utils/compress_utils.hpp>
#include "data_utils.hpp"

namespace utils::data {
    namespace {

        std::unique_ptr<byte[]> CreateRndBuffer(size_t len) {
            std::unique_ptr<byte[]> buff{ std::make_unique<byte[]>(len) };

            std::mt19937 gen{ 42 };
            std::uniform_int_distribution distrib{ 0, 255 };
            for (size_t i = 0; i < len; i++) {
                buff[i] = (byte)distrib(gen);
            }

            ASSERT_VAL("buffer null", !IsNulled(buff.get(), len));

            return buff;
        }

        constexpr size_t compBufferSize = 0x10000;

        void compress_test() {
            utils::compress::CompressionAlgorithm algs[]{ utils::compress::COMP_LZ4,
                                                          utils::compress::COMP_ZLIB,
                                                          utils::compress::COMP_NONE };

            for (utils::compress::CompressionAlgorithm alg : algs) {
                size_t dlen{ utils::compress::GetCompressSize(alg, compBufferSize) };
                std::unique_ptr<byte[]> buff{ CreateRndBuffer(compBufferSize) };
                std::unique_ptr<byte[]> compBuff{ std::make_unique<byte[]>(dlen) };
                std::unique_ptr<byte[]> decompBuff{ std::make_unique<byte[]>(compBufferSize) };

                bool cret{ utils::compress::Compress(alg, compBuff.get(), &dlen, buff.get(), compBufferSize) };
                ASSERT_VAL(std::format("Can't compress buffer {}", alg), cret);

                int dret{ utils::compress::Decompress2(alg, decompBuff.get(), compBufferSize, compBuff.get(), dlen) };

                ASSERT_VAL(std::format("Can't decompress buffer {}", alg), dret >= 0);

                ASSERT_VAL(
                    std::format("Bad decompress {}", alg),
                    !std::memcmp(buff.get(), decompBuff.get(), compBufferSize)
                );
            }
        }

        void unk_decompress_test() {
            utils::compress::CompressionAlgorithm algs[]{ utils::compress::COMP_NONE,
                                                          utils::compress::COMP_LZ4,
                                                          utils::compress::COMP_ZLIB };

            for (utils::compress::CompressionAlgorithm alg : algs) {
                size_t dlen{ utils::compress::GetCompressSize(alg, compBufferSize) };
                std::unique_ptr<byte[]> buff{ CreateRndBuffer(compBufferSize) };
                std::unique_ptr<byte[]> compBuff{ std::make_unique<byte[]>(dlen) };

                bool cret{ utils::compress::Compress(alg, compBuff.get(), &dlen, buff.get(), compBufferSize) };
                ASSERT_VAL(std::format("Can't compress buffer {}", alg), cret);

                std::vector<byte> decompBuff{};
                int dret{ utils::compress::DecompressAll(alg, decompBuff, compBuff.get(), dlen) };

                ASSERT_VAL(std::format("Can't decompress buffer {}", alg), dret >= 0);

                ASSERT_EQ(std::format("Bad decompress size {}", alg), compBufferSize, decompBuff.size());
                ASSERT_VAL(
                    std::format("Bad decompress {}", alg),
                    !std::memcmp(buff.get(), decompBuff.data(), compBufferSize)
                );
            }
        }

        ADD_TEST(compress_test, compress_test);
        // ADD_TEST(unk_decompress_test, unk_decompress_test);
    } // namespace
} // namespace utils::data
