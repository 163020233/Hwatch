/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: At gpu function \n
 *
 */

#include <string.h>
#include "tcxo.h"
#include "at_cmd.h"
#include "at_gpu_cmd_register.h"
#include "at_gpu_cmd_table.h"
#include "debug_print.h"
#include "drv_gpu_dfx.h"
#include "drv_gfx_type.h"
#ifdef SUPPORT_GPU_DIR_EXIST
#include "dpu_dfx.h"
#endif
#include "at_gpu.h"
#ifdef CONFIG_DFX_SUPPORT_PRINT
#include "dfx_print.h"
#endif
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "soc_osal.h"
#include "soc_vau_api.h"
#include "soc_dpu_api.h"
#include "soc_mipi_tx.h"
#include "lcd_bus.h"
#include "lcd_mipi.h"
#include "mipi_tx.h"

#ifdef SUPPORT_GPU_TEST
#include "test_gpu_comm.h"
#endif

#define TEST_DPU_LAYER_BLUE   0xff0000ff
#define TEST_DPU_LAYER_RED    0xffff0000
#define TEST_DPU_LAYER_GREEN  0xff00ff00
#define TEST_DPU_LAYER_WHITE  0xffffffff
#define TEST_DPU_LAYER_YELLOW 0xffffff00
#define wstp_dsi_short_packet(p1, p0) ((((p1) & 0xff) << 8) | ((p0) & 0xff))

#if defined(GPU_3322)
#define TEST_GPU_PSRAM_ADDR  0x22800000
#define TEST_GPU_SRAM_ADDR   0x20340000
#define DPU_REG_BASE_ADDR    0x52090000
#elif defined(GPU_1A22)
#define TEST_GPU_PSRAM_ADDR  0xC9800000
#define TEST_GPU_SRAM_ADDR   0xb0000000
#define DPU_REG_BASE_ADDR    0x53580000
#else // 1A21
#define TEST_GPU_PSRAM_ADDR  0x22000000
#define TEST_GPU_SRAM_ADDR   0x20340000
#define DPU_REG_BASE_ADDR    0x52090000
#endif

#if defined(GPU_3322)
#define TEST_VAU_FB_CMP_MODE  EXT_GFX_COMPRESS_HFBC_2X
#else
#define TEST_VAU_FB_CMP_MODE  EXT_GFX_COMPRESS_NONE
#endif

typedef struct {
    td_u32 width;
    td_u32 height;
} wstp_lcd_info;

static td_void wstp_draw_box(ext_rect *rect, td_u32 stride, td_char *mem_start, td_u32 color, td_u32 bpp)
{
    td_char *current_mem = TD_NULL;
    td_u32 column, row;
    td_u32 convert_color = color;

    for (column = (td_u32)(rect->y); column < ((td_u32)(rect->y) + rect->height); column++) {
        current_mem = mem_start + column * stride;
        for (row = (td_u32)(rect->x); row < ((td_u32)(rect->x) + rect->width); row++) {
            *(td_u32*)(current_mem + row * bpp) = convert_color;
        }
    }
}

static td_void wstp_draw_ui(td_char *mem, td_u32 width, td_u32 height, td_u32 stride, td_u32 bpp)
{
    ext_rect rect;

    rect.x = 0;
    rect.y = 0;
    rect.width  = width / 2; // 2 half
    rect.height = height / 2; // 2 half

    wstp_draw_box(&rect, stride, mem, TEST_DPU_LAYER_BLUE, bpp);
    rect.x = (td_s32)((width > (width / 2 + 1)) ? (width - width / 2 - 1) : 0); // 2 half
    rect.y = 0;
    wstp_draw_box(&rect, stride, mem, TEST_DPU_LAYER_RED, bpp);
    rect.x = 0;
    rect.y =
        (td_s32)((height > (height / 2 + 1)) ? (height - height / 2 - 1) : 0); // 2 half
    wstp_draw_box(&rect, stride, mem, TEST_DPU_LAYER_GREEN, bpp);
    rect.x = (td_s32)((width > (width / 2 + 1)) ? (width - width / 2 - 1) : 0); // 2 half
    rect.y =
        (td_s32)((height > (height / 2 + 1)) ? (height - height / 2 - 1) : 0); // 2 half
    wstp_draw_box(&rect, stride, mem, TEST_DPU_LAYER_YELLOW, bpp);
}

static td_u32 wstp_calc_stride(ext_gfx_compress_mode compress_mode, ext_gfx_fmt color_fmt, td_u32 width)
{
    td_u32 bpp = (color_fmt == EXT_GFX_FMT_ARGB8888) ? 32 : 24; // 32bits ARGB8888 24bit default RGB888

    td_u32 stride;
    if (compress_mode == EXT_GFX_COMPRESS_HFBC_2X) {
        stride = uapi_byte_align(width, 4) / 4 * uapi_byte_align(4 * 4 * bpp / 8 / 2, 8); // 4 8 2 align
    } else {
        stride = uapi_byte_align(width, 16) * bpp / 8; // 16 8 align
    }
    return stride;
}

static td_u32 wstp_calc_size(ext_gfx_compress_mode compress_mode, td_u32 stride, td_u32 height)
{
    td_u32 size;
    if (compress_mode != EXT_GFX_COMPRESS_NONE) {
        /* 4:align value */
        size = stride * uapi_byte_align(height, 4) / 4;
    } else {
        size = stride * height;
    }

    return size;
}

static td_s32 wstp_get_lcd_info(wstp_lcd_info *lcd_info)
{
    bus_panel_info_t panel_timing = { 0 };

    lcd_bus_api *bus_api = lcd_get_bus_api();
    if ((bus_api == TD_NULL) || (bus_api->bus_get_panel_timing == TD_NULL)) {
        printf("bus_api/bus_get_panel_timing is NULL!");
        return EXT_FAILURE;
    }

    td_s32 ret = bus_api->bus_get_panel_timing(TD_NULL, TD_FALSE, &panel_timing);
    if (ret != EXT_SUCCESS) {
        printf("uapi_panel_get_timing failed, ret = %#x!", ret);
        return ret;
    }

    lcd_info->width = panel_timing.width;
    lcd_info->height = panel_timing.height;

    return EXT_SUCCESS;
}

static td_void wstp_vau_fmt_trans(td_u32 src_addr, td_u32 dst_addr, wstp_lcd_info *lcd_info)
{
    td_handle handle;
    ext_vau_surface src_surface = {0};
    ext_vau_surface dst_surface = {0};
    ext_vau_surface_list surface_list = {0};

    src_surface.width          = lcd_info->width;
    src_surface.height         = lcd_info->height;
    src_surface.phy_addr       = src_addr;
    src_surface.vir_addr       = (td_uchar *)(uintptr_t)src_surface.phy_addr;
    src_surface.color_fmt      = EXT_GFX_FMT_ARGB8888;
    src_surface.compress_mode  = EXT_GFX_COMPRESS_NONE;
    src_surface.argb_order     = EXT_GFX_FMT_ORDER_ARGB;
    src_surface.stride         = wstp_calc_stride(src_surface.compress_mode, src_surface.color_fmt, src_surface.width);
    src_surface.rect.x         = 0;
    src_surface.rect.y         = 0;
    src_surface.rect.width     = src_surface.width;
    src_surface.rect.height    = src_surface.height;
    src_surface.out_rect       = src_surface.rect;

    dst_surface.width          = lcd_info->width;
    dst_surface.height         = lcd_info->height;
    dst_surface.phy_addr       = dst_addr;
    dst_surface.vir_addr       = (td_uchar *)(uintptr_t)dst_surface.phy_addr;
    dst_surface.color_fmt      = EXT_GFX_FMT_RGB888;
    dst_surface.compress_mode  = TEST_VAU_FB_CMP_MODE;
    dst_surface.argb_order     = EXT_GFX_FMT_ORDER_ARGB;
    dst_surface.rect.x         = 0;
    dst_surface.rect.y         = 0;
    dst_surface.rect.width     = dst_surface.width;
    dst_surface.rect.height    = dst_surface.height;
    dst_surface.stride         = wstp_calc_stride(dst_surface.compress_mode, dst_surface.color_fmt, dst_surface.width);

    surface_list.src_surface_cnt = 1;
    surface_list.src_surface = &src_surface;
    surface_list.dst_surface = &dst_surface;
    surface_list.opt = NULL;

    handle = uapi_vau_create();
    uapi_vau_render(handle, &surface_list);
    uapi_vau_submit(handle, TD_TRUE, 1000); /* 1000 ms timeout */
    uapi_vau_destroy(handle);

    return;
}

static td_s32 wstp_gpu_smoke_display(int argc, const char *argv[])
{
    uapi_unused(argc);
    uapi_unused(argv);
    td_handle dpu_handle = 0;
    ext_dpu_surface surface = { 0 };
    ext_dpu_layer_info layer_info = { 0 };

    const td_u32 src_addr = TEST_GPU_PSRAM_ADDR;
    td_u32 dst_addr = TEST_GPU_SRAM_ADDR;

    if (argc >= 1) {
        dst_addr = strtoul(argv[0], NULL, 16); // 16 hex
    }

    wstp_lcd_info lcd_info = {466, 466};
    if (wstp_get_lcd_info(&lcd_info) != EXT_SUCCESS) {
        printf("get lcd info fail, use default rect.\n");
    }

    /* 1.init layer info & create layer */
    layer_info.width           = lcd_info.width;
    layer_info.height          = lcd_info.height;
    layer_info.layer_id        = EXT_DPU_LAYER_ID1;
    layer_info.layer_fmt       = EXT_GFX_FMT_ARGB8888;
    layer_info.compress_mode   = EXT_GFX_COMPRESS_NONE;

    uapi_dpu_create_layer(&layer_info, &dpu_handle);

    /* 2.init surface */
    surface.width         = layer_info.width;
    surface.height        = layer_info.height;
    surface.phy_addr      = src_addr;
    surface.vir_addr      = (td_uchar *)(uintptr_t)surface.phy_addr;
    surface.update_rect   = TD_NULL;
    surface.color_fmt     = layer_info.layer_fmt;
    surface.compress_mode = EXT_GFX_COMPRESS_NONE;
    surface.stride        = wstp_calc_stride(surface.compress_mode, surface.color_fmt, surface.width);
    surface.size          = wstp_calc_size(surface.compress_mode, surface.stride, surface.height);

    /* 3.draw picture */
    wstp_draw_ui((td_char *)surface.vir_addr, surface.width, surface.height, surface.stride, 0x4);

    /* 4.vau process argb888 --> rgb888 */
    wstp_vau_fmt_trans(src_addr, dst_addr, &lcd_info);
    surface.color_fmt    = EXT_GFX_FMT_RGB888;
    surface.compress_mode = TEST_VAU_FB_CMP_MODE;
    surface.stride       = wstp_calc_stride(surface.compress_mode, surface.color_fmt, surface.width);
    surface.size         = wstp_calc_size(surface.compress_mode, surface.stride, surface.height);
    surface.phy_addr     = dst_addr;
    surface.vir_addr     = (td_uchar *)(uintptr_t)surface.phy_addr;

    /* 5.dpu display */
    uapi_dpu_set_layer_info(dpu_handle, &surface);
    uapi_dpu_refresh_layers();

    return EXT_SUCCESS;
}

static td_s32 wstp_lcd_fill(int argc, const char *argv[])
{
    if (argc < 1) {
        printf("example: AT+GPU=lcd_fill,[color]\n");
    }

    td_u32 color_input = strtoul(argv[0], NULL, 16); // 16: hex input
    printf("color = %x \r\n", color_input);

    td_handle vau_handle;
    ext_vau_surface src_surface = {0};

    wstp_lcd_info lcd_info = {466, 466};
    if (wstp_get_lcd_info(&lcd_info) != EXT_SUCCESS) {
        printf("get lcd info fail, use default rect.\n");
    }

    src_surface.width            = lcd_info.width;
    src_surface.height           = lcd_info.height;
    src_surface.phy_addr         = TEST_GPU_PSRAM_ADDR;
    src_surface.vir_addr         = (td_uchar *)(uintptr_t)src_surface.phy_addr;
    src_surface.color_fmt        = EXT_GFX_FMT_ARGB8888;
    src_surface.compress_mode    = EXT_GFX_COMPRESS_NONE;
    src_surface.argb_order       = EXT_GFX_FMT_ORDER_ARGB;
    src_surface.rect.width       = src_surface.width;
    src_surface.rect.height      = src_surface.height;
    src_surface.stride           = wstp_calc_stride(src_surface.compress_mode, src_surface.color_fmt, src_surface.width);
    src_surface.is_color_surface = TD_TRUE;
    src_surface.color            = color_input;

    vau_handle = uapi_vau_create();
    uapi_vau_fill(vau_handle, &src_surface);
    uapi_vau_submit(vau_handle, TD_FALSE, 1000); /* 1000 ms timeout */

    td_handle dpu_handle;
    ext_dpu_layer_info layer_info = {0};
    ext_dpu_surface dpu_surface = {0};

    layer_info.width          = src_surface.width;
    layer_info.height         = src_surface.height;
    layer_info.layer_id       = EXT_DPU_LAYER_ID1;
    layer_info.layer_fmt      = src_surface.color_fmt;
    layer_info.compress_mode  = src_surface.compress_mode;
    layer_info.buf_mode       = EXT_DPU_BUFFER_MODE_NORMAL;

    dpu_surface.width         = src_surface.width;
    dpu_surface.height        = src_surface.height;
    dpu_surface.stride        = src_surface.stride;
    dpu_surface.phy_addr      = src_surface.phy_addr;
    dpu_surface.vir_addr      = src_surface.vir_addr;
    dpu_surface.update_rect   = TD_NULL;
    dpu_surface.color_fmt     = src_surface.color_fmt;
    dpu_surface.compress_mode = src_surface.compress_mode;

    uapi_dpu_create_layer(&layer_info, &dpu_handle);
    uapi_dpu_set_layer_vblank(dpu_handle, TD_TRUE);
    uapi_dpu_set_layer_info(dpu_handle, &dpu_surface);
    uapi_dpu_refresh_layers();

    return EXT_SUCCESS;
}

static td_s32 wstp_set_lcd_light(int argc, const char *argv[])
{
    if (argc < 1) {
        printf("example: AT+GPU=lcd_light,[light]\n");
    }
    uint32_t light_input = strtoul(argv[0], NULL, 10); /* 10: dec */
    ext_errno ret = uapi_lcd_set_brightness(light_input);
    if (ret == EXT_ERR_SUCCESS) {
        printf("Set lcd brightness level %u succ!!\n", light_input);
    } else {
        printf("Failed to set lcd brightness!!\n");
    }
    return ERRCODE_SUCC;
}

static td_s32 wstp_gpu_mipi_cbar(int argc, const char *argv[])
{
    if (argc < 1) {
        printf("example: AT+GPU=mipi_cbar,[cbar_en][dir]\n");
        return EXT_FAILURE;
    }

    td_bool cbar_en = (td_bool)strtoul(argv[0], TD_NULL, 10); /* 10: dec */
    td_bool dir = 0;
    if (argc >= 2) { // 2 para num
        dir = (td_bool)strtoul(argv[1], TD_NULL, 10); /* 10: dec, dir: 0 水平 1 垂直 */
    }

    if (cbar_en) {
        uapi_lcd_force_exit_ulps();
        uapi_tcxo_delay_ms(2); // delay 2ms
        uapi_lcd_force_exit_ulps();
        uapi_tcxo_delay_ms(2); // delay 2ms
        mipi_tx_set_colorbar(dir, 0);
    } else {
        mipi_tx_unset_colorbar();
    }

    return EXT_SUCCESS;
}

static td_s32 wstp_gpu_dpu_qspi_cbar(int argc, const char *argv[])
{
    if (argc < 1) {
        printf("example: AT+GPU=dpu_qspi_cbar,[cbar_en][dir]\n");
        return EXT_FAILURE;
    }

    td_u32 cbar_en = (td_u32)strtoul(argv[0], TD_NULL, 10); /* 10: dec */
    td_u32 dir = 0;
    if (argc >= 2) { // 2 para num
        dir = (td_u32)strtoul(argv[1], TD_NULL, 10); /* 10: dec, dir: 0 水平 1 垂直 */
    }

    uapi_reg_write(DPU_REG_BASE_ADDR + 0x12600, 0x91000145); // wbc dhd ctrl
    uapi_reg_write(DPU_REG_BASE_ADDR + 0x12638, (cbar_en & 0x1) << 1); // bit1 colorbar_en
    uapi_reg_write(DPU_REG_BASE_ADDR + 0x1263C, 0x301E6 | (dir & 0x1)); // bit0 dir

    uapi_dpu_refresh_layers();

    return EXT_SUCCESS;
}

static td_s32 wstp_gpu_dpu_cbar(int argc, const char *argv[])
{
    if (argc < 1) {
        return EXT_FAILURE;
    }

    uapi_lcd_force_exit_ulps();

    td_bool cbar_en = (td_bool)strtoul(argv[0], TD_NULL, 10); /* 10: index */
    td_bool dir = 0;
    if (argc >= 2) { // 2 para num
        dir = (td_bool)strtoul(argv[1], TD_NULL, 10); /* 10: index, dir: 0 水平 1 垂直 */
    }
    td_u32 value = 0x80000011;
    if (cbar_en == 1) {
        value = ((dir == 0) ? 0xc0000011 : 0xc0040011);
    };

    uapi_reg_write(DPU_REG_BASE_ADDR + 0xd000, value);
    uapi_reg_write(DPU_REG_BASE_ADDR + 0xd07c, 0x1);

    return EXT_SUCCESS;
}

static td_s32 wstp_gpu_dpu_mute(int argc, const char *argv[])
{
    uapi_unused(argc);

    td_u32 mute_en = (td_bool)strtoul(argv[0], TD_NULL, 10); /* 10: index */
    td_u32 color = 0x3FF; // R 29:20 G 19:10 B 9:0

    if (argc >= 2) { // 2 cnt
        color = strtoul(argv[1], NULL, 16); // 16: hex input
    }

    // FDR0 MUTE
    uapi_reg_write(DPU_REG_BASE_ADDR + 0x10e00, 0x1000 | ((mute_en & 0x1) << 10)); // mute使能 bit 10
    uapi_reg_write(DPU_REG_BASE_ADDR + 0x10e10, 0xff);   // mute alpha
    uapi_reg_write(DPU_REG_BASE_ADDR + 0x10e14, color);  // mute rgb
    uapi_reg_write(DPU_REG_BASE_ADDR + 0x7004, 0x1);     // G0层regup

    uapi_dpu_refresh_layers();

    return EXT_SUCCESS;
}

#ifdef SUPPORT_GPU_DIR_EXIST
static td_s32 wstp_gpu_dpu_screen_cap(int argc, const char *argv[])
{
    uapi_unused(argc);
    uapi_unused(argv);

    td_u32 layer_id = 0;
    if (argc >= 1) {
        layer_id = (td_u32)strtoul(argv[0], TD_NULL, 10); /* 10: index */
    }

    dpu_dfx_info *dfx_info = dpu_dfx_get_info();
    td_u32 addr = dfx_info->layer_info[layer_id].phy_addr;
    td_u32 stride = dfx_info->layer_info[layer_id].stride;
    td_u32 height = dfx_info->layer_info[layer_id].height;
    drv_gfx_compress_mode compress_mode = dfx_info->layer_info[layer_id].compress_mode;

    printf("FB info: addr[%#x] width[%u] height[%u] stride[%u] color_fmt[%d] compress_mode[%d]\n", addr,
        dfx_info->layer_info[layer_id].width, height, stride, dfx_info->layer_info[layer_id].color_fmt, compress_mode);

    if (compress_mode != DRV_GFX_COMPRESS_NONE) {
        height = uapi_byte_align(height, 4) / 4; // 4 align
    }

    int fd = open("/user/dpu_screen_cap.bin", O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) {
        printf("open file failure!\n");
        return EXT_FAILURE;
    }

    write(fd, (td_void *)(uintptr_t)addr, stride * height);
    close(fd);

    printf("save /user/dpu_screen_cap.bin stride[%u] height[%u] success!\n", stride, height);

    return EXT_SUCCESS;
}
#endif

static td_s32 wstp_mipi_read(int argc, const char *argv[])
{
    td_u8 reg_addr;
    td_u8 read_cnt;
    td_u8 work_mode;
    td_u32 ret;
    unsigned char rd_data[96] = {0}; /* 96 is max array count */
    get_cmd_info_t get_info = { 0 };

    if (argc != 3) { // 3 is the minimum number of parameters.
        printf("Invalid Parameters num");
        return EXT_FAILURE;
    }
    work_mode = (td_u8)strtol(argv[0], NULL, 0);
    reg_addr = (td_u8)strtoul(argv[1], NULL, 16); /*    1 is parameter number 16 is the base format */
    read_cnt = (td_u8)strtol(argv[2], NULL, 0); // 2 is parameter number
    if (read_cnt > 96) { /* 96 is max array count */
        printf("Invalid read_cnt: %u\n", read_cnt);
        return EXT_FAILURE;
    }
    cmd_info_t set_info = {0};
    set_info.work_mode= work_mode;
    set_info.data_type = 0x37;
    set_info.cmd_size = read_cnt;
    ret = uapi_mipi_tx_set_cmd(&set_info);
    if (ret != EXT_ERR_SUCCESS) {
        printf("set read len fail, ret = 0x%x \r\n", ret);
    }
    get_info.work_mode = work_mode;
    get_info.data_type = 0x14;
    get_info.data_param = reg_addr;
    get_info.get_data_size = read_cnt;
    get_info.get_data = rd_data;
    ret = uapi_mipi_tx_get_cmd(&get_info);
    if (ret != EXT_ERR_SUCCESS) {
        printf("test mipi read fail, ret = 0x%x \r\n", ret);
        return ret;
    }

    printf("reg(0x%x):\n", reg_addr);
    for (int i = 0; i < read_cnt; i++) {
        printf("0x%x", get_info.get_data[i]);
        if (i == read_cnt - 1) {
            printf("\n");
        } else {
            printf(" ");
        }
    }

    return EXT_SUCCESS;
}

static td_void mipi_write_short(int work_mode, td_u8 reg_addr, td_u8 packet_len, td_u8 cfg_value)
{
    td_u32 ret;
    cmd_info_t set_info = { 0 };

    set_info.work_mode = work_mode;
    set_info.data_type = packet_len == 0 ? 0x05 : 0x15;
    set_info.cmd_size = wstp_dsi_short_packet(cfg_value, reg_addr);

    ret = uapi_mipi_tx_set_cmd(&set_info);
    if (ret != EXT_ERR_SUCCESS) {
        printf("test mipi write short packet fail, ret = 0x%x \r\n", ret);
        return;
    }
    printf("write reg(0x%x): %#x\n", reg_addr, cfg_value);
}

static td_s32 wstp_mipi_write(int argc, const char *argv[])
{
    td_u8 reg_addr, packet_len, work_mode, cfg_value = 0;
    td_u8 wr_data[160] = {0};  // 160 is Random length exceeding the maximum length
    cmd_info_t long_packet = {0};

    if (argc < 3) { // 3 is minnum parameter number
        printf("Invalid Parameters num");
        return EXT_FAILURE;
    }
    work_mode = (td_u8)strtol(argv[0], NULL, 0);
    reg_addr = (td_u8)strtoul(argv[1], NULL, 16); /* 1 is parameter number 16 is base format */
    packet_len = (td_u8)strtoul(argv[2], NULL, 0); // 2 parameter number
    if (packet_len <= 1) { // 1 is the maximum number of parameters for short writing.
        if (packet_len == 1) { // 1 is parameter number
            cfg_value = (td_u8)strtoul(argv[3], NULL, 16); /* 3 is pra num, 16 is the base format */
        }
        mipi_write_short(work_mode, reg_addr, packet_len, cfg_value);
        return EXT_SUCCESS;
    } else if (packet_len > 160) { // 160 is wr_data max len
        printf("packet len out of range. max len is 160\n");
        return EXT_FAILURE;
    }
    td_s32 param_len = packet_len;
    if (argc < (int)(packet_len + 3)) { // 3 is default para num
        param_len = argc - 3; // 3 is default para num
        printf("para num is less than packet_num, automatic zero-filling\n");
    }
    wr_data[0] = reg_addr;
    for (int i = 0; i < param_len; i++) {
        wr_data[i + 1] = (td_u8)strtoul(argv[i + 3], NULL, 16); // 1 3 16 is the base format
    }
    long_packet.work_mode = work_mode;
    long_packet.data_type = 0x29;
    long_packet.cmd_size = packet_len + 1; // 1 is offset
    long_packet.cmd = wr_data;
    td_u32 ret = uapi_mipi_tx_set_cmd(&long_packet);
    if (ret != EXT_ERR_SUCCESS) {
        printf("test mipi write long packet fail, ret = 0x%x \r\n", ret);
        return EXT_FAILURE;
    }

    printf("write reg(0x%x), len: %u\n", reg_addr, packet_len);
    for (int i = 0; i < packet_len; i++) {
        printf("0x%x", long_packet.cmd[i + 1]);
        if (i == packet_len - 1) {
            printf("\n");
        } else {
            printf(" ");
        }
    }

    return EXT_SUCCESS;
}

static td_s32 wstp_lcd_lp_ctrl(int argc, const char *argv[])
{
    if (argc < 1) { // 1 is minnum parameter number
        printf("example: AT+GPU=lcd_lp,[mode]  ulps - 2 exit 3 clear_exit 4 force_exit 5 suspend 6 not_suspend\n");
        return EXT_FAILURE;
    }
    lcd_bus_lp_ctrl_t mode = (td_u8)strtol(argv[0], NULL, 10); // 10 dec
    lcd_bus_api *bus_api = lcd_get_bus_api();
    if (bus_api == TD_NULL || bus_api->bus_lp_ctrl == TD_NULL) {
        printf("bus api is null\n");
        return EXT_FAILURE;
    }
    bus_api->bus_lp_ctrl(mode, TD_NULL);

    return EXT_SUCCESS;
}

static td_s32 wstp_lcd_write(int argc, const char *argv[])
{
    if (argc < 2) { // 2 para num
        printf("Invalid Parameters num");
        return EXT_FAILURE;
    }

    lcd_cmd_sequ wr_data = {0};
    wr_data.dcs_flag = 1;
    wr_data.data[0] = (uint8_t)strtoul(argv[0], NULL, 16); /* 16 */
    wr_data.data_len = (uint8_t)strtol(argv[1], NULL, 0) + 1;

    if (argc < wr_data.data_len + 1) {
        printf("write data(%d) less than write len(%d)\n", argc - 2, wr_data.data_len - 1); /* 2 */
        return EXT_FAILURE;
    }

    uint32_t i;
    for (i = 0; i < wr_data.data_len - 1; i++) {
        wr_data.data[i + 1] = (uint8_t)strtoul(argv[i + 2], NULL, 16); /* 2, 16 */
    }

    ext_errno ret = uapi_lcd_write_reg(&wr_data, 1);
    if (ret != EXT_ERR_SUCCESS) {
        printf("Failed to write lcd reg!! ret = 0x%x \r\n", ret);
        return ret;
    }

    printf("packet reg(0x%x), len: %d\n", wr_data.data[0], wr_data.data_len - 1);
    for (i = 0; i < wr_data.data_len - 1; i++) {
        printf("0x%x", wr_data.data[i + 1]);
        if (i == wr_data.data_len - 2) { /* 2 */
            printf("\n");
        } else {
            printf(" ");
        }
    }

    return EXT_SUCCESS;
}

static td_s32 wstp_lcd_read(int argc, const char *argv[])
{
    uint8_t buff[100] = {0}; /* 100 */
    uint8_t *pbuff = buff;
    if (argc != 2) { /* 2 means para num */
        printf("Invalid Parameters num");
        return EXT_FAILURE;
    }

    uint8_t reg_addr = (uint8_t)strtoul(argv[0], NULL, 16); /* 16 */
    uint8_t read_len = (uint8_t)strtol(argv[1], NULL, 0);
    ext_errno ret = uapi_lcd_read_reg(reg_addr, pbuff, read_len);
    if (ret != EXT_ERR_SUCCESS) {
        printf("Failed to read lcd reg!! ret = 0x%x \r\n", ret);
        return EXT_FAILURE;
    }

    printf("reg(0x%x):\n", reg_addr);
    for (int i = 0; i < read_len; i++) {
        printf("0x%x", buff[i]);
        if (i == read_len - 1) {
            printf("\n");
        } else {
            printf(" ");
        }
    }

    return EXT_SUCCESS;
}

static td_s32 wstp_lcd_dfx_proc_read(int argc, const char *argv[])
{
    uapi_unused(argc);
    uapi_unused(argv);
    uapi_lcd_dfx_proc_read();

    return EXT_SUCCESS;
}

static td_s32 wstp_gpu_mem_proc(int argc, const char *argv[])
{
    uapi_unused(argc);
    uapi_unused(argv);
    uapi_vau_proc_mem();
    return EXT_SUCCESS;
}

#define AT_GPU_FUNC_NUM (sizeof(g_at_gpu_cmd_parse_table) / sizeof(g_at_gpu_cmd_parse_table[0]))

void los_at_gpu_cmd_register(void)
{
    print_str("los_at_gpu_cmd_register EXCUTE\r\n");
    uapi_at_gpu_register_cmd(g_at_gpu_cmd_parse_table, AT_GPU_FUNC_NUM);
}

typedef int (*at_gpu_func)(int argc, const char *argv[]);
typedef struct {
    const char *cmd_str;
    at_gpu_func func;
} at_gpu_func_map;

const at_gpu_func_map g_at_gpu_func_map[] = {
    {"smoke",               wstp_gpu_smoke_display},
    {"lcd_fill",            wstp_lcd_fill},
    {"lcd_light",           wstp_set_lcd_light},
    {"mipi_cbar",           wstp_gpu_mipi_cbar},
    {"dpu_qspi_cbar",       wstp_gpu_dpu_qspi_cbar},
    {"dpu_cbar",            wstp_gpu_dpu_cbar},
    {"dpu_mute",            wstp_gpu_dpu_mute},
    {"gpu_mem",             wstp_gpu_mem_proc},
#ifdef SUPPORT_GPU_DIR_EXIST
    {"screen_cap",          wstp_gpu_dpu_screen_cap},
#endif
    {"dpu_proc",            uapi_dpu_dfx_proc_read},
    {"dpu_print",           uapi_dpu_dfx_print_enable},
    {"dpu_save_fb",         uapi_dpu_dfx_save_fb_enable},
    {"dpu_refresh_skip",    uapi_dpu_dfx_enable_dpu_refresh_skip},
    {"vau_proc",            uapi_vau_dfx_proc_read},
    {"mipi_read",           wstp_mipi_read},
    {"mipi_write",          wstp_mipi_write},
#ifdef SUPPORT_GPU_JPEG
    {"jpeg_proc",           uapi_jpeg_dfx_proc_read},
#endif
#ifdef SUPPORT_GPU_GMMU
    {"gmmu_proc",           uapi_gmmu_dfx_proc_read},
#endif
    {"lcd_proc",            wstp_lcd_dfx_proc_read},
    {"lcd_write",           wstp_lcd_write},
    {"lcd_read",            wstp_lcd_read},
    {"lcd_lp",              wstp_lcd_lp_ctrl},
};

td_void at_gpu_process(int argc, char *argv[])
{
    if (argc < 1) {
        return;
    }

    const char **para = (const char **)(argv + 1);
    td_u32 size = uapi_array_size(g_at_gpu_func_map);
    for (td_u32 i = 0; i < size; i++) {
        if (strcmp((char *)argv[0], g_at_gpu_func_map[i].cmd_str) == 0) {
            g_at_gpu_func_map[i].func(argc - 1, para);
            return;
        }
    }

#ifdef SUPPORT_GPU_TEST
    test_gpu_testcase_process(argc, argv);
#endif

    return;
}

static td_s32 at_gpu_convert_bin_to_dec(td_s32 pbin)
{
    td_s32 result = 0;
    td_s32 temp = pbin;

    while (temp != 0) {
        result += temp % 2; // 2 div
        temp /= 2; // 2 div
    }

    return result;
}

at_ret_t at_gpu_sample_process(const gpu_args_t *arg)
{
    PRINT("gpu_at enter \r\n");
    td_s32 argc = at_gpu_convert_bin_to_dec(arg->para_map);
    char *argv[] = {arg->para1, arg->para2, arg->para3, arg->para4, arg->para5, arg->para6, arg->para7, arg->para8,
        arg->para9, arg->para10, arg->para11, arg->para12, arg->para13, arg->para14, arg->para15, arg->para16,
        arg->para17, arg->para18, arg->para19, arg->para20};
    PRINT("AT+GPU=");
    for (td_s32 i = 0; i < argc; i++) {
        printf("%s,", argv[i]);
    }
    printf("\n");
    at_gpu_process(argc, argv);
    return AT_RET_OK;
}
