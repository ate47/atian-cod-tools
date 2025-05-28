#include <dll_includes.hpp>
#include <games/bo4/t8_errors.hpp>
#include <data/bo4.hpp>
#include <data/refs.hpp>
#include <core/config.hpp>
#include <core/system.hpp>
#include <hook/library.hpp>
#include <gsc/gsc_gdb.hpp>
#include <core/hashes/hash_store.hpp>

namespace {
	using namespace shared::gsc::acts_debug;
	struct objGDBFileInfo_t {
		XHash name;
		bo4::GSC_OBJ* obj;
		GSC_ACTS_DEBUG* gdb;
	};

	bool traceDetours;
	objGDBFileInfo_t gObjGDBFileInfo[bo4::scriptInstance_t::SI_COUNT][650];
	int gObjGDBFileInfoCount[bo4::scriptInstance_t::SI_COUNT];
	size_t gObjCountLinked[bo4::scriptInstance_t::SI_COUNT];

	hook::library::Detour Scr_GscLink_Detour;
	hook::library::Detour Scr_ResetLinkInfo_Detour;

	void CheckDetour(objGDBFileInfo_t* dbgobj, bo4::GSC_OBJ* obj) {
		if (!dbgobj) return;
		GSC_ACTS_DEBUG* dbg{ dbgobj->gdb };
		if (!dbg || !dbg->HasFeature(ADF_DETOUR) || !dbg->detour_count || !obj->imports_count) return;

		GSC_ACTS_DETOUR* detour{ dbg->GetDetours() };
		GSC_ACTS_DETOUR* detourend{ dbg->GetDetoursEnd() };
		for (; detour != detourend; detour++) {
			uint64_t* usings{ (uint64_t*)(obj->magic + obj->include_offset) };
			uint64_t* usingsend{ usings + obj->include_count };

			if (!detour->script && detour->script != obj->name && !std::find(usings, usingsend, detour->script)) {
				continue; // not included by this script
			}

			bo4::GSC_IMPORT_ITEM* next{ (bo4::GSC_IMPORT_ITEM*)(obj->magic + obj->imports_offset) };
			for (size_t i = 0; i < obj->imports_count; i++) {
				bo4::GSC_IMPORT_ITEM* imp{ next };
				int32_t* locs{ (int32_t*)(imp + 1) };
				next = (bo4::GSC_IMPORT_ITEM*)(locs + imp->num_address);
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
					byte* loc{ utils::Aligned<uint16_t>(obj->magic + locs[j])};

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

	// sync the registered detours for this entry
	void SyncDetours(bo4::scriptInstance_t inst, bo4::GSC_OBJ* obj) {
		size_t count{ (size_t)gObjGDBFileInfoCount[inst] };
		for (size_t i = 0; i < count; i++) {
			CheckDetour(&gObjGDBFileInfo[inst][i], obj);
		}
	}

	// sync the previously linked scripts with a new debug object
	void AddDetours(bo4::scriptInstance_t inst, objGDBFileInfo_t* dbgobj) {
		if (!dbgobj) return;
		GSC_ACTS_DEBUG* dbg{ dbgobj->gdb };
		if (!dbg || !dbg->HasFeature(ADF_DETOUR) || !dbg->detour_count) return;

		size_t count{ (size_t)bo4::gObjFileInfoCount[inst] };
		for (size_t i = 0; i < count; i++) {
			bo4::GSC_OBJ* obj{ ((*bo4::gObjFileInfo)[inst][i].activeVersion) };
			if (dbgobj->obj == obj) continue; // avoid double match
			CheckDetour(dbgobj, obj);
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

			// load dbg data
			bo4::ScriptParseTreeDBG* dbg{ bo4::DB_FindXAssetHeader(bo4::XAssetType::ASSET_TYPE_SCRIPTPARSETREEDBG, &hash, false, -1).sptdbg };
			if (dbg && dbg->gdb && dbg->gdbLen >= 8 && dbg->gdb->GetMagic() == MAGIC) {
				GSC_ACTS_DEBUG* gdb{ dbg->gdb };


				if (!gdb->HasFeature(ADF_CHECKSUM)) {
					LOG_WARNING("Found ADF file without checksum feature: {}", core::hashes::ExtractTmp("script", hash));
				}
				else {
					if (gdb->checksum != prime_obj->crc) {
						LOG_ERROR("Found ADF file with bad checksum for file {}", core::hashes::ExtractTmp("script", hash));
						continue;
					}
				}

				if (gdb->HasFeature(ADF_STRING)) {
					uint32_t* strings{ gdb->GetStrings() };
					uint32_t* stringsend{ gdb->GetStringsEnd() };

					// load strings
					for (; strings != stringsend; strings++) {
						const char* str{ gdb->GetString(*strings) };
						core::hashes::AddPrecomputed(hash::Hash64(str), str);
						core::hashes::AddPrecomputed(hash::HashT89Scr(str), str);
					}
				}

				int idx{ gObjGDBFileInfoCount[inst]++ };

				objGDBFileInfo_t* obj{ gObjGDBFileInfo[inst] + idx };
				obj->name = hash;
				obj->gdb = gdb;
				obj->obj = prime_obj;
				AddDetours(inst, obj);
			}
			SyncDetours(inst, prime_obj);
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
		GSC_ACTS_DEBUG* gdb{};
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
		bo4::ScopedCriticalSection scs{ bo4::CRITSECT_VM, bo4::SCOPED_CRITSECT_NORMAL };
		Scr_ResetLinkInfo_Detour.Call(inst);
		// cleanup debug data
		std::memset(gObjGDBFileInfo[inst], 0, sizeof(gObjGDBFileInfo[inst]));
		gObjGDBFileInfoCount[inst] = 0;
		gObjCountLinked[inst] = 0;
	}

	void PostInit(uint64_t uid) {
		traceDetours = core::config::GetBool("test.tracedetours");

		Scr_GscLink_Detour.Create(0x2748BB0_a, Scr_GscLink_Stub);
		Scr_ResetLinkInfo_Detour.Create(0x2749480_a, Scr_ResetLinkInfo_Stub);
		hook::memory::RedirectJmp(0x2748550_a, Scr_GetGscExportInfo_Stub);
	}


	REGISTER_SYSTEM(gsc_link, nullptr, PostInit);
}