#include <includes.hpp>
#include <scriptinstance.hpp>
#include <io_utils.hpp>
#include <hook/module_mapper.hpp>
#include <hook/error.hpp>
#include <decrypt.hpp>
#include <DbgHelp.h>
#pragma comment(lib, "imagehlp.lib")
#include "tools/gsc.hpp"
#include "tools/pool.hpp"
#include "tools/bo6/bo6.hpp"

namespace {
	using namespace tool::gsc::opcode;
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
		auto* pools{ mod->Get<uintptr_t>(0x8FFBE40) };
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
		std::string tmp{};
		try {
			return utils::io::DownloadFile(website, tmp, false);
		}
		catch (std::runtime_error& e) {
			LOG_TRACE("HasEnoughInternet err: {}", e.what());
			return false;
		}
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

		std::function<char*(char*)> DecryptString;
		
		switch (type) {
		case VMI_T7:
		case VMI_T71B:
		case VMI_T8:
		case VMI_JUP_8A: {
			LOG_WARNING("Decryption useless with {}", nfo->name);
			return tool::OK; // nothing to do
		}
		case VMI_T9: {
			if (!LoadMod(false)) return tool::BASIC_ERROR;
			auto DecryptStringFunc = mod->ScanSingle("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 54 41 55 41 56 41 57 48 83 EC ? 48 8B D9 0F B6", "DecryptString").GetPtr<char*(*)(char* str)>();
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
		case VMI_T10_0B:
		case VMI_T10_0C: {
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
					size_t rloc{ (size_t)((byte*)strs - handler->file) };


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
					size_t rloc{ (size_t)((byte*)animt - handler->file) };
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
					size_t rloc{ (size_t)((byte*)animtu - handler->file) };
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

			std::filesystem::path outdirvm{ outdir / utils::va("vm-%llx", nfo->vmMagic) };
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


	ADD_TOOL(exe_mapper, "dev", "[exe]", "Map exe in memory", exe_mapper);
	ADD_TOOL(bo6_data_dump, "bo6", "[exe]", "Dump common data from an exe dump", iw_data_dump);
	ADD_TOOL(sp24_data_dump, "bo6", "[exe]", "Dump common data from an exe dump", sp24_data_dump);
	ADD_TOOL(scripts_decrypt, "gsc", "[exe] [type] [scripts] [ouput]", "Map exe in memory and use it to decrypt GSC scripts", scripts_decrypt);
	ADD_TOOL(test_enough_internet, "dev", "[url]", "Test can load", test_enough_internet);
	
}