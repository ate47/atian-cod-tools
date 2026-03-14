#include <dll_includes.hpp>
#include <data/refs.hpp>
#include <core/config.hpp>
#include <core/system.hpp>
#include <core/hashes/hash_store.hpp>
#include <core/hashes/hash_lookup.hpp>
#include <core/updater.hpp>
#include <hook/library.hpp>
#include <systems/mods.hpp>

namespace systems::lua {
	namespace {
		hook::library::Detour Lua_CoD_RegisterEngineFunctions_Hook;

		template<core::logs::loglevel lvl>
		int Lua_Acts_Function_Log(bo4::lua_State* luaVM) {
			bo4::HksObject* base{ luaVM->m_apistack.base };
			bo4::HksObject* top{ luaVM->m_apistack.top };
			const char* msg{ "" };
			if (&base[0] < top) {
				msg = bo4::hks_obj_tolstring(luaVM, base, nullptr);
			}
			LOG_LVLF(lvl, "[ActsLog][LUA] {}", msg);
			return 0;
		}

		int Lua_Acts_Function_HashLookup(bo4::lua_State* luaVM) {
			bo4::HksObject* base{ luaVM->m_apistack.base };
			bo4::HksObject* top{ luaVM->m_apistack.top };
			uint64_t h{};
			if (base < top) {
				h = base->v.hash;
			}

			const char* v{ core::hashes::ExtractPtr(h) };

			if (v) {
				bo4::hksi_lua_pushlstring(luaVM, v, std::strlen(v));
			}
			else {
				bo4::HksObject* hv{ luaVM->m_apistack.top++ };
				hv->t = bo4::HKST_TXHASH;
				hv->v.hash = h;
			}

			return 1;
		}

		int Lua_Acts_Function_GetVersion(bo4::lua_State* luaVM) {
			const char* v{ core::updater::GetVersionName(core::actsinfo::VERSION_ID) };
			bo4::hksi_lua_pushlstring(luaVM, v, std::strlen(v));
			return 1;
		}

		inline void RegisterFunction(bo4::lua_State* state, const char* name, int(*func)(bo4::lua_State* s)) {
			XHash hash{ hash::Hash64(name) };

			bo4::Lua_Cod_RegisterFunction(state, &hash, func);
		}

		void Lua_CoD_RegisterActsFunction(bo4::lua_State* state) {
			bo4::Lua_BeginTableReadOnly("GlobalActsMetatable", state);

			RegisterFunction(state, "Log", Lua_Acts_Function_Log<core::logs::loglevel::LVL_INFO>);
			RegisterFunction(state, "Error", Lua_Acts_Function_Log<core::logs::loglevel::LVL_ERROR>);
			RegisterFunction(state, "Warning", Lua_Acts_Function_Log<core::logs::loglevel::LVL_WARNING>);
			RegisterFunction(state, "HashLookup", Lua_Acts_Function_HashLookup);
			RegisterFunction(state, "GetVersion", Lua_Acts_Function_GetVersion);

			bo4::Lua_EndTableReadOnly(state);
			bo4::hksi_lua_setfield(state, bo4::LUA_GLOBALS_INDEX, "Acts");
		}

		void Lua_CoD_RegisterEngineFunctions_Stub(bo4::lua_State* state) {
			Lua_CoD_RegisterActsFunction(state);
			Lua_CoD_RegisterEngineFunctions_Hook.Call(state);
		}

		void PostInit(uint64_t uid) {
			Lua_CoD_RegisterEngineFunctions_Hook.Create(0x3911C00_a, Lua_CoD_RegisterEngineFunctions_Stub);
		}
		REGISTER_SYSTEM(lua_funcs, nullptr, PostInit);
	}
}