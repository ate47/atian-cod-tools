#include <includes.hpp>
#include <tools/ff/fastfile_handlers.hpp>
#include <compatibility/xensik_gscbin.hpp>

namespace {

	class GscBinFFHandler : public fastfile::FFHandler {
	public:
		GscBinFFHandler() : fastfile::FFHandler("gscbin", "GSC Scripts GSCBIN based", true) {
		}

		void Handle(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& decReader, fastfile::FastFileContext& ctx) override {
			struct ScriptFile {
				uint64_t name;
				int32_t compressedLen;
				int32_t len;
				int32_t bytecodeLen;
				uintptr_t buffer;
				uintptr_t bytecode;
			};
			static_assert(sizeof(ScriptFile) == 0x28);

			ScriptFile sfSearch{};
			ScriptFile sfMask{};

			// use -2 to match both types
			sfSearch.buffer = 0xFFFFFFFFFFFFFFFE;
			sfMask.buffer = 0xFFFFFFFFFFFFFFFE;
			sfSearch.bytecode = 0xFFFFFFFFFFFFFFFE;
			sfMask.bytecode = 0xFFFFFFFFFFFFFFFE;

			std::filesystem::path outDir{ opt.m_output / "scriptfile" };

			size_t off{};
			while (true) {
				decReader.Goto(off);
				if (decReader.CanRead(8)) decReader.Skip(8);

				if ((off = decReader.FindMasked(&sfSearch, &sfMask, sizeof(sfSearch))) == std::string::npos) {
					break;
				}
				decReader.Goto(off);
				off++;
				ScriptFile& sf{ *decReader.ReadPtr<ScriptFile>() };

				if (sf.name < 0x1000000000 || sf.compressedLen <= 0 || sf.len <= 0 || sf.bytecodeLen <= 0 
					|| !decReader.CanRead((size_t)sf.compressedLen + sf.bytecodeLen)) {
					continue; // bad header
				}

				const char* sfname;
				if (sf.name == 0xFFFFFFFFFFFFFFFF) {
					// xstring?
					sfname = decReader.ReadString();
				}
				else {
					// xhash?
					sfname = utils::va("script_%llx", sf.name);
				}

				compatibility::xensik::gscbin::GscBinHeader header{};

				header.Magic() = compatibility::xensik::gscbin::GSCBIN_MAGIC;
				header.bytecodeLen = sf.bytecodeLen;
				header.compressedLen = sf.compressedLen;
				header.len = sf.len;
				byte* compressedData{ decReader.ReadPtr<byte>((size_t)sf.compressedLen) };
				byte* bytecodeData{ decReader.ReadPtr<byte>((size_t)sf.bytecodeLen) };

				if (*compressedData != 0x78) {
					continue; // the compressed data is zlib compressed, which is 99% of the time described by the magic 78 DA
				}

				std::filesystem::path outFile{ outDir / sfname };
				outFile.replace_extension(".gscbin");
				std::filesystem::create_directories(outFile.parent_path());
				{
					utils::OutFileCE os{ outFile, false, std::ios::binary };
					if (!os) {
						LOG_ERROR("Error when dumping {}", outFile.string());
						continue;
					}


					utils::WriteValue(os, &header, sizeof(header));
					utils::WriteValue(os, compressedData, sf.compressedLen);
					utils::WriteValue(os, bytecodeData, sf.bytecodeLen);
				}

				LOG_INFO("Dump {}", outFile.string());
					
			}
		}
	};

#ifndef CI_BUILD
	utils::ArrayAdder<GscBinFFHandler, fastfile::FFHandler> arr{ fastfile::GetHandlers() };
#endif // CI_BUILD
}