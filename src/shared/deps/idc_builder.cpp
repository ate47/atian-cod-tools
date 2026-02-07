#include <includes_shared.hpp>
#include <deps/idc_builder.hpp>
#include <hook/library.hpp>

namespace deps::idc_builder {
	size_t GetRva(void* loc) {
		hook::library::Library library{ hook::library::GetLibraryInfo(loc) };
		if (!library) {
			return 0;
		}
		return (size_t)((byte*)loc - (byte*)*library);
	}

	IdcBuilder::IdcBuilder() {

	}

	void IdcBuilder::AddHeader(const char* header) {
		headers.emplace_back(alloc.CloneStr(header));
	}
	
	void IdcBuilder::AddStruct(const char* name, bool force) {
		structs.emplace_back(alloc.CloneStr(name), force);
	}

	void IdcBuilder::AddAddress(size_t rva, const char* name, const char* type) {
		AddAddressEx(rva, name, nullptr, type);
	}
	void IdcBuilder::AddAddressEx(size_t rva, const char* name, const char* flags, const char* type) {
		addresses.emplace_back(rva, alloc.CloneStr(name), type ? alloc.CloneStr(type) : nullptr, flags ? alloc.CloneStr(flags) : nullptr);
	}

	EnumDefinition& IdcBuilder::GetEnumDef(IdcEnumId id) {
		if (id >= enums.size()) throw std::runtime_error("Invalid IdcEnumId");
		return enums[id];
	}

	IdcEnumId IdcBuilder::AddEnum(const char* name, bool force) {
		IdcEnumId idx{ enums.size() };
		enums.emplace_back(alloc.CloneStr(name), force);
		return idx;
	}

	void IdcBuilder::AddEnumMember(IdcEnumId id, const char* name, int64_t val) {
		GetEnumDef(id).values.emplace_back(alloc.CloneStr(name), val);
	}

	void IdcBuilder::AddEnumMember(IdcEnumId id, const char* name) {
		std::vector<EnumValueDefinition>& values{ GetEnumDef(id).values };
		values.emplace_back(alloc.CloneStr(name), (int64_t)values.size());
	}

	void IdcBuilder::WriteIdcFile(std::filesystem::path out) {
		utils::OutFileCE os{ out, true };
		os << 
			// base include
			"#include <idc/idc.idc>\n"
			"\n"
			// custom funcs
			"// Create struct opt\n"
			"#define AddStrucOpt(u, name) if (GetStrucIdByName(name) == -1) AddStruc(u, name)\n"
			"\n"
			;
		if (headers.size()) {
			os << "// headers\n";
			for (const char* header : headers) {
				os << header << "\n";
			}
			os << "\n";
		}

		os << "static main() {";

		if (enums.size()) {
			utils::Padding(os << "\n", 1) << "// enums\n";
			for (EnumDefinition& def : enums) {
				int pad{ 1 };
				if (!def.force) {
					utils::Padding(os, pad) << "if (GetEnum(\"" << def.name << "\") == -1) {\n";
					pad++;
				}

				utils::Padding(os, pad) << "auto enumId = AddEnum(0, \"" << def.name << "\");\n";

				for (EnumValueDefinition& val : def.values) {
					utils::Padding(os, pad) << "AddConst(enumId, \"" << val.name << "\", " << std::dec << val.val << ");\n";
				}

				if (!def.force) {
					pad--;
					utils::Padding(os, pad) << "}\n";
				}
			}
		}

		if (structs.size()) {
			utils::Padding(os << "\n", 1) << "// structs\n";
			for (StructDefinition& def : structs) {
				utils::Padding(os, 1) << (def.force ? "AddStruc" : "AddStrucOpt") << "(0, \"" << def.name << "\");\n";
			}
		}

		if (addresses.size()) {
			utils::Padding(os << "\n", 1) << "// addresses\n";
			utils::Padding(os, 1) << "auto base = get_imagebase();\n\n";

			for (AddressDefinition& def : addresses) {
				if (def.flags) {
					utils::Padding(os, 1) << "MakeNameEx(base + 0x" << std::hex << def.rva << ", \"" << def.name << "\", " << def.flags << "); \n";
				}
				else {
					utils::Padding(os, 1) << "MakeName(base + 0x" << std::hex << def.rva << ", \"" << def.name << "\");\n";
				}
				if (def.type) {
					utils::Padding(os, 1) << "SetType(base + 0x" << std::hex << def.rva << ", \"" << def.type << "\");\n";
				}
			}
		}


		os << "}\n";
	}
}