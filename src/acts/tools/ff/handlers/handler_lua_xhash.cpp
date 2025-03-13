#include <includes.hpp>
#include <tools/ff/fastfile_handlers.hpp>

namespace {

    class LuaXHashFFHandler : public fastfile::FFHandler {
    public:
        LuaXHashFFHandler() : fastfile::FFHandler("LuaXHash", "Dump lua ui scripts xhash based") {
        }

        void Handle(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& buff, fastfile::FastFileContext& ctx) override {
            // search lua file
            {
                size_t loc{};
                std::filesystem::path out{ opt.m_output / "luafile" };
                buff.Goto(0);
                byte* start{ buff.Ptr<byte>() };


                struct {
                    uint32_t magic;
                    uint32_t mask;
                    const char* type;
                } magics[]{
                    { 0x61754C1B, 0xFFFFFFFF, "hks lua" },
                    { 0x4A4C1B, 0xFFFFFF, "luajit" },
                };

                for (auto& magic : magics) {
                    LOG_INFO("searching {} 0x{:x}/0x{:x}", magic.type, magic.magic, magic.mask);
                    while (true) {
                        loc = buff.FindMasked((byte*)&magic.magic, (byte*)&magic.mask, sizeof(magic.magic));
                        if (loc == std::string::npos) break;

                        buff.Goto(loc);

                        size_t size;
                        byte* bufferData{ buff.Ptr<byte>() };

                        if ((*(uintptr_t*)(bufferData - 8)) != 0xFFFFFFFFFFFFFFFF) {
                            LOG_ERROR("Can't get ptr for loc 0x{:x}", loc);
                            return; // not a valid candidate
                        }
                        size = (size_t)*(uint32_t*)(bufferData - 0x10);

                        uint64_t name;

                        if (*(uint64_t*)(bufferData - 0x18)) {
                            name = *(uint64_t*)(bufferData - 0x18); // xhash64
                        }
                        else {
                            name = *(uint64_t*)(bufferData - 0x20); // xhash128
                        }

                        LOG_TRACE("lua: 0x{:x} 0x{:x}: {}", loc, size, hashutils::ExtractTmpScript(name));


                        if (!buff.CanRead(size)) {
                            loc++;
                            LOG_ERROR("Bad size for loc 0x{:x}", size, loc);
                            return;
                        }
                        buff.Skip(size);

                        const char* nameStr{ hashutils::ExtractPtr(name) };

                        if (!nameStr) {
                            nameStr = utils::va("hashed/%llx.lua", name);
                        }

                        std::filesystem::path outFile{ out / nameStr };
                        std::filesystem::create_directories(outFile.parent_path());

                        if (!utils::WriteFile(outFile, bufferData, size)) {
                            LOG_ERROR("Can't write {}", outFile.string());
                        }
                        else {
                            LOG_INFO("Dump {}", outFile.string());
                        }

                        loc++;
                    }
                }



            }
        }
    };

    utils::ArrayAdder<LuaXHashFFHandler, fastfile::FFHandler> arr{ fastfile::GetHandlers() };
}