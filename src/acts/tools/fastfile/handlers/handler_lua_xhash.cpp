#include <includes.hpp>
#include <tools/fastfile/fastfile_handlers.hpp>

namespace {

    class LuaXHashFFHandler : public fastfile::FFHandler {
    public:
        LuaXHashFFHandler() : fastfile::FFHandler("LuaXHash", "Dump lua ui scripts xhash based", true) {
        }

        void Handle(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& buff, fastfile::FastFileContext& ctx) override {
            // search lua file
            {
                std::filesystem::path out{ opt.m_output / "luafile" };

                struct {
                    uint32_t magic;
                    uint32_t mask;
                    const char* type;
                } magics[]{
                    { 0x61754C1B, 0xFFFFFFFF, "hks lua" },
                    { 0x4A4C1B, 0xFFFFFF, "luajit" },
                };

                for (auto& magic : magics) {
                    size_t loc{};
                    LOG_DEBUG("searching {} 0x{:x}/0x{:x}", magic.type, magic.magic, magic.mask);
                    while (true) {
                        buff.Goto(loc);
                        if (buff.CanRead(4)) buff.Skip(4);
                        loc = buff.FindMasked((byte*)&magic.magic, (byte*)&magic.mask, sizeof(magic.magic));

                        if (loc == std::string::npos) break;

                        buff.Goto(loc);

                        size_t size;
                        byte* bufferData{ buff.ReadPtr<byte>() };

                        if ((*(uintptr_t*)(bufferData - 8)) != 0xFFFFFFFFFFFFFFFF) {
                            LOG_TRACE("Can't get ptr for loc 0x{:x}", loc);
                            continue; // not a valid candidate
                        }
                        size = (size_t)*(uint32_t*)(bufferData - 0x10);

                        if (!size) {
                            continue;
                        }

                        uint64_t name;

                        if (*(uint64_t*)(bufferData - 0x18)) {
                            name = *(uint64_t*)(bufferData - 0x18); // xhash64
                        }
                        else {
                            name = *(uint64_t*)(bufferData - 0x20); // xhash128
                        }

                        LOG_TRACE("lua: 0x{:x} 0x{:x}: {}", loc, size, hashutils::ExtractTmpScript(name));



                        const char* nameStr{ hashutils::ExtractPtr(name) };

                        if (!nameStr) {
                            nameStr = utils::va("hashed/%llx.lua", name);
                        }

                        if (!buff.CanRead(size - 1)) {
                            LOG_ERROR("Bad size 0x{:x} for loc 0x{:x}, name: {}", size, loc, nameStr);
                            continue;
                        }

                        buff.Skip(size - 1);

                        std::filesystem::path outFile{ out / nameStr };
                        std::filesystem::create_directories(outFile.parent_path());

                        if (!utils::WriteFile(outFile, bufferData, size)) {
                            LOG_ERROR("Can't write {}", outFile.string());
                        }
                        else {
                            LOG_OPT_INFO("Dump {}", outFile.string());
                        }
                    }
                }



            }
        }
    };

    // utils::ArrayAdder<LuaXHashFFHandler, fastfile::FFHandler> arr{ fastfile::GetHandlers() };
}