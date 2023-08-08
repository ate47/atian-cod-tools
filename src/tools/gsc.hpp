#pragma once
#include <includes.hpp>

namespace tool::gsc {
    namespace opcode {
        struct decompcontext {
            int padding{};
        };
        enum asmcontextnode_priority : UINT {
            PRIORITY_INST,

            PRIORITY_BIT_OR,
            PRIORITY_BIT_XOR,
            PRIORITY_BIT_AND,

            PRIORITY_EQUALS,
            PRIORITY_COMPARE,

            PRIORITY_BIT_SHIFTS,
            PRIORITY_PLUS,
            PRIORITY_MULT,

            PRIORITY_DOT,

            PRIORITY_UNARY,

            PRIORITY_VALUE
        };
        class asmcontextnode {
        public:
            asmcontextnode_priority m_priority;
            asmcontextnode(asmcontextnode_priority priority);
            virtual ~asmcontextnode();
            virtual void Dump(std::ostream& out, decompcontext& ctx) const;
        };

        struct asmcontextlocation {
            INT32 rloc;
            bool handled;
            bool ref;
            asmcontextlocation();
        };

        struct asmcontextlocalvar {
            UINT32 name;
            BYTE flags;
        };

        struct asmcontextstatement {
            asmcontextnode* node;
            asmcontextlocation& location;
        };

        class asmcontext {
        public:
            // fonction start location
            BYTE* m_fonctionStart;
            // locations
            std::map<INT32, asmcontextlocation> m_locs{};
            // current context location
            BYTE* m_bcl;
            // last op base
            INT32 m_lastOpCodeBase;
            // context stack
            std::vector<asmcontextnode*> m_stack{};
            // decompiled nodes
            std::vector<asmcontextstatement> m_nodes{};
            // local vars
            std::vector<asmcontextlocalvar> m_localvars{};

            asmcontext(BYTE* fonctionStart);

            // @return relative location in the function
            inline INT32 FunctionRelativeLocation() {
                return FunctionRelativeLocation(m_bcl);
            }
            // Push the current location to the locations
            inline asmcontextlocation& PushLocation() {
                return PushLocation(m_bcl);
            }
            // @return Push a location to the locations and return it
            asmcontextlocation& PushLocation(BYTE* location);

            // @return find the next location to handle, false if no new location can be find, false otherwise
            bool FindNextLocation();
            /*
             * Get a relative location from the function start
             * @param bytecodeLocation Location
             * @return relative from function start
             */
            INT32 FunctionRelativeLocation(BYTE* bytecodeLocation);
            // @return align and return m_bcl on a particular datatype
            template<typename Type>
            inline BYTE*& Aligned() {
                return m_bcl = utils::Aligned<Type>(m_bcl);
            }
            // @return Write asm padding and return out
            std::ostream& WritePadding(std::ostream& out);

            // @return the Final size of the function by looking at the last position
            UINT FinalSize() const;

            void PushASMCNode(asmcontextnode* node);
            asmcontextnode* PopASMCNode();

            void CompleteStatement();
        };
    }
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

        void DumpFunctionHeader(std::ostream& out, BYTE* gscFile, T8GSCOBJContext& ctx, tool::gsc::opcode::asmcontext& asmctx) const;
        int DumpAsm(std::ostream& out, BYTE* gscFile, T8GSCOBJContext& ctx, tool::gsc::opcode::asmcontext& asmctx) const;
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