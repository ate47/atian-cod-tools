#pragma once

namespace compatibility::acti::archive_checksums {
    constexpr const char* DEFAULT_ARCHIVE_CHECKSUMS_FILE = "archive_checksums.csv";

    struct ArchiveChecksums {
        const char* id;
        uint32_t checksums[4];
    };

    std::vector<ArchiveChecksums*>& GetChecksums();
    void LoadArchiveChecksums(const std::filesystem::path& path);
    const ArchiveChecksums* GetChecksumsByName(const char* name);
} // namespace compatibility::acti::archive_checksums