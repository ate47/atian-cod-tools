#include <includes.hpp>
#include <games/bo4/scriptinstance.hpp>
#include <utils/io_utils.hpp>
#include <hook/module_mapper.hpp>
#include <hook/error.hpp>
#include <hook/memory.hpp>
#include <utils/decrypt.hpp>
#include <DbgHelp.h>
#pragma comment(lib, "imagehlp.lib")
#include "tools/gsc.hpp"
#include "tools/pool.hpp"
#include "tools/bo6/bo6.hpp"
#include <core/memory_allocator.hpp>

namespace {
	using namespace tool::gsc::opcode;
	void NullStub() {}
	template<typename T, T val>
	T ReturnStub() { return val; }

	int exe_mapper(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 1)) return tool::BAD_USAGE;


		const char* exe{ argv[2] };

		hook::module_mapper::Module mod{ true };

		LOG_INFO("Loading module {}", exe);
		if (!mod.Load(exe) || !mod) {
			LOG_ERROR("Can't load module");
			return tool::BASIC_ERROR;
		}

		LOG_INFO("Loaded");

		LOG_INFO("{}", (void*)mod->GetNTHeader());
		LOG_INFO("{:x}", mod->GetOptHeader()->ImageBase);
		LOG_INFO("{}", *mod);
		LOG_INFO("{}", mod->Get<void>(0ull));

		
		utils::OutFileCE outVC{ "imports.txt" };
		LOG_INFO("imports.txt");

		hook::library::Library lib{ *mod };

		IMAGE_DATA_DIRECTORY& dir{ lib.GetOptHeader()->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT] };
		if (!dir.VirtualAddress || dir.Size <= 0) return 0; // nothing to patch

		PIMAGE_IMPORT_DESCRIPTOR imports{ (PIMAGE_IMPORT_DESCRIPTOR)(lib[dir.VirtualAddress]) };

		LOG_TRACE("Loading imports 0x{:x}/0x{:x}", dir.VirtualAddress, dir.Size);

		std::ostringstream osnames{};

		Process proc{ L"acts.exe" };

		while (imports->Name) {
			IMAGE_IMPORT_DESCRIPTOR& imp{ *imports++ };

			const char* name{ lib.Get<const char>(imp.Name) };

			if (core::logs::getlevel() <= core::logs::LVL_TRACE) osnames << " " << name;

			hook::library::Library dep{ name, false, DONT_RESOLVE_DLL_REFERENCES | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS };

			outVC << dep << ":\n";

			if (!dep) {
				osnames << "<missing>";
				LOG_TRACE("Can't load IAT lib {} to patch {}", name, *lib);
				continue;
			}

			//lib.Get<IMAGE_THUNK_DATA>(imp.)

			IMAGE_THUNK_DATA* thunks{ lib.Get<IMAGE_THUNK_DATA>(imp.FirstThunk) };
			IMAGE_THUNK_DATA* originalThunks{ lib.Get<IMAGE_THUNK_DATA>(imp.OriginalFirstThunk) };

			while (originalThunks->u1.Function) {
				IMAGE_THUNK_DATA& thunk{ *thunks++ };
				IMAGE_THUNK_DATA& originalThunk{ *originalThunks++ };

				if (originalThunk.u1.Ordinal & IMAGE_ORDINAL_FLAG64) {
					outVC << std::hex << "0x" << originalThunk.u1.Ordinal << "-> " << hook::library::CodePointer{&thunk} << "\n";
				}
				else {
					outVC << lib.Get<const char>(originalThunk.u1.Function + 2) << "-> " << hook::library::CodePointer{ &thunk } << "\n";
				}
			}
		}
		//tool::pool::WriteHex(std::cout, 0, mod->GetOptHeader(), sizeof(*mod->GetOptHeader()));

		// TEST BO4 Dump data

		/*
		struct T8BuiltInFunc {
			uint32_t name;
			uint32_t minArgs;
			uint32_t maxArgs;
			uint32_t pad01;
			uintptr_t function;
			uint32_t type;
			uint32_t pad02;
		};

		const T8BuiltInFunc* defs = mod->Get<T8BuiltInFunc>(0x4f437c0);
		constexpr size_t len = 370;

		for (size_t i = 0; i < len; i++) {
			LOG_INFO("{} -> {}", hashutils::ExtractTmp("function", defs[i].name), mod->RlocBased(defs[i].function));
		}
		*/

		/*
		// Dump bo6 pool names
		auto* pools{ mod->Get<uintptr_t>(0x8FFBE40) };

		size_t idx{};
		while (pools[idx]) {
			const char* cc = mod->Rebase<const char>(pools[idx]);
			char* c = utils::CloneString(cc);
			LOG_INFO("    T10R_ASSET_{} = 0x{:x},", utils::UpperCase(c), idx);
			idx++;
		}
		*/

		LOG_INFO("Done");

		return tool::OK;
	}
	int exe_pool_dumper(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 3)) {
			LOG_ERROR("Different values (first, last):");
			LOG_ERROR("BO4 ...... \"physpreset\" \"assetlist\" ");
			LOG_ERROR("BOCW ..... \"zone\" \"\" ");
			LOG_ERROR("IW games . \"physicslibrary\" \"string\" ");
			return tool::BAD_USAGE;
		}
		if (!argv[3][0]) {
			LOG_ERROR("Can't search empty first element");
			return tool::BAD_USAGE;
		}

		std::filesystem::path exe{ argv[2] };

		hook::module_mapper::Module mod{ true };

		LOG_INFO("Loading module {}", exe.string());
		if (!mod.Load(exe) || !mod) {
			LOG_ERROR("Can't load module");
			return tool::BASIC_ERROR;
		}

		const char* first{ argv[3] };
		const char* last{ argv[4][0] ? argv[4] : nullptr };
		std::filesystem::path pools{ tool::NotEnoughParam(argc, 4) ? exe.parent_path() / "pools.hpp" : argv[5] };
		const char* prefix{ tool::NotEnoughParam(argc, 5) ? "ASSET_TYPE_" : argv[6] };

		LOG_INFO("Searching {}->{}", first, last ? last : "");

		for (hook::library::ScanResult& firstStringOffsetRes : mod->ScanString(first)) {
			uintptr_t firstStringOffset{ firstStringOffsetRes.GetPtr<uintptr_t>() };
			LOG_INFO("try string \"{}\" -> 0x{:x}", first, mod->Rloc(firstStringOffset));
			for (hook::library::ScanResult& poolNamesRes : mod->ScanNumber(firstStringOffset)) {
				uintptr_t poolNamesOffset{ poolNamesRes.GetPtr<uintptr_t>() };

				uintptr_t* poolNames{ reinterpret_cast<uintptr_t*>(poolNamesOffset) };
				LOG_INFO("try poolNames -> 0x{:x}", mod->Rloc(poolNamesOffset));

				// we can try to see if the next one is a valid string

				void* next{ reinterpret_cast<void*>(poolNames[1]) };

				if (next < **mod || next >(*mod)[0x10000000]) {
					LOG_ERROR("Not inside library");
					continue; // not inside the module
				}


				size_t count{};
				while (true) {
					if (!poolNames[count]) {
						if (last) {
							LOG_ERROR("Can't find last pool name"); // cw?
						}
						break;
					}
					const char* cc = mod->Rebase<const char>(poolNames[count]);
					LOG_TRACE("Find {}", cc);
					if (last && !_strcmpi(cc, last)) break;
					count++;
				}

				if (count <= 40) {
					LOG_ERROR("Not enough candidates: {}", count);
					continue;
				}

				LOG_INFO("poolNames -> 0x{:x}", mod->Rloc(poolNamesOffset));
				LOG_INFO("Found {} pool(s)", count);

				std::ofstream os{ pools };
				if (!os) {
					LOG_ERROR("Can't open {}", pools.string());
					return tool::BASIC_ERROR;
				}

				utils::CloseEnd osce{ os };

				os << "// Asset types (" << std::dec << count << "/0x" << std::hex << mod->Rloc(poolNamesOffset) << ")\n";
				os << "enum AssetType : int {\n";
				for (size_t i = 0; i < count; i++) {
					const char* cc = mod->Rebase<const char>(poolNames[i]);
					char* id{ utils::MapString(utils::CloneString(cc), [](char c) -> char {
						c = std::toupper(c);

						if (!(c == '_' || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))) {
							return '_'; // remap bad char
						}

						return c;
					}) };
					os << "    " << prefix << id << " = 0x" << std::hex << i << ", // " << std::dec << i << " " << cc << "\n";
				}
				os << "    " << prefix << "COUNT" << " = 0x" << std::hex << count << ",\n";
				os << "};\n\n";

				os << "const char* poolNames[] {\n";
				for (size_t i = 0; i < count; i++) {
					const char* cc = mod->Rebase<const char>(poolNames[i]);
					os << "    \"" << cc << "\",\n";
				}
				os << "};\n\n";

				LOG_INFO("Dump into {}", pools.string());

				return tool::OK;
			}
			LOG_ERROR("Can't find ref");
		}

		LOG_ERROR("Can't find candidate");
		return tool::BASIC_ERROR;
	}

	int sp24_data_dump(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 1)) return tool::BAD_USAGE;


		std::filesystem::path exe{ argv[2] };

		hook::module_mapper::Module mod{ true };

		LOG_INFO("Loading module {}", exe.string());
		if (!mod.Load(exe) || !mod) {
			LOG_ERROR("Can't load module");
			return tool::BASIC_ERROR;
		}

		LOG_INFO("Loaded");
		LOG_INFO("base: {:x}", mod->GetOptHeader()->ImageBase);

		// Dump bo6 pool names
		auto* pools{ mod->Get<uintptr_t>(0x8DFCC90) };
		{
			std::filesystem::path pf{ exe.parent_path() / "pools.csv" };
			std::ofstream osf{ pf };
			if (!osf) {
				LOG_ERROR("Can't open {}", pf.string());
			}
			else {
				utils::CloseEnd osfce{ osf };
				osf << "id,name";
				for (size_t idx = 0; idx < bo6::T10R_ASSET_COUNT; idx++) {
					const char* cc = mod->Rebase<const char>(pools[idx]);
					osf << "\n" << std::dec << idx << "," << cc;
				}
			}
			LOG_INFO("Created {}", pf.string());
		}

		return tool::OK;
	}

	struct GscFunction {
		uint64_t name;
		void(__fastcall* func)(void* vm);
		bool devFunc;
		byte minArgs;
		byte maxArgs;
	};

	struct GscFuncContainerData {
		GscFunction* functions;
		uint64_t count;
	};
	struct GscFuncContainerDataType {
		GscFuncContainerData data;
		bool isMethod;
	};
	static struct {
		std::vector<std::vector<GscFuncContainerDataType>> pool{};
		std::vector<GscFuncContainerDataType>* functions{};
	} bo6GscData;

	template<bool isMethod>
	void Bo6LoadFuncs(void* container, GscFuncContainerData* cfg) {
		bo6GscData.functions->emplace_back(*cfg, isMethod);
	}

	int bo6_gsc_dump(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 2)) return tool::BAD_USAGE;

		std::filesystem::path exe{ argv[2] };

		hook::module_mapper::Module mod{ true };

		LOG_INFO("Loading module {}", exe.string());
		if (!mod.Load(exe) || !mod) {
			LOG_ERROR("Can't load module");
			return tool::BASIC_ERROR;
		}

		LOG_INFO("Loaded");

		constexpr size_t len = 0x100000;

		std::unique_ptr<byte[]> data{ std::make_unique<byte[]>(len) };

		bo6GscData.pool.clear();

		mod->Redirect("4C 8B 4A 08 4D 85 C9 74 35", Bo6LoadFuncs<false>);
		mod->Redirect("4C 8B 4A 08 4D 85 C9 74 3F", Bo6LoadFuncs<true>);

		for (hook::library::ScanResult& res : mod->Scan("48 89 5C 24 08 55 48 8B EC 48 83 EC 40 48 8B D9 E8 ? ? ? ? ? ? ? ?")) {
			LOG_INFO("Loading function: {}", hook::library::CodePointer{ res.location });

			bo6GscData.pool.emplace_back();
			bo6GscData.functions = &bo6GscData.pool[bo6GscData.pool.size() - 1];

			res.GetPtr<void(*)(byte*)>()(data.get());
		}

		utils::OutFileCE os{ argv[3] };

		if (!os) {
			LOG_ERROR("Can't open {}", argv[3]);
			return tool::BASIC_ERROR;
		}

		LOG_INFO("Dumping functions in {}", argv[3]);

		os << "pool,container,type,name,dev,minargs,maxargs,offset";

		for (size_t i = 0; i < bo6GscData.pool.size(); i++) {
			std::vector<GscFuncContainerDataType>& pool{ bo6GscData.pool[i] };
			for (size_t j = 0; j < pool.size(); j++) {
				GscFuncContainerDataType& container{ pool[j] };
				for (size_t k = 0; k < container.data.count; k++) {
					GscFunction& func{ container.data.functions[k] };
					os
						<< "\n" << std::dec
						<< i << ","
						<< j << ","
						<< (container.isMethod ? "method" : "function") << ","
						<< hashutils::ExtractTmp("function", func.name) << ","
						<< (int)func.minArgs << ","
						<< (int)func.maxArgs << ","
						<< (func.devFunc ? "dev" : "common") << ","
						<< hook::library::CodePointer{ func.func };
				}
			}
		}


		return tool::OK;
	}

	constexpr const char* defaultBo6LuaFuncObject = "default";
	static struct {
		std::map<const char*, std::map<uint64_t, void*>> funcs{};
		const char* currentFunc{ defaultBo6LuaFuncObject };

		void Cleanup() {
			funcs.clear();
			bo6LuaData.currentFunc = defaultBo6LuaFuncObject;
		}
	} bo6LuaData;

	void Bo6LoadLuaPop(void* vm, size_t count) {
		bo6LuaData.currentFunc = defaultBo6LuaFuncObject;
	}
	void Bo6LoadLuaPush(void* vm, int a2, const char* category) {
		bo6LuaData.currentFunc = category;
	}

	void Bo6LoadLuaFunc(void* vm, uint64_t hash, void* func) {
		bo6LuaData.funcs[bo6LuaData.currentFunc][hash] = func;
	}
	void Bo6LoadLuaFuncStr(void* vm, const char* str, void* func) {
		uint64_t hash{ hash::Hash64A(str) };
		hashutils::AddPrecomputed(hash, str, true);
		Bo6LoadLuaFunc(vm, hash, func);
	}

	int bo6_lua_dump(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 2)) return tool::BAD_USAGE;

		std::filesystem::path exe{ argv[2] };

		hook::module_mapper::Module mod{ true };

		LOG_INFO("Loading module {}", exe.string());
		if (!mod.Load(exe) || !mod) {
			LOG_ERROR("Can't load module");
			return tool::BASIC_ERROR;
		}

		LOG_INFO("Loaded");

		bo6LuaData.Cleanup();
		
		// remove lua things
		mod->Redirect("48 89 5C 24 10 57 48 83 EC 20 48 8B 44 24 28 48 8B D9 48 8D 0D 87", NullStub); // 88F4A60
		mod->Redirect("48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 48 8B 44 24 28 48 8B D9 48 8D 0D D2 A9", NullStub); // 88F5610
		mod->Redirect("48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 48 8B 44 24 28 48 8B D9 48 8D 0D 62", NullStub); // 88F2B80
		// if false it creates some useless calls
		mod->Redirect("40 53 48 83 EC 20 48 8B 44 24 28 48 8B D9 48 8D 0D 5B", ReturnStub<bool, true>); // 88F5390


		// registry function
		mod->Redirect("40 53 48 83 EC 20 48 8B 44 24 28 48 8B D9 48 8D 0D 6B", Bo6LoadLuaPop); // 88F4E80
		mod->Redirect("48 89 5C 24 10 57 48 83 EC 20 48 8B 44 24 28 48 8B D9 48 8D 0D 47", Bo6LoadLuaPush); // 88F2FA0

		void* stub{ mod->ScanSingle("E8 ? ? ? ? 48 BA 34 DB FC 73 86 28 B6 F9").GetRelative<int32_t>(1) };
		void* stubStr{ mod->ScanSingle("E8 ? ? ? ? 4C 8D 05 ? ? ? ? 48 8D 15 ? ? ? ? 49 8B CC").GetRelative<int32_t>(1) };

		LOG_INFO("redirect hash . {}", hook::library::CodePointer{ stub });
		LOG_INFO("redirect str .. {}", hook::library::CodePointer{ stubStr });

		hook::memory::RedirectJmp(
			stub,
			Bo6LoadLuaFunc
		);

		hook::memory::RedirectJmp(
			stubStr,
			Bo6LoadLuaFuncStr
		);

		mod->ScanSingle("E9 EC AA 0A 00").GetPtr<void(*)(void*)>()(nullptr);

		utils::OutFileCE os{ argv[3] };

		if (!os) {
			LOG_ERROR("Can't open {}", argv[3]);
			return tool::BASIC_ERROR;
		}

		LOG_INFO("Dumping functions in {}", argv[3]);

		os << "object,name,offset";

		for (auto& [cat, funcs] : bo6LuaData.funcs) {
			for (auto& [name, func] : funcs) {
				os << "\n" << cat << "," << hashutils::ExtractTmp("hash", name) << "," << hook::library::CodePointer{func};
			}
		}

		bo6LuaData.Cleanup();


		return tool::OK;
	}

	int iw_data_dump(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 1)) return tool::BAD_USAGE;


		std::filesystem::path exe{ argv[2] };

		hook::module_mapper::Module mod{ true };

		LOG_INFO("Loading module {}", exe.string());
		if (!mod.Load(exe) || !mod) {
			LOG_ERROR("Can't load module");
			return tool::BASIC_ERROR;
		}

		LOG_INFO("Loaded");
		LOG_INFO("base: {:x}", mod->GetOptHeader()->ImageBase);

		// Dump bo6 pool names
		// I know it not that, but it's fine idc
		struct PoolInfo {
			uint32_t size;
			uint32_t unk04;
			uint64_t unk08;
			uint64_t unk10;
			uint64_t unk18;
			uint64_t unk20;
			uint64_t unk28;
		};
		auto* pools{ mod->Get<uintptr_t>(0x8FFBE40) };
		auto* poolsSizes{ mod->Get<PoolInfo>(0x8ED5FF0) };
		{
			std::filesystem::path pf{ exe.parent_path() / "pools.csv" };
			std::ofstream osf{ pf };
			if (!osf) {
				LOG_ERROR("Can't open {}", pf.string());
			}
			else {
				utils::CloseEnd osfce{ osf };
				osf << "id,name,size";
				for (size_t idx = 0; idx < bo6::T10R_ASSET_COUNT; idx++) {
					const char* cc = mod->Rebase<const char>(pools[idx]);
					osf << "\n" << std::dec << idx << "," << cc << ",0x" << std::hex << poolsSizes[idx].size;
				}
			}
			LOG_INFO("Created {}", pf.string());
		}

		struct GscObj {
			byte magic[0x58];
		};
		struct builtin_func {
			uint64_t name;
			void* func;
		};

		struct gscvm_builtins {
			builtin_func builtinsF[2200];
			builtin_func builtinsM[3200];
			uint32_t countFunc;
			uint32_t countMethod;
			bool funcSorted;
			bool methodSorted;
		};


		struct gscvm2 {
			gscvm_builtins* builtins;
			// ...
		};


		struct GscInfo {
			GscObj* obj;
			uint32_t count;
			uint32_t group;
		};

		struct gscvm {
			gscvm2* vm2;
			uint64_t unk08;
			GscInfo* objInfo;
			uint32_t objCount;
			uint32_t group_id;
			GscObj default_obj;
		};

		struct gscvm3 {
			byte pad[2510576];
			gscvm vm;
		};

		auto* gscVm{ mod->Get<gscvm3>(0xC4096A0) };
		LOG_INFO("gscVm: {:x}", mod->Rloc(gscVm));
		auto* vm2{ mod->Rebase(gscVm->vm.vm2) };
		LOG_INFO("vm2: {:x}", mod->Rloc(vm2));
		auto* builtins{ mod->Rebase(vm2->builtins) };
		LOG_INFO("builtins: {:x}", mod->Rloc(builtins));
		{
			std::filesystem::path pf{ exe.parent_path() / "funcs.csv" };
			std::ofstream osf{ pf };
			if (!osf) {
				LOG_ERROR("Can't open {}", pf.string());
			}
			else {
				utils::CloseEnd osfce{ osf };
				osf << "name,type,ptr";
				for (size_t idx = 0; idx < builtins->countFunc; idx++) {
					auto bi{ builtins->builtinsF[idx] };

					osf << "\n" << hashutils::ExtractTmp("function", bi.name) << ",function," << mod->RlocBased(bi.func);
				}
				for (size_t idx = 0; idx < builtins->countMethod; idx++) {
					auto bi{ builtins->builtinsM[idx] };

					osf << "\n" << hashutils::ExtractTmp("function", bi.name) << ",function," << mod->RlocBased(bi.func);
				}
			}
			LOG_INFO("Created {}", pf.string());
		}



		LOG_INFO("Done");

		return tool::OK;
	}

	bool HasEnoughInternet(const char* website = "https://example.org") {
		//std::string tmp{};
		//try {
		//	return utils::io::DownloadFile(website, tmp, false);
		//}
		//catch (std::runtime_error& e) {
		//	LOG_TRACE("HasEnoughInternet err: {}", e.what());
		//	return false;
		//}
		return false;
	}

	int test_enough_internet(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 1)) return tool::BAD_USAGE;
		LOG_INFO("HasEnoughInternet() = {}", HasEnoughInternet(argv[2]));
		return tool::OK;
	}

	int scripts_decrypt(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 4)) return tool::BAD_USAGE;

		const char* exe{ argv[2] };
		VmInfo* nfo{};
		VMId type{ VMOf(argv[3]) };
		const char* scripts{ argv[4] };
		const char* output{ argv[5] };

		if (!type || !IsValidVm(type, nfo)) {
			LOG_ERROR("Can't find type {}", argv[3]);
			return tool::BAD_USAGE;
		}
		std::vector<std::filesystem::path> paths{};

		utils::GetFileRecurse(scripts, paths, [](const std::filesystem::path& p) {
			auto s = p.string();
			return s.ends_with(".gscc") || s.ends_with(".cscc") || s.ends_with(".gscbin") || s.ends_with(".cscbin");
			});

		if (paths.empty()) {
			LOG_ERROR("Not gsc files found in {}", scripts);
			return tool::BASIC_ERROR;
		}

		hook::module_mapper::Module mod{ true };

		auto LoadMod = [&mod, exe](bool hasAC) -> bool {
			if (hasAC && HasEnoughInternet()) {
				LOG_ERROR("Reminder: DO NOT MAP EXE WITH AC WHILE BEING CONNECTED TO INTERNET");
				return false;
			}
			LOG_INFO("Loading module {}", exe);
			if (!mod.Load(exe)) {
				LOG_ERROR("Can't load module");
				return false;
			}
			return true;
			};

		std::function<char* (char*)> DecryptString;

		switch (type) {
		case VMI_T7_1C:
		case VMI_T7_1B:
		case VMI_T8_36:
		case VMI_JUP_8A: {
			LOG_WARNING("Decryption useless with {}", nfo->name);
			return tool::OK; // nothing to do
		}
		case VMI_T9_38: {
			if (!LoadMod(false)) return tool::BASIC_ERROR;
			auto DecryptStringFunc = mod->ScanSingle("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 54 41 55 41 56 41 57 48 83 EC ? 48 8B D9 0F B6", "DecryptString").GetPtr<char* (*)(char* str)>();
			DecryptString = [DecryptStringFunc](char* s) -> char* {
				char* sd = DecryptStringFunc(s);

				// save the decrypted string at the same location
				memmove(s, sd, std::strlen(sd) + 1);

				return s;
				};
			break;
		}
		case VMI_JUP_8B:
		case VMI_T10_06:
		case VMI_T10_07:
		case VMI_T10_08:
		case VMI_T10_09:
		case VMI_T10_0A:
		case VMI_T10_0B:
		case VMI_T10_0C:
		case VMI_T10_0D:
		case VMI_T10_0E:
		case VMI_T10_10:
		case VMI_T10_11:
		case VMI_T10_12: {
			if (!LoadMod(true)) return tool::BASIC_ERROR;
			auto DecryptStringFunc = mod->ScanSingle("48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 41 54 41 55 41 56 41 57 48 83 EC 20 0F B6 01", "DecryptString").GetPtr<char* (*)(char* str)>();
			DecryptString = [DecryptStringFunc](char* s) -> char* {
				char* sd = DecryptStringFunc(s);

				// save the decrypted string at the same location
				memmove(s, sd, std::strlen(sd) + 1);

				return s;
				};
			break;
		}
		default:
			LOG_ERROR("No module information about {} are registered", nfo->name);
			return tool::BASIC_ERROR;
		}
		LOG_INFO("Module loaded");

		hashutils::ReadDefaultFile();

		std::filesystem::path outdir{ output };

		std::string buffer{};
		size_t c{};
		for (const std::filesystem::path& path : paths) {
			if (!utils::ReadFile(path, buffer) || buffer.size() < sizeof(uint64_t)) {
				LOG_ERROR("Can't read {}", path.string());
				continue; // ignore
			}

			uint64_t magic{ *(uint64_t*)buffer.data() };

			VmInfo* nfo{};

			if (!IsValidVmMagic(magic, nfo)) {
				LOG_ERROR("Can't read {}: Unknown magic {:x}", path.string(), magic);
				continue; // ignore
			}

			auto* hbuilder{ tool::gsc::GetGscReader(nfo->vmMagic) };

			if (!hbuilder) {
				LOG_ERROR("Can't read {}: No gsc handler builder for vm {}", path.string(), nfo->name);
				continue;
			}
			auto handler{ (*hbuilder)((byte*)buffer.data(), buffer.size()) };

			if (!handler->IsValidHeader(buffer.size())) {
				LOG_ERROR("Can't read {}: Invalid header", path.string());
				continue;
			}

			size_t strscount{ handler->GetStringsCount() };
			size_t animtcount{ handler->GetAnimTreeDoubleCount() };
			size_t animtucount{ handler->GetAnimTreeSingleCount() };

			LOG_DEBUG("Decrypting {} ({}) ({}/{}/{})...", path.string(), hashutils::ExtractTmpScript(handler->GetName()), strscount, animtcount, animtucount);

			// strings table

			if (strscount) {
				tool::gsc::T8GSCString* strs{ handler->Ptr<tool::gsc::T8GSCString>(handler->GetStringsOffset()) };
				size_t i = 0;
				for (; i < strscount; i++) {
					size_t rloc{ (size_t)((byte*)strs - handler->Ptr()) };


					if (rloc + sizeof(*strs) + sizeof(uint32_t) * strs->num_address > buffer.size() || strs->string >= buffer.size()) {
						LOG_ERROR("Can't decrypt {}: String too far 0x{:x} >= {:x}", path.string(), strs->string, buffer.size());
						break;
					}
					DecryptString(handler->Ptr<char>(strs->string));

					strs = (tool::gsc::T8GSCString*)&((uint32_t*)(&strs[1]))[strs->num_address];
				}
				if (i != strscount) {
					continue;
				}
				LOG_TRACE("{} string(s) decrypted", i);
			}

			// animtrees tables


			if (animtcount) {
				tool::gsc::GSC_ANIMTREE_ITEM* animt{ handler->Ptr<tool::gsc::GSC_ANIMTREE_ITEM>(handler->GetAnimTreeDoubleOffset()) };
				size_t j = 0;
				for (; j < animtcount; j++) {
					size_t rloc{ (size_t)((byte*)animt - handler->Ptr()) };
					if (rloc + sizeof(*animt) + sizeof(uint32_t) * animt->num_address > buffer.size() || animt->address_str1 >= buffer.size() || animt->address_str2 >= buffer.size()) {
						LOG_ERROR("Can't decrypt {}: Anim2 too far", path.string());
						break;
					}
					DecryptString(handler->Ptr<char>(animt->address_str1));
					DecryptString(handler->Ptr<char>(animt->address_str2));

					animt = reinterpret_cast<tool::gsc::GSC_ANIMTREE_ITEM*>(reinterpret_cast<uint32_t*>(&animt[1]) + animt->num_address);
				}
				if (j != animtcount) {
					continue;
				}
				LOG_TRACE("{} anim2(s) decrypted", j);
			}

			if (animtucount) {
				tool::gsc::GSC_USEANIMTREE_ITEM* animtu{ handler->Ptr<tool::gsc::GSC_USEANIMTREE_ITEM>(handler->GetAnimTreeSingleOffset()) };
				size_t k = 0;
				for (; k < animtucount; k++) {
					size_t rloc{ (size_t)((byte*)animtu - handler->Ptr()) };
					if (rloc + sizeof(*animtu) + sizeof(uint32_t) * animtu->num_address > buffer.size() || animtu->address >= buffer.size()) {
						LOG_ERROR("Can't decrypt {}: Anim1 too far", path.string());
						break;
					}

					DecryptString(handler->Ptr<char>(animtu->address));

					animtu = reinterpret_cast<tool::gsc::GSC_USEANIMTREE_ITEM*>(reinterpret_cast<uint32_t*>(&animtu[1]) + animtu->num_address);
				}
				if (k != animtucount) {
					continue;
				}
				LOG_TRACE("{} anim1(s) decrypted", k);
			}

			std::filesystem::path outdirvm{ outdir / tool::gsc::opcode::VMIdFancyName(nfo->vmMagic) };
			std::filesystem::create_directories(outdirvm);
			std::filesystem::path outfile{ outdirvm / utils::va("script_%llx.gscc", handler->GetName()) };

			if (!utils::WriteFile(outfile, buffer)) {
				LOG_ERROR("Can't write to '{}'", outfile.string());
				continue;
			}

			size_t total{ strscount + animtucount + animtcount };
			LOG_INFO("Decrypted {} ({}) into {} / {} (0x{:x}) string(s)", path.string(), hashutils::ExtractTmpScript(handler->GetName()), outfile.string(), total, total);
			c++;
		}
		LOG_INFO("{} (0x{:x}) script(s) decrypted", c, c);

		return tool::OK;
	}

	constexpr bool IsValidChar(char c) {
		return (c >= 0x20 && c < 0x7F) || c == '\n' || c == '\r' || c == '\t';
	}

	int read_strings(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 2)) {
			return tool::BAD_USAGE;
		}

		std::filesystem::path exe{ argv[2] };
		std::filesystem::path output{ argv[3] };
		int64_t minSizeOpt{ tool::NotEnoughParam(argc, 3) ? 4 : utils::ParseFormatInt(argv[4]) };

		if (minSizeOpt < 1) {
			LOG_ERROR("Invalid min size {}", minSizeOpt);
			return tool::BASIC_ERROR;
		}
		size_t minSize{ (size_t)minSizeOpt };

		LOG_INFO("Loading {} -> {} with min size {}", exe.string(), output.string(), minSize);

		std::string file{};

		if (!utils::ReadFile(exe, file)) {
			LOG_ERROR("Can't load file {}", exe.string());
			return tool::BASIC_ERROR;
		}
		{
			std::ofstream os{ output };
			if (!os) {
				LOG_ERROR("Can't open output {}", output.string());
				return tool::BASIC_ERROR;
			}
			utils::CloseEnd osce{ os };

			char* start{ file.data() };
			char* ptr{ file.data() + file.size() - 1 };

			while (ptr > start) {
				if (*ptr) {
					ptr--;
					continue;
				}

				size_t len{};

				while (ptr > start && IsValidChar(ptr[-1])) {
					ptr--;
					len++;
				}

				if (len >= minSize) {
					os << ptr << "\n";
				}

				ptr--;
			}
		}
		LOG_INFO("Dump into {}", output.string());

		return tool::OK;
	}

	int exe_scan(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 2)) return tool::BAD_USAGE;

		const char* exe{ argv[2] };
		const char* pattern{ argv[3] };

		hook::module_mapper::Module mod{ true };

		if (!mod.Load(exe, false)) {
			LOG_ERROR("no load");
			return tool::BASIC_ERROR;
		}

		for (auto& patt : mod->Scan(pattern)) {
			LOG_INFO("{} -> {}", pattern, (void*)patt.location);
		}

		return tool::OK;
	}


	ADD_TOOL(exe_mapper, "dev", "[exe]", "Map exe in memory", exe_mapper);
	ADD_TOOL(exe_scan, "dev", "[exe] [pattern]", "Scan exe", exe_scan);
	ADD_TOOL(read_strings, "dev", "[file] [output] (min size=4)", "Dump file strings", read_strings);
	ADD_TOOL(bo6_data_dump, "bo6", "[exe]", "Dump common data from an exe dump", iw_data_dump);
	ADD_TOOL(bo6_gsc_dump, "bo6", "[exe]", "Dump gsc function data from an exe dump", bo6_gsc_dump);
	ADD_TOOL(bo6_lua_dump, "bo6", "[exe]", "Dump lua function data from an exe dump", bo6_lua_dump);
	ADD_TOOL(exe_pool_dumper, "common", "[exe] [start] [end] (outfile) (prefix)", "Dump pool names", exe_pool_dumper);
	ADD_TOOL(sp24_data_dump, "bo6", "[exe]", "Dump common data from an exe dump", sp24_data_dump);
	ADD_TOOL(scripts_decrypt, "gsc", "[exe] [type] [scripts] [ouput]", "Map exe in memory and use it to decrypt GSC scripts", scripts_decrypt);
	ADD_TOOL(test_enough_internet, "dev", "[url]", "Test can load", test_enough_internet);


}