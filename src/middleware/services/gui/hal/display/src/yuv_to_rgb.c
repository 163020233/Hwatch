/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023. All rights reserved.
 * Description: yuv to rgb
 */

#include "yuv_to_rgb.h"

#define GET1BYTESONCE            1
#define GET4BYTESONCE            4
#define GET8BYTESONCE            8
#define MAXLIMITTABLE            255
#define FIX(value)               ((int)(value))
#define WRITE3BYTES(dst, src)    ((dst) = (src))
#define WRITE12BYTES(dst, src)   ((dst) = (src))
#define UNUSE_FLOAT

typedef struct {
    td_u8 B00;
    td_u8 B01;
    td_u8 B02;
} Write3BytesBuf;

typedef struct {
    td_u8 B00;
    td_u8 B01;
    td_u8 B02;
    td_u8 B10;
    td_u8 B11;
    td_u8 B12;
    td_u8 B20;
    td_u8 B21;
    td_u8 B22;
    td_u8 B30;
    td_u8 B31;
    td_u8 B32;
} Write12BytesBuf;

static const td_uchar LIMIT_TABLE[1408] = {0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
    17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
    33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
    49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64,
    65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
    81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96,
    97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112,
    113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128,
    129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144,
    145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160,
    161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176,
    177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192,
    193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208,
    209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224,
    225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240,
    241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
    17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
    33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
    49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64,
    65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
    81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96,
    97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112,
    113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127
};

/* should the same as dpu coef */
static inline td_void YuvToRgb(td_u8 *mem, const td_uchar *limitTable, td_s32 y, td_s32 cr, td_s32 cb)
{
#ifndef UNUSE_FLOAT
    td_s32 r = (td_s32)((((y << 2) - 64) + 1.5391f * ((cr << 2) - 512)));
    td_s32 g = (td_s32)((((y << 2) - 64) - 0.1826f * ((cb << 2) - 512) - 0.4590f * ((cr << 2) - 512)));
    td_s32 b = (td_s32)((((y << 2) - 64) + 1.8154f * ((cb << 2) - 512)));
#else
    td_s32 r = (td_s32)(((((y << 2) - 64) * 1024) + (((cr << 2) - 512) * 1576)) >> 10) >> 2;
    td_s32 g = (td_s32)(((((y << 2) - 64) * 1024) - (((cb << 2) - 512) * 187) - (((cr << 2) - 512) * 470)) >> 10) >> 2;
    td_s32 b = (td_s32)(((((y << 2) - 64) * 1024) + (((cb << 2) - 512) * 1859)) >> 10) >> 2;
#endif
    mem[0] = limitTable[b]; /* 0: b */
    mem[1] = limitTable[g]; /* 1: g */
    mem[2] = limitTable[r]; /* 2: r */
}

/*
 * y_row_col(uv_row_col)
 * y_0_0(uv_0_1)  y_0_1(uv_0_1) y_0_2(uv_2_3)  y_0_3(uv_2_3)
 * y_1_0(uv_0_1)  y_1_1(uv_0_1) y_1_2(uv_2_3)  y_1_3(uv_2_3)
 */
td_s32 CscConvertYuvToRgb(CscRgbBuf *dst, CscYuvBuf *src)
{
    td_uchar *limitTable = (td_uchar *)LIMIT_TABLE;
    td_uchar *rangeLimitTable = (td_uchar *)(limitTable + MAXLIMITTABLE + 1);

    Write3BytesBuf w3Buf = { 0 };
    Write12BytesBuf w12Buf = { 0 };

    td_u16 maxW = src->width - GET4BYTESONCE;
    td_u32 *yArr  = (td_u32 *)(src->yAddr);
    td_u32 *uvArr = (td_u32 *)(src->uvAddr);

    for (td_u16 i = 0; i < src->height; i += 2) { /* 2: read 2 line */
        td_u8 *dstRow0 = (td_u8 *)(dst->addr + dst->stride * i);
        td_u8 *dstRow1 = (td_u8 *)(dst->addr + dst->stride * (i + 1));
        td_u16 j = 0;

        for (j = 0; j < maxW; j += GET4BYTESONCE) {
            td_u8 *dstRow0Tmp = dstRow0 + j * 3;
            td_u8 *dstRow1Tmp = dstRow1 + j * 3;

            td_u32 yRow0Val   = yArr[i * src->yStride / GET4BYTESONCE + j / GET4BYTESONCE];
            td_u32 yRow1Val   = yArr[(i + 1) * src->yStride / GET4BYTESONCE + j / GET4BYTESONCE];
            td_u32 uvRow0Val = uvArr[i / 2 * src->uvStride / GET4BYTESONCE + (j / GET4BYTESONCE)];

            td_s32 uv00 = FIX(uvRow0Val & 0xff);
            td_s32 uv01 = FIX((uvRow0Val >> 8) & 0xff);
            td_s32 uv02 = FIX((uvRow0Val >> 16) & 0xff);
            td_s32 uv03 = FIX(uvRow0Val >> 24);

            /* ======================== for row0 ==================================*/
            td_s32 y00 = FIX(yRow0Val & 0xff);
            YuvToRgb((td_u8 *)(&w12Buf.B00), rangeLimitTable, y00, uv00, uv01);

            td_s32 y01 = FIX((yRow0Val >> 8) & 0xff);
            YuvToRgb((td_u8 *)(&w12Buf.B10), rangeLimitTable, y01, uv00, uv01);

            td_s32 y02 = FIX((yRow0Val >> 16) & 0xff);
            YuvToRgb((td_u8 *)(&w12Buf.B20), rangeLimitTable, y02, uv02, uv03);

            td_s32 y03 = FIX(yRow0Val >> 24);
            YuvToRgb((td_u8 *)(&w12Buf.B30), rangeLimitTable, y03, uv02, uv03);

            WRITE12BYTES(*((Write12BytesBuf *)dstRow0Tmp), *((Write12BytesBuf *)(&w12Buf.B00)));

            /* ======================== for row1 ==================================*/
            td_s32 y10 = FIX(yRow1Val & 0xff);
            YuvToRgb((td_u8 *)(&w12Buf.B00), rangeLimitTable, y10, uv00, uv01);

            td_s32 y11 = FIX((yRow1Val >> 8) & 0xff);
            YuvToRgb((td_u8 *)(&w12Buf.B10), rangeLimitTable, y11, uv00, uv01);

            td_s32 y12 = FIX((yRow1Val>> 16) & 0xff);
            YuvToRgb((td_u8 *)(&w12Buf.B20), rangeLimitTable, y12, uv02, uv03);

            td_s32 y13 = FIX(yRow1Val >> 24);
            YuvToRgb((td_u8 *)(&w12Buf.B30), rangeLimitTable, y13, uv02, uv03);

            WRITE12BYTES(*((Write12BytesBuf *)dstRow1Tmp), *((Write12BytesBuf *)(&w12Buf.B00)));
        }

        for (td_u16 k = j; k < src->width; k++) {
            td_u8 *dstRow0Tmp = dstRow0 + k * 3;
            td_u8 *dstRow1Tmp = dstRow1 + k * 3;

            td_s32 y = FIX(src->yAddr[i * src->yStride + k]);
            td_s32 cb = FIX(src->uvAddr[(i / 2) * src->uvStride + (k / 2) * 2]);
            td_s32 cr = FIX(src->uvAddr[(i / 2) * src->uvStride + (k / 2) * 2 + 1]);

            /* ======================== for row0 ==================================*/
            YuvToRgb((td_u8 *)(&w3Buf.B00), rangeLimitTable, y, cb, cr);
            WRITE3BYTES(*((Write3BytesBuf *)dstRow0Tmp), *((Write3BytesBuf *)(&w3Buf.B00)));

            /* ======================== for row1 ==================================*/
            y = FIX(src->yAddr[(i + 1) * src->yStride + k]);
            YuvToRgb((td_u8 *)(&w3Buf.B00), rangeLimitTable, y, cb, cr);
            WRITE3BYTES(*((Write3BytesBuf *)dstRow1Tmp), *((Write3BytesBuf *)(&w3Buf.B00)));
        }
    }

    return 0;
}

/*
 * y_row_col(uv_row_col)
 * y_0_0(uv_0_1)  y_0_1(uv_0_1) y_0_2(uv_2_3)  y_0_3(uv_2_3)
 * y_1_0(uv_0_1)  y_1_1(uv_0_1) y_1_2(uv_2_3)  y_1_3(uv_2_3)
 */
td_s32 CscConvertYuvToRgbWithSkipLP(CscRgbBuf *dst, CscYuvBuf *src)
{
    td_uchar *limitTable = (td_uchar *)LIMIT_TABLE;
    td_uchar *rangeLimitTable = (td_uchar *)(limitTable + MAXLIMITTABLE + 1);

    Write3BytesBuf w3Buf = { 0 };
    Write12BytesBuf w12Buf = { 0 };

    td_u16 maxW = src->width - GET8BYTESONCE;
    td_u64 *yArr  = (td_u64 *)(src->yAddr);
    td_u64 *uvArr = (td_u64 *)(src->uvAddr);

    for (td_u16 i = 0; i < src->height; i += 2) { /* 2: read 2 line */
        td_u8 *dstRow0 = (td_u8 *)(dst->addr + dst->stride * (i / 2));
        td_u16 j = 0;
        for (j = 0; j < maxW; j += GET8BYTESONCE) {
            td_u8 *dstRow0Tmp = dstRow0 + (j / 2) * 3;

            td_u64 yRow0Val  = yArr[i * src->yStride / GET8BYTESONCE + (j / GET8BYTESONCE)];
            td_u64 uvRow0Val = uvArr[(i / 2) * src->uvStride / GET8BYTESONCE + (j / GET8BYTESONCE)];

            /* ======================== for first 4 Bytes ==================================*/
            td_s32 uv00 = FIX(uvRow0Val & 0xff);
            td_s32 uv01 = FIX((uvRow0Val >> 8)  & 0xff);
            td_s32 uv02 = FIX((uvRow0Val >> 16) & 0xff);
            td_s32 uv03 = FIX((uvRow0Val >> 24) & 0xff);

            td_s32 y00 = FIX(yRow0Val & 0xff);
            YuvToRgb((td_u8 *)(&w12Buf.B00), rangeLimitTable, y00, uv00, uv01);

            td_s32 y02 = FIX((yRow0Val >> 16) & 0xff);
            YuvToRgb((td_u8 *)(&w12Buf.B10), rangeLimitTable, y02, uv02, uv03);

            /* ======================== for next 4 Bytes ==================================*/
            td_s32 uv04 = FIX((uvRow0Val >> 32) & 0xff);
            td_s32 uv05 = FIX((uvRow0Val >> 40) & 0xff);
            td_s32 uv06 = FIX((uvRow0Val >> 48) & 0xff);
            td_s32 uv07 = FIX(uvRow0Val >> 56);

            td_s32 y04 = FIX((yRow0Val >> 32) & 0xff);
            YuvToRgb((td_u8 *)(&w12Buf.B20), rangeLimitTable, y04, uv04, uv05);

            td_s32 y06 = FIX((yRow0Val >> 48) & 0xff);
            YuvToRgb((td_u8 *)(&w12Buf.B30), rangeLimitTable, y06, uv06, uv07);

            WRITE12BYTES(*((Write12BytesBuf *)dstRow0Tmp), *((Write12BytesBuf *)(&w12Buf.B00)));
        }

        for (td_u16 k = j; k < src->width; k += 2) { /* 2: read 2 line */
            td_u8 *dstRow0Tmp = dstRow0 + (k / 2) * 3;

            td_s32 y  = FIX(src->yAddr[i * src->yStride + k]);
            td_s32 cb = FIX(src->uvAddr[(i / 2) * src->uvStride + (k / 2) * 2]);
            td_s32 cr = FIX(src->uvAddr[(i / 2) * src->uvStride + (k / 2) * 2 + 1]);

            YuvToRgb((td_u8 *)(&w3Buf.B00), rangeLimitTable, y, cb, cr);
            WRITE3BYTES(*((Write3BytesBuf *)dstRow0Tmp), *((Write3BytesBuf *)(&w3Buf.B00)));
        }
    }

    return 0;
}
