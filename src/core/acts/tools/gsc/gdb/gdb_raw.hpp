#pragma once
#include <core/actsinfo.hpp>
#include <core/async.hpp>
#include <core/updater.hpp>
#include <tools/gsc/gsc_vm.hpp>
#include <tools/gsc/gsc_opcodes.hpp>
#include <gsc/gsc_acts_addons.hpp>

namespace tool::gsc::vm {
    using namespace tool::gsc;
    using namespace tool::gsc::opcode;
    using namespace tool::gsc::gdb;

    class GscGdbRaw : public GscGdb {
      public:
        GscGdbRaw() : GscGdb(hash::Hash64("raw"), "raw") {}
        bool DbgSave(GscDecompilerGDBData* gdb, std::string& buffer) override {
            std::stringstream ss{};

            auto WriteFmtString = [&ss](const char* str) -> std::stringstream& {
                ss << "\"";
                if (str) {
                    ss << utils::FormattedString{ str };
                }
                ss << "\"";
                return ss;
                };

            ss << "NAME " << hashutils::ExtractTmpScript(gdb->filename) << "\n"
               << "VERSION 0\n"
               << "CHECKSUM 0x" << std::hex << gdb->checksum << "\n";

            if (!gdb->devStringsLocations.empty()) {
                ss << "# DEV STRINGS\n";

                for (const auto& [floc, str] : gdb->devStringsLocations) {
                    ss << "# dev string:x" << std::hex << floc << "\n";
                    ss << "STRING ";
                    WriteFmtString(str);
                    ss << " 0x" << floc;
                }
                ss << "\n";
            }

            if (!gdb->devBlocksLocation.empty()) {
                ss << "# DEV STRINGS\n";

                for (const uint32_t floc : gdb->devBlocksLocation) {
                    ss << "\n" << "DEVBLOCK 0x" << floc;
                }
                ss << "\n";
            }
            

            if (!gdb->lazyLinks.empty()) {
                ss << "# LAZY LINKS\n";

                for (const NameLocated& nl: gdb->lazyLinks) {
                    ss << "\n" << "LAZYLINK ";

                    WriteFmtString(hashutils::ExtractTmp("script", nl.script)) << " ";
                    WriteFmtString(hashutils::ExtractTmp("namespace", nl.name_space)) << " ";
                    WriteFmtString(hashutils::ExtractTmp("function", nl.name));
                }
                ss << "\n";
            }

            if (!gdb->detours.empty()) {
                ss << "# DETOURS\n";

                for (const GscDecompilerGDBDataDetour& det: gdb->detours) {
                    ss << "\n" << "DETOUR ";

                    WriteFmtString(hashutils::ExtractTmp("script", det.script)) << " ";
                    WriteFmtString(hashutils::ExtractTmp("namespace", det.name_space)) << " ";
                    WriteFmtString(hashutils::ExtractTmp("function", det.name)) << " ";
                    ss << "0x" << std::hex << det.location << " ";
                    ss << "0x" << std::hex << det.size;
                }
                ss << "\n";
            }

            if (!gdb->lineInfos.empty()) {
                ss << "# LINE INFO\n";
                for (const auto& [line, floc] : gdb->lineInfos) {
                    ss << "\n" << "LINEINFO " << line << " 0x" << floc;
                }
                ss << "\n";
            }

            buffer = ss.str();
            return true;
        }
    };

} // namespace tool::gsc::vm