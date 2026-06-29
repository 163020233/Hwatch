/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: Matrix multiplication file
 * Author: Huawei LiteOS Team
 * Create: 2024-04-14
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */

#include "libmat.h"


#ifndef LOSCFG_ARCH_ARM_SVE2
uint32_t matrix_mul_f32(float *matrixASrc, uint32_t aRowNum, uint32_t aColNum, float *matrixBSrc, uint32_t bRowNum,
                        uint32_t bColNum, float *matrixDst, uint32_t dRowNum, uint32_t dColNum)
{
    float *srcAData, *srcBData, *dstData;
    uint32_t curRow, curCol, curPos;

    if (matrixASrc == NULL || matrixBSrc == NULL || matrixDst == NULL) {
        return MATRIX_NOK;
    }

    if (aRowNum == 0 || aColNum == 0 || bRowNum == 0 || bColNum == 0 || dRowNum == 0 || dColNum == 0) {
        return MATRIX_NOK;
    }

    if (aColNum != bRowNum ||
        aRowNum != dRowNum ||
        bColNum != dColNum) {
        return MATRIX_SIZE_MISMATCH;
    }

    for (curRow = 0; curRow < aRowNum; curRow++) {
        for (curCol = 0; curCol < bColNum; curCol++) {
            srcAData = matrixASrc + curRow * aColNum;
            srcBData = matrixBSrc + curCol;
            dstData = GET_MATRIX_ELEM(matrixDst, dColNum, curRow, curCol);

            for (curPos = 0; curPos < aColNum; curPos++) {
                *dstData += (*srcAData) * (*srcBData);
                srcAData++;
                srcBData += bColNum;
            }
        }
    }

    return MATRIX_OK;
}

uint32_t matrix_mul_f64(double *matrixASrc, uint32_t aRowNum, uint32_t aColNum, double *matrixBSrc, uint32_t bRowNum,
                        uint32_t bColNum, double *matrixDst, uint32_t dRowNum, uint32_t dColNum)
{
    double *srcAData, *srcBData, *dstData;
    uint32_t curRow, curCol, curPos;

    if (matrixASrc == NULL || matrixBSrc == NULL || matrixDst == NULL) {
        return MATRIX_NOK;
    }

    if (aRowNum == 0 || aColNum == 0 || bRowNum == 0 || bColNum == 0 || dRowNum == 0 || dColNum == 0) {
        return MATRIX_NOK;
    }

    if (aColNum != bRowNum ||
        aRowNum != dRowNum ||
        bColNum != dColNum) {
        return MATRIX_SIZE_MISMATCH;
    }

    for (curRow = 0; curRow < aRowNum; curRow++) {
        for (curCol = 0; curCol < bColNum; curCol++) {
            srcAData = matrixASrc + curRow * aColNum;
            srcBData = matrixBSrc + curCol;
            dstData = GET_MATRIX_ELEM(matrixDst, dColNum, curRow, curCol);

            for (curPos = 0; curPos < aColNum; curPos++) {
                *dstData += (*srcAData) * (*srcBData);
                srcAData++;
                srcBData += bColNum;
            }
        }
    }

    return MATRIX_OK;
}
#else
#include "arm_sve.h"

uint32_t matrix_mul_f32(float *matrixASrc, uint32_t aRowNum, uint32_t aColNum, float *matrixBSrc, uint32_t bRowNum,
                        uint32_t bColNum, float *matrixDst, uint32_t dRowNum, uint32_t dColNum)
{
    float *opPosA;
    svfloat32_t svMatrixB, svMatrixC;
    uint32_t curRow, curCol, opRow;
    svbool_t pred;

    if (matrixASrc == NULL || matrixBSrc == NULL || matrixDst == NULL) {
        return MATRIX_NOK;
    }

    if (aRowNum == 0 || aColNum == 0 || bRowNum == 0 || bColNum == 0 || dRowNum == 0 || dColNum == 0) {
        return MATRIX_NOK;
    }

    if (aColNum != bRowNum ||
        aRowNum != dRowNum ||
        bColNum != dColNum) {
        return MATRIX_SIZE_MISMATCH;
    }

    for (curRow = 0; curRow < dRowNum; curRow++) {
        for (curCol = 0; curCol < dColNum; curCol += svcntw()) {
            svMatrixC = svdup_f32(0);
            opPosA = matrixASrc + curRow * aColNum;
            pred = svwhilelt_b32(curCol, dColNum);

            for (opRow = 0; opRow < bRowNum; opRow++) {
                svMatrixB = svld1(pred, GET_MATRIX_ELEM(matrixBSrc, bColNum, opRow, curCol));
                svMatrixC = svmla_m(pred, svMatrixC, svMatrixB, *opPosA);
                opPosA++;
            }
            svst1(pred, GET_MATRIX_ELEM(matrixDst, dColNum, curRow, curCol), svMatrixC);
        }
    }
    return MATRIX_OK;
}

uint32_t matrix_mul_f64(double *matrixASrc, uint32_t aRowNum, uint32_t aColNum, double *matrixBSrc, uint32_t bRowNum,
                        uint32_t bColNum, double *matrixDst, uint32_t dRowNum, uint32_t dColNum)
{
    double *opPosA;
    svfloat64_t svMatrixB, svMatrixC;
    uint32_t curRow, curCol, opRow;
    svbool_t pred;

    if (matrixASrc == NULL || matrixBSrc == NULL || matrixDst == NULL) {
        return MATRIX_NOK;
    }

    if (aRowNum == 0 || aColNum == 0 || bRowNum == 0 || bColNum == 0 || dRowNum == 0 || dColNum == 0) {
        return MATRIX_NOK;
    }

    if (aColNum != bRowNum ||
        aRowNum != dRowNum ||
        bColNum != dColNum) {
        return MATRIX_SIZE_MISMATCH;
    }

    for (curRow = 0; curRow < dRowNum; curRow++) {
        for (curCol = 0; curCol < dColNum; curCol += svcntd()) {
            svMatrixC = svdup_f64(0);
            opPosA = matrixASrc + curRow * aColNum;
            pred = svwhilelt_b64(curCol, dColNum);

            for (opRow = 0; opRow < bRowNum; opRow++) {
                svMatrixB = svld1(pred, GET_MATRIX_ELEM(matrixBSrc, bColNum, opRow, curCol));
                svMatrixC = svmla_m(pred, svMatrixC, svMatrixB, *opPosA);
                opPosA++;
            }

            svst1(pred, GET_MATRIX_ELEM(matrixDst, dColNum, curRow, curCol), svMatrixC);
        }
    }
    return MATRIX_OK;
}
#endif
