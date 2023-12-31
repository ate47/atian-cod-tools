#include <includes.hpp>

static std::unordered_map<UINT64, std::string> g_hashMap{};
static CHAR g_buffer[2048];
static std::set<UINT64> g_extracted{};
static bool g_saveExtracted = false;

void hashutils::ReadDefaultFile(bool ignoreCol, bool iw) {
	static std::once_flag f{};
	
	std::call_once(f, [ignoreCol, iw] {
		LoadMap(DEFAULT_HASH_FILE, ignoreCol, iw);
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
	std::ofstream out{ file };

	if (!out) {
		std::cerr << "Can't open extracted file " << file << "\n";
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
}

int hashutils::LoadMap(LPCWCH file, bool ignoreCol, bool iw) {
	// add common hashes

	// special value
	g_hashMap[0] = "";
	
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
	Add("scripts/core_common/system_shared.csc", true,iw);
	Add("scripts/core_common/system_shared.gsc", true,iw);
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
	Add("<error>", true, iw);
	Add("self", true, iw);

	std::ifstream s(file);

	if (!s) {
		return 0; // nothing to read
	}

	std::string line;
	int issues = 0;
	while (s.good() && std::getline(s, line)) {
		if (!Add(line.c_str(), ignoreCol, iw)) {
			issues++;
		}
	}

	s.close();
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
				std::cerr << "Coll '" << str << "'='" << find->second.data() << "' #" << std::hex << h << "\n";
				return false;
			}
		}
		g_hashMap.emplace(h, str);
	}
	return true;
}

bool hashutils::Extract(LPCCH type, UINT64 hash, LPCH out, SIZE_T outSize) {
	ReadDefaultFile();
	const auto res = g_hashMap.find(hash);
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
	ReadDefaultFile();
	Extract(type, hash, g_buffer, 2048);
	return g_buffer;
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
