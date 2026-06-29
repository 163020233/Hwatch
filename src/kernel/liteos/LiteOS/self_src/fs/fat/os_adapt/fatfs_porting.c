/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Fatfs Adapter Interface Implementation
 * Author: Huawei LiteOS Team
 * Create: 2022-8-20
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

#include "fatfs_extend.h"
#include "disk.h"
#include "diskio.h"
#include "los_mux.h"
#include "disk_pri.h"

static UINT32 gFatFsLock[FF_VOLUMES + 1];

/* Adapt for ff.c */
static DWORD FatTime(time_t sec)
{
    time_t seconds = sec;
    struct tm localTime = {0};

    if (localtime_r(&seconds, &localTime) == NULL) {
        return 0;
    }

    /* Year must start at 1980 */
    if ((localTime.tm_year + 1900) < 1980) {
        return 0;
    }

    /* Bit shift : transfer seconds to fat time field */
    return  ((DWORD)(localTime.tm_year - 80) << 25) |
            ((DWORD)(localTime.tm_mon + 1) << 21) |
            ((DWORD)localTime.tm_mday << 16) |
            ((DWORD)localTime.tm_hour << 11) |
            ((DWORD)localTime.tm_min << 5) |
            ((DWORD)localTime.tm_sec >> 1);
}

DWORD get_fattime(VOID)
{
    time_t seconds;

    if (gTimeStatus == SYSTEM_TIME_DISABLE) {
        return 0;
    }

    seconds = time(NULL);
    return FatTime(seconds);
}

/* Adapt for diskio.c */
DSTATUS disk_status(BYTE pdrv)
{
    (VOID)pdrv;
    return 0;
}

DSTATUS disk_initialize(BYTE pdrv)
{
    (VOID)pdrv;
    return 0;
}

DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
    int result;

    result = los_part_read((INT32)pdrv, (VOID *)buff, sector, (UINT32)count);

    return (result == 0) ? RES_OK : RES_ERROR;
}

DRESULT disk_raw_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
    int result;

    result = los_disk_read((INT32)pdrv, (VOID *)buff, sector, (UINT32)count);

    return (result == 0) ? RES_OK : RES_ERROR;
}

DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
    int result;

    result = los_part_write((INT32)pdrv, (const VOID *)buff, sector, (UINT32)count);

    return (result == 0) ? RES_OK : RES_ERROR;
}

DRESULT disk_raw_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
    int result;

    result = los_disk_write((INT32)pdrv, (const VOID *)buff, sector, (UINT32)count);

    return (result == 0) ? RES_OK : RES_ERROR;
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    int result;

    result = los_part_ioctl((INT32)pdrv, (INT32)cmd, buff);

    return (result == 0) ? RES_OK : RES_ERROR;
}

/* Adapt for ffsystem.c */
void *ff_memalloc(UINT size)
{
    if(size == 0) {
        return NULL;
    }

    return fat_memzalloc(size);
}

void ff_memfree(void *ptr)
{
    if (ptr == NULL) {
        return;
    }

    fat_memfree(ptr);
}

int ff_mutex_create(int vol)
{
    UINT32 ret;

    if ((vol > FF_VOLUMES) || (vol < 0)) {
        return 0;
    }

    ret = LOS_MuxCreate(&gFatFsLock[vol]);

    return (ret == LOS_OK) ? 1 : 0; /* 1 :success 0: fail */
}

void ff_mutex_delete(int vol)
{
    if ((vol > FF_VOLUMES) || (vol < 0)) {
        return;
    }

    (VOID)LOS_MuxDelete(gFatFsLock[vol]);
}

int ff_mutex_take(int vol)
{
    if ((vol > FF_VOLUMES) || (vol < 0)) {
        return 0;
    }

    if (FS_OPERATE_IN_EXC) {
        return 1;
    }

    if (LOS_MuxPend(gFatFsLock[vol], FF_FS_TIMEOUT) != LOS_OK) {
        return 0; /* fail */
    }

    return 1; /* success */
}

void ff_mutex_give(int vol)
{
    if ((vol > FF_VOLUMES) || (vol < 0)) {
        return;
    }

    if (FS_OPERATE_IN_EXC) {
        return;
    }

    (VOID)LOS_MuxPost(gFatFsLock[vol]);
}
