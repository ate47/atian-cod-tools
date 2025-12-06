#pragma once

#include <tools/ff/handlers/handler_game_bo4.hpp>

namespace fastfile::handlers::bo4::map {

	/*
		[ ] col_map (clipmap)
		[~] com_map
		[ ] cpu_occlusion_data
		[x] entitylist
		[ ] game_map
		[ ] gfx_map
		[ ] glasses
		[ ] lighting
		[ ] staticlevelfxlist
		[ ] terraingfx
		[x] triggerlist
	*/

	enum SpawnVarKeyType : uint16_t {
		SVKT_UNK_0,
		SVKT_UNK_1,
		SVKT_STRING = 2,
		SVKT_VECTOR = 3,
		SVKT_XHASH = 4,
		SVKT_FLOAT = 5,
		SVKT_INT = 6,
	};

	struct SpawnVarKeyValue {
		const char* key;
		union {
			const char* string;
			vec3_t vector;
			float flt;
			int32_t i32;
			XHash hash;
			byte __pad[0x10];
		} value;
		SpawnVarKeyType type;
		bool unk1a;
	};
	static_assert(sizeof(SpawnVarKeyValue) == 0x20);

	struct LinkerSpawnVar {
		uint32_t numSpawnVars;
		SpawnVarKeyValue* spawnVars;
		int32_t modelIndex;
		vec3_t origin;
		vec3_t angles;
	};
	static_assert(sizeof(LinkerSpawnVar) == 0x30);


	const char* SpawnVarKeyTypeName(SpawnVarKeyType type);
}