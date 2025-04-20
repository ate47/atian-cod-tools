#include <includes.hpp>
#include <tools/gsc.hpp>
#include <core/bytebuffer.hpp>
#include <hook/module_mapper.hpp>
#include <compatibility/xensik_gscbin.hpp>
#include "mw19.hpp"


namespace tool::mw19 {
    namespace {
        const char* IW19PoolNames[]{
            "physicslibrary",
            "physicssfxeventasset",
            "physicsvfxeventasset",
            "physicsasset",
            "physicsfxpipeline",
            "physicsfxshape",
            "physicsdebugdata",
            "xanim",
            "xmodelsurfs",
            "xmodel",
            "mayhem",
            "material",
            "computeshader",
            "libshader",
            "vertexshader",
            "hullshader",
            "domainshader",
            "pixelshader",
            "techset",
            "image",
            "soundglobals",
            "soundbank",
            "soundbanktransient",
            "col_map",
            "com_map",
            "glass_map",
            "aipaths",
            "navmesh",
            "tacgraph",
            "map_ents",
            "fx_map",
            "gfx_map",
            "gfx_map_trzone",
            "iesprofile",
            "lightdef",
            "gradingclut",
            "ui_map",
            "fogspline",
            "animclass",
            "playeranim",
            "gesture",
            "localize",
            "attachment",
            "weapon",
            "vfx",
            "impactfx",
            "surfacefx",
            "aitype",
            "mptype",
            "character",
            "xmodelalias",
            "rawfile",
            "scriptfile",
            "scriptdebugdata",
            "stringtable",
            "leaderboarddef",
            "virtualleaderboarddef",
            "ddl",
            "tracer",
            "vehicle",
            "addon_map_ents",
            "netconststrings",
            "luafile",
            "scriptable",
            "equipsndtable",
            "vectorfield",
            "particlesimanimation",
            "streaminginfo",
            "laser",
            "ttf",
            "suit",
            "suitanimpackage",
            "camera",
            "hudoutline",
            "spaceshiptarget",
            "rumble",
            "rumblegraph",
            "animpkg",
            "sfxpkg",
            "vfxpkg",
            "footstepvfx",
            "behaviortree",
            "aianimset",
            "aiasm",
            "proceduralbones",
            "dynamicbones",
            "reticle",
            "xanimcurve",
            "coverselector",
            "enemyselector",
            "clientcharacter",
            "clothasset",
            "cinematicmotion",
            "accessory",
            "locdmgtable",
            "bulletpenetration",
            "scriptbundle",
            "blendspace2d",
            "xcam",
            "camo",
            "xcompositemodel",
            "xmodeldetailcollision",
            "streamkey",
            "streamtreeoverride",
            "keyvaluepairs",
            "stterrain",
            "nativescriptpatch",
            "collisiontile",
            "execution",
            "carryobject",
            "soundbanklist",
            "decalvolumematerial",
            "decalvolumemask",
            "dynentitylist",
            "fx_map_trzone",
            "dlogschema",
            "edgelist",
            "triggereffect",
            "weapontrigger",
            "dwidata",
        };


	    int mw19_exe(int argc, const char* argv[]) {
		    std::filesystem::path exe{ argv[2] };

hook::module_mapper::Module mod{ true };

LOG_INFO("Loading module {}", exe.string());
if (!mod.Load(exe) || !mod) {
    LOG_ERROR("Can't load module");
    return tool::BASIC_ERROR;
}

LOG_INFO("Loaded");



auto* pools{ mod->Get<uintptr_t>(0x7221160) };
{
    std::filesystem::path pf{ exe.parent_path() / "pools19.cpp" };
    std::ofstream osf{ pf };
    if (!osf) {
        LOG_ERROR("Can't open {}", pf.string());
    }
    else {
        utils::CloseEnd osfce{ osf };
        osf << "enum IW19PoolType : int {\n";
        for (size_t idx = 0; idx < mw19::IW19_ASSETTYPE_COUNT; idx++) {
            if (!pools[idx]) break;
            const char* cc = mod->Rebase<const char>(pools[idx]);
            char* ccu = utils::UpperCase(utils::CloneString(cc));
            osf << "    IW19_ASSETTYPE_" << ccu << " = " << std::dec << idx << ", // 0x" << std::hex << idx << "\n";
        }
        osf << "};\n";

        osf << "const char* IW19PoolNames[] {\n";
        for (size_t idx = 0; idx < mw19::IW19_ASSETTYPE_COUNT; idx++) {
            if (!pools[idx]) break;
            const char* cc = mod->Rebase<const char>(pools[idx]);
            osf << "    \"" << cc << "\",\n";
        }
        osf << "};\n";
    }
    LOG_INFO("Created {}", pf.string());
}

return tool::OK;
        }

        const char* DecrytString(const char* str) {
            if ((*str & 0xC0) == 0x80) {
                return "encrypted";
            }

            return str;
        }

        int gscdmw19(int argc, const char* argv[]) {
            LOG_WARNING("TOOL MOVED TO 'gscd'");
            using namespace tool::gsc;
            using namespace tool::gsc::opcode;
            if (tool::NotEnoughParam(argc, 2)) return tool::BAD_USAGE;

            std::filesystem::path outDir{ argv[3] };
            std::vector<std::filesystem::path> scriptFiles{};
            std::filesystem::path scriptDir{ argv[2] };
            utils::GetFileRecurse(scriptDir, scriptFiles, [](const std::filesystem::path& f) {
                const auto str = f.string();
                return str.ends_with(".gscbin") || str.ends_with(".gscc");
            }, true);

            std::filesystem::create_directories(outDir);

            std::string scriptBuffer{};

            RegisterOpCodesMap();
            VmInfo* iw8{ tool::gsc::opcode::GetVm(VMI_IW_BIN_MW19) };

            if (!iw8) {
                LOG_ERROR("Can't find iw8 vm");
                return tool::BASIC_ERROR;
            }

            for (const std::filesystem::path& scriptFile : scriptFiles) {
                if (!utils::ReadFile(scriptDir / scriptFile, scriptBuffer)) {
                    LOG_ERROR("Can't read {}", scriptFile.string());
                    return tool::BASIC_ERROR;
                }
                LOG_DEBUG("Loading {}", scriptFile.string());

                compatibility::xensik::gscbin::GscBinHeader& header{ *reinterpret_cast<compatibility::xensik::gscbin::GscBinHeader*>(scriptBuffer.data()) };

                std::filesystem::path outScript{ outDir / scriptFile.parent_path() / std::format("{}.gsc", scriptFile.filename().string()) };
                std::filesystem::create_directories(outScript.parent_path());
                std::ofstream asmout{ outScript };
                if (!asmout) {
                    LOG_ERROR("Can't open {}", outScript.string());
                    return tool::BASIC_ERROR;
                }
                utils::CloseEnd osce{ asmout };

                asmout << "// file .......... " << scriptFile.string() << "\n";
                asmout << "// bytecodeLen ... " << header.bytecodeLen << "\n";
                asmout << "// compressedLen . " << header.compressedLen << "\n";
                asmout << "// len ........... " << header.len << "\n";

                if (!header.len || !header.bytecodeLen) {
                    LOG_INFO("Done into {}", outScript.string());
                    continue;
                }

                auto decompressedData{ std::make_unique<byte[]>(header.len) };

                uLongf sizef = (uLongf)header.len;
                uLongf sizef2{ header.compressedLen };
                int ret;
                if (header.len && (ret = uncompress2(decompressedData.get(), &sizef, reinterpret_cast<const Bytef*>(header.GetBuffer()), &sizef2) < 0)) {
                    LOG_ERROR("Can't decompress file: {}", zError(ret));
                    return tool::BASIC_ERROR;
                }

                core::bytebuffer::ByteBuffer sourceReader{ decompressedData.get(), sizef };
                core::bytebuffer::ByteBuffer bytecodeReader{ header.GetByteCode(), header.bytecodeLen };


                auto ReadSourceToken = [&sourceReader]() -> const char* {
                    uint32_t id = sourceReader.Read<uint32_t>();

                    if (!id) {
                        return DecrytString(sourceReader.ReadString());
                    }

                    return utils::va("ref_%x", id);
                };

                constexpr size_t opaqueStringCount = 0x1472F;

                asmout << "// end: 0x" << std::hex << (int)bytecodeReader.Read<byte>() << "\n";
                // mw19 3b? mwii 32 end

                bool end{};
                while (!bytecodeReader.End() && !end) {
                    size_t offset{ bytecodeReader.Loc() };
                    byte* funcStart{ bytecodeReader.Ptr() };
                    uint32_t size{ sourceReader.Read<uint32_t>() };
                    byte* funcEnd{ funcStart + size };
                    const char* name{ ReadSourceToken() };

                    asmout
                        << "\n"
                        << "// name: " << name << "\n"
                        << "// offset: 0x" << std::hex << offset << "\n"
                        << "// size: 0x" << std::hex << size << "\n"
                        ;

                    while (bytecodeReader.Ptr() < funcEnd && !end) {
                        asmout << "." << std::hex << std::setfill('0') << std::setw(4) << bytecodeReader.Loc() << " ";
                        byte opcode = bytecodeReader.Read<byte>();

                        const OPCodeInfo* nfo{ LookupOpCode(VMI_IW_BIN_MW19, PLATFORM_PC, opcode) };

                        asmout 
                            << "0x" << std::hex << std::setfill('0') << std::setw(2) << (int)opcode 
                            << " " << std::setw(0x20) << std::setfill('.') << nfo->m_name << "(" << std::dec << std::setfill(' ') << std::setw(3) << (int)opcode << ")" << " | ";
                        switch (opcode) {
                        case 0x01:
                        case 0x03:
                        case 0x14:
                        case 0x16:
                        case 0x28:
                        case 0x30:
                        case 0x34:
                        case 0x44:
                        case 0x45:
                        case 0x46:
                        case 0x47:
                        case 0x51:
                        case 0x52:
                        case 0x5b:
                        case 0x63:
                        case 0x68:
                        case 0x6b:
                        case 0x71:
                        case 0x74:
                        case 0x8c:
                        case 0x8e:
                        case 0x93:
                        case 0xa1:
                        case 0xb9:
                        case 0xba:
                        case 0xbb:
                        case 0xbc:
                            asmout << std::hex << "0x" << (int)bytecodeReader.Read<byte>() << "\n";
                            break;
                        case 0x0b:
                        case 0x0e:
                        case 0x19:
                        case 0x1b:
                        case 0x1c:
                        case 0x1d:
                        case 0x1e:
                        case 0x1f:
                        case 0x20:
                        case 0x21:
                        case 0x22:
                        case 0x32:
                        case 0x50:
                        case 0x55:
                        case 0x66:
                        case 0x72:
                        case 0x7e:
                        case 0x7f:
                        case 0x80:
                        case 0x81:
                        case 0x82:
                        case 0x83:
                            // += 2
                            asmout
                                << std::hex << "0x" << bytecodeReader.Read<uint16_t>()
                                << "\n"
                                ;
                            break;
                        case 0x23:
                        case 0x33:
                        case 0x4f:
                        case 0x84:
                        case 0x89:
                        case 0x98:
                        case 0xa9:
                        case 0xaa:
                        case 0xab:
                        case 0xac:
                            // += 3
                            asmout
                                << std::hex << "0x" << bytecodeReader.Read<uint16_t>()
                                << std::hex << ", 0x" << (int)bytecodeReader.Read<byte>()
                                << "\n"
                                ;
                            break;
                        case 0x29:
                        case 0x35:
                        case 0x48:
                        case 0x49:
                        case 0x6e:
                        case 0x70:
                        case 0x86:
                        case 0x94:
                        case 0xad:
                        case 0xae:
                        case 0xaf:
                        case 0xb0:
                            // += 4
                            asmout
                                << std::hex << "0x" << bytecodeReader.Read<uint32_t>()
                                << "\n"
                                ;
                            break;
                        case 0xa0:
                            // += 5
                            asmout
                                << std::hex << bytecodeReader.Read<uint32_t>()
                                << std::hex << ", 0x" << (int)bytecodeReader.Read<byte>()
                                << "\n"
                                ;
                            break;
                        case 0x13:
                            // += 12
                            asmout
                                << std::dec << bytecodeReader.Read<float>()
                                << std::dec << ", " << bytecodeReader.Read<float>()
                                << std::dec << ", " << bytecodeReader.Read<float>()
                                << "\n"
                                ;
                            break;
                        case 0xa6:
                        case 0xbd: {
                            // += *bytecodeRef++
                            byte count{ bytecodeReader.Read<byte>() };
                            asmout << std::hex << "count: 0x" << (int)count;

                            while (count--) {
                                asmout << std::hex << ", 0x" << (int)bytecodeReader.Read<byte>();
                            }
                            asmout << "\n"
                                ;
                        }
                            break;
                        case 0xb1:
                        case 0xb2:
                        case 0xb3:
                        case 0xb4:
                            // += 3 and read dw data
                            asmout 
                                << "data3: 0x" << std::hex << sourceReader.Read<uint32_t>()
                                << std::hex << ", 0x" << bytecodeReader.Read<uint16_t>()
                                << std::hex << ", 0x" << (int)bytecodeReader.Read<byte>()
                                << "\n"
                                ;
                            break;
                        case 0xb5:
                        case 0xb6:
                        case 0xb7:
                        case 0xb8:
                            // += 4 and read dw data
                            asmout 
                                << "data4: 0x" << std::hex << sourceReader.Read<uint32_t>()
                                << std::hex << ", 0x" << bytecodeReader.Read<uint32_t>()
                                << "\n"
                                ;
                            break;
                        case 0x91: {
                                // #using_animtree  related
                            const char* unkstr1{ sourceReader.ReadString() };
                            const char* unkstr2{ sourceReader.ReadString() };
                            asmout 
                                << "\"" << DecrytString(unkstr1) << "\""
                                << ", \"" << DecrytString(unkstr2) << "\""
                                << std::hex << "0x" << bytecodeReader.Read<uint64_t>() << "\n"
                                ;
                        }
                            break;
                        case 0x60:
                        case 0x75:
                        case 0x92:
                        case 0x96: {
                            asmout 
                                << "token: " << ReadSourceToken()
                                << ", token2: " << ReadSourceToken();
                            // read 3 bytes
                            asmout
                                << std::hex << " / data: 0x" << bytecodeReader.Read<uint16_t>()
                                << std::hex << ", 0x" << (int)bytecodeReader.Read<byte>()
                                << "\n";
                        }

                            break;
                        case 0x4:
                        case 0x7:
                        case 0x8:
                        case 0x18:
                        case 0x25:
                        case 0x2c:
                        case 0x38:
                        case 0x3c:
                        case 0x4c:
                        case 0x6d:
                        case 0x7b:
                        case 0x8d: {
                            uint32_t id{ bytecodeReader.Read<uint32_t>() };

                            if (id <= opaqueStringCount) {
                                asmout << "str(opaque): 0x" << std::hex << id << "\n";
                            }
                            else {
                                asmout << "str: " << ReadSourceToken() << "\n";
                            }

                        }
                            break;
                        case 0x5:
                        case 0x17:
                        case 0x65:
                        case 0x8a: {
                            asmout 
                                << "token2: " << ReadSourceToken()
                                << " / " << ReadSourceToken() 
                                << ", val 0x" << std::hex << bytecodeReader.Read<uint32_t>()
                                << "\n";
                        }
                            break;
                        case 0x56: {
                            uint16_t count{ bytecodeReader.Read<uint16_t>() };

                            asmout << "count: " << std::dec << count << "\n";

                            for (size_t i = 0; i < count; i++) {
                                asmout << "." << std::hex << std::setfill('0') << std::setw(4) << bytecodeReader.Loc() << " ";
                                int32_t val{ bytecodeReader.Read<int32_t>() };
                                asmout << "0x" << std::hex << val;
                                bytecodeReader.Skip(3); //rloc/type?

                                if (val < 0x100000) {
                                    const char* valStr{ sourceReader.ReadString() };
                                    asmout << ": " << DecrytString(valStr);
                                }
                                asmout << "\n";
                            }

                        }
                            break;
                        case 0x5f:
                        case 0x7a: {
                            const char* valStr{ sourceReader.ReadString() };
                            asmout << "data95: " << DecrytString(valStr) << ", " << bytecodeReader.Read<uint32_t>() << "\n";
                        }
                            break;
                        case 0x62: { // get anim tree
                            const char* valStr{ sourceReader.ReadString() };
                            asmout << "data98: " << DecrytString(valStr) << ", " << (int)bytecodeReader.Read<byte>() << "\n";
                        }
                            break;
                        default:
                            asmout << "-\n";
                            break;
                        }
                    }

                }
                LOG_INFO("Done into {}", outScript.string());
            }

            return tool::OK;
        }


    }

    const char* PoolName(IW19PoolType type) {
        return type >= 0 && type < ARRAYSIZE(IW19PoolNames) ? IW19PoolNames[type] : "<invalid>";
    }

    IW19PoolType PoolId(const char* name) {
        for (size_t i = 0; i < ARRAYSIZE(IW19PoolNames); i++) {
            if (!_strcmpi(IW19PoolNames[i], name)) {
                return (IW19PoolType)i;
            }
        }
        return IW19_ASSETTYPE_COUNT;
    }
    int gscopaquemw19(int argc, const char* argv[]) {
        if (tool::NotEnoughParam(argc, 2)) return tool::BAD_USAGE;

        hook::module_mapper::Module mod{ true };
        if (!mod.Load(argv[2], false)) {
            LOG_ERROR("Can't load module");
            return tool::BASIC_ERROR;
        }

        auto* opaques{ mod->Get<char*>(0x47559D0) };
        constexpr size_t count = 72013;

        {
            utils::OutFileCE os{ argv[3], true };

            for (size_t i = 0; i < count; i++) {
                os << "0x" << std::hex << std::setfill('0') << std::setw(4) << i << "\t" << mod->Rebase(opaques[i]) << "\n";
            }
        }
        LOG_INFO("Dump into {}", argv[3]);



        return tool::OK;
    }

    // moved to gscd
    //ADD_TOOL(gscdmw19, "dev_gsc", " [script]", "Test gsc decompiler", gscdmw19);
    ADD_TOOL(gscopaquemw19, "dev_gsc", " [dump] [out]", "Test opaque dump", gscopaquemw19);
}