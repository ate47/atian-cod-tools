// masks, used to reduce the size of the hashes
#define HASH_MASK32 ((ulong)0xFFFFFFFFUL)
#define HASH_MASK60 ((ulong)0x0FFFFFFFFFFFFFFFUL)
#define HASH_MASK63 ((ulong)0x7FFFFFFFFFFFFFFFUL)

// FNV1A primes
#define HASH_IV_DEFAULT      ((ulong)0x100000001B3UL)
#define HASH_IV_32_DEFAULT   ((ulong)0x1000193UL)
#define HASH_IV_TYPE2        ((ulong)0x10000000233UL)
#define HASH_IV_TYPE3        ((ulong)0x100000002C1UL)

// FNV1A start offsets
#define HASH_FNV1A_PRIME            ((ulong)0xCBF29CE484222325UL)
#define HASH_FNV1A_32_PRIME         ((ulong)0x811C9DC5UL)
#define HASH_FNV1A_IW_ASSET_PRIME   ((ulong)0x47F5817A5EF961BAUL)
#define HASH_FNV1A_IW_SCR_PRIME     ((ulong)0x79D6530B0BB9B5D1UL)
#define HASH_FNV1A_IW_DVAR_OFFSET   ((ulong)0xD86A3B09566EBAACUL)
#define HASH_FNV1A_T10_OMNVAR_OFFSET ((ulong)0xCBF28CE593123345UL)
#define HASH_FNV1A_T10_SCR_OFFSET   ((ulong)0x1C2F2E3C8A257D07UL)
#define HASH_FNV1A_32_T7_PRIME      ((ulong)0x4B9ACE2FUL)

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


#define HASH_FNVA        (1UL << 0)
#define HASH_RES         (1UL << 1)
#define HASH_DVAR        (1UL << 2)
#define HASH_SCR_JUP     (1UL << 3)
#define HASH_SCR_T10     (1UL << 4)
#define HASH_SCR_T10_SP  (1UL << 5)
#define HASH_SCR_T89     (1UL << 6)
#define HASH_FNVA32      (1UL << 7)
#define HASH_PRIME       (1UL << 8)
#define MAX_WORD_LEN     256

inline long binary_search(__global const ulong* arr,
                              ulong start,
                              ulong end,
                              ulong key)
{
    long lo = (long)start;
    long hi = (long)end - 1;

    while (lo <= hi) {
        long mid = (lo + hi) >> 1;
        ulong v = arr[mid];
        if (v < key) {
            lo = mid + 1;
        } else if (v > key) {
            hi = mid - 1;
        } else {
            return mid; // found
        }
    }
    return -1; // not found
}
inline int build_combinator_word(ulong i,
                                 uint wordsCount,
                                 __global const ulong* offsets,
                                 __global const char* dictChars,
                                 __global const char* prefixStr,
                                 __global const char* middleStr,
                                 __global const char* suffixStr,
                                 __private char* outBuf,
                                 uint outBufSize)
{
    uint pos = 0;

    // 1. Prefix
    if (prefixStr != 0) {
        const __global char* p = prefixStr;
        while (*p && pos < outBufSize - 1)
            outBuf[pos++] = *p++;
    }

    // 2. Variable-length base-N combinator (1-based, CPU-compatible)
    bool first = true;
    ulong x = i;

    while (x > 0) {
        ulong adj = x - 1UL;
        uint idx = (uint)(adj % wordsCount);
        x = adj / (ulong)wordsCount;

        // Middle between parts
        if (!first && middleStr != 0) {
            const __global char* m = middleStr;
            while (*m && pos < outBufSize - 1)
                outBuf[pos++] = *m++;
        }
        first = false;

        // Append dictionary word
        ulong off = offsets[idx];
        const __global char* w = dictChars + off;
        while (*w && pos < outBufSize - 1)
            outBuf[pos++] = *w++;
    }

    // 3. Suffix
    if (suffixStr != 0) {
        const __global char* s = suffixStr;
        while (*s && pos < outBufSize - 1)
            outBuf[pos++] = *s++;
    }

    outBuf[pos] = 0;
    return (int)pos;
}

inline int check_hash(__global const ulong* header, 
                                    __global const ulong* hashes,
                                    uint indexSize,
                                    ulong hval) {
    ulong h = (hval & HASH_MASK60);
    ulong bucketIndex = h & (ulong)(indexSize - 1U);
    ulong start = header[bucketIndex * 2 + 0];
    ulong end = header[bucketIndex * 2 + 1];
    return start != end && binary_search(hashes, start, end, h) >= 0;
}

inline int hash_compute(__private const char* word,
                                    __global const ulong* mapBuf,
                                    uint indexSize,
                                    ulong algoMask) {
    __global const ulong* header = mapBuf;
    __global const ulong* hashes = mapBuf + indexSize * 2;

#define TRY_HASH(alg, hval) do { if ((algoMask & (alg)) && check_hash(header, hashes, indexSize, (ulong)(hval))) { return 1; } } while (0)

    TRY_HASH(HASH_FNVA, hash_HashX64(word, HASH_FNV1A_PRIME));
    TRY_HASH(HASH_RES, hash_HashIWAsset(word, HASH_FNV1A_IW_ASSET_PRIME));
    TRY_HASH(HASH_DVAR, hash_HashIWDVar(word, 0UL));
    TRY_HASH(HASH_SCR_JUP, hash_HashJupScr(word, HASH_FNV1A_IW_SCR_PRIME));
    TRY_HASH(HASH_SCR_T10, hash_HashT10Scr(word, 0UL));
    TRY_HASH(HASH_SCR_T10_SP, hash_HashT10ScrSP(word));
    TRY_HASH(HASH_SCR_T89, hash_HashT89Scr(word));
    TRY_HASH(HASH_FNVA32, hash_HashX32(word, HASH_FNV1A_32_PRIME));

#undef TRY_HASH
    return 0;
}

// brute force hashes using dict kernel
// 0 hashes to compute
// 1 dictionary offsets in dictData
// 2 dictionary data
// 3 prefix string
// 4 middle string
// 5 suffix string
// 6 out data
// 7 startIndex
// 8 wordsCount
// 9 indexSize
// 10 algoMask
__kernel void hash_brute(
    __global const ulong* hashMap,
    __global const ulong* dictOffsets,
    __global const char*  dictData,
    __global const char* prefixStr,
    __global const char* middleStr,
    __global const char* suffixStr,
    __global ulong*       outIndex,
    ulong                 startIndex,
    uint                  wordsCount,
    uint                  indexSize,
    ulong                 algoMask) {
    ulong gid = get_global_id(0);
    ulong i = startIndex + gid;

    if (wordsCount == 0) {
        outIndex[gid] = 0UL;
        return;
    }

    // Build combinator word into private buffer
    char buf[MAX_WORD_LEN];
    int len = build_combinator_word(
        i,
        wordsCount,
        dictOffsets,
        dictData,
        prefixStr,
        middleStr,
        suffixStr,
        buf,
        (uint)MAX_WORD_LEN
    );
    
    if (len <= 0) {
        outIndex[gid] = 0UL;
        return;
    }

    // Check all enabled hashes against map
    int matched = hash_compute(buf, hashMap, indexSize, algoMask);

    if (matched) {
        // index start at 1 because 0 = bad
        outIndex[gid] = i + 1UL;
    } else {
        outIndex[gid] = 0UL;
    }
}
