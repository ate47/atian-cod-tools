#pragma once
#include "raw_file.hpp"

namespace core::raw_file::json {
	enum JsonFormat {
		JFF_PRETTY,
		JFF_INLINE,
		JFF_REDUCED
	};

	class RawFileJsonWriter {
		core::raw_file::RawFileWriter writer{};
		JsonFormat format{ JFF_PRETTY };

		size_t depth{};
		bool startStruct{ true };
		bool inlined{ true };

	public:
		RawFileJsonWriter(JsonFormat format = JFF_PRETTY) : format(format) {
		}

		void WriteNewSpace() {
			switch (format) {
			case JFF_PRETTY:
			case JFF_INLINE:
				writer.WriteString(" ");
				break;
			}
		}
		void WriteNewLine() {
			switch (format) {
			case JFF_PRETTY:
				writer.WriteString("\n");
				// padding
				for (size_t i = 0; i < depth; i++) {
					writer.WriteString("    ");
				}
				break;
			case JFF_INLINE:
				writer.WriteString(" ");
				break;
			}
		}

		void SetFormat(JsonFormat _format) {
			format = _format;
		}

		void WritePreData(bool forceNoInline = false) {
			if (!forceNoInline && inlined) return;
			inlined = false;
			if (!startStruct) {
				writer.WriteString(",");
			}
			else {
				startStruct = false;
			}
			WriteNewLine();
		}

		void WriteValueNull() {
			WritePreData();
			writer.WriteString("null");
		}

		void WriteValueString(const char* str) {
			WritePreData();
			writer.WriteString("\"");
			writer.WriteString(str);
			writer.WriteString("\"");
		}


		void WriteValueString(const std::string& str) {
			WriteValueString(str.c_str());
		}

		void WriteValueEncrypted(const char* str) {
			WritePreData();
			writer.WriteString("\"");
			writer.WriteEncryptedString(str);
			writer.WriteString("\"");
		}

		void WriteValueHash(uint64_t hash, const char* type = "#") {
			WritePreData();
			writer.WriteString("\"");
			writer.WriteString(type);
			writer.WriteHashEscaped(hash);
			writer.WriteString("\"");
		}

		template<typename T, bool hex = false>
		void WriteValueNumber(T hash) {
			WritePreData();
			if constexpr (hex) {
				writer.WriteString(std::format("0x{:x}", hash));
			}
			else {
				writer.WriteString(std::format("{}", hash));
			}
		}

		void WriteValueLiteral(const char* val) {
			WritePreData();
			writer.WriteString(val);
		}

		void WriteValueLiteral(const std::string& val) {
			WriteValueLiteral(val.data());
		}

		void WriteValueBool(bool val) {
			WriteValueLiteral(val ? "true" : "false");
		}

		void WriteFieldNameString(const char* name) {
			WritePreData(true);
			inlined = true;
			WriteValueString(name);
			writer.WriteString(":");
			WriteNewSpace();
		}

		void WriteFieldNameStringEncrypted(const char* name) {
			WritePreData(true);
			inlined = true;
			WriteValueEncrypted(name);
			writer.WriteString(":");
			WriteNewSpace();
		}

		void WriterFieldNameHash(uint64_t name, const char* type = "#") {
			WritePreData(true);
			inlined = true;
			WriteValueHash(name, type);
			writer.WriteString(":");
			WriteNewSpace();
		}

		void WriteFieldValueHash(int64_t name, uint64_t val) {
			WriterFieldNameHash(name);
			WriteValueHash(val);
		}

		template<bool encryptedKey = false>
		void WriteFieldValueHash(const char* name, uint64_t val) {
			if constexpr (encryptedKey) {
				WriteFieldNameStringEncrypted(name);
			}
			else {
				WriteFieldNameString(name);
			}
			WriteValueHash(val);
		}

		template<typename T, bool encryptedKey = false>
		void WriteFieldValueUnknown(const char* name, T val) {
			if (!val) return;
			WriteFieldValueUnknown(name, &val, sizeof(val));
		}

		template<bool encryptedKey = false>
		void WriteFieldValueUnknown(const char* name, void* ptr, size_t l) {
			if constexpr (encryptedKey) {
				WriteFieldNameStringEncrypted(name);
			}
			else {
				WriteFieldNameString(name);
			}
			WriteUnknownData(ptr, l);
		}

		template<typename T, bool hex = false>
		void WriteFieldValueNumber(int64_t hash, T val) {
			WriterFieldNameHash(hash);
			WriteValueNumber<T, hex>(val);
		}

		template<typename T, bool encryptedKey = false, bool hex = false>
		void WriteFieldValueNumber(const char* name, T val) {
			if constexpr (encryptedKey) {
				WriteFieldNameStringEncrypted(name);
			}
			else {
				WriteFieldNameString(name);
			}
			WriteValueNumber<T, hex>(val);
		}

		void WriteFieldValueBool(int64_t name, bool val) {
			WriterFieldNameHash(name);
			WriteValueBool(val);
		}

		template<bool encryptedKey = false>
		void WriteFieldValueBool(const char* name, bool val) {
			if constexpr (encryptedKey) {
				WriteFieldNameStringEncrypted(name);
			}
			else {
				WriteFieldNameString(name);
			}
			WriteValueBool(val);
		}

		template<bool encryptedStr = false>
		void WriteFieldValueString(int64_t name, const char* val) {
			WriterFieldNameHash(name);
			if constexpr (encryptedStr) {
				WriteValueEncrypted(val);
			}
			else {
				WriteValueString(val);
			}
		}

		template<bool encryptedKey = false, bool encryptedStr = false>
		void WriteFieldValueString(const char* name, const char* val) {
			if constexpr (encryptedKey) {
				WriteFieldNameStringEncrypted(name);
			}
			else {
				WriteFieldNameString(name);
			}
			if constexpr (encryptedStr) {
				WriteValueEncrypted(val);
			}
			else {
				WriteValueString(val);
			}
		}

		void WriteFieldValueString(int64_t name, const std::string& val) {
			WriteFieldValueString(name, val.c_str());
		}

		template<bool encryptedKey = false>
		void WriteFieldValueString(const char* name, const std::string& val) {
			WriteFieldValueString(name, val.c_str());
		}

		void WriteUnknownData(const void* data, size_t len);

		template<typename T>
		inline void WriteUnknownData(const T t) {
			WriteUnknownData(&t, sizeof(t));
		}

		void BeginObject() {
			WritePreData();
			writer.WriteString("{");
			depth++;
			startStruct = true;
			inlined = false;
		}

		void BeginArray() {
			WritePreData();
			writer.WriteString("[");
			depth++;
			startStruct = true;
			inlined = false;
		}

		void EndObject() {
			if (!depth) throw std::runtime_error("End array without creating one");
			depth--;
			if (!startStruct) {
				WriteNewLine();
			}
			writer.WriteString("}");
			startStruct = false;
			inlined = false;
		}

		void EndArray() {
			if (!depth) throw std::runtime_error("End array without creating one");
			depth--;
			if (!startStruct) {
				WriteNewLine();
			}
			writer.WriteString("]");
			startStruct = false;
			inlined = false;
		}


		std::vector<byte>& Build() {
			return writer.Build();
		}
	};

}