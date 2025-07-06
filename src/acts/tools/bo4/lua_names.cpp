#include <includes.hpp>
#include <hook/module_mapper.hpp>

namespace {

	int bo4luanames(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 1)) return tool::BAD_USAGE;

		hook::module_mapper::Module mod{ true };
		if (!mod.Load(argv[2], false)) {
			LOG_ERROR("Can't load module");
			return tool::BASIC_ERROR;
		}

		LOG_INFO("patching...");
		mod->NullFunc(0x390D3D0);
		mod->NullFunc(0x3962D30);
		mod->NullFunc(0x390D650);
		mod->NullFunc(0x390D490);
		mod->NullFunc(0x22C8E80);
		
		std::unordered_map<uint64_t, void*> ff{};

		mod->Redirect(0x3911980, [](std::unordered_map<uint64_t, void*>* funcs, uint64_t* xhash, void* func) {
			(*funcs)[*xhash] = func;
		});

		LOG_INFO("load funcs...");
		mod->Get<void(void*)>(0x3911C30)(&ff);

		std::filesystem::path funcs{ "output_bo4/luafuncs.csv" };

		utils::OutFileCE os{ funcs, true };

		os << "name,func";

		for (auto& [k, v] : ff) {
			os << "\n#" << hashutils::ExtractTmp("hash", k) << "," << hook::library::CodePointer{ v };
		}

		LOG_INFO("Loaded {} funcs into {}", ff.size(), funcs.string());

		return tool::OK;
	}

	ADD_TOOL(bo4luanames, "bo4", " [exe]", "dump lua funcs", bo4luanames);
}