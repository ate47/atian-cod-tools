#include <includes.hpp>
#include <tools/compatibility/acti_archive_checksums.hpp>
#include <core/memory_allocator.hpp>

namespace compatibility::acti::archive_checksums {
    namespace {
        core::memory_allocator::MemoryAllocator alloc{};
    }

    std::vector<ArchiveChecksums*>& GetChecksums() {
        static std::vector<ArchiveChecksums*> archives{};
        return archives;
    }

    void LoadArchiveChecksums(const std::filesystem::path& path) {
        utils::InFileCE is{ path };
        if (!is) {
            return; // no file
        }

        std::string line;
        size_t idx{};
        size_t added{};
        while (*is && std::getline(*is, line)) {
            idx++;
            if (line.empty() || line[0] == '#') {
                continue;
            }

            size_t cut1{ line.find(',') };
            size_t cut2{ cut1 != std::string::npos ? line.find(',', cut1 + 1) : cut1 };
            size_t cut3{ cut2 != std::string::npos ? line.find(',', cut2 + 1) : cut2 };
            size_t cut4{ cut3 != std::string::npos ? line.find(',', cut3 + 1) : cut3 };

            if (cut4 == std::string::npos) {
                LOG_WARNING(
                    "invalid line {}::{}: a line should contain 5 parts (NAME,VAL1,VAL2,VAL3,VAL4)",
                    path.string(),
                    idx
                );
                continue;
            }

            char* file{ line.data() };
            file[cut1] = 0;
            file[cut2] = 0;
            file[cut3] = 0;
            file[cut4] = 0;

            uint32_t checksums[4];
            checksums[0] = (uint32_t)utils::ParseFormatInt(&file[cut1 + 1]);
            checksums[1] = (uint32_t)utils::ParseFormatInt(&file[cut2 + 1]);
            checksums[2] = (uint32_t)utils::ParseFormatInt(&file[cut3 + 1]);
            checksums[3] = (uint32_t)utils::ParseFormatInt(&file[cut4 + 1]);

            ArchiveChecksums* check{ alloc.Alloc<ArchiveChecksums>() };
            check->id = alloc.CloneStr(file);
            std::memcpy(check->checksums, checksums, sizeof(checksums));
            GetChecksums().push_back(check);
            added++;
        }

        if (added) {
            LOG_TRACE("loaded {} archive checksums", added);
        }
    }

    const ArchiveChecksums* GetChecksumsByName(const char* name) {
        for (const ArchiveChecksums* chk : GetChecksums()) {
            if (!_strcmpi(chk->id, name)) {
                return chk;
            }
        }
        return nullptr;
    }
} // namespace compatibility::acti::archive_checksums