#pragma once
#include <includes.hpp>

namespace tool {
	namespace dump {
        struct T8ScriptParseTreeEntry {
            UINT64 name;
            UINT64 pad0;
            uintptr_t buffer;
            UINT32 size;
            UINT32 pad02;
        };

        struct T8GSCSimpleHeader {
            UINT64 magic;
            INT32 crc;
            INT32 pad;
            UINT64 name;
        };

        struct T8ObjFileInfo {
            uintptr_t activeVersion;
            INT32 slot;
            INT32 refCount;
            UINT32 groupId;
            INT32 Pad0;
        };

        struct T8EventMapObj {
            uintptr_t next;
            uintptr_t handler;
            UINT32 name;
            INT8 gameSideRegistered;
            INT8 Pad01;
            INT16 Pad02;
        };
        struct T8BuiltInFunc
        {
            UINT32 name;
            UINT32 minArgs;
            UINT32 maxArgs;
            UINT32 pad01;
            uintptr_t function;
            UINT32 type;
            UINT32 pad02;
        };

        int poolscripts(const process& proc, int argc, const char* argv[]);
        int writepoolscripts(const process& proc, int argc, const char* argv[]);
		int linkedscripts(const process& proc, int argc, const char* argv[]);
        int events(const process& proc, int argc, const char* argv[]);
        int dumpfunctions(const process& proc, int argc, const char* argv[]);
	}
}