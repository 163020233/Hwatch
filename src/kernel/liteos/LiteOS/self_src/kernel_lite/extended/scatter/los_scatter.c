/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2019. All rights reserved.
 * Description: Scatter Implementation
 * Author: Huawei LiteOS Team
 * Create: 2013-01-01
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

#include "los_scatter_pri.h"
#include "los_printf.h"
#include "los_hw.h"
#include "los_atomic.h"

#define SCATTER_AFTER_FUNC_SECTION __attribute__((section(".data.g_SCATTER_AFTER_FUNC")))

STATIC const UINTPTR g_entryOffset = (UINTPTR)&__ram_vectors_vma;
STATIC Atomic g_scatterLoadFlag = SCATTER_TODO;
/*
 * Description : calculate the length of scatterload in the first and second copy stage
 * Input       : stage1EndAddr   ---  Text start address
 *               stage2EndAddr   ---  Bss start address
 *               readAlignSize   ---  align size
 *               stage1Len       ---  Length of scatterload in the first copy stage
 *               stage2Len       ---  Length of scatterload in the second copy stage
 */
LITE_OS_SEC_TEXT_MINOR STATIC VOID OsAlignedLoadLenGet(UINTPTR stage1EndAddr, UINTPTR stage2EndAddr,
                                                       size_t readAlignSize, size_t *stage1Len, size_t *stage2Len)
{
    UINTPTR alignedStage1EndAddr;
    UINTPTR alignedStage2EndAddr;

    if ((stage1Len == NULL) || (stage2Len == NULL)) {
        return;
    }

    alignedStage1EndAddr = (stage1EndAddr + readAlignSize - 1) & ~(readAlignSize - 1);
    alignedStage2EndAddr = (stage2EndAddr + readAlignSize - 1) & ~(readAlignSize - 1);

    /* calculate the length of scatterload in the first copy stage */
    *stage1Len = alignedStage1EndAddr - g_entryOffset;

    /* calculate the length of scatterload in the second copy stage */
    *stage2Len = alignedStage2EndAddr - alignedStage1EndAddr;
}

/*
 * Description : According to the length to copy image data into memory
 * Input       : imageFlashStart  --- the address of image in flash
 *               stage1Len        ---  Length of scatterload in the first copy stage
 *               stage2Len        ---  Length of scatterload in the second copy stage
 *               flashReadFunc    ---  the hook function of reading image
 */
LITE_OS_SEC_TEXT_MINOR STATIC VOID OsFastBootLoad(UINTPTR imageFlashStart, size_t stage1Len, size_t stage2Len,
                                                  SCATTER_FLASH_READ_FUNC flashReadFunc)
{
    UINTPTR stage2FlashStartAddr = imageFlashStart + stage1Len;
    VOID *stage2MemStartAddr = (VOID *)(g_entryOffset + stage1Len);

    if ((flashReadFunc != NULL) && (flashReadFunc(stage2MemStartAddr, stage2FlashStartAddr, stage2Len) != 0)) {
        PRINT_ERR("%s, %d\n", __FUNCTION__, __LINE__);
        return;
    }

    dsb();
    flush_icache();
    flush_dcache((UINTPTR)stage2MemStartAddr, (UINTPTR)stage2MemStartAddr + stage2Len);
}

LITE_OS_SEC_DATA_MINOR SCATTER_AFTER_FUNC_SECTION SCATTER_ENTRY_FUNC g_SCATTER_AFTER_FUNC = NULL;
LITE_OS_SEC_TEXT_MINOR VOID LOS_ScatterLoad(UINTPTR imageFlashAddr, SCATTER_FLASH_READ_FUNC flashReadFunc,
                                            size_t readAlignSize)
{
    size_t stage1Len = 0;
    size_t stage2Len = 0;

    if (LOS_AtomicCmpXchg32bits(&g_scatterLoadFlag, SCATTER_DONE, SCATTER_TODO)) {
        PRINTK("other core already started scatter\n");
        return;
    }

    OsAlignedLoadLenGet((UINTPTR)&__text_start, (UINTPTR)&__int_stack_start, readAlignSize, &stage1Len, &stage2Len);
    OsFastBootLoad(imageFlashAddr, stage1Len, stage2Len, flashReadFunc);

    if (g_SCATTER_AFTER_FUNC != NULL) {
        g_SCATTER_AFTER_FUNC();
    }
}
