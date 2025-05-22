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
	struct objGDBFileInfo_t {
		XHash name;
		bo4::GSC_OBJ* obj;
		shared::gsc::acts_debug::GSC_ACTS_DEBUG* gdb;
	};

	objGDBFileInfo_t gObjGDBFileInfo[2][650];
	int gObjGDBFileInfoCount[2];

	hook::library::Detour Scr_GscObjLink_Detour;
	hook::library::Detour Scr_ResetLinkInfo_Detour;

	int Scr_GscObjLink_Stub(bo4::scriptInstance_t inst, bo4::GSC_OBJ* prime_obj, bool runScript) {
		bo4::ScopedCriticalSection scs{ bo4::CRITSECT_VM, bo4::SCOPED_CRITSECT_NORMAL };

		int r{ Scr_GscObjLink_Detour.Call<int>(inst, prime_obj, runScript) };

		if (r < 0) return r;

		// todo: load detour data

		XHash hash{ prime_obj->name };

		bo4::ScriptParseTreeDBG* dbg{ bo4::DB_FindXAssetHeader(bo4::XAssetType::ASSET_TYPE_SCRIPTPARSETREEDBG, &hash, false, -1).sptdbg };
		if (!dbg || !dbg->gdb || dbg->gdbLen < 8 || dbg->gdb->GetMagic() != shared::gsc::acts_debug::MAGIC) return r; // bad or no file
		shared::gsc::acts_debug::GSC_ACTS_DEBUG* gdb{ dbg->gdb };


		if (!gdb->HasFeature(shared::gsc::acts_debug::ADF_CHECKSUM)) {
			LOG_WARNING("Found ADF file without checksum feature: {}", core::hashes::ExtractTmp("script", hash));
		}
		else {
			if (gdb->checksum != prime_obj->crc) {
				LOG_ERROR("Found ADF file with bad checksum for file {}", core::hashes::ExtractTmp("script", hash));
				return -1;
			}
		}

		if (gdb->HasFeature(shared::gsc::acts_debug::ADF_STRING)) {
			uint32_t* strings{ gdb->GetStrings() };
			uint32_t* stringsend{ gdb->GetStringsEnd() };

			// load strings
			for (; strings != stringsend; strings++) {
				const char* str{ gdb->GetString(*strings) };
				core::hashes::AddPrecomputed(hash::Hash64(str), str);
				core::hashes::AddPrecomputed(hash::HashT89Scr(str), str);
			}
		}

		// todo: load detour data

		int idx{ gObjGDBFileInfoCount[inst]++ };

		objGDBFileInfo_t* obj{ gObjGDBFileInfo[inst] + idx };
		obj->name = hash;
		obj->gdb = gdb;
		obj->obj = prime_obj;

		return r;
	}

	void Scr_GetGscExportInfo_Stub(bo4::scriptInstance_t inst, byte* codepos, const char** scriptname, int32_t* sloc, int32_t* crc, int32_t* vm) {
		static char scriptnamebuffer[bo4::scriptInstance_t::SI_COUNT][0x200];
		bo4::GSC_OBJ* script_obj{};
		shared::gsc::acts_debug::GSC_ACTS_DEBUG* gdb{};
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
			shared::gsc::acts_debug::GetGDBInfo(rloc, gdb, &filename, &line);
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
	}

	void PostInit(uint64_t uid) {
		Scr_GscObjLink_Detour.Create(0x2748E70_a, Scr_GscObjLink_Stub);
		Scr_ResetLinkInfo_Detour.Create(0x2749480_a, Scr_ResetLinkInfo_Stub);
		hook::memory::RedirectJmp(0x2748550_a, Scr_GetGscExportInfo_Stub);
	}


	REGISTER_SYSTEM(gsc_link, nullptr, PostInit);
}