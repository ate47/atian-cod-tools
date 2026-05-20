#pragma once

namespace tool::hash::scanner {
	enum VmHashes : uint64_t {
		HASH_FNVA = 1,
		HASH_RES = 1ull << 1,
		HASH_DVAR = 1ull << 2,
		HASH_SCR_JUP = 1ull << 3,
		HASH_SCR_T10 = 1ull << 4,
		HASH_SCR_T10_SP = 1ull << 5,
		HASH_SCR_T89 = 1ull << 6,
		HASH_FNVA32 = 1ull << 7,
		HASH_PRIME = 1ull << 8,
		HASH_OMNVAR = 1ull << 9,
		HASH_DJB2 = 1ull << 10,
		HASH_T7 = 1ull << 11,
		HASH_KVP = 1ull << 12,

		HASH_BLACKOPS3 = HASH_T7,
		HASH_BLACKOPS4 = HASH_FNVA,
		HASH_BLACKOPS4SCR = HASH_SCR_T89,
		HASH_BLACKOPS6 = HASH_FNVA | HASH_RES | HASH_DVAR | HASH_SCR_T10,
		HASH_BLACKOPS6_SP = HASH_FNVA | HASH_RES | HASH_DVAR | HASH_SCR_T10_SP,
		HASH_BLACKOPS7 = HASH_FNVA | HASH_RES | HASH_DVAR | HASH_SCR_T10 | HASH_OMNVAR,
		HASH_IW = HASH_FNVA | HASH_RES | HASH_DVAR | HASH_SCR_JUP,
		HASH_ALL32 = HASH_FNVA32 | HASH_PRIME | HASH_SCR_T89 | HASH_DJB2 | HASH_T7 | HASH_KVP,
		HASH_ALL = ~HASH_ALL32,
	};
	uint64_t ReadVmHashes(const char* opt);

	class HashFnv1a { public: static constexpr uint64_t Hash(const char* str, uint64_t base = ::hash::FNV1A_PRIME) { return ::hash::Hash64A(str, base); } };
	class HashFnv1a32 { public: static constexpr uint64_t Hash(const char* str, uint64_t base = ::hash::FNV1A_32_PRIME) { return ::hash::HashX32(str, base); } };
	class HashPrime { public: static constexpr uint64_t Hash(const char* str, uint64_t base = 5381) { return ::hash::HashPrime(str, (uint32_t)base); } };
	class HashDJB2 { public: static constexpr uint64_t Hash(const char* str, uint64_t base = 0) { return ::hash::HashDJB2(str, (uint32_t)base); } };
	class HashT10Scr { public: static constexpr uint64_t Hash(const char* str, uint64_t base = 0) { return ::hash::HashT10Scr(str, base); } };
	class HashT10OmnVar { public: static constexpr uint64_t Hash(const char* str, uint64_t base = 0) { return ::hash::HashT10OmnVar(str, base); } };
	class HashT10ScrSP {
		public:
		static constexpr uint64_t Hash(const char* str, uint64_t base = ::hash::FNV1A_T10_SCR_OFFSET) { return ::hash::HashT10ScrSPPre(str, base); }
		static constexpr uint64_t HashPost(uint64_t base) { return ::hash::HashT10ScrSPPost(base); }
	};
	class HashT89Scr {
		public:
		static constexpr uint64_t Hash(const char* str, uint64_t base = ::hash::FNV1A_32_T7_PRIME) { return ::hash::HashT89ScrPre(str, (uint32_t)base); }
		static constexpr uint64_t HashPost(uint64_t base) { return ::hash::HashT89ScrPost((uint32_t)base); }
	};
	class HashT7 {
		public:
		static constexpr uint64_t Hash(const char* str, uint64_t base = ::hash::FNV1A_32_T7_PRIME) { return ::hash::HashT7Pre(str, (uint32_t)base); }
		static constexpr uint64_t HashPost(uint64_t base) { return ::hash::HashT7Post((uint32_t)base); }
	};
	class HashJupScr { public: static constexpr uint64_t Hash(const char* str, uint64_t base = ::hash::FNV1A_IW_SCR_PRIME) { return ::hash::HashJupScr(str, base); } };
	class HashIWAsset { public: static constexpr uint64_t Hash(const char* str, uint64_t base = ::hash::FNV1A_IW_ASSET_PRIME) { return ::hash::HashIWAsset(str, base); } };
	class HashIWDVar { public: static constexpr uint64_t Hash(const char* str, uint64_t base = 0) { return ::hash::HashIWDVar(str, base); } };

	class AbstractHashData {
	public:
		std::ofstream output;
		uint64_t funcs{};
		size_t found{};


		AbstractHashData(const char* file, const char* funcs) : output(file, std::ios::app) {
			ReadFuncs(funcs);
		}
		~AbstractHashData() {
			output.close();
		}

		constexpr bool UseFunc(VmHashes hash) const {
			return (funcs & hash) != 0;
		}

		void ReadFuncs(const char* n) {
			funcs = ReadVmHashes(n);
		}
	};

	void ScanHashes(const std::vector<std::filesystem::path>& files, std::unordered_set<uint64_t>& hashes, uint64_t mask = ~0ull);
	std::vector<const char*> ReadDict(std::filesystem::path file, std::string& store);
	std::vector<std::filesystem::path> GetHashFiles(const std::filesystem::path& path);

}