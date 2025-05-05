#pragma once

/*
MIT License

Copyright (c) 2025 Antoine Willerval

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

namespace hash {
	// masks, used to reduce the size of the hashes
	constexpr uint64_t MASK32 = 0xFFFFFFFF;
	constexpr uint64_t MASK60 = 0xFFFFFFFFFFFFFFFull;
	constexpr uint64_t MASK63 = 0x7FFFFFFFFFFFFFFFull;

	// FNV1A primes
	constexpr uint64_t IV_DEFAULT = 0x100000001b3;
	constexpr uint64_t IV_32_DEFAULT = 0x1000193;
	constexpr uint64_t IV_TYPE2 = 0x10000000233;
	constexpr uint64_t IV_TYPE3 = 0x100000002C1;

	// FNV1A start offsets
	constexpr uint64_t FNV1A_PRIME = 0xcbf29ce484222325LL;
	constexpr uint64_t FNV1A_32_PRIME = 0x811C9DC5;
	constexpr uint64_t FNV1A_IW_ASSET_PRIME = 0x47F5817A5EF961BA;
	constexpr uint64_t FNV1A_IW_SCR_PRIME = 0x79D6530B0BB9B5D1;
	constexpr uint64_t FNV1A_IW_DVAR_OFFSET = 0xD86A3B09566EBAAC;
	constexpr uint64_t FNV1A_T10_OMNVAR_OFFSET = 0xCBF28CE593123345;
	constexpr uint64_t FNV1A_T10_SCR_OFFSET = 0x1C2F2E3C8A257D07;
	constexpr uint64_t FNV1A_32_T7_PRIME = 0x4B9ACE2F;

	// Secure strings
	constexpr const char* XHASHSEC_DVAR_STR = "q6n-+7=tyytg94_*";
	constexpr const char* XHASHSEC_T10_SCR_STR = "zt@f3yp(d[kkd=_@";
	constexpr const char* XHASHSEC_T10_OMNVAR_STR = "gvbs9*vpm@mh@krh";

	constexpr char lowerc(char c) {
		if (c >= 'A' && c <= 'Z') {
			// to lower
			return 'a' + (c - 'A');
		}
		else if (c == '\\') {
			// replace path char
			return '/';
		}
		else {
			return c;
		}
	}

	constexpr uint64_t Hash64A(const char* str, uint64_t start = FNV1A_PRIME, uint64_t iv = IV_DEFAULT) {
		uint64_t hash = start;

		for (const char* data = str; *data; data++) {
			hash = (hash ^ lowerc(*data)) * iv;
		}

		return hash;
	}
	constexpr uint32_t HashT89ScrPre(const char* str, uint32_t start = 0x4B9ACE2F) {
		uint32_t hash{ start };

		for (const char* data = str; *data; data++) {
			char c = lowerc(*data);
			hash = ((c + hash) ^ ((c + hash) << 10)) + (((c + hash) ^ ((c + hash) << 10)) >> 6);
		}

		return hash;
	}

	constexpr uint32_t HashT89ScrPost(uint32_t hash) {
		return 0x8001 * ((9 * hash) ^ ((9 * hash) >> 11));
	}

	constexpr uint64_t Hash64(const char* str, uint64_t start = FNV1A_PRIME, uint64_t iv = IV_DEFAULT) {
		return Hash64A(str, start, iv) & MASK63;
	}

	constexpr uint64_t HashT10ScrSPPre(const char* str, uint64_t start = FNV1A_T10_SCR_OFFSET) { return Hash64A(str, start, IV_TYPE2); }
	constexpr uint64_t HashT10ScrSPPost(uint64_t hash) { return Hash64A(XHASHSEC_T10_SCR_STR, hash, IV_TYPE2); }
	constexpr uint64_t HashSecure(const char* pattern, uint64_t start, const char* str, uint64_t iv) {
		if (!str || !*str) {
			return 0;
		}

		uint64_t base = Hash64A(pattern, (start ^ *str) * iv, iv);
		return Hash64A(str + 1, base, iv);
	}

	// Hash algorithms

	constexpr uint64_t HashX64(const char* str, uint64_t start = FNV1A_PRIME) { return Hash64A(str, start); }
	constexpr uint64_t HashX63(const char* str, uint64_t start = FNV1A_PRIME) { return Hash64(str, start); }
	constexpr uint64_t HashX32(const char* str, uint64_t start = FNV1A_32_PRIME) { return Hash64(str, start, IV_32_DEFAULT) & MASK32; }
	constexpr uint32_t HashT7(const char* str) { return (uint32_t)(Hash64A(str, FNV1A_32_T7_PRIME, IV_32_DEFAULT) & MASK32) * IV_32_DEFAULT; }
	constexpr uint32_t HashT89Scr(const char* str) { return HashT89ScrPost(HashT89ScrPre(str)); }
	constexpr uint64_t HashIWAsset(const char* str, uint64_t start = FNV1A_IW_ASSET_PRIME) { return Hash64(str, start); }
	constexpr uint64_t HashJupScr(const char* str, uint64_t start = FNV1A_IW_SCR_PRIME) { return Hash64A(str, start, IV_TYPE2); }
	constexpr uint64_t HashIWDVar(const char* str, uint64_t start = 0) { return !start ? HashSecure(XHASHSEC_DVAR_STR, FNV1A_IW_DVAR_OFFSET, str, IV_TYPE2) : Hash64A(str, start, IV_TYPE2); }
	constexpr uint64_t HashT10Scr(const char* str, uint64_t start = 0) { return !start ? HashSecure(XHASHSEC_T10_SCR_STR, FNV1A_T10_SCR_OFFSET, str, IV_TYPE2) : Hash64A(str, start, IV_TYPE2); }
	constexpr uint64_t HashT10ScrSP(const char* str) { return HashT10ScrSPPost(HashT10ScrSPPre(str)); }
	constexpr uint64_t HashT10OmnVar(const char* str, uint64_t start = 0) { return !start ? HashSecure(XHASHSEC_T10_OMNVAR_STR, FNV1A_T10_OMNVAR_OFFSET, str, IV_TYPE3) : Hash64A(str, start, IV_TYPE3); }
}
