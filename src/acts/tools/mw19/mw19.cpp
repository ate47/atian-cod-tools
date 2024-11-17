#include <includes.hpp>
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

                asmout << "// unk0: 0x" << std::hex << (int)bytecodeReader.Read<byte>() << "\n";
                // mw19 3b? mwii 32 bytecode revision??

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

                        asmout << std::dec << std::setfill(' ') << std::setw(3) << (int)opcode << " | ";
                        switch (opcode) {
                        case 1:
                        case 3:
                        case 20:
                        case 22:
                        case 40:
                        case 48:
                        case 52:
                        case 68:
                        case 69:
                        case 70:
                        case 71:
                        case 81:
                        case 82:
                        case 91:
                        case 99:
                        case 104:
                        case 107:
                        case 113:
                        case 116:
                        case 140:
                        case 142:
                        case 147:
                        case 161:
                        case 185:
                        case 186:
                        case 187:
                        case 188:
                            asmout << std::hex << "0x" << (int)bytecodeReader.Read<byte>() << "\n";
                            break;
                        case 11:
                        case 14:
                        case 25:
                        case 27:
                        case 28:
                        case 29:
                        case 30:
                        case 31:
                        case 32:
                        case 33:
                        case 34:
                        case 50:
                        case 80:
                        case 85:
                        case 102:
                        case 114:
                        case 126:
                        case 127:
                        case 128:
                        case 129:
                        case 130:
                        case 131:
                            // += 2
                            asmout
                                << std::hex << "0x" << bytecodeReader.Read<uint16_t>()
                                << "\n"
                                ;
                            break;
                        case 35:
                        case 51:
                        case 79:
                        case 132:
                        case 137:
                        case 152:
                        case 169:
                        case 170:
                        case 171:
                        case 172:
                            // += 3
                            asmout
                                << std::hex << "0x" << bytecodeReader.Read<uint16_t>()
                                << std::hex << ", 0x" << (int)bytecodeReader.Read<byte>()
                                << "\n"
                                ;
                            break;
                        case 41:
                        case 53:
                        case 72:
                        case 73:
                        case 110:
                        case 112:
                        case 134:
                        case 148:
                        case 173:
                        case 174:
                        case 175:
                        case 176:
                            // += 4
                            asmout
                                << std::hex << "0x" << bytecodeReader.Read<uint32_t>()
                                << "\n"
                                ;
                            break;
                        case 160:
                            // += 5
                            asmout
                                << std::hex << bytecodeReader.Read<uint32_t>()
                                << std::hex << ", 0x" << (int)bytecodeReader.Read<byte>()
                                << "\n"
                                ;
                            break;
                        case 19:
                            // += 12
                            asmout
                                << std::dec << bytecodeReader.Read<float>()
                                << std::dec << ", " << bytecodeReader.Read<float>()
                                << std::dec << ", " << bytecodeReader.Read<float>()
                                << "\n"
                                ;
                            break;
                        case 166:
                        case 189: {
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
                        case 177:
                        case 178:
                        case 179:
                        case 180:
                            // += 3 and read dw data
                            asmout 
                                << "data3: 0x" << std::hex << sourceReader.Read<uint32_t>()
                                << std::hex << ", 0x" << bytecodeReader.Read<uint16_t>()
                                << std::hex << ", 0x" << (int)bytecodeReader.Read<byte>()
                                << "\n"
                                ;
                            break;
                        case 181:
                        case 182:
                        case 183:
                        case 184:
                            // += 4 and read dw data
                            asmout 
                                << "data4: 0x" << std::hex << sourceReader.Read<uint32_t>()
                                << std::hex << ", 0x" << bytecodeReader.Read<uint32_t>()
                                << "\n"
                                ;
                            break;
                        case 145: {
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
                        case 96:
                        case 117:
                        case 146:
                        case 150: {
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
                        case 4:
                        case 7:
                        case 8:
                        case 24:
                        case 37:
                        case 44:
                        case 56:
                        case 60:
                        case 76:
                        case 109:
                        case 123:
                        case 141: {
                            uint32_t id{ bytecodeReader.Read<uint32_t>() };

                            if (id <= opaqueStringCount) {
                                asmout << "str(opaque): 0x" << std::hex << id << "\n";
                            }
                            else {
                                asmout << "str: " << ReadSourceToken() << "\n";
                            }

                        }
                            break;
                        case 5:
                        case 23:
                        case 101:
                        case 138: {
                            asmout 
                                << "token2: " << ReadSourceToken()
                                << " / " << ReadSourceToken() 
                                << ", val 0x" << std::hex << bytecodeReader.Read<uint32_t>()
                                << "\n";
                        }
                            break;
                        case 86: {
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
                        case 95:
                        case 122: {
                            const char* valStr{ sourceReader.ReadString() };
                            asmout << "data95: " << DecrytString(valStr) << ", " << bytecodeReader.Read<uint32_t>() << "\n";
                        }
                            break;
                        case 98: {
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
    ADD_TOOL(gscdmw19, "mw19", "[script]", "Test gsc decompiler", gscdmw19);
}