#include <includes.hpp>
#include <core/memory_allocator.hpp>
#include <core/config.hpp>
#include <tools/tools_nui.hpp>
#include "hash_scanner.hpp"
#include <tools/hash.hpp>
#include "text_expand.hpp"
#include <regex>
#include <future>
#include <BS_thread_pool.hpp>
#include <actslib/profiler.hpp>

namespace tool::hash::scanner {

	void ScanHashes(const std::vector<std::filesystem::path>& files, std::unordered_set<uint64_t>& hashes) {
		static std::regex pattern{ "(hash|file|var|function|namespace|event|script|class|_id|x64)[:_]([0-9a-fA-F]{1,16})" };

		BS::thread_pool pool{};
		std::mutex mtx{};

		actslib::profiler::Profiler profiler{ "scan hashes" };

		profiler.PushSection("Scan");
		for (const std::filesystem::path& p : files) {
			pool.detach_task([p, &mtx, &hashes] {
				std::unordered_set<uint64_t> read{};
				std::string fns{ p.string() };
				auto nrbegin = std::sregex_iterator(fns.begin(), fns.end(), pattern);
				auto nrend = std::sregex_iterator();
				for (std::sregex_iterator it = nrbegin; it != nrend; ++it) {
					std::smatch match = *it;
					read.insert(std::stoull(match[2].str(), nullptr, 16));
				}

				std::string buffer{};
				if (!utils::ReadFile(p, buffer)) {
					LOG_ERROR("Can't read file {}.", p.string());
					return;
				}
				auto rbegin = std::sregex_iterator(buffer.begin(), buffer.end(), pattern);
				auto rend = std::sregex_iterator();
				for (std::sregex_iterator it = rbegin; it != rend; ++it) {
					std::smatch match = *it;

					read.insert(std::stoull(match[2].str(), nullptr, 16));
				}

				{
					std::lock_guard lg{ mtx };
					hashes.insert(read.begin(), read.end());
				}
			});
		}
		profiler.PopSection();
		profiler.PushSection("Wait");
		pool.wait();
		profiler.PopSection();
		if (core::logs::getlevel() == core::logs::LVL_TRACE) {
			profiler.WriteToStr(std::cout);
		}
	}

	std::vector<std::filesystem::path> GetHashFiles(const std::filesystem::path& path) {
		std::vector<std::filesystem::path> files{};

		LOG_TRACE("Load file(s)...");
		utils::GetFileRecurse(path, files, [](const std::filesystem::path& p) {
			std::string name = p.string();
			return
				name.ends_with(".gsc") || name.ends_with(".csc")
				|| name.ends_with(".csv") || name.ends_with(".tsv")
				|| name.ends_with(".dec.lua")
				|| name.ends_with(".txt")
				|| name.ends_with(".ddl") || name.ends_with(".ddldef")
				|| name.ends_with(".json");
			});

		return files;
	}
	namespace {

		int strscan(int argc, const char* argv[]) {
			if (tool::NotEnoughParam(argc, 2)) return tool::BAD_USAGE;
			std::vector<std::filesystem::path> files{ GetHashFiles(argv[2]) };
			LOG_TRACE("{} file(s) loaded...", files.size());

			static std::regex pattern{ "([0-9a-zA-Z_]+)" };

			utils::OutFileCE os{ argv[3], true };


			std::unordered_set<uint64_t> hashes{};

			for (const std::filesystem::path& path : files) {
				std::string str{ utils::ReadFile<std::string>(path) };
				auto nrbegin = std::sregex_iterator(str.begin(), str.end(), pattern);
				auto nrend = std::sregex_iterator();
				for (std::sregex_iterator it = nrbegin; it != nrend; ++it) {
					std::smatch match = *it;
					std::string sh{ match[1] };
					uint64_t ho;
					if (::hash::TryHashPattern(sh.data(), ho)) continue;
					uint64_t h{ ::hash::Hash64(sh) };

					if (hashes.contains(h)) {
						continue;
					}
					else {
						hashes.insert(h);
					}
					os << match[1] << "\n";
				}
			}

			LOG_INFO("dump into {}", argv[3]);

			return tool::OK;
		}

		int hashscan(Process& proc, int argc, const char* argv[]) {
			if (argc < 4) {
				return tool::BAD_USAGE;
			}
			std::vector<std::filesystem::path> files{ GetHashFiles(argv[2]) };
			LOG_TRACE("{} file(s) loaded...", files.size());

			std::unordered_set<uint64_t> hashes{};
			ScanHashes(files, hashes);

			LOG_INFO("Find {} hash(es)", hashes.size());

			std::ofstream output{ argv[3] };

			if (!output) {
				LOG_ERROR("Can't open {}", argv[3]);
				return tool::BASIC_ERROR;
			}

			for (uint64_t h : hashes) {
				output << "hash_" << std::hex << h << "\n";
			}

			output.close();

			LOG_INFO("Dump into {}", argv[3]);

			return tool::OK;
		}

		int scanlookup(Process& proc, int argc, const char* argv[]) {
			if (argc < 4) {
				return tool::BAD_USAGE;
			}
			std::vector<std::filesystem::path> files{ GetHashFiles(argv[2]) };
			LOG_TRACE("{} file(s) loaded...", files.size());

			std::unordered_set<uint64_t> hashes{};
			ScanHashes(files, hashes);

			LOG_INFO("Find {} hash(es)", hashes.size());

			std::ofstream output{ argv[3] };

			if (!output) {
				LOG_ERROR("Can't open {}", argv[3]);
				return tool::BASIC_ERROR;
			}

			hashutils::ReadDefaultFile();

			for (uint64_t h : hashes) {
				const char* ptr{ hashutils::ExtractPtr(h) };
				if (ptr) {
					output << std::hex << h << "," << ptr << "\n";
				}
			}

			output.close();

			LOG_INFO("Dump into {}", argv[3]);

			return tool::OK;
		}
		enum VmHashes : uint64_t {
			HASH_FNVA = 1,
			HASH_RES = 1ull << 1,
			HASH_DVAR = 1ull << 2,
			HASH_SCR_JUP = 1ull << 3,
			HASH_SCR_T10 = 1ull << 4,
			HASH_SCR_T10_SP = 1ull << 5,
			HASH_SCR_T89 = 1ull << 6,

			HASH_BLACKOPS4 = HASH_FNVA,
			HASH_BLACKOPS4SCR = HASH_SCR_T89,
			HASH_BLACKOPS6 = HASH_FNVA | HASH_RES | HASH_DVAR | HASH_SCR_T10,
			HASH_BLACKOPS6_SP = HASH_FNVA | HASH_RES | HASH_DVAR | HASH_SCR_T10_SP,
			HASH_IW = HASH_FNVA | HASH_RES | HASH_DVAR | HASH_SCR_JUP,
			HASH_ALL = ~0ull,
		};


		class HashData {
		public:
			std::ofstream output;
			std::unordered_set<uint64_t> hashes{};
			std::mutex mtx{};
			uint64_t funcs{};
			const char* prefix{};
			const char* suffix{};
			const char* dict{ hash::text_expand::cdict };
			size_t dictSize{ sizeof(hash::text_expand::cdict) - 1};

			HashData(const char* file) : output(file, std::ios::app) {}
			~HashData() {
				output.close();
			}

			inline void TestHash(uint64_t v, const char* str) {
				auto it = hashes.find(v & hashutils::MASK60);
				if (it != hashes.end()) {
					std::lock_guard lg{ mtx };
					output << std::hex << v << "," << (prefix ? prefix : "") << str << (suffix ? suffix : "") << std::endl;
					LOG_INFO("{:x},{}{}{}", v, (prefix ? prefix : ""), str, (suffix ? suffix : ""));
				}
			}
			constexpr bool UseFunc(VmHashes hash) {
				return (funcs & hash) != 0;
			}
		};

		int hashbrute(Process& proc, int argc, const char* argv[]) {
			if (argc < 5) {
				return tool::BAD_USAGE;
			}

			HashData data{ argv[3] };
			const char* n{ argv[4] };
			if (argc >= 6 && argv[5][0]) data.prefix = argv[5];
			if (argc >= 7 && argv[6][0]) data.suffix = argv[6];
			if (argc >= 8 && argv[7][0]) {
				data.dict = argv[7];
				data.dictSize = std::strlen(data.dict);
			}

			if (!_strcmpi(n, "bo4")) {
				data.funcs = HASH_BLACKOPS4;
			}
			else if (!_strcmpi(n, "bo4scr")) {
				data.funcs = HASH_BLACKOPS4SCR;
			}
			else if (!_strcmpi(n, "bo6")) {
				data.funcs = HASH_BLACKOPS6;
			}
			else if (!_strcmpi(n, "bo6sp")) {
				data.funcs = HASH_BLACKOPS6_SP;
			}
			else if (!_strcmpi(n, "bo6all")) {
				data.funcs = HASH_BLACKOPS6_SP | HASH_BLACKOPS6;
			}
			else if (!_strcmpi(n, "iw")) {
				data.funcs = HASH_IW;
			}
			else if (!_strcmpi(n, "iwres")) {
				data.funcs = HASH_RES;
			}
			else if (!_strcmpi(n, "iwdvar")) {
				data.funcs = HASH_DVAR;
			}
			else if (!_strcmpi(n, "fnva")) {
				data.funcs = HASH_FNVA;
			}
			else if (!_strcmpi(n, "all")) {
				data.funcs = HASH_ALL;
			}
			else {
				LOG_WARNING("Invalid name {}, use all hashes", n);
				data.funcs = HASH_ALL;
			}

			LOG_TRACE("Load file(s)...");
			std::vector<std::filesystem::path> files{ GetHashFiles(argv[2]) };
			LOG_TRACE("{} file(s) loaded...", files.size());

			std::unordered_set<uint64_t> hashesTmp{};
			ScanHashes(files, hashesTmp);


			for (uint64_t h : hashesTmp) {
				data.hashes.insert(h & hashutils::MASK60);
			}

			LOG_INFO("Find {} hash(es)", data.hashes.size());
			if (!data.prefix && !data.suffix) {
				tool::hash::text_expand::GetDynamicAsync<HashData>(~0, [](const char* str, HashData* data) {
					if (data->UseFunc(HASH_FNVA)) data->TestHash(::hash::Hash64A(str), str);
					if (data->UseFunc(HASH_SCR_T10)) data->TestHash(::hash::HashT10Scr(str), str);
					if (data->UseFunc(HASH_SCR_T10_SP)) data->TestHash(::hash::HashT10ScrSP(str), str);
					if (data->UseFunc(HASH_SCR_T89)) data->TestHash(::hash::HashT89Scr(str), str);
					if (data->UseFunc(HASH_SCR_JUP)) data->TestHash(::hash::HashJupScr(str), str);
					if (data->UseFunc(HASH_RES)) data->TestHash(::hash::HashIWAsset(str), str);
					if (data->UseFunc(HASH_DVAR)) data->TestHash(::hash::HashIWDVar(str), str);
				}, &data, data.dict, data.dictSize);
			}
			else if (data.prefix) {
				if (data.suffix) {
					// prefix + suffix
					tool::hash::text_expand::GetDynamicAsync<HashData>(~0, [](const char* str, HashData* data) {
						if (data->UseFunc(HASH_FNVA)) data->TestHash(::hash::Hash64A(data->suffix, ::hash::Hash64A(str, ::hash::Hash64A(data->prefix))), str);
						if (data->UseFunc(HASH_SCR_T10)) data->TestHash(::hash::HashT10Scr(data->suffix, ::hash::HashT10Scr(str, ::hash::HashT10Scr(data->prefix))), str);
						if (data->UseFunc(HASH_SCR_T10_SP)) data->TestHash(::hash::HashT10ScrSPPost(::hash::HashT10ScrSPPre(data->suffix, ::hash::HashT10ScrSPPre(str, ::hash::HashT10ScrSPPre(data->prefix)))), str);
						if (data->UseFunc(HASH_SCR_T89)) data->TestHash(::hash::HashT89ScrPost(::hash::HashT89ScrPre(data->suffix, ::hash::HashT89ScrPre(str, ::hash::HashT89ScrPre(data->prefix)))), str);
						if (data->UseFunc(HASH_SCR_JUP)) data->TestHash(::hash::HashJupScr(data->suffix, ::hash::HashJupScr(str, ::hash::HashJupScr(data->prefix))), str);
						if (data->UseFunc(HASH_RES)) data->TestHash(::hash::HashIWAsset(data->suffix, ::hash::HashIWAsset(str, ::hash::HashIWAsset(data->prefix))), str);
						if (data->UseFunc(HASH_DVAR)) data->TestHash(::hash::HashIWDVar(data->suffix, ::hash::HashIWDVar(str, ::hash::HashIWDVar(data->prefix))), str);
					}, &data, data.dict, data.dictSize);
				}
				else {
					// prefix only
					tool::hash::text_expand::GetDynamicAsync<HashData>(~0, [](const char* str, HashData* data) {
						if (data->UseFunc(HASH_FNVA)) data->TestHash(::hash::Hash64A(str, ::hash::Hash64A(data->prefix)), str);
						if (data->UseFunc(HASH_SCR_T10)) data->TestHash(::hash::HashT10Scr(str, ::hash::HashT10Scr(data->prefix)), str);
						if (data->UseFunc(HASH_SCR_T10_SP)) data->TestHash(::hash::HashT10ScrSPPost(::hash::HashT10ScrSPPre(str, ::hash::HashT10ScrSPPre(data->prefix))), str);
						if (data->UseFunc(HASH_SCR_T89)) data->TestHash(::hash::HashT89ScrPost(::hash::HashT89ScrPre(str, ::hash::HashT89ScrPre(data->prefix))), str);
						if (data->UseFunc(HASH_SCR_JUP)) data->TestHash(::hash::HashJupScr(str, ::hash::HashJupScr(data->prefix)), str);
						if (data->UseFunc(HASH_RES)) data->TestHash(::hash::HashIWAsset(str, ::hash::HashIWAsset(data->prefix)), str);
						if (data->UseFunc(HASH_DVAR)) data->TestHash(::hash::HashIWDVar(str, ::hash::HashIWDVar(data->prefix)), str);
					}, &data, data.dict, data.dictSize);
				}
			}
			else {
				// suffix only
				tool::hash::text_expand::GetDynamicAsync<HashData>(~0, [](const char* str, HashData* data) {
					if (data->UseFunc(HASH_FNVA)) data->TestHash(::hash::Hash64A(data->suffix, ::hash::Hash64A(str)), str);
					if (data->UseFunc(HASH_SCR_T10)) data->TestHash(::hash::HashT10Scr(data->suffix, ::hash::HashT10Scr(str)), str);
					if (data->UseFunc(HASH_SCR_T10_SP)) data->TestHash(::hash::HashT10ScrSPPost(::hash::HashT10ScrSPPre(data->suffix, ::hash::HashT10ScrSPPre(str))), str);
					if (data->UseFunc(HASH_SCR_T89)) data->TestHash(::hash::HashT89ScrPost(::hash::HashT89ScrPre(data->suffix, ::hash::HashT89ScrPre(str))), str);
					if (data->UseFunc(HASH_SCR_JUP)) data->TestHash(::hash::HashJupScr(data->suffix, ::hash::HashJupScr(str)), str);
					if (data->UseFunc(HASH_RES)) data->TestHash(::hash::HashIWAsset(data->suffix, ::hash::HashIWAsset(str)), str);
					if (data->UseFunc(HASH_DVAR)) data->TestHash(::hash::HashIWDVar(data->suffix, ::hash::HashIWDVar(str)), str);
				}, &data, data.dict, data.dictSize);
			}

			return tool::OK;
		}

		class HashDataDict {
		public:
			std::ofstream output;
			std::unordered_set<uint64_t> hashes{};
			std::mutex mtx{};
			uint64_t funcs{};
			const char* prefix{};
			const char* suffix{};
			const char* mid{ "_" };

			HashDataDict(const char* file) : output(file, std::ios::app) {}
			~HashDataDict() {
				output.close();
			}

			constexpr bool UseFunc(VmHashes hash) {
				return (funcs & hash) != 0;
			}

			template<typename Func, bool prefix, bool suffix, bool midc>
			inline uint64_t HashMultiple(const char** str) {
				if (!*str) {
					uint64_t base = 0;
					if constexpr (prefix) {
						base = Func::Hash(this->prefix);

					}
					if constexpr (suffix) {
						base = Func::Hash(this->suffix, base);
					}
					return base;
				}
				uint64_t base;
				if constexpr (prefix) {
					base = Func::Hash(*(str++), Func::Hash(this->prefix));
				}
				else {
					base = Func::Hash(*(str++));
				}


				while (*(str)) {
					base = Func::Hash(mid, base);
					base = Func::Hash(*(str++), base);
				}

				if constexpr (suffix) {
					base = Func::Hash(this->suffix, base);
				}

				return base;
			}

			template<typename Func, bool prefix, bool suffix, bool midc, bool prePost = false>
			inline void TestHash(VmHashes func, const char** str) {
				if (!UseFunc(func)) {
					return;
				}
				uint64_t v = HashMultiple<Func, prefix, suffix, midc>(str);
				if constexpr (prePost) {
					v = Func::HashPost(v);
				}
				auto it = hashes.find(v & hashutils::MASK60);
				if (it != hashes.end()) {
					std::ostringstream oss{};
					if constexpr (prefix) {
						oss << this->prefix;
					}
					if (*str) {
						oss << *str;
						while (*(++str)) {
							if constexpr (midc) oss << mid;
							oss << *str;
						}
					}
					if constexpr (suffix) {
						oss << this->suffix;
					}
					std::string strs{ oss.str() };

					{
						std::lock_guard lg{ mtx };

						output << std::hex << v << "," << strs << std::endl;
						LOG_INFO("{:x},{}", v, strs);
					}
				}
			}

			template<bool prefix, bool suffix, bool midc>
			inline void TestHashes(const char** str) {
				class HashFnv1a { public: static constexpr uint64_t Hash(const char* str, uint64_t base = 0xcbf29ce484222325LL) { return ::hash::Hash64A(str, base); } };
				class HashT10Scr { public: static constexpr uint64_t Hash(const char* str, uint64_t base = 0) { return ::hash::HashT10Scr(str, base); } };
				class HashT10ScrSP { public:
					static constexpr uint64_t Hash(const char* str, uint64_t base = 0x1C2F2E3C8A257D07) { return ::hash::HashT10ScrSPPre(str, base); }
					static constexpr uint64_t HashPost(uint64_t base) { return ::hash::HashT10ScrSPPost(base); }
				};
				class HashT89Scr { public:
					static constexpr uint64_t Hash(const char* str, uint64_t base = 0x4B9ACE2F) { return ::hash::HashT89ScrPre(str, (uint32_t)base); }
					static constexpr uint64_t HashPost(uint64_t base) { return ::hash::HashT89ScrPost((uint32_t)base); }
				};
				class HashJupScr { public: static constexpr uint64_t Hash(const char* str, uint64_t base = 0x79D6530B0BB9B5D1) { return ::hash::HashJupScr(str, base); } };
				class HashIWAsset { public: static constexpr uint64_t Hash(const char* str, uint64_t base = 0x47F5817A5EF961BA) { return ::hash::HashIWAsset(str, base); } };
				class HashIWDVar { public: static constexpr uint64_t Hash(const char* str, uint64_t base = 0) { return ::hash::HashIWDVar(str, base); } };
				TestHash<HashFnv1a, prefix, suffix, midc>(HASH_FNVA, str);
				TestHash<HashT10Scr, prefix, suffix, midc>(HASH_SCR_T10, str);
				TestHash<HashT10ScrSP, prefix, suffix, midc, true>(HASH_SCR_T10_SP, str);
				TestHash<HashT89Scr, prefix, suffix, midc, true>(HASH_SCR_T89, str);
				TestHash<HashJupScr, prefix, suffix, midc>(HASH_SCR_JUP, str);
				TestHash<HashIWAsset, prefix, suffix, midc>(HASH_RES, str);
				TestHash<HashIWDVar, prefix, suffix, midc>(HASH_DVAR, str);
			}
		};

		int hashbrutedict(Process& proc, int argc, const char* argv[]) {
			if (argc < 6) {
				return tool::BAD_USAGE;
			}

			HashDataDict data{ argv[3] };
			const char* n{ argv[4] };
			std::ifstream dictIs{ argv[5] };

			if (!dictIs) {
				LOG_ERROR("Can't open {}", argv[5]);
				return tool::BASIC_ERROR;
			}

			std::string line{};
			core::memory_allocator::MemoryAllocator alloc{};

			std::vector<const char*> dictVec{};

			while (dictIs.good() && std::getline(dictIs, line)) {
				dictVec.push_back((const char*)alloc.Alloc(line));
			}
			dictVec.push_back(nullptr);
			dictIs.close();
			if (argc >= 7 && argv[6][0]) {
				data.prefix = argv[6];
				LOG_INFO("prefix: {}", data.prefix);
			}
			if (argc >= 8 && argv[7][0]) {
				data.suffix = argv[7];
				LOG_INFO("suffix: {}", data.suffix);
			}
			if (argc >= 9) {
				data.mid = argv[8];
				LOG_INFO("middle: {}", data.mid);
			}
			size_t maxLen{ std::string::npos };
			if (argc >= 10) {
				size_t count = (size_t)utils::ParseFormatInt(argv[9]);
				if (count) {
					maxLen = 1;
					for (size_t i = 0; i < count; i++) {
						maxLen *= dictVec.size();
					}
					maxLen++;
				}
				LOG_INFO("count: {}", count);
			}

			if (!_strcmpi(n, "bo4scr")) {
				data.funcs = HASH_BLACKOPS4SCR;
			}
			else if (!_strcmpi(n, "bo4")) {
				data.funcs = HASH_BLACKOPS4;
			}
			else if (!_strcmpi(n, "bo6")) {
				data.funcs = HASH_BLACKOPS6;
			}
			else if (!_strcmpi(n, "bo6sp")) {
				data.funcs = HASH_BLACKOPS6_SP;
			}
			else if (!_strcmpi(n, "bo6scr")) {
				data.funcs = HASH_SCR_T10;
			}
			else if (!_strcmpi(n, "bo6spscr")) {
				data.funcs = HASH_SCR_T10_SP;
			}
			else if (!_strcmpi(n, "iwscr")) {
				data.funcs = HASH_SCR_JUP;
			}
			else if (!_strcmpi(n, "iwres")) {
				data.funcs = HASH_RES;
			}
			else if (!_strcmpi(n, "bo6all")) {
				data.funcs = HASH_BLACKOPS6_SP | HASH_BLACKOPS6;
			}
			else if (!_strcmpi(n, "iw")) {
				data.funcs = HASH_IW;
			}
			else if (!_strcmpi(n, "all")) {
				data.funcs = HASH_ALL;
			}
			else {
				LOG_WARNING("Invalid name {}, use all hashes", n);
				data.funcs = HASH_ALL;
			}

			LOG_TRACE("Load file(s) for {} (0x{:x})...", n, data.funcs);
			std::vector<std::filesystem::path> files{ GetHashFiles(argv[2]) };
			LOG_TRACE("{} file(s) loaded...", files.size());

			std::unordered_set<uint64_t> hashesTmp{};
			ScanHashes(files, hashesTmp);


			for (uint64_t h : hashesTmp) {
				data.hashes.insert(h & hashutils::MASK60);
			}

			LOG_INFO("Find {} hash(es)", data.hashes.size());

			if (data.prefix || data.suffix) {
				if (data.prefix && data.suffix) {
					if (*data.mid) {
						tool::hash::text_expand::GetDynamicAsyncDict<HashDataDict>(maxLen, [](const char** str, HashDataDict* data) {
							data->TestHashes<true, true, true>(str);
						}, dictVec.data(), &data);
					}
					else {
						tool::hash::text_expand::GetDynamicAsyncDict<HashDataDict>(maxLen, [](const char** str, HashDataDict* data) {
							data->TestHashes<true, true, false>(str);
						}, dictVec.data(), &data);
					}
				}
				else if (data.suffix) {
					if (*data.mid) {
						tool::hash::text_expand::GetDynamicAsyncDict<HashDataDict>(maxLen, [](const char** str, HashDataDict* data) {
							data->TestHashes<false, true, true>(str);
						}, dictVec.data(), &data);
					}
					else {
						tool::hash::text_expand::GetDynamicAsyncDict<HashDataDict>(maxLen, [](const char** str, HashDataDict* data) {
							data->TestHashes<false, true, false>(str);
						}, dictVec.data(), &data);
					}
				}
				else {
					if (*data.mid) {
						tool::hash::text_expand::GetDynamicAsyncDict<HashDataDict>(maxLen, [](const char** str, HashDataDict* data) {
							data->TestHashes<true, false, true>(str);
							}, dictVec.data(), &data);
					}
					else {
						tool::hash::text_expand::GetDynamicAsyncDict<HashDataDict>(maxLen, [](const char** str, HashDataDict* data) {
							data->TestHashes<true, false, false>(str);
						}, dictVec.data(), & data);
					}
				}
			}
			else {
				if (*data.mid) {
					tool::hash::text_expand::GetDynamicAsyncDict<HashDataDict>(maxLen, [](const char** str, HashDataDict* data) {
						data->TestHashes<false, false, true>(str);
					}, dictVec.data(), &data);
				}
				else {
					tool::hash::text_expand::GetDynamicAsyncDict<HashDataDict>(maxLen, [](const char** str, HashDataDict* data) {
						data->TestHashes<false, false, false>(str);
					}, dictVec.data(), &data);
				}
			}

			return tool::OK;
		}
		
		void hashscanner() {
			tool::nui::NuiUseDefaultWindow dw{};
			
			static char inPath[MAX_PATH + 1]{ 0 };
			static char outPath[MAX_PATH + 1]{ 0 };
			static char dictPath[MAX_PATH + 1]{ 0 };
			static char startStr[0x100]{ 0 };
			static char midStr[sizeof(startStr)]{ '_' };
			static char endStr[sizeof(startStr)]{ 0 };
			static int count{ 2 };
			static std::string guessOut{};
			static std::unordered_set<uint64_t> loadedHashes{};
			static std::once_flag of;
			static std::string notif{};

			std::call_once(of, [] {
				std::string inPathCfg = core::config::GetString("hash.scanner.input", "");
				std::string outPathCfg = core::config::GetString("hash.scanner.output", "");
				std::string dictPathCfg = core::config::GetString("hash.scanner.dict", "");
				count = (int)core::config::GetInteger("hash.scanner.count", count);

				if (count <= 0) count = 1;
				snprintf(outPath, sizeof(outPath), "%s", outPathCfg.c_str());
				snprintf(inPath, sizeof(inPath), "%s", inPathCfg.c_str());
				snprintf(dictPath, sizeof(dictPath), "%s", dictPathCfg.c_str());
			});
			::hash::HashAlg::SyncAlgCfg();

			bool c{};
			ImGui::SeparatorText("Config");
			if (ImGui::InputText("input file", inPath, sizeof(inPath))) {
				core::config::SetString("hash.scanner.input", inPath);
				c = true;
			}
			if (ImGui::Button("Open input file...")) {
				if (tool::nui::OpenFile(L"Input file", L"Any file\0*.*\0", inPath, sizeof(inPath), OFN_PATHMUSTEXIST)) {
					core::config::SetString("hash.scanner.input", inPath);
					c = true;
				}
			}
			ImGui::Separator();
			if (ImGui::InputText("Output file", outPath, sizeof(outPath))) {
				core::config::SetString("hash.scanner.output", outPath);
				c = true;
			}
			if (ImGui::Button("Open output file...")) {
				if (tool::nui::OpenFile(L"Output file", L"Csv file (.csv)\0*.csv\0All\0*.*\0", outPath, sizeof(outPath), OFN_PATHMUSTEXIST)) {
					core::config::SetString("hash.scanner.output", outPath);
					c = true;
				}
			}
			ImGui::Separator();
			if (ImGui::InputText("Dictionary file", dictPath, sizeof(dictPath))) {
				core::config::SetString("hash.scanner.dict", dictPath);
				c = true;
			}
			if (ImGui::Button("Open dictionary file...")) {
				if (tool::nui::OpenFile(L"Dictionary file", L"Text file (.txt)\0*.txt\0All\0*.*\0", dictPath, sizeof(dictPath), OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST)) {
					core::config::SetString("hash.scanner.dict", dictPath);
					c = true;
				}
			}
			ImGui::Separator();
			
			if (ImGui::Button("Load hashes")) {
				loadedHashes.clear();
				std::vector<std::filesystem::path> files{ tool::hash::scanner::GetHashFiles(inPath) };
				LOG_TRACE("{} file(s) loaded...", files.size());
				tool::hash::scanner::ScanHashes(files, loadedHashes);
			}

			if (loadedHashes.size()) {
				ImGui::Text("%llu hash(es) loaded", loadedHashes.size());

				ImGui::SeparatorText("Scan options");

				if (ImGui::BeginCombo("Selected hashes", nullptr, ImGuiComboFlags_NoPreview)) {
					for (::hash::HashAlg& alg : ::hash::HashAlg::algs) {
						if (ImGui::Checkbox(alg.desc, &alg.selected)) {
							core::config::SetBool(std::format("hash.alg.{}", alg.id), alg.selected);
							c = true;
						}
					}
					ImGui::EndCombo();
				}
				
				ImGui::InputText("Start", startStr, sizeof(startStr));
				ImGui::InputText("Middle", midStr, sizeof(midStr));
				ImGui::InputText("End", endStr, sizeof(endStr));
				if (ImGui::SliderInt("Count", &count, 1, 10, "%d")) {
					core::config::SetInteger("hash.scanner.count", count);
					c = true;
				}

				static BS::thread_pool pool{};
				static tool::hash::text_expand::AsyncStop asyncStop{};

				if (asyncStop.IsRunning()) {
					if (ImGui::Button("Stop")) {
						asyncStop.Stop();
					}
				} else if (asyncStop.IsStop()) {
					ImGui::Text("Stopping...");
				} else if (ImGui::Button("Start")) {
					notif = "";
					std::ifstream dictIs{ dictPath };

					if (!dictIs) {
						notif = std::format("Can't open {}", dictPath);
					} else {
						std::string line{};
						core::memory_allocator::MemoryAllocator alloc{};

						std::vector<const char*> dictVec{};

						while (dictIs.good() && std::getline(dictIs, line)) {
							dictVec.push_back((const char*)alloc.Alloc(line));
						}
						dictVec.push_back(nullptr);
						dictIs.close();

						size_t maxLen{};
						if (count) {
							maxLen = 1;
							for (size_t i = 0; i < count; i++) {
								maxLen *= dictVec.size();
							}
							maxLen++;
						} else {
							maxLen = std::string::npos;
						}
						tool::hash::text_expand::GetDynamicAsyncDict<void>(maxLen, [](const char** str, void* data) {
							for (::hash::HashAlg& alg : ::hash::HashAlg::algs) {
								static thread_local char buff[0x400];
								tool::hash::text_expand::MergeInto(buff, sizeof(buff), str, midStr);
								uint64_t h{ alg.hashFunc(buff) };

								// todo: check h
							}
						}, dictVec.data(), nullptr);
					}
				}


			} else {
				ImGui::Text("No hash loaded");
			}
			
			if (!notif.empty()) {
				ImGui::Separator();

				ImGui::Text("%s", notif.data());
			}
			
			if (c) tool::nui::SaveNextConfig();
		}


		ADD_TOOL(hashscan, "hash", " [dir] [output]", "scan hashes in a directory", nullptr, hashscan);
		ADD_TOOL(scanlookup, "hash", " [dir] [output]", "scan hashes in a directory with lookup", nullptr, scanlookup);
		ADD_TOOL(hashbrute, "hash", " [dir] [output] (prefix) (suffix)", "brute search hashes in a directory", nullptr, hashbrute);
		ADD_TOOL(hashbrutedict, "hash", " [dir] [output] [dict] (prefix) (suffix) (maxlen)", "brute search hashes in a directory with dictionary", nullptr, hashbrutedict);
		ADD_TOOL(strscan, "hash", " [dir] [output]", "brute search hashes in a directory with dictionary", strscan);
		ADD_TOOL_NUI(hashscanner, "Hash Brute Searcher", hashscanner);
	}


}