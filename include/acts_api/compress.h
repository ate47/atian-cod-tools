#ifndef __ACTS_API_COMPRESS_H__
#define __ACTS_API_COMPRESS_H__
#include "api.h"
/*
 * Atian Tools Compress API
 */

 /* compression type */
typedef enum {
    COMP_INVALID = 0
} ActsAPICompress_Algorithm;

/* compression type config */
#define ACTS_API_COMPRESS_CONFIG_NONE "none"
#define ACTS_API_COMPRESS_CONFIG_ZLIB "zlib"
#define ACTS_API_COMPRESS_CONFIG_ZLIB_HC "zlib_hc"
#define ACTS_API_COMPRESS_CONFIG_LZMA "lzma"
#define ACTS_API_COMPRESS_CONFIG_LZMA_HC "lzma_hc"
#define ACTS_API_COMPRESS_CONFIG_LZ4 "lz4"
#define ACTS_API_COMPRESS_CONFIG_LZ4_HC "lz4_hc"
#define ACTS_API_COMPRESS_CONFIG_ZSTD "zstd"
#define ACTS_API_COMPRESS_CONFIG_ZSTD_HC "zstd_hc"
#define ACTS_API_COMPRESS_CONFIG_OODLE "oodle"
#define ACTS_API_COMPRESS_CONFIG_OODLE_HC "oodle_hc"
#define ACTS_API_COMPRESS_CONFIG_OODLE_KRAKEN "oodle_kraken"
#define ACTS_API_COMPRESS_CONFIG_OODLE_KRAKEN_HC "oodle_kraken_hc"
#define ACTS_API_COMPRESS_CONFIG_OODLE_LEVIATHAN "oodle_leviathan"
#define ACTS_API_COMPRESS_CONFIG_OODLE_LEVIATHAN_HC "oodle_leviathan_hc"
#define ACTS_API_COMPRESS_CONFIG_OODLE_MERMAID "oodle_mermaid"
#define ACTS_API_COMPRESS_CONFIG_OODLE_MERMAID_HC "oodle_mermaid_hc"
#define ACTS_API_COMPRESS_CONFIG_OODLE_SELKIE "oodle_selkie"
#define ACTS_API_COMPRESS_CONFIG_OODLE_SELKIE_HC "oodle_selkie_hc"
#define ACTS_API_COMPRESS_CONFIG_OODLE_HYDRA "oodle_hydra"
#define ACTS_API_COMPRESS_CONFIG_OODLE_HYDRA_HC "oodle_hydra_hc"

/*
 * Get the compression algorithm
 * @param cfg algorithm string
 * @return algorithm configuration
 */
ACTS_COMMON_API ActsAPICompress_Algorithm ActsAPICompress_GetAlgorithm(const char* cfg);
/*
 * Get the compression size to create the compressed destination buffer for ActsAPICompress_Compress
 * @param alg compression algorithm
 * @param srcSize source size
 * @param outSize output size
 * @return ACTS_STATUS_OK if the size can be get, ACTS_STATUS_ERROR otherwise with ActsGetAPILastMessage set
 */
ACTS_COMMON_API ActsStatus ActsAPICompress_GetCompressSize(ActsAPICompress_Algorithm alg, size_t srcSize, size_t* outSize);

/*
 * Compress a buffer
 * @param alg compression algorithm
 * @param dest destination data
 * @param destSize destination data, will be replaced by the new size, should be at least ActsAPICompress_GetCompressSize for a safe usage
 * @param src source data
 * @param srcSize source size
 * @return ACTS_STATUS_OK if the buffer is compressed, ACTS_STATUS_ERROR otherwise with ActsGetAPILastMessage set
 */
ACTS_COMMON_API ActsStatus ActsAPICompress_Compress(ActsAPICompress_Algorithm alg, void* dest, size_t* destSize, const void* src, size_t srcSize);
/*
 * Decompress a buffer
 * @param alg compression algorithm
 * @param dest destination data
 * @param destSize destination data, will be replaced by the new size
 * @param src source data
 * @param srcSize source size
 * @return ACTS_STATUS_OK if the buffer is decompressed, ACTS_STATUS_ERROR otherwise with ActsGetAPILastMessage set
 */
ACTS_COMMON_API ActsStatus ActsAPICompress_Decompress(ActsAPICompress_Algorithm alg, void* dest, size_t* destSize, const void* src, size_t srcSize);


#endif // __ACTS_API_COMPRESS_H__