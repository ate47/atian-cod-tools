#pragma once
#include <tools/fastfile/fastfile_handlers.hpp>

namespace fastfile {
	const char* GetFastFileCompressionName(FastFileIWCompression comp);
	FastFileIWCompression GetFastFileIWCompression(const char* name);
	utils::compress::CompressionAlgorithm GetFastFileCompressionAlgorithm(FastFileIWCompression comp);

	namespace flexible {
		struct PFFIWUnk {
			uint64_t unk0;
			uint64_t unk8;
			uint64_t unk10;
		};
		struct PFFIWDeps {
			uint64_t fastfileHash;
			uint64_t unk8;
		};
		struct PFFIW_0xc95c6b9c {
			bool loaded;
			uint64_t unk8;
		};

		struct PFFIWChecksums {
			bool loaded;
			uint32_t checksum[4];
		};

		struct PFFIWBootInfoAssetLimit {
			uint64_t assetName;
			uint64_t limit;
		};

		struct PFFIWBootInfo {
			uint64_t projectName;
			uint64_t unk8;
			uint64_t unk10;
			uint64_t unk18;
			uint32_t count;
			PFFIWBootInfoAssetLimit assetLimits[1];
		};

		inline PFFIWDeps& GetIWDeps(FlexibleFastFileReader& reader) { return *reader.GetChunkVal<PFFIWDeps>(SectionType::ST_IW_DEPS, true); }
		inline PFFIWChecksums& GetIWChecksums(FlexibleFastFileReader& reader) { return *reader.GetChunkVal<PFFIWChecksums>(SectionType::ST_IW_FASTFILE_CHECKSUM, true); }
		inline PFFIW_0xc95c6b9c& GetIWUnkc95c6b9c(FlexibleFastFileReader& reader) { return *reader.GetChunkVal<PFFIW_0xc95c6b9c>(SectionType::ST_IW_UNK_0xc95c6b9c, true); }
	}
}