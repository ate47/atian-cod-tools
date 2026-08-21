#include <includes.hpp>
#include <core/bytebuffer.hpp>

namespace {
    constexpr const char MAGIC[] = "CODIDS";
    int idsr(int argc, const char* argv[]) {
        if (tool::NotEnoughParam(argc, 1)) {
            return tool::BAD_USAGE;
        }
        std::vector<std::filesystem::path> files{};

        utils::GetFileRecurseExt(argv[2], files, ".ids\0");

        for (const std::filesystem::path& path : files) {
            std::filesystem::path out{ path };
            out.replace_extension(".ids.csv");

            std::vector<byte> buff{ utils::ReadFile<std::vector<byte>>(path) };

            core::bytebuffer::ByteBuffer reader{ buff };
            reader.SetEndian(std::endian::little);

            char magic[sizeof(MAGIC)]{};

            reader.Read(magic, sizeof(magic) - 1);

            if (std::memcmp(MAGIC, magic, sizeof(magic) - 1)) {
                LOG_ERROR("Invalid file {}: bad magic {}", path.string(), magic);
                continue;
            }
            uint16_t version{ reader.Read<uint16_t>() };

            if (version != 1) {
                LOG_ERROR("Invalid file {}: bad version {}", path.string(), version);
                continue;
            }

            std::string game{ reader.ReadSizedString<uint16_t>() };
            uint64_t records{ reader.Read<uint64_t>() };
            std::map<uint16_t, std::set<uint64_t>> ds{};
            for (size_t i = 0; i < records; i++) {
                uint64_t h{ reader.Read<uint64_t>() };
                uint16_t t{ reader.Read<uint16_t>() };

                ds[t].insert(h);
            }

            LOG_INFO("read {} {} records {} -> {}", records, game, path.string(), out.string());
            utils::OutFileCE os{ out, true };

            os << "type,hash";
            for (auto& [t, dd] : ds) {
                for (uint64_t h : dd) {
                    os << "\n" << std::dec << t << ",#hash_" << std::hex << h;
                }
            }
        }
        LOG_INFO("done.");
        return tool::OK;
    }
    ADD_TOOL(idsr, "hash", " [files]", "unpack .ids files", idsr);
} // namespace