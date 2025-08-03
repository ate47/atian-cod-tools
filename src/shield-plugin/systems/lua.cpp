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
		hook::library::Detour hksl_loadfile_Hook;
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
			bo4::hksi_lua_setfield(state, -10002, "Acts");
		}

		void Lua_CoD_RegisterEngineFunctions_Stub(bo4::lua_State* state) {
			Lua_CoD_RegisterActsFunction(state);
			Lua_CoD_RegisterEngineFunctions_Hook.Call(state);
		}

		struct LuaHook {
			const char* filename;
			uint64_t hash;
		};

		std::unordered_map<uint64_t, std::vector<LuaHook>> luaHooks{};

		uint64_t LuaHash(const char* filename) {
			std::string_view v{ filename };
			if (!v.rfind("x64:", 0)) {
				if (v.length() <= 0x18 && v.ends_with(".lua")) {
					char tmp[0x17]{};

					memcpy(tmp, &filename[4], v.size() - 8);

					return hash::Hash64(".lua", std::strtoull(&filename[4], nullptr, 16));
				}

				return std::strtoull(&filename[4], nullptr, 16) & hash::MASK63;
			}
			return hash::Hash64(filename);
		}

		int hksl_loadfile_Stub(bo4::lua_State* state, const char* filename) {
			int l{ hksl_loadfile_Hook.Call<int>(state, filename) };

			if (!luaHooks.empty()) {
				uint64_t hash{ LuaHash(filename) };

				auto it{ luaHooks.find(hash) };
				if (it != luaHooks.end()) {
					for (LuaHook& h : it->second) {
						const char* str{ utils::va("x64:%llx.lua", h.hash) };
						LOG_TRACE("hook luafile {} -> {}/({})", filename, h.filename, str)
						if (!bo4::Lua_CoD_LoadLuaFile(state, str)) {
							LOG_ERROR("Error when hooking {}->{}", filename, h.filename);
						}
					}
				}
			}
			return l;
		}

		void LoadLuaCfg(const char* modid, core::memory_allocator::MemoryAllocator& alloc, core::config::Config& cfg) {
			auto itHooks{ cfg.main.FindMember("lua_hook") };

			if (itHooks != cfg.main.MemberEnd() && itHooks->value.IsArray()) {
				for (rapidjson::Value& hook : itHooks->value.GetArray()) {
					if (!hook.IsObject()) {
						LOG_WARNING("Found invalid luafile hook: not an object");
						continue;
					}

					auto obj{ hook.GetObj() };

					auto nameIt{ obj.FindMember("name") };
					auto hookIt{ obj.FindMember("hook") };

					if (nameIt == obj.MemberEnd()) {
						LOG_WARNING("Found invalid luafile hook: missing name");
						continue;
					}

					const char* luaName{ nameIt->value.GetString() };
					if (hookIt == obj.MemberEnd()) {
						LOG_WARNING("Found invalid luafile hook: missing hook for {}", luaName);
						continue;
					}
					const char* hook{ hookIt->value.GetString() };

					uint64_t luaHash{ hash::Hash64Pattern(luaName) };
					uint64_t hookHash{ hash::Hash64Pattern(hook) };

					if (!luaHash || !hookHash) {
						LOG_ERROR("Can't hash '{}' or '{}'", luaName, hook);
						continue;
					}
					uint64_t lookupHash{ core::hashes::lookup::LookupFNV1A64(".lua", luaHash) };

					if (!lookupHash) {
						LOG_ERROR("Can't find filename for '{}'", luaName);
						continue;
					}

					luaHooks[hookHash & hash::MASK63].emplace_back(alloc.CloneStr(luaName), lookupHash);

					LOG_INFO("Loaded luafile hook {}({:x})->{}({:x}/{:x}.lua)", hook, hookHash, luaName, luaHash, lookupHash);
				}
			}

		}

		void PostInit(uint64_t uid) {
			hksl_loadfile_Hook.Create(0x375D6A0_a, hksl_loadfile_Stub);
			Lua_CoD_RegisterEngineFunctions_Hook.Create(0x3911C00_a, Lua_CoD_RegisterEngineFunctions_Stub);
		}

		utils::ArrayAdder<systems::mods::ModLoadingHook> cfgHook{ systems::mods::GetModLoadingHooks(), LoadLuaCfg};
		REGISTER_SYSTEM(lua, nullptr, PostInit);
	}
}