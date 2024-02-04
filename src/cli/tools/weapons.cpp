#include <includes.hpp>

namespace {
	const char* fieldTypeNames[]{
		"int",
		"short",
		"byte",
		"float",
		"lstring",
		"string",
		"hash",
		"vector",
		"entity",
		"enthandle",
		"actor",
		"sentient",
		"sentienthandle",
		"client",
		"pathnode",
		"actorgroup",
		"object",
		"xmodel_index",
		"xmodel",
		"bitflag",
		"bitflag64",
		"fx",
		"weapon",
		"rumble",
		"scriptbundle",
	};
	enum fieldtype_t : UINT32 {
		F_INT = 0x0,
		F_SHORT = 0x1,
		F_BYTE = 0x2,
		F_FLOAT = 0x3,
		F_LSTRING = 0x4,
		F_STRING = 0x5,
		F_HASH = 0x6,
		F_VECTOR = 0x7,
		F_ENTITY = 0x8,
		F_ENTHANDLE = 0x9,
		F_ACTOR = 0xA,
		F_SENTIENT = 0xB,
		F_SENTIENTHANDLE = 0xC,
		F_CLIENT = 0xD,
		F_PATHNODE = 0xE,
		F_ACTORGROUP = 0xF,
		F_OBJECT = 0x10,
		F_XMODEL_INDEX = 0x11,
		F_XMODEL = 0x12,
		F_BITFLAG = 0x13,
		F_BITFLAG64 = 0x14,
		F_FX = 0x15,
		F_WEAPON = 0x16,
		F_RUMBLE = 0x17,
		F_SCRIPTBUNDLE = 0x18,
		F_COUNT = 0x19
	};

	const char* fieldNames[]{
		"invalid",
		"def",
		"properties",
	};

	enum WeaponFieldType : UINT32 {
		F_WEAPON_INVALID = 0x0,
		F_WEAPON_DEF = 0x1,
		F_WEAPON_PROPERTIES = 0x2,
		F_WEAPON_COUNT = 0x3
	};


	struct scr_weapon_field_t {
		UINT32 canonId;
		fieldtype_t type;
		bool isreadonly;
		int ofs;
		int size;
		WeaponFieldType weaponType;
		uintptr_t getter; // typedef void (*ScriptCallbackWeapon)(scriptInstance_t, WeaponDef, const scr_weapon_field_t *);
	};



	int wfdump(Process& proc, int argc, const char* argv[]) {
		std::vector<scr_weapon_field_t> fields[2]{};
		scr_weapon_field_t field{};


		auto loc = proc[0x4962660];

		std::ofstream of{ "wfield.csv" };

		if (!of) {
			std::cerr << "Can't open output file\n";
			return tool::BASIC_ERROR;
		}

		hashutils::ReadDefaultFile();

		of << "name,type,readonly,offset,size,weaponType,getter";

		while (true) {
			if (!proc.ReadMemory(&field, loc, sizeof(field))) {
				proc.WriteLocation(std::cerr << "Can't read weapon field at ", loc) << "\n";
				of.close();
				return tool::BASIC_ERROR;
			}
			loc += sizeof(field);

			if (!field.canonId) {
				break;
			}


			of
				<< "\n"
				<< hashutils::ExtractTmp("var", field.canonId) << ","
				<< (field.type < ARRAYSIZE(fieldTypeNames) ? fieldTypeNames[field.type] : "invalid") << ","
				<< (field.isreadonly ? "true" : "false") << ","
				<< std::hex
				<< "0x" << field.ofs << ","
				<< "0x" << field.size << ","
				<< (field.weaponType < ARRAYSIZE(fieldNames) ? fieldNames[field.weaponType] : "invalid") << ","
				;

			proc.WriteLocation(of, field.getter);

			if (!field.getter && field.weaponType && field.weaponType < F_WEAPON_COUNT) {
				fields[field.weaponType - 1].emplace_back(field);
			}
		}

		of.close();
		
		std::cout << "fields dumped in wfield.csv\n";

		// sort fields
		for (size_t i = 0; i < 2; i++) {
			std::sort(fields[i].begin(), fields[i].end(), [](const scr_weapon_field_t& o1, const scr_weapon_field_t& o2) -> bool {
				return o1.ofs < o2.ofs;
			});
		}

		std::ofstream hof{ "wfield.hpp" };

		if (!hof) {
			std::cerr << "Can't open output file\n";
			return tool::BASIC_ERROR;
		}


		hof
			<< "#pragma once\n"
			<< "\n"
			<< "namespace weaponfield {\n"
			;
		utils::Padding(hof, 1) << "typedef uint32_t bitflag;\n";
		utils::Padding(hof, 1) << "typedef uint64_t bitflag64;\n";
		utils::Padding(hof, 1) << "typedef uint32_t ScrString_t;\n";
		utils::Padding(hof, 1) << "\n";
		for (size_t i = 0; i < 2; i++) {
			utils::Padding(hof, 1) << "struct " << (i ? "WeaponDefProperties" : "WeaponDef") << " {\n";

			size_t current = 0;
			for (const auto& f : fields[i]) {
				if (f.ofs < current) {
					std::cerr << "bad sort " << std::hex << f.ofs << " < " << current << " for " << hashutils::ExtractTmp("var", f.canonId) << "\n";
					continue;
				}

				// fill with unk values
				while (f.ofs > current) {
					size_t rdelta = f.ofs - current;


					size_t delta;

					// force align

					if ((current & 1) && rdelta >= 2) {
						delta = 1; // align byte
					}
					else if ((current & 3) && rdelta >= 4) {
						delta = 3; // align short
					}
					else if ((current & 7) && rdelta >= 8) {
						delta = 7; // align int
					}
					else {
						delta = rdelta;
					}

					utils::Padding(hof, 2);

					if (delta < 2) {
						hof << "byte";
						current += 1;
					}
					else if (delta < 4) {
						hof << "uint16_t";
						current += 2;
					}
					else if (delta < 8) {
						hof << "uint32_t";
						current += 4;
					}
					else {
						hof << "uint64_t";
						current += 8;
					}
					hof << " unk" << std::hex << current << "; \n";
				}

				utils::Padding(hof, 2);

				size_t estimated;
				const char* typeName;
				switch (f.type) {
				case F_INT:
					typeName = "int32_t";
					estimated = 4;
					break;
				case F_SHORT:
					typeName = "int16_t";
					estimated = 2;
					break;
				case F_BYTE:
					typeName = "byte";
					estimated = 1;
					break;
				case F_FLOAT:
					typeName = "float";
					estimated = 4;
					break;
				case F_LSTRING:
					typeName = "const char*";
					estimated = 8;
					break;
				case F_STRING:
					typeName = "ScrString_t";
					estimated = 4;
					break;
				case F_HASH:
					typeName = "XHash";
					estimated = 0x10;
					break;
				case F_VECTOR:
					typeName = "vec3_t*";
					estimated = 8;
					break;
				case F_XMODEL:
					typeName = "XModel*";
					estimated = 8;
					break;
				case F_BITFLAG:
					typeName = "bitflag";
					estimated = 4;
					break;
				case F_BITFLAG64:
					typeName = "bitflag64";
					estimated = 8;
					break;
				case F_FX:
					typeName = "FxEffectDef*";
					estimated = 8;
					break;
				case F_SCRIPTBUNDLE:
					typeName = "ScriptBundle*";
					estimated = 8;
					break; 
				case F_RUMBLE:
					typeName = "RumbleInfo*";
					estimated = 8;
					break;
				case F_WEAPON:
					typeName = "WeaponDef*";
					estimated = 8;
					break;
				default:
					estimated = 0;
					break;
				}
				if (!estimated) {
					hof << "// can't read def type at " << std::hex << current << ", type=" << (f.type < ARRAYSIZE(fieldTypeNames) ? fieldTypeNames[f.type] : "invalid") << "\n";
					current += f.size;
					continue;
				}

				if (estimated != f.size) {
					hof << "// can't read def type at " << std::hex << current << ", type=" << (f.type < ARRAYSIZE(fieldTypeNames) ? fieldTypeNames[f.type] : "invalid") << " for " << typeName << "\n";
					current += f.size;
					continue;
				}

				hof << typeName << " " << hashutils::ExtractTmp("var", f.canonId) << ";\n";
				current += f.size;
			}

			utils::Padding(hof, 1) << "};\n";
		}
		hof
			<< "}\n"
			;
		
		hof.close();

		std::cout << "fields dumped in wfield.hpp\n";


		return tool::OK;
	}


}

ADD_TOOL("wfd", "", "Weapon field dump", L"BlackOps4.exe", wfdump);