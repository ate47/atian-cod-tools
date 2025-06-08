#include <includes.hpp>
#include <tools/ff/fastfile_handlers.hpp>
#include <tools/gsc_opcodes_load.hpp>

namespace {

	class GscXHashFFHandler : public fastfile::FFHandler {
        size_t count{};
        bool anyDbg{};
	public:
		GscXHashFFHandler() : fastfile::FFHandler("GscXHash", "Dump gsc scripts xhash based") {
		}
        void Init(fastfile::FastFileOption& opt) override {
            count = 0;
            anyDbg = false;
        }

        void Cleanup() override {
            if (count) LOG_INFO("{} file(s) dumped", count);
            if (anyDbg) LOG_WARNING("Debug file found");
        }

		void Handle(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& buff, fastfile::FastFileContext& ctx) override {

            // search scriptparsetreedbg
            uint64_t dbgMagic{ 0xA0D42444780 };
            uint64_t dbgMagicMask{ 0xFFFFFFFFFFFFF };
            if (buff.FindMasked(&dbgMagic, &dbgMagicMask, sizeof(dbgMagic)) != std::string::npos) {
                LOG_WARNING("FIND A DBG SPT");
                anyDbg = true;
            }

            // search scriptparsetree
            {
                uint64_t magic{ tool::gsc::opcode::VMI_TRE_BASE };
                uint64_t magicMask{ 0x00FFFFFFFFFFFFFF };

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
                        size = ((T8SPT*)sptCan)->size & 0x7FFFFFFF;
                    }
                    else if (((T8SPTOld*)sptCan)->buffer == 0xFFFFFFFFFFFFFFFF) {
                        size = ((T8SPTOld*)sptCan)->size & 0x7FFFFFFF;
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
                        LOG_INFO("Dump {} ({})", outFile.string(), hashutils::ExtractTmpScript(obj->name));
                        count++;
                    }

                    loc++;
                }

            }
            // search gscobj
            {
                uint64_t magic{ tool::gsc::opcode::VMI_IW_BASE };
                uint64_t magicMask{ 0xFFFFFFFFFFFFFF00 };

                size_t loc{};
                std::filesystem::path out{ opt.m_output / "gscobj" };
                buff.Goto(0);
                byte* start{ buff.Ptr<byte>() };
                while (true) {
                    loc = buff.FindMasked((byte*)&magic, (byte*)&magicMask, sizeof(magic));
                    if (loc == std::string::npos) break;
                    struct GscObjEntry {
                        uint64_t name;
                        uint32_t len;
                        void* obj;
                    };

                    buff.Goto(loc);
                    uint64_t smagic{ buff.Read<uint64_t>() };

                    if (loc < sizeof(GscObjEntry)) {
                        continue; // not enough data
                    }
                    buff.Goto(loc - sizeof(GscObjEntry));

                    GscObjEntry* entry{ buff.ReadPtr<GscObjEntry>() };

                    if (!entry->name) {
                        LOG_ERROR("0x{:x} INVALID {}/{} for 0x{:x}", smagic, entry->obj, hashutils::ExtractTmpScript(entry->name), loc);
                        continue;
                    }


                    if (!buff.CanRead(entry->len)) {
                        loc++;
                        LOG_ERROR("Bad size 0x{:x} for loc 0x{:x}", entry->len, loc);
                        continue;
                    }

                    LOG_TRACE("gsc: 0x{:x} 0x{:x} 0x{:x}: {}", smagic, loc, entry->len, hashutils::ExtractTmpScript(entry->name));

                    byte* obj{ buff.ReadPtr<byte>(entry->len) };

                    std::filesystem::path outFile{ out / std::format("vm_{:x}/script_{:x}.gscc", smagic, entry->name) };
                    std::filesystem::create_directories(outFile.parent_path());

                    if (!utils::WriteFile(outFile, obj, entry->len)) {
                        LOG_ERROR("Can't write {}", outFile.string());
                    }
                    else {
                        LOG_INFO("Dump {} ({})", outFile.string(), hashutils::ExtractTmpScript(entry->name));
                        count++;
                    }

                    loc++;
                }

            }
		}
	};

	utils::ArrayAdder<GscXHashFFHandler, fastfile::FFHandler> arr{ fastfile::GetHandlers() };
}