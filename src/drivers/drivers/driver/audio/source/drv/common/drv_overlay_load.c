/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: header file for audio input
 * Author: audio
 */

#include "securec.h"
#include "mem_addr.h"
#include "osal_list.h"
#include "audio_osal.h"
#include "audio_debug.h"
#include "drv_msg_sys.h"
#include "drv_audio_osal.h"
#include "core_overlay_info.h"
#ifdef SAP_MCU_PSRAM_DYNAMIC_VETO_SUPPORT
#include "pm_veto_porting.h"
#include "pm_clocks.h"
#include "pm_veto.h"
#endif
#include "drv_overlay_load.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define AUDIO_CORE_MASK_DSP0 (1L << AUDIO_CORE_DSP0)
#define AUDIO_CORE_MASK_DSP1 (1L << AUDIO_CORE_DSP1)
#define AUDIO_CORE_MASK_ALL_DSP (AUDIO_CORE_MASK_DSP0 | AUDIO_CORE_MASK_DSP1)

typedef struct {
    td_u32 core_mask;
    td_u32 addr;
    td_u32 size;
    td_u32 map_addr; /* A cpu映射的新物理地址 */
    section_type sec_type;     /* IRAM、DRAM内存段只能四字节读写 */
} elf_section_map;

static const elf_section_map g_elf_section_map[] = {
    /* sram */
    {AUDIO_CORE_MASK_ALL_DSP, SRAM_ADDR, SIZE_4M, SRAM_MAP_ADDR, SECTION_TYPE_OCRAM},

    /* share ram */
    {AUDIO_CORE_MASK_ALL_DSP, SHM_ADDR, COM_MEM_MAP_SIZE, SHM_MAP_ADDR, SECTION_TYPE_SHM},

    /*
     * dsp0 dram 0xe8058000, size 32k + 128k
     */
    {AUDIO_CORE_MASK_DSP0, DRAM_ADDR, DRAM_SIZE, ACORE_DSP0_DRAM_ADDR, SECTION_TYPE_TCM},

    /*
     * dsp0 iram 0xe8080000, size 32k + 16k
     */
    {AUDIO_CORE_MASK_DSP0, IRAM_ADDR, IRAM_SIZE, ACORE_DSP0_IRAM_ADDR, SECTION_TYPE_TCM},

    /*
     * dsp1 dram 0xe8058000, size 32k + 128k
     */
    {AUDIO_CORE_MASK_DSP1, DRAM_ADDR, DRAM_SIZE, ACORE_DSP1_DRAM_ADDR, SECTION_TYPE_TCM},

    /*
     * dsp1 iram 0xe8080000, size 32k + 16k
     */
    {AUDIO_CORE_MASK_DSP1, IRAM_ADDR, IRAM_SIZE, ACORE_DSP1_IRAM_ADDR, SECTION_TYPE_TCM},

    /* psram */
    {AUDIO_CORE_MASK_ALL_DSP, PSRAM_ADDR, PSRAM_MAP_SIZE, PSRAM_MAP_ADDR, SECTION_TYPE_PSRAM},
};

typedef struct {
    td_u32 addr;
    td_u32 size;
    audio_core_id core_id;
    struct osal_list_head node;
} elf_section_item;

static OSAL_LIST_HEAD(g_section_item_list);

static td_u32 overlay_map_addr(audio_core_id core_id, td_u32 addr, section_type *sec_type)
{
    td_u32 i;
    const elf_section_map *sec_map = TD_NULL;
    td_u32 core_mask = (1L << (td_u32)core_id);

    *sec_type = SECTION_TYPE_MAX;
    for (i = 0; i < sizeof(g_elf_section_map) / sizeof(g_elf_section_map[0]); i++) {
        sec_map = &g_elf_section_map[i];
        if ((sec_map->core_mask & core_mask) == 0) {
            continue;
        }

        if (sec_map->map_addr == 0) {
            continue;
        }

        if ((addr >= sec_map->addr) && (addr < (sec_map->addr + sec_map->size))) {
            *sec_type = sec_map->sec_type;
            return sec_map->map_addr + (addr - sec_map->addr);
        }
    }

    return addr;
}

td_u32 drv_overlay_map_addr(audio_core_id core_id, td_u32 addr, section_type *sec_type)
{
    return overlay_map_addr(core_id, addr, sec_type);
}

td_s32 drv_overlay_add_section_item(audio_core_id core_id, td_u32 addr, td_u32 size)
{
    elf_section_item *sec_item = TD_NULL;

    sec_item = (elf_section_item *)audio_osal_kmalloc(sizeof(*sec_item));
    if (sec_item == TD_NULL) {
        return ERR_SYS_MEM_ALLOC;
    }

    sec_item->addr = addr;
    sec_item->size = size;
    sec_item->core_id = core_id;
    osal_list_add_tail(&sec_item->node, &g_section_item_list);
    return AUDIO_SUCCESS;
}

td_void drv_overlay_clear_section_list(td_void)
{
    elf_section_item *sec_item = TD_NULL;
    elf_section_item *tmp_item = TD_NULL;

    osal_list_for_each_entry_safe(sec_item, tmp_item, &g_section_item_list, node) {
        osal_list_del(&sec_item->node);
        audio_osal_kfree(sec_item);
    }
}

static td_s32 overlay_sync_tcm_elf(audio_core_id core_id, td_u32 addr, td_u32 size, td_bool bak_flag)
{
    td_u32 tcm_phys, bak_phys;
    td_void *tcm_virt, *bak_virt;
    td_char *offset = TD_NULL;
    elf_section_item *sec_item = TD_NULL;
    section_type sec_type;

    bak_phys = overlay_map_addr(core_id, addr, &sec_type);
    bak_virt = audio_osal_address_map((td_ulong)bak_phys, size);
    if (bak_virt == TD_NULL) {
        sap_err_log_fun(audio_osal_address_map, ERR_SYS_MEM_EXCEPTION);
        return ERR_SYS_MEM_EXCEPTION;
    }
    offset = (td_char *)bak_virt;

    osal_list_for_each_entry(sec_item, &g_section_item_list, node) {
        tcm_phys = overlay_map_addr(sec_item->core_id, sec_item->addr, &sec_type);
        if (sec_type != SECTION_TYPE_TCM) {
            continue;
        }

        audio_log_info("sec[addr: 0x%08X, size: 0x%08X]", tcm_phys, sec_item->size);
        tcm_virt = audio_osal_address_map((td_ulong)tcm_phys, sec_item->size);
        if (tcm_virt == TD_NULL) {
            audio_osal_address_unmap((td_ulong)bak_phys, bak_virt);
            sap_err_log_fun(audio_osal_address_map, ERR_SYS_MEM_EXCEPTION);
            return ERR_SYS_MEM_EXCEPTION;
        }

        if (bak_flag) {
            drv_overlay_copy_section((td_u32 *)offset, tcm_virt, sec_item->size);
        } else {
            drv_overlay_copy_section(tcm_virt, (td_u32 *)offset, sec_item->size);
            audio_osal_dcache_region_wb((td_ulong)tcm_phys, tcm_virt, sec_item->size);
        }
        offset += sec_item->size;

        audio_osal_address_unmap((td_ulong)tcm_phys, tcm_virt);
    }

    if (bak_flag) {
        audio_osal_dcache_region_wb((td_ulong)bak_phys, bak_virt, size);
    }

    audio_osal_address_unmap((td_ulong)bak_phys, bak_virt);
    return AUDIO_SUCCESS;
}

td_s32 drv_overlay_backup_tcm(audio_core_id core_id)
{
    td_s32 ret;
    audio_func_enter();

    if (core_id == AUDIO_CORE_DSP1) {
        return AUDIO_SUCCESS;
    }

    ret = overlay_sync_tcm_elf(core_id, AUDIO_DRV_TCM_OVERLAY_ENTRY_ADDR, AUDIO_DRV_TCM_OVERLAY_BIN_SIZE, TD_TRUE);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(overlay_sync_tcm_elf, ret);
        return ret;
    }

    audio_func_exit();
    return AUDIO_SUCCESS;
}

td_s32 drv_overlay_restore_tcm(audio_core_id core_id)
{
    td_s32 ret;
    audio_func_enter();

    if (core_id == AUDIO_CORE_DSP1) {
        return AUDIO_SUCCESS;
    }

    ret = overlay_sync_tcm_elf(core_id, AUDIO_DRV_TCM_OVERLAY_ENTRY_ADDR, AUDIO_DRV_TCM_OVERLAY_BIN_SIZE, TD_FALSE);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(overlay_sync_tcm_elf, ret);
        return ret;
    }

    audio_func_exit();
    return AUDIO_SUCCESS;
}

static const struct {
    audio_lib_id overlay_type;
    const td_char *name;
} g_overlay_name[] = {
    /* audio HAID libs */
    {AUDIO_LIB_DSP0_APP, "dsp_main.bin"},
    {AUDIO_LIB_DSP1_APP, "dsp_main1.bin"},

    {AUDIO_LIB_DSP0_ROM, "dsp_rom.bin"},

    /* audio HAID libs */
    {AUDIO_LIB_HAID, "haid.bin"},

    /* audio decode libs */
    {AUDIO_LIB_DEC_PCM, "pcm_dec.bin"},
    {AUDIO_LIB_DEC_MP3, "mp3_dec.bin"},
    {AUDIO_LIB_DEC_AAC, "aac_dec.bin"},
    {AUDIO_LIB_DEC_SBC, "sbc_dec.bin"},
    {AUDIO_LIB_DEC_OPUS, "opus_dec.bin"},
    {AUDIO_LIB_DEC_FLAC, "flac_dec.bin"},
    {AUDIO_LIB_DEC_L2HC_V2, "l2hc_dec_v2.bin"},
    {AUDIO_LIB_DEC_L2HC_V4, "l2hc_dec_v4.bin"},
    {AUDIO_LIB_DEC_LC3, "lc3_dec.bin"},
    {AUDIO_LIB_DEC_SILK, "silk_dec.bin"},
    {AUDIO_LIB_DEC_AMRWB, "amrwb.bin"},
    {AUDIO_LIB_DEC_AMRNB, "amrnb_dec.bin"},
    {AUDIO_LIB_DEC_VORBIS, "vorbis_dec.bin"},
    {AUDIO_LIB_DEC_ALAC, "alac_dec.bin"},
    {AUDIO_LIB_DEC_LHDC, "lhdc_dec.bin"},
    {AUDIO_LIB_DEC_LDAC, "ldac_dec.bin"},
    {AUDIO_LIB_DEC_L2HC_VOICE, "l2hc_voice.bin"},
    /* audio encode libs */
    {AUDIO_LIB_ENC_PCM, "pcm_enc.bin"},
    {AUDIO_LIB_ENC_MP3, "mp3_enc.bin"},
    {AUDIO_LIB_ENC_AAC, "aac_enc.bin"},
    {AUDIO_LIB_ENC_SBC, "sbc_enc.bin"},
    {AUDIO_LIB_ENC_OPUS, "opus_enc.bin"},
    {AUDIO_LIB_ENC_L2HC, "l2hc_enc.bin"},
    {AUDIO_LIB_ENC_LC3, "lc3_enc.bin"},
    {AUDIO_LIB_ENC_SILK, "silk_enc.bin"},
    {AUDIO_LIB_ENC_AMRWB, "amrwb.bin"},
    {AUDIO_LIB_ENC_AMRNB, "amrnb_enc.bin"},
    {AUDIO_LIB_ENC_L2HC_VOICE, "l2hc_voice.bin"},

    {AUDIO_LIB_SEA_AFE_SEE, "see_1mic.bin"},
    {AUDIO_LIB_SEA_AFE_VQE, "see_1mic.bin"},
    {AUDIO_LIB_SEA_AFE_AHE, "ahe_1mic.bin"},
    {AUDIO_LIB_SEA_AAI_KWS, "phs_gru.bin"},
    {AUDIO_LIB_SEA_AAI_ASR, "phs_gru.bin"},

    {AUDIO_LIB_SEA_AFE_EXT0, "sea_lib0.bin"},
    {AUDIO_LIB_SEA_AFE_EXT1, "sea_lib1.bin"},
    {AUDIO_LIB_SEA_AAI_EXT0, "sea_lib2.bin"},
    {AUDIO_LIB_SEA_AAI_EXT1, "sea_lib3.bin"},

    /* aef libs */
    {AUDIO_LIB_AEF_SMARTPA, "aef.bin"},
    {AUDIO_LIB_AEF_SWS, "aef.bin"},
    {AUDIO_LIB_AEF_HVS, "aef.bin"},
    {AUDIO_LIB_AEF_WDRC, "wdrc.bin"},
#if defined(SAP_EFFECT_CUSTOMER_SUPPORT)
    {AUDIO_LIB_AEF_CUSTOMER, "aef.bin"},
#endif
    /* pre-processing aef libs */
    {AUDIO_LIB_AEF_PRE, "aef_pre.bin"},
};

static const td_char *drv_overlay_find_and_reset_lib(audio_core_id core_id, audio_lib_id type)
{
    td_u32 i;

    for (i = 0; i < sizeof(g_overlay_name) / sizeof(g_overlay_name[0]); i++) {
        if (type != g_overlay_name[i].overlay_type) {
            continue;
        }
#ifdef SAP_DEBUG_BASIC_STATUS_INFO
        audio_logx(AUDIO_LOG_LEVEL_ALERT, "[audio] load lib: type = 0x%x; name = %s", type, g_overlay_name[i].name);
#endif
        return g_overlay_name[i].name;
    }

    audio_unused(core_id);
    return TD_NULL;
}

#define OVERLAY_REF_CNT_MAX 8

struct overlay_mmg {
    td_handle handle[OVERLAY_REF_CNT_MAX];
    td_u32 ref_cnt;
#ifdef SAP_MCU_PSRAM_DYNAMIC_VETO_SUPPORT
    td_u32 vote_cnt;
#endif
};

static struct overlay_mmg g_overlay_mmg[OVERLAY_MAX];

static td_s32 drv_overlay_clear_section(core_overlay_id overlay_id)
{
    td_s32 ret;

    if (overlay_id == OVERLAY_MAIN || overlay_id == OVERLAY_ROM) {
        return AUDIO_SUCCESS;
    }

    ret = drv_msg_sys_clear_overlay_section(overlay_id);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_msg_sys_clear_overlay_section, ret);
        return ret;
    }

    return AUDIO_SUCCESS;
}

static td_s32 drv_overlay_get_ref_cnt(core_overlay_id overlay_id, td_u32 *ref_cnt)
{
    if (overlay_id >= OVERLAY_MAX) {
        sap_fatal_log_s32(overlay_id);
        return ERR_SYS_INVALID_PARAM;
    }

    *ref_cnt = g_overlay_mmg[overlay_id].ref_cnt;

    return AUDIO_SUCCESS;
}

td_s32 drv_overlay_request_firmware(td_handle handle, core_overlay_id overlay_id)
{
    td_u32 i;
    struct overlay_mmg *mmg;

    if (overlay_id >= OVERLAY_MAX) {
        sap_fatal_log_s32(overlay_id);
        return ERR_SYS_INVALID_PARAM;
    }

    mmg = &g_overlay_mmg[overlay_id];
    if (mmg->ref_cnt >= OVERLAY_REF_CNT_MAX) {
        return ERR_SYS_NOT_AVAILABLE;
    }

    for (i = 0; i < OVERLAY_REF_CNT_MAX; i++) {
        if (mmg->handle[i] == 0) {
            mmg->handle[i] = handle;
            mmg->ref_cnt++;
            audio_log4(AUDIO_LOG_LEVEL_WARN,
                "[overlay] request_firmware, ref_cnt: %u |i: %u |handle: 0x%x |overlay_id: %u",
                mmg->ref_cnt, i, handle, overlay_id);
            return AUDIO_SUCCESS;
        }
    }

    return ERR_SYS_INST_FULL;
}

drv_overlay_lib_inst *drv_overlay_init_lib(td_void)
{
    drv_overlay_lib_inst *inst = (drv_overlay_lib_inst *)audio_osal_malloc(sizeof(drv_overlay_lib_inst));
    if (inst == TD_NULL) {
        return TD_NULL;
    }

    inst->lib_name = TD_NULL;
    inst->flip = TD_NULL;
    inst->lib_buf = TD_NULL;

    return inst;
}

static td_s32 drv_overlay_load_lib_elf(audio_core_id core_id, audio_lib_id type, core_overlay_id overlay_id)
{
    td_s32 ret;
    elf_section_info elf_sec_info;
    drv_overlay_lib_inst *lib_inst = drv_overlay_init_lib();
    if (lib_inst == TD_NULL) {
        return ERR_SYS_MEM_ALLOC;
    }

    lib_inst->lib_name = drv_overlay_find_and_reset_lib(core_id, type);
    if (lib_inst->lib_name == TD_NULL) {
        sap_err_log_s32(type);
        ret = ERR_SYS_NOT_SUPPORTED;
        goto out;
    }

    ret = overlay_unpack_lib(lib_inst);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(overlay_unpack_lib, ret);
        goto out;
    }

    ret = drv_overlay_clear_section(overlay_id);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_overlay_clear_section, ret);
        goto out;
    }

    ret = overlay_load_elf(core_id, lib_inst, &elf_sec_info);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(overlay_load_elf, ret);
        goto out;
    }

#ifdef SAP_MCU_PSRAM_DYNAMIC_VETO_SUPPORT
    if (elf_sec_info.psram_flag == TD_TRUE) {
        ret = drv_overlay_add_vote(overlay_id);
        if (ret != AUDIO_SUCCESS) {
            sap_err_log_fun(drv_overlay_add_vote, ret);
            goto out;
        }
    }
#endif

    ret = AUDIO_SUCCESS;

out:
    if (lib_inst->flip != TD_NULL) {
        drv_audio_osal_klib_fclose(lib_inst->flip);
        lib_inst->flip = TD_NULL;
    }
    if (lib_inst != TD_NULL) {
        audio_osal_free(lib_inst);
        lib_inst = TD_NULL;
    }
    audio_unused(overlay_id);
    return ret;
}

td_s32 drv_overlay_release_firmware(td_handle handle, core_overlay_id overlay_id)
{
    td_u32 i;
    struct overlay_mmg *mmg;
    if (overlay_id >= OVERLAY_MAX) {
        sap_fatal_log_s32(overlay_id);
        return ERR_SYS_INVALID_PARAM;
    }

    mmg = &g_overlay_mmg[overlay_id];
    if (mmg->ref_cnt == 0) {
        return ERR_SYS_NOT_AVAILABLE;
    }

    for (i = 0; i < OVERLAY_REF_CNT_MAX; i++) {
        if (mmg->handle[i] == handle) {
            mmg->handle[i] = 0;
            mmg->ref_cnt--;
            audio_log4(AUDIO_LOG_LEVEL_WARN,
                "[overlay] release_firmware, ref_cnt: %u |i: %u |handle: 0x%x |overlay_id: %u",
                mmg->ref_cnt, i, handle, overlay_id);
            return AUDIO_SUCCESS;
        }
    }
    return ERR_SYS_INVALID_PARAM;
}

#ifdef SAP_MCU_PSRAM_DYNAMIC_VETO_SUPPORT
td_void drv_overlay_set_elf_section_info(elf_section_info *elf_sec_info, section_type sec_type)
{
    switch (sec_type) {
        case SECTION_TYPE_PSRAM:
            elf_sec_info->psram_flag = TD_TRUE;
            break;
        default:
            break;
    }
}

td_s32 drv_overlay_add_vote(core_overlay_id overlay_id)
{
    td_s32 ret;
    struct overlay_mmg *mmg;

    if (overlay_id >= OVERLAY_MAX) {
        sap_fatal_log_s32(overlay_id);
        return ERR_SYS_INVALID_PARAM;
    }

    if (overlay_id == OVERLAY_SEA) {
        // 由于SEA算法开销对PSRAM性能敏感，对PSRAM提频进行投票
        ret = uapi_clocks_config_mode(PM_ID_AUDIO, PM_MODE_PERFORMACE);
        if (ret != AUDIO_SUCCESS) {
            sap_err_log_fun(uapi_clocks_config_mode, ret);
            return ret;
        }
        audio_log2(AUDIO_LOG_LEVEL_ALERT, "overlay_id=%d, add clock veto_id=%d", overlay_id, PM_ID_AUDIO);
    }

    ret = uapi_pm_add_sleep_veto_with_symmetry(PM_ID_AUDIO);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(uapi_pm_add_sleep_veto_with_symmetry, ret);
        return ret;
    }
    audio_log2(AUDIO_LOG_LEVEL_ALERT, "overlay_id=%d, add pm veto_id=%d", overlay_id, PM_ID_AUDIO);

    mmg = &g_overlay_mmg[overlay_id];
    mmg->vote_cnt++;

    return AUDIO_SUCCESS;
}

td_s32 drv_overlay_remove_vote(core_overlay_id overlay_id)
{
    struct overlay_mmg *mmg;

    if (overlay_id >= OVERLAY_MAX) {
        sap_fatal_log_s32(overlay_id);
        return ERR_SYS_INVALID_PARAM;
    }

    mmg = &g_overlay_mmg[overlay_id];
    if (mmg->vote_cnt == 0) {
        return AUDIO_SUCCESS;
    }

    (td_void)uapi_pm_remove_sleep_veto_with_symmetry(PM_ID_AUDIO);
    audio_log2(AUDIO_LOG_LEVEL_ALERT, "overlay_id=%d, remove pm veto_id=%d", overlay_id, PM_ID_AUDIO);

    if (overlay_id == OVERLAY_SEA) {
        // SEA撤销对PSRAM提频的投票
        (td_void)uapi_clocks_config_mode(PM_ID_AUDIO, PM_MODE_LP);
        audio_log2(AUDIO_LOG_LEVEL_ALERT, "overlay_id=%d, remove clock veto_id=%d", overlay_id, PM_ID_AUDIO);
    }

    mmg->vote_cnt--;

    return AUDIO_SUCCESS;
}
#endif

td_s32 drv_overlay_load_lib_program(audio_core_id core_id, audio_lib_id type, core_overlay_id overlay_id)
{
    td_s32 ret;
    td_u32 ref_cnt;

    audio_func_enter();

    if (type >= AUDIO_LIB_MAX) {
        sap_err_log_s32(type);
        return ERR_SYS_INVALID_PARAM;
    }

    ret = drv_overlay_get_ref_cnt(overlay_id, &ref_cnt);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_overlay_get_ref_cnt, ret);
        return ret;
    }
    if (ref_cnt > 1) {
#ifdef SAP_DEBUG_BASIC_STATUS_INFO
        audio_logx(AUDIO_LOG_LEVEL_ALERT, "[audio] lib already loaded: type = 0x%x; ref_cnt = %u", type, ref_cnt);
#endif
        return AUDIO_SUCCESS;
    }

    ret = drv_overlay_load_lib_elf(core_id, type, overlay_id);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_overlay_load_lib_elf, ret);
        return ret;
    }

    audio_func_exit();
    return AUDIO_SUCCESS;
}

td_s32 drv_overlay_unload_lib_program(core_overlay_id overlay_id)
{
    td_s32 ret;
    td_u32 ref_cnt;

    ret = drv_overlay_get_ref_cnt(overlay_id, &ref_cnt);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_overlay_get_ref_cnt, ret);
        return ret;
    }
    if (ref_cnt > 0) {
        return AUDIO_SUCCESS;
    }

    ret = drv_overlay_clear_section(overlay_id);
    if (ret != AUDIO_SUCCESS) {
        sap_err_log_fun(drv_overlay_clear_section, ret);
        return ret;
    }

#ifdef SAP_MCU_PSRAM_DYNAMIC_VETO_SUPPORT
    drv_overlay_remove_vote(overlay_id);
#endif

    return AUDIO_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
