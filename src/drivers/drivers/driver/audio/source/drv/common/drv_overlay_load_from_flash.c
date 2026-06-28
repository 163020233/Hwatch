/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: header file for audio input
 * Author: audio
 */

#include "securec.h"
#include "mem_addr.h"
#include "partition.h"
#include "audio_osal.h"
#include "audio_debug.h"
#include "drv_msg_sys.h"
#include "drv_audio_osal.h"
#include "drv_overlay_load.h"
#ifdef SAP_SIGN_CHECK_ONLY_SUPPORT
#include "memory_config_shared.h"
#endif
#include "core_overlay_info.h"

#ifdef SAP_SOCMN2
#include "tcxo.h"
#include "pm_stage_switch.h"
#include "clocks_core.h"
#endif

#ifdef CFG_NORFLASH_ONLY
#include "memory_config_flash.h"
#include "sfc_v2.h"
#endif

static td_bool g_dsp_fw_signed = TD_FALSE;

#define DSP0_PARTITION_ID PARTITION_DSP0_IMAGE
#define DSP1_PARTITION_ID PARTITION_DSP1_IMAGE

#if (SAP_CHIP_TYPE == socmn1)
#include "ux_lpc.h"
#include "image_verify.h"

static td_bool g_dsp_lib_signed = TD_FALSE;

#define HAID_DST_ADDR 0x5818c000
#define FILE_SIGNED_HEADER_OFFSET 0x800

#else /* SAP_CHIP_TYPE */
#if (defined SAP_SIGN_SUPPORT)
#include "secure_verify_boot_porting.h"
#define FILE_SIGNED_HEADER_OFFSET SAP_SIGN_HEADER_LEN

#else /* SAP_SIGN_SUPPORT */
#define FILE_SIGNED_HEADER_OFFSET 0x0
#endif /* SAP_SIGN_SUPPORT */
#endif /* SAP_CHIP_TYPE */

#ifdef SAP_SIGN_CHECK_ONLY_SUPPORT
#define DSP_VERIFY_PASS_FLAG 0x5A5A001
#endif

/* single-norflash版本禁用总线读，dsp加载数据必须采用4字节读写，而sfc寄存器读写一次256字节，所以创建64个u32的缓冲区，保证4字节对齐 */
#define SFC_READ_BYTES 256

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32 overlay_set_dsp_file_path(const char *file_path, td_u32 len)
{
    audio_unused(file_path);
    audio_unused(len);
    return AUDIO_SUCCESS;
}

static td_s32 drv_overlay_read_flash(td_void *dst, td_u32 size, td_u8 *src, td_u32 count)
{
#ifdef CFG_NORFLASH_ONLY
    audio_unused(count);
    return uapi_sfc_read(0, (td_u32)(src - FLASH_START), (td_u8 *)dst, size);
#else
    return memcpy_s(dst, size, src, count);
#endif
}

#ifdef CFG_NORFLASH_ONLY
static td_void drv_overlay_copy_section_by_sfc(td_u32 *virt_addr, td_u8 *lib_addr, td_u32 size)
{
    td_s32 ret;
    td_u32 *buf;
    td_u32 remain_size = size;
    td_u32 offset = 0;

    buf = (td_u32 *)audio_osal_malloc(SFC_READ_BYTES);
    if (buf == TD_NULL) {
        sap_err_log_fun(audio_osal_malloc, ERR_SYS_MEM_ALLOC);
        return;
    }

    while (remain_size > SFC_READ_BYTES) {
        ret = drv_overlay_read_flash(buf, SFC_READ_BYTES, lib_addr + offset, SFC_READ_BYTES);
        if (ret != EOK) {
            sap_err_log_fun(drv_overlay_read_flash, ret);
            audio_osal_free(buf);
            return;
        }

        drv_overlay_copy_section(virt_addr + (offset >> 2), (const td_u32 *)buf, SFC_READ_BYTES);
        remain_size -= SFC_READ_BYTES;
        offset += SFC_READ_BYTES;
    }

    ret = drv_overlay_read_flash(buf, remain_size, lib_addr + offset, remain_size);
    if (ret != EOK) {
        sap_err_log_fun(drv_overlay_read_flash, ret);
        audio_osal_free(buf);
        return;
    }

    drv_overlay_copy_section(virt_addr + (offset >> 2), (const td_u32 *)buf, remain_size);

    audio_osal_free(buf);
}
#endif

td_s32 overlay_load_elf(audio_core_id core_id, drv_overlay_lib_inst *lib_inst, elf_section_info *elf_sec_info)
{
    td_u32 i, sec_cnt;
    section_type sec_type;
    td_u32 phys_addr;
    td_u32 *virt_addr = TD_NULL;
    td_u32 offset = 0;
    td_s32 ret;
    struct {
        td_u32 addr;
        td_u32 size;
    } sec_hdr;

    if (lib_inst->lib_buf == TD_NULL || lib_inst->lib_size > OVERLAY_MAX_SIZE) {
        return ERR_SYS_BUF_OVERFLOW;
    }

    (td_void)memset_s(elf_sec_info, sizeof(elf_section_info), 0, sizeof(elf_section_info));
    ret = drv_overlay_read_flash(&sec_cnt, sizeof(td_u32), lib_inst->lib_buf + offset, sizeof(td_u32));
    if (ret != EOK) {
        return ret;
    }

    offset += (td_u32)sizeof(td_u32);

    for (i = 0; i < sec_cnt; i++) {
        ret = drv_overlay_read_flash(&sec_hdr, sizeof(sec_hdr), lib_inst->lib_buf + offset, sizeof(sec_hdr));
        if (ret != EOK) {
            return ret;
        }
        offset += (td_u32)sizeof(sec_hdr);

        if (is_4byte_aligned(sec_hdr.addr) != TD_TRUE || is_4byte_aligned(sec_hdr.size) != TD_TRUE) {
            audio_log_err("addr=%#x/size=%#x, not aligned to 4 bytes", sec_hdr.addr, sec_hdr.size);
            return ERR_SYS_VERIFY_FAIL;
        }

        phys_addr = drv_overlay_map_addr(core_id, sec_hdr.addr, &sec_type);
        virt_addr = audio_osal_address_map((td_ulong)phys_addr, sec_hdr.size);
#ifdef CFG_NORFLASH_ONLY
        drv_overlay_copy_section_by_sfc(virt_addr, lib_inst->lib_buf + offset, sec_hdr.size);
#else
        drv_overlay_copy_section(virt_addr, (const td_u32 *)(lib_inst->lib_buf + offset), sec_hdr.size);
#endif

        audio_osal_dcache_region_wb((td_ulong)phys_addr, virt_addr, sec_hdr.size);
        audio_osal_address_unmap((td_ulong)phys_addr, virt_addr);
#ifdef SAP_MCU_PSRAM_DYNAMIC_VETO_SUPPORT
        drv_overlay_set_elf_section_info(elf_sec_info, sec_type);
#endif
        offset += sec_hdr.size;
    }

    return AUDIO_SUCCESS;
}

static td_u8 *overlay_find_lib(const td_char *lib_name, td_u32 addr, td_u32 *lib_size)
{
    td_u32 i;
    td_u32 sec_cnt = 0;
    td_u32 offset = 0;
    td_s32 ret;
    struct {
        td_char name[OVERLAY_NAME_LEN];
        td_u32 size;
    } sec_hdr;
    td_u8 *lib = (td_u8 *)(td_uintptr_t)addr;

    if (lib_name == TD_NULL || lib == TD_NULL || lib_size == TD_NULL) {
        return TD_NULL;
    }

    ret = drv_overlay_read_flash(&sec_cnt, sizeof(td_u32), lib + offset, sizeof(td_u32));
    if (ret != EOK) {
        sap_err_log_fun(drv_overlay_read_flash, ret);
        return TD_NULL;
    }
    offset += (td_u32)sizeof(td_u32);

    for (i = 0; i < sec_cnt && offset < *lib_size; i++) {
        ret = drv_overlay_read_flash(&sec_hdr, sizeof(sec_hdr), lib + offset, sizeof(sec_hdr));
        if (ret != EOK) {
            sap_err_log_fun(drv_overlay_read_flash, ret);
            return TD_NULL;
        }
        offset += (td_u32)sizeof(sec_hdr);
        if (audio_osal_strncmp(sec_hdr.name, sizeof(sec_hdr.name), lib_name, sizeof(sec_hdr.name)) == 0) {
            sap_warn_log_str(sec_hdr.name);
            *lib_size = sec_hdr.size;
            return lib + offset;
        }
        offset += sec_hdr.size;
    }

    return TD_NULL;
}

static td_void overlay_get_flash_addr_size(partition_ids_t partition_id, td_u32 *addr, td_u32 *size)
{
    errcode_t ret;
    struct {
        partition_ids_t partition_id;
        partition_information_t partition_info;
        td_u32 addr;
        td_u32 size;
        td_u32 offset;
    } dsp_image_info[] = {
        {DSP0_PARTITION_ID, {0}, FLASH_DSP_MAIN_ADDR, FLASH_DSP_MAIN_SIZE, FILE_SIGNED_HEADER_OFFSET},
#if defined(SAP_TWS_PRODUCT) || defined(SAP_SOUND_BOX_PRODUCT)
        {DSP1_PARTITION_ID, {0}, FLASH_DSP_OVERLAY_ADDR, FLASH_DSP_OVERLAY_SIZE, 0},
#else
        {DSP1_PARTITION_ID, {0}, FLASH_DSP_OVERLAY_ADDR, FLASH_DSP_OVERLAY_SIZE, FILE_SIGNED_HEADER_OFFSET},
#endif
    };
    const td_u32 cnt = (td_u32)(sizeof(dsp_image_info) / sizeof(dsp_image_info[0]));

    for (td_u32 i = 0; i < cnt; i++) {
        if (dsp_image_info[i].partition_id != partition_id) {
            continue;
        }

#if defined(SAP_FLASH_PARTITION_SUPPORT)
        *addr = 0;
        *size = 0;
        ret = uapi_partition_get_info(dsp_image_info[i].partition_id, &dsp_image_info[i].partition_info);
        if (ret == ERRCODE_SUCC && dsp_image_info[i].partition_info.type == PARTITION_BY_ADDRESS) {
            dsp_image_info[i].addr = dsp_image_info[i].partition_info.part_info.addr_info.addr + FLASH_BASEADDR;
            dsp_image_info[i].size = dsp_image_info[i].partition_info.part_info.addr_info.size;
        } else {
            sap_warn_log_fun(uapi_partition_get_info, ret);
            continue;
        }
#endif /* SAP_FLASH_PARTITION_SUPPORT */

        *addr = dsp_image_info[i].addr + dsp_image_info[i].offset;
        *size = dsp_image_info[i].size - dsp_image_info[i].offset;

        sap_trace_log_h32(*addr);
        sap_trace_log_h32(*size);

        return;
    }

    audio_unused(ret);
    return;
}

td_s32 overlay_unpack_lib(drv_overlay_lib_inst *lib_inst)
{
    td_u32 addr = FLASH_DSP_OVERLAY_ADDR;
    td_u32 size = FLASH_DSP_OVERLAY_SIZE;
    partition_ids_t dsp_image_pool[] = {
#if defined(SAP_TWS_PRODUCT) || defined(SAP_SOUND_BOX_PRODUCT)
        DSP1_PARTITION_ID,
        DSP0_PARTITION_ID,
#else
        DSP0_PARTITION_ID,
        DSP1_PARTITION_ID,
#endif
    };
    const td_u32 cnt = (td_u32)(sizeof(dsp_image_pool) / sizeof(dsp_image_pool[0]));

    for (td_u32 i = 0; i < cnt; i++) {
        overlay_get_flash_addr_size(dsp_image_pool[i], &addr, &size);
        if (addr == 0) {
            continue;
        }

        lib_inst->lib_buf = overlay_find_lib(lib_inst->lib_name, addr, &size);
        if (lib_inst->lib_buf != TD_NULL) {
            lib_inst->lib_size = size;
            return AUDIO_SUCCESS;
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
        sap_err_log_str(lib_name);
        audio_osal_free(lib_inst);
        return ret;
    }

    if (lib_inst->lib_size > *lib_buf_size) { /* input buf size check */
        sap_err_log_str(lib_inst->lib_size);
        audio_osal_free(lib_inst);
        return ERR_SYS_BUF_OVERFLOW;
    }

    ret = drv_overlay_read_flash(lib_buf, *lib_buf_size, lib_inst->lib_buf, lib_inst->lib_size);
    if (ret != EOK) {
        sap_err_log_fun(drv_overlay_read_flash, ret);
        audio_osal_free(lib_inst);
        return ERR_SYS_MEM_COPY;
    }

    *lib_buf_size = lib_inst->lib_size; /* output buf actual data size */

    audio_osal_free(lib_inst);
    return AUDIO_SUCCESS;
}

#ifdef SAP_SOCMN2
static td_s32 drv_overlay_load_dsp(audio_core_id core_id)
{
    td_s32 ret;

    if (core_id != AUDIO_CORE_DSP0) {
        sap_err_log_s32(core_id);
        return ERR_SYS_INVALID_PARAM;
    }

#ifdef SAP_AUDIO_DSP_ROM_SUPPORT
    ret = drv_overlay_load_lib_program(core_id, AUDIO_LIB_DSP0_ROM, OVERLAY_ROM);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_overlay_load_lib_program, ret);
        return ret;
    }
#endif

    ret = drv_overlay_load_lib_program(core_id, AUDIO_LIB_DSP0_APP, OVERLAY_MAIN);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_overlay_load_lib_program, ret);
        return ret;
    }

    return AUDIO_SUCCESS;
}

td_s32 drv_overlay_load_main_program(audio_core_id core_id)
{
    td_s32 ret;

    /* 起高频 */
    pm_stage_switch(UXS_HIGH_PERF_ON);
    clocks_set_dsp_freq_max();

    ret = drv_overlay_load_dsp(core_id);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_overlay_load_dsp, ret);
    }

    /* 关高频 */
    pm_stage_switch(UXS_HIGH_PERF_OFF);
    return ret;
}

#else

static td_void overlay_update_section_list(td_void)
{
    drv_overlay_clear_section_list();
}

td_s32 drv_overlay_load_main_program(audio_core_id core_id)
{
    errcode_t ret;
    td_u32 addr = FLASH_DSP_MAIN_ADDR;
    td_u32 size = FLASH_DSP_MAIN_SIZE;
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
#endif /* SAP_AUDIO_DSP_ROM_SUPPORT */

    overlay_get_flash_addr_size(DSP0_PARTITION_ID, &addr, &size);
    lib_inst->lib_buf = (td_u8 *)addr;
    lib_inst->lib_size = size;
    ret = overlay_load_elf(core_id, lib_inst, &elf_sec_info);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(overlay_load_elf, ret);
        audio_osal_free(lib_inst);
        return ret;
    }

    audio_osal_free(lib_inst);
    overlay_update_section_list();
    return AUDIO_SUCCESS;
}
#endif

#if defined(SAP_SIGN_CHECK_ONLY_SUPPORT)
static td_s32 verify_dsp_fw(td_void)
{
    td_u32 sign_flag;

    audio_reg_read(AUDIO_FLAG_REGION_START, sign_flag);
    if (sign_flag == DSP_VERIFY_PASS_FLAG) {
        return AUDIO_SUCCESS;
    } else {
        return AUDIO_FAILURE;
    }
}
#elif defined(SAP_SIGN_SUPPORT)
static td_s32 verify_dsp_fw(td_void)
{
    td_s32 ret;

    struct {
        image_type_t type;
        td_u32 addr;
    } img_info[] = {
        {OEM_DSP0_TYPE, FLASH_DSP_MAIN_ADDR},
        {OEM_DSP1_TYPE, FLASH_DSP_OVERLAY_ADDR},
    };
    const td_u32 cnt = (td_u32)(sizeof(img_info) / sizeof(img_info[0]));

    for (td_u32 i = 0; i < cnt; i++) {
        ret = (td_s32)secure_verify_image(img_info[i].type, img_info[i].addr, FLASH_ROOT_PUB_KEY_START);
        if (ret != ERRCODE_SUCC) {
            sap_err_log_h32(img_info[i].type);
            sap_err_log_h32(img_info[i].addr);
            sap_err_log_fun(secure_verify_image, ret);
            return ret;
        }
    }

    return AUDIO_SUCCESS;
}
#endif /* SAP_SIGN_SUPPORT */

td_s32 drv_overlay_verify_dsp_fw(td_void)
{
#if defined(SAP_SIGN_SUPPORT) || defined(SAP_SIGN_CHECK_ONLY_SUPPORT)
    td_s32 ret;

    if (g_dsp_fw_signed == TD_TRUE) { /* Only verify once at first poweron */
        return AUDIO_SUCCESS;
    }

    ret = verify_dsp_fw();
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(verify_dsp_fw, ret);
        return ret;
    }
#endif

    g_dsp_fw_signed = TD_TRUE;
    return AUDIO_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
