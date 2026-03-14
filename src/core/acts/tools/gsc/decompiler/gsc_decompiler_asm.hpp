#pragma once
#include <tools/gsc/gsc_opcodes_load.hpp>
#include <tools/gsc/gsc_decompiler.hpp>

namespace tool::gsc {
    struct T8GSCExport;
    struct IW23GSCImport;
    class T8GSCOBJContext;
    class GSCOBJHandler;
    struct GSCExportReader;
    struct GscDecompilerGDBData;
    struct GscDecompilerGDBData;

    namespace opcode {
        using namespace tool::gsc;

        struct ASMContextLocation;
        class ASMContext;
        class OPCodeInfo;
        class ASMContextNodeBlock;

        struct DecompContext {
            int padding{};
            int rloc{};
            const GscInfoOption& opt;
            int paddingPre{};
            uint32_t baseloc{};
            GscDecompilerGDBData* gdbData{};
            utils::LineStreamBuf* lineBuf{};

            std::ostream& WritePadding(std::ostream& out, bool forceNoRLoc = false);
        };
        enum ASMContextNodePriority : UINT {
            PRIORITY_INST,

            PRIORITY_SET,
            PRIORITY_TERNARY,

            PRIORITY_COALESCE,
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
            TYPE_COMMENT,
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
            TYPE_JUMP_ONDEFINED,

            TYPE_SWITCH_PRECOMPUTE,
            TYPE_SWITCH_POSTCOMPUTE,
            TYPE_TRY_PRECOMPUTE,
            TYPE_CATCH_PRECOMPUTE,

            TYPE_CONST_HASH,
            TYPE_STRING,
            TYPE_VECTOR,
            TYPE_VECTORN,
            TYPE_FLOAT,

            TYPE_ARRAY_FIRSTKEY,
            TYPE_ARRAY_NEXTKEY,
            TYPE_ARRAY_NEXTKEYPUSH,
            TYPE_ARRAY_NEXTKEY_IT,
            TYPE_ARRAY_NEXTVAL_IT,
            TYPE_ARRAY_NEXT_IT,

            TYPE_ARRAY_BUILD,
            TYPE_STRUCT_BUILD,

            TYPE_FUNC_CALL,
            TYPE_FUNC_REFNAME,
            TYPE_FUNC_IS_DEFINED,
            TYPE_FUNC_IS_DEFINED_OP,
            TYPE_GET_REF,

            TYPE_PRECODEPOS,
            TYPE_SET,
            TYPE_SET_OP,
            TYPE_WAITTILL,
            TYPE_WAITTILL_SET,
            TYPE_DELTA,
            TYPE_ACCESS,
            TYPE_ARRAY_ACCESS,
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

            TYPE_JUMP_STACK_TOP,
            TYPE_GET_CODE_REF,
        };

        inline bool IsJumpType(ASMContextNodeType type) {
            switch (type) {
            case TYPE_JUMP:
            case TYPE_JUMP_ONFALSE:
            case TYPE_JUMP_ONTRUE:
            case TYPE_JUMP_LOWERTHAN:
            case TYPE_JUMP_ONDEFINED:
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
            T9_VAR_REF = 0x04, // T9
            IW_VARIADIC_COUNT = 0x80, // Special value
        };


        struct SubNodeContext {
            ASMContext& ctx;
            ASMContextLocation* stmtRloc{};
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
            virtual ASMContextNode* Clone0() const;
            ASMContextNode* Clone() const;
            virtual ASMContextNode* ConvertToBool();
            virtual int64_t GetIntConst() const;
            virtual bool IsIntConst() const;
            virtual bool IsBoolConvertable(bool strict, ASMContext& ctx);
            virtual bool IsConstNumber() const { return IsIntConst() || m_type == TYPE_FLOAT; };

            virtual void ApplySubBlocks(const std::function<void(ASMContextNodeBlock* block, ASMContext& ctx)>&, ASMContext& ctx);
            virtual void ApplySubNodes(const std::function<void(ASMContextNode*& node, SubNodeContext& ctx)>& func, SubNodeContext& ctx);
            friend std::ostream& operator<<(std::ostream& os, const ASMContextNode& obj);
        };

        class ASMContextLocationOp {
        public:
            const char* type;
            ASMContextLocationOp(const char* type) : type(type) {}
            virtual ~ASMContextLocationOp();
            virtual void Run(ASMContext& context, T8GSCOBJContext& objctx) const;
        };

        struct ASMContextLocation {
            int32_t rloc = 0;
            bool handled = false;
            std::set<int32_t> refs{};
            ASMContextNode* objectId = nullptr;
            ASMContextNode* fieldId = nullptr;
            std::vector<ASMContextNode*> m_stack{};
            std::vector<ASMContextLocationOp*> m_lateop{};
            ASMContextLocation();
            ~ASMContextLocation();

            void RemoveRef(int32_t origin);
        };

        struct ASMContextStatement {
            ASMContextNode* node = nullptr;
            ASMContextLocation* location;
        };

        enum ASMContextNodeBlockType {
            BLOCK_DEFAULT,
            BLOCK_DEV,
            BLOCK_PADDING
        };

        class ASMContextNodeBlock : public ASMContextNode {
        public:
            std::vector<ASMContextStatement> m_statements{};
            ASMContextNodeBlockType m_blockType;
            bool m_disabled;
            bool m_allowInline;
            ASMContextNodeBlock(ASMContextNodeBlockType blockType = BLOCK_DEFAULT, bool disabled = false, bool allowInline = true);
            ~ASMContextNodeBlock();
            void Dump(std::ostream& out, DecompContext& ctx) const override;
            ASMContextNode* Clone0() const override;
            int ComputeDevBlocks(ASMContext& ctx);
            int ComputeForEachBlocksKeys(ASMContext& ctx);
            int ComputeForEachBlocksIterator(ASMContext& ctx);
            int ComputeForEachBlocks(ASMContext& ctx);
            int ComputeForBlocks(ASMContext& ctx);
            int ComputeWhileBlocks(ASMContext& ctx);
            int ComputeIfBlocks(ASMContext& ctx);
            int ComputeSwitchBlocks(ASMContext& ctx);
            int ComputeReturnJump(ASMContext& ctx);
            int ComputeBoolReturn(ASMContext& ctx);
            int ComputeSpecialPattern(ASMContext& ctx);
            int ComputePreSpecialPattern(ASMContext& ctx);
            int ComputeCustomCompilerPattern(ASMContext& ctx);

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

        class ASMContext {
        public:
            uint32_t funcRloc;
            // cli opt
            const GscInfoOption& m_opt;
            // object context
            T8GSCOBJContext& m_objctx;
            // run the decompiler logic
            bool m_runDecompiler;
            // find error
            std::string m_disableDecompilerError{};
            bool m_disableDecompiler{};
            // fonction start location
            byte* m_fonctionStart;
            // script reader
            GSCOBJHandler& m_gscReader;
            // locations
            std::map<int32_t, ASMContextLocation> m_locs{};
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
            // used animtree
            const char* useAnimTree{};
            bool noFunctionPrefix{};
            // if this function is a dev function candidate
            bool m_devFuncCandidate{};
            bool m_boolFuncCandidate{};
            bool m_vtable{};
            // export
            GSCExportReader& m_exp;
            void* m_readerHandle;
            // file vm
            uint64_t m_vm;
            // file platform
            Platform m_platform;

            ASMContext(byte* fonctionStart, GSCOBJHandler& gscReader, T8GSCOBJContext& objctx, const GscInfoOption& opt, uint64_t nsp, GSCExportReader& exp, void* m_readerHandle, uint64_t vm, Platform platform);
            ~ASMContext();

            // @return relative location in the function
            inline int32_t FunctionRelativeLocation() {
                return FunctionRelativeLocation(m_bcl);
            }
            // @return absolute location in the script
            inline uint32_t ScriptAbsoluteLocation() {
                return ScriptAbsoluteLocation(m_bcl);
            }
            // @return if we are in the script
            inline bool IsInsideScript() {
                return IsInsideScript(m_bcl);
            }
            // @return Push the current location to the locations
            inline ASMContextLocation& PushLocation() {
                return PushLocation(m_bcl);
            }
            // @return Push a location to the locations and return it
            ASMContextLocation& PushLocation(byte* location);

            // @return find the next location to handle, false if no new location can be find, false otherwise
            bool FindNextLocation();

            /*
             * test a loc is inside the script
             * @param bytecodeLocation Location
             * @return If the location is inside the script
             */
            bool IsInsideScript(byte* bytecodeLocation);
            /*
             * Get a relative location from the function start
             * @param bytecodeLocation Location
             * @return relative from function start
             */
            int32_t FunctionRelativeLocation(byte* bytecodeLocation);
            /*
             * Get the absolute location from the script start
             * @param location location
             * @return absolute location from script start
             */
            uint32_t ScriptAbsoluteLocation(byte* location);
            /*
             * Check we are inside the script
             * @param location pointer
             */
            void CheckInsideScript(byte* location);

            // @return if the endian is switched
            bool SwitchEndian() const;

            inline void CheckInsideScript() {
                CheckInsideScript(m_bcl);
            }
            // @return align and return m_bcl on a particular datatype
            template<typename Type>
            inline byte*& Aligned() {
                CheckInsideScript();
                return m_bcl = utils::Aligned<Type>(m_bcl);
            }
            template<typename Type>
            Type Read(byte* loc = m_bcl);

            template<typename Type>
            inline void Read(Type* out, size_t count = 1, byte* loc = m_bcl) {
                CheckInsideScript(loc);
                CheckInsideScript(loc + (sizeof(Type) * count - 1));
                if ((byte*)out != loc) {
                    std::memcpy(out, loc, sizeof(Type) * count);
                }
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
             * @param resetLoc reset the location after the statement
             */
            void CompleteStatement(bool resetLoc = true);

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
             * Compute the special pattern candidates
             */
            inline void ComputePreSpecialPattern() {
                m_funcBlock.ComputePreSpecialPattern(*this);
            }
            /*
             * Compute the custom compiler pattern candidates
             */
            inline void ComputeCustomCompilerPattern() {
                m_funcBlock.ComputeCustomCompilerPattern(*this);
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
            /*
             * Disable the decompiler
             */
            void DisableDecompiler(const std::string& reason);
        };

        template<typename Type>
        Type ASMContext::Read(byte* loc) {
            CheckInsideScript(loc);
            CheckInsideScript(loc + (sizeof(Type) - 1));
            Type t{ *(Type*)loc };
            if (SwitchEndian()) {
                utils::SwapByte(&t, sizeof(Type));
            }
            return t;
        }


        size_t SizeNoEmptyNode(const std::vector<ASMContextStatement>& statements);
        ASMContextStatement* GetNoEmptyNode(std::vector<ASMContextStatement>& statements, size_t index);
        void PatchNumber(ASMContextNode*& node, double value);
        void PatchNumber(ASMContextNode*& node, int64_t value);
        void PatchNumber(ASMContextNode*& node, ASMContextNode* other);
        float GetFloatNumber(ASMContextNode* node);
        bool IsEqualNumber(ASMContextNode* a, ASMContextNode* b);
        bool IsStructSimilar(ASMContextNode* a, ASMContextNode* b);

        /*
         * Convert a node to a boolean node, the input value might be free
         * @param node node to convert
         * @return boolean version of node or node
         */
        ASMContextNode* ASMCNodeConvertToBool(ASMContextNode* node, ASMContext& ctx);
    }
}
template<>
struct std::formatter<tool::gsc::opcode::ASMContextNode, char> : utils::BasicFormatter<tool::gsc::opcode::ASMContextNode> {};