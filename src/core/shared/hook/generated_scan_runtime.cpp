#include <includes_shared.hpp>
#include <hook/generated_scan_runtime.hpp>

namespace hook::generated_scan_runtime {

    void LoadScans(hook::scan_container::ScanContainer& scan, ScanInformation* list, size_t count) {
        bool oldIgnoreMissing{ scan.ignoreMissing };
        scan.ignoreMissing = true;
        for (size_t i = 0; i < count; i++) {
            ScanInformation& info{ list[i] };
            switch (info.type) {
            case ST_RELATIVE:
                if (info.single) {
                    *info.ref =
                        scan.ScanSingle(info.scan, info.id).GetRelative<int32_t, byte*>(info.offset, info.postOffset);
                } else {
                    *info.ref =
                        scan.ScanAny(info.scan, info.id).GetRelative<int32_t, byte*>(info.offset, info.postOffset);
                }
                break;
            case ST_ABSOLUTE:
                if (info.single) {
                    *info.ref = scan.ScanSingle(info.scan, info.id).GetPtr<byte*>(info.offset) + info.postOffset;
                } else {
                    *info.ref = scan.ScanAny(info.scan, info.id).GetPtr<byte*>(info.offset) + info.postOffset;
                }
                break;
            case ST_OFFSET:
                if (info.scan[0] == '0' && info.scan[1] == 'x') {
                    *info.ref = hook::process::Relativise(std::strtoull(&info.scan[2], nullptr, 16));
                } else {
                    *info.ref = hook::process::Relativise(std::strtoull(info.scan, nullptr, 10));
                }
                break;
            case ST_GETOFFSET32:
                if (info.single) {
                    *info.ref = scan.GetLibrary()
                                    [scan.ScanSingle(info.scan, info.id).Get<uint32_t>(info.offset) + info.postOffset];
                } else {
                    *info.ref = scan.GetLibrary()
                                    [scan.ScanAny(info.scan, info.id).Get<uint32_t>(info.offset) + info.postOffset];
                }
                break;
            }
        }
        scan.ignoreMissing = oldIgnoreMissing;

        if (scan.foundMissing && !oldIgnoreMissing) {
            throw std::runtime_error("Missing scans found");
        }
        LOG_TRACE("Loaded {} scans", count);
    }
} // namespace hook::generated_scan_runtime