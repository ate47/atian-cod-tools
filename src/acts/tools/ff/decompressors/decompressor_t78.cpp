#include <includes.hpp>
#include <deps/oodle.hpp>
#include <tools/fastfile.hpp>
#include <tools/ff/fastfile_handlers.hpp>
#include <games/bo4/pool.hpp>
#include <tools/compatibility/acti_crypto_keys.hpp>
#include <bcrypt.h>

namespace {
	struct Instruction {
		int16_t op;
		int16_t mode;
		uint32_t size;
	};

	struct DoubleInstruction {
		Instruction instruction[2];
	};

	DoubleInstruction instructions[]{
		{ Instruction{ 0x2, 0x0, 0x0 }, Instruction{0x0, 0x0, 0x0 } }, // 0x0
		{ Instruction{ 0x1, 0x0, 0x0 }, Instruction{0x0, 0x0, 0x0 } }, // 0x1
		{ Instruction{ 0x1, 0x0, 0x1 }, Instruction{0x0, 0x0, 0x0 } }, // 0x2
		{ Instruction{ 0x1, 0x0, 0x2 }, Instruction{0x0, 0x0, 0x0 } }, // 0x3
		{ Instruction{ 0x1, 0x0, 0x3 }, Instruction{0x0, 0x0, 0x0 } }, // 0x4
		{ Instruction{ 0x1, 0x0, 0x4 }, Instruction{0x0, 0x0, 0x0 } }, // 0x5
		{ Instruction{ 0x1, 0x0, 0x5 }, Instruction{0x0, 0x0, 0x0 } }, // 0x6
		{ Instruction{ 0x1, 0x0, 0x6 }, Instruction{0x0, 0x0, 0x0 } }, // 0x7
		{ Instruction{ 0x1, 0x0, 0x7 }, Instruction{0x0, 0x0, 0x0 } }, // 0x8
		{ Instruction{ 0x1, 0x0, 0x8 }, Instruction{0x0, 0x0, 0x0 } }, // 0x9
		{ Instruction{ 0x1, 0x0, 0x9 }, Instruction{0x0, 0x0, 0x0 } }, // 0xa
		{ Instruction{ 0x1, 0x0, 0xa }, Instruction{0x0, 0x0, 0x0 } }, // 0xb
		{ Instruction{ 0x1, 0x0, 0xb }, Instruction{0x0, 0x0, 0x0 } }, // 0xc
		{ Instruction{ 0x1, 0x0, 0xc }, Instruction{0x0, 0x0, 0x0 } }, // 0xd
		{ Instruction{ 0x1, 0x0, 0xd }, Instruction{0x0, 0x0, 0x0 } }, // 0xe
		{ Instruction{ 0x1, 0x0, 0xe }, Instruction{0x0, 0x0, 0x0 } }, // 0xf
		{ Instruction{ 0x1, 0x0, 0xf }, Instruction{0x0, 0x0, 0x0 } }, // 0x10
		{ Instruction{ 0x1, 0x0, 0x10 }, Instruction{0x0, 0x0, 0x0 } }, // 0x11
		{ Instruction{ 0x1, 0x0, 0x11 }, Instruction{0x0, 0x0, 0x0 } }, // 0x12
		{ Instruction{ 0x3, 0x0, 0x0 }, Instruction{0x0, 0x0, 0x0 } }, // 0x13
		{ Instruction{ 0x3, 0x0, 0x4 }, Instruction{0x0, 0x0, 0x0 } }, // 0x14
		{ Instruction{ 0x3, 0x0, 0x5 }, Instruction{0x0, 0x0, 0x0 } }, // 0x15
		{ Instruction{ 0x3, 0x0, 0x6 }, Instruction{0x0, 0x0, 0x0 } }, // 0x16
		{ Instruction{ 0x3, 0x0, 0x7 }, Instruction{0x0, 0x0, 0x0 } }, // 0x17
		{ Instruction{ 0x3, 0x0, 0x8 }, Instruction{0x0, 0x0, 0x0 } }, // 0x18
		{ Instruction{ 0x3, 0x0, 0x9 }, Instruction{0x0, 0x0, 0x0 } }, // 0x19
		{ Instruction{ 0x3, 0x0, 0xa }, Instruction{0x0, 0x0, 0x0 } }, // 0x1a
		{ Instruction{ 0x3, 0x0, 0xb }, Instruction{0x0, 0x0, 0x0 } }, // 0x1b
		{ Instruction{ 0x3, 0x0, 0xc }, Instruction{0x0, 0x0, 0x0 } }, // 0x1c
		{ Instruction{ 0x3, 0x0, 0xd }, Instruction{0x0, 0x0, 0x0 } }, // 0x1d
		{ Instruction{ 0x3, 0x0, 0xe }, Instruction{0x0, 0x0, 0x0 } }, // 0x1e
		{ Instruction{ 0x3, 0x0, 0xf }, Instruction{0x0, 0x0, 0x0 } }, // 0x1f
		{ Instruction{ 0x3, 0x0, 0x10 }, Instruction{0x0, 0x0, 0x0 } }, // 0x20
		{ Instruction{ 0x3, 0x0, 0x11 }, Instruction{0x0, 0x0, 0x0 } }, // 0x21
		{ Instruction{ 0x3, 0x0, 0x12 }, Instruction{0x0, 0x0, 0x0 } }, // 0x22
		{ Instruction{ 0x3, 0x1, 0x0 }, Instruction{0x0, 0x0, 0x0 } }, // 0x23
		{ Instruction{ 0x3, 0x1, 0x4 }, Instruction{0x0, 0x0, 0x0 } }, // 0x24
		{ Instruction{ 0x3, 0x1, 0x5 }, Instruction{0x0, 0x0, 0x0 } }, // 0x25
		{ Instruction{ 0x3, 0x1, 0x6 }, Instruction{0x0, 0x0, 0x0 } }, // 0x26
		{ Instruction{ 0x3, 0x1, 0x7 }, Instruction{0x0, 0x0, 0x0 } }, // 0x27
		{ Instruction{ 0x3, 0x1, 0x8 }, Instruction{0x0, 0x0, 0x0 } }, // 0x28
		{ Instruction{ 0x3, 0x1, 0x9 }, Instruction{0x0, 0x0, 0x0 } }, // 0x29
		{ Instruction{ 0x3, 0x1, 0xa }, Instruction{0x0, 0x0, 0x0 } }, // 0x2a
		{ Instruction{ 0x3, 0x1, 0xb }, Instruction{0x0, 0x0, 0x0 } }, // 0x2b
		{ Instruction{ 0x3, 0x1, 0xc }, Instruction{0x0, 0x0, 0x0 } }, // 0x2c
		{ Instruction{ 0x3, 0x1, 0xd }, Instruction{0x0, 0x0, 0x0 } }, // 0x2d
		{ Instruction{ 0x3, 0x1, 0xe }, Instruction{0x0, 0x0, 0x0 } }, // 0x2e
		{ Instruction{ 0x3, 0x1, 0xf }, Instruction{0x0, 0x0, 0x0 } }, // 0x2f
		{ Instruction{ 0x3, 0x1, 0x10 }, Instruction{0x0, 0x0, 0x0 } }, // 0x30
		{ Instruction{ 0x3, 0x1, 0x11 }, Instruction{0x0, 0x0, 0x0 } }, // 0x31
		{ Instruction{ 0x3, 0x1, 0x12 }, Instruction{0x0, 0x0, 0x0 } }, // 0x32
		{ Instruction{ 0x3, 0x2, 0x0 }, Instruction{0x0, 0x0, 0x0 } }, // 0x33
		{ Instruction{ 0x3, 0x2, 0x4 }, Instruction{0x0, 0x0, 0x0 } }, // 0x34
		{ Instruction{ 0x3, 0x2, 0x5 }, Instruction{0x0, 0x0, 0x0 } }, // 0x35
		{ Instruction{ 0x3, 0x2, 0x6 }, Instruction{0x0, 0x0, 0x0 } }, // 0x36
		{ Instruction{ 0x3, 0x2, 0x7 }, Instruction{0x0, 0x0, 0x0 } }, // 0x37
		{ Instruction{ 0x3, 0x2, 0x8 }, Instruction{0x0, 0x0, 0x0 } }, // 0x38
		{ Instruction{ 0x3, 0x2, 0x9 }, Instruction{0x0, 0x0, 0x0 } }, // 0x39
		{ Instruction{ 0x3, 0x2, 0xa }, Instruction{0x0, 0x0, 0x0 } }, // 0x3a
		{ Instruction{ 0x3, 0x2, 0xb }, Instruction{0x0, 0x0, 0x0 } }, // 0x3b
		{ Instruction{ 0x3, 0x2, 0xc }, Instruction{0x0, 0x0, 0x0 } }, // 0x3c
		{ Instruction{ 0x3, 0x2, 0xd }, Instruction{0x0, 0x0, 0x0 } }, // 0x3d
		{ Instruction{ 0x3, 0x2, 0xe }, Instruction{0x0, 0x0, 0x0 } }, // 0x3e
		{ Instruction{ 0x3, 0x2, 0xf }, Instruction{0x0, 0x0, 0x0 } }, // 0x3f
		{ Instruction{ 0x3, 0x2, 0x10 }, Instruction{0x0, 0x0, 0x0 } }, // 0x40
		{ Instruction{ 0x3, 0x2, 0x11 }, Instruction{0x0, 0x0, 0x0 } }, // 0x41
		{ Instruction{ 0x3, 0x2, 0x12 }, Instruction{0x0, 0x0, 0x0 } }, // 0x42
		{ Instruction{ 0x3, 0x3, 0x0 }, Instruction{0x0, 0x0, 0x0 } }, // 0x43
		{ Instruction{ 0x3, 0x3, 0x4 }, Instruction{0x0, 0x0, 0x0 } }, // 0x44
		{ Instruction{ 0x3, 0x3, 0x5 }, Instruction{0x0, 0x0, 0x0 } }, // 0x45
		{ Instruction{ 0x3, 0x3, 0x6 }, Instruction{0x0, 0x0, 0x0 } }, // 0x46
		{ Instruction{ 0x3, 0x3, 0x7 }, Instruction{0x0, 0x0, 0x0 } }, // 0x47
		{ Instruction{ 0x3, 0x3, 0x8 }, Instruction{0x0, 0x0, 0x0 } }, // 0x48
		{ Instruction{ 0x3, 0x3, 0x9 }, Instruction{0x0, 0x0, 0x0 } }, // 0x49
		{ Instruction{ 0x3, 0x3, 0xa }, Instruction{0x0, 0x0, 0x0 } }, // 0x4a
		{ Instruction{ 0x3, 0x3, 0xb }, Instruction{0x0, 0x0, 0x0 } }, // 0x4b
		{ Instruction{ 0x3, 0x3, 0xc }, Instruction{0x0, 0x0, 0x0 } }, // 0x4c
		{ Instruction{ 0x3, 0x3, 0xd }, Instruction{0x0, 0x0, 0x0 } }, // 0x4d
		{ Instruction{ 0x3, 0x3, 0xe }, Instruction{0x0, 0x0, 0x0 } }, // 0x4e
		{ Instruction{ 0x3, 0x3, 0xf }, Instruction{0x0, 0x0, 0x0 } }, // 0x4f
		{ Instruction{ 0x3, 0x3, 0x10 }, Instruction{0x0, 0x0, 0x0 } }, // 0x50
		{ Instruction{ 0x3, 0x3, 0x11 }, Instruction{0x0, 0x0, 0x0 } }, // 0x51
		{ Instruction{ 0x3, 0x3, 0x12 }, Instruction{0x0, 0x0, 0x0 } }, // 0x52
		{ Instruction{ 0x3, 0x4, 0x0 }, Instruction{0x0, 0x0, 0x0 } }, // 0x53
		{ Instruction{ 0x3, 0x4, 0x4 }, Instruction{0x0, 0x0, 0x0 } }, // 0x54
		{ Instruction{ 0x3, 0x4, 0x5 }, Instruction{0x0, 0x0, 0x0 } }, // 0x55
		{ Instruction{ 0x3, 0x4, 0x6 }, Instruction{0x0, 0x0, 0x0 } }, // 0x56
		{ Instruction{ 0x3, 0x4, 0x7 }, Instruction{0x0, 0x0, 0x0 } }, // 0x57
		{ Instruction{ 0x3, 0x4, 0x8 }, Instruction{0x0, 0x0, 0x0 } }, // 0x58
		{ Instruction{ 0x3, 0x4, 0x9 }, Instruction{0x0, 0x0, 0x0 } }, // 0x59
		{ Instruction{ 0x3, 0x4, 0xa }, Instruction{0x0, 0x0, 0x0 } }, // 0x5a
		{ Instruction{ 0x3, 0x4, 0xb }, Instruction{0x0, 0x0, 0x0 } }, // 0x5b
		{ Instruction{ 0x3, 0x4, 0xc }, Instruction{0x0, 0x0, 0x0 } }, // 0x5c
		{ Instruction{ 0x3, 0x4, 0xd }, Instruction{0x0, 0x0, 0x0 } }, // 0x5d
		{ Instruction{ 0x3, 0x4, 0xe }, Instruction{0x0, 0x0, 0x0 } }, // 0x5e
		{ Instruction{ 0x3, 0x4, 0xf }, Instruction{0x0, 0x0, 0x0 } }, // 0x5f
		{ Instruction{ 0x3, 0x4, 0x10 }, Instruction{0x0, 0x0, 0x0 } }, // 0x60
		{ Instruction{ 0x3, 0x4, 0x11 }, Instruction{0x0, 0x0, 0x0 } }, // 0x61
		{ Instruction{ 0x3, 0x4, 0x12 }, Instruction{0x0, 0x0, 0x0 } }, // 0x62
		{ Instruction{ 0x3, 0x5, 0x0 }, Instruction{0x0, 0x0, 0x0 } }, // 0x63
		{ Instruction{ 0x3, 0x5, 0x4 }, Instruction{0x0, 0x0, 0x0 } }, // 0x64
		{ Instruction{ 0x3, 0x5, 0x5 }, Instruction{0x0, 0x0, 0x0 } }, // 0x65
		{ Instruction{ 0x3, 0x5, 0x6 }, Instruction{0x0, 0x0, 0x0 } }, // 0x66
		{ Instruction{ 0x3, 0x5, 0x7 }, Instruction{0x0, 0x0, 0x0 } }, // 0x67
		{ Instruction{ 0x3, 0x5, 0x8 }, Instruction{0x0, 0x0, 0x0 } }, // 0x68
		{ Instruction{ 0x3, 0x5, 0x9 }, Instruction{0x0, 0x0, 0x0 } }, // 0x69
		{ Instruction{ 0x3, 0x5, 0xa }, Instruction{0x0, 0x0, 0x0 } }, // 0x6a
		{ Instruction{ 0x3, 0x5, 0xb }, Instruction{0x0, 0x0, 0x0 } }, // 0x6b
		{ Instruction{ 0x3, 0x5, 0xc }, Instruction{0x0, 0x0, 0x0 } }, // 0x6c
		{ Instruction{ 0x3, 0x5, 0xd }, Instruction{0x0, 0x0, 0x0 } }, // 0x6d
		{ Instruction{ 0x3, 0x5, 0xe }, Instruction{0x0, 0x0, 0x0 } }, // 0x6e
		{ Instruction{ 0x3, 0x5, 0xf }, Instruction{0x0, 0x0, 0x0 } }, // 0x6f
		{ Instruction{ 0x3, 0x5, 0x10 }, Instruction{0x0, 0x0, 0x0 } }, // 0x70
		{ Instruction{ 0x3, 0x5, 0x11 }, Instruction{0x0, 0x0, 0x0 } }, // 0x71
		{ Instruction{ 0x3, 0x5, 0x12 }, Instruction{0x0, 0x0, 0x0 } }, // 0x72
		{ Instruction{ 0x3, 0x6, 0x0 }, Instruction{0x0, 0x0, 0x0 } }, // 0x73
		{ Instruction{ 0x3, 0x6, 0x4 }, Instruction{0x0, 0x0, 0x0 } }, // 0x74
		{ Instruction{ 0x3, 0x6, 0x5 }, Instruction{0x0, 0x0, 0x0 } }, // 0x75
		{ Instruction{ 0x3, 0x6, 0x6 }, Instruction{0x0, 0x0, 0x0 } }, // 0x76
		{ Instruction{ 0x3, 0x6, 0x7 }, Instruction{0x0, 0x0, 0x0 } }, // 0x77
		{ Instruction{ 0x3, 0x6, 0x8 }, Instruction{0x0, 0x0, 0x0 } }, // 0x78
		{ Instruction{ 0x3, 0x6, 0x9 }, Instruction{0x0, 0x0, 0x0 } }, // 0x79
		{ Instruction{ 0x3, 0x6, 0xa }, Instruction{0x0, 0x0, 0x0 } }, // 0x7a
		{ Instruction{ 0x3, 0x6, 0xb }, Instruction{0x0, 0x0, 0x0 } }, // 0x7b
		{ Instruction{ 0x3, 0x6, 0xc }, Instruction{0x0, 0x0, 0x0 } }, // 0x7c
		{ Instruction{ 0x3, 0x6, 0xd }, Instruction{0x0, 0x0, 0x0 } }, // 0x7d
		{ Instruction{ 0x3, 0x6, 0xe }, Instruction{0x0, 0x0, 0x0 } }, // 0x7e
		{ Instruction{ 0x3, 0x6, 0xf }, Instruction{0x0, 0x0, 0x0 } }, // 0x7f
		{ Instruction{ 0x3, 0x6, 0x10 }, Instruction{0x0, 0x0, 0x0 } }, // 0x80
		{ Instruction{ 0x3, 0x6, 0x11 }, Instruction{0x0, 0x0, 0x0 } }, // 0x81
		{ Instruction{ 0x3, 0x6, 0x12 }, Instruction{0x0, 0x0, 0x0 } }, // 0x82
		{ Instruction{ 0x3, 0x7, 0x0 }, Instruction{0x0, 0x0, 0x0 } }, // 0x83
		{ Instruction{ 0x3, 0x7, 0x4 }, Instruction{0x0, 0x0, 0x0 } }, // 0x84
		{ Instruction{ 0x3, 0x7, 0x5 }, Instruction{0x0, 0x0, 0x0 } }, // 0x85
		{ Instruction{ 0x3, 0x7, 0x6 }, Instruction{0x0, 0x0, 0x0 } }, // 0x86
		{ Instruction{ 0x3, 0x7, 0x7 }, Instruction{0x0, 0x0, 0x0 } }, // 0x87
		{ Instruction{ 0x3, 0x7, 0x8 }, Instruction{0x0, 0x0, 0x0 } }, // 0x88
		{ Instruction{ 0x3, 0x7, 0x9 }, Instruction{0x0, 0x0, 0x0 } }, // 0x89
		{ Instruction{ 0x3, 0x7, 0xa }, Instruction{0x0, 0x0, 0x0 } }, // 0x8a
		{ Instruction{ 0x3, 0x7, 0xb }, Instruction{0x0, 0x0, 0x0 } }, // 0x8b
		{ Instruction{ 0x3, 0x7, 0xc }, Instruction{0x0, 0x0, 0x0 } }, // 0x8c
		{ Instruction{ 0x3, 0x7, 0xd }, Instruction{0x0, 0x0, 0x0 } }, // 0x8d
		{ Instruction{ 0x3, 0x7, 0xe }, Instruction{0x0, 0x0, 0x0 } }, // 0x8e
		{ Instruction{ 0x3, 0x7, 0xf }, Instruction{0x0, 0x0, 0x0 } }, // 0x8f
		{ Instruction{ 0x3, 0x7, 0x10 }, Instruction{0x0, 0x0, 0x0 } }, // 0x90
		{ Instruction{ 0x3, 0x7, 0x11 }, Instruction{0x0, 0x0, 0x0 } }, // 0x91
		{ Instruction{ 0x3, 0x7, 0x12 }, Instruction{0x0, 0x0, 0x0 } }, // 0x92
		{ Instruction{ 0x3, 0x8, 0x0 }, Instruction{0x0, 0x0, 0x0 } }, // 0x93
		{ Instruction{ 0x3, 0x8, 0x4 }, Instruction{0x0, 0x0, 0x0 } }, // 0x94
		{ Instruction{ 0x3, 0x8, 0x5 }, Instruction{0x0, 0x0, 0x0 } }, // 0x95
		{ Instruction{ 0x3, 0x8, 0x6 }, Instruction{0x0, 0x0, 0x0 } }, // 0x96
		{ Instruction{ 0x3, 0x8, 0x7 }, Instruction{0x0, 0x0, 0x0 } }, // 0x97
		{ Instruction{ 0x3, 0x8, 0x8 }, Instruction{0x0, 0x0, 0x0 } }, // 0x98
		{ Instruction{ 0x3, 0x8, 0x9 }, Instruction{0x0, 0x0, 0x0 } }, // 0x99
		{ Instruction{ 0x3, 0x8, 0xa }, Instruction{0x0, 0x0, 0x0 } }, // 0x9a
		{ Instruction{ 0x3, 0x8, 0xb }, Instruction{0x0, 0x0, 0x0 } }, // 0x9b
		{ Instruction{ 0x3, 0x8, 0xc }, Instruction{0x0, 0x0, 0x0 } }, // 0x9c
		{ Instruction{ 0x3, 0x8, 0xd }, Instruction{0x0, 0x0, 0x0 } }, // 0x9d
		{ Instruction{ 0x3, 0x8, 0xe }, Instruction{0x0, 0x0, 0x0 } }, // 0x9e
		{ Instruction{ 0x3, 0x8, 0xf }, Instruction{0x0, 0x0, 0x0 } }, // 0x9f
		{ Instruction{ 0x3, 0x8, 0x10 }, Instruction{0x0, 0x0, 0x0 } }, // 0xa0
		{ Instruction{ 0x3, 0x8, 0x11 }, Instruction{0x0, 0x0, 0x0 } }, // 0xa1
		{ Instruction{ 0x3, 0x8, 0x12 }, Instruction{0x0, 0x0, 0x0 } }, // 0xa2
		{ Instruction{ 0x1, 0x0, 0x1 }, Instruction{0x3, 0x0, 0x4 } }, // 0xa3
		{ Instruction{ 0x1, 0x0, 0x1 }, Instruction{0x3, 0x0, 0x5 } }, // 0xa4
		{ Instruction{ 0x1, 0x0, 0x1 }, Instruction{0x3, 0x0, 0x6 } }, // 0xa5
		{ Instruction{ 0x1, 0x0, 0x2 }, Instruction{0x3, 0x0, 0x4 } }, // 0xa6
		{ Instruction{ 0x1, 0x0, 0x2 }, Instruction{0x3, 0x0, 0x5 } }, // 0xa7
		{ Instruction{ 0x1, 0x0, 0x2 }, Instruction{0x3, 0x0, 0x6 } }, // 0xa8
		{ Instruction{ 0x1, 0x0, 0x3 }, Instruction{0x3, 0x0, 0x4 } }, // 0xa9
		{ Instruction{ 0x1, 0x0, 0x3 }, Instruction{0x3, 0x0, 0x5 } }, // 0xaa
		{ Instruction{ 0x1, 0x0, 0x3 }, Instruction{0x3, 0x0, 0x6 } }, // 0xab
		{ Instruction{ 0x1, 0x0, 0x4 }, Instruction{0x3, 0x0, 0x4 } }, // 0xac
		{ Instruction{ 0x1, 0x0, 0x4 }, Instruction{0x3, 0x0, 0x5 } }, // 0xad
		{ Instruction{ 0x1, 0x0, 0x4 }, Instruction{0x3, 0x0, 0x6 } }, // 0xae
		{ Instruction{ 0x1, 0x0, 0x1 }, Instruction{0x3, 0x1, 0x4 } }, // 0xaf
		{ Instruction{ 0x1, 0x0, 0x1 }, Instruction{0x3, 0x1, 0x5 } }, // 0xb0
		{ Instruction{ 0x1, 0x0, 0x1 }, Instruction{0x3, 0x1, 0x6 } }, // 0xb1
		{ Instruction{ 0x1, 0x0, 0x2 }, Instruction{0x3, 0x1, 0x4 } }, // 0xb2
		{ Instruction{ 0x1, 0x0, 0x2 }, Instruction{0x3, 0x1, 0x5 } }, // 0xb3
		{ Instruction{ 0x1, 0x0, 0x2 }, Instruction{0x3, 0x1, 0x6 } }, // 0xb4
		{ Instruction{ 0x1, 0x0, 0x3 }, Instruction{0x3, 0x1, 0x4 } }, // 0xb5
		{ Instruction{ 0x1, 0x0, 0x3 }, Instruction{0x3, 0x1, 0x5 } }, // 0xb6
		{ Instruction{ 0x1, 0x0, 0x3 }, Instruction{0x3, 0x1, 0x6 } }, // 0xb7
		{ Instruction{ 0x1, 0x0, 0x4 }, Instruction{0x3, 0x1, 0x4 } }, // 0xb8
		{ Instruction{ 0x1, 0x0, 0x4 }, Instruction{0x3, 0x1, 0x5 } }, // 0xb9
		{ Instruction{ 0x1, 0x0, 0x4 }, Instruction{0x3, 0x1, 0x6 } }, // 0xba
		{ Instruction{ 0x1, 0x0, 0x1 }, Instruction{0x3, 0x2, 0x4 } }, // 0xbb
		{ Instruction{ 0x1, 0x0, 0x1 }, Instruction{0x3, 0x2, 0x5 } }, // 0xbc
		{ Instruction{ 0x1, 0x0, 0x1 }, Instruction{0x3, 0x2, 0x6 } }, // 0xbd
		{ Instruction{ 0x1, 0x0, 0x2 }, Instruction{0x3, 0x2, 0x4 } }, // 0xbe
		{ Instruction{ 0x1, 0x0, 0x2 }, Instruction{0x3, 0x2, 0x5 } }, // 0xbf
		{ Instruction{ 0x1, 0x0, 0x2 }, Instruction{0x3, 0x2, 0x6 } }, // 0xc0
		{ Instruction{ 0x1, 0x0, 0x3 }, Instruction{0x3, 0x2, 0x4 } }, // 0xc1
		{ Instruction{ 0x1, 0x0, 0x3 }, Instruction{0x3, 0x2, 0x5 } }, // 0xc2
		{ Instruction{ 0x1, 0x0, 0x3 }, Instruction{0x3, 0x2, 0x6 } }, // 0xc3
		{ Instruction{ 0x1, 0x0, 0x4 }, Instruction{0x3, 0x2, 0x4 } }, // 0xc4
		{ Instruction{ 0x1, 0x0, 0x4 }, Instruction{0x3, 0x2, 0x5 } }, // 0xc5
		{ Instruction{ 0x1, 0x0, 0x4 }, Instruction{0x3, 0x2, 0x6 } }, // 0xc6
		{ Instruction{ 0x1, 0x0, 0x1 }, Instruction{0x3, 0x3, 0x4 } }, // 0xc7
		{ Instruction{ 0x1, 0x0, 0x1 }, Instruction{0x3, 0x3, 0x5 } }, // 0xc8
		{ Instruction{ 0x1, 0x0, 0x1 }, Instruction{0x3, 0x3, 0x6 } }, // 0xc9
		{ Instruction{ 0x1, 0x0, 0x2 }, Instruction{0x3, 0x3, 0x4 } }, // 0xca
		{ Instruction{ 0x1, 0x0, 0x2 }, Instruction{0x3, 0x3, 0x5 } }, // 0xcb
		{ Instruction{ 0x1, 0x0, 0x2 }, Instruction{0x3, 0x3, 0x6 } }, // 0xcc
		{ Instruction{ 0x1, 0x0, 0x3 }, Instruction{0x3, 0x3, 0x4 } }, // 0xcd
		{ Instruction{ 0x1, 0x0, 0x3 }, Instruction{0x3, 0x3, 0x5 } }, // 0xce
		{ Instruction{ 0x1, 0x0, 0x3 }, Instruction{0x3, 0x3, 0x6 } }, // 0xcf
		{ Instruction{ 0x1, 0x0, 0x4 }, Instruction{0x3, 0x3, 0x4 } }, // 0xd0
		{ Instruction{ 0x1, 0x0, 0x4 }, Instruction{0x3, 0x3, 0x5 } }, // 0xd1
		{ Instruction{ 0x1, 0x0, 0x4 }, Instruction{0x3, 0x3, 0x6 } }, // 0xd2
		{ Instruction{ 0x1, 0x0, 0x1 }, Instruction{0x3, 0x4, 0x4 } }, // 0xd3
		{ Instruction{ 0x1, 0x0, 0x1 }, Instruction{0x3, 0x4, 0x5 } }, // 0xd4
		{ Instruction{ 0x1, 0x0, 0x1 }, Instruction{0x3, 0x4, 0x6 } }, // 0xd5
		{ Instruction{ 0x1, 0x0, 0x2 }, Instruction{0x3, 0x4, 0x4 } }, // 0xd6
		{ Instruction{ 0x1, 0x0, 0x2 }, Instruction{0x3, 0x4, 0x5 } }, // 0xd7
		{ Instruction{ 0x1, 0x0, 0x2 }, Instruction{0x3, 0x4, 0x6 } }, // 0xd8
		{ Instruction{ 0x1, 0x0, 0x3 }, Instruction{0x3, 0x4, 0x4 } }, // 0xd9
		{ Instruction{ 0x1, 0x0, 0x3 }, Instruction{0x3, 0x4, 0x5 } }, // 0xda
		{ Instruction{ 0x1, 0x0, 0x3 }, Instruction{0x3, 0x4, 0x6 } }, // 0xdb
		{ Instruction{ 0x1, 0x0, 0x4 }, Instruction{0x3, 0x4, 0x4 } }, // 0xdc
		{ Instruction{ 0x1, 0x0, 0x4 }, Instruction{0x3, 0x4, 0x5 } }, // 0xdd
		{ Instruction{ 0x1, 0x0, 0x4 }, Instruction{0x3, 0x4, 0x6 } }, // 0xde
		{ Instruction{ 0x1, 0x0, 0x1 }, Instruction{0x3, 0x5, 0x4 } }, // 0xdf
		{ Instruction{ 0x1, 0x0, 0x1 }, Instruction{0x3, 0x5, 0x5 } }, // 0xe0
		{ Instruction{ 0x1, 0x0, 0x1 }, Instruction{0x3, 0x5, 0x6 } }, // 0xe1
		{ Instruction{ 0x1, 0x0, 0x2 }, Instruction{0x3, 0x5, 0x4 } }, // 0xe2
		{ Instruction{ 0x1, 0x0, 0x2 }, Instruction{0x3, 0x5, 0x5 } }, // 0xe3
		{ Instruction{ 0x1, 0x0, 0x2 }, Instruction{0x3, 0x5, 0x6 } }, // 0xe4
		{ Instruction{ 0x1, 0x0, 0x3 }, Instruction{0x3, 0x5, 0x4 } }, // 0xe5
		{ Instruction{ 0x1, 0x0, 0x3 }, Instruction{0x3, 0x5, 0x5 } }, // 0xe6
		{ Instruction{ 0x1, 0x0, 0x3 }, Instruction{0x3, 0x5, 0x6 } }, // 0xe7
		{ Instruction{ 0x1, 0x0, 0x4 }, Instruction{0x3, 0x5, 0x4 } }, // 0xe8
		{ Instruction{ 0x1, 0x0, 0x4 }, Instruction{0x3, 0x5, 0x5 } }, // 0xe9
		{ Instruction{ 0x1, 0x0, 0x4 }, Instruction{0x3, 0x5, 0x6 } }, // 0xea
		{ Instruction{ 0x1, 0x0, 0x1 }, Instruction{0x3, 0x6, 0x4 } }, // 0xeb
		{ Instruction{ 0x1, 0x0, 0x2 }, Instruction{0x3, 0x6, 0x4 } }, // 0xec
		{ Instruction{ 0x1, 0x0, 0x3 }, Instruction{0x3, 0x6, 0x4 } }, // 0xed
		{ Instruction{ 0x1, 0x0, 0x4 }, Instruction{0x3, 0x6, 0x4 } }, // 0xee
		{ Instruction{ 0x1, 0x0, 0x1 }, Instruction{0x3, 0x7, 0x4 } }, // 0xef
		{ Instruction{ 0x1, 0x0, 0x2 }, Instruction{0x3, 0x7, 0x4 } }, // 0xf0
		{ Instruction{ 0x1, 0x0, 0x3 }, Instruction{0x3, 0x7, 0x4 } }, // 0xf1
		{ Instruction{ 0x1, 0x0, 0x4 }, Instruction{0x3, 0x7, 0x4 } }, // 0xf2
		{ Instruction{ 0x1, 0x0, 0x1 }, Instruction{0x3, 0x8, 0x4 } }, // 0xf3
		{ Instruction{ 0x1, 0x0, 0x2 }, Instruction{0x3, 0x8, 0x4 } }, // 0xf4
		{ Instruction{ 0x1, 0x0, 0x3 }, Instruction{0x3, 0x8, 0x4 } }, // 0xf5
		{ Instruction{ 0x1, 0x0, 0x4 }, Instruction{0x3, 0x8, 0x4 } }, // 0xf6
		{ Instruction{ 0x3, 0x0, 0x4 }, Instruction{0x1, 0x0, 0x1 } }, // 0xf7
		{ Instruction{ 0x3, 0x1, 0x4 }, Instruction{0x1, 0x0, 0x1 } }, // 0xf8
		{ Instruction{ 0x3, 0x2, 0x4 }, Instruction{0x1, 0x0, 0x1 } }, // 0xf9
		{ Instruction{ 0x3, 0x3, 0x4 }, Instruction{0x1, 0x0, 0x1 } }, // 0xfa
		{ Instruction{ 0x3, 0x4, 0x4 }, Instruction{0x1, 0x0, 0x1 } }, // 0xfb
		{ Instruction{ 0x3, 0x5, 0x4 }, Instruction{0x1, 0x0, 0x1 } }, // 0xfc
		{ Instruction{ 0x3, 0x6, 0x4 }, Instruction{0x1, 0x0, 0x1 } }, // 0xfd
		{ Instruction{ 0x3, 0x7, 0x4 }, Instruction{0x1, 0x0, 0x1 } }, // 0xfe
		{ Instruction{ 0x3, 0x8, 0x4 }, Instruction{0x1, 0x0, 0x1 } }, // 0xff
	};

	int64_t __fastcall bd_vbyte(byte** buffer) {
		int64_t result; // rax
		char v2; // r9

		result = 0i64;
		do
		{
			v2 = *(*buffer)++;
			result = (result << 7) | v2 & 0x7F;
		} while (v2 < 0);
		return result;
	}

	unsigned __int64 unk_bdiff(int* a1, __int64 a2, unsigned __int16 a3) {
		int* v3; // r10
		unsigned __int64 v6; // r9
		char v7; // dl
		__int64 v8; // r8
		char v9; // dl
		int v10; // ecx
		unsigned __int64 result; // rax

		v3 = a1 + 1546;
		if (a3)
		{
			if (a3 == 1)
			{
				v8 = 0i64;
				do
				{
					v9 = *(byte*)(*(int64_t*)v3)++;
					v8 = (v8 << 7) | v9 & 0x7F;
				} while (v9 < 0);
				v6 = a2 - v8;
			}
			else if (a3 >= 6u)
			{
				v10 = *(unsigned __int8*)(*(int64_t*)v3)++;
				v6 = *(int64_t*)&a1[512 * a3 - 3062 + 2 * v10];
			}
			else
			{
				v6 = *(int64_t*)&a1[2 * a3 - 2] + bd_vbyte((byte**)a1 + 773);
			}
		}
		else
		{
			v6 = 0i64;
			do
			{
				v7 = *(byte*)(*(int64_t*)v3)++;
				v6 = (v6 << 7) | v7 & 0x7F;
			} while (v7 < 0);
		}
		*(int64_t*)&a1[2 * *a1 + 2] = v6;
		result = v6;
		*a1 = ((unsigned __int8)*a1 + 1) & 3;
		*(int64_t*)&a1[2 * (v6 % 0x300) + 10] = v6;
		return result;
	}

	void bdiff(core::bytebuffer::ByteBuffer& ffdata, core::bytebuffer::ByteBuffer& fddata, std::vector<byte>& outwindow) {
#pragma region vars


		byte* magicLoc; // rax
		int v10; // ecx
		byte* bdiffData; // rax
		byte* bdiffDataStart; // rbx
		unsigned __int8 flags; // r10
		char flagsm3; // r8
		byte* bdiffDataNext; // rdi
		__int64 unkvbyte0; // r9
		char v22; // dl
		unsigned int v23; // ecx
		__int64 sourceSegmentLocationEx; // rsi
		char v25; // dl
		unsigned int v26; // ecx
		__int64 sourceSegmentLocation; // r8
		char v28; // dl
		unsigned int v29; // ecx
		__int64 sizeUnk; // rsi
		byte* diffblock; // rax
		__int64 unkvbyte2; // rsi
		char v33; // dl
		unsigned int v34; // ecx
		byte* patchBuff; // rax
		char* v36; // r14
		__int64 unkvbyte3; // r13
		char v40; // dl
		unsigned int v41; // ecx
		byte* v42; // rax
		char* v43; // rbp
		int32_t* v45; // rbx
		char v46; // al
		char* v47; // r14
		__int64 v49; // rdi
		char v50; // dl
		unsigned int v51; // ecx
		__int64 v52; // rsi
		char v53; // dl
		unsigned int v54; // ecx
		__int64 v55; // r8
		char v56; // dl
		unsigned int v57; // ecx
		char* v58; // r15
		unsigned __int8* v59; // rsi
		__int64 v60; // rax
		DoubleInstruction* dinst; // rax
		__int64 i; // r13
		Instruction inst; // rax
		unsigned __int64 v64; // rbx
		char v65; // di
		unsigned int v66; // edx
		char* v67; // rsi
		char v68; // al
		__int64 v69; // rsi
		unsigned __int64 v70; // rdi
		unsigned __int64 v71; // rax
		int v72; // edi
		unsigned __int8 flags_; // [rsp+20h] [rbp-18E8h]
		size_t offset; // [rsp+28h] [rbp-18E0h] BYREF
		__int64 _unkvbyte0; // [rsp+30h] [rbp-18D8h]
		byte* diffBlockPost; // [rsp+38h] [rbp-18D0h]
		char* v79; // [rsp+40h] [rbp-18C8h]
		DoubleInstruction* v80; // [rsp+48h] [rbp-18C0h]
		unsigned __int64 v81; // [rsp+50h] [rbp-18B8h]
		__int64 v82; // [rsp+58h] [rbp-18B0h]
		unsigned __int8* v83; // [rsp+60h] [rbp-18A8h]
		byte* patchBuff2; // [rsp+68h] [rbp-18A0h]
		//vcDiffCB_t* patchDataCB2; // [rsp+70h] [rbp-1898h]
		int dest[0x1830]; // [rsp+80h] [rbp-1888h] BYREF
		unsigned __int64 v87; // [rsp+18A8h] [rbp-60h]
#pragma endregion
		struct BDiffState {
			bool headerRead{};
			bool error{};
			bool eof{};
			unsigned int features{};
		};

		// vcSourceCB_t* 
		auto sourceDataCB = [&ffdata, &fddata](size_t offset, size_t size) -> byte* {
			ffdata.Goto(offset);
			if (!ffdata.CanRead(size)) throw std::runtime_error("end source data cb");
			return ffdata.ReadPtr<byte>(size);
		};
		// vcDiffCB_t* 
		size_t patchWindowOffsetLast{};
		auto patchDataCB = [&ffdata, &fddata, &patchWindowOffsetLast](size_t offset, size_t size, size_t* pOffset) -> byte* {
			//LOG_TRACE("patchDataCB(0x{:x}, 0x{:x}, {})")
			if (!offset) {
				offset = patchWindowOffsetLast;
			}
			else {
				patchWindowOffsetLast = offset;
			}
			if (pOffset) *pOffset = offset;
			fddata.Goto(offset);
			if (!fddata.CanRead(size)) throw std::runtime_error("end patch data cb");
			return fddata.ReadPtr<byte>(size);
		};
		// vcDestCB_t* 
		auto destDataCB = [&outwindow](size_t size) -> byte* {
			if (outwindow.size() < size) {
				throw std::runtime_error("window size too small");
			}
			return outwindow.data();
		};

		BDiffState diffState[1]{};

		static byte magicBDiff[3]{ 0xD6, 0xC3, 0xC4 };
		magicLoc = patchDataCB(0i64, 0x405i64, 0i64);
		if (std::memcmp(magicLoc, magicBDiff, 3)) {
			throw std::runtime_error("Invalid bdiff magic");
		}
		v10 = *(unsigned __int8*)(magicLoc + 4);
		if ((v10 & 0xFFFFFFF3) != 0) {
			throw std::runtime_error("Invalid bdiff header");
		}
		//*(_DWORD*)&diffState[1].headerRead = v10;
		patchDataCB(5i64, 0i64, 0i64);
		diffState->headerRead = 1;
		bdiffData = patchDataCB(0i64, 0x400i64, &offset);
		bdiffDataStart = bdiffData;
		if (!bdiffData) {
			throw std::runtime_error("Invalid bdiff data 1");
		}
		flags = *bdiffData;
		flagsm3 = *bdiffData & 3;
		flags_ = *bdiffData;
		bdiffDataNext = bdiffData + 1;
		if (flagsm3 == 3) {
			// can't be 1 and 2, flags?
			throw std::runtime_error("Invalid bdiff data 2");
		}
		unkvbyte0 = 0i64;
		_unkvbyte0 = 0i64;
		diffBlockPost = 0i64;
		if (flagsm3) {
			do {
				v22 = *bdiffDataNext;
				v23 = *bdiffDataNext++ & 0x7F;
				unkvbyte0 = (unkvbyte0 << 7) | v23;
			} while (v22 < 0);
			_unkvbyte0 = unkvbyte0;
			sourceSegmentLocationEx = 0i64;
			do {
				v25 = *bdiffDataNext;
				v26 = *bdiffDataNext++ & 0x7F;
				sourceSegmentLocationEx = (sourceSegmentLocationEx << 7) | v26;
			} while (v25 < 0);
			if (flagsm3 != 1) {
				throw std::runtime_error("Invalid bdiff data 3");
			}
			if ((flags & 4) != 0) {
				sourceSegmentLocation = 0i64;
				do {
					v28 = *bdiffDataNext;
					v29 = *bdiffDataNext++ & 0x7F;
					sourceSegmentLocation = (sourceSegmentLocation << 7) | v29;
				} while (v28 < 0);
			}
			else {
				sourceSegmentLocation = sourceSegmentLocationEx;
			}
			sizeUnk = sourceSegmentLocationEx - sourceSegmentLocation;
			diffblock = sourceDataCB(sourceSegmentLocation, sizeUnk + unkvbyte0);
			flags = flags_;
			diffBlockPost = sizeUnk + diffblock;
		}
		unkvbyte2 = 0i64;
		do
		{
			v33 = *bdiffDataNext;
			v34 = *bdiffDataNext++ & 0x7F;
			unkvbyte2 = (unkvbyte2 << 7) | v34;
		} while (v33 < 0);
		patchBuff = patchDataCB(
			offset + bdiffDataNext - bdiffDataStart,
			unkvbyte2 + (((unsigned __int64)flags >> 1) & 4),
			&offset);
		v36 = (char*)patchBuff;
		if (!patchBuff) {
			throw std::runtime_error("Invalid bdiff data 4");
		}
		unkvbyte3 = 0i64;
		patchBuff2 = patchBuff;
		do {
			v40 = *v36;
			v41 = *v36++ & 0x7F;
			unkvbyte3 = (unkvbyte3 << 7) | v41;
		} while (v40 < 0);
		v82 = unkvbyte3;
		v42 = destDataCB(unkvbyte3);
		v43 = (char*)v42;
		if (!v42) {
			throw std::runtime_error("Invalid bdiff data 5");
		}
		v45 = (int32_t*)v42;
		v79 = (char*)v42;
		v46 = *v36;
		v47 = v36 + 1;
		if (v46) {
			throw std::runtime_error("Invalid bdiff data 6");
		}
		std::memset(dest, 0, 0x1830ui64);
		v49 = 0i64;
		do {
			v50 = *v47;
			v51 = *v47++ & 0x7F;
			v49 = (v49 << 7) | v51;
		} while (v50 < 0);
		v52 = 0i64;
		do {
			v53 = *v47;
			v54 = *v47++ & 0x7F;
			v52 = (v52 << 7) | v54;
		} while (v53 < 0);
		v55 = 0i64;
		do {
			v56 = *v47;
			v57 = *v47++ & 0x7F;
			v55 = (v55 << 7) | v57;
		} while (v56 < 0);
		v58 = &v47[v49];
		v81 = (unsigned __int64)&v47[v49 + v52];
		v59 = (unsigned __int8*)(v55 + v81);
		v83 = (unsigned __int8*)(v55 + v81);
		v87 = v81;
		if ((unsigned __int64)&v47[v49] >= v81)
			goto LABEL_58;
		while (2) {
			v60 = (unsigned __int8)*v58++;
			dinst = &instructions[v60];
			v80 = dinst;
			for (i = 0i64; i < 2; i++) {
				inst = dinst->instruction[i];
				if (!inst.op)
					goto LABEL_55;
				v64 = inst.size;
				if (!inst.size)
				{
					do
					{
						v65 = *v58;
						v66 = *v58++ & 0x7F;
						v64 = (v64 << 7) | v66;
					} while (v65 < 0);
				}
				if (inst.op == 1)
				{
					v67 = v47;
					v47 += v64;
				LABEL_53:
					std::memcpy(v43, v67, v64);
					goto LABEL_54;
				}
				if (inst.op != 2)
				{
					v69 = (__int64)v79;
					v70 = _unkvbyte0;
					v71 = unk_bdiff(dest, _unkvbyte0 + v43 - v79, inst.mode);
					if (v71 < v70) {
						if (!diffBlockPost) {
							throw std::runtime_error("Invalid bdiff data 7");
						}
						v67 = (char*)(v71 + diffBlockPost);
					}
					else {
						v67 = (char*)(v71 - v70 + v69);
					}
					goto LABEL_53;
				}
				v68 = *v47++;
				memset(v43, v68, v64);
			LABEL_54:
				v43 += v64;
			LABEL_55:
				dinst = v80;
			}
			if ((unsigned __int64)v58 < v81)
				continue;
			break;
		}
		v45 = (int32_t*)v79;
		unkvbyte3 = v82;
		v59 = v83;
	LABEL_58:
		if ((flags_ & 8) != 0)
		{
			v72 = *v59 | ((v59[1] | (*((unsigned __int16*)v59 + 1) << 8)) << 8);
			if (v72 != (unsigned int)fastfiles::ComputeChecksum32((const char*)v45, (unsigned int)unkvbyte3, 0))
				throw std::runtime_error("Invalid bdiff data 8");
			v59 += 4;
		}
		if (patchDataCB(v59 + offset - patchBuff2, 0i64, 0i64))
			return;
		throw std::runtime_error("Invalid bdiff data 9");
	}

	static deps::oodle::Oodle oodle{};

	class T78FFDecompressor : public fastfile::FFDecompressor {
	public:
		T78FFDecompressor() : fastfile::FFDecompressor("Black Ops 3/4", 0x3030303066664154, fastfile::MASK64) {}

		void LoadFastFile(fastfile::FastFileOption& opt, core::bytebuffer::ByteBuffer& reader, fastfile::FastFileContext& ctx, std::vector<byte>& ffdata) {
			if (!reader.CanRead(sizeof(fastfile::TXFileHeader))) {
				throw std::runtime_error("Can't read XFile header");
			}

			fastfile::TXFileHeader* header{ reader.Ptr<fastfile::TXFileHeader>() };

			if (opt.m_header) {
				LOG_INFO("{} fastfile version 0x{:x}, encrypted:{}, platform: {}, builder: {}", 
					fastfile::GetFastFileCompressionName(header->compression), header->version, header->encrypted ? "true" : "false", 
					fastfile::GetFastFilePlatformName(header->platform), header->builder
				);
			}

			size_t fastFileSize;
			size_t decompressedSizeLoc;
			size_t fastfileNameLoc;
			size_t aesIVLoc{};
			size_t aesIVSize{};

			bool xhashType{};

			// todo: probably we can scan the data and find the chunks
			switch (header->version) {
			case 0x251: // Black ops 3
				fastFileSize = 0x248;
				decompressedSizeLoc = 0x90;
				fastfileNameLoc = 0xF8;
				break;
			case 0x265: // Black ops 4 Dev
				fastFileSize = 0x638;
				decompressedSizeLoc = 0x490;
				fastfileNameLoc = 0x4E8;
				aesIVLoc = 0x628;
				aesIVSize = 16;
				break;
			case 0x27E:
			case 0x27F: // Black Ops 4
				fastFileSize = 0x840;
				decompressedSizeLoc = 0x490;
				fastfileNameLoc = 0x6F0;
				xhashType = true;
				break;
			default:
				throw std::runtime_error(std::format("Fast file version not supported: 0x{:x}", header->version));
			}

			if (!reader.CanRead(fastFileSize)) {
				throw std::runtime_error("Can't read XFile header");
			}
			reader.Goto(decompressedSizeLoc);
			uint64_t decompressedSize{ reader.Read<uint64_t>() };

			reader.Goto(fastfileNameLoc);
			ctx.ffname = reader.ReadString();

			byte* aesIV{};
			if (aesIVLoc) {
				if (!aesIVSize) throw std::runtime_error("Missing aesIVSize value");
				reader.Goto(aesIVLoc);
				aesIV = reader.ReadPtr<byte>(aesIVSize);
			}

			if (opt.m_header) {
				LOG_INFO("{} size:0x{:x}", ctx.ffname, decompressedSize);
			}

			reader.Goto(fastFileSize);


			BCRYPT_ALG_HANDLE hAlgorithm{};
			BCRYPT_KEY_HANDLE hKey{};
			utils::CloseEnd hProvCE{ [hAlgorithm, hKey] {
				if (hKey) BCryptDestroyKey(hKey);
				if (hAlgorithm) BCryptCloseAlgorithmProvider(hAlgorithm, 0);
			} };

			if (header->encrypted) {
				if (!aesIV) {
					throw std::runtime_error(std::format("No aesIV location for this fast file version 0x{:x}", header->version));
				}
				NTSTATUS status;
				compatibility::acti::crypto_keys::AesKeyLocal* key{ compatibility::acti::crypto_keys::GetKeyByName(ctx.ffname) };

				if (!key) {
					throw std::runtime_error(std::format("Missing aes key for ff {}", ctx.ffname));
				}

				status = BCryptOpenAlgorithmProvider(&hAlgorithm, BCRYPT_AES_ALGORITHM, NULL, 0);
				if (status < 0) {
					throw std::runtime_error(std::format("Can't acquire algorithm provider {}", status));
				}

				struct Aes256Blob {
					BCRYPT_KEY_DATA_BLOB_HEADER header;
					BYTE key[32];
				};
				Aes256Blob blob{};
				blob.header.dwMagic = BCRYPT_KEY_DATA_BLOB_MAGIC;
				blob.header.dwVersion = 1;
				blob.header.cbKeyData = sizeof(blob.key);

				static_assert(sizeof(blob.key) == sizeof(key->key));
				std::memcpy(blob.key, key->key, sizeof(blob.key));


				status = BCryptImportKey(hAlgorithm, NULL, BCRYPT_KEY_DATA_BLOB, &hKey, NULL, 0, (PUCHAR)&blob, sizeof(blob), 0);
				if (status < 0) {
					throw std::runtime_error(std::format("Failed to import key {}", status));
				}
			}


			ffdata.resize(decompressedSize);

			size_t idx{};
			size_t offset{};
			while (offset < decompressedSize) {
				size_t loc{ reader.Loc() };

				fastfile::DBStreamHeader* block{ reader.ReadPtr<fastfile::DBStreamHeader>() };

				if (block->offset != loc) {
					LOG_ERROR("bad block position: 0x{:x} != 0x{:x}", loc, block->offset);
					break;
				}
				idx++;

				if (!block->uncompressedSize) {
					reader.Align(0x800000);
					continue;
				}

				byte* blockBuff{ reader.ReadPtr<byte>(block->alignedSize) };


				LOG_TRACE("Decompressing {}{} block 0x{:x} (0x{:x}/0x{:x} -> 0x{:x})",
					header->encrypted ? "encrypted " : "",
					fastfile::GetFastFileCompressionName(header->compression),
					loc,
					block->compressedSize, block->alignedSize, block->uncompressedSize
				);

				byte* decompressed{ &ffdata[offset] };
				offset += block->uncompressedSize;

				if (header->encrypted) {
					DWORD comp{ block->compressedSize };
					//if (!CryptDecrypt(hKey, hHash, true, 0, blockBuff, &comp)) {
					//	throw std::runtime_error(std::format("Can't read decrypt chunk 0x{:x}", GetLastError()));
					//}
					DWORD decryptedDataSize{};
					NTSTATUS status = BCryptDecrypt(hKey, blockBuff, block->alignedSize, NULL, aesIV, (ULONG)aesIVSize, NULL, 0, &decryptedDataSize, BCRYPT_BLOCK_PADDING);
					
					if (status < 0) {
						throw std::runtime_error(std::format("Can't decrypt block {:x}", status));
					}

					LOG_TRACE("decryptedDataSize=0x{:x}", decryptedDataSize);
					continue;
				}

				switch (header->compression) {
				case fastfile::XFILE_UNCOMPRESSED:
					if (block->uncompressedSize > block->compressedSize) {
						throw std::runtime_error(std::format("Can't decompress block, decompressed size isn't big enough: 0x{:x} != 0x{:x}", block->compressedSize, block->uncompressedSize));
					}
					memcpy(decompressed, blockBuff, block->uncompressedSize);
					break;
				case fastfile::XFILE_ZLIB:
				case fastfile::XFILE_ZLIB_HC: {

					uLongf sizef = (uLongf)block->uncompressedSize;
					uLongf sizef2{ (uLongf)block->compressedSize };
					int ret;
					if (ret = uncompress2(decompressed, &sizef, blockBuff, &sizef2) < 0) {
						throw std::runtime_error(std::format("error when decompressing {}", zError(ret)));
					}

					if (block->uncompressedSize != sizef) {
						throw std::runtime_error(std::format("Can't decompress block, returned size isn't the expected one: 0x{:x} != 0x{:x}", sizef, block->uncompressedSize));
					}
					break;
				}
				case fastfile::XFILE_OODLE_KRAKEN:
				case fastfile::XFILE_OODLE_MERMAID:
				case fastfile::XFILE_OODLE_SELKIE:
				case fastfile::XFILE_OODLE_LZNA: {
					if (!oodle && !oodle.LoadOodle(deps::oodle::OO2CORE_6)) {
						throw std::runtime_error("Oodle is required to read this fastfile, did you put oo2core_6_win64.dll inside the deps directory?");
					}
					int ret{ oodle.Decompress(blockBuff, block->compressedSize, decompressed, block->uncompressedSize, deps::oodle::OODLE_FS_YES) };

					if (ret != block->uncompressedSize) {
						throw std::runtime_error(std::format("Can't decompress block, returned size isn't the expected one: 0x{:x} != 0x{:x}", ret, block->uncompressedSize));
					}
					break;
				}
				default:
					throw std::runtime_error(std::format("No fastfile decompressor for type {}", (int)header->compression));
				}
			}

			LOG_TRACE("Decompressed 0x{:x} byte(s) from 0x{:x} block(s)", ffdata.size(), idx);

			if (opt.m_fd) {
				std::filesystem::path fdfile{ ctx.file };

				fdfile.replace_extension(".fd");

				std::vector<byte> fileFDBuff{};
				if (opt.ReadFile(fdfile.string(), fileFDBuff)) {
					core::bytebuffer::ByteBuffer fdreader{ fileFDBuff };

					// todo: handle fd file

					/*
					struct BDiffHeader {
						XFile newXFileHeader;
						XFile baseXFileHdr;
						uint32_t size;
						float version;
						uint32_t flags;
						size_t maxDestWindowSize;
						uint64_t maxSourceWindowSize;
						uint64_t maxDiffWindowSize;
					}; static_assert(sizeof(BDiffHeader) == 0x10a8);
					*/

					struct BDiffHeaderPost {
						uint32_t size;
						float version;
						uint32_t flags;
						size_t maxDestWindowSize;
						uint64_t maxSourceWindowSize;
						uint64_t maxDiffWindowSize;
					}; static_assert(sizeof(BDiffHeaderPost) == 0x28);

					size_t bdiffHeaderSize{ fastFileSize * 2 + sizeof(BDiffHeaderPost) };

					if (!fdreader.CanRead(bdiffHeaderSize)) {
						throw std::runtime_error("Can't read patch file header");
					}

					fastfile::TXFileHeader* newXFileHeader{ fdreader.Ptr<fastfile::TXFileHeader>() };
					fdreader.Goto(decompressedSizeLoc);
					uint64_t fdDecompressedSize{ fdreader.Read<uint64_t>() };
					fdreader.Goto(fastFileSize);
					fastfile::TXFileHeader* baseXFileHdr{ fdreader.Ptr<fastfile::TXFileHeader>() };
					fdreader.Goto(fastFileSize * 2);
					BDiffHeaderPost* bdiffHeader{ fdreader.ReadPtr<BDiffHeaderPost>() };

					if (opt.m_header) {
						LOG_INFO("s:0x{:x}, version:{}, flags:0x{:x} 0x{:x} 0x{:x} 0x{:x}", 
							bdiffHeader->size, bdiffHeader->version, bdiffHeader->flags, bdiffHeader->maxDestWindowSize, bdiffHeader->maxSourceWindowSize, bdiffHeader->maxDiffWindowSize);
					}

					if (bdiffHeader->size != bdiffHeaderSize) {
						throw std::runtime_error(std::format("Can't read patch file header size: 0x{:x} != 0x{:x}", bdiffHeader->size, bdiffHeaderSize));
					}

					if (std::memcmp(baseXFileHdr, header, fastFileSize)) {
						throw std::runtime_error("The patch file is for this fast file");
					}

					size_t compressedSize{ fdreader.Remaining() };

					LOG_TRACE("Decompressing patch {} 0x{:x}: 0x{:x} -> 0x{:x}", fastfile::GetFastFileCompressionName(newXFileHeader->compression), fdreader.Loc(), compressedSize, fdDecompressedSize);
					auto uncompress{ std::make_unique<byte[]>(fdDecompressedSize) };
					size_t uncompressSize{};

					switch (newXFileHeader->compression) {
					case fastfile::XFILE_BDELTA_UNCOMP: {
						if (fdDecompressedSize > compressedSize) {
							throw std::runtime_error(std::format("Can't decompress block, decompressed size isn't big enough: 0x{:x} != 0x{:x}", compressedSize, fdDecompressedSize));
						}
						memcpy(uncompress.get(), fdreader.ReadPtr<byte>(fdDecompressedSize), fdDecompressedSize);
						uncompressSize = fdDecompressedSize;
						break;
					}
					case fastfile::XFILE_BDELTA_ZLIB: {
						uLongf sizef = (uLongf)fdDecompressedSize;
						uLongf sizef2{ (uLongf)(compressedSize) };
						int ret;
						if (ret = uncompress2((Bytef*)uncompress.get(), &sizef, fdreader.Ptr<const Bytef>(), &sizef2) < 0) {
							throw std::runtime_error(std::format("error when decompressing {}", zError(ret)));
						}

						uncompressSize = sizef;
						break;
					}
					//case fastfile::XFILE_BDELTA_LZMA:
					default:
						throw std::runtime_error(std::format("No fastfile decompressor for type {}", (int)newXFileHeader->compression));
					}

					LOG_TRACE("Decompressed 0x{:x} byte(s) from patch file", uncompressSize);

					core::bytebuffer::ByteBuffer patchdata{ uncompress.get(), uncompressSize };
					// todo: read patch data

					core::bytebuffer::ByteBuffer ffwriter{ ffdata };

					std::vector<byte> outwindow{};
					outwindow.resize(bdiffHeader->maxDestWindowSize);
					bdiff(ffwriter, patchdata, outwindow);
				}
				else {
					LOG_TRACE("No patch file {}", fdfile.string());
					// this isn't an error because we might not have a patch file
				}
			}
		}
	};

	utils::ArrayAdder<T78FFDecompressor, fastfile::FFDecompressor> arr{ fastfile::GetDecompressors() };
}