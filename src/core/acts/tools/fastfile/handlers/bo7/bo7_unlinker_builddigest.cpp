#include <includes.hpp>
#include <tools/fastfile/handlers/handler_game_bo7.hpp>

namespace {
	using namespace fastfile::handlers::bo7;
	struct BuildDigestData {
		uint64_t unk0;
		uint64_t unk8;
		const char* unk10;
		const char* unk18;
	};
	static_assert(sizeof(BuildDigestData) == 0x20);

	struct BuildDigestEntry {
		const char* key;
		BuildDigestData data;
	};
	static_assert(sizeof(BuildDigestEntry) == 0x28);

	struct BuildDigest {
		XHash64 name;
		uint64_t count;
		BuildDigestEntry* entries;
	};
	static_assert(sizeof(BuildDigest) == 0x18);
}