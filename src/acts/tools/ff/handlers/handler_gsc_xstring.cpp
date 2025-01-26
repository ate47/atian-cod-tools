#include <includes.hpp>
#include <tools/ff/fastfile_handlers.hpp>

namespace {

	class GscXStringFFHandler : public fastfile::FFHandler {
	public:
		GscXStringFFHandler() : fastfile::FFHandler("GscXString", "Dump gsc scripts xstring based") {
		}

		void Handle(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& decReader, fastfile::FastFileContext& ctx) override {

			{

				struct ScriptParseTree {
					int64_t name;
					int32_t len;
					int32_t pad;
					int64_t buffer;
				}; static_assert(sizeof(ScriptParseTree) == 0x18);
				// dump spt dbg
				ScriptParseTree sptSearch{};
				ScriptParseTree sptMask{};
				memset(&sptMask, 0xff, sizeof(sptMask));
				sptMask.len = 0;
				sptSearch.buffer = 0xFFFFFFFFFFFFFFFF;
				sptSearch.name = 0xFFFFFFFFFFFFFFFF;

				std::filesystem::path outDir{ opt.m_output / "spt" };

				size_t off{};
				while (true) {
					if ((off = decReader.FindMasked(&sptSearch, &sptMask, sizeof(sptSearch))) == std::string::npos) {
						break;
					}
					decReader.Goto(off);
					off++;
					ScriptParseTree& spt{ *decReader.ReadPtr<ScriptParseTree>() };

					const char* name{ decReader.ReadString() };

					std::string_view sw{ name };
					if (spt.len < 8 || !(sw.ends_with(".gsc") || sw.ends_with(".csc") || sw.ends_with(".gsh") || sw.ends_with(".csh"))) {
						continue; // not a candidate
					}


					LOG_TRACE("spt {} -> 0x{:x}", name, spt.len);

					if (!decReader.CanRead(spt.len)) {
						continue; // bad size
					}
					byte* data{ decReader.ReadPtr<byte>(spt.len + 1) };

					if ((*(uint64_t*)data & 0xFFFFFFFFFFFFF) != 0xa0d43534780) {
						if ((*(uint64_t*)data & 0xFFFFFFFFFFFFF) != 0xA0D42444780) {
							LOG_ERROR("Bad gsc magic {:x}", *(uint64_t*)data); // not a gscc file
						}
						continue; // bad magic
					}

					std::filesystem::path outFile{ outDir / std::format("vm_{:x}/{}c", *(uint64_t*)data, name) };
					std::filesystem::create_directories(outFile.parent_path());
					if (!utils::WriteFile(outFile, data, spt.len)) {
						LOG_ERROR("Error when dumping");
					}
					else {
						LOG_INFO("Dump into {}", outFile.string());
					}
				}
			}

			{
				struct ScriptParseTreeDBG
				{
					uintptr_t name;
					int gdbLen;
					int srcLen;
					uintptr_t gdb;
					uintptr_t src;
				}; static_assert(sizeof(ScriptParseTreeDBG) == 0x20);

				// dump spt
				ScriptParseTreeDBG sptSearch{};
				ScriptParseTreeDBG sptMask{};
				memset(&sptMask, 0xff, sizeof(sptMask));
				sptMask.gdbLen = 0;
				sptMask.srcLen = 0;
				sptSearch.name = 0xFFFFFFFFFFFFFFFF;
				sptSearch.gdb = 0xFFFFFFFFFFFFFFFF;
				sptSearch.src = 0xFFFFFFFFFFFFFFFF;
				
				std::filesystem::path outDir{ opt.m_output / "sptdbg" };

				size_t off{};
				while (true) {
					if ((off = decReader.FindMasked(&sptSearch, &sptMask, sizeof(sptSearch))) == std::string::npos) {
						break;
					}
					decReader.Goto(off);
					off++;
					ScriptParseTreeDBG& spt{ *decReader.ReadPtr<ScriptParseTreeDBG>() };

					const char* name{ decReader.ReadString() };

					std::string_view sw{ name };
					if (spt.gdbLen < 8 || !(sw.ends_with(".gsc") || sw.ends_with(".csc") || sw.ends_with(".gsh") || sw.ends_with(".csh"))) {
						continue; // not a candidate
					}


					LOG_INFO("dbg {} -> 0x{:x}", name, spt.gdbLen);

					if (!decReader.CanRead(spt.gdbLen)) {
						continue; // bad size
					}
					byte* data{ decReader.ReadPtr<byte>(spt.gdbLen + 1) };

					if ((*(uint64_t*)data & 0xFFFFFFFFFFFFF) != 0xA0D42444780) {
						// bad magic
						if ((*(uint64_t*)data & 0xFFFFFFFFFFFFF) != 0xa0d43534780) {
							LOG_ERROR("Bad gsc magic {:x}", *(uint64_t*)data); // not a dbg file
						}
						continue;
					}
					{
						std::filesystem::path outFile{ outDir / std::format("vm_{:x}/{}.gdbc", *(uint64_t*)data, name) };
						std::filesystem::create_directories(outFile.parent_path());
						if (!utils::WriteFile(outFile, data, spt.gdbLen)) {
							LOG_ERROR("Error when dumping");
						}
						else {
							LOG_INFO("Dump into {}", outFile.string());
						}
					}

					if (!decReader.CanRead(spt.srcLen)) {
						continue; // bad size
					}
					{
						byte* datasrc{ decReader.ReadPtr<byte>(spt.srcLen + 1) };

						std::filesystem::path outFile{ outDir / std::format("vm_{:x}/{}", *(uint64_t*)data, name) };
						std::filesystem::create_directories(outFile.parent_path());
						if (!utils::WriteFile(outFile, datasrc, spt.srcLen)) {
							LOG_ERROR("Error when dumping");
						}
						else {
							LOG_INFO("Dump into {}", outFile.string());
						}
					}
				}
			}
		}
	};

	utils::ArrayAdder<GscXStringFFHandler, fastfile::FFHandler> arr{ fastfile::GetHandlers() };
}