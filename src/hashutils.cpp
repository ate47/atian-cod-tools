#include <includes.hpp>

static std::unordered_map<UINT64, std::string> g_hashMap{};
static CHAR g_buffer[2048];
static std::set<UINT64> g_extracted{};
static bool g_saveExtracted = false;

void hashutils::ReadDefaultFile() {
	static std::once_flag f{};

	std::call_once(f, [] {
		LoadMap(L"strings.txt");
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

void hashutils::LoadMap(LPCWCH file) {
	// add common hashes
	
	// class special things
	Add("__constructor");
	Add("__destructor");
	Add("__vtable");
	Add("_deleted");

	// global vars
	Add("level");
	Add("game");
	Add("classes");
	Add("mission");
	Add("anim");
	Add("world");
	Add("memory");

	// structure basic hashes
	Add("system");
	Add("scripts/core_common/system_shared.csc");
	Add("scripts/core_common/system_shared.gsc");
	Add("register");
	Add("__init__system__");
	Add("__init__");
	Add("__main__");
	Add("main");
	Add("init");
	// it seems all the varargs are called "vararg", but a flag is also describing, so idk
	Add("vararg"); 

	// Decompiler special values
	Add("<error>");
	Add("self");

	std::ifstream s(file);

	if (!s) {
		return; // nothing to read
	}

	std::string line;
	while (s.good() && std::getline(s, line)) {
		Add(line.c_str());
	}

	s.close();
}

void hashutils::Add(LPCCH str) {
	g_hashMap.emplace(hashutils::Hash32(str), str);
	g_hashMap.emplace(hashutils::Hash64(str), str);
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


UINT32 hashutils::Hash32(LPCCH str) {
	uint32_t hash = 0x4B9ACE2F;

	for (LPCCH data = str; *data; data++) {
		char c = tolower(*data);
		hash = ((c + hash) ^ ((c + hash) << 10)) + (((c + hash) ^ ((c + hash) << 10)) >> 6);
	}

	return 0x8001 * ((9 * hash) ^ ((9 * hash) >> 11));
}

UINT64 hashutils::Hash64(LPCCH str, UINT64 start) {
	UINT64 hash = start;

	for (LPCCH data = str; *data; data++) {
		if (*data >= 'A' && *data <= 'Z') {
			// to lower
			hash = hash ^ (UINT8)(*data - 'A');
		}
		else if (*data == '\\') {
			// replace path char
			hash = hash ^ '/';
		}
		else {
			hash = hash ^ *data;
		}
		
		hash *= 0x100000001b3;
	}

	return hash & 0x7FFFFFFFFFFFFFFF;
}

UINT32 hashutils::Hash32Pattern(LPCCH str) {
	std::string_view v{ str };

	if (!v.rfind("var_", 0)) {
		return std::strtoul(&str[4], nullptr, 16);
	}
	if (!v.rfind("event_", 0)) {
		return std::strtoul(&str[6], nullptr, 16);
	}
	if (!v.rfind("function_", 0)) {
		return std::strtoul(&str[9], nullptr, 16);
	}
	if (!v.rfind("namespace_", 0)) {
		return std::strtoul(&str[10], nullptr, 16);
	}

	return Hash32(str);
}

UINT64 hashutils::Hash64Pattern(LPCCH str) {
	std::string_view v{ str };

	if (!v.rfind("string_", 0)) {
		return std::strtoull(&str[7], nullptr, 16);
	}

	if (!v.rfind("hash_", 0)) {
		return std::strtoull(&str[5], nullptr, 16);
	}

	return Hash64(str);
}