#include<includes.hpp>
#include <tools/ff/fastfile_bdiff.hpp>

namespace fastfile::bdiff {
    namespace {
        struct VcdState {
            unsigned int next_slot;
            size_t anear[4];
            size_t asame[768];
            unsigned char* pAddr;
        };


        enum VCD {
            VCD_SELF = 0,
            VCD_HERE = 1
        };

        enum VCDInstructionCode : uint16_t {
            VCDI_NOOP = 0,
            VCDI_ADD = 1,
            VCDI_RUN = 2,
            VCDI_COPY = 3,
            VCDI_COUNT
        };
        static const char* vcdInstructionCodeNames[VCDI_COUNT]{ "NOOP", "ADD", "RUN", "COPY" };

        struct Instruction {
            VCDInstructionCode op;
            uint16_t mode;
            uint32_t size;
        };

        struct DoubleInstruction {
            Instruction instruction[2];
        };
        static DoubleInstruction instructions[256] =
        {
            { { { VCDI_RUN, 0x0, 0x0 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_ADD, 0x0, 0x0 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_ADD, 0x0, 0x1 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_ADD, 0x0, 0x2 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_ADD, 0x0, 0x3 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_ADD, 0x0, 0x4 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_ADD, 0x0, 0x5 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_ADD, 0x0, 0x6 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_ADD, 0x0, 0x7 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_ADD, 0x0, 0x8 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_ADD, 0x0, 0x9 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_ADD, 0x0, 0xa }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_ADD, 0x0, 0xb }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_ADD, 0x0, 0xc }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_ADD, 0x0, 0xd }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_ADD, 0x0, 0xe }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_ADD, 0x0, 0xf }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_ADD, 0x0, 0x10 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_ADD, 0x0, 0x11 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x0, 0x0 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x0, 0x4 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x0, 0x5 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x0, 0x6 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x0, 0x7 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x0, 0x8 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x0, 0x9 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x0, 0xa }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x0, 0xb }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x0, 0xc }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x0, 0xd }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x0, 0xe }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x0, 0xf }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x0, 0x10 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x0, 0x11 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x0, 0x12 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x1, 0x0 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x1, 0x4 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x1, 0x5 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x1, 0x6 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x1, 0x7 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x1, 0x8 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x1, 0x9 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x1, 0xa }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x1, 0xb }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x1, 0xc }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x1, 0xd }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x1, 0xe }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x1, 0xf }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x1, 0x10 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x1, 0x11 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x1, 0x12 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x2, 0x0 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x2, 0x4 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x2, 0x5 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x2, 0x6 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x2, 0x7 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x2, 0x8 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x2, 0x9 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x2, 0xa }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x2, 0xb }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x2, 0xc }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x2, 0xd }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x2, 0xe }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x2, 0xf }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x2, 0x10 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x2, 0x11 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x2, 0x12 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x3, 0x0 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x3, 0x4 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x3, 0x5 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x3, 0x6 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x3, 0x7 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x3, 0x8 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x3, 0x9 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x3, 0xa }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x3, 0xb }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x3, 0xc }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x3, 0xd }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x3, 0xe }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x3, 0xf }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x3, 0x10 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x3, 0x11 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x3, 0x12 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x4, 0x0 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x4, 0x4 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x4, 0x5 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x4, 0x6 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x4, 0x7 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x4, 0x8 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x4, 0x9 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x4, 0xa }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x4, 0xb }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x4, 0xc }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x4, 0xd }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x4, 0xe }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x4, 0xf }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x4, 0x10 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x4, 0x11 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x4, 0x12 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x5, 0x0 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x5, 0x4 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x5, 0x5 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x5, 0x6 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x5, 0x7 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x5, 0x8 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x5, 0x9 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x5, 0xa }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x5, 0xb }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x5, 0xc }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x5, 0xd }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x5, 0xe }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x5, 0xf }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x5, 0x10 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x5, 0x11 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x5, 0x12 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x6, 0x0 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x6, 0x4 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x6, 0x5 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x6, 0x6 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x6, 0x7 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x6, 0x8 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x6, 0x9 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x6, 0xa }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x6, 0xb }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x6, 0xc }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x6, 0xd }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x6, 0xe }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x6, 0xf }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x6, 0x10 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x6, 0x11 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x6, 0x12 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x7, 0x0 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x7, 0x4 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x7, 0x5 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x7, 0x6 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x7, 0x7 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x7, 0x8 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x7, 0x9 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x7, 0xa }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x7, 0xb }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x7, 0xc }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x7, 0xd }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x7, 0xe }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x7, 0xf }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x7, 0x10 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x7, 0x11 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x7, 0x12 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x8, 0x0 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x8, 0x4 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x8, 0x5 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x8, 0x6 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x8, 0x7 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x8, 0x8 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x8, 0x9 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x8, 0xa }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x8, 0xb }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x8, 0xc }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x8, 0xd }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x8, 0xe }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x8, 0xf }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x8, 0x10 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x8, 0x11 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_COPY, 0x8, 0x12 }, { VCDI_NOOP, 0x0, 0x0 } } } ,
            { { { VCDI_ADD, 0x0, 0x1 }, { VCDI_COPY, 0x0, 0x4 } } } ,
            { { { VCDI_ADD, 0x0, 0x1 }, { VCDI_COPY, 0x0, 0x5 } } } ,
            { { { VCDI_ADD, 0x0, 0x1 }, { VCDI_COPY, 0x0, 0x6 } } } ,
            { { { VCDI_ADD, 0x0, 0x2 }, { VCDI_COPY, 0x0, 0x4 } } } ,
            { { { VCDI_ADD, 0x0, 0x2 }, { VCDI_COPY, 0x0, 0x5 } } } ,
            { { { VCDI_ADD, 0x0, 0x2 }, { VCDI_COPY, 0x0, 0x6 } } } ,
            { { { VCDI_ADD, 0x0, 0x3 }, { VCDI_COPY, 0x0, 0x4 } } } ,
            { { { VCDI_ADD, 0x0, 0x3 }, { VCDI_COPY, 0x0, 0x5 } } } ,
            { { { VCDI_ADD, 0x0, 0x3 }, { VCDI_COPY, 0x0, 0x6 } } } ,
            { { { VCDI_ADD, 0x0, 0x4 }, { VCDI_COPY, 0x0, 0x4 } } } ,
            { { { VCDI_ADD, 0x0, 0x4 }, { VCDI_COPY, 0x0, 0x5 } } } ,
            { { { VCDI_ADD, 0x0, 0x4 }, { VCDI_COPY, 0x0, 0x6 } } } ,
            { { { VCDI_ADD, 0x0, 0x1 }, { VCDI_COPY, 0x1, 0x4 } } } ,
            { { { VCDI_ADD, 0x0, 0x1 }, { VCDI_COPY, 0x1, 0x5 } } } ,
            { { { VCDI_ADD, 0x0, 0x1 }, { VCDI_COPY, 0x1, 0x6 } } } ,
            { { { VCDI_ADD, 0x0, 0x2 }, { VCDI_COPY, 0x1, 0x4 } } } ,
            { { { VCDI_ADD, 0x0, 0x2 }, { VCDI_COPY, 0x1, 0x5 } } } ,
            { { { VCDI_ADD, 0x0, 0x2 }, { VCDI_COPY, 0x1, 0x6 } } } ,
            { { { VCDI_ADD, 0x0, 0x3 }, { VCDI_COPY, 0x1, 0x4 } } } ,
            { { { VCDI_ADD, 0x0, 0x3 }, { VCDI_COPY, 0x1, 0x5 } } } ,
            { { { VCDI_ADD, 0x0, 0x3 }, { VCDI_COPY, 0x1, 0x6 } } } ,
            { { { VCDI_ADD, 0x0, 0x4 }, { VCDI_COPY, 0x1, 0x4 } } } ,
            { { { VCDI_ADD, 0x0, 0x4 }, { VCDI_COPY, 0x1, 0x5 } } } ,
            { { { VCDI_ADD, 0x0, 0x4 }, { VCDI_COPY, 0x1, 0x6 } } } ,
            { { { VCDI_ADD, 0x0, 0x1 }, { VCDI_COPY, 0x2, 0x4 } } } ,
            { { { VCDI_ADD, 0x0, 0x1 }, { VCDI_COPY, 0x2, 0x5 } } } ,
            { { { VCDI_ADD, 0x0, 0x1 }, { VCDI_COPY, 0x2, 0x6 } } } ,
            { { { VCDI_ADD, 0x0, 0x2 }, { VCDI_COPY, 0x2, 0x4 } } } ,
            { { { VCDI_ADD, 0x0, 0x2 }, { VCDI_COPY, 0x2, 0x5 } } } ,
            { { { VCDI_ADD, 0x0, 0x2 }, { VCDI_COPY, 0x2, 0x6 } } } ,
            { { { VCDI_ADD, 0x0, 0x3 }, { VCDI_COPY, 0x2, 0x4 } } } ,
            { { { VCDI_ADD, 0x0, 0x3 }, { VCDI_COPY, 0x2, 0x5 } } } ,
            { { { VCDI_ADD, 0x0, 0x3 }, { VCDI_COPY, 0x2, 0x6 } } } ,
            { { { VCDI_ADD, 0x0, 0x4 }, { VCDI_COPY, 0x2, 0x4 } } } ,
            { { { VCDI_ADD, 0x0, 0x4 }, { VCDI_COPY, 0x2, 0x5 } } } ,
            { { { VCDI_ADD, 0x0, 0x4 }, { VCDI_COPY, 0x2, 0x6 } } } ,
            { { { VCDI_ADD, 0x0, 0x1 }, { VCDI_COPY, 0x3, 0x4 } } } ,
            { { { VCDI_ADD, 0x0, 0x1 }, { VCDI_COPY, 0x3, 0x5 } } } ,
            { { { VCDI_ADD, 0x0, 0x1 }, { VCDI_COPY, 0x3, 0x6 } } } ,
            { { { VCDI_ADD, 0x0, 0x2 }, { VCDI_COPY, 0x3, 0x4 } } } ,
            { { { VCDI_ADD, 0x0, 0x2 }, { VCDI_COPY, 0x3, 0x5 } } } ,
            { { { VCDI_ADD, 0x0, 0x2 }, { VCDI_COPY, 0x3, 0x6 } } } ,
            { { { VCDI_ADD, 0x0, 0x3 }, { VCDI_COPY, 0x3, 0x4 } } } ,
            { { { VCDI_ADD, 0x0, 0x3 }, { VCDI_COPY, 0x3, 0x5 } } } ,
            { { { VCDI_ADD, 0x0, 0x3 }, { VCDI_COPY, 0x3, 0x6 } } } ,
            { { { VCDI_ADD, 0x0, 0x4 }, { VCDI_COPY, 0x3, 0x4 } } } ,
            { { { VCDI_ADD, 0x0, 0x4 }, { VCDI_COPY, 0x3, 0x5 } } } ,
            { { { VCDI_ADD, 0x0, 0x4 }, { VCDI_COPY, 0x3, 0x6 } } } ,
            { { { VCDI_ADD, 0x0, 0x1 }, { VCDI_COPY, 0x4, 0x4 } } } ,
            { { { VCDI_ADD, 0x0, 0x1 }, { VCDI_COPY, 0x4, 0x5 } } } ,
            { { { VCDI_ADD, 0x0, 0x1 }, { VCDI_COPY, 0x4, 0x6 } } } ,
            { { { VCDI_ADD, 0x0, 0x2 }, { VCDI_COPY, 0x4, 0x4 } } } ,
            { { { VCDI_ADD, 0x0, 0x2 }, { VCDI_COPY, 0x4, 0x5 } } } ,
            { { { VCDI_ADD, 0x0, 0x2 }, { VCDI_COPY, 0x4, 0x6 } } } ,
            { { { VCDI_ADD, 0x0, 0x3 }, { VCDI_COPY, 0x4, 0x4 } } } ,
            { { { VCDI_ADD, 0x0, 0x3 }, { VCDI_COPY, 0x4, 0x5 } } } ,
            { { { VCDI_ADD, 0x0, 0x3 }, { VCDI_COPY, 0x4, 0x6 } } } ,
            { { { VCDI_ADD, 0x0, 0x4 }, { VCDI_COPY, 0x4, 0x4 } } } ,
            { { { VCDI_ADD, 0x0, 0x4 }, { VCDI_COPY, 0x4, 0x5 } } } ,
            { { { VCDI_ADD, 0x0, 0x4 }, { VCDI_COPY, 0x4, 0x6 } } } ,
            { { { VCDI_ADD, 0x0, 0x1 }, { VCDI_COPY, 0x5, 0x4 } } } ,
            { { { VCDI_ADD, 0x0, 0x1 }, { VCDI_COPY, 0x5, 0x5 } } } ,
            { { { VCDI_ADD, 0x0, 0x1 }, { VCDI_COPY, 0x5, 0x6 } } } ,
            { { { VCDI_ADD, 0x0, 0x2 }, { VCDI_COPY, 0x5, 0x4 } } } ,
            { { { VCDI_ADD, 0x0, 0x2 }, { VCDI_COPY, 0x5, 0x5 } } } ,
            { { { VCDI_ADD, 0x0, 0x2 }, { VCDI_COPY, 0x5, 0x6 } } } ,
            { { { VCDI_ADD, 0x0, 0x3 }, { VCDI_COPY, 0x5, 0x4 } } } ,
            { { { VCDI_ADD, 0x0, 0x3 }, { VCDI_COPY, 0x5, 0x5 } } } ,
            { { { VCDI_ADD, 0x0, 0x3 }, { VCDI_COPY, 0x5, 0x6 } } } ,
            { { { VCDI_ADD, 0x0, 0x4 }, { VCDI_COPY, 0x5, 0x4 } } } ,
            { { { VCDI_ADD, 0x0, 0x4 }, { VCDI_COPY, 0x5, 0x5 } } } ,
            { { { VCDI_ADD, 0x0, 0x4 }, { VCDI_COPY, 0x5, 0x6 } } } ,
            { { { VCDI_ADD, 0x0, 0x1 }, { VCDI_COPY, 0x6, 0x4 } } } ,
            { { { VCDI_ADD, 0x0, 0x2 }, { VCDI_COPY, 0x6, 0x4 } } } ,
            { { { VCDI_ADD, 0x0, 0x3 }, { VCDI_COPY, 0x6, 0x4 } } } ,
            { { { VCDI_ADD, 0x0, 0x4 }, { VCDI_COPY, 0x6, 0x4 } } } ,
            { { { VCDI_ADD, 0x0, 0x1 }, { VCDI_COPY, 0x7, 0x4 } } } ,
            { { { VCDI_ADD, 0x0, 0x2 }, { VCDI_COPY, 0x7, 0x4 } } } ,
            { { { VCDI_ADD, 0x0, 0x3 }, { VCDI_COPY, 0x7, 0x4 } } } ,
            { { { VCDI_ADD, 0x0, 0x4 }, { VCDI_COPY, 0x7, 0x4 } } } ,
            { { { VCDI_ADD, 0x0, 0x1 }, { VCDI_COPY, 0x8, 0x4 } } } ,
            { { { VCDI_ADD, 0x0, 0x2 }, { VCDI_COPY, 0x8, 0x4 } } } ,
            { { { VCDI_ADD, 0x0, 0x3 }, { VCDI_COPY, 0x8, 0x4 } } } ,
            { { { VCDI_ADD, 0x0, 0x4 }, { VCDI_COPY, 0x8, 0x4 } } } ,
            { { { VCDI_COPY, 0x0, 0x4 }, { VCDI_ADD, 0x0, 0x1 } } } ,
            { { { VCDI_COPY, 0x1, 0x4 }, { VCDI_ADD, 0x0, 0x1 } } } ,
            { { { VCDI_COPY, 0x2, 0x4 }, { VCDI_ADD, 0x0, 0x1 } } } ,
            { { { VCDI_COPY, 0x3, 0x4 }, { VCDI_ADD, 0x0, 0x1 } } } ,
            { { { VCDI_COPY, 0x4, 0x4 }, { VCDI_ADD, 0x0, 0x1 } } } ,
            { { { VCDI_COPY, 0x5, 0x4 }, { VCDI_ADD, 0x0, 0x1 } } } ,
            { { { VCDI_COPY, 0x6, 0x4 }, { VCDI_ADD, 0x0, 0x1 } } } ,
            { { { VCDI_COPY, 0x7, 0x4 }, { VCDI_ADD, 0x0, 0x1 } } } ,
            { { { VCDI_COPY, 0x8, 0x4 }, { VCDI_ADD, 0x0, 0x1 } } }
        };

        enum VCDFlags {
            VCDF_UNK1 = 1,
            VCDF_UNK2 = 2,
            VCDF_MASK12 = VCDF_UNK2 | VCDF_UNK1,
            VCDF_SOURCE_SEGMENT_RELATIVE = 4,
            VCDF_CHECKSUM = 8,
        };

        size_t ReadULEB128(byte** data) {
            size_t output{};
            byte c;

            do {
                c = **data;
                (*data)++;
                output = (output << 7) | (c & 0x7F);
            } while (c & 0x80);

            return output;
        }

        int32_t ComputeChecksum32(void* _buffer, unsigned int len, int start) {
            const char* buffer{ (const char*)_buffer };
            int* v3; // rbx
            int* v4; // r9
            int v5; // r10d
            int v6; // r11d
            int v7; // eax
            int v8; // ecx
            int v9; // r8d
            int v10; // eax
            unsigned int v11; // eax
            int* i; // rdx
            int v13; // ecx
            unsigned __int64 v14; // r8
            unsigned __int64 v15; // r10
            int v16; // ecx
            unsigned int v17; // edx

            v3 = (int*)&buffer[len];
            v4 = (int*)buffer;
            if (len < 0x10) {
                v10 = start + 374761393;
            }
            else {
                v5 = start + 606290984;
                v6 = start - 2048144777;
                v7 = start + 1640531535;
                do
                {
                    v5 = -1640531535 * RotateLeft32(v5 - 2048144777 * *v4, 13);
                    v6 = -1640531535 * RotateLeft32(v6 - 2048144777 * v4[1], 13);
                    v8 = 2048144777 * v4[3];
                    v9 = RotateLeft32(start - 2048144777 * v4[2], 13);
                    v4 += 4;
                    start = -1640531535 * v9;
                    v7 = -1640531535 * RotateLeft32(v7 - v8, 13);
                } while (v4 <= v3 - 4);
                v10 = RotateLeft32(v5, 1) + RotateLeft32(v6, 7) + RotateLeft32(start, 12) + RotateLeft32(v7, 18);
            }
            v11 = len + v10;
            for (i = v4 + 1; i <= v3; v11 = 668265263 * RotateLeft32(v11 - v13, 17)) {
                v13 = 1028477379 * *v4;
                v4 = i++;
            }
            v14 = 0i64;
            v15 = (char*)v3 - (char*)v4;
            if (v4 > v3)
                v15 = 0i64;
            if (v15)
            {
                do
                {
                    v16 = *(unsigned __int8*)v4;
                    v4 = (int*)((char*)v4 + 1);
                    ++v14;
                    v11 = -1640531535 * RotateLeft32(v11 + 374761393 * v16, 11);
                } while (v14 < v15);
            }
            v17 = -1028477379 * ((-2048144777 * (v11 ^ (v11 >> 15))) ^ ((-2048144777 * (v11 ^ (v11 >> 15))) >> 13));
            return v17 ^ HIWORD(v17);
        }

        size_t AddressDecode(VcdState* vcd, size_t here, int mode) {
            size_t address;

            if (mode == VCD_SELF) {
                address = ReadULEB128(&vcd->pAddr);
            }
            else if (mode == VCD_HERE) {
                address = here - ReadULEB128(&vcd->pAddr);
            }
            else if (mode >= 6) {
                int index = (mode - 6) << 8;
                index += *(vcd->pAddr++);
                address = vcd->asame[index];
            }
            else {
                address = vcd->anear[mode - 2] + ReadULEB128(&vcd->pAddr);
            }

            vcd->anear[vcd->next_slot] = address;
            vcd->next_slot = (vcd->next_slot + 1) & 3;
            vcd->asame[address % 0x300] = address;

            return address;
        }
    }

    void memmove2(void* _dest, void* _src, size_t n) {
        byte* dest{ (byte*)_dest };
        byte* src{ (byte*)_src };

        for (size_t i = 0; i < n; i++) {
            dest[i] = src[i];
        }
    }

    bool bdiff(
        BDiffState* state,
        vcSourceCB_t* sourceDataCB,
        vcDiffCB_t* patchDataCB,
        vcDestCB_t* destDataCB)
    {
        if (!state->headerRead) {
            byte* header{ patchDataCB(state->state, 0, 0x405, nullptr) };

            if (!header || header[0] != 0xD6 || header[1] != 0xC3 || header[2] != 0xC4) {
                state->error = "Invalid magic";
                return false;
            }
            if ((header[4] & 0xF3) != 0) {
                state->error = "Invalid header";
                return false;
            }
            patchDataCB(state->state, 5, 0, nullptr);
            state->headerRead = true;
        }

        size_t offset{};
        // fetch enough data for the block info, 0x400 is the alignment?
        byte* bdiffData{ patchDataCB(state->state, 0, 0x400, &offset) };
        if (!bdiffData) {
            state->error = "Missing data";
            return false;
        }
        byte* bdiffDataStart{ bdiffData };
        byte flags{ *(bdiffData++) };
        if ((flags & VCDF_MASK12) == VCDF_MASK12) {
            state->error = "Invalid flag";
            return false;
        }
        size_t sourceSegmentOffset{};
        byte* sourceSegment{};
        if (flags & VCDF_MASK12) {
            sourceSegmentOffset = ReadULEB128(&bdiffData);
            size_t sourceSegmentLocationEx{ ReadULEB128(&bdiffData) };
            size_t sourceSegmentLocation;
            if (flags & VCDF_SOURCE_SEGMENT_RELATIVE) {
                sourceSegmentLocation = ReadULEB128(&bdiffData);
            }
            else {
                sourceSegmentLocation = sourceSegmentLocationEx;
            }

            size_t sourceSegmentStart{ sourceSegmentLocationEx - sourceSegmentLocation };
            sourceSegment = &sourceDataCB(state->state, sourceSegmentLocation, sourceSegmentStart + sourceSegmentOffset)[sourceSegmentStart];
        }

        size_t patchDataLen{ ReadULEB128(&bdiffData) };

        byte* patchData{ patchDataCB(
            state->state,
            offset + bdiffData - bdiffDataStart,
            patchDataLen + (flags & VCDF_CHECKSUM ? 4 : 0),
            &offset
        ) };
        byte* patchDataStart{ patchData };

        size_t patchDestLen{ ReadULEB128(&patchData) };
        byte* patchDest{ destDataCB(state->state, patchDestLen) };
        byte* patchDestStart{ patchDest };
        if (!patchDest) {
            state->error = "No patch data destination";
            return false;
        }

        if (*(patchData++)) {
            state->error = "Invalid patch data";
            return false;
        }

        VcdState vcd{};
        size_t instructionOffset{ ReadULEB128(&patchData) };
        size_t instructionSize{ ReadULEB128(&patchData) };
        size_t instructionCheckOffset{ ReadULEB128(&patchData) };

        byte* instructionData{ &patchData[instructionOffset] };
        byte* instructionDataEnd{ &patchData[instructionOffset + instructionSize] };
        byte* instructionCheck{ &instructionDataEnd[instructionCheckOffset] };

        vcd.pAddr = instructionDataEnd;

        while (instructionData < instructionDataEnd) {
            byte opcode{ *(instructionData++) };
            DoubleInstruction* dinst{ &instructions[opcode] };

            for (size_t i = 0; i < 2; i++) {
                Instruction inst{ dinst->instruction[i] };
                assert(inst.op < VCDI_COUNT && "invalid op");

                size_t ilen{ inst.size };
                if (!ilen && inst.op != VCDI_NOOP) ilen = ReadULEB128(&instructionData);

                //LOG_TRACE(
                //    "r:0x{:03x} 0x{:02x} inst[{}] = "
                //    "(VCDI_{}, m:{}, s:0x{:x}/0x{:x})"
                //    " 0x{:x}/0x{:x}"
                //    " 0x{:x}",
                //    instructionDataEnd - instructionData, (int)opcode, i, 
                //    vcdInstructionCodeNames[inst.op], inst.mode, inst.size, ilen,
                //    patchDest - patchDestStart, patchDestLen,
                //    patchData - patchDataStart
                //);

                switch (inst.op) {
                case VCDI_NOOP: // 0 no op
                    break;
                case VCDI_ADD: { // 1 add to block
                    std::memmove(patchDest, patchData, ilen);
                    patchData += ilen;
                    patchDest += ilen;
                    break;
                }
                case VCDI_RUN: { // 2 empty a block
                    byte runchar{ *(patchData++) };
                    std::memset(patchDest, runchar, ilen);
                    patchDest += ilen;
                    break;
                }
                case VCDI_COPY: { // 3 copy existing block
                    size_t address{ AddressDecode(&vcd, sourceSegmentOffset + patchDest - patchDestStart, inst.mode) };

                    byte* readLoc;
                    if (address < sourceSegmentOffset) {
                        if (!sourceSegment) {
                            state->error = "Missing source segment";
                            return false;
                        }
                        readLoc = &sourceSegment[address];
                        //LOG_TRACE("VCDI_COPY(0x{:x}, b:0x{:x}, 0x{:x})", patchDest - patchDestStart, address, ilen);
                    }
                    else {
                        readLoc = &patchDestStart[address - sourceSegmentOffset];
                        //LOG_TRACE("VCDI_COPY(0x{:x}, d:0x{:x}, 0x{:x})", patchDest - patchDestStart, address - sourceSegmentOffset, ilen);
                    }

                    memmove2(patchDest, readLoc, ilen);
                    patchDest += ilen;
                    break;
                }
                }
            }

        }

        if (flags & VCDF_CHECKSUM) {
            // check checksum
            int32_t realChecksum{ *(int32_t*)instructionCheck };
            instructionCheck += 4;

            switch (state->type) {
            case BDT_TREYARCH: {
                int32_t calculatedChecksum{ ComputeChecksum32(patchDestStart, (uint32_t)patchDestLen, 0) };
                if (calculatedChecksum != realChecksum) {
                    state->error = utils::va("Data is corrupt. %x != %x", realChecksum, calculatedChecksum);
                    return false;
                }
                break;
            }
            case BDT_IW:
                // no implemented
                break;
            default:
                break;
            }
        }

        patchDataCB(state->state, instructionCheck - patchDataStart + offset, 0, nullptr);
        return true;
    }

}