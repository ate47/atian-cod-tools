#include <includes.hpp>
#include <utils/utils.hpp>
#include <core/async.hpp>
#include <rapidcsv.h>
#include "actscli.hpp"
#include "compatibility/scobalula_wni.hpp"

namespace {
	std::unordered_map<uint64_t, std::string> g_hashMap{};
	std::set<uint64_t> g_extracted{};
	const char* hashPrefix{};
	bool g_saveExtracted = false;
	bool g_saveExtractedUnk = false;
	bool show0 = false;
	bool markHash = false;
	bool heavyHashes = false;
	std::mutex asyncMutex{};
}

namespace hashutils {
	std::mutex* GetMutex(bool forceAsync) {
		if (forceAsync || !core::async::IsSync(core::async::AT_HASHES)) {
			return nullptr;
		}
		return &asyncMutex;
	}

	const std::unordered_map<uint64_t, std::string>& GetMap() {
		return g_hashMap;
	}

	static void ReadDefaultFile0() {
		auto& opt = actscli::options();
		std::filesystem::path wniPackageIndex;
		if (opt.wniFiles) {
			wniPackageIndex = opt.wniFiles;
		}
		else {
			wniPackageIndex = utils::GetProgDir() / compatibility::scobalula::wni::packageIndexDir;
		}

		if (opt.installDirHashes) {
			if (!compatibility::scobalula::wni::ReadWNIFiles(wniPackageIndex, [](uint64_t hash, const char* str) {
				AddPrecomputed(hash, str, true);
				})) {
				LOG_ERROR("Error when reading WNI files");
			};

			std::vector<std::filesystem::path> csvs{};

			utils::GetFileRecurse(wniPackageIndex, csvs, [](const std::filesystem::path& p) {
				auto s = p.string();
				return s.ends_with(".hash.csv");
				});

			for (const std::filesystem::path& csv : csvs) {
				LOG_DEBUG("Reading HASH CSV {}", csv.string());
				std::string buffer{};

				if (!utils::ReadFile(csv, buffer)) {
					LOG_WARNING("Can't read hash csv {}", csv.string());
					continue;
				}

				rapidcsv::Document doc{};

				std::stringstream stream{ buffer };

				doc.Load(stream, rapidcsv::LabelParams(-1, -1), rapidcsv::SeparatorParams(','));

				if (doc.GetColumnCount() < 2) {
					LOG_WARNING("Can't read hash csv {}: invalid file", csv.string());
					continue;
				}

				for (size_t i = 0; i < doc.GetRowCount(); i++) {
					const std::string hash = doc.GetCell<std::string>(0, i);
					const std::string value = doc.GetCell<std::string>(1, i);

					try {
						AddPrecomputed(std::strtoull(hash.c_str(), nullptr, 16), value.c_str(), true);
					}
					catch (std::runtime_error& e) {
						LOG_WARNING("Error when reading {}: invalid line {}: {}", csv.string(), i, e.what());
					}
				}

			}

		}

		show0 = opt.show0Hash;
		markHash = opt.markHash;
		hashPrefix = opt.hashPrefixByPass;
		heavyHashes = opt.heavyHashes;

		if (opt.noDefaultHash) {
			return;
		}
		const char* file = opt.defaultHashFile;

		if (!file) {
			file = DEFAULT_HASH_FILE;
		}
		std::filesystem::path filePath{ file };
		LOG_DEBUG("Load default hash file {}", filePath.string());
		if (!actscli::options().noTreyarchHash) {
			LoadMap(file, true, false, true);
		}
		if (!opt.noIWHash) {
			LoadMap(file, true, true, true);
		}
		LOG_DEBUG("End load default hash file");
	}

	void ReadDefaultFile(bool cleanup) {
		static bool loaded{};
		std::lock_guard lg{ asyncMutex };

		if (cleanup) {
			g_hashMap.clear();
			loaded = false;
		}
		if (!loaded) {
			ReadDefaultFile0();
			loaded = true;
		}
	}

	void SaveExtracted(bool value, bool unk) {
		g_saveExtracted = value;
		g_saveExtractedUnk = unk;
		g_extracted.clear();
	}

	void WriteExtracted(const char* file) {
		if (!g_saveExtracted || !file) {
			return; // nothing to do
		}
		LOG_TRACE("Start write extracted");
		std::ofstream out{ file };

		if (!out) {
			LOG_ERROR("Can't open extracted file {}", file);
			return;
		}

		for (const auto& v : g_extracted) {
			auto e = g_hashMap.find(v & hashutils::MASK63);
			if (e != g_hashMap.end()) {
				out << std::hex << v << "," << e->second << "\n";
			}
			else {
				out << std::hex << v << ",hash_" << v << "\n";
			}
		}

		out.close();
		// clear and unset extract
		SaveExtracted(false, false);
		LOG_TRACE("End write extracted into {}", file);
	}

	int LoadMap(const char* file, bool ignoreCol, bool iw, bool async) {
		core::async::opt_lock_guard lg{ GetMutex(async) };
		// add common hashes
		LOG_TRACE("Load hash file {}", file);

		// special value
		AddPrecomputed(0, "", true);
		AddPrecomputed(hash::HashT89Scr("<error>"), "<error>", true);
		AddPrecomputed(hash::HashT89Scr("self"), "self", true);
		AddPrecomputed(hash::HashT89Scr("size"), "size", true);
		AddPrecomputed(hash::HashT89Scr("nextarray"), "nextarray", true);
		AddPrecomputed(hash::HashT89Scr("_"), "_", true);

		// class special things
		Add("__constructor", true, iw, true);
		Add("__destructor", true, iw, true);
		Add("__vtable", true, iw, true);
		Add("_deleted", true, iw, true);

		// global vars
		Add("level", true, iw, true);
		Add("game", true, iw, true);
		Add("classes", true, iw, true);
		Add("mission", true, iw, true);
		Add("anim", true, iw, true);
		Add("world", true, iw, true);
		Add("sharedstructs", true, iw, true);
		Add("memory", true, iw, true);

		// structure basic hashes
		Add("system", true, iw, true);
		Add("scripts/core_common/system_shared.csc", true, iw, true);
		Add("scripts/core_common/system_shared.gsc", true, iw, true);
		Add("scripts/common/system.gsc", true, iw, true);
		Add("register", true, iw, true);
		Add("__init__system__", true, iw, true);
		Add("__init__", true, iw, true);
		Add("__main__", true, iw, true);
		Add("main", true, iw, true);
		Add("init", true, iw, true);
		// it seems all the varargs are called "vararg", but a flag is also describing, so idk
		// in t10 the vararg param is followed by varargcount
		Add("vararg", true, iw, true);
		Add("varargcount", true, iw, true);

		// basic letter
		char buff[2] = { 0, 0 };
		for (char c = 'a'; c <= 'z'; c++) {
			*buff = c;
			Add(buff, true, iw, true);
		}
		for (char c = '0'; c <= '9'; c++) {
			*buff = c;
			Add(buff, true, iw, true);
		}

		// nameless fields in compiler
		Add("$$tmp", true, iw, true);
		Add("$notif_checkum", true, iw, true);
		Add("$BAD_VAR", true, iw, true);
		for (size_t i = 0; i < 0x100; i++) {
			Add(utils::va("$nameless_%llx", i), true, iw, true);
			Add(utils::va("$$v%llx", i), true, iw, true);
		}

		// Decompiler special values
		Add("self", true, iw, true);
		Add("size", true, iw, true);

		// DDL names
		Add("root", true, iw, true); // root struct
		Add("__pad", true, iw, true); // padding

		// ADL names
		AddPrecomputed(hash::Hash64("bool"), "bool", true);
		AddPrecomputed(hash::Hash64("byte"), "byte", true);
		AddPrecomputed(hash::Hash64("uint8"), "uint8", true);
		AddPrecomputed(hash::Hash64("uint8_t"), "uint8_t", true);
		AddPrecomputed(hash::Hash64("uint16"), "uint16", true);
		AddPrecomputed(hash::Hash64("uint16_t"), "uint16_t", true);
		AddPrecomputed(hash::Hash64("uint32"), "uint32", true);
		AddPrecomputed(hash::Hash64("uint32_t"), "uint32_t", true);
		AddPrecomputed(hash::Hash64("uint64"), "uint64", true);
		AddPrecomputed(hash::Hash64("uint64_t"), "uint64_t", true);
		AddPrecomputed(hash::Hash64("char"), "char", true);
		AddPrecomputed(hash::Hash64("int8"), "int8", true);
		AddPrecomputed(hash::Hash64("int8_t"), "int8_t", true);
		AddPrecomputed(hash::Hash64("int16"), "int16", true);
		AddPrecomputed(hash::Hash64("int16_t"), "int16_t", true);
		AddPrecomputed(hash::Hash64("int32"), "int32", true);
		AddPrecomputed(hash::Hash64("int32_t"), "int32_t", true);
		AddPrecomputed(hash::Hash64("int64"), "int64", true);
		AddPrecomputed(hash::Hash64("int64_t"), "int64_t", true);
		AddPrecomputed(hash::Hash64("float"), "float", true);
		AddPrecomputed(hash::Hash64("double"), "double", true);
		AddPrecomputed(hash::Hash64("string"), "string", true);
		AddPrecomputed(hash::Hash64("hash"), "hash", true);
		AddPrecomputed(hash::Hash64("int"), "int", true);
		AddPrecomputed(hash::Hash64("uint"), "uint", true);
		AddPrecomputed(hash::Hash64("long"), "long", true);
		AddPrecomputed(hash::Hash64("ulong"), "ulong", true);
		AddPrecomputed(hash::Hash64("$$padding"), "$$padding", true);
		// Dump CF
		AddPrecomputed(hash::Hash64("localize.json"), "localize.json", true);

		std::ifstream s(file);

		if (!s) {
			LOG_TRACE("End load hash file {}", file);
			return 0; // nothing to read
		}

		std::string line;
		int issues{};
		size_t count{};
		while (s.good() && std::getline(s, line)) {
			if (!Add(line.c_str(), ignoreCol, iw, true)) {
				issues++;
			}
			count++;
		}

		s.close();
		LOG_TRACE("End load hash file {} -> {}", file, count);
		return issues;
	}

	bool Add(const char* str, bool ignoreCol, bool iw, bool async) {
		core::async::opt_lock_guard lg{ GetMutex(async) };
		AddPrecomputed(hash::Hash64(str), str, true);
		if (iw) {
			AddPrecomputed(hash::HashIWRes(str), str, true);
			AddPrecomputed(hash::HashJupScr(str), str, true);
			AddPrecomputed(hash::Hash64(str, 0x811C9DC5, 0x1000193) & 0xFFFFFFFF, str, true);
			AddPrecomputed(hash::HashIWDVar(str), str, true);
			AddPrecomputed(hash::HashT10Scr(str), str, true);
			AddPrecomputed(hash::HashT10ScrSP(str), str, true);
			return true;
		}
		bool cand32 = true;

		for (const char* s = str; *s; s++) {
			auto c = *s;
			if (!(
				(c >= 'A' && c <= 'Z')
				|| (c >= 'a' && c <= 'z')
				|| (c >= '0' && c <= '9')
				|| c == '_')) {
				cand32 = false; // a hash32 can only match [a-z0-9A-Z_]* in this context
				break;
			}
		}

		if (cand32) {
			AddPrecomputed(hash::HashT7(str), str, true);

			auto h = hash::HashT89Scr(str);
			if (!ignoreCol) {
				auto find = g_hashMap.find(h);
				if (find != g_hashMap.end() && _strcmpi(str, find->second.data())) {
					LOG_WARNING("Coll '{}'='{}' #{:x}", str, find->second.data(), h);
					return false;
				}
			}
			AddPrecomputed(h, str, true);
		}
		return true;
	}
	void AddPrecomputed(uint64_t value, const char* str, bool async) {
		core::async::opt_lock_guard lg{ GetMutex(async) };
		g_hashMap.emplace(value & hashutils::MASK63, str);
	}

	bool Extract(const char* type, uint64_t hash, char* out, size_t outSize) {
		ReadDefaultFile();
		core::async::opt_lock_guard lg{ GetMutex(false) };
		if (hashPrefix) type = hashPrefix;
		if (!hash) {
			if (show0 || markHash) {
				snprintf(out, outSize, "%s_0", type);
			}
			else if (outSize) {
				*out = 0;
			}
			return true;
		}
		const auto res = g_hashMap.find(hash & hashutils::MASK63);
		if (g_saveExtracted) {
			if (g_saveExtractedUnk || res != g_hashMap.end()) {
				g_extracted.emplace(hash);
			}
		}
		if (res == g_hashMap.end()) {
			snprintf(out, outSize, heavyHashes ? "%s_%016llX" : "%s_%llx", type, hash);
			return false;
		}
		if (markHash) {
			snprintf(out, outSize, heavyHashes ? "<%016llX>%s" : "<%llx>%s", hash, res->second.c_str());
		}
		else {
			snprintf(out, outSize, "%s", res->second.c_str());
		}
		return true;
	}

	std::pair<bool, char*> ExtractTmpPair(const char* type, uint64_t hash) {
		static thread_local char buffer[10][0x600];
		static thread_local size_t bufferIndex = 0;
		ReadDefaultFile();
		bufferIndex = (bufferIndex + 1) % ARRAYSIZE(buffer);
		auto& buff = buffer[bufferIndex];
		return { Extract(type, hash, buff, sizeof(buff)), buff };
	}

	char* ExtractTmp(const char* type, uint64_t hash) {
		auto [ok, buff] = ExtractTmpPair(type, hash);
		return buff;
	}

	const char* ExtractPtr(uint64_t hash) {
		ReadDefaultFile();
		const auto res = g_hashMap.find(hash & hashutils::MASK63);
		if (res == g_hashMap.end()) {
			return NULL;
		}
		return res->second.data();
	}

	size_t Size() {
		ReadDefaultFile();
		return g_hashMap.size() >> 1; // 2 hashes/string
	}

}