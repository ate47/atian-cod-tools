#include <dll_includes.hpp>
#include <hash.hpp>

static std::unordered_map<uint64_t, std::string> g_lookupMap{};
static char g_tempBuffer[3][2000];


bool Add(const char* str) {
	g_lookupMap.emplace(hash::Hash64(str), str);
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

void hash_lookup::LoadFile(const char* file) {
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

const char* hash_lookup::Extract(uint64_t hash) {
	const auto res = g_lookupMap.find(hash);
	if (res == g_lookupMap.end()) {
		return NULL; // can't find
	}
	return res->second.data();
}

const char* hash_lookup::ExtractTmp(scriptinstance::ScriptInstance inst, uint64_t hash, const char* prefix) {
	auto& buff = g_tempBuffer[inst];

	auto res = Extract(hash);
	if (res) {
		sprintf_s(buff, "%s", res);
	}
	else if (prefix) {
		sprintf_s(buff, "%s_%llx", prefix, hash);
	}
	else {
		sprintf_s(buff, "%llx", hash);
	}


	return buff;
}
