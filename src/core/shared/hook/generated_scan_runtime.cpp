#include <includes_shared.hpp>
#include <hook/generated_scan_runtime.hpp>
#include <hook/scan_loader.hpp>
#include <hook/scan_container.hpp>

namespace hook::generated_scan_runtime {
    struct CompiledScan {
        ScanInformation* scan{};
        std::vector<uint8_t> mask{};
        std::vector<uint8_t> searched{};
        size_t delta{};
        hook::scan_container::ResultValue* res{};
        CompiledScan* next{};
    };

    static void LoadScanOffset(ScanInformation& info, hook::scan_container::ScanContainer& scan) {
        if (info.type == ST_OFFSET) {
            void* ref;
            if (info.scan[0] == '0' && info.scan[1] == 'x') {
                ref = scan.GetLibrary()[std::strtoull(&info.scan[2], nullptr, 16)];
            } else {
                ref = scan.GetLibrary()[std::strtoull(info.scan, nullptr, 10)];
            }
            if (info.multiple) {
                ((std::vector<void*>*)info.ref)->push_back(ref);
            } else {
                *info.ref = ref;
            }
        } else {
            throw std::runtime_error("Invalid LoadScanOffset type");
        }
    }

    static void*
    GetScanResult(ScanInformation& info, hook::library::ScanResult& res, hook::scan_container::ScanContainer& scan) {
        switch (info.type) {
        case ST_RELATIVE:
            return res.GetRelative<int32_t, byte*>(info.offset, info.postOffset);
        case ST_ABSOLUTE:
            return res.GetPtr<byte*>(info.offset) + info.postOffset;
        case ST_GETOFFSET32:
            return scan.GetLibrary()[res.Get<uint32_t>(info.offset) + info.postOffset];
        default:
            throw std::runtime_error("Invalid LoadScanResult type");
        }
    }

    static void LoadScanResultAll(
        std::vector<void*>& out, ScanInformation& info, std::vector<hook::library::ScanResult>& ress,
        hook::scan_container::ScanContainer& scan
    ) {
        for (hook::library::ScanResult& res : ress) {
            out.push_back(GetScanResult(info, res, scan));
        }
    }

    static void LoadScansMulti(hook::scan_container::ScanContainer& scan, ScanInformation* list, size_t count) {
        std::unique_ptr<CompiledScan[]> cscans{ std::make_unique<CompiledScan[]>(count) };
        CompiledScan* scindex[0x100]{};
        CompiledScan* maskedSc{};
        size_t numCScans{};
        size_t maxSize{};
        bool anyMissing{};

        bool oldIgnoreMissing{ scan.ignoreMissing };
        scan.ignoreMissing = true;
        // compile scans
        for (size_t i = 0; i < count; i++) {
            ScanInformation& si{ list[i] };

            if (si.type == ST_OFFSET) {
                LoadScanOffset(si, scan);
                continue;
            }

            hook::scan_container::ResultValue& cached{ scan.GetCached(si.scan) };

            if (cached.loaded) {
                // load existing scan
                std::vector<hook::library::ScanResult> res{ scan.ScanCached(cached, si.scan, si.id) };

                if (si.multiple) {
                    LoadScanResultAll(*(std::vector<void*>*)si.ref, si, res, scan);
                } else if (res.empty()) {
                    // no result
                    anyMissing = true;
                    LOG_ERROR("Can't find {}", si.id);
                } else if (si.single && res.size() != 1) {
                    // too many results
                    anyMissing = true;
                    LOG_ERROR("Too many finds for {}", si.id);
                } else {
                    *si.ref = GetScanResult(si, res[0], scan);
                }
                continue;
            }

            CompiledScan& cs{ cscans[numCScans++] };
            cs.scan = &si;
            cs.res = &cached;
            hook::scan_loader::PackScan(si.scan, cs.mask, cs.searched, cs.delta, si.id);

            if (cs.mask[0] != 0xFF) {
                // the mask isn't full for this scan, so we need to search for all
                cs.next = maskedSc;
                maskedSc = &cs;
            } else {
                cs.next = scindex[cs.searched[0]];
                scindex[cs.searched[0]] = &cs;
            }

            if (cs.searched.size() > maxSize) {
                maxSize = cs.searched.size();
            }
        }

        scan.foundMissing = anyMissing;
        if (!numCScans) {
            if (anyMissing && !oldIgnoreMissing) {
                throw std::runtime_error("Missing scans found");
            }
            scan.ignoreMissing = oldIgnoreMissing;
            return; // already loaded
        }

        for (platform::MemoryRegion& region :
             platform::EnumerateMemoryRegions(const_cast<void*>(scan.GetLibrary().Get<void>(0)))) {
            if (!region.readable) {
                continue; // can't read
            }

            uint8_t* current = region.base;
            size_t regionSize = region.size;
            for (size_t off = 0; off + maxSize <= regionSize; off++) {
                for (CompiledScan* msc = maskedSc; msc; msc = msc->next) {
                    bool match = true;
                    for (size_t i = 0; i < msc->searched.size(); i++) {
                        if ((current[off + i] & msc->mask[i]) != msc->searched[i]) {
                            match = false;
                            break;
                        }
                    }
                    if (match) {
                        uint32_t rva{ (uint32_t)((current + off - msc->delta) - scan.GetLibrary().Get<uint8_t>(0)) };
                        if (std::find(msc->res->res.begin(), msc->res->res.end(), rva) == msc->res->res.end()) {
                            msc->res->res.emplace_back(rva);
                        }
                    }
                }

                for (CompiledScan* msc = scindex[current[off]]; msc; msc = msc->next) {
                    bool match = true;
                    for (size_t i = 0; i < msc->searched.size(); i++) {
                        if ((current[off + i] & msc->mask[i]) != msc->searched[i]) {
                            match = false;
                            break;
                        }
                    }
                    if (match) {
                        uint32_t rva{ (uint32_t)((current + off - msc->delta) - scan.GetLibrary().Get<uint8_t>(0)) };
                        if (std::find(msc->res->res.begin(), msc->res->res.end(), rva) == msc->res->res.end()) {
                            msc->res->res.emplace_back(rva);
                        }
                    }
                };
            }
        }

        for (size_t i = 0; i < numCScans; i++) {
            cscans[i].res->loaded = true;

            // now that the result is loaded, we can use it

            ScanInformation& info{ *cscans[i].scan };

            if (info.multiple) {
                std::vector<hook::library::ScanResult> ress{ scan.Scan(info.scan, info.id) };
                LoadScanResultAll(*(std::vector<void*>*)info.ref, info, ress, scan);
            } else {
                hook::library::ScanResult res;
                if (info.single) {
                    res = scan.ScanSingle(info.scan, info.id);
                } else {
                    res = scan.ScanAny(info.scan, info.id);
                }
                *info.ref = GetScanResult(info, res, scan);
            }
        }

        scan.ignoreMissing = oldIgnoreMissing;

        if (scan.foundMissing && !oldIgnoreMissing) {
            throw std::runtime_error("Missing scans found");
        }
        LOG_TRACE("Loaded {} scans", count);
    }

    void LoadScans(hook::scan_container::ScanContainer& scan, ScanInformation* list, size_t count) {
        LoadScansMulti(scan, list, count);
    }
} // namespace hook::generated_scan_runtime