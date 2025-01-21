#include <includes.hpp>
#include <core/bytebuffer.hpp>
#include <hook/module_mapper.hpp>
#include <hook/memory.hpp>
#include <deps/oodle.hpp>
#include <deps/bo4.hpp>
#include <games/bo4/pool.hpp>
#include <utils/decrypt.hpp>

namespace fastfiles {
	int ComputeChecksum32(const char* buffer, unsigned int len, int start) {
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

}
#include <deps/base64.hpp>

#pragma warning(push)
#pragma warning(disable:4996)
#include <CascLib.h>
#include <CascCommon.h>
#pragma warning(pop)

namespace {
	struct Instruction {
		__int32 op : 16;
		unsigned __int32 mode : 16;
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


	void WriteHex(std::ostream& out, uintptr_t base, byte* buff, size_t size) {
		for (size_t j = 0; j < size; j++) {
			if (j % 8 == 0) {
				if (base) {
					out << std::hex << std::setw(16) << std::setfill('0') << (base + j) << "~";
				}
				out << std::hex << std::setw(3) << std::setfill('0') << j << "|";
				if (j + 7 < size) {
					out << std::hex << std::setw(16) << std::setfill('0') << *reinterpret_cast<uint64_t*>(&buff[j]);
				}
			}
			if (j - j % 8 + 7 >= size) {
				out << std::hex << std::setw(2) << std::setfill('0') << (int)buff[j];
			}
			if ((j + 1) % 8 == 0) {
				out << "|";

				for (size_t i = j - 7; i <= j; i++) {
					if (buff[i] >= ' ' && buff[i] <= '~') {
						out << (char)buff[i];
					}
					else {
						out << ".";
					}
				}

				// check x64 values
				if (j >= 7) {
					auto val = *reinterpret_cast<uint64_t*>(&buff[j - 7]);
					if (val) {
						// not null, hash?
						auto* h = hashutils::ExtractPtr(val);
						if (h) {
							out << " #" << h;
						}
					}
				}
				out << "\n";
			}
		}
		out << "\n";
	}
	enum XFileCompression : byte {
		XFILE_UNCOMPRESSED = 0x0,
		XFILE_ZLIB = 0x1,
		XFILE_ZLIB_HC = 0x2,
		XFILE_LZ4 = 0x3,
		XFILE_LZ4_HC = 0x4,
		XFILE_BDELTA_UNCOMP = 0x5,
		XFILE_BDELTA_ZLIB = 0x6,
		XFILE_BDELTA_LZMA = 0x7,
		XFILE_OODLE_KRAKEN = 0x8,
		XFILE_OODLE_MERMAID = 0x9,
		XFILE_OODLE_SELKIE = 0xA,
		XFILE_OODLE_LZNA = 0xB,
		XFILE_COMPRESSION_COUNT = 0xC,
	};
	const char* CompressionNames[]{
		"none",
		"zlib",
		"zlib_hc",
		"lz4",
		"lz4_hc",
		"bdelta_uncomp",
		"bdelta_zlib",
		"bdelta_lzma",
		"oodle_kraken",
		"oodle_mermaid",
		"oodle_selkie",
		"oodle_lzna",
	};
	struct XFile {
		uint8_t magic[8];
		uint32_t version;
		uint8_t server;
		XFileCompression compression;
		uint8_t platform;
		uint8_t encrypted;
		uint64_t timestamp;
		uint32_t changelist;
		uint32_t archiveChecksum[4];
		char builder[32];
		uint32_t metaVersion;
		char mergeFastfile[64];
		char missionFastFiles[16][64];
		uint64_t size;
		uint64_t externalSize;
		uint64_t memMappedOffset;
		uint64_t blockSize[8];
		int8_t fastfileName[64];
		uint8_t signature[256];
		uint8_t aesIV[16];
		byte pad0[184];
		char key[8];
		byte pad1[328];
	}; static_assert(sizeof(XFile) == 0x840);
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

	struct DBStreamHeader {
		uint32_t compressedSize;
		uint32_t uncompressedSize;
		uint32_t alignedSize;
		uint32_t offset;
	}; static_assert(sizeof(DBStreamHeader) == 0x10);


	class FastFileOption {
	public:
		bool m_help{};
		bool m_fd{};
		bool m_header{};
		const char* m_casc{};
		const char* game{};
		HANDLE cascStorage{};
		const char* m_output{ "output_ff" };
		std::vector<const char*> files{};

		~FastFileOption() {
			if (cascStorage) {
				CascCloseStorage(cascStorage);
			}
		}

		bool Compute(const char** args, INT startIndex, INT endIndex) {
			// default values
			for (size_t i = startIndex; i < endIndex; i++) {
				const char* arg = args[i];

				if (!strcmp("-?", arg) || !_strcmpi("--help", arg) || !strcmp("-h", arg)) {
					m_help = true;
				}
				else if (!strcmp("-o", arg) || !_strcmpi("--output", arg)) {
					if (i + 1 == endIndex) {
						std::cerr << "Missing value for param: " << arg << "!\n";
						return false;
					}
					m_output = args[++i];
				}
				else if (!strcmp("-g", arg) || !_strcmpi("--game", arg)) {
					if (i + 1 == endIndex) {
						std::cerr << "Missing value for param: " << arg << "!\n";
						return false;
					}
					game = args[++i];
				}
				else if (!strcmp("-C", arg) || !_strcmpi("--casc", arg)) {
					if (i + 1 == endIndex) {
						std::cerr << "Missing value for param: " << arg << "!\n";
						return false;
					}
					m_casc = args[++i];

					if (!CascOpenStorage(m_casc, 0, &cascStorage)) {
						LOG_ERROR("Can't open casc storage {}: 0x{:x}", m_casc, GetCascError());
						return false;
					}
				}
				else if (!_strcmpi("--fd", arg) || !strcmp("-p", arg)) {
					m_fd = true;
				}
				else if (!_strcmpi("--header", arg) || !strcmp("-H", arg)) {
					m_header = true;
				}
				else if (*arg == '-') {
					std::cerr << "Invalid argument: " << arg << "!\n";
					return false;
				}
				else {
					files.push_back(arg);
				}
			}
			return true;
		}
		void PrintHelp() {
			LOG_INFO("-h --help            : Print help");
			LOG_INFO("-o --output [d]      : Output dir");
			LOG_INFO("-H --header          : Dump header info");
			LOG_INFO("-C --casc [c]        : Use casc db");
			LOG_INFO("-g --game [g]        : exe");
			LOG_INFO("-p --fd              : Use patch file (fd)");
		}

		std::vector<std::string> GetFileRecurse(const char* path) {
			std::vector<std::string> res{};
			if (cascStorage) {
				CASC_FIND_DATA data;

				HANDLE firstFileHandle{ CascFindFirstFile(cascStorage, path, &data, NULL) };

				if (!firstFileHandle) {
					LOG_ERROR("Can't find path {}", path);
					return res;
				}
				utils::CloseEnd ce{ [firstFileHandle] { CascFindClose(firstFileHandle); } };


				do {
					if (data.bFileAvailable) {
						res.push_back(data.szFileName);
					}
				} while (CascFindNextFile(firstFileHandle, &data));
			}
			else {
				std::vector<std::filesystem::path> files{};

				utils::GetFileRecurse(path, files);


				for (const std::filesystem::path& file : files) {
					res.push_back(file.string());
				}
			}

			return res;

		}

		bool ReadFile(const char* path, std::vector<byte>& buff) {
			if (cascStorage) {
				CASC_FIND_DATA data;

				HANDLE firstFileHandle{ CascFindFirstFile(cascStorage, path, &data, NULL) };

				if (!firstFileHandle || !data.bFileAvailable) {
					LOG_ERROR("Can't find path {}", path);
					return false;
				}
				utils::CloseEnd ce{ [firstFileHandle] { CascFindClose(firstFileHandle); } };


				buff.clear();
				if (!data.FileSize) {
					return true; // nothing to read
				}
				buff.resize(data.FileSize);
				HANDLE fhandle;
				if (!CascOpenFile(cascStorage, data.szFileName, CASC_LOCALE_ALL, 0, &fhandle)) {
					LOG_ERROR("Can't open path {}", data.szFileName);
					return false;
				}
				utils::CloseEnd cef{ [fhandle] { CascCloseFile(fhandle); } };

				DWORD read{};
				return CascReadFile(fhandle, buff.data(), (DWORD)data.FileSize, &read) && read == data.FileSize;
			}

			return utils::ReadFile(path, buff);
		}
	};

	enum BDiffFlags : byte {
		BDF_START_BD = 1,
		BDF_UNK2 = 2,
		BDF_MASK21 = BDF_START_BD | BDF_UNK2,
		BDF_FROM_START = 4,
		BDF_UNK8 = 8,
	};

	void bdiff(core::bytebuffer::ByteBuffer& patchData, core::bytebuffer::ByteBuffer& buffer) {
		size_t offset{ 0x405 };
		if (!patchData.CanRead(offset)) {
			throw std::runtime_error(std::format("Can't read 0x{:x} bytes", offset));
		}

		byte* bdiffMagic{ patchData.ReadPtr<byte>(5) };

		if (std::memcmp(bdiffMagic, "\xd6\xc3\xc4", 3)) {
			throw std::runtime_error(std::format("Invalid bdiff magic for 0x{:x} 0x{:x} 0x{:x}", bdiffMagic[0], bdiffMagic[1], bdiffMagic[2]));
		}

		if (bdiffMagic[4] & 0xF3) {
			LOG_WARNING("unk & 0xF3, error?");
		}

		// ok bdiff

		while (patchData.CanRead(1)) {
			byte bdiffFlags{ patchData.Read<byte>() };

			size_t unkShift{ (size_t)((bdiffFlags >> 1) & 4) };

			if ((bdiffFlags & BDF_UNK2)) {
				throw std::runtime_error("flag 2 sets, can't load");
			}

			LOG_INFO("---------- {:x}", patchData.Loc());
			// diff? maybe offset/size -> data[]
			size_t unk0;
			byte* diffBlockPost{};
			if (bdiffFlags & BDF_MASK21) {
				// layer start
				unk0 = patchData.ReadVByte();
				size_t sourceSegmentLocationEx{ patchData.ReadVByte() };

				size_t sourceSegmentLocation;

				if (bdiffFlags & BDF_FROM_START) {
					sourceSegmentLocation = patchData.ReadVByte();
				}
				else {
					sourceSegmentLocation = sourceSegmentLocationEx;
				}

				size_t sizeUnk{ sourceSegmentLocationEx - sourceSegmentLocation };
				size_t rsized{ sizeUnk + unk0 };

				LOG_INFO("{:x} {:x} {:x} {:x} {:x}", unk0, sourceSegmentLocationEx, sourceSegmentLocation, sizeUnk, rsized);
				buffer.Goto(sourceSegmentLocation);
				byte* diffblock{ buffer.ReadPtr<byte>(rsized) };
				diffBlockPost = diffblock + sizeUnk;
			}
			else {
				// layer post
				throw std::runtime_error("bdiff 0 part not implemented");
			}

			size_t unkvbyte2{ patchData.ReadVByte() };
			LOG_INFO("{:x} {:x}", unkvbyte2, unkvbyte2 + unkShift);

			// patch data:
			// off: ptr + offset - bdiffDataStart 
			// siz: unkvbyte2 + unkShift
			byte* patchDataBf{ patchData.ReadPtr<byte>(unkvbyte2 + unkShift) };

			//size_t unkvbyte3{ patchData.ReadVByte() };


			//utils::OutFileCE os{ "output_ff/test.ff.dec" };
			//WriteHex(os, sizeof(*fdheader), uncompress.get(), uncompressSize);
		}
	}

	int fftest(int argc, const char* argv[]) {
		FastFileOption opt{};

		if (!opt.Compute(argv, 2, argc) || opt.m_help) {
			opt.PrintHelp();
			return opt.m_help ? tool::OK : tool::BASIC_ERROR;
		}

		deps::bo4::Bo4 bo4{};

		if (opt.m_fd) {
			if (!bo4.LoadFromDeps()) {
				LOG_ERROR("Can't load bo4 exec from deps, did you put a dump inside the deps/ directory?");
				return tool::BASIC_ERROR;
			}
			bo4.PatchErrorAsThrow();
			LOG_INFO("{} loaded", *bo4);
		}

		std::vector<byte> fileBuff{};
		std::vector<byte> fileFDBuff{};
		for (const char* f : opt.files) {
			for (const std::string filename : opt.GetFileRecurse(f)) {
				if (!filename.ends_with(".ff")) {
					LOG_DEBUG("Ignore {}", filename);
					continue;
				}
				if (!opt.ReadFile(filename.data(), fileBuff)) {
					LOG_ERROR("Can't read file {}", filename);
					return tool::BASIC_ERROR;
				}

				LOG_INFO("Reading {}", filename);

				core::bytebuffer::ByteBuffer reader{ fileBuff };

				int ret{};
				auto PrintXFile = [&opt, &ret, &reader](XFile* buffer) {
					if (*reinterpret_cast<uint64_t*>(buffer->magic) != 0x3030303066664154) {
						LOG_ERROR("Invalid FF magic: {:x}", *reinterpret_cast<uint64_t*>(buffer));
						ret = tool::BASIC_ERROR;
						return;
					}

					if (buffer->version != 0x27F && buffer->version != 0x27E) {
						LOG_ERROR("Not a T8 FF, version: 0x{:X}", buffer->version);
						ret = tool::BASIC_ERROR;
						return;
					}

					if (buffer->compression >= XFILE_COMPRESSION_COUNT) {
						LOG_ERROR("Not implemented for this compression type: {}", (int)buffer->compression);
						ret = tool::BASIC_ERROR;
						return;
					}

					if (!opt.m_header) return;


					if (buffer->encrypted) {
						auto checksum32 = fastfiles::ComputeChecksum32(buffer->key, (unsigned int)strlen(buffer->key), 0);
						LOG_INFO("checksum32 . {}", checksum32);
					}

					LOG_INFO("server ............. {}", (buffer->server ? "true" : "false"));
					LOG_INFO("encrypted .......... {}", (buffer->encrypted ? "true" : "false"));
					LOG_INFO("compress ........... {} (0x{:x})", CompressionNames[buffer->compression], (int)buffer->compression);
					LOG_INFO("platform ........... {}", (int)buffer->platform);
					LOG_INFO("builder ............ {}", buffer->builder);
					LOG_INFO("timestamp .......... {}", buffer->timestamp);

					for (size_t i = 0; i < 4; i++) {
						LOG_INFO("archiveChecksum[{}] . {:x}", i, buffer->archiveChecksum[i]);
					}

					LOG_INFO("key ................ {}", buffer->key);
					};

				if (opt.m_header) LOG_INFO("size ............... {}B (0x{:x})", utils::FancyNumber(fileBuff.size()), fileBuff.size());
				XFile* buffer = reader.ReadPtr<XFile>();
				PrintXFile(buffer);
				if (ret) return ret;

				deps::oodle::Oodle oodle{ deps::oodle::OO2CORE_8 };

				if (!oodle) return tool::BASIC_ERROR;
				
				size_t idx{};

				std::vector<byte> ffdata{};
				while (true) {
					size_t loc{ reader.Loc() };

					DBStreamHeader* block{ reader.ReadPtr<DBStreamHeader>() };
					if (!block->alignedSize) break; // end

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


					LOG_TRACE("Decompressing block 0x{:x} (0x{:x}/0x{:x} -> 0x{:x})", loc, block->compressedSize, block->alignedSize, block->uncompressedSize);

					auto decompressed{ std::make_unique<byte[]>(block->uncompressedSize) };

					switch (buffer->compression) {
					case XFILE_UNCOMPRESSED:
						if (block->uncompressedSize > block->compressedSize) {
							throw std::runtime_error(std::format("Can't decompress block, decompressed size isn't big enough: 0x{:x} != 0x{:x}", block->compressedSize, block->uncompressedSize));
						}
						memcpy(decompressed.get(), blockBuff, block->uncompressedSize);
						break;
					case XFILE_ZLIB:
					case XFILE_ZLIB_HC: {

						uLongf sizef = (uLongf)block->uncompressedSize;
						uLongf sizef2{ (uLongf)block->compressedSize };
						int ret;
						if (ret = uncompress2(decompressed.get(), &sizef, reader.Ptr<const Bytef>(), &sizef2) < 0) {
							throw std::runtime_error(std::format("error when decompressing {}", zError(ret)));
						}
						break;
					}
					case XFILE_OODLE_KRAKEN:
					case XFILE_OODLE_MERMAID:
					case XFILE_OODLE_SELKIE:
					case XFILE_OODLE_LZNA: {
						int ret{ oodle.Decompress(blockBuff, block->compressedSize, decompressed.get(), block->uncompressedSize, deps::oodle::OODLE_FS_YES) };

						if (ret != block->uncompressedSize) {
							throw std::runtime_error(std::format("Can't decompress block, returned size isn't the expected one: 0x{:x} != 0x{:x}", ret, block->uncompressedSize));
						}
						break;
					}
					default:
						throw std::runtime_error(std::format("No fastfile decompressor for type {}", (int)buffer->compression));
					}


					utils::WriteValue(ffdata, decompressed.get(), block->uncompressedSize);
				}

				LOG_INFO("Decompressed 0x{:x} byte(s) from 0x{:x} block(s)", ffdata.size(), idx);

				core::bytebuffer::ByteBuffer buff{ ffdata };

				if (opt.m_fd) {
					std::filesystem::path fdfile{ filename };

					fdfile.replace_extension(".fd");
					const std::string filenameFd{ fdfile.string() };

					if (!opt.ReadFile(filenameFd.data(), fileFDBuff)) {
						LOG_INFO("No fd file for {}: {}", filename, filenameFd);
					}
					else {
						core::bytebuffer::ByteBuffer bufffd{ fileFDBuff };

						BDiffHeader* fdheader{ bufffd.ReadPtr<BDiffHeader>() };

						if (fdheader->size != sizeof(BDiffHeader)) {
							throw std::runtime_error(std::format("Invalid xfile fd header size 0x{:x}", fdheader->size));
						}

						LOG_INFO("Loading fd file {}", filenameFd);

						if (opt.m_header) {
							LOG_INFO("fd size ............... {}B (0x{:x})", utils::FancyNumber(fileFDBuff.size()), fileFDBuff.size());
							LOG_INFO("---- header ----");
						}
						PrintXFile(&fdheader->baseXFileHdr);
						if (opt.m_header) LOG_INFO("---- new header ----");
						PrintXFile(&fdheader->newXFileHeader);
						if (ret) return ret;

						LOG_INFO("size 0x{:x}", fdheader->size);
						LOG_INFO("version {}", fdheader->version);
						LOG_INFO("flags 0x{:x}", fdheader->flags);
						LOG_INFO("maxDestWindowSize   0x{:x}", fdheader->maxDestWindowSize);
						LOG_INFO("maxSourceWindowSize 0x{:x}", fdheader->maxSourceWindowSize);
						LOG_INFO("maxDiffWindowSize   0x{:x}", fdheader->maxDiffWindowSize);

						auto uncompress{ std::make_unique<byte[]>(fdheader->maxSourceWindowSize)};
						size_t uncompressSize{};

						switch (fdheader->newXFileHeader.compression) {
						case XFILE_BDELTA_ZLIB: {
							uLongf sizef = (uLongf)fdheader->maxSourceWindowSize;
							uLongf sizef2{ (uLongf)(fileFDBuff.size() - sizeof(BDiffHeader)) };
							int ret;
							if (ret = uncompress2((Bytef*)uncompress.get(), &sizef, bufffd.Ptr<const Bytef>(), &sizef2) < 0) {
								throw std::runtime_error(std::format("error when decompressing {}", zError(ret)));
							}

							uncompressSize = sizef;
						}
							break;
						//case XFILE_BDELTA_UNCOMP:
						//case XFILE_BDELTA_LZMA:
						default:
							throw std::runtime_error(std::format("No fastfile decompressor for type {}", (int)fdheader->baseXFileHdr.compression));
						}

						core::bytebuffer::ByteBuffer buffbd{ uncompress.get(), uncompressSize };

						deps::bo4::BDiffState state{};

						static struct {
							core::bytebuffer::ByteBuffer* buffbd{};
							core::bytebuffer::ByteBuffer* buffsrc{};
							int64_t size{};
							byte* destWindow{};
						} ffDecode{};

						ffDecode.buffbd = &buffbd;
						ffDecode.buffsrc = &buff;
						ffDecode.size = 0;
						ffDecode.destWindow = uncompress.get() + 0x80000 + 0x80000;

						if (!bo4.bdiff(&state,
							[](uint64_t offset, uint64_t size) -> byte* {
								// srcDataCallback
								ffDecode.buffsrc->Goto(offset);
								if (!ffDecode.buffbd->CanRead(size)) return nullptr;
								return ffDecode.buffbd->ReadPtr<byte>(size);
							}, 
							[](uint64_t offset, uint64_t size, uint64_t* pOffset) -> byte* {
								// patchDataCB
								if (offset) {
									ffDecode.buffbd->Goto(offset);
								}
								if (!ffDecode.buffbd->CanRead(size)) return nullptr;
								byte* ret{ ffDecode.buffbd->ReadPtr<byte>(size) };
								if (pOffset) *pOffset = ffDecode.buffbd->Loc();
								return ret;
							},
							[](uint64_t size) -> byte* {
								// destDataCB
								return ffDecode.destWindow;
							} 
							)) {
							LOG_ERROR("Error when bdiff");
						}
					}

				}
				// search gscobj
				{
					/*
					 The pattern is :
					 align<8>
					 ScriptParseTree {
					    u64 name;
						u64 namepad?; // maybe not there
						void* buffer = -1;
						u32 size;
						u32 unk;
					 }
					 align<32>
					 GSC_OBJ {
					    u64 magic = 0x??00a0d43534780
					 }
					
					*/
					uint64_t magic{ 0x00a0d43534780 };
					uint64_t magicMask{ 0xFFFFFFFFFFFFF };

					size_t loc{};
					std::filesystem::path out{ "scriptparsetree_t8ff" };
					buff.Goto(0);
					byte* start{ buff.Ptr<byte>() };
					while (true) {
						loc = buff.FindMasked((byte*)&magic, (byte*)&magicMask, sizeof(magic));
						if (loc == std::string::npos) break;
						struct T8GSCOBJ {
							byte magic[8];
							int32_t crc;
							int32_t pad;
							uint64_t name;
						};

						buff.Goto(loc);

						if (!buff.CanRead(sizeof(T8GSCOBJ))) {
							break; // can't read buffer
						}

						byte* sptCan{ buff.Ptr<byte>() - 0x18 }; // 0x18 is the minimum size to lookup

						// we are 32 bytes aligned
						T8GSCOBJ* obj{ buff.ReadPtr<T8GSCOBJ>() };

						uint64_t name{ obj->name };

						while (*(uint64_t*)sptCan != name) {
							sptCan--;
							if (start == sptCan) {
								break;
							}
						}
						if (start == sptCan) {
							loc++;
							continue;
						}


						uint64_t smagic{ *reinterpret_cast<uint64_t*>(obj) };

						size_t size;
						struct T8SPT {
							uint64_t name;
							uint64_t pad0;
							uintptr_t buffer;
							uint32_t size;
							uint32_t pad02;
						};
						struct T8SPTOld {
							uint64_t name;
							uintptr_t buffer;
							uint32_t size;
							uint32_t pad02;
						};

						// Depending on how old the ff is, we might use a XHash of 0x10 or 8 bytes. The pointer
						// to the buffer will constantly be -1 because it is not linked yet
						if (((T8SPT*)sptCan)->buffer == 0xFFFFFFFFFFFFFFFF) {
							size = ((T8SPT*)sptCan)->size;
						}
						else if (((T8SPTOld*)sptCan)->buffer == 0xFFFFFFFFFFFFFFFF) {
							size = ((T8SPTOld*)sptCan)->size;
						}
						else {
							LOG_ERROR("Can't get size 0x{:x} for loc 0x{:x}", smagic, loc);
							loc++;
							continue;
						}

						LOG_TRACE("gsc: 0x{:x} 0x{:x} 0x{:x}: {}", smagic, loc, size, hashutils::ExtractTmpScript(obj->name));


						if (!buff.CanRead(size)) {
							loc++;
							LOG_ERROR("Bad size 0x{:x} 0x{:x} for loc 0x{:x}", smagic, size, loc);
							continue;
						}
						buff.Skip(size);

						std::filesystem::path outFile{ out / std::format("vm_{:x}/script_{:x}.gscc", smagic, obj->name) };
						std::filesystem::create_directories(outFile.parent_path());

						if (!utils::WriteFile(outFile, obj->magic, size)) {
							LOG_ERROR("Can't write {}", outFile.string());
						}
						else {
							LOG_INFO("Dump {}", outFile.string());
						}

						loc++;
					}

				}
				LOG_INFO("Done");
			}
		}


		return tool::OK;
	}

	struct {
		core::bytebuffer::ByteBuffer* buffer{};

		void CheckValid() {
			if (!buffer) throw std::runtime_error("no buffer");
		}
	} streamData;

	bool Load_Stream(bool atStreamStart, void* ptr, size_t size) {
		if (!atStreamStart || !size) return true; // no read

		streamData.CheckValid();

		byte* data{ streamData.buffer->ReadPtr<byte>(size) };

		std::memmove(ptr, data, size);

		return true;
	}
	byte* DB_AllocStreamPos(int alignment) {
		streamData.CheckValid();
		
		streamData.buffer->Align(alignment + 1);
		return streamData.buffer->Ptr();
	}

	void** DB_InsertPointer() {
		streamData.CheckValid();
		streamData.buffer->Align<void*>();
		return streamData.buffer->Ptr<void*>();
	}

	void Load_SimpleAsset_Internal(void** data, games::bo4::pool::XAssetType type) {

	}

	void EmptyFunc() {}

	int ffexe(int argc, const char* argv[]) {
		FastFileOption opt{};

		if (!opt.Compute(argv, 2, argc) || opt.m_help) {
			opt.PrintHelp();
			return opt.m_help ? tool::OK : tool::BASIC_ERROR;
		}

		static hook::module_mapper::Module game{ true };

		if (!opt.game) {
			LOG_ERROR("No game set, please use -g [exe]");
			return tool::BAD_USAGE;
		}

		if (!game && !game.Load(opt.game)) {
			LOG_ERROR("Can't map {}", opt.game);
			return tool::BASIC_ERROR;
		}
		hook::memory::RedirectJmp((*game)[0x2EBC050], Load_Stream);
		hook::memory::RedirectJmp((*game)[0x2EBBBA0], DB_AllocStreamPos);
		hook::memory::RedirectJmp((*game)[0x2EBBCC0], DB_InsertPointer);
		hook::memory::RedirectJmp((*game)[0x2EBB6F0], Load_SimpleAsset_Internal);
		hook::memory::RedirectJmp((*game)[0x2EBBFF0], EmptyFunc); // DB_ConvertOffsetToPointer
		hook::memory::RedirectJmp((*game)[0x2EBC110], EmptyFunc); // unk
		hook::memory::RedirectJmp((*game)[0x2EBBFB0], EmptyFunc); // unk
		hook::memory::RedirectJmp((*game)[0x2EBBEA0], EmptyFunc); // DB_PushStreamPos
		hook::memory::RedirectJmp((*game)[0x2EBBE20], EmptyFunc); // DB_PopStreamPos
		hook::memory::RedirectJmp((*game)[0x35BA450], EmptyFunc); // Load_GfxImageAdapter

		deps::oodle::Oodle oodle{};

		if (!oodle.LoadOodle(deps::oodle::OO2CORE_7) || !oodle) return tool::BASIC_ERROR;

		std::vector<byte> fileBuff{};
		std::vector<byte> fileFDBuff{};
		for (const char* f : opt.files) {
			if (!opt.ReadFile(f, fileBuff)) {
				LOG_ERROR("Can't read file {}", f);
				return tool::BASIC_ERROR;
			}
			std::filesystem::path fffile{ f };
			std::filesystem::path fdfile{ f };

			fdfile.replace_extension(".fd");

			LOG_INFO("Reading {}", fffile.string());

			core::bytebuffer::ByteBuffer reader{ fileBuff };

			int ret{};
			auto PrintXFile = [&ret, &reader](XFile* buffer) {
				if (*reinterpret_cast<uint64_t*>(buffer->magic) != 0x3030303066664154) {
					LOG_ERROR("Invalid FF magic: {:x}", *reinterpret_cast<uint64_t*>(buffer));
					ret = tool::BASIC_ERROR;
					return;
				}

				if (buffer->version != 0x27F) {
					LOG_ERROR("Not a T8 FF, version: 0x{:x} != 0x27F", buffer->version);
					ret = tool::BASIC_ERROR;
					return;
				}


				if (buffer->encrypted) {
					auto checksum32 = fastfiles::ComputeChecksum32(buffer->key, (unsigned int)strlen(buffer->key), 0);
					LOG_INFO("checksum32 . {}", checksum32);
				}

				if (buffer->compression >= XFILE_COMPRESSION_COUNT) {
					LOG_ERROR("Not implemented for this compression type: {}", (int)buffer->compression);
					ret = tool::BASIC_ERROR;
					return;
				}

				LOG_INFO("server ............. {}", (buffer->server ? "true" : "false"));
				LOG_INFO("encrypted .......... {}", (buffer->encrypted ? "true" : "false"));
				LOG_INFO("compress ........... {} (0x{:x})", CompressionNames[buffer->compression], (int)buffer->compression);
				LOG_INFO("platform ........... {}", (int)buffer->platform);
				LOG_INFO("builder ............ {}", buffer->builder);
				LOG_INFO("timestamp .......... {}", buffer->timestamp);

				for (size_t i = 0; i < 4; i++) {
					LOG_INFO("archiveChecksum[{}] . {:x}", i, buffer->archiveChecksum[i]);
				}

				LOG_INFO("key ................ {}", buffer->key);
				};

			LOG_INFO("size ............... {}B (0x{:x})", utils::FancyNumber(fileBuff.size()), fileBuff.size());
			XFile* buffer = reader.ReadPtr<XFile>();
			PrintXFile(buffer);
			if (ret) return ret;

			size_t idx{};

			std::vector<byte> ffdata{};
			while (true) {
				size_t loc{ reader.Loc() };

				DBStreamHeader* block{ reader.ReadPtr<DBStreamHeader>() };
				if (!block->alignedSize) break; // end

				if (block->offset != loc) {
					LOG_ERROR("bad block position: 0x{:x} != 0x{:x}", loc, block->offset);
					break;
				}


				if (!block->uncompressedSize) {
					reader.Align(0x800000);
					continue;
				}

				byte* blockBuff{ reader.ReadPtr<byte>(block->alignedSize) };


				LOG_TRACE("Decompressing block 0x{:x} (0x{:x}/0x{:x} -> 0x{:x})", loc, block->compressedSize, block->alignedSize, block->uncompressedSize);

				size_t decompressedBlock{ utils::Allocate(ffdata, block->uncompressedSize) };
				byte* decompressed{ &ffdata[decompressedBlock] };

				switch (buffer->compression) {
				case XFILE_UNCOMPRESSED:
					if (block->uncompressedSize > block->compressedSize) {
						throw std::runtime_error(std::format("Can't decompress block, decompressed size isn't big enough: 0x{:x} != 0x{:x}", block->compressedSize, block->uncompressedSize));
					}
					memcpy(decompressed, blockBuff, block->uncompressedSize);
					break;
				case XFILE_ZLIB:
				case XFILE_ZLIB_HC: {

					uLongf sizef = (uLongf)block->uncompressedSize;
					uLongf sizef2{ (uLongf)block->compressedSize };
					int ret;
					if (ret = uncompress2(decompressed, &sizef, reader.Ptr<const Bytef>(), &sizef2) < 0) {
						throw std::runtime_error(std::format("error when decompressing {}", zError(ret)));
					}
					break;
				}
				case XFILE_OODLE_KRAKEN:
				case XFILE_OODLE_MERMAID:
				case XFILE_OODLE_SELKIE:
				case XFILE_OODLE_LZNA: {
					int ret{ oodle.Decompress(blockBuff, block->compressedSize, decompressed, block->uncompressedSize, deps::oodle::OODLE_FS_YES) };

					if (ret != block->uncompressedSize) {
						throw std::runtime_error(std::format("Can't decompress block, returned size isn't the expected one: 0x{:x} != 0x{:x}", ret, block->uncompressedSize));
					}
					break;
				}
				default:
					throw std::runtime_error(std::format("No fastfile decompressor for type {}", (int)buffer->compression));
				}
			}

			LOG_INFO("Decompressed 0x{:x} byte(s)", ffdata.size());

			core::bytebuffer::ByteBuffer buff{ ffdata };

			streamData.buffer = &buff;

			// assets are aligned
			{
				/*
				 The pattern is :
				 align<8>
				 ScriptParseTree {
					u64 name;
					u64 namepad?; // maybe not there
					void* buffer = -1;
					u32 size;
					u32 unk;
				 }
				 align<32>
				 GSC_OBJ {
					u64 magic = 0x??00a0d43534780
				 }

				*/
				uint64_t magic{ 0x00a0d43534780 };
				uint64_t magicMask{ 0xFFFFFFFFFFFFF };

				size_t loc{};
				std::filesystem::path out{ "scriptparsetree_t8ff" };
				buff.Goto(0);
				byte* start{ buff.Ptr<byte>() };
				while (true) {
					loc = buff.FindMasked((byte*)&magic, (byte*)&magicMask, sizeof(magic));
					if (loc == std::string::npos) break;
					struct T8GSCOBJ {
						byte magic[8];
						int32_t crc;
						int32_t pad;
						uint64_t name;
					};

					buff.Goto(loc);

					if (!buff.CanRead(sizeof(T8GSCOBJ))) {
						break; // can't read buffer
					}

					byte* sptCan{ buff.Ptr<byte>() - 0x18 }; // 0x18 is the minimum size to lookup

					// we are 32 bytes aligned
					T8GSCOBJ* obj{ buff.ReadPtr<T8GSCOBJ>() };

					uint64_t name{ obj->name };

					while (*(uint64_t*)sptCan != name) {
						sptCan--;
						if (start == sptCan) {
							break;
						}
					}
					if (start == sptCan) {
						loc++;
						continue;
					}


					uint64_t smagic{ *reinterpret_cast<uint64_t*>(obj) };

					size_t size;
					struct T8SPT {
						uint64_t name;
						uint64_t pad0;
						uintptr_t buffer;
						uint32_t size;
						uint32_t pad02;
					};
					struct T8SPTOld {
						uint64_t name;
						uintptr_t buffer;
						uint32_t size;
						uint32_t pad02;
					};

					// Depending on how old the ff is, we might use a XHash of 0x10 or 8 bytes. The pointer
					// to the buffer will constantly be -1 because it is not linked yet
					if (((T8SPT*)sptCan)->buffer == 0xFFFFFFFFFFFFFFFF) {
						size = ((T8SPT*)sptCan)->size;
					}
					else if (((T8SPTOld*)sptCan)->buffer == 0xFFFFFFFFFFFFFFFF) {
						size = ((T8SPTOld*)sptCan)->size;
					}
					else {
						LOG_ERROR("Can't get size 0x{:x} for loc 0x{:x}", smagic, loc);
						loc++;
						continue;
					}

					LOG_TRACE("gsc: 0x{:x} 0x{:x} 0x{:x}: {}", smagic, loc, size, hashutils::ExtractTmpScript(obj->name));


					if (!buff.CanRead(size)) {
						loc++;
						LOG_ERROR("Bad size 0x{:x} 0x{:x} for loc 0x{:x}", smagic, size, loc);
						continue;
					}
					buff.Skip(size);

					std::filesystem::path outFile{ out / std::format("vm_{:x}/script_{:x}.gscc", smagic, obj->name) };
					std::filesystem::create_directories(outFile.parent_path());

					if (!utils::WriteFile(outFile, obj->magic, size)) {
						LOG_ERROR("Can't write {}", outFile.string());
					}
					else {
						LOG_INFO("Dump {}", outFile.string());
					}

					loc++;
				}

			}

			LOG_INFO("Done");
		}


		return tool::OK;
	}

	int CascTestOpt(HANDLE storage) {

		CASC_FIND_DATA data;

		auto firstFileHandle = CascFindFirstFile(storage, "*", &data, NULL);

		if (!firstFileHandle) {
			LOG_ERROR("Can't find first file : 0x{:x}", GetLastError());
			return tool::BASIC_ERROR;
		}

		do {
			if (data.bFileAvailable) {
				LOG_INFO("file: {} / {}", data.szFileName, (int)data.NameType);
			}
		} while (CascFindNextFile(firstFileHandle, &data));

		CascFindClose(firstFileHandle);

		return tool::OK;
	}

	int casctest(Process& proc, int argc, const char* argv[]) {
		if (argc < 3) {
			return tool::BAD_USAGE;
		}

		LOG_INFO("{}", argv[2]);

		HANDLE storage{};

		if (!CascOpenStorage(argv[2], 0, &storage)) {
			LOG_ERROR("Can't open local path {}", argv[2]);
			return tool::BASIC_ERROR;
		}

		auto res = CascTestOpt(storage);

		CascCloseStorage(storage);

		return res;
	}

	int h32ck(Process& proc, int argc, const char* argv[]) {
		if (argc < 3) {
			return tool::BAD_USAGE;
		}

		for (size_t i = 2; i < argc; i++) {
			auto chk = fastfiles::ComputeChecksum32(argv[i], (unsigned int)strlen(argv[i]), 0);

			LOG_INFO("{} 0x{:x}", argv[i], chk);
		}

		return tool::OK;
	}

	int bdiffinst(int argc, const char* argv[]) {
		if (tool::NotEnoughParam(argc, 2)) return tool::BAD_USAGE;

		hook::module_mapper::Module bo4{ true };

		if (!bo4.Load(argv[2])) {
			LOG_ERROR("Can't load module");
			return tool::BASIC_ERROR;
		}

		const DoubleInstruction* inst{ bo4->Get<DoubleInstruction>(0xF990E80) };


		utils::OutFileCE os{ argv[3] };

		os << std::hex << "DoubleInstruction instructions[] {\n";

		for (size_t i = 0; i < 256; i++) {
			os << "    { Instruction{ "
				<< "0x" << inst[i].instruction[0].op << ", "
				<< "0x" << inst[i].instruction[0].mode << ", "
				<< "0x" << inst[i].instruction[0].size << " }, Instruction{"
				<< "0x" << inst[i].instruction[1].op << ", "
				<< "0x" << inst[i].instruction[1].mode << ", "
				<< "0x" << inst[i].instruction[1].size << " } }, "
				"// 0x" << i << "\n";
		}
		os << "};\n";

		return tool::OK;
	}
}

ADD_TOOL(fftest, "fastfile", " [ff]", "test fastfile", fftest);
ADD_TOOL(ffexe, "fastfile", " [ff]", "test fastfile", ffexe);
ADD_TOOL(casctest, "fastfile", " [path]", "test casc", nullptr, casctest);
ADD_TOOL(h32ck, "fastfile", " [path]", "hash checksum", nullptr, h32ck);
ADD_TOOL(bdiffinst, "fastfile", " [game]", "test bdiff", bdiffinst);
