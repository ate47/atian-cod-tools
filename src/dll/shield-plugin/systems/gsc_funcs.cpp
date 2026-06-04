#include <dll_includes.hpp>
#include <data/refs.hpp>
#include <core/config.hpp>
#include <core/hashes/hash_store.hpp>
#include <core/system.hpp>
#include <hook/memory.hpp>
#include <hook/library.hpp>
#include <systems/errors.hpp>
#include <systems/gsc_link.hpp>
#include <systems/gsc_funcs.hpp>
#include <systems/gsc_debug.hpp>
#include <utils/decrypt.hpp>
#include <magic_enum/magic_enum.hpp>

namespace systems::gsc::funcs {
	namespace {

		const char* Scr_Render(bo4::scriptInstance_t inst, size_t index) {
			bo4::ScrVarType_t type{ bo4::ScrVm_GetType(inst, index) };
			switch (type) {
			case bo4::TYPE_UNDEFINED:
				return "undefined";
				break;
			case bo4::TYPE_STRING:
				return utils::va("%s", bo4::ScrVm_GetString(inst, index));
			case bo4::TYPE_VECTOR: {
				bo4::vec3_t vec{};
				bo4::ScrVm_GetVector(inst, index, &vec);
				return utils::va("(%f, %f, %f)", vec[0], vec[1], vec[2]);
			}
			case bo4::TYPE_HASH: {
				XHash out{ *bo4::ScrVm_GetHash(&out, inst, index) };
				const char* post{ "" };
				if (bo4::DB_DoesXAssetExist(bo4::XAssetType::ASSET_TYPE_LOCALIZE_ENTRY, &out)) {
					bo4::LocalizeEntry* loc{ bo4::DB_FindXAssetHeader(bo4::XAssetType::ASSET_TYPE_LOCALIZE_ENTRY, &out, false, 0).localize };
					if (loc && loc->value) {
						post = utils::va(" ('%s')", decrypt::DecryptString((char*)loc->value));
					}
				}
				return utils::va("#\"%s\"%s", core::hashes::ExtractTmp("hash", out), post);
			}
			case bo4::TYPE_FLOAT:
				return utils::va("%f", bo4::ScrVm_GetFloat(inst, index));
			case bo4::TYPE_INTEGER:
				return utils::va("%lld", bo4::ScrVm_GetInt(inst, index));
			case bo4::TYPE_API_FUNCTION: {
				byte* codePos{ bo4::ScrVm_GetValue(inst, index)->u.codePosValue };
				bool func{};
				uint32_t name{};
				bool found;
				if (inst) {
					found = bo4::CScr_GetFunctionReverseLookup(codePos, &name, &func);
				}
				else {
					found = bo4::Scr_GetFunctionReverseLookup(codePos, &name, &func);
				}
				if (!found) {
					return utils::va("api[unk:%p]", (void*)codePos);
				}

				const char* apitype{ func ? "function" : "method", };
				return utils::va("api%s[%s]", apitype, core::hashes::ExtractTmp(apitype, name));
			}
			case bo4::TYPE_SCRIPT_FUNCTION: {
				byte* codePos{ bo4::ScrVm_GetValue(inst, index)->u.codePosValue };
				const char* scriptname{ "<error>" };
				bo4::Scr_GetGscExportInfo(inst, codePos, &scriptname, nullptr, nullptr, nullptr);
				return utils::va("script[%s]", scriptname);
			}
			case bo4::TYPE_POINTER: {
				bo4::ScrVarType_t ptrType{ bo4::ScrVm_GetPointerType(inst, index) };
				if (ptrType == bo4::TYPE_ENTITY) {
					bo4::scr_entref_t entref{ *bo4::ScrVm_GetEntityRef(&entref, inst, index) };
					std::string name{ magic_enum::enum_name(entref.classnum) };
					if (entref.classnum == bo4::CLASS_NUM_HUDELEM) {
						return systems::gsc::debug::RenderHudElemField(entref.u.hudElemIndex);
					}
					return utils::va("entity[%s,num=%d,val=%lld]", name.data(), (int)entref.client, entref.u.val);
				}
				return utils::va("ptr[%s]", bo4::scrVarTypeNames[ptrType]);
			}
			default:
				return utils::va("unk[%s]", bo4::scrVarTypeNames[type]);
			}
		}

		void Scr_ActsLog(bo4::scriptInstance_t inst) {
			const char* prefix{ inst ? "[ActsLog][CSC]" : "[ActsLog][GSC]" };
			std::stringstream ss{};
			for (size_t i = 0; i < bo4::ScrVm_GetNumParam(inst); i++) {
				if (i) {
					ss << " ";
				}
				ss << Scr_Render(inst, i);
			}
			LOG_INFO("{} {}", prefix, ss.str());
		}

		void Scr_ActsHashLookup(bo4::scriptInstance_t inst) {
			bo4::ScrVarType_t type{ bo4::ScrVm_GetType(inst, 0) };

			if (type == bo4::TYPE_HASH) {
				XHash hash{ *bo4::ScrVm_GetHash(&hash, inst, 0) };
				const char* str{ core::hashes::ExtractPtr(hash) };

				if (str) {
					bo4::ScrVm_AddString(inst, str);
				}
				else {
					bo4::ScrVm_AddHash(inst, &hash);
				}
			}
			else if (type == bo4::TYPE_STRING) {
				// redirect str
				bo4::ScrVm_AddConstString(inst, bo4::ScrVm_GetConstString(inst, 0));
			}
			else if (type != bo4::TYPE_UNDEFINED) {
				systems::errors::ScrVm_Error(inst, "Invalid Hash lookup type '%s'", false, bo4::scrVarTypeNames[type]);
			}
		}

		void Scr_ActsGetDetour(bo4::scriptInstance_t inst) {
			// ActsGetDetour(script, nsp, func)
			XHash script{ *bo4::ScrVm_GetHash(&script, inst, 0) };
			uint32_t nsp{ ScrVm_GetHashScr(inst, 1) };
			uint32_t func{ ScrVm_GetHashScr(inst, 2) };

			byte* codePos{ systems::gsc::link::Scr_FindDetour(inst, nullptr, script, nsp, func) };
			if (codePos) {
				bo4::ScrVm_AddScriptFunction(inst, codePos);
			}
			else {
				bo4::ScrVm_AddUndefined(inst);
			}
		}

		struct FunctionContainer {
			bo4::BuiltinFunctionDef* array;
			size_t len;
		};

		constexpr size_t maxContainer = 8;

		class GscContainer {
			FunctionContainer functions[maxContainer]{};
			FunctionContainer methods[maxContainer]{};
			size_t numFunctions{};
			size_t numMethods{};

		public:
			GscContainer() {}

			bo4::BuiltinFunctionDef* FindDef(uint32_t canonId, bool isMethod) {
				FunctionContainer* containers{ isMethod ? methods : functions };
				size_t len{ isMethod ? numMethods : numFunctions };

				for (size_t i = 0; i < len; i++) {
					const FunctionContainer& c{ containers[i] };
					for (size_t i = 0; i < c.len; i++) {
						if (c.array[i].canonId == canonId) {
							return &c.array[i];
						}
					}
				}
				return nullptr;
			}


			bo4::BuiltinFunctionDef* FindRef(void* func, bool isMethod) const {
				const FunctionContainer* containers{ isMethod ? methods : functions };
				size_t len{ isMethod ? numMethods : numFunctions };

				for (size_t i = 0; i < len; i++) {
					const FunctionContainer& c{ containers[i] };
					for (size_t i = 0; i < c.len; i++) {
						if (c.array[i].actionFunc == func) {
							return &c.array[i];
						}
					}
				}
				return nullptr;
			}

			bool LookupRef(void* func, uint32_t* hash, bool* isFunction) const {
				bo4::BuiltinFunctionDef* f{ FindRef(func, false) };
				if (f) {
					if (isFunction) *isFunction = true;
					if (hash) *hash = f->canonId;
					return true;
				}
				bo4::BuiltinFunctionDef* m{ FindRef(func, true) };
				if (m) {
					if (isFunction) *isFunction = false;
					if (hash) *hash = m->canonId;
					return true;
				}
				return false;
			}

			void RegisterFunctionContainer(bool isMethods, bo4::BuiltinFunctionDef* array, size_t len) {
				FunctionContainer* containers{ isMethods ? methods : functions };
				size_t& count{ isMethods ? numMethods : numFunctions };
				if (count == maxContainer) {
					throw std::runtime_error(std::format("Can't register more than {} containers", maxContainer));
				}

				FunctionContainer& c{ containers[count++] };
				c.array = array;
				c.len = len;
			}
		};

		GscContainer customContainerGsc[bo4::scriptInstance_t::SI_COUNT]{};

		std::array actsBaseGscFunctions{
			bo4::BuiltinFunctionDef{ // ActsLog(message)
				.canonId = hash::HashT89Scr("ActsLog"),
				.min_args = 1,
				.max_args = INT_MAX,
				.actionFunc = Scr_ActsLog,
				.type = bo4::BUILTIN_DEFAULT,
			},
			bo4::BuiltinFunctionDef{ // ActsHashLookup(hash)
				.canonId = hash::HashT89Scr("ActsHashLookup"),
				.min_args = 1,
				.max_args = 1,
				.actionFunc = Scr_ActsHashLookup,
				.type = bo4::BUILTIN_DEFAULT,
			},
			bo4::BuiltinFunctionDef{ // ActsGetDetour(script, nsp, func)
				.canonId = hash::HashT89Scr("ActsGetDetour"),
				.min_args = 3,
				.max_args = 3,
				.actionFunc = Scr_ActsGetDetour,
				.type = bo4::BUILTIN_DEFAULT,
			},
		};

		std::array actsBaseCscFunctions{
			bo4::BuiltinFunctionDef{ // ActsLog(message)
				.canonId = hash::HashT89Scr("ActsLog"),
				.min_args = 1,
				.max_args = 1,
				.actionFunc = Scr_ActsLog,
				.type = bo4::BUILTIN_DEFAULT,
			},
			bo4::BuiltinFunctionDef{ // ActsHashLookup(hash)
				.canonId = hash::HashT89Scr("ActsHashLookup"),
				.min_args = 1,
				.max_args = 1,
				.actionFunc = Scr_ActsHashLookup,
				.type = bo4::BUILTIN_DEFAULT,
			},
			bo4::BuiltinFunctionDef{ // ActsGetDetour(script, nsp, func)
				.canonId = hash::HashT89Scr("ActsGetDetour"),
				.min_args = 3,
				.max_args = 3,
				.actionFunc = Scr_ActsGetDetour,
				.type = bo4::BUILTIN_DEFAULT,
			},
		};

		hook::library::Detour Scr_GetFunction_Detour;
		hook::library::Detour Scr_GetFunctionReverseLookup_Detour;
		hook::library::Detour Scr_GetMethod_Detour;
		hook::library::Detour Scr_GetMethodReverseLookup_Detour;
		hook::library::Detour CScr_GetFunction_Detour;
		hook::library::Detour CScr_GetFunctionReverseLookup_Detour;
		hook::library::Detour CScr_GetMethod_Detour;
		hook::library::Detour CScr_GetMethodReverseLookup_Detour;

		bool Scr_GetFunctionReverseLookup_Stub(void* func, uint32_t* hash, bool* isFunction) {
			if (Scr_GetFunctionReverseLookup_Detour.Call<bool>(func, hash, isFunction)) {
				return true;
			}

			return customContainerGsc[bo4::scriptInstance_t::SI_SERVER].LookupRef(func, hash, isFunction);
		}

		bool CScr_GetFunctionReverseLookup_Stub(void* func, uint32_t* hash, bool* isFunction) {
			if (CScr_GetFunctionReverseLookup_Detour.Call<bool>(func, hash, isFunction)) {
				return true;
			}

			return customContainerGsc[bo4::scriptInstance_t::SI_CLIENT].LookupRef(func, hash, isFunction);
		}

		void* Scr_GetFunction_Stub(uint32_t name, int32_t* type, int32_t* min_args, int32_t* max_args) {
			void* func = Scr_GetFunction_Detour.Call<void*>(name, type, min_args, max_args);

			if (func) {
				return func;
			}

			const bo4::BuiltinFunctionDef* f{ customContainerGsc[bo4::scriptInstance_t::SI_SERVER].FindDef(name, false) };

			if (f) {
				*type = f->type;
				*min_args = f->min_args;
				*max_args = f->max_args;

				return f->actionFunc;
			}

			return nullptr;
		}
		void* CScr_GetFunction_Stub(uint32_t name, int32_t* type, int32_t* min_args, int32_t* max_args) {
			void* func = CScr_GetFunction_Detour.Call<void*>(name, type, min_args, max_args);

			if (func) {
				return func;
			}

			const bo4::BuiltinFunctionDef* f{ customContainerGsc[bo4::scriptInstance_t::SI_CLIENT].FindDef(name, false) };

			if (f) {
				*type = f->type;
				*min_args = f->min_args;
				*max_args = f->max_args;

				return f->actionFunc;
			}

			return nullptr;
		}

		void* Scr_GetMethod_Stub(uint32_t name, int32_t* type, int32_t* min_args, int32_t* max_args) {
			void* func = Scr_GetMethod_Detour.Call<void*>(name, type, min_args, max_args);

			if (func) {
				return func;
			}

			const bo4::BuiltinFunctionDef* m{ customContainerGsc[bo4::scriptInstance_t::SI_SERVER].FindDef(name, true) };

			if (m) {
				*type = m->type;
				*min_args = m->min_args;
				*max_args = m->max_args;

				return m->actionFunc;
			}

			return nullptr;
		}

		void* CScr_GetMethod_Stub(uint32_t name, int32_t* type, int32_t* min_args, int32_t* max_args) {
			void* func = CScr_GetMethod_Detour.Call<void*>(name, type, min_args, max_args);

			if (func) {
				return func;
			}

			const bo4::BuiltinFunctionDef* m{ customContainerGsc[bo4::scriptInstance_t::SI_CLIENT].FindDef(name, true) };

			if (m) {
				*type = m->type;
				*min_args = m->min_args;
				*max_args = m->max_args;

				return m->actionFunc;
			}

			return nullptr;
		}

		void PostInit(uint64_t uid) {
			ScrVm_RegisterFunctionContainer(bo4::scriptInstance_t::SI_SERVER, false, actsBaseGscFunctions);
			ScrVm_RegisterFunctionContainer(bo4::scriptInstance_t::SI_CLIENT, false, actsBaseCscFunctions);

			Scr_GetFunctionReverseLookup_Detour.Create(0x33AF8A0_a, Scr_GetFunctionReverseLookup_Stub);
			CScr_GetFunctionReverseLookup_Detour.Create(0x1F132A0_a, CScr_GetFunctionReverseLookup_Stub);
			Scr_GetFunction_Detour.Create(0x33AF840_a, Scr_GetFunction_Stub);
			CScr_GetFunction_Detour.Create(0x1F13140_a, CScr_GetFunction_Stub);
			CScr_GetMethod_Detour.Create(0x1F13650_a, CScr_GetMethod_Stub);
			Scr_GetMethod_Detour.Create(0x33AFC20_a, Scr_GetMethod_Stub);
		}


		REGISTER_SYSTEM(gsc_funcs, nullptr, PostInit);
	}

	uint32_t ScrVm_GetHashScr(bo4::scriptInstance_t inst, unsigned int index) {
		if (bo4::ScrVm_GetType(inst, index) == bo4::TYPE_HASH) {
			XHash hash{};
			return (uint32_t)bo4::ScrVm_GetHash(&hash, inst, index)->name;
		}

		const char* str{ bo4::ScrVm_GetString(inst, index) };
		return hash::HashT89Scr(str);
	}

	void ScrVm_RegisterFunctionContainer(bo4::scriptInstance_t inst, bool isMethods, bo4::BuiltinFunctionDef* array, size_t len) {
		if (!len) {
			return; // empty container, useless
		}
		customContainerGsc[inst].RegisterFunctionContainer(isMethods, array, len);
	}
}