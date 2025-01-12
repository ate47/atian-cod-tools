#include <includes.hpp>
#include <core/bytebuffer.hpp>
#include <deps/oodle.hpp>
#include "pools.hpp"

namespace {
	int ffrbo3(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 1)) return tool::BAD_USAGE;
		std::vector<std::filesystem::path> files{};

		utils::GetFileRecurseExt(argv[2], files, ".ff\0");

		std::vector<byte> buff{};
		for (const std::filesystem::path& path : files) {

			if (!utils::ReadFile(path, buff)) {
				LOG_ERROR("Can't read {}", path.string());
				continue;
			}
			std::filesystem::path deco{ path };
			deco.replace_extension(".ff.dec");
			
			LOG_INFO("Reading {} into {}", path.string(), deco.string());

			core::bytebuffer::ByteBuffer reader{ buff };

			try {
				uint64_t magic{ reader.Read<uint64_t>() };

				if (magic != 0x3030303066664154) {
					throw std::runtime_error(std::format("bad magic 0x{:x}", magic));
				}


				uint64_t version{ reader.Read<uint32_t>() };

				if (version != 0x251 && version != 0x265) {
					throw std::runtime_error(std::format("bad version 0x{:x}", version));
				}

				reader.Skip<byte>();
				byte compression{ reader.Read<byte>() };
				byte plt{ reader.Read<byte>() };
				byte encrypted{ reader.Read<byte>() };

				deps::oodle::Oodle oodle{};

				if (compression != 1) {
					if (compression == 8) {
						if (!oodle.LoadOodle(deps::oodle::OO2CORE_7) || !oodle) {
							throw std::runtime_error("can't load oodle");
						}
					}
					else {
						throw std::runtime_error(std::format("bad compression {}", (int)compression));
					}
				}
				if (plt) {
					LOG_WARNING("non pc fastfile");
				}
				if (encrypted) {
					LOG_WARNING("encrypted fastfile");
				}

				// skip header
				if (version == 0x251) {
					reader.Goto(0x248);
				}
				else if (version == 0x265) {
					reader.Goto(0x638);
				}
				else {
					throw std::runtime_error("bad ver");
				}


				size_t consumed{};

				std::vector<byte> ffdata{};

				size_t blocks{};
				while (true) {
					int32_t compressedSize{ reader.Read<int32_t>() };
					int32_t decompressedSize{ reader.Read<int32_t>() };
					int32_t blockSize{ reader.Read<int32_t>() };
					int32_t blockPosition{ reader.Read<int32_t>() };

					LOG_TRACE("{:x} {:x} {:x} {:x}", compressedSize, decompressedSize, blockSize, blockPosition);

					if (!blockSize) break;

					if (blockPosition != reader.Loc() - 16) {
						throw std::runtime_error("bad block position");
					}


					if (!decompressedSize) {
						reader.Align(0x800000);
						continue;
					}

					if (!reader.CanRead(compressedSize)) {
						throw std::runtime_error("can't read buffer");
					}

					try {
						
						byte* decompressed{ &ffdata[utils::Allocate(ffdata, decompressedSize)]};

						if (compression == 1) {
							uLongf sizef = (uLongf)decompressedSize;
							uLongf sizef2{ (uLongf)compressedSize };
							int ret;
							if ((ret = uncompress2(decompressed, &sizef, reader.Ptr<const Bytef>(), &sizef2)) < 0) {
								throw std::runtime_error(std::format("error when decompressing: {}", zError(ret)));
							}

							LOG_TRACE("read 0x{:x}", compressedSize);
						}
						else if (compression == 8) {
							int ret{ oodle.Decompress(reader.Ptr<byte>(), compressedSize, decompressed, decompressedSize, deps::oodle::OODLE_FS_YES) };

							if (ret != decompressedSize) {
								throw std::runtime_error(std::format("Can't decompress block, returned size isn't the expected one: 0x{:x} != 0x{:x}", ret, decompressedSize));
							}

							LOG_TRACE("read 0x{:x}", compressedSize);
						}
						else {
							throw std::runtime_error(std::format("bad compress 0x{:x}", (int)compression));
						}

						blocks++;
					}
					catch (std::runtime_error& e) {
						throw std::runtime_error(std::format("Can't read decompressed data: {}", e.what()));
					}


					consumed += decompressedSize;
					reader.Skip(compressedSize);
					reader.Goto(blockPosition + 16 + blockSize);
				}

				LOG_INFO("Loaded 0x{:x} bytes in {} block(s)", ffdata.size(), blocks);
				core::bytebuffer::ByteBuffer decReader{ ffdata };

				int32_t stringsCount{ decReader.Read<int32_t>() };
				decReader.Goto(32);
				int32_t assetsCount{ decReader.Read<int32_t>() };

				LOG_INFO("{}, {}", stringsCount, assetsCount);

				decReader.Goto(56 + (stringsCount - 1) * 8);

				//utils::OutFileCE of{ deco };
				//if (!of) {
				//	LOG_ERROR("Can't open {}", deco.string());
				//	continue;
				//}
				std::ostream& of{ utils::NullStream() };


				of << "Strings:\n";
				
				for (size_t i = 0; i < stringsCount; i++) {
					const char* cc{ decReader.ReadString() };
					//of << cc << "\n";
				}


				struct XAsset_0 {
					int64_t header;
					bo3::pool::T7XAssetType type;
				};

				decReader.Align(8); // asset align
				XAsset_0* assets{ decReader.ReadPtr<XAsset_0>(assetsCount) };

				struct ScriptParseTree {
					int64_t name;
					int32_t len;
					int32_t pad;
					int64_t buffer;
				}; static_assert(sizeof(ScriptParseTree) == 0x18);

				for (size_t i = 0; i < assetsCount; i++) {
					XAsset_0& ass{ assets[i] };

					//LOG_INFO("Reading {} {}", ass.header, bo3::pool::T7XAssetName(ass.type));

					if (ass.type == bo3::pool::T7_ASSET_TYPE_SCRIPTPARSETREE) {
						/*
						decReader.Align<void*>();
						ScriptParseTree& spt{ *decReader.ReadPtr<ScriptParseTree>() };
						const char* name;
						if (spt.name) {
							name = decReader.ReadString();
						}
						else {
							name = "<none>";
						}
						of << name << std::dec << " " << spt.name << " " << spt.buffer << "\n";

						if (spt.buffer) {
							decReader.Align(0x20);
							byte* data{ decReader.ReadPtr<byte>(spt.len + 1) };

							of << *reinterpret_cast<uint64_t*>(data) << "\n";
						}
						*/
					}
					else {
						//LOG_ERROR("Missing type impl for {}", (int)ass.type);
						//break;
					}
				}
				{
					// dump spt dbg
					ScriptParseTree sptSearch{};
					ScriptParseTree sptMask{};
					memset(&sptMask, 0xff, sizeof(sptMask));
					sptMask.len = 0;
					sptSearch.buffer = 0xFFFFFFFFFFFFFFFF;
					sptSearch.name = 0xFFFFFFFFFFFFFFFF;

					std::filesystem::path outDir{ "scriptparsetree_t7ff" };

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


						LOG_INFO("spt {} -> 0x{:x}", name, spt.len);

						if (!decReader.CanRead(spt.len)) {
							continue; // bad size
						}
						byte* data{ decReader.ReadPtr<byte>(spt.len + 1) };

						if ((*(uint64_t*)data & 0xFFFFFFFFFFFFF) != 0x00a0d43534780) {
							LOG_ERROR("Bad gsc magic {:x}", *(uint64_t*)data);
							continue; // bad magic
						}

						std::filesystem::path outFile{ outDir / std::format("{:x}/{}c", *(uint64_t*)data, name) };
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

					std::filesystem::path outDir{ "scriptparsetree_t7ff" };

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
							LOG_ERROR("Bad gsc magic {:x}", *(uint64_t*)data);
							continue; // bad magic
						}
						{
							std::filesystem::path outFile{ outDir / std::format("{:x}/{}.gdbc", *(uint64_t*)data, name) };
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

							std::filesystem::path outFile{ outDir / std::format("{:x}/{}", *(uint64_t*)data, name) };
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
			catch (std::runtime_error& e) {
				LOG_ERROR("Can't read {}: {}", path.string(), e.what());
			}

		}

		return tool::OK;
	}


	ADD_TOOL(ffrbo3, "bo3", " (ff)", "read fast file (bo3)", ffrbo3);
}