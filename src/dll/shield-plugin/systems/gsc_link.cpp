#include <dll_includes.hpp>
#include <games/bo4/t8_errors.hpp>
#include <data/bo4.hpp>
#include <data/refs.hpp>
#include <core/config.hpp>
#include <core/system.hpp>
#include <core/eventhandler.hpp>
#include <hook/library.hpp>
#include <gsc/gsc_gdb.hpp>
#include <gsc/gsc_acts_addons.hpp>
#include <core/hashes/hash_store.hpp>
#include <systems/gsc_link.hpp>
#include <systems/events.hpp>

namespace systems::gsc::link {
	using namespace shared::gsc;

	using acts_debug::GSC_ACTS_DEBUG;
	using acts_debug::GSC_ACTS_DETOUR;
	using acts_addons::GSC_ACTS_ADDONS;

	struct objGDBFileInfo_t {
		XHash name;
		bo4::GSC_OBJ* obj;
		GSC_ACTS_DEBUG* gdb;
		GSC_ACTS_ADDONS* addons;
	};

	namespace {
		bool traceDetours;
		objGDBFileInfo_t gObjGDBFileInfo[bo4::scriptInstance_t::SI_COUNT][650];
		int gObjGDBFileInfoCount[bo4::scriptInstance_t::SI_COUNT];
		size_t gObjCountLinked[bo4::scriptInstance_t::SI_COUNT];
	}

	inline byte* Scr_FindDetourArray(GSC_ACTS_DETOUR* detours, GSC_ACTS_DETOUR* end, bo4::GSC_OBJ* obj, byte* origin, uint64_t script, uint32_t nsp, uint32_t func) {
		for (; detours != end; detours++) {
			if (detours->script != script || detours->name_space != nsp || detours->name != func) {
				continue; // not it
			}

			byte* funcPtr{ obj->magic + detours->location };
			byte* funcPtrEnd{ funcPtr + detours->size };

			if (funcPtr <= origin && funcPtrEnd > origin) {
				continue; // inside the detour
			}
			return funcPtr;
		}
		return nullptr;
	}

	byte* Scr_FindDetour(bo4::scriptInstance_t inst, byte* origin, uint64_t script, uint32_t nsp, uint32_t func) {
		for (size_t i = 0; i < gObjGDBFileInfoCount[inst]; i++) {
			objGDBFileInfo_t* odbg{ &gObjGDBFileInfo[inst][i] };
			if (!odbg->obj) {
				continue; // is this mandatory?
			}
			if (odbg->addons && odbg->addons->HasFeature(acts_addons::AAF_DETOUR)) {
				// addons
				byte* d{ Scr_FindDetourArray(
					odbg->addons->GetDetours(odbg->obj->magic), odbg->addons->GetDetoursEnd(odbg->obj->magic),
					odbg->obj, origin, script, nsp, func
				) };
				if (d) {
					return d;
				}
			}
			if (odbg->gdb && odbg->gdb->HasFeature(acts_debug::ADF_DETOUR)) {
				// debug (backward compatibility with old scripts)
				byte* d{ Scr_FindDetourArray(
					odbg->gdb->GetDetours(), odbg->gdb->GetDetoursEnd(),
					odbg->obj, origin, script, nsp, func
				) };
				if (d) {
					return d;
				}
			}
		}
		return nullptr;
	}

	namespace {
		hook::library::Detour Scr_GscLink_Detour;
		hook::library::Detour Scr_ResetLinkInfo_Detour;

		inline void Scr_CheckDetour(objGDBFileInfo_t* dbgobj, bo4::GSC_OBJ* obj, GSC_ACTS_DETOUR* detour, GSC_ACTS_DETOUR* end) {
			for (; detour != end; detour++) {
				uint64_t* usings{ (uint64_t*)(obj->magic + obj->include_offset) };
				uint64_t* usingsend{ &usings[obj->include_count] };

				if (!detour->script && detour->script != obj->name && std::find(usings, usingsend, detour->script) == usingsend) {
					continue; // not included by this script
				}

				bo4::GSC_IMPORT_ITEM* next{ (bo4::GSC_IMPORT_ITEM*)&obj->magic[obj->imports_offset] };
				for (size_t i = 0; i < obj->imports_count; i++) {
					bo4::GSC_IMPORT_ITEM* imp{ next };
					int32_t* locs{ (int32_t*)&imp[1] };
					next = (bo4::GSC_IMPORT_ITEM*)&locs[imp->num_address];
					if (imp->name_space != detour->name_space || imp->name != detour->name) {
						continue; // bad export
					}

					size_t off;
					switch (imp->flags & bo4::GIF_CALLTYPE_MASK) {
					case bo4::GIF_FUNC_METHOD: // &ns::fc
						off = 0; // no count
						break;
					case bo4::GIF_FUNCTION:
					case bo4::GIF_FUNCTION_CHILDTHREAD:
					case bo4::GIF_FUNCTION_THREAD:
					case bo4::GIF_METHOD:
					case bo4::GIF_METHOD_CHILDTHREAD:
					case bo4::GIF_METHOD_THREAD:
						off = 1;
						break;
					default:
						LOG_ERROR("INVALID LINK ITEM, BAD IMPORT TYPE {}", (int)(imp->flags & bo4::GIF_CALLTYPE_MASK));
						return;
					}

					byte* detourFunc{ dbgobj->obj->magic + detour->location };
					byte* detourFuncEnd{ detourFunc + detour->size };

					size_t match{};
					for (size_t j = 0; j < imp->num_address; j++) {
						byte* loc{ utils::Aligned<uint16_t>(obj->magic + locs[j]) };

						if (loc >= detourFunc && loc < detourFuncEnd) {
							continue; // inside the def, we do not replace it
						}

						// PATCHME: patch opcode for API
						// *(uint16_t*)loc = 0;

						byte** ref{ (byte**)utils::Aligned<byte*>(loc + 2 + off) };

						*ref = detourFunc;
						match++;
					}

					if (traceDetours) {
						LOG_TRACE(
							"[{}] Loading detour {}<{}>::{} with {}/{} patch(es)",
							core::hashes::ExtractTmp("script", obj->name),
							core::hashes::ExtractTmp("namespace", detour->name_space),
							core::hashes::ExtractTmp("script", detour->script),
							core::hashes::ExtractTmp("function", detour->name),
							match, (int)imp->num_address
						);
					}
				}

			}
		}

		void Scr_CheckDetour(objGDBFileInfo_t* dbgobj, bo4::GSC_OBJ* obj) {
			if (!dbgobj || !obj->imports_count) {
				return;
			}

			if (dbgobj->addons && dbgobj->addons->HasFeature(acts_addons::AAF_DETOUR)) {
				Scr_CheckDetour(dbgobj, obj, dbgobj->addons->GetDetours(dbgobj->obj->magic), dbgobj->addons->GetDetoursEnd(dbgobj->obj->magic));
			}

			if (dbgobj->gdb && dbgobj->gdb->HasFeature(acts_debug::ADF_DETOUR)) {
				Scr_CheckDetour(dbgobj, obj, dbgobj->gdb->GetDetours(), dbgobj->gdb->GetDetoursEnd());
			}
		}

		// sync the registered detours for this entry
		void Scr_SyncDetours(bo4::scriptInstance_t inst, bo4::GSC_OBJ* obj) {
			size_t count{ (size_t)gObjGDBFileInfoCount[inst] };
			for (size_t i = 0; i < count; i++) {
				Scr_CheckDetour(&gObjGDBFileInfo[inst][i], obj);
			}
		}

		// sync the previously linked scripts with a new debug object
		void Scr_AddDetours(bo4::scriptInstance_t inst, objGDBFileInfo_t* dbgobj) {
			if (!dbgobj) {
				return;
			}

			size_t count{ (size_t)bo4::gObjFileInfoCount[inst] };
			for (size_t i = 0; i < count; i++) {
				bo4::GSC_OBJ* obj{ (*bo4::gObjFileInfo)[inst][i].activeVersion };
				if (dbgobj->obj == obj) {
					continue; // avoid double match
				}
				Scr_CheckDetour(dbgobj, obj);
			}
		}

		void Scr_GscLink_Stub(bo4::scriptInstance_t inst, XHash* filename, bool runScript) {
			bo4::ScopedCriticalSection scs{ bo4::CRITSECT_VM, bo4::SCOPED_CRITSECT_NORMAL };

			Scr_GscLink_Detour.Call(inst, filename, runScript);

			// we now have to link the newly linked scripts
			size_t count{ (size_t)bo4::gObjFileInfoCount[inst] };
			for (; gObjCountLinked[inst] < count; gObjCountLinked[inst]++) {
				bo4::GSC_OBJ* prime_obj{ (*bo4::gObjFileInfo)[inst][gObjCountLinked[inst]].activeVersion };
				XHash hash{ prime_obj->name };

				// search addon data, we don't have the size, so we assume that the cseg is not 0 (acts/treyarch valid assumption)
				objGDBFileInfo_t* obj{};
				if (prime_obj->cseg_offset > sizeof(bo4::GSC_OBJ) + sizeof(uint64_t)) {
					GSC_ACTS_ADDONS* addons{ (GSC_ACTS_ADDONS*)&prime_obj[1] };

					if (addons->GetMagic() == acts_addons::MAGIC) {
						obj = &gObjGDBFileInfo[inst][gObjGDBFileInfoCount[inst]++];
						obj->name = hash;
						obj->obj = prime_obj;
						obj->addons = addons;
						LOG_TRACE("load acts addons data {} detours={} {}", core::hashes::ExtractTmp("script", filename->name), addons->detour_count, (void*)obj);
					}
				}

				// load dbg data
				bo4::ScriptParseTreeDBG* dbg{ bo4::DB_FindXAssetHeader(bo4::XAssetType::ASSET_TYPE_SCRIPTPARSETREEDBG, &hash, false, -1).sptdbg };
				if (dbg && dbg->gdb && dbg->gdbLen >= sizeof(uint64_t) && dbg->gdb->GetMagic() == acts_debug::MAGIC) {
					acts_debug::GSC_ACTS_DEBUG* gdb{ dbg->gdb };


					if (!gdb->HasFeature(acts_debug::ADF_CHECKSUM)) {
						LOG_WARNING("Found ADF file without checksum feature: {}", core::hashes::ExtractTmp("script", hash));
					}
					else {
						if (gdb->checksum != prime_obj->crc) {
							LOG_ERROR("Found ADF file with bad checksum for file {}", core::hashes::ExtractTmp("script", hash));
							continue;
						}
					}

					if (gdb->HasFeature(acts_debug::ADF_STRING)) {
						uint32_t* strings{ gdb->GetStrings() };
						uint32_t* stringsend{ gdb->GetStringsEnd() };

						// load strings
						for (; strings != stringsend; strings++) {
							const char* str{ gdb->GetString(*strings) };
							core::hashes::AddPrecomputed(hash::Hash64(str), str);
							core::hashes::AddPrecomputed(hash::HashT89Scr(str), str);
						}
					}

					if (!obj) {
						obj = &gObjGDBFileInfo[inst][gObjGDBFileInfoCount[inst]++];
						obj->name = hash;
						obj->obj = prime_obj;
					}
					obj->gdb = gdb;
					LOG_TRACE("load acts debug data {} {}", core::hashes::ExtractTmp("script", filename->name), (void*)obj);
				}


				Scr_AddDetours(inst, obj);
				Scr_SyncDetours(inst, prime_obj);
			}
		}

		void Scr_GetGscExportInfo_Stub(bo4::scriptInstance_t inst, byte* codepos, const char** scriptname, int32_t* sloc, int32_t* crc, int32_t* vm) {
			static char scriptnamebuffer[bo4::scriptInstance_t::SI_COUNT][0x200];
			if (codepos >= bo4::scrVmInvalidFuncs[inst] && codepos < (byte*)bo4::scrVmInvalidFuncs[inst] + 32) {
				// invalid link
				if (crc) *crc = 0;
				if (vm) *vm = 0x36;
				if (sloc) *sloc = (int32_t)(codepos - (byte*)bo4::scrVmInvalidFuncs[inst]);
				if (scriptname) *scriptname = "<error>";

				return;
			}
			bo4::GSC_OBJ* script_obj{};
			acts_debug::GSC_ACTS_DEBUG* gdb{};
			{
				bo4::ScopedCriticalSection scs{ bo4::CRITSECT_VM, bo4::SCOPED_CRITSECT_NORMAL };

				// find the right script
				uint32_t count{ bo4::gObjFileInfoCount[inst] };

				for (size_t i = 0; i < count; i++) {
					bo4::objFileInfo_t& info{ (*bo4::gObjFileInfo)[inst][i] };

					bo4::GSC_OBJ* obj{ info.activeVersion };

					if (codepos >= obj->magic + obj->cseg_offset && codepos < obj->magic + obj->cseg_offset + obj->cseg_size) {
						script_obj = obj;

						int dbgcount{ gObjGDBFileInfoCount[inst] };
						// search dbg file
						for (size_t i = 0; i < dbgcount; i++) {
							if (gObjGDBFileInfo[inst][i].obj == obj) {
								gdb = gObjGDBFileInfo[inst][i].gdb;
								break;
							}
						}
						break;
					}
				}
			}

			if (!script_obj) {
				return; // can't find obj
			}


			uint32_t rloc{ (uint32_t)(codepos - script_obj->magic) };

			if (sloc) {
				*sloc = rloc;
			}

			if (crc) {
				*crc = script_obj->crc;
			}

			if (vm) {
				*vm = script_obj->magic[7];
			}

			bo4::GSC_EXPORT_ITEM* expit{};

			for (size_t i = 0; i < script_obj->exports_count; i++) {
				bo4::GSC_EXPORT_ITEM* exp{ reinterpret_cast<bo4::GSC_EXPORT_ITEM*>(script_obj->magic + script_obj->export_table_offset) + i };

				if (rloc < exp->address) {
					continue; // our code is after
				}

				if (expit && expit->address > exp->address) {
					continue; // we already have a better candidate
				}

				expit = exp;
			}

			if (gdb) {
				const char* filename{};
				uint32_t line{};
				GetGDBInfo(rloc, gdb, &filename, &line);
				if (filename) {
					// use debug data
					sprintf_s(scriptnamebuffer[inst], "%s::%s:%d@%x", filename, core::hashes::ExtractTmp("function", expit->name), line + 1, rloc - expit->address);
					*scriptname = scriptnamebuffer[inst];
					return;
				}
			}


			if (scriptname) {
				if (expit) {
					sprintf_s(
						scriptnamebuffer[inst], "%s::%s@%x",
						core::hashes::ExtractTmp("script", script_obj->name),
						core::hashes::ExtractTmp("function", expit->name),
						rloc - expit->address
					);
				}
				else {
					sprintf_s(scriptnamebuffer[inst], "%s", core::hashes::ExtractTmp("script", script_obj->name));
				}

				*scriptname = scriptnamebuffer[inst];
			}
		}

		void Scr_ResetLinkInfo_Stub(bo4::scriptInstance_t inst) {
			// cleanup debug data
			std::memset(gObjGDBFileInfo[inst], 0, sizeof(gObjGDBFileInfo[inst]));
			gObjGDBFileInfoCount[inst] = 0;
			gObjCountLinked[inst] = 0;
		}

		void PostInit(uint64_t uid) {
			traceDetours = core::config::GetBool("test.tracedetours");

			Scr_GscLink_Detour.Create(0x2748BB0_a, Scr_GscLink_Stub);
			hook::memory::RedirectJmp(0x2748550_a, Scr_GetGscExportInfo_Stub);
			events::EVENT_RESET_LINKS.Callback(Scr_ResetLinkInfo_Stub);
		}


		REGISTER_SYSTEM(gsc_link, nullptr, PostInit);
	}
}