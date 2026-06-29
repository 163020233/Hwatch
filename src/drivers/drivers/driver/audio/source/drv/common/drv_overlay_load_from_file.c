/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: header file for audio input
 * Author: audio
 */

#include "securec.h"
#include "mem_addr.h"
#include "audio_osal.h"
#include "audio_debug.h"
#include "drv_msg_sys.h"
#include "drv_audio_osal.h"
#include "drv_overlay_load.h"
#include "core_overlay_info.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define MAX_PATH_LENGTH 64
#if (defined SAP_SIGN_SUPPORT)
#define FILE_SIGNED_HEADER_OFFSET SAP_SIGN_HEADER_LEN
static const td_char *g_dsp_main_file = "dsp_main_signed.bin";
static const td_char *g_dsp_lib_file = "dsp_overlay_signed.bin";
#else
#define FILE_SIGNED_HEADER_OFFSET 0x0
static const td_char *g_dsp_main_file = "dsp_main.bin";
static const td_char *g_dsp_lib_file = "dsp_overlay.bin";
#endif /* SAP_SIGN_SUPPORT */

static td_char g_dsp_file_path[MAX_PATH_LENGTH] = {"/user/dsp/"}; /* Default dsp bin path */

td_s32 overlay_set_dsp_file_path(const char *file_path, td_u32 len)
{
    td_s32 ret;
    td_u32 dsp_main_file_length = strlen(g_dsp_main_file);
    td_u32 dsp_lib_file_length = strlen(g_dsp_lib_file);
    td_u32 max_file_length = (dsp_main_file_length > dsp_lib_file_length) ? dsp_main_file_length : dsp_lib_file_length;

    if (len + max_file_length >= MAX_PATH_LENGTH) {
        sap_err_log_u32(len);
        sap_err_log_info("file path is too long");
        return ERR_SYS_MEM_OVERLOAD;
    }

    ret = memcpy_s(g_dsp_file_path, MAX_PATH_LENGTH - 1, file_path, len);
    if (ret != EOK) {
        sap_err_log_fun(memcpy_s, ret);
        return ERR_SYS_MEM_COPY;
    }

    return AUDIO_SUCCESS;
}

static td_s32 elf_tcm_section_copy(td_u32 *virt_addr, td_void *flip, td_u32 sec_hdr_size)
{
    td_char *sec_buf = TD_NULL;

    /* Load IRAM/DRAM with 32bit operation */
    sec_buf = (td_char *)audio_osal_malloc_aligned(sec_hdr_size, sizeof(td_u32));
    if (sec_buf == TD_NULL) {
        return ERR_SYS_MEM_ALLOC;
    }

    if (drv_audio_osal_klib_fread((td_char *)sec_buf, sec_hdr_size, flip) != (td_s32)sec_hdr_size) {
        audio_osal_free_aligned(sec_buf);
        return ERR_SYS_NOT_AVAILABLE;
    }
    drv_overlay_copy_section(virt_addr, (const td_u32 *)sec_buf, sec_hdr_size);
    audio_osal_free_aligned(sec_buf);

    return AUDIO_SUCCESS;
}

td_s32 overlay_load_elf(audio_core_id core_id, drv_overlay_lib_inst *lib_inst, elf_section_info *elf_sec_info)
{
    td_s32 ret;
    td_u32 i, sec_cnt;
    section_type sec_type;
    td_u32 phys_addr;
    td_u32 *virt_addr = TD_NULL;
    struct {
        td_u32 addr;
        td_u32 size;
    } sec_hdr;

    (td_void)memset_s(elf_sec_info, sizeof(elf_section_info), 0, sizeof(elf_section_info));
    if (drv_audio_osal_klib_fread((td_char *)&sec_cnt, sizeof(sec_cnt), lib_inst->flip) != sizeof(sec_cnt)) {
        return ERR_SYS_NOT_AVAILABLE;
    }

    for (i = 0; i < sec_cnt; i++) {
        if (drv_audio_osal_klib_fread((td_char *)&sec_hdr, sizeof(sec_hdr), lib_inst->flip) != sizeof(sec_hdr)) {
            return ERR_SYS_NOT_AVAILABLE;
        }

        if (is_4byte_aligned(sec_hdr.addr) != TD_TRUE || is_4byte_aligned(sec_hdr.size) != TD_TRUE) {
            audio_log_err("addr=%#x/size=%#x, not aligned to 4 bytes", sec_hdr.addr, sec_hdr.size);
            return ERR_SYS_VERIFY_FAIL;
        }

        phys_addr = drv_overlay_map_addr(core_id, sec_hdr.addr, &sec_type);
        virt_addr = audio_osal_address_map((td_ulong)phys_addr, sec_hdr.size);
        if (virt_addr == TD_NULL) {
            return ERR_SYS_MEM_EXCEPTION;
        }

        if (sec_type == SECTION_TYPE_TCM) {
            ret = elf_tcm_section_copy(virt_addr, lib_inst->flip, sec_hdr.size);
            if (ret != AUDIO_SUCCESS) {
                audio_osal_address_unmap((td_ulong)phys_addr, virt_addr);
                return ret;
            }
        } else {
            if (drv_audio_osal_klib_fread((td_char *)virt_addr, sec_hdr.size, lib_inst->flip) != (td_s32)sec_hdr.size) {
                audio_osal_address_unmap((td_ulong)phys_addr, virt_addr);
                return ERR_SYS_NOT_AVAILABLE;
            }
        }

        audio_osal_dcache_region_wb((td_ulong)phys_addr, virt_addr, sec_hdr.size);
        audio_osal_address_unmap((td_ulong)phys_addr, virt_addr);
#ifdef SAP_MCU_PSRAM_DYNAMIC_VETO_SUPPORT
        drv_overlay_set_elf_section_info(elf_sec_info, sec_type);
#endif
    }

    return AUDIO_SUCCESS;
}

static td_s32 overlay_get_file_from_flash(const char *file, td_u32 flash_addr, td_u32 flash_len)
{
#ifdef SAP_FLASH_FIRMWARE_SUPPORT
    td_void *flip = TD_NULL;

    audio_log_warn("save %#x %#x to %s\n", flash_addr, flash_len, file);
    flip = drv_audio_osal_klib_fopen(file, DRV_AUDIO_OSAL_O_RDWR | DRV_AUDIO_OSAL_O_CREAT, 0777);  /* 0777: priv mode */
    if (flip == TD_NULL) {
        audio_log_err("open %s failed\n", file);
        return ERR_SYS_OPEN_FAILED;
    }

    if (drv_audio_osal_klib_fwrite((td_char *)flash_addr, flash_len, flip) != flash_len) {
        audio_log_err("Write %s failed\n", file);
        return ERR_SYS_MEM_COPY;
    }

    audio_log_warn("Save %s Success.\n", file);
    drv_audio_osal_klib_fclose(flip);
#endif
    audio_unused(file);
    audio_unused(flash_addr);
    audio_unused(flash_len);
    return AUDIO_SUCCESS;
}

static td_void *overlay_open_file(const char *file, td_u32 flash_addr, td_u32 flash_len)
{
    td_s32 ret;
    td_u32 offset = FILE_SIGNED_HEADER_OFFSET;
    td_void *flip = TD_NULL;
    td_char full_path[MAX_PATH_LENGTH] = {0};

    ret = snprintf_s(full_path, MAX_PATH_LENGTH - 1, MAX_PATH_LENGTH - 1, "%s%s", g_dsp_file_path, file);
    if (ret < 0) {
        sap_err_log_s32(ret);
        return TD_NULL;
    }

    flip = drv_audio_osal_klib_fopen(full_path, DRV_AUDIO_OSAL_O_RDONLY, 0);
    if (flip == TD_NULL) {
        ret = overlay_get_file_from_flash(full_path, flash_addr, flash_len);
        if (ret != AUDIO_SUCCESS) {
            sap_err_log_fun(overlay_get_file_from_flash, ret);
            return TD_NULL;
        }

        flip = drv_audio_osal_klib_fopen(full_path, DRV_AUDIO_OSAL_O_RDONLY, 0);
        if (flip == TD_NULL) {
            sap_err_log_fun(drv_audio_osal_klib_fopen, ERR_SYS_OPEN_FAILED);
            return TD_NULL;
        }
    }

    if (offset > 0) {
        drv_audio_osal_klib_fseek(offset, OSAL_SEEK_SET, flip);
    }

    return flip;
}

static td_s32 overlay_update_section_list(audio_core_id core_id, td_void *flip)
{
    td_u32 i;
    td_u32 sec_cnt;
    section_type sec_type;
    struct {
        td_u32 addr;
        td_u32 size;
    } sec_hdr;

    drv_overlay_clear_section_list();

    drv_audio_osal_klib_fseek(0, SEEK_SET, flip);
    if (drv_audio_osal_klib_fread((td_char *)&sec_cnt, sizeof(sec_cnt), flip) != sizeof(sec_cnt)) {
        return ERR_SYS_NOT_AVAILABLE;
    }

    for (i = 0; i < sec_cnt; i++) {
        if (drv_audio_osal_klib_fread((td_char *)&sec_hdr, sizeof(sec_hdr), flip) != sizeof(sec_hdr)) {
            return ERR_SYS_NOT_AVAILABLE;
        }

        (td_void)drv_overlay_map_addr(core_id, sec_hdr.addr, &sec_type);
        drv_overlay_add_section_item(core_id, sec_hdr.addr, sec_hdr.size);

        drv_audio_osal_klib_fseek(sec_hdr.size, SEEK_CUR, flip);
    }

    return AUDIO_SUCCESS;
}

td_s32 overlay_unpack_lib(drv_overlay_lib_inst *lib_inst)
{
    td_u32 i, sec_cnt = 0;
    struct {
        td_char name[OVERLAY_NAME_LEN];
        td_u32 size;
    } sec_hdr;

    lib_inst->flip = overlay_open_file(g_dsp_lib_file, FLASH_DSP_OVERLAY_ADDR, FLASH_DSP_OVERLAY_SIZE);
    if (lib_inst->flip == TD_NULL) {
        sap_err_log_fun(overlay_open_file, ERR_SYS_OPEN_FAILED);
        return ERR_SYS_OPEN_FAILED;
    }

    if (drv_audio_osal_klib_fread((td_char *)&sec_cnt, sizeof(td_u32), lib_inst->flip) != sizeof(td_u32)) {
        sap_err_log_fun(drv_audio_osal_klib_fread, AUDIO_FAILURE);
        return ERR_SYS_NOT_AVAILABLE;
    }

    for (i = 0; i < sec_cnt; i++) {
        if (drv_audio_osal_klib_fread((td_char *)&sec_hdr, sizeof(sec_hdr), lib_inst->flip) != sizeof(sec_hdr)) {
            sap_err_log_fun(drv_audio_osal_klib_fread, AUDIO_FAILURE);
            return ERR_SYS_NOT_AVAILABLE;
        }

        sap_trace_log_str(sec_hdr.name);
        if (audio_osal_strncmp(sec_hdr.name, sizeof(sec_hdr.name), lib_inst->lib_name, sizeof(sec_hdr.name)) == 0) {
            sap_warn_log_str(sec_hdr.name);
            lib_inst->lib_size = sec_hdr.size;
            return AUDIO_SUCCESS;
        }

        if (drv_audio_osal_klib_fseek(sec_hdr.size, OSAL_SEEK_CUR, lib_inst->flip) < 0) {
            sap_err_log_fun(drv_audio_osal_klib_fseek, AUDIO_FAILURE);
            return ERR_SYS_NOT_AVAILABLE;
        }
    }

    return ERR_SYS_NOT_SUPPORTED;
}

td_s32 drv_overlay_get_lib(const td_char *lib_name, td_u8 *lib_buf, td_u32 *lib_buf_size)
{
    td_s32 ret;
    drv_overlay_lib_inst *lib_inst = drv_overlay_init_lib();
    if (lib_inst == TD_NULL) {
        return ERR_SYS_MEM_ALLOC;
    }

    sys_check_null_ptr_return(lib_name);
    sys_check_null_ptr_return(lib_buf);
    sys_check_null_ptr_return(lib_buf_size);

    lib_inst->lib_name = lib_name;

    ret = overlay_unpack_lib(lib_inst);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(overlay_unpack_lib, ret);
        goto out;
    }

    if (lib_inst->lib_size > *lib_buf_size) { /* input buf size check */
        sap_err_log_str(lib_inst->lib_size);
        ret = ERR_SYS_BUF_OVERFLOW;
        goto out;
    }

    if (drv_audio_osal_klib_fread((td_char *)lib_buf, lib_inst->lib_size, lib_inst->flip) != (td_s32)lib_inst->lib_size) {
        ret = ERR_SYS_NOT_AVAILABLE;
        goto out;
    }

    *lib_buf_size = lib_inst->lib_size; /* output buf actual data size */

out:
    if (lib_inst->flip != TD_NULL) {
        drv_audio_osal_klib_fclose(lib_inst->flip);
    }
    if (lib_inst != TD_NULL) {
        audio_osal_free(lib_inst);
    }
    return ret;
}

td_s32 drv_overlay_load_main_program(audio_core_id core_id)
{
    td_s32 ret;
    elf_section_info elf_sec_info;
    drv_overlay_lib_inst *lib_inst = drv_overlay_init_lib();
    if (lib_inst == TD_NULL) {
        return ERR_SYS_MEM_ALLOC;
    }

    audio_func_enter();
#ifdef SAP_AUDIO_DSP_ROM_SUPPORT
    ret = drv_overlay_load_lib_program(core_id, AUDIO_LIB_DSP0_ROM, OVERLAY_ROM);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_overlay_load_lib_program, ret);
        audio_osal_free(lib_inst);
        return ret;
    }
#endif

    lib_inst->flip = overlay_open_file(g_dsp_main_file, FLASH_DSP_MAIN_ADDR, FLASH_DSP_MAIN_SIZE);
    if (lib_inst->flip == TD_NULL) {
        audio_osal_free(lib_inst);
        return ERR_SYS_OPEN_FAILED;
    }

    ret = overlay_load_elf(core_id, lib_inst, &elf_sec_info);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(overlay_load_elf, ret);
        drv_audio_osal_klib_fclose(lib_inst->flip);
        audio_osal_free(lib_inst);
        return ret;
    }

    overlay_update_section_list(core_id, lib_inst->flip);
    drv_audio_osal_klib_fclose(lib_inst->flip);
    audio_osal_free(lib_inst);

    audio_func_exit();
    return AUDIO_SUCCESS;
}

td_s32 drv_overlay_verify_dsp_fw(td_void)
{
    return AUDIO_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
