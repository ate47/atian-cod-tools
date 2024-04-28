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
        STEPSKIP_DEVBLOCK_INLINE = 0x100,
        STEPSKIP_CLASSMEMBER_INLINE = 0x200,
        STEPSKIP_SPECIAL_PATTERN = 0x400,
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
        const char* m_rosetta{};
        const char* m_dump_hashmap{};
        const char* m_dump_strings{};
        const char* m_outputDir{};
        const char* m_copyright{};
        bool m_show_internal_blocks{};
        bool m_show_func_vars{};
        bool m_mark_jump_type{};
        bool m_display_stack{};
        uint32_t m_stepskip{};
        opcode::Platform m_platform{ opcode::Platform::PLATFORM_PC };
        opcode::VM m_vm{ opcode::VM::VM_UNKNOWN };
        const formatter::FormatterInfo* m_formatter{};


        std::vector<const char*> m_inputFiles{};

        GscInfoOption();
        /*
         * Compute options
         * @param args Arguments
         * @param startIndex Start index in args
         * @param startIndex End index in args (exclusive)
         * @return if the arguments were correctly computed
         */
        bool Compute(const char** args, INT startIndex, INT endIndex);
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
        struct asmcontextlocation;
        class ASMContext;
        class ASMSkipContext;
        class OPCodeInfo;
        class ASMContextNodeBlock;

        struct DecompContext {
            int padding{};
            int rloc{};
            const GscInfoOption& opt;
            int paddingPre{};

            std::ostream& WritePadding(std::ostream& out, bool forceNoRLoc = false);
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
            TYPE_VECTOR,
            TYPE_FLOAT,

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
            TYPE_SET_OP,
            TYPE_WAITTILL,
            TYPE_WAITTILL_SET,
            TYPE_DELTA,
            TYPE_ACCESS,
            TYPE_END,
            TYPE_RETURN,
            TYPE_NEW,

            TYPE_UNDEFINED,
            TYPE_VALUE,
            TYPE_OPERATION_MERGE,
            TYPE_OPERATION1,
            TYPE_OPERATION2,
            TYPE_SELF,
            TYPE_GET_UNDEFINED,

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
        enum T8GSCLocalVarFlag : uint8_t {
            ARRAY_REF = 0x01,
            VARIADIC = 0x02,
            T9_VAR_REF = 0x04 // T9
        };

        class OPCodeInfo {
        public:
            OPCode m_id;
            const char* m_name;

            OPCodeInfo(OPCode id, const char* name);
            virtual int Dump(std::ostream& out, uint16_t value, ASMContext& context, T8GSCOBJContext& objctx) const;
            virtual int Skip(uint16_t value, ASMSkipContext& ctx) const;
        };

        struct SubNodeContext {
            ASMContext& ctx;
            asmcontextlocation* stmtRloc{};
            size_t devBlockDepth{};
        };

        class ASMContextNode {
        public:
            ASMContextNodePriority m_priority;
            ASMContextNodeType m_type;
            bool m_renderRefIfAny = true;
            bool m_renderSemicolon = true;
            int32_t m_rlocEstimated = -1;

            ASMContextNode(ASMContextNodePriority priority, ASMContextNodeType type = TYPE_UNDEFINED);
            virtual ~ASMContextNode();
            virtual void Dump(std::ostream& out, DecompContext& ctx) const;
            virtual ASMContextNode* Clone() const;
            virtual ASMContextNode* ConvertToBool();
            virtual int64_t GetIntConst() const;
            virtual bool IsIntConst() const;
            virtual bool IsBoolConvertable(bool strict);
            virtual bool IsConstNumber() const { return IsIntConst() || m_type == TYPE_FLOAT; };

            virtual void ApplySubBlocks(const std::function<void(ASMContextNodeBlock* block, ASMContext& ctx)>&, ASMContext& ctx);
            virtual void ApplySubNodes(const std::function<void(ASMContextNode*& node, SubNodeContext& ctx)>& func, SubNodeContext& ctx);
            friend std::ostream& operator<<(std::ostream& os, const ASMContextNode& obj);
        };

        class ASMContextLocationOp{
        public:
            const char* type;
            ASMContextLocationOp(const char* type) : type(type) {}
            virtual ~ASMContextLocationOp();
            virtual void Run(ASMContext& context, T8GSCOBJContext& objctx) const;
        };

        struct asmcontextlocation {
            int32_t rloc = 0;
            bool handled = false;
            std::set<int32_t> refs{};
            ASMContextNode* objectId = nullptr;
            ASMContextNode* fieldId = nullptr;
            std::vector<ASMContextNode*> m_stack{};
            std::vector<ASMContextLocationOp*> m_lateop{};
            asmcontextlocation();
            ~asmcontextlocation();

            void RemoveRef(int32_t origin);
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
            bool m_disabled;
            ASMContextNodeBlock(nodeblocktype blockType = BLOCK_DEFAULT, bool disabled = false);
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
            int ComputeSpecialPattern(ASMContext& ctx);

            ASMContextStatement* FetchFirstForLocation(int64_t rloc);

            void ApplySubBlocks(const std::function<void(ASMContextNodeBlock* block, ASMContext& ctx)>&, ASMContext& ctx) override;
            void ApplySubNodes(const std::function<void(ASMContextNode*& node, SubNodeContext& ctx)>& func, SubNodeContext& ctx) override;
        };

        struct ASMContextDevBlock {
            uint32_t rlocStart;
            uint16_t delta;
        };

        struct ASMContextLocalVar {
            uint64_t name;
            byte flags;
            ASMContextNode* defaultValueNode = nullptr;
        };

        class ASMSkipContext {
        public:
            // fonction start location
            byte* m_fonctionStart;
            // file platform
            Platform m_platform;
            // object context
            VmInfo* m_vminfo;
            // locations
            std::map<int32_t, asmcontextlocation> m_locs{};
            // current context location
            byte* m_bcl;
            // error
            std::string m_error{};

            ASMSkipContext(byte* fonctionStart, Platform platform, VmInfo* m_vm);

            // @return align and return m_bcl on a particular datatype
            template<typename Type>
            inline byte*& Aligned() {
                return m_bcl = utils::Aligned<Type>(m_bcl);
            }

            inline int32_t FunctionRelativeLocation(byte* bytecodeLocation) {
                return (int32_t)(reinterpret_cast<uintptr_t>(bytecodeLocation) - reinterpret_cast<uintptr_t>(m_fonctionStart));
            }
            inline int32_t FunctionRelativeLocation() {
                return FunctionRelativeLocation(m_bcl);
            }
            // Push the current location to the locations
            inline asmcontextlocation& PushLocation() {
                return PushLocation(m_bcl);
            }
            // @return Push a location to the locations and return it
            asmcontextlocation& PushLocation(byte* location);

            // find next location
            bool FindNextLocation();
            inline const tool::gsc::opcode::OPCodeInfo* LookupOpCode(uint16_t opcode) {
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
            byte* m_fonctionStart;
            // script reader
            GSCOBJReader& m_gscReader;
            // locations
            std::map<int32_t, asmcontextlocation> m_locs{};
            // current context location
            byte* m_bcl;
            // last op base
            int32_t m_lastOpCodeBase;
            // export namespace
            uint64_t m_namespace;
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
            std::unordered_map<uint64_t, int32_t> m_localvars_ref{};
            // dev blocks locations
            std::vector<ASMContextDevBlock> m_devBlocks{};
            // if this function is a dev function candidate
            bool m_devFuncCandidate{};
            // export
            GSCExportReader& m_exp;
            void* m_readerHandle;
            // file vm
            byte m_vm;
            // file platform
            Platform m_platform;

            ASMContext(byte* fonctionStart, GSCOBJReader& gscReader, T8GSCOBJContext& objctx, const GscInfoOption& opt, uint64_t nsp, GSCExportReader& exp, void* m_readerHandle, byte vm, Platform platform);
            ~ASMContext();

            // @return relative location in the function
            inline int32_t FunctionRelativeLocation() {
                return FunctionRelativeLocation(m_bcl);
            }
            // Push the current location to the locations
            inline asmcontextlocation& PushLocation() {
                return PushLocation(m_bcl);
            }
            // @return Push a location to the locations and return it
            asmcontextlocation& PushLocation(byte* location);

            // @return find the next location to handle, false if no new location can be find, false otherwise
            bool FindNextLocation();
            /*
             * Get a relative location from the function start
             * @param bytecodeLocation Location
             * @return relative from function start
             */
            int32_t FunctionRelativeLocation(byte* bytecodeLocation);
            // @return align and return m_bcl on a particular datatype
            template<typename Type>
            inline byte*& Aligned() {
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
            inline const tool::gsc::opcode::OPCodeInfo* LookupOpCode(uint16_t opcode) {
                return tool::gsc::opcode::LookupOpCode(m_vm, m_platform, opcode);
            }

            /*
             * Get a local var id by its name
             * @param name Var name
             * @return local var id or -1 if not defined in this context
             */
            int GetLocalVarIdByName(uint64_t name) const;

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
            inline void ComputeDevBlocks() {
                m_funcBlock.ComputeDevBlocks(*this);
            }
            /*
             * Compute the switch blocks
             */
            inline void ComputeSwitchBlocks() {
                m_funcBlock.ComputeSwitchBlocks(*this);
            }
            /*
             * Compute the for blocks
             */
            inline void ComputeForBlocks() {
                m_funcBlock.ComputeForBlocks(*this);
            }
            /*
             * Compute the for each blocks
             */
            inline void ComputeForEachBlocks() {
                m_funcBlock.ComputeForEachBlocks(*this);
            }
            /*
             * Compute the while blocks
             */
            inline void ComputeWhileBlocks() {
                m_funcBlock.ComputeWhileBlocks(*this);
            }
            /*
             * Compute the if blocks
             */
            inline void ComputeIfBlocks() {
                m_funcBlock.ComputeIfBlocks(*this);
            }
            /*
             * Compute the return candidates
             */
            inline void ComputeReturnJump() {
                m_funcBlock.ComputeReturnJump(*this);
            }
            /*
             * Compute the boolean return candidates
             */
            inline void ComputeBoolReturn() {
                m_funcBlock.ComputeBoolReturn(*this);
            }
            /*
             * Compute the special pattern candidates
             */
            inline void ComputeSpecialPattern() {
                m_funcBlock.ComputeSpecialPattern(*this);
            }

            /*
             * Compute the for sub nodes
             */
            inline void ForSubNodes(const std::function<void(ASMContextNode*& node, SubNodeContext& ctx)>& func) {
                SubNodeContext sctx{ *this };
                m_funcBlock.ApplySubNodes(func, sctx);
            }
            /*
             * Convert this context to a class method
             * @param selfmembers The members located inside this context
             */
            void ConvertToClassMethod(std::unordered_set<uint64_t>& selfmembers);

            /*
             * Dump the function block
             */
            inline void Dump(std::ostream& out, DecompContext& ctx) const {
                m_funcBlock.Dump(out, ctx);
            }
        };
    }

    struct GsicDetour {
        uint32_t name;
        uint32_t replaceNamespace;
        uint32_t replaceFunction;
        int32_t fixupOffset;
        int32_t fixupSize;
        int32_t replaceScriptTop;
        int32_t replaceScriptBottom;
    };
    struct GsicInfo {
        bool isGsic = false;
        size_t headerSize = 0;
        std::unordered_map<uint64_t, GsicDetour*> detours{};
    };
    struct asmcontext_func {
        uint64_t name;
        uint64_t nsp;
    };
    struct gscclass {
        uint64_t name_space = 0;
        std::set<uint64_t> m_superClass{};
        std::vector<uint64_t> m_methods{};
        std::unordered_map<uint64_t, asmcontext_func> m_vtable{};
    };
    // Result context for T8GSCOBJ::PatchCode
    class T8GSCOBJContext {
    private:
        std::unordered_map<uint16_t, uint64_t> m_gvars{};
        std::unordered_map<uint32_t, const char*> m_stringRefs{};
    public:
        std::vector<IW23GSCImport> m_linkedImports{};
        GsicInfo m_gsicInfo{};
        opcode::VmInfo* m_vmInfo{};
        std::unordered_map<uint64_t, gscclass> m_classes{};
        T8GSCOBJContext();

        /*
         * Get a name for a global var ref
         * @param gvarRef ref
         * @return name or 0
         */
        uint64_t GetGlobalVarName(uint16_t gvarRef);
        /*
         * Get a string for a string string ref
         * @param stringRef ref
         * @return string or null
         */
        const char* GetStringValue(uint32_t stringRef);
        /*
         * Add a global var
         * @param value name
         * @return var ref
         */
        uint16_t AddGlobalVarName(uint64_t value);
        /*
         * Add a string
         * @param value string
         * @return new string ref
         */
        uint32_t AddStringValue(const char* value);


    };

    struct T8GSCOBJ {
        byte magic[8];
        int32_t crc;
        int32_t pad;
        uint64_t name;
        int32_t include_offset;
        uint16_t string_count;
        uint16_t exports_count;
        int32_t cseg_offset;
        int32_t string_offset;
        int16_t imports_count;
        uint16_t fixup_count;
        int32_t ukn2c;
        int32_t export_table_offset;
        int32_t ukn34;
        int32_t imports_offset;
        uint16_t globalvar_count;
        int32_t fixup_offset;
        int32_t globalvar_offset;
        int32_t script_size;
        int32_t requires_implements_offset;
        int32_t ukn50;
        int32_t cseg_size;
        uint16_t include_count;
        byte ukn5a;
        byte requires_implements_count;

        // @return the vm
        inline byte GetVm() {
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
        inline byte GetVm() {
            return magic[7];
        }
    };

    struct T9GSCOBJ {
        byte magic[8];
        int32_t crc;
        int32_t pad0c;
        uint64_t name;
        uint16_t string_count;
        uint16_t exports_count;
        uint16_t imports_count;
        uint16_t unk1e;
        uint16_t globalvar_count;
        uint16_t unk22;
        uint16_t includes_count;
        uint16_t unk26;
        uint32_t unk28;
        uint32_t cseg_offset;
        uint32_t string_offset;
        uint32_t includes_table;
        uint32_t exports_tables;
        uint32_t import_tables;
        uint32_t unk40;
        uint32_t globalvar_offset;
        uint32_t file_size;
        uint32_t unk4c;
        uint32_t cseg_size;
        uint32_t unk54;

        // @return the vm
        inline byte GetVm() {
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

        uint64_t GetMagic() {
            return *reinterpret_cast<uint64_t*>(magic);
        }
    };

    struct T8GSCFixup {
        uintptr_t offset;
        uintptr_t address;
    };

    struct T8GSCImport {
        uint32_t name;
        uint32_t import_namespace;
        uint16_t num_address;
        uint8_t param_count;
        uint8_t flags;
    };

    struct T8GSCGlobalVar {
        uint32_t name;
        uint32_t num_address;
    };
    
    struct T8GSCExport {
        uint32_t checksum;
        uint32_t address;
        uint32_t name;
        uint32_t name_space;
        uint32_t callback_event;
        uint8_t param_count;
        uint8_t flags;
        uint16_t padding;
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

    void DumpFunctionHeader(GSCExportReader& exp, std::ostream& out, GSCOBJReader& gscFile, T8GSCOBJContext& ctx, tool::gsc::opcode::ASMContext& asmctx, int padding = 0, const char* forceName = nullptr);
    int DumpAsm(GSCExportReader& exp, std::ostream& out, GSCOBJReader& gscFile, T8GSCOBJContext& ctx, tool::gsc::opcode::ASMContext& asmctx);
    int DumpVTable(GSCExportReader& exp, std::ostream& out, GSCOBJReader& gscFile, T8GSCOBJContext& objctx, opcode::ASMContext& ctx, opcode::DecompContext& dctxt);
    /*
     * Compute the size of this export's bytecode
     * @param gscFile origin gsc file
     * @param plt origin platform
     * @param vm origin vm
     * @return size or 0 if a bad opcode was found
     */
    int ComputeSize(GSCExportReader& exp, byte* gscFile, gsc::opcode::Platform plt, gsc::opcode::VmInfo* vminfo);

    struct IW23GSCImport {
        uint64_t name;
        uint64_t name_space;
        uint16_t num_address;
        uint8_t param_count;
        uint8_t flags;
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
        byte* file;
        const GscInfoOption& opt;

        GSCOBJReader(byte* file, const GscInfoOption& opt);

        /*
         * Return a pointer at a particular shift
         * @param shift in the file
         * @param T align type
         * @return Pointer
         */
        template<typename T = byte>
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
        template<typename T = byte, typename R = T>
        inline R* PtrAlign(size_t shift = 0) {
            return reinterpret_cast<R*>(utils::Aligned<T>(file + shift));
        }

        /*
         * Return a reference to a particular shift
         * @param shift in the file
         * @param T align type
         * @return Ref
         */
        template<typename T = byte>
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
        template<typename T = byte, typename R = T>
        inline R& RefAlign(size_t shift = 0) {
            return *PtrAlign<T, R>(shift);
        }

        byte GetVM() {
            return file[7];
        }

        virtual uint64_t GetName() = 0;
        virtual bool IsValidHeader(size_t size) = 0;
        virtual uint16_t GetExportsCount() = 0;
        virtual uint32_t GetExportsOffset() = 0;
        virtual uint16_t GetIncludesCount() = 0;
        virtual uint32_t GetIncludesOffset() = 0;
        virtual uint16_t GetImportsCount() = 0;
        virtual uint32_t GetImportsOffset() = 0;
        virtual uint16_t GetStringsCount() = 0;
        virtual uint32_t GetStringsOffset() = 0;
        virtual uint16_t GetGVarsCount() = 0;
        virtual uint32_t GetGVarsOffset() = 0;
        virtual uint32_t GetFileSize() = 0;
        virtual size_t GetHeaderSize() = 0;
        virtual char* DecryptString(char* str) = 0;
        virtual byte RemapFlagsImport(byte flags);
        virtual byte RemapFlagsExport(byte flags);
        virtual uint16_t GetAnimTreeSingleCount() = 0;
        virtual uint32_t GetAnimTreeSingleOffset() = 0;
        virtual uint16_t GetAnimTreeDoubleCount() = 0;
        virtual uint32_t GetAnimTreeDoubleOffset() = 0;

        virtual void DumpHeader(std::ostream& asmout) = 0;
        virtual void PatchCode(T8GSCOBJContext& ctx);
        virtual void DumpExperimental(std::ostream& asmout, const GscInfoOption& opt);
    };

    enum T8GSCExportFlags : uint8_t {
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

    enum T8GSCImportFlags : uint8_t {
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

    enum T9GSCExportFlags : uint8_t {
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

    enum T9GSCImportFlags : uint8_t {
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
        uint32_t string;
        uint8_t num_address;
        uint8_t type;
        uint16_t pad;
    };

    enum RosettaBlockType : byte {
        RBT_START = 0x50,
        RBT_OPCODE = 0x51,
    };
    struct RosettaOpCodeBlock {
        uint32_t location;
        uint16_t opcode;
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
    void RosettaAddOpCode(uint32_t loc, uint16_t opcode);

    // gsc tool
    int gscinfo(Process& proc, int argc, const char* argv[]);
}
template<>
struct std::formatter<tool::gsc::opcode::ASMContextNode, char> : utils::BasicFormatter<tool::gsc::opcode::ASMContextNode> {};