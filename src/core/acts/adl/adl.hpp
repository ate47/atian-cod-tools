#pragma once
#include <dbflib.hpp>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>

namespace acts::compiler::adl {
	typedef uint32_t ADLDataTypeId;
	enum ADLDataTypeIdBase : ADLDataTypeId {};

	std::ostream& PrintId(std::ostream& out, ADLDataTypeId id);

	enum ADLDataTypeFlags : ADLDataTypeId {
		ADF_UNKNOWN = 0,
		ADF_PRIMITIVE_SHIFT = 26,
		ADF_PRIMITIVE_UNKNOWN = 0u << ADF_PRIMITIVE_SHIFT,
		ADF_PRIMITIVE_INT = 1u << ADF_PRIMITIVE_SHIFT,
		ADF_PRIMITIVE_FLOAT = 2u << ADF_PRIMITIVE_SHIFT,
		ADF_PRIMITIVE_MASK = 3u << ADF_PRIMITIVE_SHIFT,
		ADF_UNSIGNED = 1u << 28,
		ADF_MASK_SHIFT = 29,
		ADF_DEFAULT = 1u << ADF_MASK_SHIFT,
		ADF_STRUCT = 2u << ADF_MASK_SHIFT,
		ADF_ENUM = 3u << ADF_MASK_SHIFT,
		ADF_TYPEDEF = 4u << ADF_MASK_SHIFT,
		ADF_FLAG = 5u << ADF_MASK_SHIFT,
		ADF_CUSTOM_DEF = 6u << ADF_MASK_SHIFT,
		ADF_MASK = 7u << ADF_MASK_SHIFT,
		ADF_ID = ~(ADF_MASK | ADF_UNSIGNED | ADF_PRIMITIVE_MASK),
	};
	static_assert((ADF_MASK& ADF_UNSIGNED) == 0);
	static_assert((ADF_MASK& ADF_PRIMITIVE_MASK) == 0);
	static_assert((ADF_UNSIGNED& ADF_PRIMITIVE_MASK) == 0);

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
}

template<>
struct std::formatter<acts::compiler::adl::ADLDataTypeIdBase, char> {
	template<class ParseContext>
	constexpr ParseContext::iterator parse(ParseContext& ctx) {
		auto it = ctx.begin();
		if (it == ctx.end()) {
			return it;
		}
		if (*it != '}') {
			throw std::format_error("Invalid format args.");
		}

		return it;
	}

	template<class FmtContext>
	FmtContext::iterator format(const acts::compiler::adl::ADLDataTypeIdBase& p, FmtContext& ctx) const {
		std::ostringstream out;

		acts::compiler::adl::PrintId(out, p);

		return std::ranges::copy(std::move(out).str(), ctx.out()).out;
	}
};

namespace acts::compiler::adl {
	constexpr uint64_t PADDING_FIELD_HASH = hash::Hash64("$$padding");
	constexpr uint64_t SERIAL_MAGIC = 0x2441444c0a0d00;
	constexpr byte SERIAL_VERSION = 0x20;
	constexpr uint64_t SERIAL_MAGIC_VER = SERIAL_MAGIC | SERIAL_VERSION;

	// Serialization

	enum SerialDataType : byte {
		ADL_SERIAL_DONE = 0xFF,
		ADL_SERIAL_HASHES = 0x40,
		ADL_SERIAL_STRUCT = 0x41,
		ADL_SERIAL_TYPEDEF = 0x42,
		ADL_SERIAL_ENUM = 0x43,
		ADL_SERIAL_FLAG = 0x44,
		ADL_SERIAL_NAME = 0x45,
	};

	// Compiled structure

	struct ADLCompiledHeader {
		byte version;
		void* data;
	};


	// ADL data

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
		ADLDataTypeId id{};
		uint64_t name{};
		uint8_t align{};
		size_t size{};
		size_t forcedSize{};
		std::vector<ADLStructField> fields{};

		const ADLStructField* GetField(uint64_t name) const {
			for (const ADLStructField& f : fields) {
				if (f.name == name) return &f;
			}
			return nullptr;
		}
	};

	struct ADLEnum {
		uint64_t name;
		ADLDataTypeId typeSize{};
		std::vector<ADLEnumField> fields{};

		const ADLEnumField* GetField(uint64_t name) const {
			for (const ADLEnumField& f : fields) {
				if (f.name == name) return &f;
			}
			return nullptr;
		}
	};

	struct ADLFlag {
		uint64_t name;
		ADLDataTypeId typeSize{};
		std::vector<ADLEnumField> fields{};

		const ADLEnumField* GetField(uint64_t name) const {
			for (const ADLEnumField& f : fields) {
				if (f.name == name) return &f;
			}
			return nullptr;
		}
	};

	struct ADLCustomType {
		uint64_t name;
		size_t size;
	};

	constexpr bool IsADLCompiledFile(void* file, size_t len) {
		return len >= sizeof(SERIAL_MAGIC_VER) && *(decltype(SERIAL_MAGIC_VER)*)file == SERIAL_MAGIC_VER;
	}

	struct ADLData {
		std::unordered_set<std::filesystem::path> loadedPath{};
		uint64_t rootName{};
		uint8_t align{};
		std::unordered_map<uint64_t, ADLDataTypeId> names{};
		std::unordered_map<uint64_t, size_t> defaults{};
		std::unordered_map<ADLDataTypeId, ADLCustomType> customtypes{};
		std::unordered_map<ADLDataTypeId, ADLDataTypeId> typedefs{};
		std::unordered_map<ADLDataTypeId, ADLStruct> structs{};
		std::unordered_map<ADLDataTypeId, ADLEnum> enums{};
		std::unordered_map<ADLDataTypeId, ADLFlag> flags{};
		std::vector<ADLDataTypeId> generated{};
		std::unordered_set<std::string> hashes{};

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

			names[hash::Hash64("int")] = ADD_INT32;
			names[hash::Hash64("uint")] = ADD_UINT32;
			names[hash::Hash64("long")] = ADD_INT64;
			names[hash::Hash64("ulong")] = ADD_UINT64;

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

		void Dump() {
			hashutils::ReadDefaultFile();
			LOG_INFO("files");
			for (auto& path : loadedPath) {
				LOG_INFO("- {}", path.string());
			}
			LOG_INFO("names");
			for (auto& [name, id] : names) {
				LOG_INFO("  {} -> {}", hashutils::ExtractTmp("hash", name), (ADLDataTypeIdBase)id);
			}
			LOG_INFO("typedefs");
			for (auto& [id1, id2] : typedefs) {
				LOG_INFO("  {} -> {}", (ADLDataTypeIdBase)id1, (ADLDataTypeIdBase)id2);
			}
			LOG_INFO("structs");
			for (auto& [id, strct] : structs) {
				LOG_INFO("  {} -> {} (alig:0x{:x},size:0x{:x},forcesize:0x{:x})", (ADLDataTypeIdBase)id, hashutils::ExtractTmp("hash", strct.name), strct.align, strct.size, strct.forcedSize);

				for (auto& field : strct.fields) {

					LOG_INFO("      {} -> {} (off:0x{:x},len:{},ptr:{},size:0x{:x})", hashutils::ExtractTmp("hash", field.name), (ADLDataTypeIdBase)field.type, field.offset, field.arraySize, field.pointer, SizeOf(field.type));
				}
			}
			LOG_INFO("enums");
			for (auto& [id, enm] : enums) {
				LOG_INFO("  {} -> {} (type:{})", (ADLDataTypeIdBase)id, hashutils::ExtractTmp("hash", enm.name), (ADLDataTypeIdBase)enm.typeSize);
				for (auto& field : enm.fields) {
					LOG_INFO("      {} -> {}0x{:x}", hashutils::ExtractTmp("hash", field.name), (field.value < 0 ? "-" : ""), (field.value < 0 ? -field.value : field.value));
				}
			}
			LOG_INFO("flags");
			for (auto& [id, flg] : flags) {
				LOG_INFO("  {} -> {} (type:{})", (ADLDataTypeIdBase)id, hashutils::ExtractTmp("hash", flg.name), (ADLDataTypeIdBase)flg.typeSize);
				for (auto& field : flg.fields) {
					LOG_INFO("      {} -> {}0x{:x}", hashutils::ExtractTmp("hash", field.name), (field.value < 0 ? "-" : ""), (field.value < 0 ? -field.value : field.value));
				}
			}

			LOG_INFO("custom types");
			for (auto& [id, s] : customtypes) {
				LOG_INFO("  {} -> {} (size:0x{:x})", (ADLDataTypeIdBase)id, hashutils::ExtractTmp("hash", s.name), s.size);
			}
		}

		void AddHash(const char* str) {
			hashes.insert(str);
			hashutils::Add(str);
		}

		void AddHash(const std::string& str) {
			hashes.insert(str);
			hashutils::Add(str.c_str());
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
			case ADF_CUSTOM_DEF: {
				auto it = customtypes.find(type);
				if (it == customtypes.end()) {
					return 0;
				}
				return it->second.size;
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
			AddHash(name);
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
			AddHash(name);
			ADLDataTypeId id{ (ADLDataTypeId)((structs.size() + 1) | ADF_STRUCT) };
			names[hash] = id;
			generated.push_back(id);
			ADLStruct& s = structs[id];
			s.id = id;
			s.name = hash;
			s.align = align;
			s.forcedSize = size;
			return &s;
		}

		ADLCustomType* CreateCustomType(const std::string& name, size_t size) {
			uint64_t hash = hash::Hash64(name.c_str());
			if (size < 0 || IdOfName(hash)) {
				return nullptr;
			}
			AddHash(name);
			ADLDataTypeId id{ (ADLDataTypeId)((customtypes.size() + 1) | ADF_CUSTOM_DEF) };
			names[hash] = id;
			generated.push_back(id);
			auto& ct{ customtypes[id] };
			ct.name = hash;
			ct.size = size;

			return &ct;
		}

		ADLEnum* CreateEnum(const std::string& name, ADLDataTypeId type) {
			uint64_t hash = hash::Hash64(name.c_str());
			if (IdOfName(hash)) {
				return nullptr; // already defined
			}
			AddHash(name);
			ADLDataTypeId id{ (ADLDataTypeId)((enums.size() + 1) | ADF_ENUM) };
			names[hash] = id;
			generated.push_back(id);
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
			AddHash(name);
			ADLDataTypeId id{ (ADLDataTypeId)((flags.size() + 1) | ADF_FLAG) };
			generated.push_back(id);
			names[hash] = id;
			ADLFlag& s = flags[id];
			s.name = hash;
			s.typeSize = type;
			return &s;
		}

		bool Serialize(std::vector<byte> data) const {
			utils::WriteValue<decltype(SERIAL_MAGIC_VER)>(data, SERIAL_MAGIC_VER);
			size_t mapLocation{ utils::Allocate(data, sizeof(uint64_t)) };

			// hashes
			size_t hashLocation{ data.size() };
			utils::WriteValue<uint64_t>(data, hashes.size());
			for (const std::string& h : hashes) {
				utils::WriteString(data, h.c_str());
			}

			// names
			size_t nameLocation{ data.size() };
			utils::WriteValue<uint64_t>(data, names.size());
			for (const auto& [o, t] : names) {
				utils::WriteValue<uint64_t>(data, o);
				utils::WriteValue<uint32_t>(data, t);
			}


			// TODO: Compile the rest

			
			// write sections map
			*reinterpret_cast<uint64_t*>(&data[mapLocation]) = data.size();

			utils::WriteValue<SerialDataType>(data, SerialDataType::ADL_SERIAL_HASHES);
			utils::WriteValue<uint64_t>(data, hashLocation);

			utils::WriteValue<SerialDataType>(data, SerialDataType::ADL_SERIAL_NAME);
			utils::WriteValue<uint64_t>(data, nameLocation);

			// complete last section
			utils::WriteValue<SerialDataType>(data, SerialDataType::ADL_SERIAL_DONE);
			utils::WriteValue<uint64_t>(data, SERIAL_MAGIC_VER);

			return true;
		}

		void Deserialize(void* data, size_t len) {
			if (len < sizeof(SERIAL_MAGIC_VER)) {
				throw std::runtime_error("too small");
			}
			if (*(uint64_t*)data != SERIAL_MAGIC_VER) {
				if ((*(uint64_t*)data & ~0xFFull) != SERIAL_MAGIC) {
					throw std::runtime_error("invalid magic");
				}
				throw std::runtime_error("invalid version");
			}

			// TODO: deserialize


		}

		void WriteIntVal(byte* ptr, size_t offset, size_t size, int64_t v, bool isSigned) {
			void* r = (void*)(ptr + offset);

			switch (size) {
			case 1:
				if (isSigned) {
					*(int8_t*)r = (int8_t)v;
				}
				else {
					*(uint8_t*)r = (uint8_t)v;
				}
				break;
			case 2:
				if (isSigned) {
					*(int16_t*)r = (int16_t)v;
				}
				else {
					*(uint16_t*)r = (uint16_t)v;
				}
				break;
			case 4:
				if (isSigned) {
					*(int32_t*)r = (int32_t)v;
				}
				else {
					*(uint32_t*)r = (uint32_t)v;
				}
				break;
			case 8:
				if (isSigned) {
					*(int64_t*)r = v;
				}
				else {
					*(uint64_t*)r = (uint64_t)v;
				}
				break;
			default: throw std::runtime_error(utils::va("invalid write raw size %d", size));
			}


		}

		dbflib::BlockId ConvertStruct(rapidjson::Value& member, const ADLStruct& str, dbflib::DBFileBuilder& builder) {
			auto [structLoc, structData] = builder.CreateBlock<byte*>(str.size);

			if (!member.IsObject()) {
				return structLoc; // invalid? just alloc
			}

			for (auto& [k, v] : member.GetObj()) {
				const char* key = k.GetString();
				uint64_t keyHash = hash::Hash64(key);

				bool done{};
				for (const ADLStructField& field : str.fields) {
					if (field.name != keyHash) {
						continue;
					}
					done = true;

					ADLDataTypeId type = field.type;

					if (field.pointer) {
						// TODO: qzd ptr
					}

					switch (type & ADF_MASK) {
					case ADF_STRUCT:

						break;
					case ADF_ENUM: {
						auto eit = enums.find(field.type);

						if (eit == enums.end()) throw std::runtime_error("it exception enums");

						ADLEnum& en = eit->second;
						
						if (!v.IsString()) {
							throw std::runtime_error(utils::va("enum value for key %s should be a string", key));
						}

						const char* keyVal = v.GetString();
						const ADLEnumField* efield = en.GetField(hash::Hash64(keyVal));

						if (efield) {
							WriteIntVal(builder.GetBlock<byte>(structLoc), field.offset, SizeOf(field.type), efield->value, !IsUnsignedType(field.type));
						}
						else {
							LOG_WARNING("undefined enum value '{}' for enum {}", key, hashutils::ExtractTmp("hash", en.name));
						}
						
					}
						break;
					case ADF_FLAG:

						break;
					case ADF_CUSTOM_DEF:

						break;
					case ADF_DEFAULT:

						break;
					}

					break;
				}
				if (!done) {
					LOG_WARNING("undefined field {} for struct {}", key, hashutils::ExtractTmp("hash", str.name));
				}
			}

			// Compile json to chunks

			return structLoc;
		}

		dbflib::BlockId Convert(const std::string& json, dbflib::DBFileBuilder& builder) {
			auto [headerLoc, header] = builder.CreateBlock<ADLCompiledHeader>();

			header->version = SERIAL_VERSION;

			rapidjson::Document doc{};
			doc.Parse(json.data(), json.length());

			if (!rootName) {
				throw std::runtime_error("missing root name, please use #root to define it");
			}

			ADLDataTypeId rootId{ IdOfName(rootName) };

			if (!rootId || (rootId & ADF_MASK) != ADF_STRUCT) {
				throw std::runtime_error(utils::va("the defined root '%s' doesn't exist or isn't a structure", hashutils::ExtractTmp("hash", rootId)));
			}

			auto it = structs.find(rootId);
			if (it == structs.end()) {
				throw std::runtime_error("invalid struct root");
			}

			dbflib::BlockId root = ConvertStruct(doc, it->second, builder);

			// link the root to the header
			builder.CreateLink(headerLoc, offsetof(ADLCompiledHeader, data), root);

			return headerLoc;
		}
	};
}