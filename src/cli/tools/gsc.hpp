#pragma once
#include "gsc_opcodes.hpp"
#include <includes.hpp>

namespace tool::gsc {
    enum GscInfoOptionStepSkip {
        STEPSKIP_DEV = 1,
        STEPSKIP_SWITCH = 2,
        STEPSKIP_FOREACH = 4,
        STEPSKIP_WHILE = 8,
        STEPSKIP_IF = 0x10,
        STEPSKIP_FOR = 0x20,
    };
    // cli options
    class GscInfoOption {
    public:
        bool m_dcomp = false;
        bool m_dasm = false;
        bool m_help = false;
        bool m_verbose = true;
        bool m_header = false;
        bool m_imports = false;
        bool m_strings = false;
        bool m_gvars = false;
        bool m_includes = true;
        bool m_exptests = false;
        bool m_patch = true;
        bool m_func = true;
        bool m_func_rloc = false;
        bool m_func_header = true;
        bool m_func_header_post = false;
        bool m_show_jump_delta = false;
        bool m_show_pre_dump = false;
        bool m_show_ref_count = false;
        LPCCH m_rosetta = NULL;
        LPCCH m_dump_hashmap = NULL;
        LPCCH m_outputDir = NULL;
        LPCCH m_copyright = NULL;
        bool m_show_internal_blocks = false;
        bool m_show_func_vars = false;
        UINT32 m_stepskip = 0;
        opcode::Platform m_platform = opcode::Platform::PLATFORM_PC;


        std::vector<LPCCH> m_inputFiles{};
        /*
         * Compute options
         * @param args Arguments
         * @param startIndex Start index in args
         * @param startIndex End index in args (exclusive)
         * @return if the arguments were correctly computed
         */
        bool Compute(LPCCH* args, INT startIndex, INT endIndex);
        /*
         * Print help in a stream
         * @param out stream
         */
        void PrintHelp(std::ostream& out);
    };
    struct T8GSCExport;
    class T8GSCOBJContext;

    namespace opcode {
        class ASMContext;
        class OPCodeInfo;
        class ASMContextNodeBlock;

        struct DecompContext {
            int padding = 0;
            int rloc = 0;
            const ASMContext& asmctx;

            std::ostream& WritePadding(std::ostream& out, bool forceNoRLoc);
            inline std::ostream& WritePadding(std::ostream& out) {
                return WritePadding(out, false);
            }
        };
        enum ASMContextNodePriority : UINT {
            PRIORITY_INST,

            PRIORITY_SET,
            PRIORITY_TERNARY,

            PRIORITY_BOOL_OR,
            PRIORITY_BOOL_AND,

            PRIORITY_BIT_OR,
            PRIORITY_BIT_XOR,
            PRIORITY_BIT_AND,

            PRIORITY_EQUALS,
            PRIORITY_COMPARE,

            PRIORITY_BIT_SHIFTS,
            PRIORITY_PLUS,
            PRIORITY_MULT,

            PRIORITY_UNARY,

            PRIORITY_ACCESS,

            PRIORITY_VALUE
        };
        enum ASMContextNodeType : UINT {
            TYPE_BLOCK,
            TYPE_STATEMENT,
            TYPE_DO_WHILE,
            TYPE_WHILE,
            TYPE_FOR,
            TYPE_FOR_EACH,
            TYPE_IF,
            TYPE_TERNARY,
            TYPE_IDENTIFIER,

            TYPE_JUMP,
            TYPE_JUMP_ONFALSE,
            TYPE_JUMP_ONTRUE,
            TYPE_JUMP_LOWERTHAN,
            TYPE_JUMP_GREATERTHAN,
            TYPE_JUMP_ONFALSEEXPR,
            TYPE_JUMP_ONTRUEEXPR,
            TYPE_JUMP_DEVBLOCK,
            TYPE_JUMP_ENDSWITCH,

            TYPE_SWITCH_PRECOMPUTE,
            TYPE_SWITCH_POSTCOMPUTE,

            TYPE_CONST_HASH,
            TYPE_STRING,

            TYPE_ARRAY_FIRSTKEY,
            TYPE_ARRAY_NEXTKEY,

            TYPE_ARRAY_BUILD,
            TYPE_STRUCT_BUILD,

            TYPE_FUNC_CALL,
            TYPE_FUNC_IS_DEFINED,

            TYPE_PRECODEPOS,
            TYPE_SET,
            TYPE_DELTA,
            TYPE_ACCESS,
            TYPE_END,
            TYPE_RETURN,
            TYPE_NEW,

            TYPE_UNDEFINED,

            TYPE_JUMP_STACK_TOP
        };

        inline bool IsJumpType(ASMContextNodeType type) {
            switch (type) {
            case TYPE_JUMP:
            case TYPE_JUMP_ONFALSE:
            case TYPE_JUMP_ONTRUE:
            case TYPE_JUMP_LOWERTHAN:
            case TYPE_JUMP_GREATERTHAN:
            case TYPE_JUMP_ONFALSEEXPR:
            case TYPE_JUMP_ONTRUEEXPR:
            case TYPE_JUMP_DEVBLOCK:
            case TYPE_JUMP_ENDSWITCH:
                return true;
            default:
                return false;
            }
        }

        inline bool IsNoParamJumpType(ASMContextNodeType type) {
            switch (type) {
            case TYPE_JUMP:
            case TYPE_JUMP_DEVBLOCK:
            case TYPE_JUMP_ENDSWITCH:
                return true;
            default:
                return false;
            }
        }
        enum T8GSCLocalVarFlag : UINT8 {
            ARRAY_REF = 0x01,
            VARIADIC = 0x02
        };

        class OPCodeInfo {
        public:
            OPCode m_id;
            LPCCH m_name;

            OPCodeInfo(OPCode id, LPCCH name);
            virtual int Dump(std::ostream& out, UINT16 value, ASMContext& context, T8GSCOBJContext& objctx) const;
        };
        class ASMContextNode {
        public:
            ASMContextNodePriority m_priority;
            ASMContextNodeType m_type;
            bool m_renderRefIfAny = true;
            bool m_renderSemicolon = true;
            INT32 m_rlocEstimated = -1;

            ASMContextNode(ASMContextNodePriority priority, ASMContextNodeType type = TYPE_UNDEFINED);
            virtual ~ASMContextNode();
            virtual void Dump(std::ostream& out, DecompContext& ctx) const;
            virtual ASMContextNode* Clone() const;

            virtual void ApplySubBlocks(const std::function<void(ASMContextNodeBlock* block, ASMContext& ctx)>&, ASMContext& ctx);
        };

        class ASMContextLocationOp{
        public:
            virtual ~ASMContextLocationOp();
            virtual void Run(ASMContext& context, T8GSCOBJContext& objctx) const;
        };

        struct asmcontextlocation {
            INT32 rloc = 0;
            bool handled = false;
            std::set<INT32> refs{};
            ASMContextNode* objectId = nullptr;
            ASMContextNode* fieldId = nullptr;
            std::vector<ASMContextNode*> m_stack{};
            std::vector<ASMContextLocationOp*> m_lateop{};
            asmcontextlocation();
            ~asmcontextlocation();

            void RemoveRef(INT32 origin);
        };

        struct ASMContextStatement {
            ASMContextNode* node = nullptr;
            asmcontextlocation* location;
        };

        enum nodeblocktype {
            BLOCK_DEFAULT,
            BLOCK_DEV,
            BLOCK_PADDING
        };

        class ASMContextNodeBlock : public ASMContextNode {
        public:
            std::vector<ASMContextStatement> m_statements{};
            nodeblocktype m_blockType;
            ASMContextNodeBlock(nodeblocktype blockType = BLOCK_DEFAULT);
            ~ASMContextNodeBlock();
            void Dump(std::ostream& out, DecompContext& ctx) const override;
            ASMContextNode* Clone() const override;
            int ComputeDevBlocks(ASMContext& ctx);
            int ComputeForEachBlocks(ASMContext& ctx);
            int ComputeForBlocks(ASMContext& ctx);
            int ComputeWhileBlocks(ASMContext& ctx);
            int ComputeIfBlocks(ASMContext& ctx);
            int ComputeSwitchBlocks(ASMContext& ctx);

            ASMContextStatement* FetchFirstForLocation(INT64 rloc);

            void ApplySubBlocks(const std::function<void(ASMContextNodeBlock* block, ASMContext& ctx)>&, ASMContext& ctx) override;
        };

        struct ASMContextLocalVar {
            UINT32 name;
            BYTE flags;
            ASMContextNode* defaultValueNode = nullptr;
        };

        class ASMContext {
        public:
            // cli opt
            const GscInfoOption& m_opt;
            // run the decompiler logic
            bool m_runDecompiler;
            // fonction start location
            BYTE* m_fonctionStart;
            // locations
            std::map<INT32, asmcontextlocation> m_locs{};
            // current context location
            BYTE* m_bcl;
            // last op base
            INT32 m_lastOpCodeBase;
            // export namespace
            UINT32 m_namespace;
            // current objectid
            ASMContextNode* m_objectId = nullptr;
            // current fieldid
            ASMContextNode* m_fieldId = nullptr;
            // context stack
            std::vector<ASMContextNode*> m_stack{};
            // decompiled nodes
            ASMContextNodeBlock m_funcBlock;
            // local vars
            std::vector<ASMContextLocalVar> m_localvars{};
            // local vars ref
            std::unordered_map<UINT32, INT32> m_localvars_ref{};
            // export
            const T8GSCExport& m_exp;
            // file vm
            BYTE m_vm;
            // file platform
            Platform m_platform;

            ASMContext(BYTE* fonctionStart, const GscInfoOption& opt, UINT32 nsp, const T8GSCExport& exp, BYTE vm, Platform platform);
            ~ASMContext();

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

            /*
             * Call LookupOpCode with the ctx vm
             * @param opcode op code
             * @return handler
             */
            inline const tool::gsc::opcode::OPCodeInfo* LookupOpCode(UINT16 opcode) {
                return tool::gsc::opcode::LookupOpCode(m_vm, m_platform, opcode);
            }

            /*
             * Get a local var id by its name
             * @param name Var name
             * @return local var id or -1 if not defined in this context
             */
            int GetLocalVarIdByName(UINT32 name) const;

            /*
             * Push an ASMContext (ASMC) node on the stack
             * @param node Node
             */
            void PushASMCNode(ASMContextNode* node);

            /*
             * Pop an ASMContext (ASMC) node from the stack
             * @return Node
             */
            ASMContextNode* PopASMCNode();

            /*
             * Peek an ASMContext (ASMC) node from the stack
             * @return Node
             */
            ASMContextNode* PeekASMCNode();

            /*
             * Free the previous ObjectId ASCM node and put a new node
             * @param node Nullable node
             */
            void SetObjectIdASMCNode(ASMContextNode* node);

            /*
             * Free the previous FieldId ASCM node and put a new node
             * @param node Nullable node
             */
            void SetFieldIdASMCNode(ASMContextNode* node);

            /*
             * Get the previous ObjectId ASCM node and put a new node
             * @param node Nullable node
             */
            ASMContextNode* GetObjectIdASMCNode();

            /*
             * Get the previous FieldId ASCM node and put a new node
             * @param node Nullable node
             */
            ASMContextNode* GetFieldIdASMCNode();

            /*
             * Complete any statement on the ASCM stack
             */
            void CompleteStatement();

            /*
             * Compute the default param value from the function block
             */
            void ComputeDefaultParamValue();

            /*
             * Compute the dev blocks
             */
            inline void ComputeDevBlocks(ASMContext& ctx) {
                m_funcBlock.ComputeDevBlocks(ctx);
            }
            /*
             * Compute the switch blocks
             */
            inline void ComputeSwitchBlocks(ASMContext& ctx) {
                m_funcBlock.ComputeSwitchBlocks(ctx);
            }
            /*
             * Compute the for blocks
             */
            inline void ComputeForBlocks(ASMContext& ctx) {
                m_funcBlock.ComputeForBlocks(ctx);
            }
            /*
             * Compute the for each blocks
             */
            inline void ComputeForEachBlocks(ASMContext& ctx) {
                m_funcBlock.ComputeForEachBlocks(ctx);
            }
            /*
             * Compute the while blocks
             */
            inline void ComputeWhileBlocks(ASMContext& ctx) {
                m_funcBlock.ComputeWhileBlocks(ctx);
            }
            /*
             * Compute the if blocks
             */
            inline void ComputeIfBlocks(ASMContext& ctx) {
                m_funcBlock.ComputeIfBlocks(ctx);
            }
            /*
             * Dump the function block
             */
            inline void Dump(std::ostream& out, DecompContext& ctx) const {
                m_funcBlock.Dump(out, ctx);
            }
        };
    }

    struct GsicDetour {
        UINT32 name;
        UINT32 replaceNamespace;
        UINT32 replaceFunction;
        INT32 fixupOffset;
        INT32 fixupSize;
        INT32 replaceScriptTop;
        INT32 replaceScriptBottom;
    };
    struct GsicInfo {
        bool isGsic = false;
        size_t headerSize = 0;
        std::unordered_map<UINT32, GsicDetour*> detours{};
    };
    struct asmcontext_func {
        UINT32 name;
        UINT32 nsp;
    };
    struct gscclass {
        UINT32 name_space = 0;
        std::set<UINT32> m_superClass{};
        std::vector<UINT32> m_methods{};
        std::unordered_map<UINT64, asmcontext_func> m_vtable{};
    };
    // Result context for T8GSCOBJ::PatchCode
    class T8GSCOBJContext {
    private:
        std::unordered_map<UINT16, UINT32> m_gvars{};
        std::unordered_map<UINT32, LPCCH> m_stringRefs{};
    public:
        GsicInfo m_gsicInfo{};
        std::unordered_map<UINT32, gscclass> m_classes{};
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

        // @return the vm
        inline BYTE GetVm() {
            return magic[7];
        }
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

        void DumpFunctionHeader(std::ostream& out, BYTE* gscFile, T8GSCOBJContext& ctx, tool::gsc::opcode::ASMContext& asmctx, int padding = 0) const;
        int DumpAsm(std::ostream& out, BYTE* gscFile, T8GSCOBJContext& ctx, tool::gsc::opcode::ASMContext& asmctx) const;
        int DumpVTable(std::ostream& out, BYTE* gscFile, T8GSCOBJContext& objctx, opcode::ASMContext& ctx, opcode::DecompContext& dctxt) const;
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
        CLASS_VTABLE = 0x86
    };

    enum T8GSCImportFlags : UINT8 {
        FUNC_METHOD = 0x1,
        FUNCTION = 0x2,
        FUNCTION_THREAD = 0x3,
        FUNCTION_CHILDTHREAD = 0x4,
        METHOD = 0x5,
        METHOD_THREAD = 0x6,
        METHOD_CHILDTHREAD = 0x7,
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

    enum RosettaBlockType : BYTE {
        RBT_START = 0x50,
        RBT_OPCODE = 0x51,
    };
    struct RosettaOpCodeBlock {
        UINT32 location;
        UINT16 opcode;
    };
    struct RosettaFileData {
        T8GSCOBJ header;
        std::vector<RosettaOpCodeBlock> blocks{};
    };

    /*
     * Begin rosetta file data
     * @param obj script
     */
    void RosettaStartFile(tool::gsc::T8GSCOBJ* obj);
    /*
     * Add rosetta opcode data
     * @param loc opcode location
     * @param opcode opcode
     */
    void RosettaAddOpCode(UINT32 loc, UINT16 opcode);
}