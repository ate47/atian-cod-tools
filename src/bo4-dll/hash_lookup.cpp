#include <dll_includes.hpp>

static std::unordered_map<UINT64, std::string> g_lookupMap{};
static CHAR g_tempBuffer[3][2000];


bool Add(LPCCH str) {
	g_lookupMap.emplace(hash::Hash64(str), str);
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
		auto h = hash::Hash32(str);
		auto find = g_lookupMap.find(h);
		if (find != g_lookupMap.end() && _strcmpi(str, find->second.data())) {
			LOG_WARNING("Coll '{}' = '{}' #{:x}", str, find->second.data(), h);
			return false;
		}
		g_lookupMap.emplace(h, str);
	}
	return true;
}

void hash_lookup::LoadFile(LPCCH file) {
	std::ifstream s(file);

	if (!s) {
		return; // nothing to read
	}

	std::string line;
	int issues = 0;
	while (s.good() && std::getline(s, line)) {
		if (!Add(line.c_str())) {
			issues++;
		}
	}

	s.close();
	if (issues) {
		LOG_WARNING("Loaded lookup map with {}", issues);
	}
}

LPCCH hash_lookup::Extract(UINT64 hash) {
	const auto res = g_lookupMap.find(hash);
	if (res == g_lookupMap.end()) {
		return NULL; // can't find
	}
	return res->second.data();
}

LPCCH hash_lookup::ExtractTmp(scriptinstance::ScriptInstance inst, UINT64 hash) {
	auto& buff = g_tempBuffer[inst];

	auto res = Extract(hash);
	if (res) {
		snprintf(buff, sizeof(buff), "%s", res);
	}
	else {
		snprintf(buff, sizeof(buff), "%llx", hash);
	}


	return buff;
}
