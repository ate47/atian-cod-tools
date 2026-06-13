#pragma once
#include <gsc/gsc_acts_debug.hpp>

namespace shared::gsc::acts_addons {
	constexpr uint64_t MAGIC = 0x0d0a42494124;

	constexpr byte CURRENT_VERSION = 0x10;

	enum ActsAddonsFeatures : byte {
		AAF_DETOUR = 0x10,
		AAF_CRC_OFFSET = 0x10,
	};
	enum ActsAddonsFlags : uint32_t {};

	using GSC_ACTS_DETOUR = shared::gsc::acts_debug::GSC_ACTS_DETOUR;

	struct GSC_ACTS_ADDONS {
		byte magic[sizeof(MAGIC)];
		byte version;
		uint32_t flags;
		uint32_t detour_offset{};
		uint32_t detour_count{};
		uint32_t crc_offset{};


		constexpr bool HasFeature(ActsAddonsFeatures feature) const {
			return version >= feature;
		}

		constexpr bool HasFlag(uint32_t flag) const {
			return (flags & flag) == flag;
		}

		const char* GetString(uint32_t address) const {
			return (const char*)(magic + address);
		}

		uint64_t GetMagic() const {
			return *(uint64_t*)magic;
		}
#define __ACTS_DEBUG_FUNC1(type, id, obj) \
		type* id(byte* base) const { return (type*)(base + obj); }

#define __ACTS_DEBUG_FUNC(type, id, obj, count) \
		__ACTS_DEBUG_FUNC1(type, id, obj) \
		type* id##End(byte* base) const { return id(base) + count; }

		__ACTS_DEBUG_FUNC(GSC_ACTS_DETOUR, GetDetours, detour_offset, detour_count)

#undef __ACTS_DEBUG_FUNC
#undef __ACTS_DEBUG_FUNC1
	};
}