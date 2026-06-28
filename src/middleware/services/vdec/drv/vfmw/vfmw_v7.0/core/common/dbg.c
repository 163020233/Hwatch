/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "dbg.h"
#include "vfmw_pdt.h"
#include "soc_osal.h"
#include "vfmw_sys.h"

#ifdef NO_UART_DEBUG
#include "diag_log.h"
#endif

#define TMP_MSG_SIZE 2014

#define TRACE_MEM_SIZE    (16 * 1024 * 2)
#define MAX_FUNC_NAME_LEN 15
#define DBG_PATH_MAX_LEN  50
#define TRACE_NUMBER_SIZE 1000

#define VALID_FLAG_0 0x1a2a3a4a
#define VALID_FLAG_1 0x55657585
#define VALID_FLAG_2 0x0f152f35
#define VALID_FLAG_3 0x4a5f6a7f
#define MAX_PIX_WIDTH 8192
#define LOG_FMT_HEAD_TIME   "------------------------------------------time(us)-----------------------" \
                            "--------------------\n"
#define LOG_FMT_HEAD_DIFF   "------------------------------------------diff(us)-----------------------" \
                            "--------------------\n"
typedef struct {
    td_char func[MAX_FUNC_NAME_LEN + 1];
    td_s32 line;
    td_s32 data;
    td_u32 time;
} trace_obj;

typedef struct {
    td_s32 max_obj_num_minus4;
    td_s32 obj_num;
    td_u32 valid_flag[4]; /* 4 :a number */
    UADDR trace_phy;
    trace_obj *first_obj;
} trace_entity;

typedef struct {
    td_s32 cur_index;
    UADDR trace_phy;
    td_u32 *obj;
    td_u32 size;

    td_u32 valid_flag[4]; /* 4 :a number */
} trace_num_entity;

td_u32 g_print_word =  (1 << PRN_FATAL);
print_dev g_print_out = PRN_DEV_COM;

static trace_entity *g_tracer = TD_NULL;
static UADDR g_tracer_phy = 0;

static trace_num_entity *g_tracer_num = TD_NULL;
static UADDR g_tracer_num_phy = 0;
static td_char g_dbg_path[DBG_PATH_MAX_LEN] = "/user/test";

STATIC td_u32 str_to_val_get_index(const td_char *str)
{
    td_u32 i;

    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        i = 2; /* 0x start need skip 2 chars */
    } else {
        i = 0;
    }

    return i;
}

STATIC td_u32 str_to_val_get_weight(const td_char *str)
{
    td_u32 weight;

    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        weight = 16; /* 0x start is hex so weight is 16 */
    } else {
        weight = 10; /* none start is decimal so weight is 10 */
    }

    return weight;
}

td_s32 str_to_val(const td_char *str, td_u32 *data)
{
    td_u32 i, dat, weight;
    td_s32 d;

    vfmw_check_ret(str != TD_NULL, -1);
    vfmw_check_ret(data != TD_NULL, -1);

    dat = 0;
    i = str_to_val_get_index(str);
    weight = str_to_val_get_weight(str);

    for (; i < 10; i++) { /* up to vfmw_max bit hex has 10 chars a hexnum total */
        if (str[i] < 0x20) { /* 0x20 is uplimit of invalid char */
            break;
        } else if (weight == 16 && str[i] >= 'a' && str[i] <= 'f') { /* 16 is hex */
            d = str[i] - 'a' + 10; /* 'a' means 10 */
        } else if (weight == 16 && str[i] >= 'A' && str[i] <= 'F') { /* 16 is hex */
            d = str[i] - 'A' + 10; /* 'A' means 10 */
        } else if (str[i] >= '0' && str[i] <= '9') {
            d = str[i] - '0';
        } else {
            return -1; /* invalid num return -1 means Fail */
        }

        if ((dat * weight) < VFMW_MAX_UINT32) {
            dat = dat * weight + (td_u32)d;
        }
    }

    *data = dat;

    return 0;
}

td_s32 dbg_create_tracer(td_void)
{
    td_s32 ret;
    vfmw_mem_desc tmp_desc;
    const vfmw_product_info *pdt_info = pdt_get_product_info();

    if (g_tracer != TD_NULL) {
        return VFMW_DBG_OK;
    }
    vfmw_check_ret(os_kmem_alloc != TD_NULL, VFMW_DBG_ERR);

    tmp_desc.mode = (pdt_info->smmu_support == TD_TRUE) ? MEM_MMU_MMU : MEM_CMA_CMA;
    tmp_desc.unid = 0;
    ret = os_kmem_alloc("vfmw_tracer", TRACE_MEM_SIZE, 16, 0, &tmp_desc); /* 16 :a number */
    if (ret != OSAL_OK) {
        return VFMW_DBG_ERR;
    }

    g_tracer_phy = tmp_desc.phy_addr;
    g_tracer = uint64_ptr(tmp_desc.vir_addr);

    vfmw_check_sec_func(memset_s(uint64_ptr(tmp_desc.vir_addr), TRACE_MEM_SIZE, 0, TRACE_MEM_SIZE));

    g_tracer->first_obj = uint64_ptr(tmp_desc.vir_addr + (td_u32)sizeof(trace_entity));
    g_tracer->trace_phy = (g_tracer_phy + (td_u32)sizeof(trace_entity));
    g_tracer->max_obj_num_minus4 = (td_s32)((tmp_desc.length - (td_u32)sizeof(trace_entity)) /
        (td_u32)sizeof(trace_obj) - 4); /* 4 :a number */
    g_tracer->obj_num = 0;

    g_tracer->valid_flag[0] = VALID_FLAG_0;
    g_tracer->valid_flag[1] = VALID_FLAG_1;
    g_tracer->valid_flag[2] = VALID_FLAG_2; /* 2 :a number */
    g_tracer->valid_flag[3] = VALID_FLAG_3; /* 3 :a number */

    dprint(PRN_ALWS, "==== mem_phy, trace_phy, trace_max = 0x%x, 0x%x,%d ====\n",
           g_tracer_phy, g_tracer->trace_phy, g_tracer->max_obj_num_minus4);

    return VFMW_DBG_OK;
}

td_void dbg_destroy_tracer(td_void)
{
    vfmw_mem_desc tmp_desc;

    if (g_tracer == TD_NULL) {
        return;
    }

    if (g_tracer_phy == 0) {
        dprint(PRN_ERROR, "%s, Phyaddr == 0\n", __func__);
        return;
    }

    vfmw_check_sec_func(memset_s(&tmp_desc, sizeof(tmp_desc), 0, sizeof(tmp_desc)));
    tmp_desc.phy_addr = g_tracer_phy;
    tmp_desc.vir_addr = ptr_uint64(g_tracer);
    tmp_desc.length = TRACE_MEM_SIZE;

    os_kmem_free(&tmp_desc);
    g_tracer = TD_NULL;

    return;
}

td_void dbg_add_trace(const td_char *trace_string, td_s32 trace_line, td_s32 trace_data)
{
    trace_obj *obj = TD_NULL;

    if (g_tracer == TD_NULL) {
        return;
    }

    vfmw_check(os_get_time_us != TD_NULL);
    vfmw_check(os_strlcpy != TD_NULL);

    obj = g_tracer->first_obj + (g_tracer->obj_num % g_tracer->max_obj_num_minus4);
    g_tracer->obj_num++;

    obj->line = trace_line;
    obj->data = trace_data;

    obj->time = os_get_time_us();
    os_strlcpy(obj->func, trace_string, MAX_FUNC_NAME_LEN);

    return;
}

STATIC td_void dbg_print_tracer_info(const trace_entity *tracer)
{
    td_s32 i;
    td_s32 obj_num;
    trace_obj *obj = TD_NULL;

    (td_void)obj; /* avoid to build err when printing is not supported. */
    dprint(PRN_ALWS, "====================== tracer context =====================\n");
    dprint(PRN_ALWS, "first_obj");
    dprint(PRN_ALWS, "%-25s :%p\n", "trace_phy", tracer->trace_phy);
    dprint(PRN_ALWS, "%-25s :%d\n", "max_obj_num_minus4", tracer->max_obj_num_minus4);
    dprint(PRN_ALWS, "%-25s :%d\n", "obj_num", tracer->obj_num);

    /* only print those msg we added */
    obj_num = tracer->obj_num;
    for (i = 0; i < tracer->max_obj_num_minus4 && i < tracer->obj_num; i++) {
        obj_num = (obj_num - 1 + tracer->max_obj_num_minus4) % tracer->max_obj_num_minus4;
        obj = tracer->first_obj + obj_num;

        dprint(PRN_ALWS, "-%05d-", i);
        dprint(PRN_ALWS, "func:%-15s;", obj->func);
        dprint(PRN_ALWS, " line:%5d;", obj->line);
        dprint(PRN_ALWS, " data:0x%08x(%-10d);", obj->data, obj->data);
        dprint(PRN_ALWS, " time:%u\n", obj->time);
    }

    return;
}

td_void dbg_print_tracer(td_void)
{
    trace_entity *tracer = TD_NULL;
    td_u32 *valid_flag = TD_NULL;

    vfmw_check_print(g_tracer_phy != 0, "g_tracer_phy is invalid\n");
    vfmw_check(os_ioremap != TD_NULL);

    if (g_tracer == TD_NULL) {
        tracer = (trace_entity *)os_ioremap(g_tracer_phy, TRACE_MEM_SIZE);
        if (tracer == TD_NULL) {
            dprint(PRN_ALWS, "map vir address for '%x' failed!!!\n", g_tracer_phy);
            return;
        }

        tracer->first_obj = (trace_obj *)((td_u8 *)tracer + sizeof(trace_entity));
    } else {
        tracer = g_tracer;
    }

    if (tracer == TD_NULL) {
        dprint(PRN_ALWS, "map vir address for '%x' failed!!!\n", g_tracer_phy);
        return;
    }

    valid_flag = &tracer->valid_flag[0];

    if (valid_flag[0] != VALID_FLAG_0 || valid_flag[1] != VALID_FLAG_1 ||
        valid_flag[2] != VALID_FLAG_2 || valid_flag[3] != VALID_FLAG_3) { /* 2 :a number 3 :a number */
        dprint(PRN_ALWS, "no valid trace info stored in '%x'!!!\n", g_tracer_phy);
        goto out;
    }

    if (tracer->first_obj == TD_NULL) {
        dprint(PRN_ALWS, "tracer->first_obj address is null\n");
        goto out;
    }

    dbg_print_tracer_info(tracer);

out:
    if (g_tracer == TD_NULL) {
        os_iounmap((td_u8 *)tracer);
    }

    return;
}

td_void dbg_create_tracer_number(td_void)
{
    td_s32 ret;
    vfmw_mem_desc tmp_desc;
    td_u32 tracer_buf_size = TRACE_NUMBER_SIZE * (td_u32)sizeof(td_s32) + (td_u32)sizeof(trace_num_entity);
    const vfmw_product_info *pdt_info = pdt_get_product_info();

    if (g_tracer_num != TD_NULL) {
        return;
    }
    vfmw_check(os_kmem_alloc != TD_NULL);

    tmp_desc.mode = (pdt_info->smmu_support == TD_TRUE) ? MEM_MMU_MMU : MEM_CMA_CMA;
    tmp_desc.unid = 0;
    ret = os_kmem_alloc("VFMW_Tracer_Number", tracer_buf_size, 16, 0, &tmp_desc); /* 16 :a number */
    if (ret != OSAL_OK) {
        return;
    }

    g_tracer_num_phy = tmp_desc.phy_addr;
    g_tracer_num = uint64_ptr(tmp_desc.vir_addr);

    vfmw_check_sec_func(memset_s(uint64_ptr(tmp_desc.vir_addr), tracer_buf_size, 0, tracer_buf_size));

    g_tracer_num->obj = uint64_ptr(tmp_desc.vir_addr + (td_u32)sizeof(trace_num_entity));
    g_tracer_num->trace_phy = (UADDR)(g_tracer_num_phy + (td_u32)sizeof(trace_num_entity));
    g_tracer_num->cur_index = 0;
    g_tracer_num->valid_flag[0] = VALID_FLAG_0;
    g_tracer_num->valid_flag[1] = VALID_FLAG_1;
    g_tracer_num->valid_flag[2] = VALID_FLAG_2; /* 2 :a number */
    g_tracer_num->valid_flag[3] = VALID_FLAG_3; /* 3 :a number */
    g_tracer_num->size = tracer_buf_size;

    dprint(PRN_ALWS, "== mem_phy, g_tracer_num_phy = 0x%x, size :0x%x ==\r\n", g_tracer_num_phy, g_tracer_num->size);

    return;
}

td_void dbg_destroy_tracer_number(td_void)
{
    vfmw_mem_desc tmp_desc;

    if (g_tracer_num == TD_NULL) {
        return;
    }

    if (g_tracer_num_phy == 0) {
        dprint(PRN_ERROR, "%s, Phyaddr == 0\n", __func__);
        return;
    }

    vfmw_check_sec_func(memset_s(&tmp_desc, sizeof(tmp_desc), 0, sizeof(tmp_desc)));
    tmp_desc.phy_addr = g_tracer_num_phy;
    tmp_desc.vir_addr = ptr_uint64(g_tracer_num);
    tmp_desc.length = g_tracer_num->size;

    os_kmem_free(&tmp_desc);
    g_tracer_num = TD_NULL;

    return;
}

td_void dbg_add_trace_number(td_u32 trace_data)
{
    td_s32 idx;

    if (g_tracer_num == TD_NULL) {
        return;
    }
    if (g_tracer_num->cur_index == 0) {
        vfmw_check_sec_func(memset_s(g_tracer_num->obj, sizeof(td_s32) * TRACE_NUMBER_SIZE, 0,
            sizeof(td_s32) * TRACE_NUMBER_SIZE));
    }

    idx = g_tracer_num->cur_index;
    g_tracer_num->obj[idx] = trace_data;
    g_tracer_num->cur_index++;
    g_tracer_num->cur_index = (g_tracer_num->cur_index % TRACE_NUMBER_SIZE);
}

STATIC td_s32 dbg_save_linear(td_s32 chan_id, const vfmw_image *image, os_file *save_file)
{
    td_s32 len;
    td_u8 *tmp_mem = TD_NULL;
    td_u32 width;
    td_u32 height;
    td_u32 bit_depth;
    td_u32 luma_size = 0;
    td_u32 stride;
    td_u32 pix_byte;

    vfmw_unused(chan_id);
    width = image->disp_info.disp_width;
    height = image->disp_info.disp_height;
    stride = image->disp_info.y_stride;
    bit_depth = image->bit_depth;
    pix_byte = (bit_depth > 8) ? 2 : 1; /* 8: default 8bit, 2: 2 bytes per pixel, 1: 1 byte per pixel */
    if ((width * height < VFMW_MAX_SINT32) && (width * height * pix_byte < VFMW_MAX_SINT32)) {
        luma_size = width * height * pix_byte;
    } else {
        dprint(PRN_ERROR, "%s %d overflow!\n", __func__, __LINE__);
    }

    os_print("save linear width %d, height %d, bit_depth %d, stride %d.\n", width, height, bit_depth, stride);

    tmp_mem = (td_u8 *)os_alloc_vir(chan_id, "vfmw_dbg_yuv", (td_s32)luma_size * 3 / 2); /* 3 2 num */
    if (tmp_mem == TD_NULL) {
        dprint(PRN_ALWS, "AllocVirMem Fail\n");
        return VFMW_DBG_ERR;
    }
    vfmw_check_sec_func(memset_s(tmp_mem, luma_size * 3 / 2, 0xff, luma_size * 3 / 2)); /* 3 :a number 2 :a number */
    pdt_yuv_2d(image, tmp_mem);
    len = os_fwrite((td_char *)tmp_mem, luma_size * 3 / 2, save_file); /* 3 :a number 2 :a number */
    if (len < (td_s32)luma_size * 3 / 2) { /* 3 :a number 2 :a number */
        dprint(PRN_ALWS, "%s write yuv data failed.\n", __func__);
    }
    os_fsync(save_file);

    os_free_vir((td_void *)tmp_mem);
    return VFMW_DBG_OK;
}

td_s32 dbg_save_yuv(td_s32 chan_id, vfmw_image *image, os_file *save_file)
{
    td_s32 ret = VFMW_DBG_ERR;
    td_bool is_map = TD_FALSE;
    mem_record mem = { 0 };

    if (image == TD_NULL || save_file == TD_NULL) {
        return VFMW_DBG_ERR;
    }

    if (image->disp_info.luma_phy_addr == 0) {
        dprint(PRN_ALWS, "luma_phy_addr = 0\n");
        return VFMW_DBG_ERR;
    }

    if (image->disp_info.luma_vir_addr == 0) {
        mem.dma_buf = image->disp_info.frm_dma_buf;
        mem.phy_addr = image->disp_info.luma_phy_addr;
        mem.vir_addr = os_mmap(&mem);
        image->disp_info.luma_vir_addr = ptr_uint64(mem.vir_addr);
        if (image->disp_info.luma_vir_addr == 0) {
            dprint(PRN_ALWS, "luma_vir_addr = 0\n");
            return VFMW_DBG_ERR;
        }
        is_map = TD_TRUE;
    }

#ifndef VFMW_PRSAM_FLUSH_ALL_CACHE_SUPPORT
    os_flush_cache(uint64_ptr(image->disp_info.luma_vir_addr), image->disp_info.luma_phy_addr,
        image->disp_info.y_stride * image->disp_info.disp_height * 3 / 2); /* 3 :a number 2 :a number */
#endif
    if (image->disp_info.linear_en == 1) {
        ret = dbg_save_linear(chan_id, image, save_file);
    } else {
        dprint(PRN_ALWS, "only support linear!\n");
    }

    if (is_map == TD_TRUE) {
        os_unmap(&mem);
        image->disp_info.luma_vir_addr = 0;
    }

    return ret;
}

td_s32 dbg_save_mtdt(td_s32 chan_id, const vfmw_image *image, os_file *save_file)
{
    td_u8 *vir_addr = TD_NULL;
    td_u32 size;

    vfmw_unused(chan_id);
    vfmw_check_ret(image != TD_NULL, VFMW_DBG_ERR);
    vfmw_check_ret(save_file != TD_NULL, VFMW_DBG_ERR);

    vir_addr = uint64_ptr(image->hdr_input.hdr_metadata_vir_addr);
    size = image->hdr_input.hdr_metadata_size;

    if (vir_addr == TD_NULL) {
        dprint(PRN_ALWS, "metadata vir addr = 0\n");
        return VFMW_DBG_ERR;
    }

    if (size == 0) {
        dprint(PRN_ALWS, "metadata size = 0\n");
        return VFMW_DBG_ERR;
    }

    if (os_fwrite != TD_NULL) {
        os_fwrite((td_char *)vir_addr, size, save_file);
    }
    os_fsync(save_file);

    dprint(PRN_ALWS, "Save metadata size %d.\n", size);

    return VFMW_DBG_OK;
}

td_s32 dbg_set_print_word(td_u32 word)
{
    g_print_word = word;

    return VFMW_DBG_OK;
}

td_u32 dbg_get_print_word(td_void)
{
    return g_print_word;
}

print_dev dbg_get_print_dev(td_void)
{
    return g_print_out;
}

td_void dbg_set_print_dev(print_dev out)
{
    g_print_out = out;
}

char *dbg_show_std(vfmw_vid_std std, const vfmw_std_ext *ext)
{
    td_s32 i;
    td_s32 num;
    char *str = "NA";
    struct std2str {
        vfmw_vid_std std;
        char *str;
    } tmp[] = {
        {VFMW_HEVC, "H265"},
        {VFMW_H264, "H264"},
        {VFMW_MJPEG, "MJPEG"},
    };

    num = (td_s32)(sizeof(tmp) / sizeof(tmp[0]));
    for (i = 0; i < num; i++) {
        if (std == tmp[i].std) {
            str = tmp[i].str;
            break;
        }
    }

    if (std != VFMW_VC1 || ext == TD_NULL) {
        return str;
    } else {
        return (ext->vc1_ext.is_adv_profile != 0) ? "VC1ap" : "VC1smp";
    }
}

STATIC td_char *g_tmp_msg = TD_NULL;

td_s32 dbg_init(td_void)
{
    if (g_tmp_msg != TD_NULL) {
        return VFMW_DBG_OK;
    }

    if (os_alloc_vir == TD_NULL) {
        os_print("dbg_init os_alloc_vir is null!\r\n");
        return VFMW_DBG_ERR;
    }
    g_tmp_msg = (td_char *)os_alloc_vir(0, "dbg_tmp_msg", TMP_MSG_SIZE);
    if (g_tmp_msg == TD_NULL) {
        os_print("alloc g_tmp_msg fail!\r\n");
        return VFMW_DBG_ERR;
    }

    return VFMW_DBG_OK;
}

td_void dbg_deinit(td_void)
{
    if (g_tmp_msg != TD_NULL) {
        os_free_vir(g_tmp_msg);
        g_tmp_msg = TD_NULL;
    }
}

VFMW_ACC td_s32 dbg_print(td_u32 type, const td_char *format, ...)
{
    OS_VA_LIST args;
    td_s32 nchar;
    const td_char *prefix = "[vdec]";
    td_u32 prefix_len = (td_u32)strlen(prefix);

    if ((type != PRN_ALWS) && ((g_print_word & (1 << type)) == 0)) {
        return -1;
    }

    if (g_tmp_msg == TD_NULL) {
        return -1;
    }

    strncpy_s((td_char *)g_tmp_msg, TMP_MSG_SIZE, prefix, prefix_len);

    OS_VA_START(args, format);
    nchar = vsnprintf_s((td_char *)g_tmp_msg + prefix_len, TMP_MSG_SIZE - prefix_len,
        TMP_MSG_SIZE - prefix_len - 1, format, args);
    g_tmp_msg[TMP_MSG_SIZE - 1] = '\0';
    OS_VA_END(args);

    if ((nchar <= 0) || (nchar > TMP_MSG_SIZE - (td_s32)prefix_len)) {
        return -1;
    }

    if (g_print_out == PRN_DEV_COM) {
#ifdef NO_UART_DEBUG
        uapi_diag_error_log(0, "%s", g_tmp_msg);
#else
        os_print("%s", g_tmp_msg);
#endif
    } else if ((g_print_out == PRN_DEV_LOG) && (os_wr_log != TD_NULL)) {
        os_wr_log("%s", g_tmp_msg);
    }
    return 0;
}

td_void dbg_add_time(td_s32 chan_id, td_u32 fid, td_u32 pid, td_u32 time)
{
    vfmw_unused(chan_id);
    vfmw_unused(fid);
    vfmw_unused(pid);
    vfmw_unused(time);
}

td_s32 dbg_show_time(td_void *buf)
{
    vfmw_unused(buf);

    return EXT_SUCCESS;
}

td_void dbg_read_reg(td_void)
{
    td_s32 i;
    td_s32 ret;
    td_char data[100] = {0};
    td_char file_name[256] = {0};
    os_file *tmp_file = TD_NULL;
    td_u32 *u32_ptr;
    td_s32 len;
    td_u8 *base_vir_addr = TD_NULL;
    td_char *dbg_path = dbg_get_dbg_path();

    ret = snprintf_s(file_name, sizeof(file_name), sizeof(file_name) - 1, "%s/%s", dbg_path, "vfmw_reg.txt");
    if (ret < 0) {
        dprint(PRN_ALWS, "%s, test_read_reg snprintf_s err\r\n", __func__);
        return;
    }

    tmp_file = os_fopen(file_name, OS_RDWR | OS_CREATE | OS_TRUNC, OS_OPEN_MODE);
    if (tmp_file == TD_NULL) {
        dprint(PRN_ALWS, "%s, dbg_read_reg fopen err\r\n", __func__);
        return;
    }

    base_vir_addr = os_kmap_reg(VDH_BASE_PHY_ADDR, VDH_REGISTER_LEN);
    if (base_vir_addr == TD_NULL) {
        dprint(PRN_ALWS, "%s, %d base_vir_addr = NULL\n", __func__, __LINE__);
        goto out;
    }

    for (i = 0; i < VDH_REGISTER_LEN / 16; i++) { /* 16 a num */
        u32_ptr = (td_u32 *)(base_vir_addr + i * 16); /* 16 a num */
        ret = snprintf_s(data, sizeof(data), sizeof(data) - 1, "%08x = %08x %08x %08x %08x\n",
            VDH_BASE_PHY_ADDR + i * 16, u32_ptr[0], u32_ptr[1], u32_ptr[2], u32_ptr[3]); /* 16 3 2 a num */
        if (ret < 0) {
            dprint(PRN_ALWS, "%s, read snprintf_s err\r\n", __func__);
            break;
        }
        len = os_fwrite(data, strlen(data), tmp_file);
        if (len < (td_s32)strlen(data)) {
            dprint(PRN_ALWS, "%s, os_fwrite err\r\n", __func__);
            break;
        }
    }
    os_fsync(tmp_file);

    os_kunmap_reg(base_vir_addr);
out:
    os_fclose(tmp_file);
}

td_s32 dbg_set_dbg_path(td_char *path, td_u32 path_len)
{
    td_s32 ret;

    vfmw_check_ret((path != TD_NULL) && (path_len != 0) && (path_len < DBG_PATH_MAX_LEN), EXT_FAILURE);

    ret = strcpy_s(g_dbg_path, DBG_PATH_MAX_LEN, path);
    vfmw_check_ret(ret == EOK, EXT_FAILURE);

    return EXT_SUCCESS;
}

td_char *dbg_get_dbg_path(td_void)
{
    return g_dbg_path;
}
