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
			writer.WriteHash(hash);
			writer.WriteString("\"");
		}

		template<typename T>
		void WriteValueNumber(T hash) {
			WritePreData();
			writer.WriteString(std::format("{}", hash));
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


		const std::vector<byte>& Build() {
			return writer.Build();
		}
	};

}