#include <includes.hpp>
#include "actscli.hpp"
#include "compatibility/scobalula_wni.hpp"
#include <rapidcsv.h>

namespace {
	std::unordered_map<uint64_t, std::string> g_hashMap{};
	std::set<uint64_t> g_extracted{};
	const char* hashPrefix{};
	bool g_saveExtracted = false;
	bool show0 = false;
	bool markHash = false;
	bool heavyHashes = false;
}

namespace hashutils {

	const std::unordered_map<uint64_t, std::string>& GetMap() {
		return g_hashMap;
	}

	void ReadDefaultFile0() {
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
				hashutils::AddPrecomputed(hash, str);
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
						AddPrecomputed(std::strtoull(hash.c_str(), nullptr, 16), value.c_str());
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
			LoadMap(file, true, false);
		}
		if (!opt.noIWHash) {
			LoadMap(file, true, true);
		}
		LOG_DEBUG("End load default hash file");
	}

	void ReadDefaultFile(bool cleanup) {
		static std::mutex r{};
		static bool loaded{};
		std::lock_guard lg{ r };

		if (cleanup) {
			g_hashMap.clear();
			loaded = false;
		}
		if (!loaded) {
			ReadDefaultFile0();
			loaded = true;
		}
	}

	void SaveExtracted(bool value) {
		g_saveExtracted = value;
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
			auto e = g_hashMap.find(v);
			if (e != g_hashMap.end()) {
				out << std::hex << v << "," << e->second << "\n";
			}
			else {
				out << std::hex << v << ",hash_" << v << "\n";
			}
		}

		out.close();
		// clear and unset extract
		SaveExtracted(false);
		LOG_TRACE("End write extracted into {}", file);
	}

	int LoadMap(const char* file, bool ignoreCol, bool iw) {
		// add common hashes
		LOG_TRACE("Load hash file {}", file);

		// special value
		g_hashMap[0] = "";
		g_hashMap[Hash32("<error>")] = "<error>";
		g_hashMap[Hash32("self")] = "self";
		g_hashMap[Hash32("size")] = "size";
		g_hashMap[Hash32("nextarray")] = "nextarray";
		g_hashMap[Hash32("_")] = "_";

		// class special things
		Add("__constructor", true, iw);
		Add("__destructor", true, iw);
		Add("__vtable", true, iw);
		Add("_deleted", true, iw);

		// global vars
		Add("level", true, iw);
		Add("game", true, iw);
		Add("classes", true, iw);
		Add("mission", true, iw);
		Add("anim", true, iw);
		Add("world", true, iw);
		Add("sharedstructs", true, iw);
		Add("memory", true, iw);

		// structure basic hashes
		Add("system", true, iw);
		Add("scripts/core_common/system_shared.csc", true, iw);
		Add("scripts/core_common/system_shared.gsc", true, iw);
		Add("register", true, iw);
		Add("__init__system__", true, iw);
		Add("__init__", true, iw);
		Add("__main__", true, iw);
		Add("main", true, iw);
		Add("init", true, iw);
		// it seems all the varargs are called "vararg", but a flag is also describing, so idk
		Add("vararg", true, iw);

		// basic letter
		char buff[2] = { 0, 0 };
		for (char c = 'a'; c <= 'z'; c++) {
			*buff = c;
			Add(buff, true, iw);
		}
		for (char c = '0'; c <= '9'; c++) {
			*buff = c;
			Add(buff, true, iw);
		}

		// Decompiler special values
		Add("self", true, iw);
		Add("size", true, iw);

		// DDL names
		Add("root", true, iw); // root struct
		Add("__pad", true, iw); // padding

		// ADL names
		AddPrecomputed(Hash64("bool"), "bool");
		AddPrecomputed(Hash64("byte"), "byte");
		AddPrecomputed(Hash64("uint8"), "uint8");
		AddPrecomputed(Hash64("uint8_t"), "uint8_t");
		AddPrecomputed(Hash64("uint16"), "uint16");
		AddPrecomputed(Hash64("uint16_t"), "uint16_t");
		AddPrecomputed(Hash64("uint32"), "uint32");
		AddPrecomputed(Hash64("uint32_t"), "uint32_t");
		AddPrecomputed(Hash64("uint64"), "uint64");
		AddPrecomputed(Hash64("uint64_t"), "uint64_t");
		AddPrecomputed(Hash64("char"), "char");
		AddPrecomputed(Hash64("int8"), "int8");
		AddPrecomputed(Hash64("int8_t"), "int8_t");
		AddPrecomputed(Hash64("int16"), "int16");
		AddPrecomputed(Hash64("int16_t"), "int16_t");
		AddPrecomputed(Hash64("int32"), "int32");
		AddPrecomputed(Hash64("int32_t"), "int32_t");
		AddPrecomputed(Hash64("int64"), "int64");
		AddPrecomputed(Hash64("int64_t"), "int64_t");
		AddPrecomputed(Hash64("float"), "float");
		AddPrecomputed(Hash64("double"), "double");
		AddPrecomputed(Hash64("string"), "string");
		AddPrecomputed(Hash64("hash"), "hash");
		AddPrecomputed(Hash64("int"), "int");
		AddPrecomputed(Hash64("uint"), "uint");
		AddPrecomputed(Hash64("long"), "long");
		AddPrecomputed(Hash64("ulong"), "ulong");
		AddPrecomputed(Hash64("$$padding"), "$$padding");
		// Dump CF
		AddPrecomputed(Hash64("localize.json"), "localize.json");

		std::ifstream s(file);

		if (!s) {
			LOG_TRACE("End load hash file {}", file);
			return 0; // nothing to read
		}

		std::string line;
		int issues{};
		size_t count{};
		while (s.good() && std::getline(s, line)) {
			if (!Add(line.c_str(), ignoreCol, iw)) {
				issues++;
			}
			count++;
		}

		s.close();
		LOG_TRACE("End load hash file {} -> {}", file, count);
		return issues;
	}

	bool Add(const char* str, bool ignoreCol, bool iw) {
		g_hashMap.emplace(hashutils::Hash64(str), str);
		if (iw) {
			g_hashMap.emplace(hashutils::HashIWRes(str) & 0x7FFFFFFFFFFFFFFF, str);
			g_hashMap.emplace(hashutils::HashJupScr(str) & 0x7FFFFFFFFFFFFFFF, str);
			g_hashMap.emplace(hashutils::Hash64(str, 0x811C9DC5, 0x1000193) & 0xFFFFFFFF, str);
			g_hashMap.emplace(hashutils::HashIWDVar(str) & 0x7FFFFFFFFFFFFFFF, str);
			g_hashMap.emplace(hashutils::HashT10Scr(str) & 0x7FFFFFFFFFFFFFFF, str);
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
			g_hashMap.emplace(hashutils::HashT7(str), str);

			auto h = hashutils::Hash32(str);
			if (!ignoreCol) {
				auto find = g_hashMap.find(h);
				if (find != g_hashMap.end() && _strcmpi(str, find->second.data())) {
					LOG_WARNING("Coll '{}'='{}' #{:x}", str, find->second.data(), h);
					return false;
				}
			}
			g_hashMap.emplace(h, str);
		}
		return true;
	}
	void AddPrecomputed(uint64_t value, const char* str) {
		g_hashMap[value & 0x7FFFFFFFFFFFFFFF] = str;
	}

	bool Extract(const char* type, uint64_t hash, char* out, size_t outSize) {
		ReadDefaultFile();
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
		if (g_saveExtracted) {
			g_extracted.emplace(hash);
		}
		const auto res = g_hashMap.find(hash & 0x7FFFFFFFFFFFFFFF);
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

	char* ExtractTmp(const char* type, uint64_t hash) {
		static char buffer[10][0x600];
		static size_t bufferIndex = 0;
		ReadDefaultFile();
		bufferIndex = (bufferIndex + 1) % ARRAYSIZE(buffer);
		auto& buff = buffer[bufferIndex];
		Extract(type, hash, buff, sizeof(buff));
		return buff;
	}

	const char* ExtractPtr(uint64_t hash) {
		ReadDefaultFile();
		const auto res = g_hashMap.find(hash & 0x7FFFFFFFFFFFFFFF);
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