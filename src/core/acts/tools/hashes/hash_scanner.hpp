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

		HASH_BLACKOPS4 = HASH_FNVA,
		HASH_BLACKOPS4SCR = HASH_SCR_T89,
		HASH_BLACKOPS6 = HASH_FNVA | HASH_RES | HASH_DVAR | HASH_SCR_T10,
		HASH_BLACKOPS6_SP = HASH_FNVA | HASH_RES | HASH_DVAR | HASH_SCR_T10_SP,
		HASH_BLACKOPS7 = HASH_FNVA | HASH_RES | HASH_DVAR | HASH_SCR_T10 | HASH_OMNVAR,
		HASH_IW = HASH_FNVA | HASH_RES | HASH_DVAR | HASH_SCR_JUP,
		HASH_ALL = ~0ull,
		HASH_ALL32 = HASH_FNVA32 | HASH_PRIME | HASH_SCR_T89,
	};

	class HashFnv1a { public: static constexpr uint64_t Hash(const char* str, uint64_t base = 0xcbf29ce484222325LL) { return ::hash::Hash64A(str, base); } };
	class HashFnv1a32 { public: static constexpr uint64_t Hash(const char* str, uint64_t base = ::hash::FNV1A_32_PRIME) { return ::hash::HashX32(str, base); } };
	class HashPrime { public: static constexpr uint64_t Hash(const char* str, uint64_t base = 5381) { return ::hash::HashPrime(str, (uint32_t)base); } };
	class HashT10Scr { public: static constexpr uint64_t Hash(const char* str, uint64_t base = 0) { return ::hash::HashT10Scr(str, base); } };
	class HashT10OmnVar { public: static constexpr uint64_t Hash(const char* str, uint64_t base = 0) { return ::hash::HashT10OmnVar(str, base); } };
	class HashT10ScrSP {
		public:
		static constexpr uint64_t Hash(const char* str, uint64_t base = 0x1C2F2E3C8A257D07) { return ::hash::HashT10ScrSPPre(str, base); }
		static constexpr uint64_t HashPost(uint64_t base) { return ::hash::HashT10ScrSPPost(base); }
	};
	class HashT89Scr {
		public:
		static constexpr uint64_t Hash(const char* str, uint64_t base = 0x4B9ACE2F) { return ::hash::HashT89ScrPre(str, (uint32_t)base); }
		static constexpr uint64_t HashPost(uint64_t base) { return ::hash::HashT89ScrPost((uint32_t)base); }
	};
	class HashJupScr { public: static constexpr uint64_t Hash(const char* str, uint64_t base = 0x79D6530B0BB9B5D1) { return ::hash::HashJupScr(str, base); } };
	class HashIWAsset { public: static constexpr uint64_t Hash(const char* str, uint64_t base = 0x47F5817A5EF961BA) { return ::hash::HashIWAsset(str, base); } };
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
			if (!_strcmpi(n, "bo4scr")) {
				funcs = HASH_BLACKOPS4SCR;
			}
			else if (!_strcmpi(n, "bo4")) {
				funcs = HASH_BLACKOPS4;
			}
			else if (!_strcmpi(n, "bo6")) {
				funcs = HASH_BLACKOPS6;
			}
			else if (!_strcmpi(n, "bo6sp")) {
				funcs = HASH_BLACKOPS6_SP;
			}
			else if (!_strcmpi(n, "bo6scr") || !_strcmpi(n, "bo7scr")) {
				funcs = HASH_SCR_T10;
			}
			else if (!_strcmpi(n, "bo7")) {
				funcs = HASH_BLACKOPS7;
			}
			else if (!_strcmpi(n, "bo6spscr")) {
				funcs = HASH_SCR_T10_SP;
			}
			else if (!_strcmpi(n, "iwscr")) {
				funcs = HASH_SCR_JUP;
			}
			else if (!_strcmpi(n, "iwres")) {
				funcs = HASH_RES;
			}
			else if (!_strcmpi(n, "iwdvar")) {
				funcs = HASH_DVAR;
			}
			else if (!_strcmpi(n, "iwomnvar")) {
				funcs = HASH_OMNVAR;
			}
			else if (!_strcmpi(n, "bo6all")) {
				funcs = HASH_BLACKOPS6_SP | HASH_BLACKOPS6;
			}
			else if (!_strcmpi(n, "iw")) {
				funcs = HASH_IW;
			}
			else if (!_strcmpi(n, "FNVA")) {
				funcs = HASH_FNVA;
			}
			else if (!_strcmpi(n, "FNVA32")) {
				funcs = HASH_FNVA32;
			}
			else if (!_strcmpi(n, "prime")) {
				funcs = HASH_PRIME;
			}
			else if (!_strcmpi(n, "all")) {
				funcs = HASH_ALL;
			}
			else if (!_strcmpi(n, "all32")) {
				funcs = HASH_ALL32;
			}
			else {
				LOG_WARNING("Invalid name {}, use all hashes", n);
				funcs = HASH_ALL;
			}
		}
	};

	void ScanHashes(const std::vector<std::filesystem::path>& files, std::unordered_set<uint64_t>& hashes, uint64_t mask = ~0ull);
	std::vector<const char*> ReadDict(std::filesystem::path file, std::string& store);
	std::vector<std::filesystem::path> GetHashFiles(const std::filesystem::path& path);

}