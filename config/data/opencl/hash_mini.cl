// hash_mini.cl
// MIT License
//
// Copyright (c) 2025 Antoine Willerval
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// masks, used to reduce the size of the hashes
#define HASH_MASK32 ((ulong)0x00000000FFFFFFFFUL)
#define HASH_MASK60 ((ulong)0x0FFFFFFFFFFFFFFFUL)
#define HASH_MASK63 ((ulong)0x7FFFFFFFFFFFFFFFUL)

// FNV1A primes
#define HASH_IV_DEFAULT      ((ulong)0x0000000100000001B3UL)
#define HASH_IV_32_DEFAULT   ((ulong)0x0000000001000193UL)
#define HASH_IV_TYPE2        ((ulong)0x000000010000000233UL)
#define HASH_IV_TYPE3        ((ulong)0x0000000100000002C1UL)

// FNV1A start offsets
#define HASH_FNV1A_PRIME            ((ulong)0xCBF29CE484222325UL)
#define HASH_FNV1A_32_PRIME         ((ulong)0x00000000811C9DC5UL)
#define HASH_FNV1A_IW_ASSET_PRIME   ((ulong)0x47F5817A5EF961BAUL)
#define HASH_FNV1A_IW_SCR_PRIME     ((ulong)0x79D6530B0BB9B5D1UL)
#define HASH_FNV1A_IW_DVAR_OFFSET   ((ulong)0xD86A3B09566EBAACUL)
#define HASH_FNV1A_T10_OMNVAR_OFFSET ((ulong)0xCBF28CE593123345UL)
#define HASH_FNV1A_T10_SCR_OFFSET   ((ulong)0x1C2F2E3C8A257D07UL)
#define HASH_FNV1A_32_T7_PRIME      ((ulong)0x000000004B9ACE2FUL)

// Secure strings (stored in constant memory)
__constant char XHASHSEC_DVAR_STR[] = "q6n-+7=tyytg94_*";
__constant char XHASHSEC_T10_SCR_STR[] = "zt@f3yp(d[kkd=_@";
__constant char XHASHSEC_T10_OMNVAR_STR[] = "gvbs9*vpm@mh@krh";

// lowerc: to-lower + path separator normalization
inline char hash_lowerc(char c) {
    if (c >= 'A' && c <= 'Z') {
        return (char)('a' + (c - 'A'));
    } else if (c == '\\') {
        return '/';
    } else {
        return c;
    }
}

// Core FNV1A 64-bit, generic start/iv
inline ulong hash_Hash64A_global(__private const char* str, ulong start, ulong iv) {
    ulong h = start;
    while (*str) {
        h = (h ^ (ulong)hash_lowerc(*str)) * iv;
        str++;
    }
    return h;
}

inline ulong hash_Hash64A_const(__constant const char* str, ulong start, ulong iv) {
    ulong h = start;
    while (*str) {
        h = (h ^ (ulong)hash_lowerc(*str)) * iv;
        str++;
    }
    return h;
}

// T89 script pre-hash (32-bit)
inline uint hash_HashT89ScrPre(__private const char* str, uint start) {
    uint hash = start;
    const __private char* data = str;
    while (*data) {
        char c = hash_lowerc(*data);
        uint t = (uint)(c + hash);
        uint v = t ^ (t << 10);
        hash = v + (v >> 6);
        data++;
    }
    return hash;
}

// T89 script post-hash (32-bit)
inline uint hash_HashT89ScrPost(uint hash) {
    uint v = (uint)(9U * hash);
    return (uint)(0x8001U * (v ^ (v >> 11)));
}

// Masked 63-bit FNV1A
inline ulong hash_Hash64(__private const char* str, ulong start, ulong iv) {
    return hash_Hash64A_global(str, start, iv) & HASH_MASK63;
}

// Secure hash helper
inline ulong hash_HashSecure(__constant const char* pattern,
                             ulong start,
                             __private const char* str,
                             ulong iv) {
    if (str == 0)
        return (ulong)0;
    if (*str == 0)
        return (ulong)0;

    ulong base = hash_Hash64A_const(pattern,
                              (start ^ (ulong)hash_lowerc(*str)) * iv,
                              iv);
    return hash_Hash64A_global(str + 1, base, iv);
}

// Simple prime-based hash (like djb2 variant)
inline uint hash_HashPrime(__private const char* str, uint start, uint prime) {
    uint h = start;
    const __private char* s = str;
    while (*s) {
        h = prime * h + (uint)hash_lowerc(*s);
        s++;
    }
    return h;
}

// HashX64: 64-bit FNV1A, unmasked
inline ulong hash_HashX64(__private const char* str, ulong start) {
    return hash_Hash64A_global(str, start, HASH_IV_DEFAULT);
}

// HashX63: 63-bit masked
inline ulong hash_HashX63(__private const char* str, ulong start) {
    return hash_Hash64(str, start, HASH_IV_DEFAULT);
}

// HashX32: 32-bit masked from 64-bit FNV1A with 32-bit IV
inline ulong hash_HashX32(__private const char* str, ulong start) {
    return hash_Hash64(str, start, HASH_IV_32_DEFAULT) & HASH_MASK32;
}

// T7 hash
inline uint hash_HashT7(__private const char* str) {
    ulong v = hash_Hash64A_global(str, HASH_FNV1A_32_T7_PRIME, HASH_IV_32_DEFAULT);
    v &= HASH_MASK32;
    v *= HASH_IV_32_DEFAULT;
    return (uint)v;
}

// T89 script hash (pre + post)
inline uint hash_HashT89Scr(__private const char* str) {
    uint pre = hash_HashT89ScrPre(str, (uint)HASH_FNV1A_32_T7_PRIME);
    return hash_HashT89ScrPost(pre);
}

// IW asset hash
inline ulong hash_HashIWAsset(__private const char* str, ulong start) {
    return hash_Hash64(str, start, HASH_IV_DEFAULT);
}

// Jup script hash
inline ulong hash_HashJupScr(__private const char* str, ulong start) {
    return hash_Hash64A_global(str, start, HASH_IV_TYPE2);
}

// IW dvar hash (secure or direct)
inline ulong hash_HashIWDVar(__private const char* str, ulong start) {
    if (start == 0UL) {
        return hash_HashSecure(XHASHSEC_DVAR_STR,
                               HASH_FNV1A_IW_DVAR_OFFSET,
                               str,
                               HASH_IV_TYPE2);
    } else {
        return hash_Hash64A_global(str, start, HASH_IV_TYPE2);
    }
}

// T10 script hash (secure or direct)
inline ulong hash_HashT10Scr(__private const char* str, ulong start) {
    if (start == 0UL) {
        return hash_HashSecure(XHASHSEC_T10_SCR_STR,
                               HASH_FNV1A_T10_SCR_OFFSET,
                               str,
                               HASH_IV_TYPE2);
    } else {
        return hash_Hash64A_global(str, start, HASH_IV_TYPE2);
    }
}

// T10 script SP pre/post
inline ulong hash_HashT10ScrSPPre(__private const char* str) {
    return hash_Hash64A_global(str, HASH_FNV1A_T10_SCR_OFFSET, HASH_IV_TYPE2);
}

inline ulong hash_HashT10ScrSPPost(ulong h) {
    return hash_Hash64A_const(XHASHSEC_T10_SCR_STR, h, HASH_IV_TYPE2);
}

inline ulong hash_HashT10ScrSP(__private const char* str) {
    return hash_HashT10ScrSPPost(hash_HashT10ScrSPPre(str));
}

// T10 omnvar hash (secure or direct)
inline ulong hash_HashT10OmnVar(__private const char* str, ulong start) {
    if (start == 0UL) {
        return hash_HashSecure(XHASHSEC_T10_OMNVAR_STR,
                               HASH_FNV1A_T10_OMNVAR_OFFSET,
                               str,
                               HASH_IV_TYPE3);
    } else {
        return hash_Hash64A_global(str, start, HASH_IV_TYPE3);
    }
}

