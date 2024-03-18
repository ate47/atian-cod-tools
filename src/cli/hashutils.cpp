#include <includes.hpp>
#include "actscli.hpp"
#include "compatibility/scobalula_wni.hpp"
namespace {
	std::unordered_map<UINT64, std::string> g_hashMap{};
	std::set<UINT64> g_extracted{};
	bool g_saveExtracted = false;
}

const std::unordered_map<UINT64, std::string>& hashutils::GetMap() {
	return g_hashMap;
}

void hashutils::ReadDefaultFile() {
	static std::once_flag f{};
	
	std::call_once(f, [] {
		auto& opt = actscli::options();
		const char* wniPackageIndex = compatibility::scobalula::wni::packageIndexDir;
		if (opt.wniFiles) {
			wniPackageIndex = opt.wniFiles;
		}

		if (!compatibility::scobalula::wni::ReadWNIFiles(wniPackageIndex, [](uint64_t hash, const char* str) {
				hashutils::AddPrecomputed(hash, str);
			})) {
			LOG_ERROR("Error when reading WNI files");
		};

		if (opt.noDefaultHash) {
			return;
		}
		const char* file = opt.defaultHashFile;

		if (!file) {
			file = DEFAULT_HASH_FILE;
		}
		LOG_DEBUG("Load default hash file");
		if (!actscli::options().noTreyarchHash) {
			LoadMap(file, true, false);
		}
		if (!opt.noIWHash) {
			LoadMap(file, true, true);
		}
		LOG_DEBUG("End load default hash file");
	});
}

void hashutils::SaveExtracted(bool value) {
	g_saveExtracted = value;
	g_extracted.clear();
}

void hashutils::WriteExtracted(LPCCH file) {
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
		assert(e != g_hashMap.end());
		out << std::hex << v << "," << e->second << "\n";
	}

	out.close();
	// clear and unset extract
	SaveExtracted(false);
	LOG_TRACE("End write extracted");
}

int hashutils::LoadMap(LPCCH file, bool ignoreCol, bool iw) {
	// add common hashes
	LOG_TRACE("Load hash file {}", file);

	// special value
	g_hashMap[0] = "";
	g_hashMap[Hash32("<error>")] = "<error>";
	
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
	CHAR buff[2] = { 0, 0 };
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

	// DDL names
	Add("root", true, iw); // root struct
	Add("__pad", true, iw); // padding

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

bool hashutils::Add(LPCCH str, bool ignoreCol, bool iw) {
	g_hashMap.emplace(hashutils::Hash64(str), str);
	if (iw) {
		g_hashMap.emplace(hashutils::HashIW(str), str);
		g_hashMap.emplace(hashutils::HashIW2(str), str);
		return true;
	}
	bool cand32 = true;

	for (LPCCH s = str; *s; s++) {
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
void hashutils::AddPrecomputed(UINT64 value, LPCCH str) {
	g_hashMap[value] = str;
}

bool hashutils::Extract(LPCCH type, UINT64 hash, LPCH out, SIZE_T outSize) {
	ReadDefaultFile();
	const auto res = g_hashMap.find(hash & 0x7FFFFFFFFFFFFFFF);
	if (res == g_hashMap.end()) {
		snprintf(out, outSize, "%s_%llx", type, hash);
		return false;
	}
	snprintf(out, outSize, "%s", res->second.c_str());
	if (g_saveExtracted) {
		g_extracted.emplace(hash);
	}
	return true;
}

LPCH hashutils::ExtractTmp(LPCCH type, UINT64 hash) {
	static CHAR buffer[10][0x600];
	static size_t bufferIndex = 0;
	ReadDefaultFile();
	bufferIndex = (bufferIndex + 1) % ARRAYSIZE(buffer);
	auto& buff = buffer[bufferIndex];
	Extract(type, hash, buff, sizeof(buff));
	return buff;
}

LPCCH hashutils::ExtractPtr(UINT64 hash) {
	ReadDefaultFile();
	const auto res = g_hashMap.find(hash);
	if (res == g_hashMap.end()) {
		return NULL;
	}
	return res->second.data();
}

SIZE_T hashutils::Size() {
	ReadDefaultFile();
	return g_hashMap.size() >> 1; // 2 hashes/string
}
