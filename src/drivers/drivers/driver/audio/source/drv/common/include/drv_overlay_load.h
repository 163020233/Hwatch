/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: header file for audio overlay
 * Author: audio
 */

#ifndef __DRV_OVERLAY_LOAD_H__
#define __DRV_OVERLAY_LOAD_H__

#include "audio_struct.h"
#include "core_overlay_info.h"

#define OVERLAY_MAX_SIZE 0x200000 /* 2M */
#define OVERLAY_NAME_LEN 16

typedef enum {
    SECTION_TYPE_OCRAM,
    SECTION_TYPE_TCM,
    SECTION_TYPE_PSRAM,
    SECTION_TYPE_SHM,
    SECTION_TYPE_MAX
} section_type;

typedef struct {
    td_bool ocram_flag;
    td_bool tcm_flag;
    td_bool psram_flag;
    td_bool shm_flag;
} elf_section_info;

typedef struct {
    const td_char *lib_name;
    td_void *flip;
    td_u8 *lib_buf;
    td_u32 lib_size;
} drv_overlay_lib_inst;

static inline td_void drv_overlay_copy_section(td_u32 *to, const td_u32 *from, td_u32 size)
{
    td_u32 i;
    td_u32 *dest = to;
    const td_u32 *src = from;
    for (i = 0; i < (size >> 2); i++) { /* shift 2 bit to sizeof int */
        *dest++ = *src++;
    }
}

drv_overlay_lib_inst *drv_overlay_init_lib(td_void);

td_s32 overlay_set_dsp_file_path(const char *file_path, td_u32 len);

td_u32 drv_overlay_map_addr(audio_core_id core_id, td_u32 addr, section_type *sec_type);

td_s32 drv_overlay_add_section_item(audio_core_id core_id, td_u32 addr, td_u32 size);
td_void drv_overlay_clear_section_list(td_void);

td_s32 drv_overlay_backup_tcm(audio_core_id core_id);
td_s32 drv_overlay_restore_tcm(audio_core_id core_id);

td_s32 drv_overlay_load_main_program(audio_core_id core_id);

td_s32 drv_overlay_verify_dsp_fw(td_void);

td_s32 drv_overlay_get_lib(const td_char *lib_name, td_u8 *lib_buf, td_u32 *lib_buf_size);
td_s32 overlay_unpack_lib(drv_overlay_lib_inst *lib_inst);

td_s32 drv_overlay_load_lib_program(audio_core_id core_id, audio_lib_id type, core_overlay_id overlay_id);
td_s32 drv_overlay_unload_lib_program(core_overlay_id overlay_id);

td_s32 overlay_load_elf(audio_core_id core_id, drv_overlay_lib_inst *lib_inst, elf_section_info *elf_sec_info);
td_s32 drv_overlay_request_firmware(td_handle handle, core_overlay_id overlay_id);
td_s32 drv_overlay_release_firmware(td_handle handle, core_overlay_id overlay_id);

#ifdef SAP_MCU_PSRAM_DYNAMIC_VETO_SUPPORT
td_void drv_overlay_set_elf_section_info(elf_section_info *elf_sec_info, section_type sec_type);
td_s32 drv_overlay_add_vote(core_overlay_id overlay_id);
td_s32 drv_overlay_remove_vote(core_overlay_id overlay_id);
#endif

#endif /* __DRV_OVERLAY_LOAD_H__ */
