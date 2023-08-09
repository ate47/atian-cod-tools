#include <includes.hpp>

static std::unordered_map<INT64, std::string> g_hashMap;
static CHAR g_buffer[2048];

void hashutils::ReadDefaultFile() {
	static std::once_flag f{};

	std::call_once(f, [] {
		LoadMap(L"strings.txt");
	});
}

void hashutils::LoadMap(LPCWCH file) {
	// add common hashes
	
	// self and vararg
	Add("self");
	Add("vararg");
	
	// call special functions
	Add("__constructor");
	Add("__destructor");

	// global vars
	Add("<error>");
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
	return true;
}

LPCCH hashutils::ExtractTmp(LPCCH type, UINT64 hash) {
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

UINT64 hashutils::Hash64(LPCCH str) {
	UINT64 hash = 0xcbf29ce484222325LL;

	for (LPCCH data = str; *data; data++) {
		hash = hash ^ tolower(*data);
		hash *= 0x100000001b3;
	}

	return hash & 0x7FFFFFFFFFFFFFFF;
}

