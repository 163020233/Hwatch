/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: dpu intf file
 */

#include "dpu_dfx.h"
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "platform_core.h"
#include "drv_gfx_port.h"
#include "dpu_isr.h"
#include "dpu_disp.h"
#include "dpu_layer.h"
#include "dpu_hal.h"
#include "gpu_crg_intf.h"

#define SAVE_FILE_DIR "/user"

STATIC dpu_dfx_info g_dpu_dfx_info = {0};
STATIC  td_u32 g_save_cnt = 0;

PLT_SEC_ITCM_T dpu_dfx_info *dpu_dfx_get_info(td_void)
{
    return &g_dpu_dfx_info;
}

td_void dpu_dfx_print_info(void)
{
    if (dpu_dfx_check_mask(DPU_DFX_MASK_PRINT_FPS) != TD_TRUE) {
        return;
    }
    gpu_print("-------------------------------------\n");
    gpu_print("wait_frm_done(us)         :%u\n", (td_u32)(g_dpu_dfx_info.wait_frm_done));
    gpu_print("fresh_intf_cost(us)       :%u\n", (td_u32)(g_dpu_dfx_info.refresh_intf_cost));
    gpu_print("te_interrupt(us)          :%u\n", (td_u32)(g_dpu_dfx_info.te_cost_us));
    gpu_print("draw/flip/te(fps)         :%-6u/%-6u/%-6u\n", g_dpu_dfx_info.draw_fps,
              g_dpu_dfx_info.flip_fps, g_dpu_dfx_info.te_fps);

    return;
}

STATIC td_void dpu_get_frame_disp_cost(const dpu_disp_info *disp_dev, td_float *frame_disp_cost)
{
    td_u32 clk_cnt = dpu_hal_disp_get_dhd0_intf_pcnt_cnt();
    /* pixel_clk Unit: kHz, 1000000.0:s to ns */
    td_float time_per_pixel_clk = 1000000.0f / disp_dev->timing.pixel_clk;

    *frame_disp_cost = clk_cnt * time_per_pixel_clk / 1000000; /* 1000000:ns to ms */

    return;
}

STATIC td_void dfx_disp_proc_read(void)
{
    td_u32 chksum_y = 0;
    td_u32 chksum_u = 0;
    td_u32 chksum_v = 0;
    td_float frame_disp_cost = 0;
    dpu_disp_info *disp_dev  = dpu_disp_get_info();
    dpu_dfx_info  *dfx_info  = dpu_dfx_get_info();

    dpu_hal_disp_get_chksum(&chksum_y, &chksum_u, &chksum_v);
    dpu_get_frame_disp_cost(disp_dev, &frame_disp_cost);

    gpu_print("\n-----------DISP proc-----------\n");
    gpu_print("work_mode                :%-6d\n", disp_dev->timing.is_cmd_mode ? 1 : 0);
    gpu_print("disp_open                :%-6d\n", disp_dev->is_open ? 1 : 0);
    gpu_print("open_cnt                 :%-6u\n", disp_dev->open_cnt);

    gpu_print("work_scene/is_cmp/is_rot :%-6u/%-6u/%-6u\n", disp_dev->scene_info.work_scene,
              disp_dev->scene_info.is_cmp_out, disp_dev->scene_info.is_rotate);
    gpu_print("pixel_clk(k)             :%-6u\n", disp_dev->timing.pixel_clk);
    gpu_print("work_clk(k)              :%-6u\n", disp_dev->work_clk);
    gpu_print("clk_gpu(M)               :%-6u\n", disp_dev->clk_gpu);
    gpu_print("refresh_rate             :%-6u\n", disp_dev->timing.refresh_rate);

    gpu_print("hact/vact                :%-6u/%-6u\n", dfx_info->hact, dfx_info->vact);
    gpu_print("hfb/vfb                  :%-6u/%-6u\n", dfx_info->hfb, dfx_info->vfb);
    gpu_print("hbb/vbb                  :%-6u/%-6u\n", disp_dev->timing.hbb, disp_dev->timing.vbb);
    gpu_print("hpw/vpw                  :%-6u/%-6u\n", disp_dev->timing.hpw, disp_dev->timing.vpw);

    gpu_print("dhd0_region              :%-6d/%-6d/%-6u/%-6u\n",
              disp_dev->disp_region.x, disp_dev->disp_region.y,
              disp_dev->disp_region.width, disp_dev->disp_region.height);

    gpu_print("lowband_cnt              :%-6u\n", g_dpu_dfx_info.lowband_cnt);
    gpu_print("bus_err_cnt              :%-6u\n", g_dpu_dfx_info.bus_err_cnt);
    gpu_print("dcmp_cfg_err_cnt         :%-6u\n", g_dpu_dfx_info.dcmp_cfg_err_cnt);
    gpu_print("dcmp_bs_err_cnt          :%-6u\n", g_dpu_dfx_info.dcmp_bs_err_cnt);
    gpu_print("te_signal                :%-6u\n", g_dpu_dfx_info.te_signal_total);
    gpu_print("fresh/frm_done/to/wbc/end:%-6u/%-6u/%-6u/%-6u/%-6u\n", g_dpu_dfx_info.refresh_total,
              g_dpu_dfx_info.frm_done_total, g_dpu_dfx_info.timeout_total, g_dpu_dfx_info.wbc_done_total,
              g_dpu_dfx_info.frm_end_total);
    gpu_print("draw/flip/te(t)          :%-6u/%-6u/%-6u\n", g_dpu_dfx_info.refresh_cnt,
              g_dpu_dfx_info.frm_done_cnt, g_dpu_dfx_info.te_signal_cnt);

    gpu_print("suspend/resume           :%-6u/%-6u\n", g_dpu_dfx_info.suspend_cnt, g_dpu_dfx_info.resume_cnt);
    gpu_print("chksum[RGB]              :%#x/%#x/%#x\n", chksum_y, chksum_u, chksum_v);
    gpu_print("frame_disp_cost(ms)      :%.2f\n", frame_disp_cost);
    gpu_print("color_type/sat/csc_en    :%-6u/%-6.2f/%-6u\n", disp_dev->output_info.color_type,
              disp_dev->output_info.color_sat, disp_dev->set_csc_en);

    return;
}

STATIC td_void dfx_layer_proc_read(td_u32 layer_id)
{
    td_u32 chksum_ar = 0;
    td_u32 chksum_gb = 0;
    dpu_layer_dev *layer_dev = TD_NULL;
    dpu_dfx_info  *dfx_info  = dpu_dfx_get_info();

    if (layer_id < DRV_DPU_LAYER_ID1 || layer_id > DRV_DPU_MAX_LAYER_ID) {
        gpu_err("invalid layer id = %u, max is 1!", layer_id);
        return;
    }

    dpu_hal_layer_get_fdr_chksum(layer_id, &chksum_ar, &chksum_gb);

    layer_dev = dpu_layer_get_dev(layer_id);
    dpu_dfx_layer_info *layer_info = &dfx_info->layer_info[layer_id];

    if (layer_dev != TD_NULL) {
        gpu_print("-----------LAYER%u proc---------\n", layer_id);
        gpu_print("create                   :%-6d\n", layer_dev->is_create ? 1 : 0);
        gpu_print("create_cnt               :%-6u\n", layer_dev->create_cnt);
        gpu_print("width                    :%-6u\n", layer_dev->run_info.width);
        gpu_print("height                   :%-6u\n", layer_dev->run_info.height);
        gpu_print("stride                   :%-6u\n", layer_dev->run_info.stride);
        gpu_print("uv_stride                :%-6u\n", layer_dev->canvas_surface.uv_stride);
        gpu_print("uv_offset                :%-6u\n", layer_dev->canvas_surface.uv_offset);
        gpu_print("format                   :%-6u\n", layer_info->color_fmt);
        gpu_print("cmp_mode                 :%-6u\n", layer_info->compress_mode);
        gpu_print("vblank                   :%-6d\n", layer_dev->vblank ? 1 : 0);
        gpu_print("canvas_addr              :%#x\n",  layer_dev->run_info.current_addr);
        gpu_print("chksum[ar/gb]            :%#x/%#x\n", chksum_ar, chksum_gb);

        gpu_print("disp_rect                :%-6d/%-6d/%-6u/%-6u\n",
            layer_info->disp_rect.x, layer_info->disp_rect.y,
            layer_info->disp_rect.width, layer_info->disp_rect.height);
        gpu_print("crop_rect                :%-6d/%-6d/%-6u/%-6u\n",
            layer_info->crop_rect.x, layer_info->crop_rect.y,
            layer_info->crop_rect.width, layer_info->crop_rect.height);
    }

    return;
}

td_s32 uapi_dpu_dfx_proc_read(int argc, const char *argv[])
{
    td_u32 layer_id;

    if (argc < 1) {
        gpu_print("usage: dpu_proc layer_id. range:0~2");
        return EXT_FAILURE;
    }
    if (argv == TD_NULL || argv[0] == TD_NULL) {
        gpu_print("dfx null ptr\n");
        return EXT_FAILURE;
    }
    layer_id = (td_u32)strtoul(argv[0], TD_NULL, 10); /* 10: index */
    dfx_layer_proc_read(layer_id);
    dfx_disp_proc_read();

    return EXT_SUCCESS;
}

td_bool dpu_dfx_check_mask(td_u32 mask)
{
    dpu_dfx_info *dfx_info = dpu_dfx_get_info();
    return ((dfx_info->print_mask & mask) == 0) ? TD_FALSE : TD_TRUE;
}

td_s32 dpu_dfx_lowband_print(void)
{
    dpu_dfx_info  *dfx_info  = dpu_dfx_get_info();
    dpu_disp_info *disp_dev = dpu_disp_get_info();
    dpu_layer_dev *layer_dev = TD_NULL;
    dpu_dfx_layer_info *layer_info  = TD_NULL;
    dpu_clk_info clk_info = {};
    gpu_print("---- lowband print ----\n");
    for (int i = 0; i <= DRV_DPU_MAX_LAYER_ID; i++) {
        layer_dev = dpu_layer_get_dev(i);
        layer_info = &dfx_info->layer_info[i];
        if ((layer_dev != TD_NULL) && layer_dev->is_create) {
            gpu_print("layer[%u] info: width:%u height:%u stride:%u fmt:%u cmp_mode:%u vblank:%d addr:%#x\n",
                i, layer_dev->run_info.width, layer_dev->run_info.height, layer_dev->run_info.stride,
                layer_info->color_fmt, layer_info->compress_mode, layer_dev->vblank ? 1 : 0,
                layer_dev->run_info.current_addr);
            gpu_print("disp_rect:%d/%d/%u/%u crop_rect:%d/%d/%u/%u\n",
                layer_info->disp_rect.x, layer_info->disp_rect.y,
                layer_info->disp_rect.width, layer_info->disp_rect.height, layer_info->crop_rect.x,
                layer_info->crop_rect.y, layer_info->crop_rect.width, layer_info->crop_rect.height);
        } else {
            gpu_print("layer[%d] not created\n", i);
        }
    }
    gpu_crg_get_clk(GPU_ID_DPU, &(disp_dev->scene_info.work_scene), &clk_info);
    gpu_print("scene[work_scene/cmp/rotate]:%u/%u/%u pixel_clk:%u work_clk:%u\n",
        disp_dev->scene_info.work_scene, disp_dev->scene_info.is_cmp_out, disp_dev->scene_info.is_rotate,
        clk_info.pixel_clk, clk_info.work_clk);
    gpu_print("hpw:%u hbb:%u hact:%u hfb:%u vpw:%u vbb:%u vact:%u vfb:%u region:%d/%d/%u/%u\n",
        disp_dev->timing.hpw, disp_dev->timing.hbb, dfx_info->hact, dfx_info->hfb,
        disp_dev->timing.vpw, disp_dev->timing.vbb, dfx_info->vact, dfx_info->vfb,
        disp_dev->disp_region.x, disp_dev->disp_region.y, disp_dev->disp_region.width, disp_dev->disp_region.height);
    gpu_print("lowband:%u bus_err:%u dcmp_cfg_err:%u dcmp_bs_err:%u\n", g_dpu_dfx_info.lowband_cnt,
        g_dpu_dfx_info.bus_err_cnt, g_dpu_dfx_info.dcmp_cfg_err_cnt, g_dpu_dfx_info.dcmp_bs_err_cnt);
    return EXT_SUCCESS;
}

td_s32 uapi_dpu_dfx_print_enable(int argc, const char *argv[])
{
    if (argc < 1) {
        gpu_print("usage: gpu_print 0/1/2/3 (0:close all, 1:fps, 2:params, 3:all)\n");
        return EXT_FAILURE;
    }
    if (argv == TD_NULL || argv[0] == TD_NULL) {
        gpu_print("dfx null ptr\n");
        return EXT_FAILURE;
    }
    td_u32 value = (td_u32)strtoul(argv[0], TD_NULL, 10); /* 10: index */
    g_dpu_dfx_info.print_mask = value & DPU_DFX_MASK_PRINT_ALL;
    gpu_print("print_mask set to %u\n", g_dpu_dfx_info.print_mask);

    return EXT_SUCCESS;
}

td_s32 uapi_dpu_dfx_save_fb_enable(int argc, const char *argv[])
{
    if (argc < 0x4) {
        gpu_print("usage: layer_id: 0/1, save_en 0/1, save_all 0/1, save_cnt\n");
        return EXT_FAILURE;
    }

    if (argv == TD_NULL || argv[0x1] == TD_NULL || argv[0x2] == TD_NULL || argv[0x3] == TD_NULL) {
        gpu_print("dfx null ptr\n");
        return EXT_FAILURE;
    }
    g_dpu_dfx_info.save_layer = (td_u32)strtoul(argv[0x0], TD_NULL, 10); /* 10: index */
    g_dpu_dfx_info.save_en    = (td_bool)strtoul(argv[0x1], TD_NULL, 10); /* 10: index */
    g_dpu_dfx_info.save_all   = (td_bool)strtoul(argv[0x2], TD_NULL, 10); /* 10: index */
    g_dpu_dfx_info.save_cnt   = (td_u32)strtoul(argv[0x3], TD_NULL, 10); /* 10: index */

    return EXT_SUCCESS;
}

STATIC td_void dpu_dfx_save_fb_data_all(const drv_dpu_surface *surface)
{
    if (surface == TD_NULL) {
        gpu_print("input surface is null\n");
        return;
    }

    if ((surface->phy_addr == 0) || (surface->stride == 0) || (surface->height == 0)) {
        gpu_print("invalid input[%#X][%u][%u]\n", surface->phy_addr, surface->stride, surface->height);
        return;
    }
    td_u32 height = surface->height;
    if (surface->compress_mode != DRV_GFX_COMPRESS_NONE) {
        height = uapi_byte_align(height, 4) / 4;  // 4 is align
    }
    int fd;
    if (g_save_cnt == 1) {
        fd = open("/user/dpu_save_fb_data.bin", O_RDWR | O_CREAT | O_TRUNC, 0666); /* 0666 rw */
    } else {
        fd = open("/user/dpu_save_fb_data.bin", O_RDWR | O_APPEND, 0666); /* 0666 rw */
    }
    if (fd < 0) {
        gpu_print("open dpu_save_fb_data.bin failure\n");
        return;
    }
    write(fd, (td_void *)(uintptr_t)surface->phy_addr, surface->stride * height);
    close(fd);

    gpu_print("save /user/dpu_save_fb_data.bin add[%#X] fmt[%d] cmp[%d] width[%u] height[%u] stride[%u] success!\n",
        surface->phy_addr, surface->color_fmt, surface->compress_mode, surface->width, height, surface->stride);
}

STATIC td_void dpu_dfx_save_fb_data_part(const drv_dpu_surface *surface)
{
    if (surface == TD_NULL) {
        gpu_print("input surface is null\n");
        return;
    }

    if ((surface->phy_addr == 0) || (surface->stride == 0) || (surface->height == 0)) {
        gpu_print("invalid input[%#X][%u][%u]\n", surface->phy_addr, surface->stride, surface->height);
        return;
    }

    td_u32 pixel_byte;
    if (surface->color_fmt == DRV_GFX_FMT_ARGB8888) {
        pixel_byte = 0x4;
    } else if (surface->color_fmt == DRV_GFX_FMT_RGB888) {
        pixel_byte = 0x3;
    } else if (surface->color_fmt == DRV_GFX_FMT_RGB565) {
        pixel_byte = 0x2;
    } else {
        gpu_print("unsupport fmt: %#X", surface->color_fmt);
        return;
    }

    ext_rect rect = {0, 0, surface->width, surface->height};
    ext_rect *update_rect = (surface->update_rect == TD_NULL ? &rect : surface->update_rect);
    td_char file_name[0xff] = {'\0'};
    td_s32 ret = snprintf_s(file_name, sizeof(file_name), sizeof(file_name), "%s/%u_width_%u_height_%u.bin",
             SAVE_FILE_DIR, g_save_cnt, update_rect->width, update_rect->height);
    if (ret < 0) {
        gpu_print("write file_name err\n");
        return;
    }

    int fd = open(file_name, O_RDWR | O_CREAT, 0666);
    if (fd < 0) {
        gpu_print("open file failure\n");
        return;
    }
    td_uchar *start_addr = (td_uchar *)(uintptr_t)surface->phy_addr +
        (td_u32)update_rect->y * surface->stride + (td_u32)update_rect->x * pixel_byte;
    for (td_u32 i = 0; i < update_rect->height; i++) {
        write(fd, (td_void *)start_addr, update_rect->width * pixel_byte);
        start_addr += surface->stride;
    }
    close(fd);

    gpu_print("save %u_width_%u_height_%u.bin, addr[%#X] success!\n",
              g_save_cnt, surface->width, surface->height, surface->phy_addr);
}

td_void dfx_dpu_save_fb_to_file(drv_dpu_layer_id layer_id, drv_dpu_surface *surface)
{
    if ((g_dpu_dfx_info.save_en == TD_FALSE) || (layer_id != g_dpu_dfx_info.save_layer)) {
        return;
    }
    if (g_save_cnt >= g_dpu_dfx_info.save_cnt) {
        g_save_cnt = 0;
        g_dpu_dfx_info.save_en = TD_FALSE;
        return;
    }
    g_save_cnt++;
    if (g_dpu_dfx_info.save_all) {
        dpu_dfx_save_fb_data_all(surface);
    } else {
        dpu_dfx_save_fb_data_part(surface);
    }
}

td_s32 uapi_dpu_dfx_enable_dpu_refresh_skip(int argc, const char *argv[])
{
    if (argc < 1) {
        gpu_print("should input enable switch 0/1\n");
        return EXT_FAILURE;
    }
    if (argv == TD_NULL || argv[0] == TD_NULL) {
        gpu_print("dfx null ptr\n");
        return EXT_FAILURE;
    }
    g_dpu_dfx_info.is_skip_dpu_refresh = (td_bool)strtoul(argv[0x0], TD_NULL, 10); /* 10: index */
    if (g_dpu_dfx_info.is_skip_dpu_refresh) {
        gpu_print("open dpu_refresh_skip\n");
    } else {
        gpu_print("close dpu_refresh_skip\n");
    }

    return EXT_SUCCESS;
}