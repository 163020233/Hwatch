/**
 * @file zlix_utils.c
 * @copyright Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * @brief \n
 * 提供基础操作接口, 包括crc checksum等
 * @author Anonym
 * @date 2021-10-22
 * @version v0.1.0
 * *******************************************************************************************
 * @par 修改日志：
 * <table>
 * <tr><th>Date        <th>Version  <th>Author    <th>Description
 * <tr><td>2021-10-22  <td>0.1.0    <td>Anonym    <td>提供基础操作接口
 * </table>
 *
 * *******************************************************************************************
 */

#include "drb_zlix.h"
#include "zlix_public_inner.h"

#ifndef SINGLE_THREAD
#include <sched.h>
#endif

/* CRC32多项式公式： x^32 + x^26 + x^22 + x^16 + x^12 + x^11 + x^10
   + x^8 + x^7 + x^6 + x^5 + x^4 + x^2 + x^1 + x^0; 去掉x^32，剩余
   26 22 16 12 11 10 8 7 6 5 4 2 1 0 bit位，zlib按照从左往右看bit,
   所以相当于0x10000000右移上述bit位然后相加，得出多项式的值 */
#define ZLIX_UTILS_CRC_POLY32  0xedb88320

#define ZLIX_UTILS_CRC_TABLE_SIZE 256

/* 根据选择是否需要高内存消耗以获取高性能，
   此处为crc计算是否需要进行数据的提前保存，来进行多字节运算以提高计算速度 */
#ifndef ZLIX_LOW_COST_BUT_SLOWER
#define ZLIX_UTILS_CRC_BYTES_NUM 8
#else
#define ZLIX_UTILS_CRC_BYTES_NUM 1
#endif

#define ZLIX_UTILS_CRC_ONE_BYTE_BITS 8

#define ZLIX_UTILS_CRC_ONE_BYTE_MASK 0xff

#define ZLIX_UTILS_CRC_SHIFT_BIT8 8            /* 8bit位移 */

#ifndef ZLIX_LOW_COST_BUT_SLOWER
#define ZLIX_UTILS_CRC_SHIFT_BIT16 16          /* 16bit位移 */

#define ZLIX_UTILS_CRC_SHIFT_BIT24 24          /* 24bit位移 */

#define ZLIX_UTILS_CRC_SLICE_ALIGN 4

#define ZLIX_UTILS_CRC_BYTES_ONCE_ROLL 32

#define ZLIX_UTILS_CRC_SLICE_ROLL 4

#define ZLIX_CAL_CRC_BIG_ENDIAN(ZlixUtilsCrc8Table32, word1, word2) (   \
    (ZlixUtilsCrc8Table32)[0][(word2) & ZLIX_UTILS_CRC_ONE_BYTE_MASK] ^                                   \
    (ZlixUtilsCrc8Table32)[1][((word2) >> ZLIX_UTILS_CRC_SHIFT_BIT8) & ZLIX_UTILS_CRC_ONE_BYTE_MASK] ^    \
    (ZlixUtilsCrc8Table32)[2][((word2) >> ZLIX_UTILS_CRC_SHIFT_BIT16) & ZLIX_UTILS_CRC_ONE_BYTE_MASK] ^   \
    (ZlixUtilsCrc8Table32)[3][((word2) >> ZLIX_UTILS_CRC_SHIFT_BIT24) & ZLIX_UTILS_CRC_ONE_BYTE_MASK] ^   \
    (ZlixUtilsCrc8Table32)[4][(word1) & ZLIX_UTILS_CRC_ONE_BYTE_MASK] ^                                   \
    (ZlixUtilsCrc8Table32)[5][((word1) >> ZLIX_UTILS_CRC_SHIFT_BIT8) & ZLIX_UTILS_CRC_ONE_BYTE_MASK] ^    \
    (ZlixUtilsCrc8Table32)[6][((word1) >> ZLIX_UTILS_CRC_SHIFT_BIT16) & ZLIX_UTILS_CRC_ONE_BYTE_MASK] ^   \
    (ZlixUtilsCrc8Table32)[7][((word1) >> ZLIX_UTILS_CRC_SHIFT_BIT24) & ZLIX_UTILS_CRC_ONE_BYTE_MASK]     \
)

#define ZLIX_CAL_CRC_SMALL_ENDIAN(ZlixUtilsCrc8Table32, word1, word2) (   \
    (ZlixUtilsCrc8Table32)[0][((word2) >> ZLIX_UTILS_CRC_SHIFT_BIT24) & ZLIX_UTILS_CRC_ONE_BYTE_MASK] ^     \
    (ZlixUtilsCrc8Table32)[1][((word2) >> ZLIX_UTILS_CRC_SHIFT_BIT16) & ZLIX_UTILS_CRC_ONE_BYTE_MASK] ^     \
    (ZlixUtilsCrc8Table32)[2][((word2) >> ZLIX_UTILS_CRC_SHIFT_BIT8) & ZLIX_UTILS_CRC_ONE_BYTE_MASK] ^      \
    (ZlixUtilsCrc8Table32)[3][(word2) & ZLIX_UTILS_CRC_ONE_BYTE_MASK] ^                                     \
    (ZlixUtilsCrc8Table32)[4][((word1) >> ZLIX_UTILS_CRC_SHIFT_BIT24)  & ZLIX_UTILS_CRC_ONE_BYTE_MASK] ^    \
    (ZlixUtilsCrc8Table32)[5][((word1) >> ZLIX_UTILS_CRC_SHIFT_BIT16) & ZLIX_UTILS_CRC_ONE_BYTE_MASK] ^     \
    (ZlixUtilsCrc8Table32)[6][((word1) >> ZLIX_UTILS_CRC_SHIFT_BIT8) & ZLIX_UTILS_CRC_ONE_BYTE_MASK] ^      \
    (ZlixUtilsCrc8Table32)[7][(word1) & ZLIX_UTILS_CRC_ONE_BYTE_MASK]                                      \
)
#endif

typedef enum {
    ZLIX_UTILS_CRC_UNINIT,
    ZLIX_UTILS_CRC_ININIT,
    ZLIX_UTILS_CRC_ININTED,
    ZLIX_UTILS_CRC_BUTT,
} ZlixUtilsCrcInitState;

static uint32_t g_ZlixUtilsCrcTable32[ZLIX_UTILS_CRC_BYTES_NUM][ZLIX_UTILS_CRC_TABLE_SIZE];

ZLIX_STATIC uint32_t ZlixUtilsCrcTableValue(uint32_t index, uint32_t topBit)
{
    uint32_t temp;
    uint32_t value;

    value = index;
    /* 算出每个字节对应的模数 */
    for (temp = 0; temp < ZLIX_UTILS_CRC_ONE_BYTE_BITS; temp++) {
        if ((value & topBit) == 1) {
            value = (value >> 1) ^ ZLIX_UTILS_CRC_POLY32;
        } else {
            value = value >> 1;
        }
    }

    return value;
}

ZLIX_STATIC void ZlixUtilsCrc32Init(void)
{
    uint32_t index1;
    uint32_t index2;
    uint32_t value;
    (void)index2;

    for (index1 = 0; index1 < ZLIX_UTILS_CRC_TABLE_SIZE; index1++) {
        value = ZlixUtilsCrcTableValue(index1, 1);
        g_ZlixUtilsCrcTable32[0][index1] = value;
    }

#ifndef ZLIX_LOW_COST_BUT_SLOWER
    for (index1 = 0; index1 < ZLIX_UTILS_CRC_TABLE_SIZE; index1++) {
        value = g_ZlixUtilsCrcTable32[0][index1];

        for (index2 = 1; index2 < ZLIX_UTILS_CRC_BYTES_NUM; index2++) {
            value = g_ZlixUtilsCrcTable32[0][value & ZLIX_UTILS_CRC_ONE_BYTE_MASK] ^
                (value >> ZLIX_UTILS_CRC_SHIFT_BIT8);
            g_ZlixUtilsCrcTable32[index2][index1] = value;
        }
    }
#endif
}

ZLIX_STATIC void ZlixUtilsCrc32InitOnce(void)
{
#ifdef SINGLE_THREAD
    static int32_t state = ZLIX_UTILS_CRC_UNINIT;

    if (state == ZLIX_UTILS_CRC_UNINIT) {
        ZlixUtilsCrc32Init();
        state = ZLIX_UTILS_CRC_ININIT;
    }
#else
    int32_t oldState;
    static int32_t state = ZLIX_UTILS_CRC_UNINIT;

    do {
        oldState = __sync_fetch_and_add(&state, 0);
        if (oldState == ZLIX_UTILS_CRC_ININTED) {
            return;
        }

        if (oldState == ZLIX_UTILS_CRC_ININIT) {
            continue;
        }
    } while (!__sync_bool_compare_and_swap(&state, oldState, ZLIX_UTILS_CRC_ININIT));

    ZlixUtilsCrc32Init();

    /* 由于上述逻辑已经防护，此处不存在交换不成功的场景 */
    (void)__sync_bool_compare_and_swap(&state, ZLIX_UTILS_CRC_ININIT, ZLIX_UTILS_CRC_ININTED);
#endif
}

#ifndef ZLIX_LOW_COST_BUT_SLOWER
ZLIX_STATIC ZLIX_ALWAYS_INLINE uint32_t ZlixUtilsSwap32(uint32_t val)
{
#if (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 2)))
    return __builtin_bswap32(val);
#elif defined(_MSC_VER)
    return _byteswap_ulong(val);
#else
    return ((0xff000000U & (val << 24U)) | /* 左移24位和右移24位值互换，0xff000000表示24~31的位置 */
            (0x000000ffU & (val >> 24U)) | /* 0x000000ff表示0~7的位置，24~31位右移24位放到0~7位的位置 */
            (0x00ff0000U & (val <<  8U)) | /* 高8位和低8位值互换, 0x00ff0000表示15~24的位置 */
            (0x0000ff00U & (val >>  8U))); /* 0x0000ff00表示8~15的位置，高8位右移8位放到低8位的位置 */
#endif
}

ZLIX_STATIC void ZlixUtilsCrc32SliceBigEndian(const uint8_t **buf, size_t *length, uint32_t *crcVal)
{
    size_t len = *length;
    uint32_t word1, word2;
    const uint32_t *tmpBuf = (const uint32_t *)(uintptr_t)(*buf);
    uint32_t crc = *crcVal;
    uint32_t index;

    do {
        for (index = 0; index < ZLIX_UTILS_CRC_SLICE_ROLL; index++) {
            word1 = ZlixUtilsSwap32(crc) ^ *tmpBuf++;
            word2 = *tmpBuf++;

            crc = ZLIX_CAL_CRC_BIG_ENDIAN(g_ZlixUtilsCrcTable32, word1, word2);
        }
        len -= ZLIX_UTILS_CRC_BYTES_ONCE_ROLL;
    } while (len >= ZLIX_UTILS_CRC_BYTES_ONCE_ROLL);

    *crcVal = crc;
    *length = len;
    *buf = (const uint8_t *)tmpBuf;
}

ZLIX_STATIC void ZlixUtilsCrc32SliceLittleEndian(const uint8_t **buf, size_t *length, uint32_t *crcVal)
{
    size_t len = *length;
    uint32_t word1, word2;
    const uint32_t *tmpBuf = (const uint32_t *)(uintptr_t)(*buf);
    uint32_t crc = *crcVal;
    uint32_t index;

    do {
        for (index = 0; index < ZLIX_UTILS_CRC_SLICE_ROLL; index++) {
            word1 = crc ^ *tmpBuf++;
            word2 = *tmpBuf++;

            crc = ZLIX_CAL_CRC_SMALL_ENDIAN(g_ZlixUtilsCrcTable32, word1, word2);
        }
        len -= ZLIX_UTILS_CRC_BYTES_ONCE_ROLL;
    } while (len >= ZLIX_UTILS_CRC_BYTES_ONCE_ROLL);

    *crcVal = crc;
    *length = len;
    *buf = (const uint8_t *)tmpBuf;
}
#endif

/* CRC计算 */
int32_t ZlixUtilsCrc32Cal(const uint8_t *buf, size_t length, uint32_t *crcVal)
{
    size_t len;
    uint32_t crc;
    const uint8_t *tmpBuf;
#ifndef ZLIX_LOW_COST_BUT_SLOWER
    size_t align;
    uint32_t endian;
#endif

    if ((buf == NULL) || (crcVal == NULL)) {
        return ZLIX_INPUT_PARA_INVALID;
    }

    /* CRC表初始化, 只初始化一次，且内部已经考虑并发 */
    ZlixUtilsCrc32InitOnce();

    len = length;
    crc = ~(*crcVal);
    tmpBuf = buf;

#ifndef ZLIX_LOW_COST_BUT_SLOWER
    align = (ZLIX_UTILS_CRC_SLICE_ALIGN - (((uintptr_t)tmpBuf) & (ZLIX_UTILS_CRC_SLICE_ALIGN - 1))) &
        (ZLIX_UTILS_CRC_SLICE_ALIGN - 1);
    while ((len != 0) && (align != 0)) {
        crc = (crc >> ZLIX_UTILS_CRC_SHIFT_BIT8) ^
            g_ZlixUtilsCrcTable32[0][(crc & ZLIX_UTILS_CRC_ONE_BYTE_MASK) ^ *tmpBuf++];
        len--;
        align--;
    }
    endian = 1;
    if (len >= ZLIX_UTILS_CRC_BYTES_ONCE_ROLL) {
        endian = *((uint8_t *)(&endian));
        if (endian == 1) {
            ZlixUtilsCrc32SliceLittleEndian(&tmpBuf, &len, &crc);
        } else {
            ZlixUtilsCrc32SliceBigEndian(&tmpBuf, &len, &crc);
        }
    }
#endif

    while (len > 0) {
        crc = (crc >> ZLIX_UTILS_CRC_SHIFT_BIT8) ^
            g_ZlixUtilsCrcTable32[0][(crc & ZLIX_UTILS_CRC_ONE_BYTE_MASK) ^ *tmpBuf++];
        len--;
    }

    *crcVal = ~crc;

    return ZLIX_OK;
}

/* crc32计算对外接口 */
uint32_t ZlixCrc32(uint32_t crc, const uint8_t *buf, size_t size)
{
    ZlixUtilsCrc32Cal(buf, size, &crc);

    return crc;
}

#define ZLIX_ADLER_SHIFT_16 16
#define ZLIX_2BYTES_PRIME_MAX 65521
#define ZLIX_FAST_ADLER_BLOCK_MAX 5552
#define ZLIX_ADLER_CALC_STEP(buf, i, adler32, high) {adler32 += (buf)[i]; high += adler32;}
#define ZLIX_ADLER_CALC_PROCESS(buf, adler32, high) \
    do { \
        ZLIX_ADLER_CALC_STEP(buf, 0, adler32, high) \
        ZLIX_ADLER_CALC_STEP(buf, 1, adler32, high) \
        ZLIX_ADLER_CALC_STEP(buf, 2, adler32, high) \
        ZLIX_ADLER_CALC_STEP(buf, 3, adler32, high) \
        ZLIX_ADLER_CALC_STEP(buf, 4, adler32, high) \
        ZLIX_ADLER_CALC_STEP(buf, 5, adler32, high) \
        ZLIX_ADLER_CALC_STEP(buf, 6, adler32, high) \
        ZLIX_ADLER_CALC_STEP(buf, 7, adler32, high) \
        ZLIX_ADLER_CALC_STEP(buf, 8, adler32, high) \
        ZLIX_ADLER_CALC_STEP(buf, 9, adler32, high) \
        ZLIX_ADLER_CALC_STEP(buf, 10, adler32, high) \
        ZLIX_ADLER_CALC_STEP(buf, 11, adler32, high) \
        ZLIX_ADLER_CALC_STEP(buf, 12, adler32, high) \
        ZLIX_ADLER_CALC_STEP(buf, 13, adler32, high) \
        ZLIX_ADLER_CALC_STEP(buf, 14, adler32, high) \
        ZLIX_ADLER_CALC_STEP(buf, 15, adler32, high) \
    } while (0)

ZLIX_STATIC ZLIX_ALWAYS_INLINE uint32_t ZlixUtilsAdler32CalOneByte(uint32_t adlerIn,
    const uint8_t *buf, uint32_t highIn)
{
    uint32_t adler32 = adlerIn + buf[0];
    uint32_t high = highIn;
    if (adler32 >= ZLIX_2BYTES_PRIME_MAX) {
        adler32 -= ZLIX_2BYTES_PRIME_MAX;
    }
    high += adler32;
    if (high >= ZLIX_2BYTES_PRIME_MAX) {
        high -= ZLIX_2BYTES_PRIME_MAX;
    }
    return adler32 | (high << ZLIX_ADLER_SHIFT_16);
}

uint32_t ZlixUtilsAdler32Cal(uint32_t adlerIn, const uint8_t *buf, size_t lenIn)
{
    uint32_t high = (adlerIn >> ZLIX_ADLER_SHIFT_16) & 0xffff;
    uint32_t adler32 = adlerIn & 0xffff;
    uint32_t n;
    size_t len = lenIn;
    if (buf == NULL || len == 0) {
        return 1L;
    }
    if (len == 1) {
        return ZlixUtilsAdler32CalOneByte(adler32, buf, high);
    }
    /* less than 16 Bytes */
    if (len < ZLIX_ADLER_SHIFT_16) {
        while ((len--) != 0) {
            adler32 += *buf++;
            high += adler32;
        }
        if (adler32 >= ZLIX_2BYTES_PRIME_MAX) {
            adler32 -= ZLIX_2BYTES_PRIME_MAX;
        }
        high %= ZLIX_2BYTES_PRIME_MAX;
        return adler32 | (high << ZLIX_ADLER_SHIFT_16);
    }
    /* more than 5552 Bytes */
    while (len >= ZLIX_FAST_ADLER_BLOCK_MAX) {
        len -= ZLIX_FAST_ADLER_BLOCK_MAX;
        n = ZLIX_FAST_ADLER_BLOCK_MAX / ZLIX_ADLER_SHIFT_16;
        do {
            ZLIX_ADLER_CALC_PROCESS(buf, adler32, high);
            buf += ZLIX_ADLER_SHIFT_16;
        } while ((--n) != 0);
        adler32 %= ZLIX_2BYTES_PRIME_MAX;
        high %= ZLIX_2BYTES_PRIME_MAX;
    }
    if (len != 0) {
        while (len >= ZLIX_ADLER_SHIFT_16) {
            len -= ZLIX_ADLER_SHIFT_16;
            ZLIX_ADLER_CALC_PROCESS(buf, adler32, high);
            buf += ZLIX_ADLER_SHIFT_16;
        }
        while ((len--) != 0) {
            adler32 += *buf++;
            high += adler32;
        }
        adler32 %= ZLIX_2BYTES_PRIME_MAX;
        high %= ZLIX_2BYTES_PRIME_MAX;
    }
    return adler32 | (high << ZLIX_ADLER_SHIFT_16);
}

uint32_t ZlixUtilsAdler32(const uint8_t *buf, size_t len)
{
    return ZlixUtilsAdler32Cal(1, buf, len);
}
