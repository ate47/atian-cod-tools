#include <includes.hpp>
#include <span>
#include <tools/gsc/obfuscator/gsc_obfuscator_handler.hpp>
#include <utils/data_utils.hpp>
#include <core/bytebuffer.hpp>
// crc_cpp stuff
#undef small
#include <crc_cpp.h>

namespace tool::gsc::obfuscator::handler {
    constexpr uint64_t VMI_T7_1C = 0x1C000a0d43534780;
    namespace {
        uint16_t SafeCreateLocalVariables_Opcodes[]{
            0x1d2,  0x299,  0x2f7,  0x336,  0x3fa,  0x45a,  0x49f,  0x5b3,  0x6a7,  0x7b1,  0x80c,  0x83e,
            0x958,  0xa9a,  0xaa1,  0xb17,  0xc67,  0xcd7,  0xcef,  0xe21,  0x1210, 0x1224, 0x1260, 0x12e2,
            0x1386, 0x13c3, 0x13ed, 0x15a1, 0x1654, 0x1897, 0x18a4, 0x195f, 0x1995, 0x1b60, 0x1b6b, 0x1c6d,
            0x1e3d, 0x1f65, 0x1f6a, 0x1fa9, 0x209e, 0x213b, 0x221b, 0x23d6, 0x252d, 0x2634, 0x26eb, 0x272d,
            0x27c6, 0x280c, 0x2836, 0x2898, 0x29db, 0x2a67, 0x2b13, 0x2b42, 0x2c8f, 0x2d02, 0x2d24, 0x2dc0,
            0x2de0, 0x2e88, 0x2ebb, 0x2ecf, 0x3029, 0x3196, 0x3227, 0x3243, 0x327c, 0x3308, 0x339a, 0x33e5,
            0x366f, 0x368c, 0x3742, 0x3886, 0x38ab, 0x399f, 0x39c4, 0x3af6, 0x3bb4, 0x3e7e, 0x3ebc
        };

        uint16_t Jump_Opcodes[]{
            0x22,   0x3c,   0x7d,   0x140,  0x14b,  0x25f,  0x4b9,  0x4fa,  0x541,  0x5ac,  0x652,  0x6c3,  0x6f9,
            0x840,  0x89f,  0x945,  0xa95,  0xb1c,  0xb3f,  0xba0,  0xbc9,  0xc3f,  0xc51,  0xc5f,  0xdbf,  0xde4,
            0xdee,  0xe51,  0xeb9,  0xf26,  0xf82,  0xfd9,  0x10a9, 0x11c8, 0x1222, 0x1259, 0x12a9, 0x1307, 0x131b,
            0x1341, 0x1365, 0x1400, 0x1436, 0x1452, 0x14dc, 0x1735, 0x17a7, 0x184d, 0x18b2, 0x18b3, 0x1b0a, 0x1c22,
            0x1c56, 0x1ca5, 0x1cf3, 0x1dc9, 0x1dd2, 0x1e21, 0x1e99, 0x1ebe, 0x1ebf, 0x1ef8, 0x1f22, 0x1f45, 0x201c,
            0x2127, 0x217b, 0x21bf, 0x21e9, 0x23b9, 0x25a3, 0x25d8, 0x268e, 0x273b, 0x27f0, 0x28ed, 0x299d, 0x2b1b,
            0x2b92, 0x2bb7, 0x2be6, 0x2bed, 0x2ce2, 0x2d32, 0x2e06, 0x2eea, 0x2f4b, 0x2fd8, 0x2fe2, 0x3037, 0x30d3,
            0x3122, 0x315b, 0x3179, 0x31f8, 0x3284, 0x32c1, 0x3304, 0x344f, 0x34e2, 0x353b, 0x3596, 0x36b4, 0x36bd,
            0x37c4, 0x384a, 0x3906, 0x3933, 0x398c, 0x39c1, 0x3a48, 0x3a64, 0x3ae0, 0x3b79, 0x3b85, 0x3c54, 0x3df9,
            0x3e03, 0x3e5e, 0x3ea1, 0x3ee7
        };

        uint16_t GetRandomOpCodes(std::span<uint16_t> opcodes) { return opcodes[utils::data::Rand(opcodes.size())]; }

        uint32_t ComputeCRC32(void* data, size_t len) {
            byte* b{ (byte*)data };

            crc_cpp::crc32 crc{};

            for (size_t i = 0; i < len; i++) {
                crc.update(b[i]);
            }
            return crc.final();
        }

        size_t FindExportSizeByCrc(byte* bytecode, void* end, uint32_t crc) {
            for (size_t len = 1; bytecode + len <= end; len++) {
                if (ComputeCRC32(bytecode, len) == crc) {
                    return len;
                }
            }
            return 0;
        }
    } // namespace

    GscObfuscator::GscObfuscator(tool::gsc::obfuscator::options::GscObfOptions& opt, byte* script, size_t scriptLen)
        : opt(opt), script(script), scriptLen(scriptLen), header(*(T7GSCOBJ*)script), scriptEnd(&script[scriptLen]) {

        if (scriptLen < sizeof(T7GSCOBJ)) {
            throw std::runtime_error("Invalid size");
        }

        uint64_t magic{ *(uint64_t*)script };

        if (magic != VMI_T7_1C) {
            throw std::runtime_error(std::format("Invalid magic 0x{:x}", magic));
        }

        ReadTables();
    }
    void GscObfuscator::ReadTables() {
        if (header.export_count) {
            T7GSCExport* exports{ (T7GSCExport*)&header.magic[header.export_offset] };
            ValidateInScript(&exports[header.export_count], "export table");

            for (size_t i = 0; i < header.export_count; i++) {
                T7GSCExport& exp{ exports[i] };

                byte* bc{ &header.magic[exp.address] };
                // the minimum is createparam + end (4 bytes)
                ValidateInScript(Aligned<uint16_t>(bc) + 4, "invalid exports address");
                size_t len{ FindExportSizeByCrc(bc, scriptEnd, exp.checksum) };

                exportsMap[NameLocated{
                               .name_space = exp.name_space,
                               .name = exp.name,
                           }]
                    .emplace_back(GscExportData{ .ref = &exp, .bc = bc, .len = len });
            }
        }
        if (header.import_count) {
            T7GSCImport* imports{ (T7GSCImport*)&header.magic[header.import_offset] };
            ValidateInScript(imports + 1, "imports table");

            for (size_t i = 0; i < header.import_count; i++) {
                T7GSCImport& imp{ *imports };
                uint32_t* offsets{ (uint32_t*)&imports[1] };
                imports = (T7GSCImport*)&offsets[imports->num_address];
                ValidateInScript(imports, "imports table");

                importsMap[NameLocated{
                               .name_space = imp.import_namespace,
                               .name = imp.name,
                           }]
                    .emplace_back(&imp);
            }
        }
    }

    void GscObfuscator::PrintHeader() {
        LOG_INFO("source_crc 0x{:x}", (uint32_t)header.source_crc);
        LOG_INFO("include_offset 0x{:x}", (uint32_t)header.include_offset);
        LOG_INFO("animtree_offset 0x{:x}", (uint32_t)header.animtree_offset);
        LOG_INFO("cseg_offset 0x{:x}", (uint32_t)header.cseg_offset);
        LOG_INFO("string_offset 0x{:x}", (uint32_t)header.string_offset);
        LOG_INFO("devblock_string_offset 0x{:x}", (uint32_t)header.devblock_string_offset);
        LOG_INFO("export_offset 0x{:x}", (uint32_t)header.export_offset);
        LOG_INFO("import_offset 0x{:x}", (uint32_t)header.import_offset);
        LOG_INFO("fixup_offsets 0x{:x}", (uint32_t)header.fixup_offsets);
        LOG_INFO("profile_offset 0x{:x}", (uint32_t)header.profile_offset);
        LOG_INFO("cseg_size 0x{:x}", (uint32_t)header.cseg_size);
        LOG_INFO("name_offset 0x{:x}", (uint32_t)header.name_offset);
        LOG_INFO("string_count 0x{:x}", (uint32_t)header.string_count);
        LOG_INFO("export_count 0x{:x}", (uint32_t)header.export_count);
        LOG_INFO("import_count 0x{:x}", (uint32_t)header.import_count);
        LOG_INFO("fixup_count 0x{:x}", (uint32_t)header.fixup_count);
        LOG_INFO("profile_count 0x{:x}", (uint32_t)header.profile_count);
        LOG_INFO("devblock_string_count 0x{:x}", (uint32_t)header.devblock_string_count);
        LOG_INFO("include_count 0x{:x}", (uint32_t)header.include_count);
        LOG_INFO("animtree_count 0x{:x}", (uint32_t)header.animtree_count);
        LOG_INFO("flags 0x{:x}", (uint32_t)header.flags);
    }

    void GscObfuscator::ValidateInScript(const void* ptr, const char* descr) {
        if (ptr > scriptEnd) {
            throw std::runtime_error(std::format("Invalid {} ({} > {})", descr, ptr, scriptEnd));
        }
    };
    void GscObfuscator::ValidateStringInScript(const char* ptr, const char* descr) {
        ValidateInScript(ptr, descr);
        while (*(ptr++)) {
            ValidateInScript(ptr, descr);
        }
    }

    void GscObfuscator::KillDevByteCodeOp(uint32_t& floc, size_t len, size_t delta) {
        if (opt.noDebugKill) {
            return; // disabled
        }
        byte* bc{ Aligned(&header.magic[floc + delta], len) };

        if (floc < 2 + delta || Aligned<uint16_t>(bc + len) + 2 > scriptEnd) {
            LOG_ERROR("Invalid dev op size");
            return;
        }

        // the loc isn't used, so we can use it as a trampoline
        for (size_t i = 0; i < len / TRAMPOLINE_SIZE; i++) {
            uint32_t rloc{ (uint32_t)(&bc[TRAMPOLINE_SIZE * i] - &header.magic[0]) };
            if (!junkLocation) {
                junkLocation = rloc;
            } else if (junkLocation != rloc) {
                trampolineFreeLocations.emplace_back(rloc);
            }
        }

        // we leave a small gift in the dev block

        // due to the alignment, we maybe kill (part of) the opcode
        *(uint16_t*)&header.magic[floc - 2] = 0xdead;
        // kill the opcode after the string (from what I know, a dev block can't end with a get string or a
        // function call without dectop)
        *(uint16_t*)Aligned<uint16_t>(&bc[len]) = 0xbeef;

        if (junkLocation) {
            floc = junkLocation;
        }
    };
    void GscObfuscator::RemoveLocalVariables() {
        for (auto& [name, exps] : exportsMap) {
            for (GscExportData& expd : exps) {
                byte* bc{ Aligned<uint16_t>(expd.bc) };
                uint16_t createParams{ *(uint16_t*)bc };
                if (std::find(
                        std::begin(SafeCreateLocalVariables_Opcodes),
                        std::end(SafeCreateLocalVariables_Opcodes),
                        createParams
                    ) != std::end(SafeCreateLocalVariables_Opcodes)) {
                    // we have variables, we can remove their names
                    byte count{ bc[2] };

                    if (count) {
                        bc = Aligned<uint32_t>(bc + 3);
                        ValidateInScript(bc + count * sizeof(uint32_t) * 2, "params bytecode");

                        for (size_t j = 0; j < count; j++) {
                            uint32_t* varName{ (uint32_t*)bc };
                            bc += 4;
                            byte flags{ *bc };
                            bc += 4; // 3 padding bytes

                            if ((flags & 2) == 0) {          // !varargs
                                *(uint32_t*)varName = 1 + j; // var_1, var_2, etc.
                            }
                        }
                    }
                }
            }
        }
    }

    void GscObfuscator::KillDevImports() {
        for (auto& [name, imps] : importsMap) {
            for (T7GSCImport* imp : imps) {
                uint32_t* offsets{ (uint32_t*)&imp[1] };
                if ((imp->flags & T7GIF_DEV_CALL) != 0) {
                    // kill dev block call information
                    constexpr uint32_t assertT7Hash = hash::HashT7("assert");
                    imp->name = assertT7Hash;
                    imp->param_count = 1;

                    size_t delta = (imp->flags & T7GIF_FUNC_METHOD) != 0 ? 1 : 0;
                    for (size_t j = 0; j < imp->num_address; j++) {
                        KillDevByteCodeOp(offsets[j], 8, delta);
                    }
                }
            }
        }
    }

    void GscObfuscator::KillDevStrings() {
        if (header.devblock_string_count) {
            T7GSCString* strings{ (T7GSCString*)&header.magic[header.devblock_string_offset] };
            ValidateInScript(strings + 1, "dev strings table");

            for (size_t i = 0; i < header.devblock_string_count; i++) {
                T7GSCString& str{ *strings };
                uint32_t* offsets{ (uint32_t*)&strings[1] };
                strings = (T7GSCString*)&offsets[strings->num_address];
                ValidateInScript(strings, "dev strings table");

                for (size_t j = 0; j < str.num_address; j++) {
                    KillDevByteCodeOp(offsets[j], 4, 0);
                }

                // kill dev block string information
                std::memset(offsets, 0, sizeof(*offsets) * str.num_address);
                std::memset(&str, 0, sizeof(str));
            }
            header.devblock_string_count = 0;
        }
        header.devblock_string_offset = 0;
    }

    void GscObfuscator::KillPrivateExports() {
        uint32_t privateCount{};
        for (auto& [nl, exps] : exportsMap) {
            if (exps.size() != 1) {
                LOG_WARNING(
                    "export {}::{} defined multiple times",
                    hashutils::ExtractTmp("namespace", nl.name_space),
                    hashutils::ExtractTmp("function", nl.name)
                );
                continue;
            }

            T7GSCExport& exp{ *exps[0].ref };
            if ((exp.flags & T7GEF_PRIVATE) != 0) {
                // private export, we can remove its name

                exp.name = ++privateCount;

                auto iit{ importsMap.find(nl) };
                if (iit == importsMap.end()) {
                    continue;
                }

                for (T7GSCImport* imp : iit->second) {
                    imp->name = exp.name;
                }
            }
        }
    }
    void GscObfuscator::ApplyPrivateStrings() {
        if (header.string_count) {
            T7GSCString* strings{ (T7GSCString*)&header.magic[header.string_offset] };
            ValidateInScript(strings + 1, "strings table");

            for (size_t i = 0; i < header.string_count; i++) {
                T7GSCString& str{ *strings };
                uint32_t* offsets{ (uint32_t*)&strings[1] };
                strings = (T7GSCString*)&offsets[strings->num_address];

                ValidateInScript(strings, "strings table");

                char* s{ (char*)&header.magic[str.string] };
                ValidateStringInScript(s, "string ref");
                opt.privateFileData.RenamedString(s);
            }
        }
    }

    void GscObfuscator::ApplyPrivateScripts() {
        char* name{ (char*)&header.magic[header.name_offset] };
        ValidateStringInScript(name, "script name");
        std::string_view sw{ name };

        bool client;
        if (sw.ends_with(".csc") || sw.ends_with(".csh")) {
            client = true;
        } else if (sw.ends_with(".gsc") || sw.ends_with(".gsh")) {
            client = false;
        } else {
            LOG_WARNING("Invalid script name: {}", name);
            return;
        }

        opt.privateFileData.RenamedScript(name);

        uint32_t* includes{ (uint32_t*)&header.magic[header.include_offset] };
        ValidateInScript(&includes[header.include_count], "includes");
        for (size_t i = 0; i < header.include_count; i++) {
            char* inc{ (char*)&header.magic[includes[i]] };
            ValidateStringInScript(inc, "include value");
            opt.privateFileData.RenamedScriptExt(inc, client);
        }
    }

    void GscObfuscator::ApplyPrivateHashes() {
        core::bytebuffer::ByteBuffer bb{ script, scriptLen };
        for (auto& [str, val] : opt.privateFileData.GetHashes()) {
            // Why would the user has hashed values here, it's its strings
            uint32_t hash{ hash::HashT7(str.data()) };

            size_t off;

            while ((off = bb.Find((byte*)&hash, sizeof(hash))) != std::string::npos) {
                *(uint32_t*)&script[off] = val; // replace hash by its value
            }
        }
    }

    void GscObfuscator::CreateTrampolines() {
        std::sort(trampolineFreeLocations.begin(), trampolineFreeLocations.end(), [](uint32_t a, uint32_t b) {
            return a < b;
        });

        T7GSCExport* exports{ (T7GSCExport*)&header.magic[header.export_offset] };

        for (size_t i = 0; i < header.export_count; i++) {
            T7GSCExport& exp{ exports[i] };

            if (trampolineFreeLocations.empty()) {
                LOG_DEBUG("no enough trampolines");
                break; // no more locations
            }

            int64_t loc{};
            int64_t dest{ (int64_t)exp.address };

            for (auto it = trampolineFreeLocations.begin(); it != trampolineFreeLocations.end(); it++) {
                int64_t origin{ (int64_t)*it };
                int64_t delta{ dest - (origin + 4) }; // a jump delta starts from the end of the op data
                if (delta > INT16_MAX || delta < INT16_MIN) {
                    continue; // a jump delta is a signed 16 bits number, if we are bigger than that, we can't use
                              // this trampoline
                }

                loc = origin;
                trampolineFreeLocations.erase(it);
                break;
            }

            if (!loc) {
                LOG_DEBUG(
                    "no close trampoline for {}::{}",
                    hashutils::ExtractTmp("namespace", exp.name_space),
                    hashutils::ExtractTmp("function", exp.name)
                );
                continue; // no close location for this export
            }

            // if we are not in a dev op, we can use the devblock jump
            uint16_t jmpOpcode{ GetRandomOpCodes(Jump_Opcodes) };
            int16_t delta{ (int16_t)(dest - (loc + 4)) };

            *(uint16_t*)&header.magic[loc] = jmpOpcode;
            *(int16_t*)&header.magic[loc + 2] = delta;
            exp.address = (uint32_t)loc;

            LOG_TRACE(
                "create trampoline {}::{} at 0x{:x} -> 0x{:x} ({}0x{:x})",
                hashutils::ExtractTmp("namespace", exp.name_space),
                hashutils::ExtractTmp("function", exp.name),
                loc,
                dest,
                (delta < 0 ? "-" : ""),
                (delta < 0 ? -delta : delta)
            );
        }
    }
    void GscObfuscator::ComputeChecksums() {
        for (auto& [nl, exps] : exportsMap) {
            for (GscExportData& data : exps) {
                if (!opt.recomputeCRC) {
                    // we can remove the checksum by computing only one byte, it kills cerberus
                    data.ref->checksum = ComputeCRC32(data.bc, 1);
                } else if (data.len) {
                    // we recompute the export checksum, like that cerberus can accept the data
                    data.ref->checksum = ComputeCRC32(data.bc, data.len);
                }
            }
        }
    }

    void GscObfuscator::RunTasks() {
        if (opt.printData) {
            PrintHeader();
        }
        if (!opt.noRemoveLocals) {
            RemoveLocalVariables();
        }
        if (!opt.noRemovePrivateExports) {
            KillPrivateExports();
        }
        ApplyPrivateStrings();
        ApplyPrivateScripts();
        ApplyPrivateHashes();
        if (!opt.noDebugKill) {
            KillDevImports();
            KillDevStrings();
        }
        if (!opt.noTrampoline) {
            CreateTrampolines();
        }
        ComputeChecksums();
    }

} // namespace tool::gsc::obfuscator::handler