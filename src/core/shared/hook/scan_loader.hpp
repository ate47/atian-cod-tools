#pragma once

namespace hook::generated_scan_runtime {
    struct ScanInformation;
}
namespace hook::scan_loader {
    void PackScan(
        const char* pattern, std::vector<uint8_t>& mask, std::vector<uint8_t>& searched, size_t& delta, const char* name
    );
}