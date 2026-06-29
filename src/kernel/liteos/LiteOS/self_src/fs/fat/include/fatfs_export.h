/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Fatfs Source Code Interface Export HeadFile
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
 
#ifndef _FATFS_EXPORT_H
#define _FATFS_EXPORT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern FATFS* FatFs[FF_VOLUMES];

extern LBA_t clst2sect(FATFS* fs, DWORD clst);

extern DWORD ld_dword(const BYTE* ptr);
extern WORD ld_word(const BYTE* ptr);
extern void st_word(BYTE *ptr, WORD val);
extern void st_dword(BYTE* ptr, DWORD val);

extern DWORD get_fat(FFOBJID* obj, DWORD clst);
extern FRESULT put_fat(FATFS* fs, DWORD clst, DWORD val);

extern int lock_volume(FATFS* fs, int syslock);
extern void unlock_volume(FATFS* fs, FRESULT res);
extern UINT check_fs(FATFS* fs, LBA_t sect);
extern FRESULT empty_lock(const FATFS* fs);

extern FRESULT create_name(DIR* dp, const TCHAR** path);
extern FRESULT mount_volume(const TCHAR** path, FATFS** rfs, BYTE mode);
extern FRESULT validate(FFOBJID* obj, FATFS** rfs);
extern FRESULT remove_chain(FFOBJID* obj, DWORD clst, DWORD pclst);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _FATFS_EXPORT_H */ 