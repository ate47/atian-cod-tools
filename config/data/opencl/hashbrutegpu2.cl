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

// Core FNV1A 64-bit, generic start/iv
inline ulong hash_Hash64A_global(__global const char* str, ulong start, ulong iv) {
    ulong h = start;
    while (*str) {
        h = (h ^ (ulong)*str) * iv;
        str++;
    }
    return h;
}

inline ulong hash_Hash64A_const(__constant const char* str, ulong start, ulong iv) {
    ulong h = start;
    while (*str) {
        h = (h ^ (ulong)*str) * iv;
        str++;
    }
    return h;
}

// Secure hash helper
inline ulong hash_HashSecure(__constant const char* pattern,
                             ulong start,
                             __global const char* str,
                             ulong iv) {
    if (str == 0)
        return (ulong)0;
    if (*str == 0)
        return (ulong)0;

    ulong base = hash_Hash64A_const(pattern,
                              (start ^ (ulong)*str) * iv,
                              iv);
    return hash_Hash64A_global(str + 1, base, iv);
}


// HashX64: 64-bit FNV1A, unmasked
inline ulong hash_HashX64(__global const char* str, ulong start) {
    return hash_Hash64A_global(str, start, HASH_IV_DEFAULT);
}

inline ulong hash_EmptyPost(ulong start) {
    return start;
}

// IW asset hash
inline ulong hash_HashIWAsset(__global const char* str, ulong start) {
    return hash_Hash64A_global(str, start, HASH_IV_DEFAULT);
}

// Jup script hash
inline ulong hash_HashJupScr(__global const char* str, ulong start) {
    return hash_Hash64A_global(str, start, HASH_IV_TYPE2);
}

// IW dvar hash (secure or direct)
inline ulong hash_HashIWDVar(__global const char* str, ulong start) {
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
inline ulong hash_HashT10Scr(__global const char* str, ulong start) {
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
inline ulong hash_HashT10ScrPre(__global const char* str, ulong start) {
    return hash_Hash64A_global(str, start, HASH_IV_TYPE2);
}

inline ulong hash_HashT10ScrSPPost(ulong h) {
    return hash_Hash64A_const(XHASHSEC_T10_SCR_STR, h, HASH_IV_TYPE2);
}

inline ulong hash_HashT10ScrSP(__global const char* str) {
    return hash_HashT10ScrSPPost(hash_HashT10ScrPre(str, HASH_FNV1A_T10_SCR_OFFSET));
}

// T10 omnvar hash (secure or direct)
inline ulong hash_HashT10OmnVar(__global const char* str, ulong start) {
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
#define HASH_OMNVAR      (1UL << 9)
#define HASH_DJB2        (1UL << 10)
#define HASH_T7          (1UL << 11)
#define HASH_KVP         (1UL << 12)
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
inline long linear_search(__global const ulong* arr,
                              ulong start,
                              ulong end,
                              ulong key)
{
    while (start != end) {
        if (arr[start++] == key) {
            return start - 1; // found
        }
    }
    return -1; // not found
}

#define BUILD_COMBINATOR_HASH(alg, funcPost, i, wordsCount, offsets, dictChars, start, middleStr, suffixStr) \
do {                                                                                   \
    uint pos = 0;                                                                      \
    bool first = true;                                                                 \
    ulong x = i;                                                                       \
                                                                                       \
    while (x > 0) {                                                                    \
        ulong adj = x - 1UL;                                                           \
        uint idx = (uint)(adj % wordsCount);                                           \
        x = adj / (ulong)wordsCount;                                                   \
                                                                                       \
        /* Middle between parts */                                                     \
        if (!first && middleStr) {                                                     \
            start = alg(middleStr, start);                                             \
        }                                                                              \
        first = false;                                                                 \
        start = alg(dictChars + offsets[idx], start);                                  \
    }                                                                                  \
    if (suffixStr) {                                                                   \
        start = alg(suffixStr, start);                                                 \
    }                                                                                  \
    start = funcPost(start);                                                           \
} while (0)


inline int check_hash(__global const ulong* mapBuf,
                                    uint indexSize,
                                    ulong hval) {
   //ulong bidx = hval & 0x3ffff; // bitmap index
   //ulong bword = bidx >> 6;
   //ulong bbit = bidx & 63;

    __global const ulong* header = mapBuf;
    __global const ulong* hashes = mapBuf + indexSize * 2;
    ulong bucketIndex = hval & (ulong)(indexSize - 1U);
    ulong start = header[bucketIndex * 2 + 0];
    ulong end = header[bucketIndex * 2 + 1];
    ulong count = end - start;
    if (!count) {
        return 0;
    }
    ulong h = (hval & HASH_MASK60);
    // I vaguely remember a paper saying that the binary search is more expensive 
    // for small arrays
    if (count > 0x1000) {
        return binary_search(hashes, start, end, h) >= 0;
    } else {
        return linear_search(hashes, start, end, h) >= 0;
    }
}

// brute force hashes using dict kernel
// 0 hashes to compute
// 1 dictionary offsets in dictData
// 2 dictionary data
// 3 start offset (computed from prefix)
// 4 middle string
// 5 suffix string
// 6 out data
// 7 startIndex
// 8 wordsCount
// 9 indexSize
#define CREATE_HASH_KERNEL(kernelName, func, funcPost)                                               \
__kernel void hash_brute_dict_##kernelName(                                                          \
    __global const ulong* hashMap,                                                                   \
    __global const ulong* dictOffsets,                                                               \
    __global const char*  dictData,                                                                  \
    ulong startVal,                                                                                  \
    __global const char* mid,                                                                        \
    __global const char* suf,                                                                        \
    __global ulong*       outIndex,                                                                  \
    ulong                 startIndex,                                                                \
    uint                  wordsCount,                                                                \
    uint                  indexSize) {                                                               \
    ulong i = startIndex + get_global_id(0);                                                         \
    BUILD_COMBINATOR_HASH(func, funcPost, i, wordsCount, dictOffsets, dictData, startVal, mid, suf); \
    /* Check all enabled hashes against map*/                                                        \
    if (check_hash(hashMap, indexSize, startVal)) {                                                  \
        __global uint *count = (__global uint*)outIndex;                                             \
                                                                                                     \
        outIndex[1 + atomic_inc(count)] = i;                                                         \
    }                                                                                                \
}                                                                                                    \

#define CREATE_PREHASH_KERNEL(kernelName, func)                                                      \
__kernel void hash_pre_##kernelName(                                                                 \
    __global ulong* outres,                                                                          \
    __global const ulong* dictOffsets,                                                               \
    __global const char*  dictData,                                                                  \
    ulong startVal                                                                                   \
) {                                                                                                  \
    size_t idx = get_global_id(0);                                                                   \
    outres[idx] = func(dictData + dictOffsets[idx], startVal);                                       \
}

#define CREATE_ALG_KERNEL(kernelName, func, funcPost) \
CREATE_PREHASH_KERNEL(kernelName, func) \
CREATE_HASH_KERNEL(kernelName, func, funcPost)

// one kernel per algorithm
// hash_pre_##alg -> pre hash
// hash_brute_dict_##alg -> hash brute
CREATE_ALG_KERNEL(x64, hash_HashX64, hash_EmptyPost)
CREATE_ALG_KERNEL(iw, hash_HashIWAsset, hash_EmptyPost)
CREATE_ALG_KERNEL(jup, hash_HashJupScr, hash_EmptyPost)
CREATE_ALG_KERNEL(dvar, hash_HashIWDVar, hash_EmptyPost)
CREATE_ALG_KERNEL(t10scr, hash_HashT10Scr, hash_EmptyPost)
CREATE_ALG_KERNEL(t10scrsp, hash_HashT10ScrPre, hash_HashT10ScrSPPost)
CREATE_ALG_KERNEL(t10omn, hash_HashT10OmnVar, hash_EmptyPost)
