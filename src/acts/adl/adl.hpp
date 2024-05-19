#pragma once

namespace acts::compiler::adl {
	typedef uint32_t ADLDataTypeId;

	enum ADLDataTypeFlags : ADLDataTypeId {
		ADF_UNKNOWN = 0,
		ADF_PRIMITIVE_UNKNOWN = 0u << 26,
		ADF_PRIMITIVE_INT = 1u << 26,
		ADF_PRIMITIVE_FLOAT = 2u << 26,
		ADF_PRIMITIVE_MASK = 3u << 26,
		ADF_UNSIGNED = 1u << 28,
		ADF_DEFAULT = 1u << 29,
		ADF_STRUCT = 2u << 29,
		ADF_ENUM = 3u << 29,
		ADF_TYPEDEF = 4u << 29,
		ADF_FLAG = 5u << 29,
		ADF_MASK = 7u << 29,
		ADF_ID = ~(ADF_MASK | ADF_UNSIGNED | ADF_PRIMITIVE_MASK),
	};
	static_assert((ADF_MASK & ADF_UNSIGNED) == 0);
	static_assert((ADF_MASK & ADF_PRIMITIVE_MASK) == 0);
	static_assert((ADF_UNSIGNED & ADF_PRIMITIVE_MASK) == 0);

	enum ADLDataTypeDefault : ADLDataTypeId {
		ADD_UNKNOWN = 0,
		ADD_PADDING = ADF_DEFAULT | 1,
		ADD_FLOAT = ADF_PRIMITIVE_FLOAT | ADF_DEFAULT | 2,
		ADD_DOUBLE = ADF_PRIMITIVE_FLOAT | ADF_DEFAULT | 3,
		ADD_STRING = ADF_DEFAULT | 4,
		ADD_HASH = ADF_DEFAULT | 5,
		ADD_INT8 = ADF_PRIMITIVE_INT | ADF_DEFAULT | 7,
		ADD_INT16 = ADF_PRIMITIVE_INT | ADF_DEFAULT | 8,
		ADD_INT32 = ADF_PRIMITIVE_INT | ADF_DEFAULT | 9,
		ADD_INT64 = ADF_PRIMITIVE_INT | ADF_DEFAULT | 10,
		ADD_UINT8 = ADF_PRIMITIVE_INT | ADF_DEFAULT | ADF_UNSIGNED | 11,
		ADD_UINT16 = ADF_PRIMITIVE_INT | ADF_DEFAULT | ADF_UNSIGNED | 12,
		ADD_UINT32 = ADF_PRIMITIVE_INT | ADF_DEFAULT | ADF_UNSIGNED | 13,
		ADD_UINT64 = ADF_PRIMITIVE_INT | ADF_DEFAULT | ADF_UNSIGNED | 14,
	};

	constexpr bool IsUnsignedType(ADLDataTypeId id) {
		return id & ADF_UNSIGNED;
	}

	constexpr bool IsIntType(ADLDataTypeId id) {
		return (id & ADF_PRIMITIVE_MASK) == ADF_PRIMITIVE_INT;
	}

	constexpr bool IsFloatType(ADLDataTypeId id) {
		return (id & ADF_PRIMITIVE_MASK) == ADF_PRIMITIVE_FLOAT;
	}

	constexpr bool IsNumberType(ADLDataTypeId id) {
		return IsIntType(id) || IsFloatType(id);
	}

	constexpr bool IsDefaultType(ADLDataTypeId id) {
		return (id & ADF_MASK) == ADF_DEFAULT;
	}

	constexpr uint64_t PADDING_FIELD_HASH = hash::Hash64("$$padding");

	struct ADLStructField {
		uint64_t name;
		size_t offset{};
		ADLDataTypeId type{ ADD_UNKNOWN };
		bool pointer{};
		size_t arraySize{};
		size_t elemSize{};
	};

	struct ADLEnumField {
		uint64_t name;
		int64_t value;
	};

	struct ADLStruct {
		uint64_t name{};
		uint8_t align{};
		size_t size{};
		size_t forcedSize{};
		std::vector<ADLStructField> fields{};
	};

	struct ADLEnum {
		uint64_t name;
		ADLDataTypeId typeSize{};
		std::vector<ADLEnumField> fields{};
	};

	struct ADLFlag {
		uint64_t name;
		ADLDataTypeId typeSize{};
		std::vector<ADLEnumField> fields{};
	};

	struct ADLData {
		uint64_t rootName{};
		uint8_t align{};
		std::unordered_map<uint64_t, ADLDataTypeId> names{};
		std::unordered_map<uint64_t, size_t> defaults{};
		std::unordered_map<ADLDataTypeId, ADLDataTypeId> typedefs{};
		std::unordered_map<ADLDataTypeId, ADLStruct> structs{};
		std::unordered_map<ADLDataTypeId, ADLEnum> enums{};
		std::unordered_map<ADLDataTypeId, ADLFlag> flags{};

		ADLData() {
			names[hash::Hash64("bool")] = ADD_UINT8;
			names[hash::Hash64("byte")] = ADD_UINT8;
			names[hash::Hash64("uint8")] = ADD_UINT8;
			names[hash::Hash64("uint8_t")] = ADD_UINT8;
			names[hash::Hash64("uint16")] = ADD_UINT16;
			names[hash::Hash64("uint16_t")] = ADD_UINT16;
			names[hash::Hash64("uint32")] = ADD_UINT32;
			names[hash::Hash64("uint32_t")] = ADD_UINT32;
			names[hash::Hash64("uint64")] = ADD_UINT64;
			names[hash::Hash64("uint64_t")] = ADD_UINT64;
			names[hash::Hash64("char")] = ADD_INT8;
			names[hash::Hash64("int8")] = ADD_INT8;
			names[hash::Hash64("int8_t")] = ADD_INT8;
			names[hash::Hash64("int16")] = ADD_INT16;
			names[hash::Hash64("int16_t")] = ADD_INT16;
			names[hash::Hash64("int32")] = ADD_INT32;
			names[hash::Hash64("int32_t")] = ADD_INT32;
			names[hash::Hash64("int64")] = ADD_INT64;
			names[hash::Hash64("int64_t")] = ADD_INT64;
			names[hash::Hash64("float")] = ADD_FLOAT;
			names[hash::Hash64("double")] = ADD_DOUBLE;
			names[hash::Hash64("string")] = ADD_STRING;
			names[hash::Hash64("hash")] = ADD_HASH;

			defaults[ADD_UINT8] = 1;
			defaults[ADD_UINT16] = 2;
			defaults[ADD_UINT32] = 4;
			defaults[ADD_UINT64] = 8;
			defaults[ADD_INT8] = 1;
			defaults[ADD_INT16] = 2;
			defaults[ADD_INT32] = 4;
			defaults[ADD_INT64] = 8;
			defaults[ADD_FLOAT] = 4;
			defaults[ADD_DOUBLE] = 8;
			defaults[ADD_STRING] = 8;
			defaults[ADD_HASH] = 8;
		}

		ADLDataTypeId GetBaseType(ADLDataTypeId id) const {
			// loop until we find the base type
			while ((id & ADF_MASK) == ADF_TYPEDEF) {
				auto it2 = typedefs.find(id);

				if (it2 == typedefs.end()) {
					return 0; // wtf?
				}

				id = it2->second;
			}
			
			return id;
		}

		ADLDataTypeId IdOfName(uint64_t name) const {
			auto it = names.find(name);

			if (it == names.end()) {
				return 0;
			}

			return GetBaseType(it->second);
		}

		ADLDataTypeId IdOfName(const std::string& name) const {
			return IdOfName(hash::Hash64(name.c_str()));
		}

		size_t SizeOf(ADLDataTypeId type) const {
			type = GetBaseType(type);

			switch (type & ADF_MASK) {
			case ADF_ENUM: {
				auto it = enums.find(type);
				if (it == enums.end()) {
					return 0;
				}
				return SizeOf(it->second.typeSize);
			}
			case ADF_FLAG: {
				auto it = flags.find(type);
				if (it == flags.end()) {
					return 0;
				}
				return SizeOf(it->second.typeSize);
			}
			case ADF_STRUCT: {
				auto it = structs.find(type);
				if (it == structs.end()) {
					return 0;
				}
				return it->second.size;
			}
			case ADF_DEFAULT: {
				auto it = defaults.find(type);
				if (it == defaults.end()) {
					return 0;
				}
				return it->second;
			}
			case ADF_UNKNOWN:
			default:
				return 0; // wtf?
			}

			return 0;
		}

		ADLDataTypeId RegisterTypeDef(const std::string& name, ADLDataTypeId to) {
			uint64_t hash = hash::Hash64(name.c_str());
			if (IdOfName(hash)) {
				return 0; // already defined
			}
			ADLDataTypeId id{ (ADLDataTypeId)((typedefs.size() + 1) | ADF_TYPEDEF) };
			names[hash] = id;
			typedefs[id] = to;
			return id;
		}

		ADLStruct* CreateStruct(const std::string& name, uint8_t align, size_t size) {
			uint64_t hash = hash::Hash64(name.c_str());
			if (IdOfName(hash)) {
				return nullptr; // already defined
			}

			ADLDataTypeId id{ (ADLDataTypeId)((structs.size() + 1) | ADF_STRUCT) };
			names[hash] = id;
			auto& s = structs[id];
			s.name = hash;
			s.align = align;
			s.forcedSize = size;
			return &s;
		}

		ADLEnum* CreateEnum(const std::string& name, ADLDataTypeId type) {
			uint64_t hash = hash::Hash64(name.c_str());
			if (IdOfName(hash)) {
				return nullptr; // already defined
			}

			ADLDataTypeId id{ (ADLDataTypeId)((enums.size() + 1) | ADF_ENUM) };
			names[hash] = id;
			ADLEnum& s = enums[id];
			s.name = hash;
			s.typeSize = type;
			return &s;
		}

		ADLFlag* CreateFlag(const std::string& name, ADLDataTypeId type) {
			uint64_t hash = hash::Hash64(name.c_str());
			if (IdOfName(hash)) {
				return nullptr; // already defined
			}

			ADLDataTypeId id{ (ADLDataTypeId)((flags.size() + 1) | ADF_FLAG) };
			names[hash] = id;
			ADLFlag& s = flags[id];
			s.name = hash;
			s.typeSize = type;
			return &s;
		}
	};

}