/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: LiteOS Unite Trim Module Implementation
 * Author: Huawei LiteOS Team
 * Create: 2021-09-05
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

#include <math.h>
#include "los_bitmap.h"
#include "mmc_erase.h"
#include "utrim.h"

#define TRIM_BITMAP_BITS        32

#define TRIM_PERIOD             10000           /* 10s */
#define TRIM_UNIT_SIZE          65536           /* 65536 sector(32MB) : usual SD-NAND block size */
#define TRIM_SECTOR_RATIO       25              /* trim ratio of total part sector */

#define TRIM_TIMEOUT            2000            /* 2s */
#define TRIM_UNIT_NUMS          16              /* trim max size once : 512MB */

static INT32 DrvTrim(struct inode *priv, UINT64 start, UINT32 size)
{
    INT32 ret = (INT32)mmcsd_erase(priv, (size_t)start, size);
    if (ret != 0) {
        PRINT_ERR("%s failure\n", __FUNCTION__);
        return ret;
    }

    return ENOERR;
}

static UINT64 UnitAlignUp(const OsTrimManage *manager, UINT64 size)
{
    UINT64 align = 1ULL << manager->bit2SectorScale;
    return (size + (align - 1)) & ~(align - 1);
}

static UINT64 UnitAlignDown(const OsTrimManage *manager, UINT64 size)
{
    UINT64 align = 1ULL << manager->bit2SectorScale;
    return size & ~(align - 1);
}

static UINT32 TrimSector2Bit(const OsTrimManage *manager, UINT64 sector)
{
    return (UINT32)(sector >> manager->bit2SectorScale);
}

static UINT64 TrimUnit2Sector(const OsTrimManage *manager, UINT32 unit)
{
    return (UINT64)(unit) << manager->unit2SectorScale;
}

static UINT32 TrimSector2Unit(const OsTrimManage *manager, UINT64 sector)
{
    return (UINT32)(sector >> manager->unit2SectorScale);
}

static INT32 UniTrimInitCheck(const struct inode *devNode, UINT32 bit2SectorSize, UINT64 sectorCount)
{
    if (devNode == NULL) {
        return VFS_ERROR;
    }

    if (bit2SectorSize > (TRIM_UNIT_SIZE / TRIM_BITMAP_BITS)) {
        PRINT_ERR("bit2SectorSize is too big\n");
        return VFS_ERROR;
    }

    if (sectorCount < (TRIM_UNIT_SIZE * 100 / TRIM_SECTOR_RATIO)) { /* 100: 100% */
        PRINT_ERR("sectorCount is too small\n");
        return VFS_ERROR;
    }

    if ((bit2SectorSize == 0) || (bit2SectorSize & (bit2SectorSize - 1)) != 0) {
        PRINT_ERR("bit2SectorSize must be power of 2\n");
        return VFS_ERROR;
    }

    return ENOERR;
}

static VOID NodeMigrate(LOS_DL_LIST *listHead, LOS_DL_LIST *node)
{
    LOS_ListDelete(node);
    LOS_ListAdd(listHead, node);
}

static VOID FreeUnit(OsTrimManage *manager, OsTrimUnit *unit)
{
    NodeMigrate(&manager->freeListHead, &unit->listNode);
    (VOID)rb_erase(&unit->rbNode, &manager->rbRoot);
}

static OsTrimUnit *AllocUnit(OsTrimManage *manager)
{
    OsTrimUnit *unit = NULL;

    if (LOS_ListEmpty(&manager->freeListHead) == FALSE) {
        unit = LOS_DL_LIST_ENTRY(manager->freeListHead.pstPrev, OsTrimUnit, listNode);
        LOS_ListDelete(&unit->listNode);

        return unit;
    }

    /* Evict oldest bitmap unit */
    if (LOS_ListEmpty(&manager->bitmapListHead) == FALSE) {
        unit = LOS_DL_LIST_ENTRY(manager->bitmapListHead.pstPrev, OsTrimUnit, listNode);
        LOS_ListDelete(&unit->listNode);
        (VOID)rb_erase(&unit->rbNode, &manager->rbRoot);

        return unit;
    }

    return NULL;
}

static VOID RbAddUnit(OsTrimManage *manager, OsTrimUnit *unit)
{
    struct rb_node *node = manager->rbRoot.rb_node;
    struct rb_node **link = NULL;
    OsTrimUnit *tmpUnit = NULL;

    if (node == NULL) {
        rb_link_node(&unit->rbNode, NULL, &manager->rbRoot.rb_node);
    } else {
        for (; node != NULL; link = (tmpUnit->num > unit->num) ? &node->rb_left : &node->rb_right, node = *link) {
            tmpUnit = rb_entry(node, OsTrimUnit, rbNode);
            if (tmpUnit->num == unit->num) {
                PRINT_ERR("RbAddUnit fail, %lu, %lu\n", tmpUnit->num, unit->num);
                return;
            }
        }
        rb_link_node(&unit->rbNode, &tmpUnit->rbNode, link);
    }
    rb_insert_color(&unit->rbNode, &manager->rbRoot);
}

static OsTrimUnit *RbFindUnit(const OsTrimManage *manager, UINT32 num)
{
    OsTrimUnit *unit = NULL;
    struct rb_node *node = manager->rbRoot.rb_node;

    for (; node != NULL; node = (unit->num < num) ? node->rb_right : node->rb_left) {
        unit = rb_entry(node, OsTrimUnit, rbNode);
        if (unit->num == num) {
            return unit;
        }
    }
    return NULL;
}

static VOID InitUnit(const OsTrimManage *manager, OsTrimUnit *unit, UINT32 num)
{
    UINT32 len = manager->bitmapSize * sizeof(UINT32);

    unit->num = num;
    unit->trimFlag = FALSE;
    (VOID)memset_s(unit->bitmap, len, 0, len);
}

static BOOL UnitCanTrim(const OsTrimManage *manager, const OsTrimUnit *unit)
{
    UINT32 size = manager->bitmapSize;

    if (unit->trimFlag == TRUE) {
        return TRUE;
    }

    while (size--) {
        if (unit->bitmap[size] != ~0UL) {
            return FALSE;
        }
    }

    return TRUE;
}

VOID SortlistInsert(OsTrimManage *manager, OsTrimUnit *unit)
{
    OsTrimUnit *tmpUnit = NULL;
    LOS_DL_LIST *node = NULL;

    if (LOS_ListEmpty(&manager->sortListHead) == TRUE) {
        NodeMigrate(&manager->sortListHead, &unit->listNode);
        return;
    }
    /* Ascending sort list */
    node = manager->sortListHead.pstPrev;
    while (node != &manager->sortListHead) {
        tmpUnit = LOS_DL_LIST_ENTRY(node, OsTrimUnit, listNode);
        node = node->pstPrev;

        if (tmpUnit->num < unit->num) {
            NodeMigrate(&tmpUnit->listNode, &unit->listNode);
            return;
        }
    }

    NodeMigrate(&manager->sortListHead, &unit->listNode);
}

VOID SetUnitBitmap(OsTrimUnit *unit, UINT32 pos, UINT32 bits, UINT32 bitValue)
{
    VOID (*cb)(UINT32 *, UINT16) = NULL;
    UINT32 i, j, v;

    i = pos / TRIM_BITMAP_BITS;
    j = pos % TRIM_BITMAP_BITS;

    v  = (bitValue == 0) ? 0 : ~0UL;
    cb = (bitValue == 0) ? LOS_BitmapClr : LOS_BitmapSet;

    while (bits > 0) {
        /* pos is 32bit align, SetValue opt instead of SetBit opt for performance */
        if ((j == 0) && (bits >= TRIM_BITMAP_BITS)) {
            unit->bitmap[i] = v;
            bits -= TRIM_BITMAP_BITS;
            i++;
            continue;
        }

        cb(&unit->bitmap[i], TRIM_BITMAP_BITS - 1 - j);
        bits--;
        j++;
        if (j == TRIM_BITMAP_BITS) {
            j = 0;
            i++;
        }
    }
}

static INT32 TrimUnit(OsTrimManage *manager, UINT32 startNum)
{
    INT32 ret = ENOERR;
    UINT32 middleNum, endNum, trimNums;
    UINT64 startTick;
    OsTrimUnit *unit = NULL;
    LOS_DL_LIST *node = NULL;

    /* Trim as many units as possible to reduce IO operations */
    endNum = middleNum = startNum;
    node = manager->sortListHead.pstNext;
    while (node != &manager->sortListHead) {
        unit = LOS_DL_LIST_ENTRY(node, OsTrimUnit, listNode);
        if (unit->num != endNum) {
            break;
        }

        endNum++;
        node = node->pstNext;
    }

    startTick = LOS_TickCountGet();
    while (middleNum < endNum) {
        trimNums = ((endNum - middleNum) < TRIM_UNIT_NUMS) ? (endNum - middleNum) : TRIM_UNIT_NUMS;
        ret = manager->trimFun(manager->dev, TrimUnit2Sector(manager, middleNum),
            (UINT32)TrimUnit2Sector(manager, trimNums));
        if (ret) {
            PRINT_ERR("TrimUnit error, ret = %d\n", ret);
            break;
        }

        middleNum += trimNums;

        if (LOS_Tick2MS((UINT32)(LOS_TickCountGet() - startTick)) > TRIM_TIMEOUT) {
            break;
        }
    }

    while (startNum < middleNum) {
        unit = RbFindUnit(manager, startNum++);
        FreeUnit(manager, unit);
    }

    return ret;
}

static INT32 PeriodicTrimUnit(OsTrimManage *manager, UINT32 period)
{
    static UINT64 pre = 0;
    OsTrimUnit *unit = NULL;

    if (LOS_Tick2MS((UINT32)(LOS_TickCountGet() - pre)) < period) {
        return ENOERR;
    }

    pre = LOS_TickCountGet();

    if (LOS_ListEmpty(&manager->sortListHead) == TRUE) {
        return ENOERR;
    }

    unit = LOS_DL_LIST_ENTRY(manager->sortListHead.pstNext, OsTrimUnit, listNode);

    return TrimUnit(manager, unit->num);
}

INT32 UniUntrim(OsTrimManage *manager, UINT64 start, UINT32 trimSize)
{
    OsTrimUnit *unit = NULL;
    UINT64 end;
    UINT32 pos, unitNum, totalBits, markBits;

    /*
     * If start sector is not align at bit2SectorSize, expand the bitmap boundary :
     * The boundary bit could be set before, clear bit for protect boundary data.
     */
    end = UnitAlignUp(manager, start + trimSize);
    start = UnitAlignDown(manager, start);

    unitNum = TrimSector2Unit(manager, start);
    totalBits = TrimSector2Bit(manager, end - start);
    pos = TrimSector2Bit(manager, start) & (manager->unitBits - 1);

    while (totalBits > 0) {
        markBits = ((pos + totalBits) > manager->unitBits) ? (manager->unitBits - pos) : totalBits;

        unit = RbFindUnit(manager, unitNum);
        if (unit != NULL) {
            /*
             * Defer-set bitmap, we creat a "bitmap hole" here, ensure the unit
             * can trim if the "bitmap hole" is filled in next UniTrim call.
             */
            if (unit->trimFlag == TRUE) {
                (VOID)memset_s(unit->bitmap, manager->bitmapSize * sizeof(UINT32),
                    (UINT8)~0, manager->bitmapSize * sizeof(UINT32));
            }

            unit->trimFlag = FALSE;
            if ((pos == 0) && (markBits == manager->unitBits)) {
                FreeUnit(manager, unit);
            } else {
                SetUnitBitmap(unit, pos, markBits, 0); /* 0: unmask bitmap */
                NodeMigrate(&manager->bitmapListHead, &unit->listNode);
            }
        }

        unitNum++;
        totalBits -= markBits;
        pos = 0;
    }

    return PeriodicTrimUnit(manager, TRIM_PERIOD);
}

INT32 UniTrim(OsTrimManage *manager, UINT64 start, UINT32 trimSize)
{
    OsTrimUnit *unit = NULL;
    UINT64 end;
    UINT32 pos, unitNum, totalBits, markBits;

    /*
     * If start sector is not align at bit2SectorSize, Shrink the bitmap boundary :
     * there could be valid data at the boundary bit, We shouldn't set them for trim.
     */
    end = UnitAlignDown(manager, start + trimSize);
    start = UnitAlignUp(manager, start);
    if (end <= start) {
        return ENOERR;
    }

    unitNum = TrimSector2Unit(manager, start);
    totalBits = TrimSector2Bit(manager, end - start);
    pos = TrimSector2Bit(manager, start) & (manager->unitBits - 1);

    while (totalBits > 0) {
        markBits = ((pos + totalBits) > manager->unitBits) ? (manager->unitBits - pos) : totalBits;

        unit = RbFindUnit(manager, unitNum);
        if (unit == NULL) {
            unit = AllocUnit(manager);
            if (unit == NULL) {
                return VFS_ERROR;
            }

            InitUnit(manager, unit, unitNum);

            RbAddUnit(manager, unit);
            LOS_ListAdd(&manager->bitmapListHead, &unit->listNode);
        }

        if (unit->trimFlag == FALSE) {
            /* Only set trimFlag for performance, defer-set the bitmap when untrim the unit */
            if ((pos == 0) && (markBits == manager->unitBits)) {
                unit->trimFlag = TRUE;
            } else {
                SetUnitBitmap(unit, pos, markBits, 1); /* 1: mask bitmap */
            }

            if (UnitCanTrim(manager, unit) == TRUE) {
                SortlistInsert(manager, unit);
            }
        }

        unitNum++;
        totalBits -= markBits;
        pos = 0;
    }

    return PeriodicTrimUnit(manager, TRIM_PERIOD);
}

OsTrimManage *UniTrimInit(struct inode *devNode, UINT32 bit2SectorSize, UINT64 sectorCount)
{
    OsTrimUnit *unit = NULL;
    OsTrimManage *manager = NULL;
    UINT32 bitmapSize, unitNum, unitMemSize, i;

    if (UniTrimInitCheck(devNode, bit2SectorSize, sectorCount) != ENOERR) {
        return NULL;
    }

    bitmapSize = (TRIM_UNIT_SIZE / bit2SectorSize) / TRIM_BITMAP_BITS;

    unitNum = (UINT32)(sectorCount / TRIM_UNIT_SIZE) * TRIM_SECTOR_RATIO / 100; /* 100: 100% */
    unitMemSize = sizeof(OsTrimUnit) + bitmapSize * sizeof(UINT32);
    manager = malloc(sizeof(OsTrimManage) + unitNum * unitMemSize);
    if (manager == NULL) {
        PRINT_ERR("%s no memory\n", __func__);
        return NULL;
    }

    manager->dev = devNode;
    manager->trimFun = DrvTrim;

    manager->unit2SectorScale = (UINT32)log2(TRIM_UNIT_SIZE);
    manager->bit2SectorScale = (UINT32)log2(bit2SectorSize);
    manager->unitBits = TrimSector2Bit(manager, TRIM_UNIT_SIZE);
    manager->bitmapSize = bitmapSize;

    manager->rbRoot.rb_node = NULL;
    LOS_ListInit(&manager->bitmapListHead);
    LOS_ListInit(&manager->sortListHead);
    LOS_ListInit(&manager->freeListHead);

    unit = (OsTrimUnit *)((UINTPTR)manager + sizeof(OsTrimManage));
    for (i = 0; i < unitNum; i++) {
        LOS_ListAdd(&manager->freeListHead, &unit->listNode);
        unit = (OsTrimUnit *)((UINTPTR)unit + unitMemSize);
    }

    return manager;
}

VOID UniTrimDeinit(OsTrimManage *manager)
{
    if (manager != NULL) {
        free(manager);
    }
}
