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
            if (tool::NotEnoughParam(argc, 1)) return tool::BAD_USAGE;

            std::filesystem::path scriptFile{ argv[2] };
            std::string scriptBuffer{};

            if (!utils::ReadFile(scriptFile, scriptBuffer)) {
                LOG_ERROR("Can't read {}", scriptFile.string());
                return tool::BASIC_ERROR;
            }

            compatibility::xensik::gscbin::GscBinHeader& header{ *reinterpret_cast<compatibility::xensik::gscbin::GscBinHeader*>(scriptBuffer.data()) };

            std::filesystem::path outScript{ scriptFile.parent_path() / std::format("{}.gsc", scriptFile.filename().string()) };
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

            auto decompressedData{ std::make_unique<byte[]>(header.len) };

            uLongf sizef = (uLongf)header.len;
            uLongf sizef2{ header.compressedLen };
            int ret;
            if (header.len && (ret = uncompress2(decompressedData.get(), &sizef, reinterpret_cast<const Bytef*>(header.GetBuffer()), &sizef2) < 0)) {
                LOG_ERROR("Can't decompress file: {}", zError(ret));
            }

            core::bytebuffer::ByteBuffer dataReader{ decompressedData.get(), sizef };
            core::bytebuffer::ByteBuffer bytecodeReader{ header.GetByteCode(), header.bytecodeLen };

            asmout << "// unk0: 0x" << std::hex << (int)bytecodeReader.Read<byte>() << "\n";
            // mw19 3b? mwii 32 bytecode revision??

            while (!bytecodeReader.End()) {
                byte* funcStart = dataReader.Ptr();

                uint32_t unkfunc0 = dataReader.Read<uint32_t>();
                uint32_t unkfunc4 = dataReader.Read<uint32_t>();

                asmout << "// unkfunc0: 0x" << std::hex << unkfunc0 << "\n"
                    ;

                if (!unkfunc4) {
                    const char* unkStr{ dataReader.ReadString() };

                    asmout << "// \"" << DecrytString(unkStr) << "\"\n";
                }
                else {
                    asmout << "// unkfunc4: 0x" << std::hex << unkfunc4 << "\n";
                }

                bool end{};
                while (!bytecodeReader.End() && !end) {
                    byte opcode = bytecodeReader.Read<byte>();

                    if (!opcode) {
                        asmout << "INVALID OPCODE 0\n";
                        bytecodeReader.GotoEnd();
                        break;
                    }
                    opcode--;

                    asmout << std::dec << std::setfill(' ') << std::setw(3) << (int)opcode << " | ";
                    switch (opcode) {
                    case 4:
                    case 22:
                    case 100:
                    case 137:
                        asmout << "end\n";
                        end = true;
                        break;
                    case 0:
                    case 2:
                    case 19:
                    case 21:
                    case 39:
                    case 47:
                    case 51:
                    case 67:
                    case 68:
                    case 69:
                    case 70:
                    case 80:
                    case 81:
                    case 90:
                    case 98:
                    case 103:
                    case 106:
                    case 112:
                    case 115:
                    case 139:
                    case 141:
                    case 146:
                    case 160:
                    case 184:
                    case 185:
                    case 186:
                    case 187:
                        asmout << std::hex << "0x" << (int)bytecodeReader.Read<byte>() << "\n";
                        break;
                    case 10:
                    case 13:
                    case 24:
                    case 26:
                    case 27:
                    case 28:
                    case 29:
                    case 30:
                    case 31:
                    case 32:
                    case 33:
                    case 49:
                    case 79:
                    case 84:
                    case 101:
                    case 113:
                    case 125:
                    case 126:
                    case 127:
                    case 128:
                    case 129:
                    case 130:
                        // += 2
                        asmout
                            << std::hex << "0x" << bytecodeReader.Read<uint16_t>()
                            << "\n"
                            ;
                        break;
                    case 34:
                    case 50:
                    case 78:
                    case 131:
                    case 136:
                    case 151:
                    case 168:
                    case 169:
                    case 170:
                    case 171:
                        // += 3
                        asmout
                            << std::hex << "0x" << bytecodeReader.Read<uint16_t>()
                            << std::hex << ", 0x" << (int)bytecodeReader.Read<byte>()
                            << "\n"
                            ;
                        break;
                    case 40:
                    case 52:
                    case 71:
                    case 72:
                    case 109:
                    case 111:
                    case 133:
                    case 147:
                    case 172:
                    case 173:
                    case 174:
                    case 175:
                        // += 4
                        asmout
                            << std::hex << "0x" << bytecodeReader.Read<uint32_t>()
                            << "\n"
                            ;
                        break;
                    case 159:
                        // += 5
                        asmout
                            << std::hex << bytecodeReader.Read<uint32_t>()
                            << std::hex << ", 0x" << (int)bytecodeReader.Read<byte>()
                            << "\n"
                            ;
                        break;
                    case 18:
                        // += 12
                        asmout
                            << std::dec << bytecodeReader.Read<float>()
                            << std::dec << ", " << bytecodeReader.Read<float>()
                            << std::dec << ", " << bytecodeReader.Read<float>()
                            << "\n"
                            ;
                        break;
                    case 165:
                    case 188: {
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
                    case 176:
                    case 177:
                    case 178:
                    case 179:
                        // += 3 and read dw data
                        asmout 
                            << "data3: 0x" << std::hex << dataReader.Read<uint32_t>()
                            << std::hex << ", 0x" << bytecodeReader.Read<uint16_t>()
                            << std::hex << ", 0x" << (int)bytecodeReader.Read<byte>()
                            << "\n"
                            ;
                        break;
                    case 180:
                    case 181:
                    case 182:
                    case 183:
                        // += 4 and read dw data
                        asmout 
                            << "data4: 0x" << std::hex << dataReader.Read<uint32_t>()
                            << std::hex << ", 0x" << bytecodeReader.Read<uint32_t>()
                            << "\n"
                            ;
                        break;
                    case 144: {
                            // #using_animtree  related
                        const char* unkstr1{ dataReader.ReadString() };
                        const char* unkstr2{ dataReader.ReadString() };
                        asmout 
                            << "\"" << DecrytString(unkstr1) << "\""
                            << ", \"" << DecrytString(unkstr2) << "\""
                            << std::hex << "0x" << bytecodeReader.Read<uint64_t>() << "\n"
                            ;
                    }
                        break;
                    case 95:
                    case 116:
                    case 145:
                    case 149:

                        break;
                    case 3:
                    case 6:
                    case 7:
                    case 23:
                    case 36:
                    case 43:
                    case 55:
                    case 59:
                    case 75:
                    case 108:
                    case 122:
                    case 140:
                        // create vars?
                        // asmout << "data0: 0x" << std::hex << dataReader.Read<uint32_t>();
                    case 85:
                        //
                    case 94:
                    case 121:
                        //
                        //
                    case 97:
                        //
                        asmout << "Not implemented yet\n";
                        end = true;
                        bytecodeReader.GotoEnd();
                        break;
                    default:
                        asmout << "-\n";
                        break;
                    }
                }

            }

            LOG_INFO("Done into {}", outScript.string());
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