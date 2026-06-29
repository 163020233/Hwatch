/**
 * @file zlix_inflate.c
 * @copyright Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * @brief \n
 * 提供解压缩的关键接口，比如压缩初始化、去初始化、压缩、压缩重置以及获取压缩输出缓存区大小等关键接口
 * @author Anonym
 * @date 2021-10-22
 * @version v0.1.0
 * *******************************************************************************************
 * @par 修改日志：
 * <table>
 * <tr><th>Date        <th>Version  <th>Author    <th>Description
 * <tr><td>2022-11-27  <td>0.1.0    <td>Anonym    <td>解压缩关键接口
 * </table>
 *
 * *******************************************************************************************
 */
#include "zlix_public_inner.h"
#include "zlix_utils_inner.h"
#include "zlix_inflate_inner.h"

typedef int32_t (*ZLIX_PROC_FUNC) (ZLIX_INFLATE_STATE *interState, int32_t flushType);

#define ZLIX_INFLATE_MODE_NUM 30

ZLIX_STATIC ZLIX_PROC_FUNC g_statusFunc[ZLIX_INFLATE_MODE_NUM] = {0};

#define ZLIX_READ_BITS(bitBuf, need)  ((uint32_t)(bitBuf) & ((1 << (need)) - 1))

#define PUT_BYTE_RETURN_IF_INPUTOVER(interState, stream) do { \
    if ((interState)->readsofar < (stream)->availIn) { \
        (interState)->hold |= (uint32_t)((stream)->nextIn[(interState)->readsofar++]) << (interState)->bits; \
        (interState)->bits += 8; \
    } else { \
        return ZLIX_INFLATE_LEAVE; \
    } \
} while (0)

/* 从bit缓冲区中删掉need位元素 */
#define ZLIX_DROP_BITS(bitBuf, bitCnt, need) do { \
    (bitBuf) >>= (need); \
    (bitCnt) -= (uint8_t)(need); \
} while (0)

/* 先判断是否符合所需位数，如不符合则按字节进行读取，最后将所需位数输出到val中 */
#define READ_BITS_RETURN_IF_INPUTOVER(interState, needBits, stream, val) do { \
    while ((interState)->bits < (uint8_t)(needBits)) { \
        PUT_BYTE_RETURN_IF_INPUTOVER(interState, stream); \
    } \
    (val) = (typeof((val)))((uint64_t)(interState)->hold & ((1ULL << (needBits)) - 1)); \
    (interState)->hold = (uint32_t)((uint64_t)(interState)->hold >> (needBits)); \
    (interState)->bits -= (needBits); \
} while (0)

#define ZLIX_TABLE_CFG(tbase, tcode, tmaxLen, thres, ExtraBits, rangeBase, tableCfg) do { \
    (tableCfg).base = (tbase); \
    (tableCfg).code = (tcode); \
    (tableCfg).maxLen = (tmaxLen); \
    (tableCfg).threshold = (thres); \
    (tableCfg).extraBits = (ExtraBits); \
    (tableCfg).rangeBaseVal = (rangeBase); \
} while (0)

#define ZLIX_TABLE_CFG_SET(tbase, tcode, tmaxLen, thres, ExtraBits, rangeBase, tableCfg) do { \
    (tbase) = (tableCfg)->base; \
    (tcode) = (tableCfg)->code; \
    (tmaxLen) = (tableCfg)->maxLen; \
    (thres) = (tableCfg)->threshold; \
    (ExtraBits) = (tableCfg)->extraBits; \
    (rangeBase) = (tableCfg)->rangeBaseVal; \
} while (0)

#define ZLIX_CODE_TABLE_SET(codeTable, pos, opType, exBit, codeLen, baseVal) do { \
    (codeTable)[pos].op = (opType); \
    (codeTable)[pos].exBits = (exBit); \
    (codeTable)[pos].bits = (codeLen); \
    (codeTable)[pos].rangebase = (baseVal); \
} while (0)

#define ZLIX_CRC_2_BYTES(value, buf, temp) do { \
    (temp)[0] = (uint8_t)(buf); \
    (temp)[1] = (uint8_t)((buf) >> 8); \
    (void)ZlixUtilsCrc32Cal(temp, 2, &(value)); \
} while (0)

#define ZLIX_CRC_4_BYTES(value, buf, temp) do { \
    (temp)[0] = (uint8_t)(buf); \
    (temp)[1] = (uint8_t)((buf) >> 8); \
    (temp)[2] = (uint8_t)((buf) >> 16); \
    (temp)[3] = (uint8_t)((buf) >> 24); \
    (void)ZlixUtilsCrc32Cal(temp, 4, &(value)); \
} while (0)

#define ZLIX_FAST_GET_TWO_BYTE(hold, bits, cur) do { \
    if ((bits) < 15) { \
        uint16_t out;   \
        (cur) += ReadLE16Bit((cur), &out);  \
        (hold) |= (uint32_t)out << (bits);  \
        (bits) += 16; \
    } \
} while (0)

#define ZLIX_WITHDRAW_BYTES(hold, bits, readsofar) do { \
    uint8_t bytes = (uint8_t)((bits) / 8); \
    (bits) -= (uint32_t)(bytes * 8);  \
    (readsofar) -= (uint32_t)bytes;   \
    (hold) = (uint32_t)((hold) << (32 - (bits))) >> (32 - (bits));  \
} while (0)

/* 判断FLG各标记位是否置位 */
#define ZLIX_FLG_FHCRC(interState) ((interState)->flags & 0x0200U) /* bit1置位，对gzip文件头部进行CRC16校验 */
#define ZLIX_FLG_FEXTRA(interState) ((interState)->flags & 0x0400U) /* bit2置位，带着扩展gzip头部部分 */
/* 判断wrap的置位结果 */
#define ZLIX_WRAP_GZIP(interState) ((interState)->wrap & 0x0002U) /* 0x0002表示bit1置位，gzip格式 */
#define ZLIX_WRAP_VALID(interState) ((interState)->wrap & 0x0004U) /* 0x0004表示bit2置位，需要进行crc合法性校验 */
/* 判断压缩文件是否为gzip格式 */
#define ZLIX_IS_GZIP(interState, id) ((ZLIX_WRAP_GZIP(interState) != 0) && ((id) == ZLIX_GZIP_FLAG_ID_MAGIC))
/* 0x00ff表示判断CM的值，因为只有deflate，所以结果不为0x0008则错误；
   0xe000表示判断FLG的高3位，因为高3位为预留位，必须全0，所以结果不为0则错误 */
#define ZLIX_GZIP_CHECK_FLAGS(interState) ((((interState)->flags & 0x00ffU) != 0x0008) ||    \
                                            (((interState)->flags & 0xe000U) != 0))
/* flags&0x0200表示会对gzip头进行crc校验，wrap&0x0004表示需要进行合法性校验 */
#define ZLIX_GZIP_CRC_CHECK_FLAG(interState) ((ZLIX_FLG_FHCRC(interState) != 0)  && (ZLIX_WRAP_VALID(interState) != 0))

#define ZLIX_MATCH_COPY_FROM_THREE_TO_ONE_BYTES(nextOut, bytes, start) do { \
    while ((bytes) > 2) { \
        *(nextOut)++ = *(start)++; \
        *(nextOut)++ = *(start)++; \
        *(nextOut)++ = *(start)++; \
        (bytes) -= 3; \
    } \
    if ((bytes) > 0) { \
        *(nextOut)++ = *(start)++; \
        if ((bytes) > 1) { \
            *(nextOut)++ = *(start)++; \
        } \
    } \
} while (0)

#define ZLIX_INIT_CL_TREE_PARAM(threshold, k, thres, secondBegin, secondRoot, secondLen)  do { \
    (k) = 0; \
    (thres) = 0; \
    (secondBegin) = 0; \
    (secondRoot) = (unsigned)(-1); \
    (secondLen) = 1 << (threshold); \
} while (0)

#define ZLIX_CHECK_DATA_AND_RETURN(condition, state) do { \
    if (condition) { \
        (state)->mode = ZLIX_INFLATE_BAD; \
        (state)->error = ZLIX_DATA_INVAID; \
        return ZLIX_INFLATE_LEAVE; \
    } \
} while (0)

#define ZLIX_4BITS_MASK 0xF

/* 用于处理写matchLength时offset重叠的情况 */
uint8_t g_overlapOffset[] = {0, 1, 2, 2, 4, 3, 2, 1};

ZLIX_STATIC ZLIX_ALWAYS_INLINE int ZLIX_ZLIB_ADLER_CHECK(uint8_t *start,
    uint32_t out, ZLIX_INFLATE_STATE *interState, uint32_t checkcrcIn)
{
    uint32_t checkcrc = Swap32(checkcrcIn);
    interState->check = ZlixUtilsAdler32Cal(interState->check, start, out);
    ZLIX_CHECK_DATA_AND_RETURN(checkcrc != interState->check, interState);
    interState->mode = ZLIX_INFLATE_DONE;
    return ZLIX_INFLATE_NOT_LEAVE;
}

ZLIX_STATIC int ZlixZlibHeaderCheck(ZLIX_INFLATE_STATE *interState, ZlixStream *streamInfo, uint16_t id)
{
    uint32_t winBits;
    uint32_t hold = id; // zlib头是小端格式存储,低字节为CMF,高字节为FLG.
    // 移动8位，取出CMF，FCHECK确保CMF * 0xFF + FLG的值能整除31
    if ((interState->wrap & 1) == 0 || ((((hold & ZLIX_1BYTES_MASK) << 8) + (hold >> 8)) % 31) != 0) {
        interState->mode = ZLIX_INFLATE_BAD;
        return ZLIX_INFLATE_NOT_LEAVE; // 错误头，进入Bad处理流程钩子中进行处理
    }
    if ((hold & ZLIX_4BITS_MASK) != 8) { // CMF低4bit写死为8 == Z_DEFLATED，CMF高4bit为winBits - 8
        interState->mode = ZLIX_INFLATE_BAD;
        return ZLIX_INFLATE_NOT_LEAVE; // 错误头，进入Bad处理流程钩子中进行处理
    }
    hold >>= 4; // 移位4bit
    winBits = (hold & ZLIX_4BITS_MASK) + 8; // CMF高4bit为winBits - 8
    if (interState->wbits == 0) {
        interState->wbits = winBits;
    }
    if (winBits > 15 || winBits > interState->wbits) { // 这里只要用户传入的interState->wbits大于15即可，不必强制相等
        interState->mode = ZLIX_INFLATE_BAD;
        return ZLIX_INFLATE_NOT_LEAVE; // 错误头，进入Bad处理流程钩子中进行处理
    }
    interState->wsize = 1 << winBits;
    interState->flags = 0; // 走到这里说明zlib头校验ok
    streamInfo->adler = interState->check = 1; // adler(NULL, 0)
    interState->mode = ZLIX_INFLATE_TYPE; // zlib 2字节头处理完直接跳到TYPE模式，因为当前不支持字典模式
    interState->hold = 0; // 清空缓冲区
    interState->bits = 0;
    return ZLIX_INFLATE_NOT_LEAVE;
}

ZLIX_STATIC int32_t ZlixInflateHeadProc(ZLIX_INFLATE_STATE *interState, int32_t flushType)
{
    uint16_t id;
    ZlixStream *streamInfo = interState->stream;
    uint8_t temp[4];

    (void)flushType;

    /* raw格式 */
    if (interState->wrap == 0) {
        interState->mode = ZLIX_INFLATE_TYPEDO;
        return ZLIX_INFLATE_NOT_LEAVE;
    }

    /* gzip格式 */
    READ_BITS_RETURN_IF_INPUTOVER(interState, 16, streamInfo, id);
    if (ZLIX_IS_GZIP(interState, id) != 0) {
        if (interState->wbits == 0) {
            interState->wbits = ZLIX_INFLATE_WINBITS_HIGH;
        }
        interState->check = 0;
        /* 将id中的2个字节进行CRC校验 */
        ZLIX_CRC_2_BYTES(interState->check, id, temp);
        interState->mode = ZLIX_INFLATE_FLAGS;
        return ZLIX_INFLATE_NOT_LEAVE;
    }

    /* 其他格式就认为是zlib格式 */
    return ZlixZlibHeaderCheck(interState, streamInfo, id);
}

ZLIX_STATIC int32_t ZlixInflateFlagsProc(ZLIX_INFLATE_STATE *interState, int32_t flushType)
{
    ZlixStream *streamInfo = interState->stream;
    uint8_t temp[4];

    (void)flushType;

    READ_BITS_RETURN_IF_INPUTOVER(interState, 16, streamInfo, interState->flags);
    ZLIX_CHECK_DATA_AND_RETURN(ZLIX_GZIP_CHECK_FLAGS(interState), interState);

    if (ZLIX_GZIP_CRC_CHECK_FLAG(interState) != 0) {
        ZLIX_CRC_2_BYTES(interState->check, interState->flags, temp);
    }

    interState->mode = ZLIX_INFLATE_TIME;
    return ZLIX_INFLATE_NOT_LEAVE;
}

ZLIX_STATIC int32_t ZlixInflateTimeProc(ZLIX_INFLATE_STATE *interState, int32_t flushType)
{
    ZlixStream *streamInfo = interState->stream;
    uint8_t temp[4];
    uint32_t time;

    (void)flushType;

    READ_BITS_RETURN_IF_INPUTOVER(interState, 32, streamInfo, time);

    if (ZLIX_GZIP_CRC_CHECK_FLAG(interState) != 0) {
        ZLIX_CRC_4_BYTES(interState->check, time, temp);
    }

    interState->mode = ZLIX_INFLATE_OS;
    return ZLIX_INFLATE_NOT_LEAVE;
}

ZLIX_STATIC int32_t ZlixInflateOsProc(ZLIX_INFLATE_STATE *interState, int32_t flushType)
{
    ZlixStream *streamInfo = interState->stream;
    uint8_t temp[4];
    uint16_t os;

    (void)flushType;

    READ_BITS_RETURN_IF_INPUTOVER(interState, 16, streamInfo, os);

    if (ZLIX_GZIP_CRC_CHECK_FLAG(interState) != 0) {
        ZLIX_CRC_2_BYTES(interState->check, os, temp);
    }

    interState->mode = ZLIX_INFLATE_EXLEN;
    return ZLIX_INFLATE_NOT_LEAVE;
}

ZLIX_STATIC int32_t ZlixInflateExlenProc(ZLIX_INFLATE_STATE *interState, int32_t flushType)
{
    ZlixStream *streamInfo = interState->stream;
    uint8_t temp[4];

    (void)flushType;

    if (ZLIX_FLG_FEXTRA(interState) != 0) {
        READ_BITS_RETURN_IF_INPUTOVER(interState, 16, streamInfo, interState->length);
        if (ZLIX_GZIP_CRC_CHECK_FLAG(interState) != 0) {
            ZLIX_CRC_2_BYTES(interState->check, interState->length, temp);
        }
    }

    if (interState->length == 0) {
        interState->mode = ZLIX_INFLATE_NAME;
    } else {
        interState->mode = ZLIX_INFLATE_EXTRA;
    }
    return ZLIX_INFLATE_NOT_LEAVE;
}

ZLIX_STATIC int32_t ZlixInflateExtraProc(ZLIX_INFLATE_STATE *interState, int32_t flushType)
{
    ZlixStream *streamInfo = interState->stream;
    uint32_t remain;
    uint32_t copy;

    (void)flushType;

    if (ZLIX_FLG_FEXTRA(interState) != 0) {
        remain = streamInfo->availIn - interState->readsofar;
        copy = (interState->length < remain) ? interState->length : remain;
        if (copy != 0) {
            if (ZLIX_GZIP_CRC_CHECK_FLAG(interState) != 0) {
                (void)ZlixUtilsCrc32Cal((const uint8_t *)(&streamInfo->nextIn[interState->readsofar]),
                    copy, &interState->check);
                interState->readsofar += copy;
                interState->length -= (uint16_t)copy;
            }
        }
        if (interState->length != 0) {
            return ZLIX_INFLATE_LEAVE;
        }
    }

    interState->length = 0;

    interState->mode = ZLIX_INFLATE_NAME;
    return ZLIX_INFLATE_NOT_LEAVE;
}

ZLIX_STATIC int32_t ZlixInflateNameCommentProc(ZLIX_INFLATE_STATE *interState, int32_t flushType)
{
    ZlixStream *streamInfo = interState->stream;
    uint32_t remain;
    uint32_t len = 0;
    const uint8_t *name;
    uint8_t temp;
    uint32_t checkflag = (interState->mode == ZLIX_INFLATE_NAME) ? 0x0800 : 0x1000; /* 0x0800和0x1000用来选择对应模式 */

    (void)flushType;

    if ((interState->flags & checkflag) != 0) {
        remain = streamInfo->availIn - interState->readsofar;
        if (remain == 0) {
            return ZLIX_INFLATE_LEAVE;
        }
        name = (const uint8_t *)(&streamInfo->nextIn[interState->readsofar]);
        do {
            temp = name[len++];
        } while (temp != 0 && len < remain);
        interState->readsofar += len;
        if (ZLIX_GZIP_CRC_CHECK_FLAG(interState) != 0) {
            (void)ZlixUtilsCrc32Cal(name, len, &interState->check);
        }
        if (name[len - 1] != 0) {
            return ZLIX_INFLATE_LEAVE;
        }
    }

    interState->mode = (interState->mode == ZLIX_INFLATE_NAME) ? ZLIX_INFLATE_COMMENT : ZLIX_INFLATE_HCRC;
    return ZLIX_INFLATE_NOT_LEAVE;
}

ZLIX_STATIC int32_t ZlixInflateHcrcProc(ZLIX_INFLATE_STATE *interState, int32_t flushType)
{
    ZlixStream *streamInfo = interState->stream;
    uint16_t checkcrc;

    (void)flushType;

    if (ZLIX_FLG_FHCRC(interState) != 0) {
        READ_BITS_RETURN_IF_INPUTOVER(interState, 16, streamInfo, checkcrc);
        if (ZLIX_WRAP_VALID(interState) != 0) {
            ZLIX_CHECK_DATA_AND_RETURN(checkcrc != (interState->check & 0xffff), interState); /* 0xffff用来控制结果位数 */
        }
    }
    interState->check = 0;

    interState->mode = ZLIX_INFLATE_TYPE;
    return ZLIX_INFLATE_NOT_LEAVE;
}

ZLIX_STATIC ZLIX_ALWAYS_INLINE int32_t ZlixInflateClTreeCheck(uint16_t *bl_cnt, uint32_t isCcl, uint32_t maxCodeLen,
    uint8_t bl_cnt_size)
{
    int32_t remain = 1;
    uint32_t i = 1;

    if (maxCodeLen >= bl_cnt_size) {
        return ZLIX_ERROR;
    }

    for (; i <= maxCodeLen; i++) {
        remain = (int32_t)((uint32_t)remain << 1);
        remain -= bl_cnt[i];
        if (remain < 0) {
            return ZLIX_ERROR;
        }
    }
    if ((remain > 0) && ((isCcl == 1) || (maxCodeLen != 1))) {
        return ZLIX_ERROR;
    }
    return ZLIX_OK;
}

ZLIX_STATIC int32_t ZlixInflateCclTreesCreate(ZLIX_INFLATE_STATE *interState, CODE_INFO *cl, uint32_t range,
    uint16_t *bl_cnt, uint8_t bl_cnt_size)
{
    uint32_t index = 0;
    int32_t currCodeCnt;
    int32_t pos;
    int32_t delta;
    CODE_TABLE_INFO *code = interState->lcodes;

    if (ZlixInflateClTreeCheck(bl_cnt, 1, interState->maxcodeLen, bl_cnt_size) != ZLIX_OK) {
        return ZLIX_ERROR;
    }

    /* 找到第一个编码长度不为0的序列节点 */
    while (cl[index].codeLen == 0) {
        index++;
    }
    /* 非0的进去的顺序符合要求 */
    for (; index < range; index++) {
        currCodeCnt = 1 << (interState->maxcodeLen - cl[index].codeLen);
        delta = 1 << cl[index].codeLen;

        pos = cl[index].code;
        while (currCodeCnt-- > 0) {
            ZLIX_CODE_TABLE_SET(code, pos, 0, g_bExtraBits[cl[index].val], cl[index].codeLen, cl[index].val);
            pos += delta;
        }
    }
    return ZLIX_OK;
}

static inline uint32_t ZlixInflateSecTableLenGet(const ZLIX_INFLATE_STATE *interState, uint32_t threshold,
    int32_t *codelevel, uint16_t *bl_cnt, uint8_t bl_cnt_size)
{
    int32_t k = *codelevel;
    int32_t left = 1 << k;

    while (k < (int32_t)(interState->maxcodeLen - (uint16_t)threshold)) {
        if ((k + (int32_t)threshold) >= (int32_t)bl_cnt_size) {
            break;
        }

        left -= (int32_t)(bl_cnt[k + (int32_t)threshold]);
        if (left <= 0) {
            break;
        }
        k++;
        left = (int32_t)((uint32_t)left << 1);
    }
    *codelevel = k;
    return (1 << k);
}

ZLIX_STATIC int32_t ZlixInflateClTreesCreate(ZLIX_INFLATE_STATE *interState, const CODE_TABLE_CINFIG *config,
    CODE_INFO *cl, int32_t cl_size, uint16_t *bl_cnt, uint8_t bl_cnt_size)
{
    int32_t index = 0;
    int32_t len, k, thres;
    uint32_t currCodeCnt, delta, pos, extraBitLen, threshold, secondBegin, secondRoot, secondLen;
    const uint8_t *extra;
    uint16_t base, rangebase;
    const uint16_t *range_base;
    CODE_TABLE_INFO *codeTable;

    ZLIX_TABLE_CFG_SET(base, codeTable, len, threshold, extra, range_base, config);

    if (ZlixInflateClTreeCheck(bl_cnt, 0, interState->maxcodeLen, bl_cnt_size) != ZLIX_OK) {
        return ZLIX_ERROR;
    }

    while (cl[index].codeLen == 0) {
        index++;
        if (index >= cl_size) {
            return ZLIX_ERROR;
        }
    }

    ZLIX_INIT_CL_TREE_PARAM(threshold, k, thres, secondBegin, secondRoot, secondLen);

    if ((len - 1) >= cl_size) {
        return ZLIX_ERROR;
    }
    for (; index < len; index++) {
        /* 依次输入符号和码长的信息, k为二级码表的层数，threshold为一级码表深度阈值、codelen只有8位，不会超过int的正数最大值 */
        currCodeCnt = 1u << (uint32_t)(k + (int32_t)threshold - (int32_t)cl[index].codeLen);
        delta = 1u << (cl[index].codeLen - thres);
        pos = (cl[index].code >> thres) + secondBegin;

        /* extra bit信息获取 distance的base就是0，lit的base就是257 */
        if (cl[index].val >= base) {
            cl[index].val -= base;
            /* 获取val所在的区间，保存区间的extra bit的长度以及区间的base值, 节点为ZLIX_LITERALS_EXTRA_BASE + range */
            extraBitLen = extra[cl[index].val];
            rangebase = range_base[cl[index].val];

            while (currCodeCnt-- > 0) {
                ZLIX_CODE_TABLE_SET(
                    codeTable, pos, 1, (uint8_t)extraBitLen, cl[index].codeLen - (uint8_t)thres, rangebase);
                pos += delta;
            }
        } else {
            while (currCodeCnt-- > 0) {
                ZLIX_CODE_TABLE_SET(codeTable, pos, 0, 0, cl[index].codeLen - (uint8_t)thres, cl[index].val);
                pos += delta;
            }
        }
        if (cl[index].codeLen >= bl_cnt_size) {
            return ZLIX_ERROR;
        }
        bl_cnt[cl[index].codeLen]--;

        if (index == len - 1) {
            break;
        }

        /* 这里的code是真实的逆序编码，搭建二级码表 */
        if ((cl[index + 1].codeLen > threshold) && ((cl[index + 1].code & ((1 << threshold) - 1)) != secondRoot)) {
            secondBegin += secondLen;
            secondRoot = cl[index + 1].code & ((1 << threshold) - 1);

            k = (int32_t)(cl[index + 1].codeLen - threshold);
            secondLen = ZlixInflateSecTableLenGet(interState, threshold, &k, bl_cnt, bl_cnt_size);

            /* 更新一下二层码表的根节点信息位置 */
            thres = (int32_t)threshold;
            ZLIX_CODE_TABLE_SET(codeTable, secondRoot, 0, (uint8_t)k, (uint8_t)threshold, (uint16_t)secondBegin);
        }
    }
    return ZLIX_OK;
}

ZLIX_STATIC void ZlixInflateStLTreeCreate(ZLIX_INFLATE_STATE *interState)
{
    uint32_t i, j;
    uint32_t k = 0;
    uint16_t clCode, codeLen;
    uint16_t firstCode[ZLIX_STATIC_LIT_MAX_LEN + 1];
    CODE_TABLE_CINFIG tableCfg;
    ZLIX_INFLATE_STATE *state = interState;
    static CODE_INFO cl[ZLIX_STATIC_LIT_MAX_RANGE + 1];
    uint16_t bl_cnt[ZLIX_BL_CNT_LEN];

    state->maxcodeLen = ZLIX_STATIC_LIT_MAX_LEN;
    (void)memset_s(bl_cnt, ZLIX_BL_CNT_LEN * sizeof(uint16_t), 0, ZLIX_BL_CNT_LEN * sizeof(uint16_t));
    (void)memset_s(cl, sizeof(CODE_INFO) * (ZLIX_STATIC_LIT_MAX_RANGE + 1), 0,
        sizeof(CODE_INFO) * (ZLIX_STATIC_LIT_MAX_RANGE + 1));

    for (i = 0; i < ZLIX_STATIC_CODELEN_TYPE; i++) {
        for (j = g_staticCodeRange[i][0]; j <= g_staticCodeRange[i][1]; j++) {
            cl[k].val = (uint16_t)j;
            cl[k].codeLen = g_staticCodeLen[i];
            k++;
        }
        bl_cnt[g_staticCodeLen[i]] += g_staticCodeRange[i][1] - g_staticCodeRange[i][0] + 1;
    }

    clCode = 0;
    for (i = 1; i <= state->maxcodeLen; i++) {
        clCode = (clCode + bl_cnt[i - 1]) << 1;
        firstCode[i] = (uint16_t)clCode;
    }

    for (j = 0; j < ZLIX_STATIC_CODELEN_TYPE; j++) {
        for (i = g_fixedSeg[j].beginPos; i <= g_fixedSeg[j].endPos; i++) {
            /* 保证codeLen不为0 */
            codeLen = cl[i].codeLen;
            cl[i].code = (uint16_t)ZlixInflateReverse(firstCode[codeLen], codeLen);
            firstCode[codeLen]++;
        }
    }

    ZLIX_TABLE_CFG(ZLIX_LITERALS_EXTRA_BASE, interState->lcodes, ZLIX_STATIC_LIT_MAX_RANGE + 1,
                   ZLIX_STATIC_LIT_MAX_LEN, g_lExtraBits, g_baseLength, tableCfg);
    interState->lthres = ZLIX_STATIC_LIT_MAX_LEN;

    (void)ZlixInflateClTreesCreate(interState, &tableCfg, cl, ZLIX_STATIC_LIT_MAX_RANGE + 1, bl_cnt, ZLIX_BL_CNT_LEN);
}

ZLIX_STATIC void ZlixInflateStDTreeCreate(ZLIX_INFLATE_STATE *interState)
{
    uint32_t i;
    uint32_t pos;
    uint16_t firstCode = 0;
    ZLIX_INFLATE_STATE *state = interState;
    CODE_TABLE_INFO *codeTable = state->dcodes;

    state->maxcodeLen = ZLIX_STATIC_DIST_MAX_LEN;
    state->dthres = ZLIX_STATIC_DIST_MAX_LEN;

    /* 0-29的编码长度都为5 */
    for (i = 0; i < ZLIX_DISTANCE_CODE_RANGE; i++) {
        pos = (uint16_t)ZlixInflateReverse(firstCode, ZLIX_STATIC_DIST_MAX_LEN);
        codeTable[pos].op = 1;
        codeTable[pos].exBits = g_dExtraBits[i];
        codeTable[pos].bits = ZLIX_STATIC_DIST_MAX_LEN;
        codeTable[pos].rangebase = (uint16_t)g_baseDist[i];
        firstCode++;
    }
}

ZLIX_STATIC int32_t ZlixInflateStoredLenProc(ZLIX_INFLATE_STATE *interState, int32_t flushType)
{
    ZLIX_INFLATE_STATE *state;
    ZlixStream *streamInfo;
    uint32_t len;

    state = interState;
    streamInfo = state->stream;
    (void)flushType;

    READ_BITS_RETURN_IF_INPUTOVER(interState, 32, streamInfo, len);

    state->length = len & 0xffff; /* use 0xffff to get the last 16 bits */

    /* 取后16bit与前16bit进行计算比对，用0xffff取前16位的反集 */
    ZLIX_CHECK_DATA_AND_RETURN(state->length != ((len >> 16) ^ 0xffff), state);

    state->mode = ZLIX_INFLATE_COPY;
    if (flushType == ZLIX_TREES) {
        return ZLIX_INFLATE_LEAVE;
    }
    return ZLIX_INFLATE_NOT_LEAVE;
}

ZLIX_STATIC int32_t ZlixInflateStoredProc(ZLIX_INFLATE_STATE *interState, int32_t flushType)
{
    ZLIX_INFLATE_STATE *state = interState;
    ZlixStream *streamInfo = state->stream;
    uint32_t leftOut;
    uint32_t leftIn;
    int err = ZLIX_OK;

    (void)flushType;
    /*  store块长度为0,不用做什么直接返回即可 */
    if (state->length == 0) {
        state->mode = ZLIX_INFLATE_TYPE;
        return ZLIX_INFLATE_NOT_LEAVE;
    }

    leftOut = streamInfo->availOut - state->pending;
    leftIn = streamInfo->availIn - state->readsofar;

    /* 取输入、输出、以及目标赋值长度的最小值 */
    leftOut = (leftOut < leftIn) ? leftOut : leftIn;
    leftOut = (state->length < leftOut) ? (uint32_t)state->length : leftOut;

    if (leftOut == 0) {
        return ZLIX_INFLATE_LEAVE;
    }
    if (ZLIX_UNLIKELY(streamInfo->availOut - state->pending > SECUREC_MEM_MAX_LEN)) {
        err = (memcpy_s(streamInfo->nextOut + state->pending, SECUREC_MEM_MAX_LEN,
            streamInfo->nextIn + state->readsofar, (size_t)leftOut) == EOK) ? ZLIX_OK : ZLIX_SEC_FUNC_FAILURE;
    } else {
        err = (memcpy_s(streamInfo->nextOut + state->pending, streamInfo->availOut - state->pending,
            streamInfo->nextIn + state->readsofar, (size_t)leftOut) == EOK) ? ZLIX_OK : ZLIX_SEC_FUNC_FAILURE;
    }
    if (err != ZLIX_OK) {
        state->mode = ZLIX_INFLATE_MEM;
        state->error = ZLIX_MEM_OPER_FAILURE;
        return ZLIX_INFLATE_LEAVE;
    }
    /* 更新记录 */
    state->pending += (uint32_t)leftOut;
    state->readsofar += (uint32_t)leftOut;
    state->length -= (uint16_t)leftOut;

    /* 如果写完了，可以进入下个块 */
    if (state->length == 0) {
        state->mode = ZLIX_INFLATE_TYPE;
        return ZLIX_INFLATE_NOT_LEAVE;
    }
    /* 如果没写完，要不输入不足，要不输出不足，返回错误码 */
    return ZLIX_INFLATE_LEAVE;
}

ZLIX_STATIC int32_t ZlixInflateTypeProc(ZLIX_INFLATE_STATE *interState, int32_t flushType)
{
    if ((flushType == ZLIX_TREES) || (flushType == ZLIX_BLOCK)) {
        return ZLIX_INFLATE_LEAVE;
    }
    interState->mode = ZLIX_INFLATE_TYPEDO;
    return ZLIX_INFLATE_NOT_LEAVE;
}

ZLIX_STATIC int32_t ZlixInflateTypeDoProc(ZLIX_INFLATE_STATE *interState, int32_t flushType)
{
    uint32_t bHead = 0;
    uint32_t type;
    ZLIX_INFLATE_STATE *state = interState;
    ZlixStream *streamInfo = state->stream;

    if (state->last == 1) {
        ZLIX_DROP_BITS(state->hold, state->bits, state->bits & 7);
        if (interState->wrap != 0) {
            state->mode = ZLIX_INFLATE_CHECK;
        } else {
            state->mode = ZLIX_INFLATE_DONE;
        }
        return ZLIX_INFLATE_NOT_LEAVE;
    }
    READ_BITS_RETURN_IF_INPUTOVER(state, ZLIX_HEAD_LEN, streamInfo, bHead);

    state->last = bHead & 1;
    type = bHead >> 1;
    ZLIX_CHECK_DATA_AND_RETURN(type == DESTROYED_HEAD_FLAG, state);

    if (type == STORED_HEAD_FLAG) {
        ZLIX_DROP_BITS(state->hold, state->bits, state->bits & 7);
        state->mode = ZLIX_INFLATE_STORED;
        return ZLIX_INFLATE_NOT_LEAVE;
    }
    if (type == FIXED_HUFF_HEAD_FLAG) {
        /* 创建静态 LIT树 */
        ZlixInflateStLTreeCreate(state);
        /* 创建静态 DIST树 */
        ZlixInflateStDTreeCreate(state);
        state->strategy = ZLIX_DEFLATE_FIXED;
        state->mode = ZLIX_INFLATE_LEN_FAST;
        if (flushType == ZLIX_TREES) {
            return ZLIX_INFLATE_LEAVE;
        }
    } else if (type == DYNAMIC_HUFF_HEAD_FLAG) {
        state->strategy = ZLIX_DEFLATE_DEFAULT_STRATEGY;
        state->mode = ZLIX_INFLATE_TABLELEN;
    }
    return ZLIX_INFLATE_NOT_LEAVE;
}

/* 获取动态霍夫曼树各树序列的长度 */
ZLIX_STATIC int32_t ZlixInflateTableLenProc(ZLIX_INFLATE_STATE *interState, int32_t flushType)
{
    uint32_t val;
    ZlixStream *streamInfo;

    streamInfo = interState->stream;
    (void)flushType;

    /* 取数据14bit长度 */
    READ_BITS_RETURN_IF_INPUTOVER(interState, 14, streamInfo, val);

    /* 加上257后指的是CL1序列的长度 */
    interState->lennum = (uint16_t)((val & ((1 << ZLIX_HLIT_LEN) - 1)) + 257);
    val = val >> ZLIX_HLIT_LEN;

    /* 加上1后指的是CL2序列的长度 */
    interState->distnum = (uint16_t)((val & ((1 << ZLIX_HDIST_LEN) - 1)) + 1);
    val = val >> ZLIX_HDIST_LEN;

    /* 加上4后指的是CLL序列的长度 */
    interState->codenum = (uint16_t)((val & ((1 << ZLIX_HCCL_LEN) - 1)) + 4);

    interState->have = 0;
    interState->maxcodeLen = 0;

    ZLIX_CHECK_DATA_AND_RETURN((interState->lennum > ZLIX_LIT_LEN_CODE_RANGE) ||
        (interState->distnum > ZLIX_DISTANCE_CODE_RANGE), interState);

    interState->mode = ZLIX_INFLATE_TABLECCL;
    return ZLIX_INFLATE_NOT_LEAVE;
}

ZLIX_STATIC int32_t ZlixInflateTableCclProcWithCL(ZLIX_INFLATE_STATE *interState, int32_t flushType, CODE_INFO *cl,
    uint16_t *bl_cnt, uint8_t bl_cnt_size)
{
    uint32_t i, tmpPos;
    uint16_t clCode = 0;
    uint16_t firstCode[ZLIX_MAX_CODE_BITS_LEN_BL + 1];
    uint16_t firstIndex[ZLIX_MAX_CODE_BITS_LEN_BL + 1] = {0};
    uint16_t indexCount[ZLIX_MAX_CODE_BITS_LEN_BL + 1] = {0};
    CODE_INFO temp[ZLIX_TREE_CODE_RANGE];
    CODE_INFO *now;
    uint8_t *clArray = interState->clArray;
    ZlixStream *streamInfo = interState->stream;

    for (; interState->have < interState->codenum; interState->have++) {
        tmpPos = g_cclQueue[interState->have];
        READ_BITS_RETURN_IF_INPUTOVER(interState, 3, streamInfo, clArray[tmpPos]);
    }

    (void)memset_s(bl_cnt, sizeof(uint16_t) * ZLIX_BL_CNT_LEN, 0, sizeof(uint16_t) * ZLIX_BL_CNT_LEN);
    (void)memset_s(cl, sizeof(CODE_INFO) * ZLIX_TREE_CODE_RANGE, 0, sizeof(CODE_INFO) * ZLIX_TREE_CODE_RANGE);
    for (i = 0; i < interState->codenum; i++) {
        tmpPos = g_cclQueue[i];
        temp[tmpPos].codeLen = clArray[tmpPos];
        bl_cnt[clArray[tmpPos]]++;
        interState->maxcodeLen = (interState->maxcodeLen > clArray[tmpPos]) ? interState->maxcodeLen : clArray[tmpPos];
    }

    for (i = interState->have; i < ZLIX_TREE_CODE_RANGE; i++) {
        tmpPos = g_cclQueue[i];
        temp[tmpPos].codeLen = 0;
    }
    bl_cnt[0] += ZLIX_TREE_CODE_RANGE - interState->have;

    /* 求出各个深度（编码长度）对应的首个编码值，以及下标位置 */
    for (i = 1; i <= ZLIX_MAX_CODE_BITS_LEN_BL; i++) {
        clCode = (uint16_t)(clCode + bl_cnt[i - 1]) << 1U;
        firstCode[i] = (uint16_t)clCode;
        firstIndex[i] = firstIndex[i - 1] + bl_cnt[i - 1];
    }

    /* 把所有符号按码长大小和符号本身大小按照从小到大的顺序排序，码长相同的按符号大小排序 */
    for (i = 0; i < ZLIX_TREE_CODE_RANGE; i++) {
        now = &(cl[firstIndex[temp[i].codeLen] + indexCount[temp[i].codeLen]]);
        now->codeLen = temp[i].codeLen;
        now->val = (uint16_t)i;
        if (temp[i].codeLen != 0) {
            now->code = (uint16_t)ZlixInflateReverse(firstCode[temp[i].codeLen], temp[i].codeLen);
            firstCode[temp[i].codeLen]++;
        }
        indexCount[temp[i].codeLen]++;
    }

    ZLIX_CHECK_DATA_AND_RETURN(
        ZlixInflateCclTreesCreate(interState, cl, ZLIX_TREE_CODE_RANGE, bl_cnt, bl_cnt_size) != ZLIX_OK, interState);

    interState->mode = ZLIX_INFLATE_CODECL;

    /* 在读取解析CL序列的时候，需要用到清零的interState->have */
    interState->have = 0;

    (void)flushType;
    return ZLIX_INFLATE_NOT_LEAVE;
}

ZLIX_STATIC int32_t ZlixInflateTableCclProc(ZLIX_INFLATE_STATE *interState, int32_t flushType)
{
    /* 判断stream输出buf是否能够容纳cl的大小 */
    uint32_t leftSize = interState->stream->availOut - interState->pending;
    uint16_t clSize = ZLIX_DYNAMIC_CL1_AND_CL2_LEN * sizeof(CODE_INFO);
    uint8_t blCntSize = ZLIX_BL_CNT_LEN * sizeof(uint16_t);
    CODE_INFO cl[ZLIX_DYNAMIC_CL1_AND_CL2_LEN];
    uint16_t bl_cnt[ZLIX_BL_CNT_LEN];
    CODE_INFO *codel;
    uint16_t *blCnt;
    if (leftSize >= (uint32_t)(clSize + blCntSize)) {
        codel = (CODE_INFO *)(void *)(interState->stream->nextOut + interState->pending);
        blCnt = (uint16_t *)(void *)((uint8_t *)codel + clSize);
        return ZlixInflateTableCclProcWithCL(interState, flushType, codel, blCnt, ZLIX_BL_CNT_LEN); /* 存储节点编码信息 */
    } else {
        return ZlixInflateTableCclProcWithCL(interState, flushType, cl, bl_cnt, ZLIX_BL_CNT_LEN); /* 存储节点编码信息 */
    }
}

ZLIX_STATIC void ZlixInflateClArraySet(ZLIX_INFLATE_STATE *interState, const uint8_t *array, int32_t arrayLen,
    CODE_INFO *cl, uint16_t *bl_cnt)
{
    int32_t i;
    uint16_t clCode;
    uint16_t firstCode[ZLIX_MAX_CODE_BITS_LEN + 1];
    uint16_t firstIndex[ZLIX_MAX_CODE_BITS_LEN + 1] = {0};
    uint16_t indexCount[ZLIX_MAX_CODE_BITS_LEN + 1] = {0};
    ZLIX_INFLATE_STATE *state = interState;
    CODE_INFO *now;
    uint8_t codeLen;

    state->maxcodeLen = 0;
    (void)memset_s(bl_cnt, ZLIX_BL_CNT_LEN * sizeof(uint16_t), 0, ZLIX_BL_CNT_LEN * sizeof(uint16_t));
    for (i = 0; i < arrayLen; i++) {
        bl_cnt[array[i]]++;
    }

    clCode = 0;
    for (i = 1; i <= ZLIX_MAX_CODE_BITS_LEN; i++) {
        clCode = ((uint16_t)(clCode + bl_cnt[i - 1])) << 1;
        firstCode[i] = (uint16_t)clCode;
        firstIndex[i] = firstIndex[i - 1] + bl_cnt[i - 1];
        state->maxcodeLen = (bl_cnt[i] > 0) ? (uint16_t)i : state->maxcodeLen;
    }

    for (i = 0; i < arrayLen; i++) {
        codeLen = array[i];
        now = &(cl[firstIndex[codeLen] + indexCount[codeLen]]);
        now->codeLen = codeLen;
        now->val = (uint16_t)i;
        if (codeLen != 0) {
            now->code = (uint16_t)ZlixInflateReverse(firstCode[codeLen], codeLen);
            firstCode[codeLen]++;
        }
        indexCount[codeLen]++;
    }
}

static int32_t ZlixInflateDynTreeCreate(ZLIX_INFLATE_STATE *state, const uint8_t *clArray, const int32_t arrayLen,
    CODE_INFO *cl, uint16_t *bl_cnt, uint8_t bl_cnt_size)
{
    CODE_TABLE_CINFIG tableCfg;

    ZlixInflateClArraySet(state, clArray, state->lennum, cl, bl_cnt);
    state->lthres = (state->maxcodeLen < ZLIX_LEN_THRESHOLD) ? state->maxcodeLen : ZLIX_LEN_THRESHOLD;
    if (ZLIX_LIKELY(state->lthres != 0)) { /* 不直接返回错误等待解码符号时报错 */
        ZLIX_TABLE_CFG(ZLIX_LITERALS_EXTRA_BASE, state->lcodes, state->lennum, state->lthres,
                       g_lExtraBits, g_baseLength, tableCfg);
        /* 创建CL1码表 */
        if (ZlixInflateClTreesCreate(state, &tableCfg, cl, arrayLen, bl_cnt, bl_cnt_size) != ZLIX_OK) {
            return ZLIX_ERROR;
        }
    }

    state->distnum = ((arrayLen - state->lennum) > 0) ? state->distnum : 0;

    if (state->distnum > 0) {
        ZlixInflateClArraySet(state, &clArray[state->lennum], state->distnum, cl, bl_cnt);
        state->dthres = (state->maxcodeLen < ZLIX_DIST_THRESHOLD_DYN) ? state->maxcodeLen : ZLIX_DIST_THRESHOLD_DYN;
        if (ZLIX_LIKELY(state->dthres != 0)) { /* 不直接返回错误等待解码符号时报错 */
            ZLIX_TABLE_CFG(ZLIX_DIST_EXTRA_BASE, state->dcodes, state->distnum, state->dthres,
                           g_dExtraBits, g_baseDist, tableCfg);
            /* 创建CL2码表 */
            return ZlixInflateClTreesCreate(state, &tableCfg, cl, arrayLen, bl_cnt, bl_cnt_size);
        }
    }
    return ZLIX_OK;
}

/* 根据游程编码的标识将重复的序列解析出来 */
static inline void ZlixInflateRleArrayParse(ZLIX_INFLATE_STATE *state, const CODE_TABLE_INFO *code,
                                            uint8_t *clArray, const int32_t arrayLen, uint8_t symbol)
{
    uint32_t time;

    time = ZLIX_READ_BITS(state->hold, code->exBits);
    time += g_bExtraLenBase[code->rangebase];

    while ((time != 0) && (state->have < arrayLen)) {
        clArray[state->have++] = symbol;
        time--;
    }
}

ZLIX_STATIC int32_t ZlixInflateTableClArrayProcWithCL(ZLIX_INFLATE_STATE *interState, int32_t flushType, CODE_INFO *cl,
    uint16_t *bl_cnt, uint8_t bl_cnt_size)
{
    uint8_t sym;
    uint16_t thres;
    uint32_t readVal, lenCnt;
    uint8_t *clArray = interState->clArray;
    CODE_TABLE_INFO temp;
    ZLIX_INFLATE_STATE *state = interState;
    ZlixStream *streamInfo = interState->stream;
    size_t clSize;
    int ret;

    lenCnt = state->lennum + state->distnum;
    thres = state->maxcodeLen;

    while (state->have < lenCnt) {
        sym = 0;
        while (1) {
            readVal = ZLIX_READ_BITS(state->hold, thres);
            temp = state->lcodes[readVal];
            if ((uint8_t)(temp.bits + temp.exBits) <= state->bits) {
                break;
            }
            PUT_BYTE_RETURN_IF_INPUTOVER(state, streamInfo);
        }
        ZLIX_DROP_BITS(state->hold, state->bits, temp.bits);

        /* 18指的是游程编码的特殊值，超出18表示数据出错 */
        ZLIX_CHECK_DATA_AND_RETURN(temp.rangebase > 18, state);
        /* 小于16，则是正常的序列值 */
        if (temp.rangebase < 16) {
            clArray[state->have++] = (uint8_t)temp.rangebase;
            continue;
        }
        if (temp.rangebase == 16) { /* 16指的是游程编码的特殊值之一 */
            if (state->have == 0) {
                state->mode = ZLIX_INFLATE_BAD;
                return ZLIX_INFLATE_NOT_LEAVE;
            }
            sym = clArray[state->have - 1];
        }
        ZlixInflateRleArrayParse(state, &temp, clArray, ZLIX_DYNAMIC_CL1_AND_CL2_LEN, sym);

        ZLIX_DROP_BITS(state->hold, state->bits, temp.exBits);
    }

    /* 256的编码长度为0，则非法 */
    ZLIX_CHECK_DATA_AND_RETURN(clArray[256] == 0, state);

    clSize = ZLIX_DYNAMIC_CL1_AND_CL2_LEN * sizeof(CODE_INFO);
    (void)memset_s(cl, clSize, 0, clSize);
    ret = ZlixInflateDynTreeCreate(state, clArray, ZLIX_DYNAMIC_CL1_AND_CL2_LEN, cl, bl_cnt, bl_cnt_size);
    ZLIX_CHECK_DATA_AND_RETURN(ret != ZLIX_OK, state);

    /* 等待字符数据的输入状态 */
    state->mode = ZLIX_INFLATE_LEN_FAST;
    /* 在ZLIX_TREES模式下解析完CCL/CCL1/CCL2序列后直接退出 */
    if (flushType == ZLIX_TREES) {
        return ZLIX_INFLATE_LEAVE;
    }
    return ZLIX_INFLATE_NOT_LEAVE;
}

ZLIX_STATIC int32_t ZlixInflateTableClArrayProc(ZLIX_INFLATE_STATE *interState, int32_t flushType)
{
    /* 判断stream输出buf是否能够容纳cl的大小 */
    uint32_t leftSize = interState->stream->availOut - interState->pending;
    uint16_t clSize = ZLIX_DYNAMIC_CL1_AND_CL2_LEN * sizeof(CODE_INFO);
    uint16_t bl_cnt[ZLIX_BL_CNT_LEN];
    uint16_t *blCnt;
    uint8_t blCntSize = ZLIX_BL_CNT_LEN * sizeof(uint16_t);
    CODE_INFO *codel;
    CODE_INFO cl[ZLIX_DYNAMIC_CL1_AND_CL2_LEN];

    if (leftSize >= (uint32_t)(clSize + blCntSize)) {
        codel = (CODE_INFO *)(void *)(interState->stream->nextOut + interState->pending);
        blCnt = (uint16_t *)(void *)((uint8_t *)codel + clSize);
        /* 存储节点编码信息 */
        return ZlixInflateTableClArrayProcWithCL(interState, flushType, codel, blCnt, ZLIX_BL_CNT_LEN);
    } else {
        /* 存储节点编码信息 */
        return ZlixInflateTableClArrayProcWithCL(interState, flushType, cl, bl_cnt, ZLIX_BL_CNT_LEN);
    }
}

static inline int32_t ZlixInflateLiteralWrite(CODE_TABLE_INFO *point, ZLIX_INFLATE_STATE *state)
{
    ZlixStream *streamInfo = state->stream;
    if (state->pending == streamInfo->availOut) {
        /* 如果没有空间，设置到ZLIX_INFLATE_LIT状态 */
        state->length = point->rangebase;
        state->mode = ZLIX_INFLATE_LIT;
        return ZLIX_INFLATE_LEAVE;
    }
    /* 写入输出缓冲区 */
    streamInfo->nextOut[state->pending++] = (uint8_t)point->rangebase;
    state->mode = ZLIX_INFLATE_LEN;
    return ZLIX_INFLATE_NOT_LEAVE;
}

static inline int32_t ZlixInflateLenDistRecord(CODE_TABLE_INFO *point, ZLIX_INFLATE_STATE *state)
{
    state->extra = point->exBits;

    /* 这个分支是：解析lit中的len */
    if (state->mode == ZLIX_INFLATE_LEN) {
        state->length = point->rangebase;
        state->mode = (state->extra == 0) ? ZLIX_INFLATE_DIST : ZLIX_INFLATE_LENEXT;
        return ZLIX_INFLATE_NOT_LEAVE;
    }

    /* 这个分支是：解析dist */
    state->offset = point->rangebase;
    state->mode = (state->extra == 0) ? ZLIX_INFLATE_MATCH : ZLIX_INFLATE_DISTEXT;

    return ZLIX_INFLATE_NOT_LEAVE;
}

ZLIX_STATIC ZLIX_ALWAYS_INLINE void ZlixInflateMatchCopy8WithOverlay(uint8_t *dst, uint8_t *match, uint32_t length,
    uint32_t offset)
{
    uint32_t i = 0;
    uint8_t *dstEnd;

    for (; i < 8; i++) { /* 直接先写8个，此处需要一个一个字节写 */
        dst[i] = match[i];
    }

    if (length <= 8) { /* 长度小于8则已拷贝完毕 */
        return;
    }

    dstEnd = dst + length;
    if (offset < 8) { /* offset < 8 说明重叠了，需要根据offset前进到相同的位置 */
        match += g_overlapOffset[offset];
        dst += 8; /* 拷贝后指针8字节偏移 */
    }

    do {
        Copy8Byte(dst, match);
        dst += 8; /* 拷贝后指针8字节偏移 */
        match += 8; /* 拷贝后指针8字节偏移 */
    } while (dst < dstEnd);
}

ZLIX_STATIC ZLIX_ALWAYS_INLINE int32_t ZlixInflateMatchCopy(ZLIX_INFLATE_STATE *interState, uint32_t length,
    uint32_t offset, uint32_t wild, uint32_t pending, uint8_t *out)
{
    uint32_t winfet = 0;
    ZlixStream *stream = interState->stream;
    uint32_t temp = length;
    uint8_t *nextOut = out;
    uint8_t *startOut;
    uint8_t *startWindow;

    if (pending >= offset) { /* 全在缓冲区 */
        startOut = nextOut - offset;
        if (ZLIX_LIKELY(wild != 0)) {
            if ((offset >= 16) || (temp <= offset)) { /* 偏移大于16时一次性拷贝16字节就不会有内存重叠问题 */
                ZlixWildCopy16(startOut, nextOut, nextOut + temp);
            } else {
                ZlixInflateMatchCopy8WithOverlay(nextOut, startOut, temp, offset);
            }
        } else {
            ZLIX_MATCH_COPY_FROM_THREE_TO_ONE_BYTES(nextOut, temp, startOut);
        }
    } else if ((int32_t)(offset - pending) >= (int32_t)length) { /* 全在滑窗 */
        if (ZLIX_UNLIKELY((offset - pending) > interState->whave)) {
            return ZLIX_DATA_INVAID;
        }
        winfet = temp;
        startWindow = &(interState->window[interState->whave - (offset - pending)]);
        ZLIX_MATCH_COPY_FROM_THREE_TO_ONE_BYTES(nextOut, winfet, startWindow);
    } else { /* 部分在滑窗，部分在输出缓冲区 */
        if (ZLIX_UNLIKELY((offset - pending) > interState->whave)) {
            return ZLIX_DATA_INVAID;
        }
        winfet = offset - pending;
        temp -= winfet;
        startWindow = &(interState->window[interState->whave - (offset - pending)]);
        startOut = stream->nextOut;
        ZLIX_MATCH_COPY_FROM_THREE_TO_ONE_BYTES(nextOut, winfet, startWindow);
        ZLIX_MATCH_COPY_FROM_THREE_TO_ONE_BYTES(nextOut, temp, startOut);
    }
    return ZLIX_OK;
}

ZLIX_STATIC ZLIX_ALWAYS_INLINE int32_t ZlixInflateMatchCopyPending(ZLIX_INFLATE_STATE *interState, uint32_t length,
    uint32_t offset, uint32_t wild)
{
    int32_t ret = ZlixInflateMatchCopy(interState, length, offset, wild,
        interState->pending, &(interState->stream->nextOut[interState->pending]));
    if (ret == ZLIX_OK) {
        interState->pending += length;
    }
    return ret;
}

ZLIX_STATIC ZLIX_ALWAYS_INLINE int32_t ZlixInflateMatchCopyCurout(ZLIX_INFLATE_STATE *interState, uint32_t length,
    uint32_t offset, const uint8_t *cur, uint8_t **curOutP)
{
    uint8_t *curOut = *curOutP;
    ZlixStream *stream = interState->stream;
    uint32_t pending = curOut - stream->nextOut;

    int32_t ret = ZlixInflateMatchCopy(interState, length, offset, 1, pending, curOut);
    if (ret == ZLIX_OK) {
        *curOutP += length;
    } else {
        interState->readsofar = cur - stream->nextIn;
        interState->pending = curOut - stream->nextOut;
    }
    return ret;
}

ZLIX_STATIC int32_t ZlixInflateLenDisFastDecode(ZLIX_INFLATE_STATE *interState, int32_t flushType)
{
    uint32_t lmask, dmask;
    uint32_t readVal, lenbits, length, offset;
    register uint32_t hold, bits;
    CODE_TABLE_INFO *tableInfo, *secRoot;
    CODE_TABLE_INFO *lcodes = interState->lcodes;
    CODE_TABLE_INFO *dcodes = interState->dcodes;

    ZlixStream *stream = interState->stream;
    const uint8_t *cur = stream->nextIn + interState->readsofar;
    const uint8_t *end = stream->nextIn + stream->availIn - 6;  /* 单个lit和各一个len加offset符号的长度都不会超过6字节 */
    uint8_t *curOut = stream->nextOut + interState->pending;
    uint8_t *endOut = stream->nextOut + stream->availOut - 274; /* 单次匹配的最大长度为258，加上16字节拷贝缓冲，一共274字节 */

    (void)flushType;

    lmask = (1 << interState->lthres) - 1;
    dmask = (1 << interState->dthres) - 1;
    hold = interState->hold;
    bits = interState->bits;

    while ((cur <= end) && (curOut <= endOut)) {
        ZLIX_FAST_GET_TWO_BYTE(hold, bits, cur);

        readVal = hold & lmask;
        tableInfo = lcodes + readVal;

        if (ZLIX_UNLIKELY((tableInfo->exBits != 0) && (tableInfo->op == 0))) {
            secRoot = tableInfo;
            lenbits = secRoot->bits + secRoot->exBits;
            readVal = ZLIX_READ_BITS(hold, lenbits);
            tableInfo = lcodes + secRoot->rangebase + (readVal >> secRoot->bits);
            ZLIX_DROP_BITS(hold, bits, secRoot->bits + tableInfo->bits);
        } else {
            ZLIX_DROP_BITS(hold, bits, tableInfo->bits);
        }

        if (tableInfo->op == 0) {
            if (ZLIX_LIKELY(tableInfo->rangebase != 256)) {  /* 流结束标志符号位为256 */
                *curOut++ = (uint8_t)tableInfo->rangebase;
                continue;
            }
            interState->mode = ZLIX_INFLATE_TYPE;
            interState->hold = hold;
            interState->bits = bits;
            interState->readsofar = cur - stream->nextIn;
            interState->pending = curOut - stream->nextOut;
            return ZLIX_INFLATE_NOT_LEAVE;
        }

        length = tableInfo->rangebase;
        if (tableInfo->exBits != 0) {
            ZLIX_FAST_GET_TWO_BYTE(hold, bits, cur);
            length += ZLIX_READ_BITS(hold, tableInfo->exBits);
            ZLIX_DROP_BITS(hold, bits, tableInfo->exBits);
        }

        ZLIX_FAST_GET_TWO_BYTE(hold, bits, cur);

        readVal = hold & dmask;
        tableInfo = dcodes + readVal;

        if ((tableInfo->op == 0) && (tableInfo->exBits != 0)) {
            secRoot = tableInfo;
            lenbits = secRoot->bits + secRoot->exBits;
            readVal = ZLIX_READ_BITS(hold, lenbits);
            tableInfo = dcodes + secRoot->rangebase + (readVal >> secRoot->bits);
            ZLIX_DROP_BITS(hold, bits, secRoot->bits);
        }
        ZLIX_DROP_BITS(hold, bits, tableInfo->bits);

        offset = tableInfo->rangebase;
        if (tableInfo->exBits != 0) {
            ZLIX_FAST_GET_TWO_BYTE(hold, bits, cur);
            offset += ZLIX_READ_BITS(hold, tableInfo->exBits);
            ZLIX_DROP_BITS(hold, bits, tableInfo->exBits);
        }

        ZLIX_CHECK_DATA_AND_RETURN(
            ZlixInflateMatchCopyCurout(interState, length, offset, cur, &curOut) != ZLIX_OK, interState);
    }

    interState->hold = hold;
    interState->bits = bits;

    interState->mode = ZLIX_INFLATE_LEN;
    interState->readsofar = cur - stream->nextIn;
    interState->pending = curOut - stream->nextOut;
    return ZLIX_INFLATE_NOT_LEAVE;
}

ZLIX_STATIC int32_t ZlixInflateLenDisDecode(ZLIX_INFLATE_STATE *interState, int32_t flushType)
{
    uint32_t thresl;
    uint32_t lenbits;
    uint32_t readVal;
    CODE_TABLE_INFO temp, secRoot;
    CODE_TABLE_INFO *code;
    ZlixStream *streamInfo = interState->stream;

    (void)flushType;

    if (interState->mode == ZLIX_INFLATE_LEN) {
        thresl = interState->lthres;
        code = interState->lcodes;
    } else {
        thresl = interState->dthres;
        code = interState->dcodes;
    }

    while (1) {
        readVal = ZLIX_READ_BITS(interState->hold, thresl);
        temp = code[readVal];
        if (temp.bits <= interState->bits) {
            break;
        }
        PUT_BYTE_RETURN_IF_INPUTOVER(interState, streamInfo);
    }

    if ((temp.exBits != 0) && (temp.op == 0)) {
        secRoot = temp;
        while (1) {
            lenbits = secRoot.bits + secRoot.exBits;

            readVal = ZLIX_READ_BITS(interState->hold, lenbits);
            temp = code[secRoot.rangebase + (readVal >> secRoot.bits)];
            if ((secRoot.bits + temp.bits) <= interState->bits) {
                break;
            }
            PUT_BYTE_RETURN_IF_INPUTOVER(interState, streamInfo);
        }
        ZLIX_DROP_BITS(interState->hold, interState->bits, secRoot.bits);
    }

    ZLIX_DROP_BITS(interState->hold, interState->bits, temp.bits);

    /* 这个分支是：解析lit中的literal */
    if (temp.op == 0) {
        /* 256表示结束符 */
        if (temp.rangebase == 256) {
            interState->mode = ZLIX_INFLATE_TYPE;
            return ZLIX_INFLATE_NOT_LEAVE;
        }
        return ZlixInflateLiteralWrite(&temp, interState);
    }
    return ZlixInflateLenDistRecord(&temp, interState);
}

ZLIX_STATIC int32_t ZlixInflateLenExtProc(ZLIX_INFLATE_STATE *interState, int32_t flushType)
{
    uint32_t val;
    ZLIX_INFLATE_STATE *state = interState;
    ZlixStream *streamInfo = state->stream;

    READ_BITS_RETURN_IF_INPUTOVER(state, state->extra, streamInfo, val);

    state->length += (uint16_t)val;
    state->mode = ZLIX_INFLATE_DIST;
    (void)flushType;
    return ZLIX_INFLATE_NOT_LEAVE;
}

ZLIX_STATIC int32_t ZlixInflateDisExtProc(ZLIX_INFLATE_STATE *interState, int32_t flushType)
{
    uint16_t val;
    ZLIX_INFLATE_STATE *state = interState;
    ZlixStream *streamInfo = state->stream;

    READ_BITS_RETURN_IF_INPUTOVER(state, state->extra, streamInfo, val);

    state->offset += val;
    state->mode = ZLIX_INFLATE_MATCH;

    (void)flushType;
    return ZLIX_INFLATE_NOT_LEAVE;
}
ZLIX_STATIC int32_t ZlixInflateMatchProc(ZLIX_INFLATE_STATE *interState, int32_t flushType)
{
    ZLIX_INFLATE_STATE *state = interState;
    ZlixStream *streamInfo = state->stream;
    uint32_t fetch = streamInfo->availOut - state->pending;

    (void)flushType;

    /* match开头检查缓冲区是否为0，如果为0，直接转到leave */
    if (fetch == 0) {
        return ZLIX_INFLATE_LEAVE;
    }

    fetch = (fetch < state->length) ? fetch : state->length; /* 取当前能复制的长度 */
    state->length = state->length - (uint16_t)fetch;

    ZLIX_CHECK_DATA_AND_RETURN(ZlixInflateMatchCopyPending(interState, fetch, state->offset, 0) != ZLIX_OK, state);

    if (state->length == 0) {
        state->mode = ZLIX_INFLATE_LEN;
        return ZLIX_INFLATE_NOT_LEAVE;
    }

    /* 输出缓冲区不够 */
    return ZLIX_INFLATE_LEAVE;
}

ZLIX_STATIC int32_t ZlixInflateLitState(ZLIX_INFLATE_STATE *interState, int32_t flushType)
{
    ZlixStream *streamInfo = interState->stream;
    (void)flushType;

    ZLIX_CHECK_DATA_AND_RETURN(interState->pending >= streamInfo->availOut, interState); /* 流式情况下此处pending为0 */

    streamInfo->nextOut[interState->pending++] = (uint8_t)interState->length;
    interState->mode = ZLIX_INFLATE_LEN;
    return ZLIX_INFLATE_NOT_LEAVE;
}

ZLIX_STATIC int32_t ZlixInflateCheckState(ZLIX_INFLATE_STATE *interState, int32_t flushType)
{
    ZlixStream *streamInfo = interState->stream;
    uint32_t checkcrc;
    uint32_t out;
    uint8_t *start;

    (void)flushType;

    READ_BITS_RETURN_IF_INPUTOVER(interState, 32, streamInfo, checkcrc);
    out = interState->pending;
    start = streamInfo->nextOut;
    streamInfo->totalOut += out;
    streamInfo->nextOut += out;
    streamInfo->availOut -= out;
    interState->pending = 0;
    if (ZLIX_WRAP_VALID(interState) != 0) { // 这里去掉了if (out > 0), 0字节文件也支持adler校验
        if (interState->flags != 0) { // gzip
            (void)ZlixUtilsCrc32Cal(start, out, &interState->check);
            ZLIX_CHECK_DATA_AND_RETURN(checkcrc != interState->check, interState);
        } else { // zlib
            return ZLIX_ZLIB_ADLER_CHECK(start, out, interState, checkcrc);
        }
    }
    interState->mode = ZLIX_INFLATE_LENGTH;
    return ZLIX_INFLATE_NOT_LEAVE;
}

ZLIX_STATIC int32_t ZlixInflateLengthState(ZLIX_INFLATE_STATE *interState, int32_t flushType)
{
    ZlixStream *streamInfo = interState->stream;
    uint32_t total;
    uint32_t checkTotal = (uint32_t)streamInfo->totalOut;

    (void)flushType;

    READ_BITS_RETURN_IF_INPUTOVER(interState, 32, streamInfo, total);
    ZLIX_CHECK_DATA_AND_RETURN(total != checkTotal, interState);

    interState->mode = ZLIX_INFLATE_DONE;
    return ZLIX_INFLATE_NOT_LEAVE;
}

ZLIX_STATIC int32_t ZlixInflateDoneBuf(ZLIX_INFLATE_STATE *interState, int32_t flushType)
{
    /* 撤回之前额外读取到缓冲区的内容，以防干扰到下一次解压 */
    ZLIX_WITHDRAW_BYTES(interState->hold, interState->bits, interState->readsofar);
    interState->error = ZLIX_STREAM_END;
    (void)flushType;
    return ZLIX_INFLATE_LEAVE;
}

ZLIX_STATIC int32_t ZlixInflateBadBuf(ZLIX_INFLATE_STATE *interState, int32_t flushType)
{
    (void)interState;
    (void)flushType;
    interState->error = ZLIX_DATA_INVAID;
    return ZLIX_INFLATE_LEAVE;
}

ZLIX_STATIC int32_t ZlixInflateMemErrBuf(ZLIX_INFLATE_STATE *interState, int32_t flushType)
{
    (void)interState;
    (void)flushType;
    return ZLIX_INFLATE_LEAVE;
}

ZLIX_STATIC ZLIX_ALWAYS_INLINE void ZlixFreshStreamInfo(ZlixStream *streamInfo, ZLIX_INFLATE_STATE *state)
{
    streamInfo->nextOut += state->pending;
    streamInfo->availOut -= state->pending;
    streamInfo->totalOut += state->pending;
    streamInfo->nextIn += state->readsofar;
    streamInfo->availIn -= state->readsofar;
    streamInfo->totalIn += state->readsofar;
}

ZLIX_STATIC int ZlixInflateWindowCopy(ZLIX_INFLATE_STATE *state, ZlixStream *streamInfo)
{
    int err;
    uint8_t *dst;
    uint8_t *src;
    uint32_t dstSize;
    uint32_t srcSize;
    uint32_t left;

    /* 输出大于wsize窗口全量大小 */
    if (state->pending >= state->wsize) {
        /* nextOut要从最新的开始复制 */
        dst = state->window;
        dstSize = state->wsize;
        src = streamInfo->nextOut + (state->pending - state->wsize);
        srcSize = state->wsize;
    } else if (state->pending <= (state->wsize - state->whave)) {
        /* 输出小于wsize窗口剩余大小 */
        dst = state->window + state->whave;
        dstSize = state->wsize - state->whave;
        src = streamInfo->nextOut;
        srcSize = state->pending;
    } else {
        /* 滑窗内需要保存一部分，再从缓冲区拷贝当前最新输出到滑窗中 */
        left = state->whave - (state->wsize - state->pending);
        err = (memmove_s(state->window, state->wsize, state->window + left, state->wsize - state->pending) == EOK) ?
            ZLIX_OK : ZLIX_SEC_FUNC_FAILURE;
        if (err != ZLIX_OK) {
            return ZLIX_MEM_OPER_FAILURE;
        }
        dst = state->window + state->wsize - state->pending;
        dstSize = state->pending;
        src = streamInfo->nextOut;
        srcSize = state->pending;
    }
    if (memcpy_s(dst, dstSize, src, srcSize) != EOK) {
        return ZLIX_MEM_OPER_FAILURE;
    }
    state->whave += state->pending;
    if (state->whave > state->wsize) {
        state->whave = state->wsize;
    }
    return ZLIX_OK;
}

ZLIX_STATIC int32_t ZlixInflateLeave(ZLIX_INFLATE_STATE *interState, int32_t flushType)
{
    ZLIX_INFLATE_STATE *state = interState;
    ZlixStream *streamInfo = state->stream;

    /* 复制state->wsize或更小的输出字节到窗口中 */
    if ((state->pending != 0) && (state->mode < ZLIX_INFLATE_BAD) &&
        ((state->mode < ZLIX_INFLATE_CHECK) || (flushType != ZLIX_FINISH))) {
        if (state->window == NULL) {
            state->window = (uint8_t *)ZLIX_LABEL_MALLOC(state->zlixMemOper,
                ZLIX_INFLATE_WINDLE_LABEL, (1 << state->wbits) * sizeof(uint8_t));
            if (state->window == NULL) {
                state->mode = ZLIX_INFLATE_MEM;
                return ZLIX_MEM_OPER_FAILURE;
            }
            state->whave = 0;
        }

        if (state->wsize == 0) {
            state->wsize = 1 << state->wbits;
            state->whave = 0;
        }

        if (ZlixInflateWindowCopy(state, streamInfo) != ZLIX_OK) {
            state->mode = ZLIX_INFLATE_MEM;
            return ZLIX_MEM_OPER_FAILURE;
        }
    }

    if ((ZLIX_WRAP_VALID(interState) != 0) && (state->pending > 0)) {
        if (interState->flags != 0) { // gzip
            (void)ZlixUtilsCrc32Cal(streamInfo->nextOut, state->pending, &interState->check);
        } else { // zlib
            interState->check = ZlixUtilsAdler32Cal(interState->check, streamInfo->nextOut, state->pending);
        }
    }
    streamInfo->adler = interState->check;

    /* 需要更新 next_out和next_in */
    ZlixFreshStreamInfo(streamInfo, state);

    if ((((state->pending == 0) && (state->readsofar == 0)) || flushType == ZLIX_FINISH) &&
        (state->error == ZLIX_OK)) {
        state->error = ZLIX_OUTPUT_BUF_INSUFFICENT;
    }

    state->readsofar = 0;
    state->pending = 0;
    return state->error;
}

int32_t ZlixInflate(int32_t flushType, ZlixStream *streamInfo)
{
    int32_t needleave = 0;
    ZLIX_INFLATE_STATE *interState;

    if ((streamInfo == NULL) || (streamInfo->context == NULL)) {
        return ZLIX_STREAM_ERROR;
    }

    interState = (ZLIX_INFLATE_STATE *)(streamInfo->context);
    interState->error = ZLIX_OK;

    if ((streamInfo != interState->stream) || (flushType < 0) || (flushType > ZLIX_TREES)) {
        return ZLIX_STREAM_ERROR;
    }

    /* 调用Z_FINISH后，不能再设置其他解压选项 */
    if (((streamInfo->nextIn == 0) && (streamInfo->availIn != 0)) ||
        ((interState->mode < ZLIX_INFLATE_HEAD) || (interState->mode > ZLIX_INFLATE_MEM))) {
        return ZLIX_STREAM_ERROR;
    }

    if (interState->mode == ZLIX_INFLATE_DONE) {
        return 1;
    }

     /* 输入的输出缓冲区为空 */
    if (streamInfo->nextOut == 0) {
        return ZLIX_STREAM_ERROR;
    }

    /* ZLIX_TREES或者ZLIX_BLOCK模式下需要按块进行解压 */
    if (interState->mode == ZLIX_INFLATE_TYPE) {
        interState->mode = ZLIX_INFLATE_TYPEDO;
    }

    /* 不断切换功能，直到needleave为1 */
    while (needleave == 0) {
        needleave = g_statusFunc[interState->mode](interState, flushType);
    }

    return ZlixInflateLeave(interState, flushType);
}

void ZlixInflateStatusFuncReg(void)
{
    g_statusFunc[ZLIX_INFLATE_HEAD] = ZlixInflateHeadProc;
    g_statusFunc[ZLIX_INFLATE_FLAGS] = ZlixInflateFlagsProc;
    g_statusFunc[ZLIX_INFLATE_TIME] = ZlixInflateTimeProc;
    g_statusFunc[ZLIX_INFLATE_OS] = ZlixInflateOsProc;
    g_statusFunc[ZLIX_INFLATE_EXLEN] = ZlixInflateExlenProc;
    g_statusFunc[ZLIX_INFLATE_EXTRA] = ZlixInflateExtraProc;
    g_statusFunc[ZLIX_INFLATE_NAME] = ZlixInflateNameCommentProc;
    g_statusFunc[ZLIX_INFLATE_COMMENT] = ZlixInflateNameCommentProc;
    g_statusFunc[ZLIX_INFLATE_HCRC] = ZlixInflateHcrcProc;
    g_statusFunc[ZLIX_INFLATE_TYPE] = ZlixInflateTypeProc;
    g_statusFunc[ZLIX_INFLATE_TYPEDO] = ZlixInflateTypeDoProc;
    g_statusFunc[ZLIX_INFLATE_TABLELEN] = ZlixInflateTableLenProc;
    g_statusFunc[ZLIX_INFLATE_TABLECCL] = ZlixInflateTableCclProc;
    g_statusFunc[ZLIX_INFLATE_CODECL] = ZlixInflateTableClArrayProc;
    g_statusFunc[ZLIX_INFLATE_LEN_FAST] = ZlixInflateLenDisFastDecode;
    g_statusFunc[ZLIX_INFLATE_LEN] = ZlixInflateLenDisDecode;
    g_statusFunc[ZLIX_INFLATE_LENEXT] = ZlixInflateLenExtProc;
    g_statusFunc[ZLIX_INFLATE_DIST] = ZlixInflateLenDisDecode;
    g_statusFunc[ZLIX_INFLATE_DISTEXT] = ZlixInflateDisExtProc;
    g_statusFunc[ZLIX_INFLATE_MATCH] = ZlixInflateMatchProc;
    g_statusFunc[ZLIX_INFLATE_LIT] = ZlixInflateLitState;
    g_statusFunc[ZLIX_INFLATE_CHECK] = ZlixInflateCheckState;
    g_statusFunc[ZLIX_INFLATE_LENGTH] = ZlixInflateLengthState;
    g_statusFunc[ZLIX_INFLATE_DONE] = ZlixInflateDoneBuf;
    g_statusFunc[ZLIX_INFLATE_BAD] = ZlixInflateBadBuf;
    g_statusFunc[ZLIX_INFLATE_MEM] = ZlixInflateMemErrBuf;
    g_statusFunc[ZLIX_INFLATE_STORED] = ZlixInflateStoredLenProc;
    g_statusFunc[ZLIX_INFLATE_COPY] = ZlixInflateStoredProc;
}

void ZlixInflateInterStateInit(ZLIX_INFLATE_STATE *interState)
{
    interState->window = NULL;
    interState->wsize = 0;
    interState->mode = ZLIX_INFLATE_HEAD;
    interState->bits = 0;
    interState->readsofar = 0;
    interState->pending = 0;
    interState->length = 0;
    interState->last = 0;
}

int32_t ZlixInflateInit(const ZlixConfigInfo *usrConfigInfo, ZlixStream *streamInfo)
{
    ZlixMemFuncs zlixMemOper;
    ZLIX_INFLATE_STATE *interState;
    int32_t winBits;
    int32_t ret;

    if (streamInfo == 0) {
        return ZLIX_STREAM_ERROR;
    }

    ret = ZlixMemFuncsCheck(usrConfigInfo, &zlixMemOper);
    if (ret != ZLIX_OK) {
        return ret;
    }

    interState = (ZLIX_INFLATE_STATE *)ZLIX_LABEL_MALLOC(zlixMemOper,
        ZLIX_INFLATE_INIT_LABEL, sizeof(ZLIX_INFLATE_STATE));
    if (interState == NULL) {
        return ZLIX_MEM_OPER_FAILURE;
    }
    streamInfo->method = usrConfigInfo->method;
    (void)memset_s(interState, sizeof(ZLIX_INFLATE_STATE), 0, sizeof(ZLIX_INFLATE_STATE));
    streamInfo->context = interState;
    interState->zlixMemOper = zlixMemOper;
    interState->stream = streamInfo;
    winBits = usrConfigInfo->winBits;

    interState->wrap = 1;
    if (winBits < 0) {
        interState->wrap = 0;
        interState->wbits = (uint32_t)(-winBits);
    } else {
        interState->wrap = ((uint32_t)winBits >> 4) + 5; /* 计算wrap值用于头格式操作，和开源计算方式一致，右移4，再增加5 */
        interState->wbits =
            (winBits >= 48) ? (uint32_t)winBits : ((uint32_t)winBits & 0xf); /* 如果大于48，需要通过掩码0xf进行缩小 */
    }
    if (((interState->wbits > ZLIX_INFLATE_WINBITS_HIGH) || (interState->wbits < ZLIX_INFLATE_WINBITS_LOW)) &&
        (interState->wbits != 0)) {
        ZLIX_LABEL_FREE(zlixMemOper, ZLIX_INFLATE_INIT_LABEL, interState);
        return ZLIX_INPUT_PARA_INVALID;
    }
    ZlixInflateInterStateInit(interState);
    interState->stream = streamInfo;

    /* 考虑到对枚举的顺序产生依赖，此处进行赋值 */
    ZlixInflateStatusFuncReg();

    return ZLIX_OK;
}

int32_t ZlixInflateFini(ZlixStream *streamInfo)
{
    ZLIX_INFLATE_STATE *interState = (ZLIX_INFLATE_STATE *)(streamInfo->context);

    if (interState != NULL) {
        ZlixMemFuncs zlixMemOper = interState->zlixMemOper;
        if (interState->window != NULL) {
            ZLIX_LABEL_FREE(zlixMemOper, ZLIX_INFLATE_WINDLE_LABEL, interState->window);
            interState->window = NULL;
        }
        ZLIX_LABEL_FREE(zlixMemOper, ZLIX_INFLATE_INIT_LABEL, interState);
        streamInfo->context = NULL;
    }
    return ZLIX_OK;
}

CONSTRUCTOR void ZlixInflateModeInit(void)
{
    ZlixStreamOperFuncs *zlixStreamOprTbl = ZlixStreamOperTblGet();
    zlixStreamOprTbl[ZLIX_DEFLATE].decodeResInit = ZlixInflateInit;
    zlixStreamOprTbl[ZLIX_DEFLATE].decodeResReset = NULL;
    zlixStreamOprTbl[ZLIX_DEFLATE].decodeStream = ZlixInflate;
    zlixStreamOprTbl[ZLIX_DEFLATE].decodeResFini = ZlixInflateFini;
}
