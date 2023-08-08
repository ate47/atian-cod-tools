#pragma once
#include <includes.hpp>

namespace tool::gsc {
    // Result context for T8GSCOBJ::PatchCode
    class T8GSCOBJContext {
    private:
        std::unordered_map<UINT16, UINT32> m_gvars;
        std::unordered_map<UINT32, LPCCH> m_stringRefs;
    public:
        T8GSCOBJContext();

        /*
         * Get a name for a global var ref
         * @param gvarRef ref
         * @return name or 0
         */
        UINT32 GetGlobalVarName(UINT16 gvarRef);
        /*
         * Get a string for a string string ref
         * @param stringRef ref
         * @return string or null
         */
        LPCCH GetStringValue(UINT32 stringRef);
        /*
         * Add a global var
         * @param value name
         * @return var ref
         */
        UINT16 AddGlobalVarName(UINT32 value);
        /*
         * Add a string
         * @param value string
         * @return new string ref
         */
        UINT32 AddStringValue(LPCCH value);
    };
    struct T8GSCOBJ {
        BYTE magic[8];
        INT32 crc;
        INT32 pad;
        UINT64 name;
        INT32 include_offset;
        UINT16 string_count;
        UINT16 exports_count;
        INT32 ukn20;
        INT32 string_offset;
        INT16 imports_count;
        UINT16 fixup_count;
        INT32 ukn2c;
        INT32 export_table_offset;
        INT32 ukn34;
        INT32 imports_offset;
        UINT16 globalvar_count;
        INT32 fixup_offset;
        INT32 globalvar_offset;
        INT32 script_size;
        INT32 ukn4c_offset;
        INT32 ukn50;
        INT32 ukn54;
        UINT16 include_count;
        BYTE ukn5a;
        BYTE ukn4c_count;

        /*
         * Path the code to remove linking
         * @param context Context to fill during patching
         */
        void PatchCode(T8GSCOBJContext& context);
    };

    struct T8GSCFixup {
        uintptr_t offset;
        uintptr_t address;
    };

    struct T8GSCImport {
        UINT32 name;
        UINT32 import_namespace;
        UINT16 num_address;
        UINT8 param_count;
        UINT8 flags;
    };

    struct T8GSCGlobalVar {
        UINT32 name;
        UINT32 num_address;
    };
    
    struct T8GSCExport {
        UINT32 checksum;
        UINT32 address;
        UINT32 name;
        UINT32 name_space;
        UINT32 callback_event;
        UINT8 param_count;
        UINT8 flags;
        UINT16 padding;

        int DumpAsm(std::ostream& out, BYTE* gscFile, T8GSCOBJContext& ctx) const;
    };

    enum T8GSCExportFlags : UINT8 {
        LINKED = 0x01,
        AUTOEXEC = 0x02,
        PRIVATE = 0x04,
        CLASS_MEMBER = 0x08,
        CLASS_DESTRUCTOR = 0x10,
        VE = 0x20,
        EVENT = 0x40,
        CLASS_LINKED = 0x80,
    };

    enum T8GSCImportFlags : UINT8 {
        FUNC_METHOD = 0x1,
        FUNCTION = 0x2,
        FUNCTION_THREAD = 0x3,
        FUNCTION_THREAD_ENDON = 0x4,
        METHOD = 0x5,
        METHOD_THREAD = 0x6,
        METHOD_THREAD_ENDON = 0x7,
        CALLTYPE_MASK = 0xF,
        DEV_CALL = 0x10,
        GET_CALL = 0x20,
        UKN40 = 0x40,
        UKN80 = 0x80
    };

    struct T8GSCString {
        UINT32 string;
        UINT8 num_address;
        UINT8 type;
        UINT16 pad;
    };

    int gscinfo(const process& proc, int argc, const char* argv[]);
    int dumpdataset(const process& proc, int argc, const char* argv[]);
}