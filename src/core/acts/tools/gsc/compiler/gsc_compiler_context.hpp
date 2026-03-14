#pragma once
#include <tools/gsc/gsc_compiler.hpp>

namespace tool::gsc::compiler {
    using namespace tool::gsc::opcode;

    constexpr size_t CRC_LEN_PADDING = 21;

    class GscCompilerOption;
    class FunctionObject;
    class CompileObject;
    class ACTSErrorListener;
    struct InputInfo;


    struct FunctionVar {
        std::string name;
        size_t id;
        byte flags;
    };

    struct Located {
        uint64_t name_space;
        uint64_t name;
        uint64_t script{};
    };
    struct LocatedHash {
        size_t operator()(const Located& k) const {
            return k.name_space ^ RotateLeft64(k.name, 21) ^ RotateLeft64(k.script, 42);
        }
    };
    struct LocatedEquals {
        bool operator()(const Located& a, const Located& b) const {
            return a.name == b.name && a.name_space == b.name_space && a.script == b.script;
        }
    };

    template<typename Type>
    constexpr size_t maxNumberSize() {
        return (1ull << (sizeof(Type) << 3));
    }

    constexpr int64_t MAX_JUMP = maxNumberSize<uint16_t>();

    class AscmCompilerContext {
    public:
        const VmInfo* vmInfo;
        CompilerConfig& cfg;
        Platform plt;
        std::vector<byte>& data;
        size_t lvars;

        AscmCompilerContext(const VmInfo* vmInfo, Platform plt, size_t lvars, std::vector<byte>& data, CompilerConfig& cfg) : vmInfo(vmInfo), plt(plt), data(data), lvars(lvars), cfg(cfg) {}

        bool HasAlign() const {
            return vmInfo->HasFlag(VmFlags::VMF_ALIGN);
        }

        void Align(size_t len) {
            if (HasAlign()) {
                size_t pre = data.size();
                size_t post = (data.size() + (len - 1)) & ~(len - 1);

                for (size_t i = pre; i < post; i++) {
                    data.push_back(0);
                }
            }
            // not required
        }

        template<typename Type>
        void Align() {
            Align(sizeof(Type));
        }
        template<typename Type>
        void Write(Type value) {
            utils::WriteValue<Type>(data, value);
        }
    };

}