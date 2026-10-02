#include <includes.hpp>
#include <tools/tools.hpp>
#include <utils/utils.hpp>
#include <zlib.h>

// PS4 "Black Ops" (2010 T5 zones) remaster (~2025). Scripts are stored uncompiled, each as a
// self-contained entry in the decompressed zone: [name\0][u32 uncompressedSize][u32 compressedSize]
// [zlib data]. See decompressor_iw.cpp (IWFV_BO1_REMASTER) for the outer fastfile container format.
namespace {
    constexpr uint64_t IW_FF_MAGIC_MASK = 0xFFFFFF00FFFFFFFF;
    constexpr uint64_t IW_FF_MAGIC = 0x3030310066665749;
    constexpr uint32_t BO1_REMASTER_HEADER_VERSION = 0x1D9;
    constexpr size_t BO1_REMASTER_CHUNK_MAX = 0xC000;

    bool IsNameChar(byte b) {
        return (b >= '0' && b <= '9') || (b >= 'a' && b <= 'z') || (b >= 'A' && b <= 'Z') || b == '/' || b == '_' ||
               b == '.' || b == '-';
    }

    bool DecompressBO1RemasterFF(const std::vector<byte>& raw, std::vector<byte>& ffdata) {
        if (raw.size() < 16 || (*(uint64_t*)raw.data() & IW_FF_MAGIC_MASK) != IW_FF_MAGIC) {
            return false;
        }
        uint32_t headerVersion{ *(uint32_t*)&raw[8] };
        if (headerVersion != BO1_REMASTER_HEADER_VERSION) {
            return false;
        }

        auto chunkBuff{ std::make_unique<byte[]>(BO1_REMASTER_CHUNK_MAX) };

        size_t off{ 16 };
        while (off + sizeof(uint32_t) <= raw.size()) {
            uint32_t compressedSize{ *(uint32_t*)&raw[off] };
            off += sizeof(uint32_t);
            if (!compressedSize) {
                break; // terminator
            }
            if (off + compressedSize > raw.size()) {
                LOG_ERROR("Truncated fastfile chunk at 0x{:x}", off);
                return false;
            }

            z_stream strm{};
            if (inflateInit2(&strm, -15) != Z_OK) {
                return false;
            }
            strm.next_in = (Bytef*)&raw[off];
            strm.avail_in = (uInt)compressedSize;
            strm.next_out = (Bytef*)chunkBuff.get();
            strm.avail_out = (uInt)BO1_REMASTER_CHUNK_MAX;
            int ret{ inflate(&strm, Z_FINISH) };
            size_t written{ BO1_REMASTER_CHUNK_MAX - strm.avail_out };
            inflateEnd(&strm);

            if (ret != Z_STREAM_END) {
                LOG_ERROR("Can't inflate chunk at 0x{:x}", off);
                return false;
            }

            ffdata.insert(ffdata.end(), chunkBuff.get(), chunkBuff.get() + written);
            off += compressedSize;
        }
        return true;
    }

    size_t ExtractScripts(const std::vector<byte>& data, const std::filesystem::path& outDir) {
        static const char* exts[]{ ".gsc", ".csc" };
        size_t count{};

        for (size_t i = 0; i + 5 <= data.size(); i++) {
            for (const char* ext : exts) {
                size_t elen{ std::strlen(ext) };
                if (i + elen + 1 > data.size() || std::memcmp(&data[i], ext, elen) || data[i + elen] != 0) {
                    continue;
                }

                size_t start{ i };
                while (start > 0 && IsNameChar(data[start - 1])) {
                    start--;
                }
                size_t nameLen{ i + elen - start };
                if (nameLen < 4 || nameLen > 260) {
                    continue;
                }

                size_t off{ i + elen + 1 };
                if (off + 8 > data.size()) {
                    continue;
                }
                uint32_t uncompressedSize, compressedSize;
                std::memcpy(&uncompressedSize, &data[off], 4);
                std::memcpy(&compressedSize, &data[off + 4], 4);
                off += 8;
                if (!compressedSize || !uncompressedSize || off + compressedSize > data.size() ||
                    uncompressedSize > 0x4000000) {
                    continue;
                }

                std::vector<byte> dec(uncompressedSize);
                uLongf destLen{ (uLongf)uncompressedSize };
                int r{ uncompress((Bytef*)dec.data(), &destLen, &data[off], (uLongf)compressedSize) };
                if (r != Z_OK) {
                    continue;
                }
                dec.resize(destLen);

                std::string name{ (const char*)&data[start], nameLen };
                std::filesystem::path outPath{ outDir / name };
                std::filesystem::create_directories(outPath.parent_path());
                if (utils::WriteFile(outPath, dec)) {
                    count++;
                } else {
                    LOG_ERROR("Can't write {}", outPath.string());
                }

                i = off + compressedSize - 1; // skip past this entry, -1 for the loop's ++i
                break;
            }
        }

        return count;
    }

    // Extracts scripts from a single .ff/.ff.dec file into outDir, returns the number of scripts
    // extracted or -1 on error.
    long long ExtractOne(const std::filesystem::path& inPath, const std::filesystem::path& outDir) {
        std::vector<byte> raw;
        if (!utils::ReadFile(inPath, raw)) {
            LOG_ERROR("Can't read {}", inPath.string());
            return -1;
        }

        std::vector<byte> data;
        if (raw.size() >= 8 && (*(uint64_t*)raw.data() & IW_FF_MAGIC_MASK) == IW_FF_MAGIC) {
            if (!DecompressBO1RemasterFF(raw, data)) {
                LOG_ERROR("Can't decompress {}: not a BO1 remaster fastfile", inPath.string());
                return -1;
            }
        } else {
            data = std::move(raw); // already-decompressed .ff.dec blob
        }

        return (long long)ExtractScripts(data, outDir);
    }

    int bo1remasterscripts(int argc, const char* argv[]) {
        if (tool::NotEnoughParam(argc, 1)) {
            return tool::BAD_USAGE;
        }

        std::filesystem::path inPath{ argv[2] };
        std::filesystem::path outDir{ argc > 3 ? argv[3] : "bo1_scripts" };

        if (!std::filesystem::is_directory(inPath)) {
            long long r{ ExtractOne(inPath, outDir) };
            if (r < 0) {
                return tool::BASIC_ERROR;
            }
            LOG_INFO("Extracted {} script(s) into {}", r, outDir.string());
            return tool::OK;
        }

        // Bulk mode: every .ff/.ff.dec under inPath gets its own sub-folder under outDir, mirroring
        // the input's relative directory structure.
        size_t fileCount{};
        size_t totalScripts{};
        size_t errCount{};

        for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator{ inPath }) {
            if (!entry.is_regular_file()) {
                continue;
            }
            std::filesystem::path ext{ entry.path().extension() };
            if (ext != ".ff" && ext != ".dec") {
                continue;
            }

            std::filesystem::path rel{ std::filesystem::relative(entry.path(), inPath) };
            std::filesystem::path stem{ rel };
            while (stem.has_extension()) {
                stem.replace_extension();
            }
            std::filesystem::path subOut{ outDir / stem };

            long long r{ ExtractOne(entry.path(), subOut) };
            fileCount++;
            if (r < 0) {
                errCount++;
                continue;
            }
            totalScripts += (size_t)r;
            LOG_INFO("[{}] {} script(s) -> {}", rel.string(), r, subOut.string());
        }

        LOG_INFO("Done: {} file(s) processed, {} script(s) extracted, {} error(s)", fileCount, totalScripts, errCount);
        return errCount ? tool::BASIC_ERROR : tool::OK;
    }

    ADD_TOOL(
        bo1remasterscripts, "fastfile", " [ff/dec/dir] [output=bo1_scripts]",
        "extract .gsc/.csc scripts from a BO1 PS4 remaster zone, pass a directory to bulk-extract", bo1remasterscripts
    );
} // namespace
