#pragma once
#include <scriptinstance.hpp>
#include "data/cw.hpp"

namespace gsc {
	extern cw::ObjFileInfoStruct* gObjFileInfo;
	extern uint32_t* gObjFileInfoCount;
	extern void** gVmOpJumpTable;
	extern void (*Scr_GscObjLink)(scriptinstance::ScriptInstance inst, uint64_t scriptname);
	extern cw::XAssetHeader(*DB_FindXAssetHeader)(cw::XAssetType type, uint64_t name, bool inv, int timeout);
}