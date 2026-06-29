/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: Matrix inversion file
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
#include "stdlib.h"
#include "securec.h"

#ifndef LOSCFG_ARCH_ARM_SVE2
STATIC INLINE VOID matrixSwapRow_f32(float *matrix, uint32_t colNum, uint32_t startCol,
                                     uint32_t curRow, uint32_t selRow)
{
    float *curData = matrix;
    float *selData = matrix;
    const uint32_t swapNum = colNum - startCol;
    float tmpData = 0.0f;
    uint32_t opPos;

    curData += curRow * colNum + startCol;
    selData += selRow * colNum + startCol;
    for (opPos = 0; opPos < swapNum; opPos++) {
        tmpData = *curData;
        *curData++ = *selData;
        *selData++ = tmpData;
    }
}

STATIC INLINE VOID matrixSwapRow_f64(double *matrix, uint32_t colNum, uint32_t startCol,
                                     uint32_t curRow, uint32_t selRow)
{
    double *curData = matrix;
    double *selData = matrix;
    const uint32_t swapNum = colNum - startCol;
    double tmpData = 0.0;
    uint32_t opPos;

    curData += curRow * colNum + startCol;
    selData += selRow * colNum + startCol;
    for (opPos = 0; opPos < swapNum; opPos++) {
        tmpData = *curData;
        *curData++ = *selData;
        *selData++ = tmpData;
    }
}

STATIC INLINE VOID matrixScaleRow_f32(float *matrix, uint32_t colNum, uint32_t startCol,
                                      float pivot, uint32_t pivotRow)
{
    float *curData = matrix;
    const uint32_t pivotNum = colNum - startCol;
    uint32_t opPos;

    curData += pivotRow * colNum + startCol;
    for (opPos = 0; opPos < pivotNum; opPos++) {
        *curData++ *= pivot;
    }
}

STATIC INLINE VOID matrixScaleRow_f64(double *matrix, uint32_t colNum, uint32_t startCol,
                                      double pivot, uint32_t pivotRow)
{
    double *curData = matrix;
    const uint32_t pivotNum = colNum - startCol;
    uint32_t opPos;

    curData += pivotRow * colNum + startCol;
    for (opPos = 0; opPos < pivotNum; opPos++) {
        *curData++ *= pivot;
    }
}

STATIC INLINE VOID matrixMulSubRow_f32(float *matrix, uint32_t colNum, uint32_t startCol, float pivot,
                                       uint32_t curRow, uint32_t pivotRow)
{
    float *curData = matrix;
    float *pivotData = matrix;
    const uint32_t pivotNum = colNum - startCol;
    uint32_t opPos;

    curData += curRow * colNum + startCol;
    pivotData += pivotRow * colNum + startCol;
    for (opPos = 0; opPos < pivotNum; opPos++) {
        *curData++ -= pivot * (*pivotData++);
    }
}

STATIC INLINE VOID matrixMulSubRow_f64(double *matrix, uint32_t colNum, uint32_t startCol, double pivot,
                                       uint32_t curRow, uint32_t pivotRow)
{
    double *curData = matrix;
    double *pivotData = matrix;
    const uint32_t pivotNum = colNum - startCol;
    uint32_t opPos;

    curData += curRow * colNum + startCol;
    pivotData += pivotRow * colNum + startCol;
    for (opPos = 0; opPos < pivotNum; opPos++) {
        *curData++ -= pivot * (*pivotData++);
    }
}
#else
#include "arm_sve.h"

STATIC INLINE VOID matrixSwapRow_f32(float *matrix, uint32_t colNum, uint32_t startCol,
                                     uint32_t curRow, uint32_t selRow)
{
    uint32_t opPos;
    svfloat32_t svCur, svSel;
    svbool_t pred;

    for (opPos = startCol; opPos < colNum; opPos += svcntw()) {
        pred = svwhilelt_b32(opPos, colNum);

        svCur = svld1(pred, &matrix[curRow * colNum + opPos]);
        svSel = svld1(pred, &matrix[selRow * colNum + opPos]);
        svst1(pred, &matrix[curRow * colNum + opPos], svSel);
        svst1(pred, &matrix[selRow * colNum + opPos], svCur);
    }
}

STATIC INLINE VOID matrixSwapRow_f64(double *matrix, uint32_t colNum, uint32_t startCol,
                                     uint32_t curRow, uint32_t selRow)
{
    uint32_t opPos;
    svfloat64_t svCur, svSel;
    svbool_t pred;

    for (opPos = startCol; opPos < colNum; opPos += svcntd()) {
        pred = svwhilelt_b64(opPos, colNum);

        svCur = svld1(pred, &matrix[curRow * colNum + opPos]);
        svSel = svld1(pred, &matrix[selRow * colNum + opPos]);
        svst1(pred, &matrix[curRow * colNum + opPos], svSel);
        svst1(pred, &matrix[selRow * colNum + opPos], svCur);
    }
}

STATIC INLINE VOID matrixScaleRow_f32(float *matrix, uint32_t colNum, uint32_t startCol,
                                      float pivot, uint32_t pivotRow)
{
    uint32_t opPos;
    svfloat32_t svPivot = svdup_f32(pivot);
    svfloat32_t svCur;
    svbool_t pred;

    for (opPos = startCol; opPos < colNum; opPos += svcntw()) {
        pred = svwhilelt_b32(opPos, colNum);

        svCur = svld1(pred, &matrix[pivotRow * colNum + opPos]);
        svCur = svmul_m(pred, svCur, svPivot);
        svst1(pred, &matrix[pivotRow * colNum + opPos], svCur);
    }
}

STATIC INLINE VOID matrixScaleRow_f64(double *matrix, uint32_t colNum, uint32_t startCol,
                                      double pivot, uint32_t pivotRow)
{
    uint32_t opPos;
    svfloat64_t svPivot = svdup_f64(pivot);
    svfloat64_t svCur;
    svbool_t pred;

    for (opPos = startCol; opPos < colNum; opPos += svcntd()) {
        pred = svwhilelt_b64(opPos, colNum);

        svCur = svld1(pred, &matrix[pivotRow * colNum + opPos]);
        svCur = svmul_m(pred, svCur, svPivot);
        svst1(pred, &matrix[pivotRow * colNum + opPos], svCur);
    }
}

STATIC INLINE VOID matrixMulSubRow_f32(float *matrix, uint32_t colNum, uint32_t startCol, float pivot,
                                       uint32_t curRow, uint32_t pivotRow)
{
    uint32_t opPos;
    svfloat32_t svPivot = svdup_f32(pivot);
    svfloat32_t svCur, svBase;
    svbool_t pred;

    for (opPos = startCol; opPos < colNum; opPos += svcntw()) {
        pred = svwhilelt_b32(opPos, colNum);

        svCur = svld1(pred, &matrix[curRow * colNum + opPos]);
        svBase = svld1(pred, &matrix[pivotRow * colNum + opPos]);
        svCur = svmls_m(pred, svCur, svBase, svPivot);
        svst1(pred, &matrix[curRow * colNum + opPos], svCur);
    }
}

STATIC INLINE VOID matrixMulSubRow_f64(double *matrix, uint32_t colNum, uint32_t startCol, double pivot,
                                       uint32_t curRow, uint32_t pivotRow)
{
    uint32_t opPos;
    svfloat64_t svPivot = svdup_f64(pivot);
    svfloat64_t svCur, svBase;
    svbool_t pred;

    for (opPos = startCol; opPos < colNum; opPos += svcntd()) {
        pred = svwhilelt_b64(opPos, colNum);

        svCur = svld1(pred, &matrix[curRow * colNum + opPos]);
        svBase = svld1(pred, &matrix[pivotRow * colNum + opPos]);
        svCur = svmls_m(pred, svCur, svBase, svPivot);
        svst1(pred, &matrix[curRow * colNum + opPos], svCur);
    }
}
#endif

/*
    Inverse operation of Gauss:
    1.Prepare an augmented matrix, with the augmented identity matrix as the result.
    2.Find the element with the largest absolute value as the pivot element (this can reduce
      calculation errors and improve numerical stability).
    3.Normalize the pivot element by scaling, making it equal to 1.
    4.Eliminate other rows by performing row operations to make their elements become 0.
    5.Repeat steps 2, 3, and 4 until the original matrix becomes the identity matrix.
*/

uint32_t matrix_inv_f32(const float *matrixSrc, uint32_t sRowNum, uint32_t sColNum,
                        float *matrixDst, uint32_t dRowNum, uint32_t dColNum)
{
    float *opMatrix;
    float *opPos;
    float pivot, newpivot;
    uint32_t selectRow, pivotRow;
    uint32_t curRow, curCol;

    if (matrixSrc == NULL || matrixDst == NULL) {
        return MATRIX_NOK;
    }

    if (sRowNum == 0 || sColNum == 0 || dRowNum == 0 || dColNum == 0) {
        return MATRIX_NOK;
    }

    if (sRowNum != sColNum || sRowNum != dRowNum || sRowNum != dColNum) {
        return MATRIX_SIZE_MISMATCH;
    }

    opMatrix = (float *)malloc(sizeof(float) * sRowNum * sColNum);
    if (opMatrix == NULL) {
        return MATRIX_NOK;
    }

    (void)memcpy(opMatrix, matrixSrc, sizeof(float) * sRowNum * sColNum);

    // Making identity matrix.
    opPos = matrixDst;
    for (curRow = 0; curRow < sRowNum; curRow++) {
        for (curCol = 0; curCol < sRowNum; curCol++) {
            if (curRow == curCol) {
                *opPos++ = 1.0;
            } else {
                *opPos++ = 0;
            }
        }
    }

    for (curCol = 0; curCol < sColNum; curCol++) {
        opPos = GET_MATRIX_ELEM(opMatrix, sColNum, curCol, curCol);
        pivot = *opPos;
        selectRow = curCol;
        pivotRow = curCol;

        // Find the largest absolute value as the pivot.
        for (curRow = curCol + 1; curRow < sRowNum; curRow++) {
            opPos = GET_MATRIX_ELEM(opMatrix, sColNum, curRow, curCol);
            newpivot = *opPos;
            if (fabs(newpivot) > fabs(pivot)) {
                selectRow = curRow;
                pivot = newpivot;
            }
        }

        // Determine whether the determinant is possible to be 0.
        if (isEqual_f32(pivot, 0.0f)) {
            free(opMatrix);
            return MATRIX_SINGULAR;
        }

        // Move the pivot to current operation row.
        if (selectRow != curCol) {
            matrixSwapRow_f32(opMatrix, sColNum, curCol, pivotRow, selectRow);
            matrixSwapRow_f32(matrixDst, dColNum, 0, pivotRow, selectRow);
        }

        // The pivot becomes one.
        pivot = 1.0f / pivot;
        matrixScaleRow_f32(opMatrix, sColNum, curCol, pivot, pivotRow);
        matrixScaleRow_f32(matrixDst, dColNum, 0, pivot, pivotRow);

        // Matrix elimination on current column.
        for (curRow = 0; curRow < sRowNum; curRow++) {
            if (curRow == curCol) {
                continue;
            }

            opPos = GET_MATRIX_ELEM(opMatrix, sColNum, curRow, curCol);
            pivot = *opPos;
            matrixMulSubRow_f32(opMatrix, sColNum, curCol, pivot, curRow, pivotRow);
            matrixMulSubRow_f32(matrixDst, dColNum, 0, pivot, curRow, pivotRow);
        }
    }
    free(opMatrix);

    return MATRIX_OK;
}

uint32_t matrix_inv_f64(const double *matrixSrc, uint32_t sRowNum, uint32_t sColNum,
                        double *matrixDst, uint32_t dRowNum, uint32_t dColNum)
{
    double *opMatrix;
    double *opPos;
    double pivot, newpivot;
    uint32_t selectRow, pivotRow;
    uint32_t curRow, curCol;

    if (matrixSrc == NULL || matrixDst == NULL) {
        return MATRIX_NOK;
    }

    if (sRowNum == 0 || sColNum == 0 || dRowNum == 0 || dColNum == 0) {
        return MATRIX_NOK;
    }

    if (sRowNum != sColNum || sRowNum != dRowNum || sRowNum != dColNum) {
        return MATRIX_SIZE_MISMATCH;
    }

    opMatrix = (double *)malloc(sizeof(double) * sRowNum * sColNum);
    if (opMatrix == NULL) {
        return MATRIX_NOK;
    }

    (void)memcpy(opMatrix, matrixSrc, sizeof(double) * sRowNum * sColNum);

    // Making identity matrix.
    opPos = matrixDst;
    for (curRow = 0; curRow < sRowNum; curRow++) {
        for (curCol = 0; curCol < sRowNum; curCol++) {
            if (curRow == curCol) {
                *opPos++ = 1.0f;
            } else {
                *opPos++ = 0;
            }
        }
    }

    for (curCol = 0; curCol < sColNum; curCol++) {
        opPos = GET_MATRIX_ELEM(opMatrix, sColNum, curCol, curCol);
        pivot = *opPos;
        selectRow = curCol;
        pivotRow = curCol;

        // Find the largest absolute value as the pivot.
        for (curRow = curCol + 1; curRow < sRowNum; curRow++) {
            opPos = GET_MATRIX_ELEM(opMatrix, sColNum, curRow, curCol);
            newpivot = *opPos;
            if (fabs(newpivot) > fabs(pivot)) {
                selectRow = curRow;
                pivot = newpivot;
            }
        }

        // Determine whether the determinant is possible to be 0.
        if (isEqual_f64(pivot, 0.0)) {
            free(opMatrix);
            return MATRIX_SINGULAR;
        }

        // Move the pivot to current operation row.
        if (selectRow != curCol) {
            matrixSwapRow_f64(opMatrix, sColNum, curCol, pivotRow, selectRow);
            matrixSwapRow_f64(matrixDst, dColNum, 0, pivotRow, selectRow);
        }

        // The pivot becomes one.
        pivot = 1.0 / pivot;
        matrixScaleRow_f64(opMatrix, sColNum, curCol, pivot, pivotRow);
        matrixScaleRow_f64(matrixDst, dColNum, 0, pivot, pivotRow);

        // Matrix elimination on current column.
        for (curRow = 0; curRow < sRowNum; curRow++) {
            if (curRow == curCol) {
                continue;
            }

            opPos = GET_MATRIX_ELEM(opMatrix, sColNum, curRow, curCol);
            pivot = *opPos;
            matrixMulSubRow_f64(opMatrix, sColNum, curCol, pivot, curRow, pivotRow);
            matrixMulSubRow_f64(matrixDst, dColNum, 0, pivot, curRow, pivotRow);
        }
    }
    free(opMatrix);

    return MATRIX_OK;
}
