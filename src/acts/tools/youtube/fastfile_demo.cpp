#include <includes.hpp>
#include <core/bytebuffer.hpp>
#include <utils/compress_utils.hpp>
#include <tools/fastfile/fastfile_handlers.hpp>
#include <tools/fastfile/fastfile_data_tre.hpp>

/*
 List:
 - header
 - chunks (schema)
 - chunks decompression
 - show chunk data
 - concept of blocks with linking (schema)
 - handlers and Load_XAssetHeader magic explain
 - GscXHash tool explain
 - bdiff (.fd, .fp)
 */

namespace {
	int ffdemo(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 1)) return tool::BAD_USAGE;

		std::vector<byte> buffer{ utils::ReadFile<std::vector<byte>>(argv[2]) };
		core::bytebuffer::ByteBuffer reader{ buffer };

//		uint64_t magic{ reader.Read<uint64_t>() };
//		LOG_INFO("0x{:x}", magic);


		fastfile::TXFileHeader* header{ reader.ReadPtr<fastfile::TXFileHeader>() };
		LOG_INFO("0x{:x}", header->version);
		// 0x251 -> bo3
		// 0x27f -> bo4
		utils::compress::CompressionAlgorithm alg;
		size_t fastFileSize;
		size_t decompressedSizeLoc;
		switch (header->version) {
		case 0x251:
			fastFileSize = 0x248;
			decompressedSizeLoc = 0x90;
			alg = utils::compress::CompressionAlgorithm::COMP_ZLIB;
			break;
		case 0x27f:
			fastFileSize = 0x840;
			decompressedSizeLoc = 0x490;
			alg = utils::compress::CompressionAlgorithm::COMP_OODLE;
			break;
		default:
			throw std::runtime_error("bad version");
		}
		reader.Goto(decompressedSizeLoc);

		uint64_t decompressedSize{ reader.Read<uint64_t>() };
		LOG_INFO("decompressedSize: 0x{:x}", decompressedSize);

		reader.Goto(fastFileSize);

		size_t id{};

		std::vector<byte> data{};

		while (reader.CanRead(sizeof(uint32_t) * 4)) {
			size_t loc{ reader.Loc() };
			uint32_t compressedSize{ reader.Read<uint32_t>() };
			uint32_t uncompressedSize{ reader.Read<uint32_t>() };
			uint32_t alignedSize{ reader.Read<uint32_t>() };
			uint32_t offset{ reader.Read<uint32_t>() };
			id++;
			if (uncompressedSize == 0) break;
			if (loc != offset) {
				throw std::runtime_error("bad loc");
			}

			//LOG_INFO("block#{} at 0x{:x} | 0x{:x} 0x{:x} 0x{:x} 0x{:x}", id, loc, compressedSize, uncompressedSize, alignedSize, offset);

			byte* alignedData{ reader.ReadPtr<byte>(alignedSize) };

			size_t off{ data.size() };
			data.resize(off + uncompressedSize);
			byte* decomp{ &data[off] };

			if (utils::compress::Decompress2(alg, decomp, uncompressedSize, alignedData, compressedSize) < 0) {
				throw std::runtime_error("decompression error");
			}
		}

		LOG_INFO("decompressed 0x{:x} bytes", data.size());

		std::filesystem::path outdec{ argv[2] };
		outdec.replace_extension(".ff.dec");
		utils::WriteFile(outdec, data);

		core::bytebuffer::ByteBuffer decr{ data };
		/*
		struct XAsset_0 {
			byte type;
			void* header;
		}; static_assert(sizeof(XAsset_0) == 0x10);


		struct XAssetList_0 {
			int stringsCount;
			const char** strings;
			uint32_t unk10;
			void* unk18;
			int assetCount;
			XAsset_0* assets;
		}; static_assert(sizeof(XAssetList_0) == 0x30);

		XAssetList_0* list{ decr.ReadPtr<XAssetList_0>() };

		LOG_INFO("{}", list->stringsCount);

		list->strings = decr.ReadPtr<const char*>(list->stringsCount);

		for (size_t i = 0; i < list->stringsCount; i++) {
			if (list->strings[i]) {
				list->strings[i] = decr.ReadString();
			}
			else {
				list->strings[i] = "";
			}

			//LOG_INFO("str {}", list->strings[i]);
		}

		LOG_INFO("{}", list->assetCount);

		list->assets = decr.ReadPtr<XAsset_0>(list->assetCount);

		for (size_t i = 0; i < list->assetCount; i++) {
			LOG_INFO("{} {}", (int)list->assets[i].type, list->assets[i].header);

			//Load_XAsset(false, &list->assets[i]);
			// 0xffffffffffffffff (-1) -> apr�s
			// 0xfffffffffffffffe (-2) -> apr�s + ins�rer un pointer
			// reste -> lire pointer - 1
			// 0 = nullptr
			//LOG_INFO("str {}", list->strings[i]);
		}
		*/

		uint64_t magic{ 0x00000a0d43534700 };
		uint64_t magicMask{ 0x00FFFFFFFFFFFF00 };

		for (size_t off : decr.FindAllMasked(&magic, &magicMask, sizeof(magic))) {
			LOG_INFO("{:x}", off);

			decr.Goto(off);


		}
		return tool::OK;
	}

	int ffdemo2(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 1)) return tool::BAD_USAGE;

		std::vector<byte> buffer{ utils::ReadFile<std::vector<byte>>(argv[2]) };
		core::bytebuffer::ByteBuffer decr{ buffer };

		struct ScriptParseTree {
			uint64_t name;
			uint64_t pad;
			void* buffer;
			int32_t len;
		};
		uint64_t magic{ 0x00000a0d43534700 };
		uint64_t magicMask{ 0x00FFFFFFFFFFFF00 };

		for (size_t off : decr.FindAllMasked(&magic, &magicMask, sizeof(magic))) {
			LOG_INFO("{:x}", off);

			decr.Goto(off - sizeof(ScriptParseTree));

			ScriptParseTree* spt{ decr.ReadPtr<ScriptParseTree>() };
			byte* data{ decr.ReadPtr<byte>(spt->len) };

			utils::WriteFile(std::format("script_{:x}.gscc", spt->name), data, spt->len);
		}



		return tool::OK;

	}


#ifndef CI_BUILD

	ADD_TOOL(ffdemo, "dev", "", "", ffdemo);
	ADD_TOOL(ffdemo2, "dev", "", "", ffdemo2);

#endif
}