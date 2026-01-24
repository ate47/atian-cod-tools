#pragma once
#include <tools/gsc/compiler/gsc_compiler_grammar.hpp>
#include <tools/gsc/compiler/gsc_compiler_asm.hpp>

namespace tool::gsc::compiler {
    class CompileObject;

    struct FunctionJumpLoc {
        AscmNode* node{};
        ParseTree* def{};
        bool defined{};
    };

    struct DetourData {
        uint64_t nsp{};
        uint64_t script{};
        uint64_t func{};
    };

    class FunctionObject {
    public:
        CompileObject& obj;
        int64_t autoexecOrder{};
        uint64_t m_name;
        uint64_t m_name_space;
        uint64_t m_data_name;
        byte m_params{};
        byte m_flags{};
        size_t location{};
        size_t size{};
        size_t rndVarStart{};
        FunctionVar m_vars[256]{};
        FunctionVar* specialTempVar{};
        size_t m_allocatedVar{};
        std::vector<AscmNode*> m_nodes{};
        std::stack<AscmNode*> m_jumpBreak{};
        std::stack<AscmNode*> m_jumpContinue{};
        std::unordered_map<std::string, FunctionJumpLoc> m_jumpLocs{};
        VmInfo* m_vmInfo;
        DetourData detour{};
        FunctionObject(
            CompileObject& obj,
            uint64_t name,
            uint64_t name_space,
            uint64_t file_name_space,
            VmInfo* vmInfo
        ) : obj(obj), m_name(name), m_name_space(name_space), m_data_name(file_name_space), m_vmInfo(vmInfo) {
        }
        ~FunctionObject() {
            for (AscmNode* node : m_nodes) {
                delete node;
            }
        }

        bool IsDetour() const {
            return detour.func || detour.nsp || detour.script;
        }

        AscmNode* CreateParamNode() const;

        AscmNode* CreateFieldHashRaw(uint64_t v) const;

        AscmNode* CreateFieldHash(uint64_t v, OPCode op) const;

        AscmNode* CreateFieldHash(const char* v, OPCode op) const;

        AscmNode* CreateFieldHash(const std::string& v, OPCode op) const;

        AscmNode* CreateFieldHashRaw(const char* v) const;

        AscmNode* CreateFieldHashRaw(const std::string& v) const;

        AscmNode* PeekBreakNode() const;

        AscmNode* PeekContinueNode() const;

        void PushBreakNode(AscmNode* node);

        void PopBreakNode();

        void PushContinueNode(AscmNode* node);

        void PopContinueNode();

        FunctionVar* VarEnd();

        void AddNode(ParseTree* tree, AscmNode* node);

        void AddNode(decltype(m_nodes)::iterator it, ParseTree* tree, AscmNode* node);


        /*
         * Find a variable with its name
         * @param name name
         * @return iterator from m_vars
         */
        FunctionVar* FindVar(const std::string& name);

        /*
         * Register a variable with a name
         * @param name name
         * @param allowExisting return existing variables
         * @param flags flags
         * @return pair with an error message (for errors) or the variable
         */
        std::pair<const char*, FunctionVar*> RegisterVar(const std::string& name, bool allowExisting, byte flags = 0);

        /*
         * Register a variable with a random name
         * @return same as RegisterVar
         */
        std::pair<const char*, FunctionVar*> RegisterVarRnd();

        /*
         * Get or create the tmp variable for this function
         */
        std::pair<const char*, FunctionVar*> GetSpecialTmpVar();

        /*
         * Compute the nodes relative locations
         * @return -1 in case of error, the size otherwise
         */
        int32_t ComputeRelativeLocations(int32_t floc);

        void OptimizeNodes();
    };


}