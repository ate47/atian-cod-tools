#pragma once
#include "gsc_opcodes.hpp"
#include "gsc_formatter.hpp"
#include <includes.hpp>

namespace tool::gsc {
    enum GscInfoOptionStepSkip {
        STEPSKIP_DEV = 1,
        STEPSKIP_SWITCH = 2,
        STEPSKIP_FOREACH = 4,
        STEPSKIP_WHILE = 8,
        STEPSKIP_IF = 0x10,
        STEPSKIP_FOR = 0x20,
        STEPSKIP_RETURN = 0x40,
        STEPSKIP_BOOL_RETURN = 0x80,
    };
    // cli options
    class GscInfoOption {
    public:
        bool m_dcomp{};
        bool m_dasm{};
        bool m_help{};
        bool m_header{};
        bool m_imports{};
        bool m_strings{};
        bool m_gvars{};
        bool m_includes{ true };
        bool m_exptests{};
        bool m_patch{ true };
        bool m_func{ true };
        bool m_func_rloc{};
        bool m_func_header{ true };
        bool m_func_header_post{};
        bool m_show_jump_delta{};
        bool m_show_pre_dump{};
        bool m_show_ref_count{};
        bool m_test_header{};
        LPCCH m_rosetta{};
        LPCCH m_dump_hashmap{};
        LPCCH m_dump_strings{};
        LPCCH m_outputDir{};
        LPCCH m_copyright{};
        bool m_show_internal_blocks{};
        bool m_show_func_vars{};
        bool m_mark_jump_type{};
        UINT32 m_stepskip{};
        opcode::Platform m_platform{ opcode::Platform::PLATFORM_PC };
        opcode::VM m_vm{ opcode::VM::VM_UNKNOWN };
        const formatter::FormatterInfo* m_formatter{};


        std::vector<LPCCH> m_inputFiles{};

        GscInfoOption();
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
         */
        void PrintHelp();
    };
    struct T8GSCExport;
    struct IW23GSCImport;
    class T8GSCOBJContext;
    class GSCOBJReader;
    struct GSCExportReader;

    namespace opcode {
        class ASMContext;
        class ASMSkipContext;
        class OPCodeInfo;
        class ASMContextNodeBlock;

        struct DecompContext {
            int padding = 0;
            int rloc = 0;
            const GscInfoOption& opt;

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
            TYPE_ARRAY_NEXTKEY_IT,
            TYPE_ARRAY_NEXTVAL_IT,
            TYPE_ARRAY_NEXT_IT,

            TYPE_ARRAY_BUILD,
            TYPE_STRUCT_BUILD,

            TYPE_FUNC_CALL,
            TYPE_FUNC_REFNAME,
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
            VARIADIC = 0x02,
            T9_VAR_REF = 0x04 // T9
        };

        class OPCodeInfo {
        public:
            OPCode m_id;
            LPCCH m_name;

            OPCodeInfo(OPCode id, LPCCH name);
            virtual int Dump(std::ostream& out, UINT16 value, ASMContext& context, T8GSCOBJContext& objctx) const;
            virtual int Skip(UINT16 value, ASMSkipContext& ctx) const;
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
            virtual ASMContextNode* ConvertToBool();
            virtual bool IsBoolConvertable(bool strict);

            virtual void ApplySubBlocks(const std::function<void(ASMContextNodeBlock* block, ASMContext& ctx)>&, ASMContext& ctx);
            friend std::ostream& operator<<(std::ostream& os, const ASMContextNode& obj);
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
            int ComputeReturnJump(ASMContext& ctx);
            int ComputeBoolReturn(ASMContext& ctx);

            ASMContextStatement* FetchFirstForLocation(INT64 rloc);

            void ApplySubBlocks(const std::function<void(ASMContextNodeBlock* block, ASMContext& ctx)>&, ASMContext& ctx) override;
        };

        struct ASMContextLocalVar {
            UINT64 name;
            BYTE flags;
            ASMContextNode* defaultValueNode = nullptr;
        };

        class ASMSkipContext {
        public:
            // fonction start location
            BYTE* m_fonctionStart;
            // file platform
            Platform m_platform;
            // object context
            VmInfo* m_vminfo;
            // locations
            std::map<INT32, asmcontextlocation> m_locs{};
            // current context location
            BYTE* m_bcl;
            // error
            std::string m_error{};

            ASMSkipContext(BYTE* fonctionStart, Platform platform, VmInfo* m_vm);

            // @return align and return m_bcl on a particular datatype
            template<typename Type>
            inline BYTE*& Aligned() {
                return m_bcl = utils::Aligned<Type>(m_bcl);
            }

            inline INT32 FunctionRelativeLocation(BYTE* bytecodeLocation) {
                return (INT32)(reinterpret_cast<uintptr_t>(bytecodeLocation) - reinterpret_cast<uintptr_t>(m_fonctionStart));
            }
            inline INT32 FunctionRelativeLocation() {
                return FunctionRelativeLocation(m_bcl);
            }
            // Push the current location to the locations
            inline asmcontextlocation& PushLocation() {
                return PushLocation(m_bcl);
            }
            // @return Push a location to the locations and return it
            asmcontextlocation& PushLocation(BYTE* location);

            // find next location
            bool FindNextLocation();
            inline const tool::gsc::opcode::OPCodeInfo* LookupOpCode(UINT16 opcode) {
                return tool::gsc::opcode::LookupOpCode(m_vminfo->vm, m_platform, opcode);
            }
        };

        class ASMContext {
        public:
            // cli opt
            const GscInfoOption& m_opt;
            // object context
            T8GSCOBJContext& m_objctx;
            // run the decompiler logic
            bool m_runDecompiler;
            // find error
            bool m_disableDecompiler{};
            // fonction start location
            BYTE* m_fonctionStart;
            // script reader
            GSCOBJReader& m_gscReader;
            // locations
            std::map<INT32, asmcontextlocation> m_locs{};
            // current context location
            BYTE* m_bcl;
            // last op base
            INT32 m_lastOpCodeBase;
            // export namespace
            UINT64 m_namespace;
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
            std::unordered_map<UINT64, INT32> m_localvars_ref{};
            // export
            GSCExportReader& m_exp;
            void* m_readerHandle;
            // file vm
            BYTE m_vm;
            // file platform
            Platform m_platform;

            ASMContext(BYTE* fonctionStart, GSCOBJReader& gscReader, T8GSCOBJContext& objctx, const GscInfoOption& opt, UINT64 nsp, GSCExportReader& exp, void* m_readerHandle, BYTE vm, Platform platform);
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
            int GetLocalVarIdByName(UINT64 name) const;

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
             * Compute the return candidates
             */
            inline void ComputeReturnJump(ASMContext& ctx) {
                m_funcBlock.ComputeReturnJump(ctx);
            }
            /*
             * Compute the boolean return candidates
             */
            inline void ComputeBoolReturn(ASMContext& ctx) {
                m_funcBlock.ComputeBoolReturn(ctx);
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
        std::unordered_map<UINT64, GsicDetour*> detours{};
    };
    struct asmcontext_func {
        UINT64 name;
        UINT64 nsp;
    };
    struct gscclass {
        UINT64 name_space = 0;
        std::set<UINT64> m_superClass{};
        std::vector<UINT64> m_methods{};
        std::unordered_map<UINT64, asmcontext_func> m_vtable{};
    };
    // Result context for T8GSCOBJ::PatchCode
    class T8GSCOBJContext {
    private:
        std::unordered_map<UINT16, UINT64> m_gvars{};
        std::unordered_map<UINT32, LPCCH> m_stringRefs{};
    public:
        std::vector<IW23GSCImport> m_linkedImports{};
        GsicInfo m_gsicInfo{};
        opcode::VmInfo* m_vmInfo{};
        std::unordered_map<UINT64, gscclass> m_classes{};
        T8GSCOBJContext();

        /*
         * Get a name for a global var ref
         * @param gvarRef ref
         * @return name or 0
         */
        UINT64 GetGlobalVarName(UINT16 gvarRef);
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
        UINT16 AddGlobalVarName(UINT64 value);
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
        INT32 cseg_offset;
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
        INT32 requires_implements_offset;
        INT32 ukn50;
        INT32 cseg_size;
        UINT16 include_count;
        BYTE ukn5a;
        BYTE requires_implements_count;

        // @return the vm
        inline BYTE GetVm() {
            return magic[7];
        }
    };
    struct T937GSCOBJ {
        byte magic[8];
        uint32_t crc;
        uint32_t pad0c;
        uint64_t name;
        uint32_t includes_table;
        uint16_t string_count;
        uint16_t export_count;
        uint32_t cseg_offset;
        uint32_t string_offset;
        uint16_t imports_count;
        uint16_t fixup_count;
        uint32_t unk2c;
        uint32_t exports_tables;
        uint32_t imports_offset;
        uint16_t globalvar_count;
        uint16_t unk3a;
        uint32_t fixup_offset;
        uint32_t globalvar_offset;
        uint32_t file_size;
        uint32_t unk48;
        uint32_t cseg_size;
        uint16_t includes_count;
        uint16_t unk52;
        uint32_t unk54;

        // @return the vm
        inline BYTE GetVm() {
            return magic[7];
        }
    };

    struct T9GSCOBJ {
        BYTE magic[8];
        INT32 crc;
        INT32 pad0c;
        UINT64 name;
        UINT16 string_count;
        UINT16 exports_count;
        UINT16 imports_count;
        UINT16 unk1e;
        UINT16 globalvar_count;
        UINT16 unk22;
        UINT16 includes_count;
        UINT16 unk26;
        UINT32 unk28;
        UINT32 cseg_offset;
        UINT32 string_offset;
        UINT32 includes_table;
        UINT32 exports_tables;
        UINT32 import_tables;
        UINT32 unk40;
        UINT32 globalvar_offset;
        UINT32 file_size;
        UINT32 unk4c;
        UINT32 cseg_size;
        UINT32 unk54;

        // @return the vm
        inline BYTE GetVm() {
            return magic[7];
        }
    };

    struct GscObj23 {
        byte magic[8];
        uint64_t name;
        uint16_t unk10;
        uint16_t animtree_use_count;
        uint16_t animtree_count;
        uint16_t unk16;
        uint16_t export_count;
        uint16_t fixup_count;
        uint16_t unk1C;
        uint16_t imports_count;
        uint16_t includes_count;
        uint16_t unk22;
        uint16_t string_count;
        uint16_t unk26;
        uint16_t unk28;
        uint16_t unk2A;
        uint32_t animtree_use_offset;
        uint32_t animtree_offset;
        uint32_t cseg_offset;
        uint32_t cseg_size;
        uint32_t unk3C;
        uint32_t export_offset;
        uint32_t fixup_offset;
        uint32_t size1;
        uint32_t import_table;
        uint32_t include_table;
        uint32_t size2;
        uint32_t string_table;
        uint32_t unk5C;

        UINT64 GetMagic() {
            return *reinterpret_cast<UINT64*>(magic);
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
    };

    struct GSCExportReader {
        virtual void SetHandle(void* handle) = 0;
        virtual uint64_t GetName() = 0;
        virtual uint64_t GetNamespace() = 0;
        virtual uint64_t GetFileNamespace() = 0;
        virtual uint64_t GetChecksum() = 0;
        virtual uint32_t GetAddress() = 0;
        virtual uint8_t GetParamCount() = 0;
        virtual uint8_t GetFlags() = 0;
        virtual size_t SizeOf() = 0;
    };

    void DumpFunctionHeader(GSCExportReader& exp, std::ostream& out, GSCOBJReader& gscFile, T8GSCOBJContext& ctx, tool::gsc::opcode::ASMContext& asmctx, int padding = 0);
    int DumpAsm(GSCExportReader& exp, std::ostream& out, GSCOBJReader& gscFile, T8GSCOBJContext& ctx, tool::gsc::opcode::ASMContext& asmctx);
    int DumpVTable(GSCExportReader& exp, std::ostream& out, GSCOBJReader& gscFile, T8GSCOBJContext& objctx, opcode::ASMContext& ctx, opcode::DecompContext& dctxt);
    /*
     * Compute the size of this export's bytecode
     * @param gscFile origin gsc file
     * @param plt origin platform
     * @param vm origin vm
     * @return size or 0 if a bad opcode was found
     */
    int ComputeSize(GSCExportReader& exp, BYTE* gscFile, gsc::opcode::Platform plt, gsc::opcode::VmInfo* vminfo);

    struct IW23GSCImport {
        UINT64 name;
        UINT64 name_space;
        UINT16 num_address;
        UINT8 param_count;
        UINT8 flags;
    };

    struct IW23GSCExport {
        uint64_t name;
        uint64_t name_space;
        uint64_t file_name_space;
        uint64_t checksum;
        uint32_t address;
        uint8_t param_count;
        uint8_t flags;
    };

    class GSCOBJReader {
    public:
        BYTE* file;
        const GscInfoOption& opt;

        GSCOBJReader(BYTE* file, const GscInfoOption& opt);

        /*
         * Return a pointer at a particular shift
         * @param shift in the file
         * @param T align type
         * @return Pointer
         */
        template<typename T = BYTE>
        inline T* Ptr(size_t shift = 0) {
            return reinterpret_cast<T*>(file + shift);
        }

        /*
         * Return a pointer at a particular shift aligned to a type
         * @param shift in the file
         * @param T align type
         * @param R return type
         * @return Pointer
         */
        template<typename T = BYTE, typename R = T>
        inline R* PtrAlign(size_t shift = 0) {
            return reinterpret_cast<R*>(utils::Aligned<T>(file + shift));
        }

        /*
         * Return a reference to a particular shift
         * @param shift in the file
         * @param T align type
         * @return Ref
         */
        template<typename T = BYTE>
        inline T& Ref(size_t shift = 0) {
            return *Ptr<T>(shift);
        }

        /*
         * Return a reference to a particular shift aligned to a type
         * @param shift in the file
         * @param T align type
         * @param R return type
         * @return Ref
         */
        template<typename T = BYTE, typename R = T>
        inline R& RefAlign(size_t shift = 0) {
            return *PtrAlign<T, R>(shift);
        }

        BYTE GetVM() {
            return file[7];
        }

        virtual UINT64 GetName() = 0;
        virtual bool IsValidHeader(size_t size) = 0;
        virtual UINT16 GetExportsCount() = 0;
        virtual UINT32 GetExportsOffset() = 0;
        virtual UINT16 GetIncludesCount() = 0;
        virtual UINT32 GetIncludesOffset() = 0;
        virtual UINT16 GetImportsCount() = 0;
        virtual UINT32 GetImportsOffset() = 0;
        virtual UINT16 GetStringsCount() = 0;
        virtual UINT32 GetStringsOffset() = 0;
        virtual UINT16 GetGVarsCount() = 0;
        virtual UINT32 GetGVarsOffset() = 0;
        virtual UINT32 GetFileSize() = 0;
        virtual size_t GetHeaderSize() = 0;
        virtual char* DecryptString(char* str) = 0;
        virtual BYTE RemapFlagsImport(BYTE flags);
        virtual BYTE RemapFlagsExport(BYTE flags);
        virtual UINT16 GetAnimTreeSingleCount() = 0;
        virtual UINT32 GetAnimTreeSingleOffset() = 0;
        virtual UINT16 GetAnimTreeDoubleCount() = 0;
        virtual UINT32 GetAnimTreeDoubleOffset() = 0;

        virtual void DumpHeader(std::ostream& asmout) = 0;
        virtual void PatchCode(T8GSCOBJContext& ctx);
        virtual void DumpExperimental(std::ostream& asmout, const GscInfoOption& opt);
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

    enum T9GSCExportFlags : UINT8 {
        T9_EF_AUTOEXEC = 0x01,
        T9_EF_LINKED = 0x02,
        T9_EF_PRIVATE = 0x04,
        T9_EF_CLASS_MEMBER = 0x08,
        T9_EF_CLASS_DESTRUCTOR = 0x10,
        T9_EF_EVENT = 0x20,
        T9_EF_VE = 0x40,
        T9_EF_CLASS_LINKED = 0x80,
        T9_EF_CLASS_VTABLE = 0x15
    };

    enum T9GSCImportFlags : UINT8 {
        T9_IF_METHOD_CHILDTHREAD = 0x1,
        T9_IF_METHOD_THREAD = 0x2,
        T9_IF_FUNCTION_CHILDTHREAD = 0x3,
        T9_IF_FUNCTION = 0x4,
        T9_IF_FUNC_METHOD = 0x5,
        T9_IF_FUNCTION_THREAD = 0x6,
        T9_IF_METHOD = 0x7,
        T9_IF_CALLTYPE_MASK = 0xF,
        T9_IF_DEV_CALL = 0x10,
        T9_IF_GET_CALL = 0x20,
        T9_IF_UKN40 = 0x40,
        T9_IF_UKN80 = 0x80
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
     * @param reader reader
     */
    void RosettaStartFile(GSCOBJReader& reader);
    /*
     * Add rosetta opcode data
     * @param loc opcode location
     * @param opcode opcode
     */
    void RosettaAddOpCode(UINT32 loc, UINT16 opcode);

    // gsc tool
    int gscinfo(Process& proc, int argc, const char* argv[]);
}
template<>
struct std::formatter<tool::gsc::opcode::ASMContextNode, char> : utils::BasicFormatter<tool::gsc::opcode::ASMContextNode> {};