#include <includes.hpp>
#include <unit_test.hpp>
#include <utils/compress_utils.hpp>

namespace {

    void name_compress_utils_test() {
        // zdq
        ASSERT_EQ("name", utils::compress::COMP_ZLIB, utils::compress::GetConfigName("zlib"));
        ASSERT_EQ(
            "name hc",
            utils::compress::COMP_ZLIB | utils::compress::COMP_HIGH_COMPRESSION,
            utils::compress::GetConfigName("zlib_hc")
        );
        ASSERT_EQ(
            "name stored",
            utils::compress::COMP_ZLIB | utils::compress::COMP_STORED,
            utils::compress::GetConfigName("zlib_stored")
        );
        ASSERT_EQ("deflate", utils::compress::COMP_ZLIB_DEFLATE, utils::compress::GetConfigName("deflate"));

        ASSERT_FALSE("invalid no name", utils::compress::GetCompressionName({}, nullptr));

        for (size_t i = 1; i < utils::compress::COMP_COUNT; i++) {
            utils::compress::CompressionAlgorithm alg{ (utils::compress::CompressionAlgorithm)i };
            ASSERT_VAL(std::format("all algorithm has name #{}", i), utils::compress::GetCompressionName(alg, nullptr));
        }

        static struct TestCompressName {
            const char* name;
            utils::compress::CompressionAlgorithm val;
        } compressNames[]{
            { "zlib", utils::compress::COMP_ZLIB },
            { "zlib_hc", utils::compress::COMP_ZLIB | utils::compress::COMP_HIGH_COMPRESSION },
            { "lz4_stored", utils::compress::COMP_LZ4 | utils::compress::COMP_STORED },
            { "oodle_leviathan_hc",
              utils::compress::COMP_OODLE | utils::compress::COMP_OODLE_TYPE_LEVIATHAN |
                  utils::compress::COMP_HIGH_COMPRESSION },
            { "none", utils::compress::COMP_NONE },
        };

        for (const TestCompressName& test : compressNames) {
            ASSERT_EQ(
                std::format("GetConfigName {}", test.name ? test.name : "<nullptr>"),
                test.val,
                utils::compress::GetConfigName(test.name)
            );
            ASSERT_EQ_STR(
                std::format("GetCompressionName {}", test.name ? test.name : "<nullptr>"),
                test.name,
                utils::compress::GetCompressionName(test.val)
            );
        }
    }
    void zlib_compress_utils_test() {
        // base windows
        ASSERT_EQ("zlib window", 15, utils::compress::GetZlibWindow(utils::compress::COMP_ZLIB));
        ASSERT_EQ("deflate window", -15, utils::compress::GetZlibWindow(utils::compress::COMP_ZLIB_DEFLATE));
        ASSERT_EQ("gzip window", 31, utils::compress::GetZlibWindow(utils::compress::COMP_ZLIB_GZIP));

        // special windows
        ASSERT_EQ("zlib comp", utils::compress::COMP_ZLIB, utils::compress::GetZlibWindowedCompression(15));
        ASSERT_EQ("deflate comp", utils::compress::COMP_ZLIB_DEFLATE, utils::compress::GetZlibWindowedCompression(-15));
        ASSERT_EQ("gzip comp", utils::compress::COMP_ZLIB_GZIP, utils::compress::GetZlibWindowedCompression(31));

        // custom window bits
        for (int i = -31; i < 32; i++) {
            utils::compress::CompressionAlgorithm alg{ utils::compress::GetZlibWindowedCompression(i) };
            int window{ utils::compress::GetZlibWindow(alg) };

            if (i >= 8 && i <= 15) {
                ASSERT_EQ(
                    std::format("custom zlib window {}", i),
                    utils::compress::COMP_ZLIB,
                    utils::compress::GetCompressionType(alg)
                );
            } else if (i >= -15 && i <= -8) {
                ASSERT_EQ(
                    std::format("custom deflate window {}", i),
                    utils::compress::COMP_ZLIB_DEFLATE,
                    utils::compress::GetCompressionType(alg)
                );
            } else if (i >= 24 && i <= 31) {
                ASSERT_EQ(
                    std::format("custom gzip window {}", i),
                    utils::compress::COMP_ZLIB_GZIP,
                    utils::compress::GetCompressionType(alg)
                );
            } else {
                ASSERT_EQ("invalid window", 0, window);
                continue;
            }

            ASSERT_EQ(std::format("custom window {} {}", i, window), window, i);
        }
    }
    ADD_TEST(name_compress_utils_test, name_compress_utils_test);
    ADD_TEST(zlib_compress_utils_test, zlib_compress_utils_test);
} // namespace