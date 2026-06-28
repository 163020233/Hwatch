/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: viu_extern_mem.c
 * Author: AuthorNameMagicTag
 * Create: 2025-03-06
 */

#include "drv_mcp_dependency.h"
#include "memory_config.h"
#include "viu_public.h"
#include "drv_vb_ext.h"

#ifdef VI_SELF_MANAGE_MEM_SUPPORT
#define VI_MEM_PSRAM_ADDR    (PSRAM_GUI_MEM_POOL_ADDR)
#define VI_MEM_PSRAM_LEN     (PSRAM_GUI_MEM_POOL_SIZE)
#endif

/*-------------------------------- macro define -------------------------------------------*/
#define VIU_MEM_NAME_MAX_LEN      100
#define VIU_MEM_ALIGN             16
#define VIU_MEM_CHECK_DIGIT_LEN   4
#define VIU_MEM_CHECK_DATA        0x1234abcd

#define VI_MEM_L2RAM_ADDR    (0xFFFFFFFF)
#define VI_MEM_L2RAM_LEN     (0x0)

#define uint64_ptr(ptr) ((td_void *)(td_uintptr_t)(ptr))
#define ptr_uint64(ptr) ((td_u64)(td_uintptr_t)(ptr))

typedef struct {
    td_char name[VIU_MEM_NAME_MAX_LEN];
    td_u32 phy_addr;
    td_u8 *vir_addr;
    td_u32 len;
    td_u32 align;
    td_u32 offset;
    struct osal_list_head node;
} mem_node;

typedef struct {
    td_u32 psram_addr;
    td_u32 psram_len;
    td_u32 offset;
    td_u32 free_size;
    mcp_osal_spinlock spin_lock;
    td_bool is_self_manage;
    td_bool is_psram_cache;
} liteos_mem_ctx;

/*-------------------------------- global define ------------------------------------------*/
static liteos_mem_ctx g_liteos_mem_ctx;
static struct osal_list_head g_list_mem;


static liteos_mem_ctx *liteos_mem_entry(td_void)
{
    return &g_liteos_mem_ctx;
}

static td_void liteos_mem_lock(td_ulong *lock_flag)
{
    mcp_osal_spin_lock_irqsave(&liteos_mem_entry()->spin_lock, lock_flag);
}

static td_void liteos_mem_unlock(td_ulong *lock_flag)
{
    mcp_osal_spin_unlock_irqrestore(&liteos_mem_entry()->spin_lock, lock_flag);
}

#ifdef VI_SELF_MANAGE_MEM_SUPPORT
static td_s32 liteos_mem_pool_init(td_void)
{
    td_s32 ret;

    ret = osal_pool_mem_init((td_void *)VI_MEM_PSRAM_ADDR, VI_MEM_PSRAM_LEN);
    if (ret != OSAL_SUCCESS) {
        viu_err_trace("osal_pool_mem_init failed!\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_s32 liteos_mem_pool_deinit(td_void)
{
    td_s32 ret;

    ret = osal_pool_mem_deinit((td_void *)VI_MEM_PSRAM_ADDR);
    if (ret != OSAL_SUCCESS) {
        viu_err_trace("osal_pool_mem_deinit failed!\n");
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

static td_void liteos_mem_pool_calc_free(liteos_mem_ctx *ctx)
{
    viu_unused(ctx);

    td_u32 used_len = 0;
    td_u32 node_size = (td_u32)sizeof(mem_node);
    mem_node *p_node = TD_NULL;

    if (mcp_osal_list_empty(&g_list_mem) == 0) {
        mcp_osal_list_for_each_entry(p_node, &g_list_mem, node) {
            used_len += p_node->len + node_size;
        }
    }

    ctx->offset = used_len;
    ctx->free_size = ctx->psram_len - used_len;
}
#endif

#ifdef VI_CHECK_MEM_SUPPORT
static td_void liteos_mem_add_check_data(td_u8 *vir_addr, td_u32 len)
{
    td_u32 *data = TD_NULL;

    data = (td_u32 *)vir_addr;
    if (data == TD_NULL) {
        return;
    }

    data[0] = VFMW_MEM_CHECK_DATA;
    data = (td_u32 *)(vir_addr + len - VFMW_MEM_CHECK_DIGIT_LEN);
    if (data == TD_NULL) {
        return;
    }

    data[0] = VFMW_MEM_CHECK_DATA;
}

static td_bool liteos_mem_check_mem(td_u8 *vir_addr, td_u32 len)
{
    viu_unused(len);

    td_bool is_err;
    td_u32 *data = TD_NULL;

    data = (td_u32 *)vir_addr;
    if (data == TD_NULL) {
        return  TD_TRUE;
    }

    is_err = (data[0] == VIU_MEM_CHECK_DATA);
    if (is_err != TD_TRUE) {
        return TD_TRUE;
    }

    data = (td_u32 *)(vir_addr + len - VIU_MEM_CHECK_DIGIT_LEN);
    if (data == TD_NULL) {
        return TD_TRUE;
    }

    is_err = (data[0] == VIU_MEM_CHECK_DATA);
    if (data != TD_NULL) {
        return TD_TRUE;
    }

    return TD_FALSE;
}
#endif

static td_void liteos_mem_get_real_info(td_u32 size, td_u32 align, td_u32 *real_size, td_u32 *offset)
{
#ifdef VI_CHECK_MEM_SUPPORT
    /* The alignment parameter value must be a power of 2 with the minimum value being 4 */
    /* Non-aligned The size of the input parameter size must be four byte-aligned */
    *real_size = (align == 1) ? viu_align_up(2 * VIU_MEM_CHECK_DIGIT_LEN + size, 4) : /* 2 is mult 4 byte align */
        (viu_align_up(VIU_MEM_CHECK_DIGIT_LEN, align) + size + VIU_MEM_CHECK_DIGIT_LEN);
    *offset = (align == 1) ? VIU_MEM_CHECK_DIGIT_LEN : vfmw_align_up(VIU_MEM_CHECK_DIGIT_LEN, align);
#else
    viu_unused(align);
    *real_size = size;
    *offset = 0;
#endif
}

static td_uchar *liteos_mem_pool_alloc(td_u32 size)
{
    liteos_mem_ctx *ctx = liteos_mem_entry();

    viu_info_trace("%s, %d ===> size %d, psram_addr= 0x%x\n", __func__, __LINE__, size,
        ctx->psram_addr, uint64_ptr(ctx->psram_addr));

    return (td_uchar *)osal_pool_mem_alloc(uint64_ptr(ctx->psram_addr), size);
}

static td_uchar *liteos_mem_pool_alloc_align(td_u32 size, td_u32 align)
{
    liteos_mem_ctx *ctx = liteos_mem_entry();

    return (td_uchar *)osal_pool_mem_alloc_align(uint64_ptr(ctx->psram_addr), size, align);
}

static td_void liteos_mem_pool_free(const td_uchar *vir_addr)
{
    liteos_mem_ctx *ctx = liteos_mem_entry();

    osal_pool_mem_free(uint64_ptr(ctx->psram_addr), (td_void *)vir_addr);
    return;
}

typedef struct {
    td_char *mem_name;
    td_u8 *vir_addr;
    td_phys_addr_t phy_addr;
    td_u32 length;
} mem_record; /* no use btw arm64/mcu32/tee */

typedef struct {
    mem_record rec;
    td_s8     valid;
    td_s8     use_cnt;
    td_s8     uid;     /* block id */
} vi_mem_rec;

static vi_mem_rec *g_vi_mem_rec = TD_NULL;

vb_blk_handle viu_mem_get_blk_by_size(td_u64 blk_size, td_u32 uid)
{
    viu_unused(uid);

    td_s32 i;
    vb_blk_handle blk_handle = VB_INVALID_HANDLE;

    for (i = 0; i < VICAP_BUF_NUM; i++) {
        if (g_vi_mem_rec[i].valid == 1 && g_vi_mem_rec[i].use_cnt == 0) {
            if (g_vi_mem_rec[i].rec.length < blk_size) {
                viu_err_trace("id %d is valid, memory len %d < expect size %d, skip!\n",
                    i, g_vi_mem_rec[i].rec.length, blk_size);
                continue;
            }
            blk_handle = i;
            g_vi_mem_rec[i].use_cnt++;
            break;
        }
    }

    if (blk_handle == VB_INVALID_HANDLE) {
        viu_err_trace("--------------vb_state:\n");
        for (i = 0; i < VICAP_BUF_NUM; i++) {
            viu_err_trace("[%u %u]\n", g_vi_mem_rec[i].valid, g_vi_mem_rec[i].use_cnt);
        }
    }

    return blk_handle;
}

vb_blk_handle viu_mem_phy_to_handle(td_u64 phy_addr)
{
    td_u32 i;

    for (i = 0; i < VICAP_BUF_NUM; i++) {
        if (g_vi_mem_rec[i].valid == 1 && g_vi_mem_rec[i].rec.phy_addr == phy_addr) {
            return i;
        }
    }

    return VB_INVALID_HANDLE;
}

td_u64 viu_mem_handle_to_kern(vb_blk_handle handle)
{
    if (g_vi_mem_rec == TD_NULL || handle >= VICAP_BUF_NUM) {
        viu_err_trace("blk handle %d not valid!\n", handle);
        return (td_u64)TD_NULL;
    }

    return (td_u64)g_vi_mem_rec[handle].rec.vir_addr;
}

td_u64 viu_mem_handle_to_phys(vb_blk_handle handle)
{
    if (g_vi_mem_rec == TD_NULL || handle >= VICAP_BUF_NUM) {
        viu_err_trace("blk handle %d not valid!\n", handle);
        return (td_u64)TD_NULL;
    }

    return (td_phys_addr_t)g_vi_mem_rec[handle].rec.phy_addr;
}

td_bool viu_mem_is_blk_valid(td_u32 pool_id, td_phys_addr_t phys_addr)
{
    if (pool_id >= VICAP_BUF_NUM || g_vi_mem_rec[pool_id].rec.phy_addr != phys_addr) {
        return TD_FALSE;
    }

    return TD_TRUE;
}

td_s32 viu_mem_user_add(td_u32 pool_id, td_u64 phy_addr, td_u32 uid)
{
    viu_unused(uid);

    if (viu_mem_is_blk_valid(pool_id, phy_addr) == TD_FALSE) {
        return TD_FAILURE;
    }
    if (g_vi_mem_rec[pool_id].use_cnt >= 0x7F) {
        viu_err_trace("Error: pool id %d, phyaddr 0x%x use_cnt >=0x7F!\n", pool_id, phy_addr);
        return TD_FAILURE;
    }

    g_vi_mem_rec[pool_id].use_cnt++;

    return TD_SUCCESS;
}

td_s32 viu_mem_user_sub(td_u32 pool_id, td_u64 phy_addr, td_u32 uid)
{
    viu_unused(uid);

    if (viu_mem_is_blk_valid(pool_id, phy_addr) == TD_FALSE) {
        return TD_FAILURE;
    }

    g_vi_mem_rec[pool_id].use_cnt--;
    if (g_vi_mem_rec[pool_id].use_cnt < 0) {
        viu_err_trace("Error: pool id %d, phyaddr 0x%x use_cnt < 0!\n", pool_id, phy_addr);
        g_vi_mem_rec[pool_id].use_cnt = 0;
        return TD_FAILURE;
    }

    return TD_SUCCESS;
}

static td_u32 get_main_size_for_uncompress_common_buffer(td_u32 y_size, ext_pixel_format pixel_format)
{
    switch (pixel_format) {
        case EXT_PIXEL_FORMAT_YVU_SEMIPLANAR_420:
        case EXT_PIXEL_FORMAT_YUV_SEMIPLANAR_420:
            return ((y_size * 3) >> 1); /* (3 / 2): sp420, size of c is half of y */
        case EXT_PIXEL_FORMAT_YVU_SEMIPLANAR_422:
        case EXT_PIXEL_FORMAT_YUV_SEMIPLANAR_422:
        case EXT_PIXEL_FORMAT_VY1UY0_PACKAGE_422:
            return (y_size * 2); /* 2: sp422, size of c is equal to y */
        case EXT_PIXEL_FORMAT_YUV_400:
            return y_size;
        default:
            return (y_size * 3); /* 3: default size */
    }
}

td_void viu_mem_get_calc_cfg(const ext_vb_base_info *vb_info, ext_isp_vb_cal_config *vb_calc_cfg)
{
    td_u32 width = vb_info->width;
    td_u32 bit_width = 8; /* 8: YUV fix 8 bit */
    td_u32 align_height = viu_align_up(vb_info->height, 2);  /* 2 pixel align */
    td_u32 head_stride = 0;
    td_u32 head_size = 0;
    td_u32 head_y_size = 0;
    td_u32 main_size, main_stride, y_size;
    td_u32 align = viu_align_up(vb_info->align, 16);   /* 16: logic limit */

    main_stride = viu_align_up((width * bit_width + 7) >> 3, align); // align 7, shift 3
    y_size = main_stride * align_height;
    main_size = get_main_size_for_uncompress_common_buffer(y_size, vb_info->pixel_format);
    /* main_stride needs to be multiplied by 2 when pkg422 */
    main_stride = (vb_info->pixel_format == EXT_PIXEL_FORMAT_VY1UY0_PACKAGE_422) ? main_stride * 2 : main_stride;

    vb_calc_cfg->vb_size = head_size + main_size;
    vb_calc_cfg->head_y_size = head_y_size;
    vb_calc_cfg->head_size = head_size;
    vb_calc_cfg->head_stride = head_stride;
    vb_calc_cfg->main_stride = main_stride;
    vb_calc_cfg->main_y_size = y_size;
    vb_calc_cfg->main_size = main_size;
}

td_s32 viu_mem_init(td_void)
{
    td_s32 ret;
    liteos_mem_ctx *ctx = liteos_mem_entry();

    ret = memset_s(ctx, sizeof(liteos_mem_ctx), 0, sizeof(liteos_mem_ctx));
    if (ret != EOK) {
        viu_err_trace("%s, %d memset_s fail\n", __func__, __LINE__);
        return TD_FAILURE;
    }
    mcp_osal_spin_lock_init(&ctx->spin_lock);

#ifdef VI_SELF_MANAGE_MEM_SUPPORT
    ctx->psram_addr = VI_MEM_PSRAM_ADDR;
    ctx->psram_len = VI_MEM_PSRAM_LEN;
    ctx->offset = 0;
    ctx->free_size = VI_MEM_PSRAM_LEN;
    ctx->is_self_manage = TD_TRUE;
    ctx->is_psram_cache = TD_FALSE;
    liteos_mem_pool_init();
#else
    ctx->psram_addr = uapi_sys_get_memory_pool_addr(MEM_POOL_VICAP);
    ctx->psram_len = PSRAM_HEAP_LENGTH;
    ctx->is_self_manage = TD_FALSE;
    ctx->is_psram_cache = uapi_sys_get_memory_pool_cache_attr(MEM_POOL_VICAP);
    viu_info_trace("%s, %d ==> psram_addr = 0x%x\n", __func__, __LINE__, ctx->psram_addr);
#endif
    MCP_OSAL_INIT_LIST_HEAD(&g_list_mem);

    return TD_SUCCESS;
}

td_void viu_mem_deinit(td_void)
{
    td_ulong lock_flag = 0;
    mem_node *p_node = TD_NULL;
    mem_node *next_node = TD_NULL;
    liteos_mem_ctx *ctx = liteos_mem_entry();

    liteos_mem_lock(&lock_flag);
    if (mcp_osal_list_empty(&g_list_mem) == 0) {
        mcp_osal_list_for_each_entry_safe(p_node, next_node, &g_list_mem, node) {
            if (p_node != TD_NULL) {
                liteos_mem_pool_free(p_node->vir_addr);
                mcp_osal_list_del(&(p_node->node));
                liteos_mem_pool_free((td_void *)p_node);
                p_node = TD_NULL;
            }
        }
    }

#ifdef VFMW_MANAGE_MEM_SUPPORT
    liteos_mem_pool_deinit();
#endif

    liteos_mem_unlock(&lock_flag);

    mcp_osal_spin_lock_destroy(&ctx->spin_lock);
}

td_s32 vi_mem_init(td_void)
{
    td_s32 ret;
    ret = viu_mem_init();
    if (ret != TD_SUCCESS) {
        viu_err_trace("vi mem init failed! ret 0x%x\n", ret);
        goto err0;
    }

    g_vi_mem_rec = mcp_osal_vmalloc(sizeof(vi_mem_rec) * VICAP_BUF_NUM);
    if (g_vi_mem_rec == TD_NULL) {
        viu_err_trace("vi mem init failed! ret 0x%x\n", ret);
        goto err1;
    }
    (td_void)memset_s(g_vi_mem_rec, sizeof(vi_mem_rec) * VICAP_BUF_NUM, 0, sizeof(vi_mem_rec) * VICAP_BUF_NUM);

    return ret;

err1:
    viu_mem_deinit();
err0:
    return ret;
}

td_void vi_mem_deinit(td_void)
{
    if (g_vi_mem_rec != TD_NULL) {
        mcp_osal_vfree(g_vi_mem_rec);
        g_vi_mem_rec = TD_NULL;
    }
    viu_mem_deinit();
}

static td_s32 viu_mem_alloc(const td_char *name, td_u32 size, td_u32 align, mem_record *mem_rec)
{
    td_s32 ret;
    td_ulong lock_flag = 0;
    td_u8 *vir_addr = TD_NULL;
    mem_node *p_node = TD_NULL;
    td_u32 real_size;
    td_u32 offset;

    p_node = (mem_node *)liteos_mem_pool_alloc(sizeof(mem_node));
    if (p_node == TD_NULL) {
        return TD_FAILURE;
    }

    liteos_mem_lock(&lock_flag);

    liteos_mem_get_real_info(size, align, &real_size, &offset);
    if (align == 1) {
        vir_addr = liteos_mem_pool_alloc(real_size);
    } else {
        vir_addr = liteos_mem_pool_alloc_align(real_size, align);
    }
    if (vir_addr == NULL) {
        goto err2;
    }

    ret = strcpy_s(p_node->name, sizeof(p_node->name), name);
    if (ret != EOK) {
        goto err3;
    }

    p_node->vir_addr = vir_addr;
    p_node->phy_addr = (td_u32)ptr_uint64(vir_addr);
    p_node->len = real_size;
    p_node->align = align;
    p_node->offset = offset;
#ifdef VI_CHECK_MEM_SUPPORT
    liteos_mem_add_check_data(vir_addr, real_size);
#endif
    mcp_osal_list_add_tail(&(p_node->node), &g_list_mem);

    mem_rec->length = size;
    mem_rec->phy_addr = p_node->phy_addr + offset;
    mem_rec->vir_addr = p_node->vir_addr + offset;

#ifdef MEM_DEBUG_PRINT_EN
    viu_err_trace("alloc name %s alloc phy_addr (0x%x) p_node %p\r\n", name, p_node->phy_addr, p_node);
#endif
    liteos_mem_unlock(&lock_flag);
    return TD_SUCCESS;

err3:
    liteos_mem_pool_free(vir_addr);
err2:
    liteos_mem_pool_free((td_void *)p_node);
    liteos_mem_unlock(&lock_flag);
    return TD_FAILURE;
}

static td_void viu_mem_free(mem_record *mem)
{
    td_ulong lock_flag = 0;
    td_u32 is_found = 0;
    mem_node *p_node = TD_NULL;

    if (mcp_osal_list_empty(&g_list_mem) == 1) {
        viu_err_trace("%s, %d list empty.free err 0x%x \r\n", __func__, __LINE__, mem->phy_addr);
        return;
    }

    liteos_mem_lock(&lock_flag);

    mcp_osal_list_for_each_entry(p_node, &g_list_mem, node) {
        if (p_node->phy_addr + p_node->offset == mem->phy_addr) {
            mcp_osal_list_del(&(p_node->node));
            is_found = 1;
            break;
        }
    }

    if (is_found == 1) {
#ifdef MEM_DEBUG_PRINT_EN
        viu_err_trace("free phy_addr 0x%x p_node %p \r\n", mem->phy_addr, p_node);
#endif
        liteos_mem_pool_free(p_node->vir_addr);
        liteos_mem_pool_free((td_void *)p_node);
        p_node = TD_NULL;
#ifdef MEM_DEBUG_PRINT_EN
        viu_err_trace("====free phy_addr ok==== \r\n");
#endif
    } else {
        viu_err_trace("%s, %d phy_addr 0x%x is not found !\n", __func__, __LINE__, mem->phy_addr);
    }
    liteos_mem_unlock(&lock_flag);
}

td_s32 vi_mem_create_private_pool(td_u32 blk_cnt, td_u64 blk_size)
{
    td_u32 i;
    td_s32 ret;
    mem_record mem_rec = {};

    if (g_vi_mem_rec == TD_NULL) {
        viu_err_trace("vi mem ctx not ready!!!\n");
        return TD_FAILURE;
    }

    for (i = 0; i < blk_cnt; i++) {
        ret = viu_mem_alloc("vi_buf", blk_size, 0x10, &mem_rec);
        if (ret != TD_SUCCESS) {
            viu_err_trace("viu alloc%u memory size %d failed! ret 0x%x\n", i, blk_size, ret);
            goto err;
        }

        /* add to list */
        g_vi_mem_rec[i].rec = mem_rec;
        g_vi_mem_rec[i].use_cnt = 0;
        g_vi_mem_rec[i].valid = 1;
        g_vi_mem_rec[i].uid = i;
    }

    return TD_SUCCESS;

err:
    for (i = 0; i < blk_cnt; i++) {
        if (g_vi_mem_rec[i].valid) {
            viu_mem_free(&g_vi_mem_rec[i].rec);
        }
    }
    (td_void)memset_s(g_vi_mem_rec, sizeof(vi_mem_rec) * blk_cnt, 0, sizeof(vi_mem_rec) * blk_cnt);
    return TD_FAILURE;
}

static td_bool check_private_pool_free(td_void)
{
    td_s32 i;
    td_bool flag = TD_FALSE;

    for (i = 0; i < VICAP_BUF_NUM; i++) {
        if (g_vi_mem_rec[i].use_cnt != 0) {
            viu_err_trace("[%d,%d]\n", i, g_vi_mem_rec[i].use_cnt);
            flag = TD_TRUE;
            break;
        }
    }

    return (flag == TD_FALSE) ? TD_TRUE : TD_FALSE;
}

td_s32 vi_mem_destroy_private_pool(td_void)
{
    td_s32 i;

    if (check_private_pool_free() != TD_TRUE) {
        viu_err_trace("vi pool still inuse!\n");
        return TD_FAILURE;
    }

    for (i = 0; i < VICAP_BUF_NUM; i++) {
        if (g_vi_mem_rec[i].valid) {
            viu_mem_free(&g_vi_mem_rec[i].rec);
        }
    }
    (td_void)memset_s(g_vi_mem_rec, sizeof(vi_mem_rec) * VICAP_BUF_NUM, 0, sizeof(vi_mem_rec) * VICAP_BUF_NUM);

    return TD_SUCCESS;
}

#ifdef VI_CHECK_MEM_SUPPORT
td_u32 viu_mem_check(td_void)
{
    td_u32 is_find = 0;
    td_bool mem_err = TD_FALSE;
    td_ulong lock_flag = 0;
    mem_node *p_node = TD_NULL;

    liteos_mem_lock(&lock_flag);
    if (mcp_osal_list_empty(&g_list_mem) == 0) {
        mcp_osal_list_for_each_entry(p_node, &g_list_mem, node) {
            mem_err = liteos_mem_check_mem(p_node->vir_addr, p_node->len);
            if (mem_err == TD_TRUE) {
                viu_err_trace("====mem: %s is damaged, pls check!==== \r\n", p_node->name);
                is_find = 1;
            }
        }
    }
    liteos_mem_unlock(&lock_flag);

    return (is_find == 1) ? TD_TRUE : TD_FALSE;
}

td_void viu_mem_record(td_void)
{
    td_s32 ret;
    td_s32 len;
    td_bool mem_err = TD_FALSE;
    td_ulong lock_flag = 0;
    os_file* tmp_file =  NULL;
    mem_node *p_node = TD_NULL;
    td_char file_name[VFMW_MEM_NAME_MAX_LEN] = {0};
    td_char buf[VFMW_MEM_NAME_MAX_LEN];
    td_char *dbg_path = dbg_get_dbg_path();

    ret = snprintf_s(file_name, sizeof(file_name), sizeof(file_name) - 1, "%s/%s", dbg_path, "vfmw_err_mem.txt");
    vfmw_check(ret >= 0);

    tmp_file = os_fopen(file_name, OS_RDWR | OS_CREATE | OS_TRUNC, 0);
    vfmw_check(tmp_file != TD_NULL);

    liteos_mem_lock(&lock_flag);
    ret = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "%-19s|%-19s|%-19s|%-10s|%-8s|\r\n",
            "buf_name", "phy_addr", "buf_len", "align", "mem_err");
    if (ret < 0) {
        goto out;
    }

    len = os_fwrite(buf, strlen(buf), tmp_file);
    if (len != strlen(buf)) {
        goto out;
    }

    if (mcp_osal_list_empty(&g_list_mem) == 0) {
        mcp_osal_list_for_each_entry(p_node, &g_list_mem, node) {
            mem_err = liteos_mem_check_mem(p_node->vir_addr, p_node->len);
            if (mem_err == TD_TRUE) {
                viu_err_trace("====mem: %s is damaged, pls check: %s!==== \r\n", p_node->name, file_name);
            }
        }
    }
out:
    os_fclose(tmp_file);
    liteos_mem_unlock(&lock_flag);
}
#endif

