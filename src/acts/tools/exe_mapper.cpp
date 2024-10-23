#include <includes.hpp>
#include <scriptinstance.hpp>
#include <hook/module_mapper.hpp>
#include <hook/error.hpp>
#include <decrypt.hpp>
#include "tools/gsc.hpp"

namespace {
	using namespace tool::gsc::opcode;
	int exe_mapper(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 1)) return tool::BAD_USAGE;


		const char* exe{ argv[2] };

		hook::module_mapper::Module mod{ true };

		LOG_INFO("Loading module {}", exe);
		if (!mod.Load(exe)) {
			LOG_ERROR("Can't load module");
			return tool::BASIC_ERROR;
		}

		LOG_INFO("Loaded");

		/*

		// TEST BO4 Dump data
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
            LOG_INFO("{} -> {:x}", hashutils::ExtractTmp("function", defs[i].name), defs[i].function);
        }

		*/

		LOG_INFO("Done");

		return tool::OK;
	}

	int scripts_decrypt(int argc, const char* argv[]) {
		LOG_WARNING("Reminder: DO NOT MAP EXE WITH AC WHILE BEING CONNECTED TO INTERNET");

		if (tool::NotEnoughParam(argc, 4)) return tool::BAD_USAGE;

		const char* exe{ argv[2] };
		VmInfo* nfo{};
		VM type{ VMOf(argv[3]) };
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

		auto LoadMod = [&mod, exe]() -> bool {
			LOG_INFO("Loading module {}", exe);
			if (!mod.Load(exe)) {
				LOG_ERROR("Can't load module");
				return false;
			}
			return true;
		};

		std::function<char*(char*)> DecryptString;
		
		switch (type) {
		case VM_T7:
		case VM_T71B:
		case VM_T8:
		case VM_MW23: {
			LOG_WARNING("Decryption useless with {}", nfo->name);
			return tool::OK; // nothing to do
		}
		case VM_T9: {
			if (!LoadMod()) return tool::BASIC_ERROR;
			auto DecryptStringFunc = mod->ScanSingle("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 54 41 55 41 56 41 57 48 83 EC ? 48 8B D9 0F B6", "DecryptString").GetPtr<char*(*)(char* str)>();
			DecryptString = [DecryptStringFunc](char* s) -> char* { 
				char* sd = DecryptStringFunc(s);

				// save the decrypted string at the same location
				memmove(s, sd, std::strlen(sd));
				
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

			auto* hbuilder{ tool::gsc::GetGscReader(nfo->vm) };

			if (!hbuilder) {
				LOG_ERROR("Can't read {}: No gsc handler builder for vm {}", path.string(), nfo->name);
				continue;
			}
			auto handler{ (*hbuilder)((byte*)buffer.data(), buffer.size()) };

			if (!handler->IsValidHeader(buffer.size())) {
				LOG_ERROR("Can't read {}: Invalid header", path.string());
				continue;
			}
			LOG_DEBUG("Decrypting {} ({})...", path.string(), hashutils::ExtractTmpScript(handler->GetName()));

			// strings table

			tool::gsc::T8GSCString* strs{ handler->Ptr<tool::gsc::T8GSCString>(handler->GetStringsOffset()) };
			size_t strscount{ handler->GetStringsCount() };

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

			// animtrees tables

			tool::gsc::GSC_ANIMTREE_ITEM* animt{ handler->Ptr<tool::gsc::GSC_ANIMTREE_ITEM>(handler->GetAnimTreeDoubleOffset()) };
			size_t animtcount{ handler->GetAnimTreeDoubleCount() };

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

			tool::gsc::GSC_USEANIMTREE_ITEM* animtu{ handler->Ptr<tool::gsc::GSC_USEANIMTREE_ITEM>(handler->GetAnimTreeSingleOffset()) };
			size_t animtucount{ handler->GetAnimTreeSingleCount() };

			size_t k = 0;
			for (; k < animtucount; k++) {
				size_t rloc{ (size_t)((byte*)animt - handler->file) };
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

			std::filesystem::path outdirvm{ outdir / utils::va("vm-%02x", nfo->vm) };
			std::filesystem::create_directories(outdirvm);
			std::filesystem::path outfile{ outdirvm / utils::va("script_%llx.gscc", handler->GetName()) };

			if (!utils::WriteFile(outfile, buffer)) {
				LOG_ERROR("Can't write to '{}'", outfile.string());
				continue;
			}

			size_t total{ i + j + k };
			LOG_INFO("Decrypted {} ({}) into {} / {} (0x{:x}) string(s)", path.string(), hashutils::ExtractTmpScript(handler->GetName()), outfile.string(), total, total);
			c++;
		}
		LOG_INFO("{} (0x{:x}) script(s) decrypted", c, c);

		return tool::OK;
	}


	ADD_TOOL(exe_mapper, "dev", "[exe]", "Map exe in memory", exe_mapper);
	ADD_TOOL(scripts_decrypt, "gsc", "[exe] [type] [scripts] [ouput]", "Map exe in memory and use it to decrypt GSC scripts", scripts_decrypt);

}