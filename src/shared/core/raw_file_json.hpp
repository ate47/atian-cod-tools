#pragma once
#include "raw_file.hpp"

namespace core::raw_file::json {
	class RawFileJsonWriter {
		core::raw_file::RawFileWriter writer{};

		size_t depth{};
		bool startStruct{ true };
		bool inlined{ true };

	public:
		void WritePadding() {
			for (size_t i = 0; i < depth; i++) {
				writer.WriteString("    ");
			}
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
			writer.WriteString("\n");
			WritePadding();
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
			writer.WriteString(": ");
		}

		void WriteFieldNameStringEncrypted(const char* name) {
			WritePreData(true);
			inlined = true;
			WriteValueEncrypted(name);
			writer.WriteString(": ");
		}

		void WriterFieldNameHash(uint64_t name, const char* type = "#") {
			WritePreData(true);
			inlined = true;
			WriteValueHash(name, type);
			writer.WriteString(": ");
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
				writer.WriteString("\n");
				WritePadding();
			}
			writer.WriteString("}");
			startStruct = false;
			inlined = false;
		}

		void EndArray() {
			if (!depth) throw std::runtime_error("End array without creating one");
			depth--;
			if (!startStruct) {
				writer.WriteString("\n");
				WritePadding();
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