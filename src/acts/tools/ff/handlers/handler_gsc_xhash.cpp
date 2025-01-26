#include <includes.hpp>
#include <tools/ff/fastfile_handlers.hpp>

namespace {

	class GscXHashFFHandler : public fastfile::FFHandler {
	public:
		GscXHashFFHandler() : fastfile::FFHandler("GscXHash", "Dump gsc scripts xhash based") {
		}

		void Handle(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& buff, fastfile::FastFileContext& ctx) override {

            uint64_t dbgMagic{ 0xA0D42444780 };
            uint64_t dbgMagicMask{ 0xFFFFFFFFFFFFF };
            if (buff.FindMasked(&dbgMagic, &dbgMagicMask, sizeof(dbgMagic)) != std::string::npos) {
                LOG_WARNING("FIND A DBG SPT");
            }

            // search gscobj
            {
                /*
                 The pattern is :
                 align<8>
                 ScriptParseTree {
                    u64 name;
                    u64 namepad?; // maybe not there
                    void* buffer = -1;
                    u32 size;
                    u32 unk;
                 }
                 align<32>
                 GSC_OBJ {
                    u64 magic = 0x??00a0d43534780
                 }

                */
                uint64_t magic{ 0x00a0d43534780 };
                uint64_t magicMask{ 0xFFFFFFFFFFFFF };

                size_t loc{};
                std::filesystem::path out{ opt.m_output / "spt" };
                buff.Goto(0);
                byte* start{ buff.Ptr<byte>() };
                while (true) {
                    loc = buff.FindMasked((byte*)&magic, (byte*)&magicMask, sizeof(magic));
                    if (loc == std::string::npos) break;
                    struct T8GSCOBJ {
                        byte magic[8];
                        int32_t crc;
                        int32_t pad;
                        uint64_t name;
                    };

                    buff.Goto(loc);

                    if (!buff.CanRead(sizeof(T8GSCOBJ))) {
                        break; // can't read buffer
                    }

                    byte* sptCan{ buff.Ptr<byte>() - 0x18 }; // 0x18 is the minimum size to lookup

                    // we are 32 bytes aligned
                    T8GSCOBJ* obj{ buff.ReadPtr<T8GSCOBJ>() };

                    uint64_t name{ obj->name };

                    while (*(uint64_t*)sptCan != name) {
                        sptCan--;
                        if (start == sptCan) {
                            break;
                        }
                    }
                    if (start == sptCan) {
                        loc++;
                        continue;
                    }


                    uint64_t smagic{ *reinterpret_cast<uint64_t*>(obj) };

                    size_t size;

                    struct T8SPT {
                        uint64_t name;
                        uint64_t pad0;
                        uintptr_t buffer;
                        uint32_t size;
                        uint32_t pad02;
                    };

                    struct T8SPTOld {
                        uint64_t name;
                        uintptr_t buffer;
                        uint32_t size;
                        uint32_t pad02;
                    };

                    // Depending on how old the ff is, we might use a XHash of 0x10 or 8 bytes. The pointer
                    // to the buffer will constantly be -1 because it is not linked yet
                    if (((T8SPT*)sptCan)->buffer == 0xFFFFFFFFFFFFFFFF) {
                        size = ((T8SPT*)sptCan)->size;
                    }
                    else if (((T8SPTOld*)sptCan)->buffer == 0xFFFFFFFFFFFFFFFF) {
                        size = ((T8SPTOld*)sptCan)->size;
                    }
                    else {
                        LOG_ERROR("Can't get size 0x{:x} for loc 0x{:x}", smagic, loc);
                        loc++;
                        continue;
                    }

                    LOG_TRACE("gsc: 0x{:x} 0x{:x} 0x{:x}: {}", smagic, loc, size, hashutils::ExtractTmpScript(obj->name));


                    if (!buff.CanRead(size)) {
                        loc++;
                        LOG_ERROR("Bad size 0x{:x} 0x{:x} for loc 0x{:x}", smagic, size, loc);
                        continue;
                    }
                    buff.Skip(size);

                    std::filesystem::path outFile{ out / std::format("vm_{:x}/script_{:x}.gscc", smagic, obj->name) };
                    std::filesystem::create_directories(outFile.parent_path());

                    if (!utils::WriteFile(outFile, obj->magic, size)) {
                        LOG_ERROR("Can't write {}", outFile.string());
                    }
                    else {
                        LOG_INFO("Dump {}", outFile.string());
                    }

                    loc++;
                }

            }
		}
	};

	utils::ArrayAdder<GscXHashFFHandler, fastfile::FFHandler> arr{ fastfile::GetHandlers() };
}