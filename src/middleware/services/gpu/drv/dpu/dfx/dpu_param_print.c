/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: dpu parameter print implementation
 */
#include "drv_dpu_struct.h"
#include "drv_dpu_type.h"
#include "drv_dpu_ioctl.h"
#include "gpu_dfx.h"
#include "dpu_param_print.h"

typedef td_s32 (*dpu_para_print_fn)(td_u32 cmd, td_void *arg);

static inline td_bool is_get_cmd(td_u32 cmd)
{
    return (cmd == DRV_DPU_CMD_GETLAYERALPHA || cmd == DRV_DPU_CMD_GETLAYERPOS ||
        cmd == DRV_DPU_CMD_GETLAYERZORDER || cmd == DRV_DPU_CMD_GETLAYERINFO ||
        cmd == DRV_DPU_CMD_GETYLAYERCOLORKYE || cmd == DRV_DPU_CMD_GETLAYERVBLANK);
}

static td_s32 dpu_print_layer_id(td_u32 cmd, td_void *arg)
{
    uapi_unused(cmd);
    drv_dpu_layer_id *layer_id = (drv_dpu_layer_id *)arg;
    gpu_print("layer_id:%d\n", *layer_id);
    return EXT_SUCCESS;
}

static td_s32 dpu_print_layer_alpha(td_u32 cmd, td_void *arg)
{
    dpu_layer_alpha_cmd *print_cmd = (dpu_layer_alpha_cmd *)arg;
    if (is_get_cmd(cmd)) {
        gpu_print("layer_id:%d\n", print_cmd->layer_id);
        return EXT_SUCCESS;
    }
    gpu_print("layer_id:%d alpha:%#X\n", print_cmd->layer_id, print_cmd->alpha);
    return EXT_SUCCESS;
}

static td_s32 dpu_print_layer_pos(td_u32 cmd, td_void *arg)
{
    dpu_layer_pos_cmd *print_cmd = (dpu_layer_pos_cmd *)arg;
    if (is_get_cmd(cmd)) {
        gpu_print("layer_id:%d\n", print_cmd->layer_id);
        return EXT_SUCCESS;
    }
    gpu_print("layer_id:%d pos_x:%d pos_y:%d\n", print_cmd->layer_id, print_cmd->pos_x, print_cmd->pos_y);
    return EXT_SUCCESS;
}

static td_s32 dpu_print_layer_zorder(td_u32 cmd, td_void *arg)
{
    dpu_layer_zorder_cmd *print_cmd = (dpu_layer_zorder_cmd *)arg;
    if (is_get_cmd(cmd)) {
        gpu_print("layer_id:%d\n", print_cmd->layer_id);
        return EXT_SUCCESS;
    }
    gpu_print("layer_id:%d zorder:%u\n", print_cmd->layer_id, print_cmd->zorder);
    return EXT_SUCCESS;
}

static td_s32 dpu_print_layer_info(td_u32 cmd, td_void *arg)
{
    dpu_layer_info_cmd *print_cmd = (dpu_layer_info_cmd *)arg;

    if (is_get_cmd(cmd)) {
        gpu_print("layer_id:%d\n", print_cmd->layer_id);
        return EXT_SUCCESS;
    }
    gpu_print("layer_id:%d w:%u h:%u stride:%u addr:%#x fmt:%u premulti:%d flip:%d cmp:%u rotate:%u\n",
        print_cmd->layer_id, print_cmd->surface.width, print_cmd->surface.height, print_cmd->surface.stride,
        print_cmd->surface.phy_addr, print_cmd->surface.color_fmt, print_cmd->surface.premulti_en,
        print_cmd->surface.flip_en, print_cmd->surface.compress_mode, print_cmd->surface.rotation_mode);
    if (print_cmd->surface.update_rect != TD_NULL) {
        gpu_print("update_rect x:%d y:%d w:%u h:%u", print_cmd->surface.update_rect->x,
            print_cmd->surface.update_rect->y, print_cmd->surface.update_rect->width,
            print_cmd->surface.update_rect->height);
    } else {
        gpu_print("update_rect:null");
    }
    if (print_cmd->surface.matrix != TD_NULL) {
        gpu_print(" matrix:");
        for (td_u32 i = 0; i < 3; i++) { // 3 is matrix len
            gpu_print("[");
            for (td_u32 j = 0; j < 3; j++) { // 3 is matrix len
                gpu_print(" %.4f", print_cmd->surface.matrix->matrix[i][j]);
            }
            gpu_print("]");
        }
    } else {
        gpu_print(" matrix:null");
    }
    gpu_print("\n");

    return EXT_SUCCESS;
}

static td_s32 dpu_print_output(td_u32 cmd, td_void *arg)
{
    uapi_unused(cmd);
    dpu_output_cmd *print_cmd = (dpu_output_cmd *)arg;
    gpu_print("work_scene:%u is_la:%d la:%u mir:%d cmp_out:%d  ",
        print_cmd->output.work_scene, print_cmd->output.is_la_valid, print_cmd->output.la,
        print_cmd->output.mir_en, print_cmd->output.compress_output_mode);
    gpu_print("color_type:%u color_sat:%.4f  ", print_cmd->output.color_type, print_cmd->output.color_sat);
    if (print_cmd->output.wbc_info != TD_NULL) {
        gpu_print("wbc bind:%u stride:%u w:%u h:%u addr:%#x fmt:%u\n",
            print_cmd->output.wbc_info->wbc_bind_mode, print_cmd->output.wbc_info->wbc_stride,
            print_cmd->output.wbc_info->wbc_width, print_cmd->output.wbc_info->wbc_height,
            print_cmd->output.wbc_info->wbc_addr, print_cmd->output.wbc_info->color_fmt);
    } else {
        gpu_print("wbc_info:NULL\n");
    }
    return EXT_SUCCESS;
}

static td_s32 dpu_print_colorkey(td_u32 cmd, td_void *arg)
{
    dpu_layer_colokey_cmd *print_cmd = (dpu_layer_colokey_cmd *)arg;
    if (is_get_cmd(cmd)) {
        gpu_print("layer_id:%d\n", print_cmd->layer_id);
        return EXT_SUCCESS;
    }
    gpu_print("layer_id:%d key_en:%d key_value:%#x\n",
        print_cmd->layer_id, print_cmd->colorkey.key_en, print_cmd->colorkey.key_value);
    return EXT_SUCCESS;
}

static td_s32 dpu_print_vblank(td_u32 cmd, td_void *arg)
{
    dpu_layer_vlank_cmd *print_cmd = (dpu_layer_vlank_cmd *)arg;
    if (is_get_cmd(cmd)) {
        gpu_print("layer_id:%d\n", print_cmd->layer_id);
        return EXT_SUCCESS;
    }
    gpu_print("layer_id:%d vblank:%d\n", print_cmd->layer_id, print_cmd->vblank);
    return EXT_SUCCESS;
}

static td_s32 dpu_print_none(td_u32 cmd, td_void *arg)
{
    uapi_unused(cmd);
    uapi_unused(arg);
    return EXT_SUCCESS;
}

static td_s32 dpu_print_layer_info_full(td_u32 cmd, td_void *arg)
{
    uapi_unused(cmd);
    drv_dpu_layer_info *info = (drv_dpu_layer_info *)arg;
    gpu_print("layer_id:%d w:%u h:%u fmt:%u buf_mode:%u cmp:%u rot:%u\n",
        info->layer_id, info->width, info->height, info->layer_fmt,
        info->buf_mode, info->compress_mode, info->rotation_mode);
    return EXT_SUCCESS;
}

typedef struct {
    td_u32 cmd;
    dpu_para_print_fn print_fn;
    td_char *func_name;
} dpu_cmd_print_entry;

static const dpu_cmd_print_entry g_dpu_cmd_print_list[] = {
    { DRV_DPU_CMD_CREATELAYER,       dpu_print_layer_info_full, "create_layer" },
    { DRV_DPU_CMD_DESTROYLAYER,      dpu_print_layer_id,        "destroy_layer" },
    { DRV_DPU_CMD_PUTLAYERALPHA,     dpu_print_layer_alpha,     "set_alpha" },
    { DRV_DPU_CMD_GETLAYERALPHA,     dpu_print_layer_alpha,     "get_alpha" },
    { DRV_DPU_CMD_PUTLAYERPOS,       dpu_print_layer_pos,       "set_pos" },
    { DRV_DPU_CMD_GETLAYERPOS,       dpu_print_layer_pos,       "get_pos" },
    { DRV_DPU_CMD_PUTLAYERZORDER,    dpu_print_layer_zorder,    "set_zorder" },
    { DRV_DPU_CMD_GETLAYERZORDER,    dpu_print_layer_zorder,    "get_zorder" },
    { DRV_DPU_CMD_SHOWLAYER,         dpu_print_layer_id,        "show_layer" },
    { DRV_DPU_CMD_HIDELAYER,         dpu_print_layer_id,        "hide_layer" },
    { DRV_DPU_CMD_PUTLAYERINFO,      dpu_print_layer_info,      "set_layer_info" },
    { DRV_DPU_CMD_GETLAYERINFO,      dpu_print_layer_info,      "get_layer_info" },
    { DRV_DPU_CMD_PUTYLAYERCOLORKYE, dpu_print_colorkey,        "set_colorkey" },
    { DRV_DPU_CMD_GETYLAYERCOLORKYE, dpu_print_colorkey,        "get_colorkey" },
    { DRV_DPU_CMD_REFRESHLAYERS,     dpu_print_none,            "refresh_layers" },
    { DRV_DPU_CMD_WAITLAYERVSYNC,    dpu_print_none,            "wait_vsync" },
    { DRV_DPU_CMD_PUTLAYERVBLANK,    dpu_print_vblank,          "set_vblank" },
    { DRV_DPU_CMD_GETLAYERVBLANK,    dpu_print_vblank,          "get_vblank" },
    { DRV_DPU_CMD_QUERYLAYERSTATE,   dpu_print_layer_id,        "query_state" },
    { DRV_DPU_CMD_SETOUTPUTINFO,     dpu_print_output,          "set_output_info" },
    { DRV_DPU_CMD_WAITFORDONE,       dpu_print_none,            "wait_done" },
};

#define DPU_CMD_PRINT_LIST_SIZE (sizeof(g_dpu_cmd_print_list) / sizeof(g_dpu_cmd_print_list[0]))

td_s32 dpu_para_print(td_u32 cmd, td_void *arg)
{
    td_char *func_name = "unknown";
    for (td_u32 i = 0; i < DPU_CMD_PRINT_LIST_SIZE; i++) {
        if (g_dpu_cmd_print_list[i].cmd == cmd) {
            func_name = g_dpu_cmd_print_list[i].func_name;
            if (arg == TD_NULL) {
                gpu_print("[%s] arg:NULL\n", func_name);
                return EXT_SUCCESS;
            }
            gpu_print("[%s] para: ", func_name);
            return g_dpu_cmd_print_list[i].print_fn(cmd, arg);
        }
    }

    gpu_print("unknown cmd:%u arg:%p\n", cmd, arg);
    return EXT_SUCCESS;
}