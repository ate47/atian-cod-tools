#include <includes.hpp>
#include <games/bo4/pool.hpp>
#include <deps/idc_builder.hpp>

namespace {
    union DvarValue {
        bool enabled;
        int integer;
        uint32_t unsignedInt;
        int64_t integer64;
        uint64_t unsignedInt64;
        float value;
        float vector[4];
        const char* string;
        byte color[4];
        uintptr_t indirect[3]; // const dvar_t*
    };

    struct DvarData {
        DvarValue current;
        DvarValue latched;
        DvarValue reset;
        DvarValue unk48;
    };
    enum dvarType_t : __int32 {
        DVAR_TYPE_INVALID = 0x0,
        DVAR_TYPE_BOOL = 0x1,
        DVAR_TYPE_FLOAT = 0x2,
        DVAR_TYPE_FLOAT_2 = 0x3,
        DVAR_TYPE_FLOAT_3 = 0x4,
        DVAR_TYPE_FLOAT_4 = 0x5,
        DVAR_TYPE_INT = 0x6,
        DVAR_TYPE_ENUM = 0x7,
        DVAR_TYPE_STRING = 0x8,
        DVAR_TYPE_COLOR = 0x9,
        DVAR_TYPE_INT64 = 0xA,
        DVAR_TYPE_UINT64 = 0xB,
        DVAR_TYPE_LINEAR_COLOR_RGB = 0xC,
        DVAR_TYPE_COLOR_XYZ = 0xD,
        DVAR_TYPE_COLOR_LAB = 0xE,
        DVAR_TYPE_SESSIONMODE_BASE_DVAR = 0xF,
        DVAR_TYPE_COUNT = 0x10,
    };
    union DvarLimits {
        struct {
            int stringCount;
            const char** strings;
        } enumeration;
        struct {
            int min;
            int max;
        } integer;
        struct {
            int64_t min;
            int64_t max;
        } integer64;
        struct {
            uint64_t min;
            uint64_t max;
        } unsignedInt64;
        struct {
            float min;
            float max;
        } value;
        struct {
            float min;
            float max;
        } vector;
    };

    struct dvar_t {
        XHash name;
        uintptr_t hashnext; // dvar_t*
        uintptr_t value; // DvarData*
        dvarType_t type;
        unsigned int flags;
        DvarLimits domain;
        uint64_t unk38;
    };

    void WriteDVarData(Process& proc, utils::OutFileCE& out, const dvar_t& dvar, const DvarData* value) {
        switch (dvar.type) {
        case DVAR_TYPE_BOOL: {
            out << "BOOL,,"
                << (value[0].current.enabled ? "true" : "false");
            break;
        }
        case DVAR_TYPE_FLOAT: {
            out << "FLOAT,"
                << dvar.domain.value.min << ":" << dvar.domain.value.min << ","
                << value[0].current.value;
            break;
        }
        case DVAR_TYPE_FLOAT_2: {
            out << "FLOAT_2,"
                << dvar.domain.vector.min << ":" << dvar.domain.vector.min << ","
                << value[0].current.vector[0] << " " << value[0].current.vector[1];
            break;
        }
        case DVAR_TYPE_FLOAT_3: {
            out << "FLOAT_3"
                << dvar.domain.vector.min << ":" << dvar.domain.vector.min << ","
                << value[0].current.vector[0] << " " << value[0].current.vector[1] << " " << value[0].current.vector[2];

            break;
        }
        case DVAR_TYPE_FLOAT_4: {
            out << "FLOAT_4"
                << dvar.domain.vector.min << ":" << dvar.domain.vector.min << ","
                << value[0].current.vector[0]
                << " " << value[0].current.vector[1]
                << " " << value[0].current.vector[2]
                << " " << value[0].current.vector[3];

            break;
        }
        case DVAR_TYPE_INT: {
            out << "INT";

            break;
        }
        case DVAR_TYPE_ENUM: {
            out << "ENUM";

            break;
        }
        case DVAR_TYPE_STRING: {
            out << "STRING";

            break;
        }
        case DVAR_TYPE_COLOR: {
            out << "COLOR";

            break;
        }
        case DVAR_TYPE_INT64: {
            out << "INT64";

            break;
        }
        case DVAR_TYPE_UINT64: {
            out << "UINT64";

            break;
        }
        case DVAR_TYPE_LINEAR_COLOR_RGB: {
            out << "LINEAR_COLOR_RGB";

            break;
        }
        case DVAR_TYPE_COLOR_XYZ: {
            out << "COLOR_XYZ";

            break;
        }
        case DVAR_TYPE_COLOR_LAB: {
            out << "COLOR_LAB";

            break;
        }
        case DVAR_TYPE_SESSIONMODE_BASE_DVAR: {
            out << "SESSIONMODE_BASE_DVAR";

            break;
        }
        default: out << "INVALID"; break;
        }
    }

    int dumpdvars(Process& proc, int argc, const char* argv[]) {
        hashutils::ReadDefaultFile();

        const char* outFile;
        if (argc == 2) {
            outFile = "dvars.csv";
        }
        else {
            outFile = argv[2];
        }

        int count = proc.ReadMemory<int>(proc[0xFA0EF8C]);
        auto [pool, ok] = proc.ReadMemoryArray<uintptr_t>(proc[0xFA0EFB0], count);

        if (!ok) {
            LOG_ERROR("Can't read pool");
            return tool::BASIC_ERROR;
        }

        utils::OutFileCE out{ outFile, true };

        dvar_t dvar{};
        DvarData value[4];

        out << "address,name,flags,type,domain,value";

        for (size_t i = 0; i < count; i++) {
            if (!proc.ReadMemory(&dvar, pool[i], sizeof(dvar)) || !proc.ReadMemory(&value[0], dvar.value, sizeof(value[0]))) {
                LOG_INFO("Can't read dvar at index {}", i);
                continue;
            }

            out
                << "\n"
                << proc.GetLocation(pool[i]) << ","
                << hashutils::ExtractTmp("hash", dvar.name.name) << ","
                << "0x" << std::hex << dvar.flags << ","
                ;

            WriteDVarData(proc, out, dvar, value);
            out << ",";
        }

        return tool::OK;
    }

    ADD_TOOL(ddv, "bo4", " [output=dvars.csv]", "dump dvars", L"BlackOps4.exe", dumpdvars);
}