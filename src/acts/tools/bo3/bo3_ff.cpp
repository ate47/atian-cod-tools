#include <includes.hpp>
#include <core/bytebuffer.hpp>
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

				if (version != 0x251) {
					throw std::runtime_error(std::format("bad version 0x{:x}", version));
				}

				reader.Skip<byte>();
				byte compression{ reader.Read<byte>() };
				byte plt{ reader.Read<byte>() };
				byte encrypted{ reader.Read<byte>() };

				if (compression != 1) {
					throw std::runtime_error(std::format("bad compression {}", (int)compression));
				}
				if (plt) {
					throw std::runtime_error("non pc fastfile");
				}
				if (encrypted) {
					throw std::runtime_error("encrypted fastfile");
				}

				reader.Goto(144);
				size_t size{ reader.Read<size_t>() };
				LOG_INFO("size 0x{:x}", size);
				// skip header
				reader.Goto(584);


				size_t consumed{};

				std::vector<byte> ffdata{};

				size_t blocks{};
				while (consumed < size) {
					int32_t compressedSize{ reader.Read<int32_t>() };
					int32_t decompressedSize{ reader.Read<int32_t>() };
					int32_t blockSize{ reader.Read<int32_t>() };
					int32_t blockPosition{ reader.Read<int32_t>() };

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

						auto decompressed{ std::make_unique<byte[]>(decompressedSize) };

						uLongf sizef = (uLongf)decompressedSize;
						uLongf sizef2{ (uLongf)compressedSize };
						int ret;
						if (ret = uncompress2(decompressed.get(), &sizef, reader.Ptr<const Bytef>(), &sizef2) < 0) {
							throw std::runtime_error(std::format("error when decompressing {}", zError(ret)));
						}

						LOG_TRACE("read 0x{:x}", compressedSize);

						utils::WriteValue(ffdata, decompressed.get(), sizef);
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
					int64_t len;
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


					LOG_INFO("{} -> 0x{:x}", name, spt.len);
					
					byte* data{ decReader.ReadPtr<byte>(spt.len + 1) };

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
			catch (std::runtime_error& e) {
				LOG_ERROR("Can't read {}: {}", path.string(), e.what());
			}

		}

		return tool::OK;
	}


	ADD_TOOL(ffrbo3, "bo3", " (ff)", "read fast file (bo3)", ffrbo3);
}