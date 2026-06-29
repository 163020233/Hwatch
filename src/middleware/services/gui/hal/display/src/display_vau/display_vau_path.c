/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: display vau hal layer
 * Author: Hisi Graphic Team
 */

#include "display_vau_internal.h"
#include "vau_render_offline_list.h"

static ext_vau_path g_path = {0};
static ext_vau_render_quality g_renderQuality = 0;
static uint8_t g_cacheStatus = 0;

VGUResult RenderClearRect(VGUSurface *target, const VGURect *rect, uint32_t color)
{
    CHECK_NULLPOINTER_RETURN_VALUE(target, VGU_INVALID_PARAMETER);
    CreateHandleIfNull();

    ext_vau_surface vauSurface = {0};
    if (Convert2VauSurface(&target->buffer, &vauSurface) != VGU_SUCCESS) {
        DISP_LOGE("Convert2VauSurface failed\n");
        return VGU_OPERATION_FAILED;
    }

    ext_vau_surface_list surface_list = {0};
    surface_list.src_surface_cnt = 1;
    surface_list.src_surface = &vauSurface;
    ext_rect curRect = {0, 0, target->buffer.width, target->buffer.height};
    if (rect != NULL) {
        curRect.x = rect->x;
        curRect.y = rect->y;
        curRect.width = rect->w;
        curRect.height = rect->h;
    }
    surface_list.src_surface->rect = curRect;

    ext_vau_surface dst_surface = {0};
    dst_surface = vauSurface;
    surface_list.dst_surface = &dst_surface;
    surface_list.src_surface->is_color_surface = TD_TRUE;
    surface_list.src_surface->color = color;
    surface_list.src_surface->color_fmt = EXT_GFX_FMT_ARGB8888;
    surface_list.src_surface->stride = CalcStride(surface_list.src_surface->width, PIXEL_FMT_RGBA_8888,
        target->buffer.compressMode);

    int32_t ret = uapi_vau_render(g_hdl, &surface_list);
    if (ret != VGU_SUCCESS) {
        DISP_LOGE("uapi_vau_render failure, ret = 0x%x!\n", ret);
        uapi_vau_cancel(g_hdl);
        g_hdl = INVALID_HDL;
        return VGU_OPERATION_FAILED;
    }

    return VGU_SUCCESS;
}

VGUResult VGUPathSetData(uintptr_t path, uint32_t num, const uint8_t *cmd, uint32_t dataNum,
    const void *data)
{
    CHECK_NULLPOINTER_RETURN_VALUE(cmd, VGU_INVALID_PARAMETER);
    if (num == 0) {
        DISP_LOGE("invalid cmd num: 0.\n");
        return VGU_INVALID_PARAMETER;
    }

    if (path != g_hdl) {
        DISP_LOGE("VGUPathSetData with invalid path.\n");
        return VGU_INVALID_PARAMETER;
    }

    g_path.cmd_num = num;
    g_path.cmds = cmd;
    g_path.data_num = dataNum;
    g_path.datas = data;
    return VGU_SUCCESS;
}

VGUResult VGUPathAppendData(uintptr_t path, uint32_t num, const uint8_t *cmd, uint32_t dataNum,
    const void *data)
{
    CHECK_NULLPOINTER_RETURN_VALUE(cmd, VGU_INVALID_PARAMETER);
    if (num == 0) {
        DISP_LOGE("invalid cmd num: 0.\n");
        return VGU_INVALID_PARAMETER;
    }

    if (path != g_hdl) {
        DISP_LOGE("VGUPathAppendData with invalid path.\n");
        return VGU_INVALID_PARAMETER;
    }

    uint32_t oldCmdCount = g_path.cmd_num;
    g_path.cmd_num += num;
    if (g_path.cmds != NULL) {
        td_uchar *temp = (td_uchar *)DispMalloc(sizeof(td_uchar) * g_path.cmd_num);
        if (memcpy_s(temp, sizeof(td_uchar) * g_path.cmd_num, g_path.cmds, sizeof(td_uchar) * oldCmdCount) != EOK) {
            DISP_LOGE("%s: %d memcpy_s failed\n", __FUNCTION__, __LINE__);
            return VGU_OPERATION_FAILED;
        }
        DispFree(g_path.cmds);
        g_path.cmds = temp;
    } else {
        g_path.cmds = (td_uchar *)DispMalloc(sizeof(td_uchar) * g_path.cmd_num);
    }
    if (memcpy_s(g_path.cmds + oldCmdCount, sizeof(td_uchar) * (g_path.cmd_num - oldCmdCount),
        cmd, sizeof(uint8_t) * num) != EOK) {
        DISP_LOGE("%s: %d memcpy_s for cmd failed.\n", __FUNCTION__, __LINE__);
        return VGU_OPERATION_FAILED;
    }

    uint32_t oldDataCount = g_path.data_num;
    g_path.data_num += dataNum;
    if (dataNum != 0 && data != NULL) {
        if (g_path.datas != NULL) {
            float *temp = (float *)DispMalloc(sizeof(float) * g_path.data_num);
            if (memcpy_s(temp, sizeof(float) * g_path.data_num, g_path.datas, sizeof(float) * oldDataCount) != EOK) {
                DISP_LOGE("%s: %d memcpy_s failed\n", __FUNCTION__, __LINE__);
                return VGU_OPERATION_FAILED;
            }
            DispFree(g_path.datas);
            g_path.datas = temp;
        } else {
            g_path.datas = (float *)DispMalloc(sizeof(float) * g_path.data_num);
        }
        if (memcpy_s(g_path.datas + oldDataCount, sizeof(float) * (g_path.data_num - oldDataCount),
            (float *)data, sizeof(float) * dataNum) != EOK) {
            return VGU_OPERATION_FAILED;
        }
    }
    return VGU_SUCCESS;
}

static void ConvertFillSpread(VGUFillSpread vguSpread, ext_vau_paint_tile_mode* tile)
{
    switch (vguSpread) {
        case VGU_SPREAD_PAD:
            *tile = EXT_VAU_PAINT_TILE_MODE_CLAMP;
            break;
        case VGU_SPREAD_REFLECT:
            *tile = EXT_VAU_PAINT_TILE_MODE_MIRROR;
            break;
        case VGU_SPREAD_REPEAT:
            *tile = EXT_VAU_PAINT_TILE_MODE_REPEAT;
            break;
        default:
            *tile = EXT_VAU_PAINT_TILE_MODE_CLAMP;
            break;
    }
}

static VGUResult HandlePaintStyle(ext_vau_paint_attr *paint, VGUPaintStyle *style, uint16_t *stopNum)
{
    CHECK_NULLPOINTER_RETURN_VALUE(style, VGU_INVALID_PARAMETER);
    CHECK_NULLPOINTER_RETURN_VALUE(paint, VGU_INVALID_PARAMETER);

    if (style->type == VGU_PAINT_COLOR) {
        paint->paint_type = EXT_VAU_PAINT_TYPE_COLOR;
        paint->solid.color = style->color;
    } else if (style->type == VGU_PAINT_COLOR_ALPHA_FF) {
        paint->paint_type = EXT_VAU_PAINT_TYPE_COLOR_ALPHA_FF;
        paint->solid.color = style->color;
    } else if (style->type == VGU_PAINT_GRADIENT) {
        VGUGradient* gradient = style->gradient;
        if (gradient == NULL) {
            return VGU_INVALID_PARAMETER;
        }
        *stopNum = style->gradient->stopCount;
        ConvertFillSpread(style->gradient->spread, &paint->gradient.tile_mode);
        if (gradient->type == VGU_GRADIENT_LINEAR) {
            paint->paint_type = EXT_VAU_PAINT_TYPE_LINEAR_GRADIENT;
            paint->gradient.linear.start_point.x = gradient->linear.x1;
            paint->gradient.linear.start_point.y = gradient->linear.y1;
            paint->gradient.linear.end_point.x = gradient->linear.x2;
            paint->gradient.linear.end_point.y = gradient->linear.y2;
        } else if (gradient->type == VGU_GRADIENT_RADIAL) {
            paint->paint_type = EXT_VAU_PAINT_TYPE_RADIAL_GRADIENT;
            paint->gradient.radial.radius =  gradient->radial.r0;
            paint->gradient.radial.center.x = gradient->radial.x0;
            paint->gradient.radial.center.y = gradient->radial.y0;
        } else if (gradient->type == VGU_GRADIENT_SWEEP) {
            paint->paint_type = EXT_VAU_PAINT_TYPE_SWEEP_GRADIENT;
            paint->gradient.sweep.center.x = gradient->sweep.cx;
            paint->gradient.sweep.center.y = gradient->sweep.cy;
            paint->gradient.sweep.start_angle = gradient->sweep.startAngle;
        } else {
            DISP_LOGE("Invalid gradient type: %d\n", gradient->type);
            return VGU_INVALID_PARAMETER;
        }
    } else if (style->type == VGU_PAINT_PATTERN) {
        paint->paint_type = EXT_VAU_PAINT_TYPE_COLOR;
        paint->solid.color = style->pattern->image->buffer.color;
        return VGU_SUCCESS;
    }
    return VGU_SUCCESS;
}

static VGUResult HandlePathFillAttr(const VGUFillAttr *fillAttr, ext_vau_fill_attr *attr, uint16_t *stopNum)
{
    if (fillAttr->rule == VGU_RULE_WINDING) {
        attr->type = EXT_VAU_NON_ZERO_FILL;
    } else if (fillAttr->rule == VGU_RULE_EVEN_ODD) {
        attr->type = EXT_VAU_EVEN_ODD_FILL;
    } else {
        DISP_LOGE("Invalid VGUFillRule.\n");
        return VGU_INVALID_PARAMETER;
    }

    if (HandlePaintStyle(&attr->paint_attr, fillAttr->style, stopNum) != VGU_SUCCESS) {
        DISP_LOGE("HandlePaintStyle failed.\n");
        return VGU_OPERATION_FAILED;
    }
    return VGU_SUCCESS;
}

static VGUResult Convert2VGCapStyle(VGULineCap vguCap, ext_vau_stroke_line_cap *vgCap)
{
    VGUResult result = VGU_SUCCESS;
    switch (vguCap) {
        case VGU_LINECAP_BUTT:
            *vgCap = EXT_VAU_STROKE_LINE_CAP_BUTT;
            break;
        case VGU_LINECAP_ROUND:
            *vgCap = EXT_VAU_STROKE_LINE_CAP_ROUND;
            break;
        case VGU_LINECAP_SQUARE:
            *vgCap = EXT_VAU_STROKE_LINE_CAP_SQUARE;
            break;
        default:
            DISP_LOGE("Invalid Cap Style!\n");
            result = VGU_INVALID_PARAMETER;
            break;
    }
    return result;
}

static VGUResult Convert2VGJoinStyle(VGUJointType vguJoin, ext_vau_stroke_line_join *vgJoin)
{
    VGUResult result = VGU_SUCCESS;
    switch (vguJoin) {
        case VGU_LINE_JOIN_MITER:
            *vgJoin = EXT_VAU_STROKE_LINE_JOIN_MITER;
            break;
        case VGU_LINE_JOIN_ROUND:
            *vgJoin = EXT_VAU_STROKE_LINE_JOIN_ROUND;
            break;
        case VGU_LINE_JOIN_BEVEL:
            *vgJoin = EXT_VAU_STROKE_LINE_JOIN_BEVEL;
            break;
        default:
            DISP_LOGE("Invalid Join Style!\n");
            result = VGU_INVALID_PARAMETER;
            break;
    }
    return result;
}

static VGUResult Convert2VGDashStyle(VGUDashAttr *vguDash, ext_vau_dash *vgDash)
{
    VGUResult result = VGU_SUCCESS;

    if (vguDash != NULL && vguDash->count != 0) {
        if (vguDash->lengths == NULL) {
            DISP_LOGE("Invalid dash parameter, count %u lengths %p\n", vguDash->count, vguDash->lengths);
            return VGU_INVALID_PARAMETER;
        }
        vgDash->count = vguDash->count;
        vgDash->array = vguDash->lengths;
        vgDash->offset = vguDash->offset;
        vgDash->reset = vguDash->reset;
        vgDash->enable = true;
    }

    return result;
}

static VGUResult HandlePathStrokeAttr(const VGUStrokeAttr *strokeAttr, ext_vau_stroke_attr *attr, uint16_t *stopNum)
{
    if ((Convert2VGCapStyle(strokeAttr->cap, &attr->cap) != VGU_SUCCESS) ||
        (Convert2VGJoinStyle(strokeAttr->join, &attr->join) != VGU_SUCCESS)) {
        return VGU_INVALID_PARAMETER;
    }
    if (Convert2VGDashStyle(strokeAttr->dash, &attr->dash) != VGU_SUCCESS) {
        return VGU_OPERATION_FAILED;
    }
    if (HandlePaintStyle(&attr->paint_attr, strokeAttr->style, stopNum) != VGU_SUCCESS) {
        DISP_LOGE("HandlePaintStyle failed.\n");
        return VGU_OPERATION_FAILED;
    }
    attr->width = strokeAttr->width;
    attr->miter_limit = strokeAttr->miterLimit;
    return VGU_SUCCESS;
}

static void ConvertVGUStops(const VGUGradient *gradient, ext_vau_color *colors,
    td_float *offsets, ext_vau_paint_attr *paintAttr)
{
    for (uint16_t i = 0; i < gradient->stopCount; i++) {
        offsets[i] = gradient->colorStops[i].stop;
        colors[i] = gradient->colorStops[i].color;
    }
    if ((gradient->type == VGU_GRADIENT_LINEAR) ||
        (gradient->type == VGU_GRADIENT_RADIAL) ||
        (gradient->type == VGU_GRADIENT_SWEEP)) {
        paintAttr->gradient.stops.colors = colors;
        paintAttr->gradient.stops.offsets = offsets;
        paintAttr->gradient.stops.num = (td_u8)(gradient->stopCount);
    }
}

static void HandlePathBlend(ext_vau_surface *src, ext_vau_surface *dst, ext_vau_blit_opt *opt)
{
    dst->alpha_info.global_alpha_en = TD_FALSE;
    dst->alpha_info.global_alpha = 0xFF;
    dst->alpha_info.pixel_alpha_en = TD_TRUE;
    dst->alpha_info.premult_en = TD_TRUE;

    src->alpha_info.premult_en = TD_TRUE;
    src->alpha_info.pixel_alpha_en = TD_TRUE;
    src->alpha_info.global_alpha_en = TD_FALSE;
    src->alpha_info.global_alpha = 0XFF;

    opt->blend_opt.blend_enable = TD_TRUE;
    opt->blend_opt.blend_cmd = EXT_VAU_BLENDCMD_SRCOVER;
}

static VGUResult CheckAndGetPathRect(ext_rect *pathRect)
{
    td_uchar *cmds = g_path.cmds;
    td_u32 cmd_num = g_path.cmd_num;
    td_float *datas = g_path.datas;
    td_s32 x = 0;
    td_s32 y = 0;
    int j = 0;

    for (td_u32 i = 0; i < cmd_num; i++) {
        if (cmds[i] == VAU_PATH_MOVE_TO) { // move to start line
            pathRect->x = x = datas[j++];
            pathRect->y = y = datas[j++];
        } else if (cmds[i] == VAU_PATH_VLINE_TO) { // draw vertical line
            pathRect->height = (td_u32)VauMax((int32_t)abs((int)datas[j] - pathRect->y), (int32_t)pathRect->height);
            y = datas[j++];
        } else if (cmds[i] == VAU_PATH_HLINE_TO) { // draw horizontal line
            pathRect->width = (td_u32)VauMax((int32_t)abs((int)datas[j] - pathRect->x), (int32_t)pathRect->width);
            x = datas[j++];
        } else if (cmds[i] == VAU_PATH_CLOSE_PATH) { // outline close
            if (((pathRect->x == x) || (pathRect->y == y)) && (pathRect->width != 0) && (pathRect->height != 0)) {
                return VGU_SUCCESS;
            }
        } else if (cmds[i] == VAU_PATH_LINE_TO) { // draw line
            if (VauEqual(datas[j], (float)x) || VauEqual(datas[j + 1], (float)y)) {
                pathRect->width = (td_u32)VauMax((int32_t)abs((int)datas[j] - pathRect->x), (int32_t)pathRect->width);
                x = datas[j++];
                pathRect->height = (td_u32)VauMax((int32_t)abs((int)datas[j] - pathRect->y), (int32_t)pathRect->height);
                y = datas[j++];
            } else {
                return VGU_OPERATION_FAILED;
            }
        } else {
            return VGU_OPERATION_FAILED;
        }
    }

    if ((pathRect->x == x) && (pathRect->y == y)) {
        return VGU_SUCCESS;
    }

    return VGU_OPERATION_FAILED;
}

static VGUResult InitPathDrawAttr(ext_vau_draw_attr *draw, DrawAttrParams *params,
    const VGUFillAttr *fillAttr, const VGUStrokeAttr *strokeAttr)
{
    draw->paint = &params->paint;
    uint16_t fillStopNum = 0;
    uint16_t strokeStopNum = 0;
    if ((fillAttr != NULL) &&
        (HandlePathFillAttr(fillAttr, &params->vaufillAttr, &fillStopNum) == VGU_SUCCESS)) {
        if (fillStopNum > MAX_GRADIENT_STOPS_NUM) {
            DISP_LOGE("invalid fill stops num: %u!\n", fillStopNum);
            return VGU_INVALID_PARAMETER;
        }
        draw->paint->fill_attr = &params->vaufillAttr;
        draw->paint->paint_mode |= EXT_VAU_PAINT_MODE_FILL;
        if ((fillStopNum != 0) && ((draw->paint->paint_mode & EXT_VAU_PAINT_MODE_FILL) == EXT_VAU_PAINT_MODE_FILL)) {
            ConvertVGUStops(fillAttr->style->gradient, params->fillColors, params->fillOffsets,
                &draw->paint->fill_attr->paint_attr);
        }
    }
    if ((strokeAttr != NULL) &&
        (HandlePathStrokeAttr(strokeAttr, &params->vauStrokeAttr, &strokeStopNum) == VGU_SUCCESS)) {
        if (strokeStopNum > MAX_GRADIENT_STOPS_NUM) {
            DISP_LOGE("invalid stroke stops num: %u!\n", strokeStopNum);
            return VGU_INVALID_PARAMETER;
        }
        draw->paint->stroke_attr = &params->vauStrokeAttr;
        draw->paint->paint_mode |= EXT_VAU_PAINT_MODE_STROKE;
        if ((strokeStopNum != 0) &&
            ((draw->paint->paint_mode & EXT_VAU_PAINT_MODE_STROKE) == EXT_VAU_PAINT_MODE_STROKE)) {
            ConvertVGUStops(strokeAttr->style->gradient, params->strokeColors, params->strokeOffsets,
                &draw->paint->stroke_attr->paint_attr);
        }
    }
    if (draw->paint->paint_mode == EXT_VAU_PAINT_MODE_NONE) {
        return VGU_OPERATION_FAILED;
    }

    draw->path = &g_path;
    draw->paint->render_cfg.render_quality = g_renderQuality;
    return VGU_SUCCESS;
}

static void InitMatrix(ext_vau_matrix **dstMixPtr, ext_vau_matrix *dstMatrix, const VGUMatrix3 *srcMatrix)
{
    if (srcMatrix != NULL) {
        ext_vau_matrix mat = {{{srcMatrix->m[0][0], srcMatrix->m[0][1], srcMatrix->m[0][2]},
            {srcMatrix->m[1][0], srcMatrix->m[1][1], srcMatrix->m[1][2]},
            {srcMatrix->m[2][0], srcMatrix->m[2][1], srcMatrix->m[2][2]}}};
        *dstMatrix = mat;
        *dstMixPtr = dstMatrix;
    } else {
#ifndef _WIN32
        *dstMixPtr = NULL;
#else
        ext_vau_matrix mat = {{{1, 0, 0},
            {0, 1, 0},
            {0, 0, 1}}};
        *dstMatrix = mat;
        *dstMixPtr = dstMatrix;
#endif
    }
}

static VGUResult InitPathSurfaceList(ext_vau_surface_list *surfaceList, SurfaceListParams *params,
    VGUSurface *target, const VGUMatrix3 *matrix, const VGUFillAttr *fillAttr, const VGUStrokeAttr *strokeAttr)
{
    if (fillAttr == NULL && strokeAttr == NULL) {
        return VGU_OPERATION_FAILED;
    }
    if (Convert2VauSurface(&target->buffer, &params->dst) != VGU_SUCCESS) {
        return VGU_OPERATION_FAILED;
    }
    ext_rect dstRect = {0, 0, params->dst.width, params->dst.height};
    params->dst.rect = dstRect;
    surfaceList->dst_surface = &params->dst;

    ext_vau_surface src = {0};
    src.width = target->buffer.width;
    src.height = target->buffer.height;
    src.color_fmt = EXT_GFX_FMT_ARGB8888;
    src.argb_order = EXT_GFX_FMT_ORDER_ARGB;
    src.compress_mode = params->dst.compress_mode;
    src.rect.x = 0;
    src.rect.y = 0;
    src.rect.width = target->buffer.width;
    src.rect.height = target->buffer.height;
    src.stride = CalcStride(src.width, PIXEL_FMT_RGBA_8888, target->buffer.compressMode);
    src.vir_addr = params->dst.vir_addr;
    src.phy_addr = params->dst.phy_addr;

    InitMatrix(&src.matrix, &params->vauMatrix, matrix);

    VGUResult result = InitPathDrawAttr(&params->drawAttr, &params->drawParams, fillAttr, strokeAttr);
    if (result != VGU_SUCCESS) {
        DISP_LOGE("InitPathDrawAttr failure, ret = 0x%x!\n", result);
        return result;
    }
    src.draw = &params->drawAttr;

    if (target->clipNum != 0) {
        params->dst.rect.x = target->clipRects[0].x;
        params->dst.rect.y = target->clipRects[0].y;
        params->dst.rect.width = target->clipRects[0].w;
        params->dst.rect.height = target->clipRects[0].h;
    }

    params->srcs[0] = params->dst;
    params->srcs[1] = src;
    surfaceList->src_surface = params->srcs;
    surfaceList->src_surface_cnt = 2; // 2: src count

    surfaceList->opt = &params->opt;
    HandlePathBlend(&surfaceList->src_surface[1], &surfaceList->src_surface[0], surfaceList->opt);
    return VGU_SUCCESS;
}

static VGUResult RenderPathInternal(VGUSurface *target, td_handle handle, const VGUMatrix3 *matrix,
    const VGUFillAttr *fillAttr, const VGUStrokeAttr *strokeAttr)
{
    ext_vau_surface_list surfaceList = {0};
    SurfaceListParams params = {0};
    VGUResult result = (VGUResult)(InitPathSurfaceList(&surfaceList, &params, target, matrix, fillAttr, strokeAttr));
    if (result != VGU_SUCCESS) {
        DISP_LOGE("InitPathSurfaceList failure, ret = 0x%x!\n", result);
        goto SAFE_EXIT;
    }

    result = (VGUResult)(uapi_vau_render(handle, &surfaceList));
    if (result != VGU_SUCCESS) {
        DISP_LOGE("uapi_vau_render failure, ret = 0x%x!\n", result);
    }

SAFE_EXIT:
    memset_s(&g_path, sizeof(ext_vau_path), 0, sizeof(ext_vau_path));
    g_renderQuality = 0;
    return result;
}

static VGUResult RenderPatternInternal(VGUSurface *target, ext_rect rect, const VGUPattern *pattern, td_handle handle)
{
    ext_vau_surface_list surfaceList = { 0 };
    ext_vau_surface dst = { 0 };
    if (Convert2VauSurface(&target->buffer, &dst) != VGU_SUCCESS) {
        return VGU_OPERATION_FAILED;
    }
    dst.argb_order = EXT_GFX_FMT_ORDER_ARGB;
    dst.vir_addr = (td_uchar *)(uintptr_t)dst.phy_addr;
    dst.rect = rect;
    surfaceList.dst_surface = &dst;

    ext_vau_surface src = { 0 };
    if (Convert2VauSurface(&pattern->image->buffer, &src) != VGU_SUCCESS) {
        return VGU_OPERATION_FAILED;
    }

    if (!HandleCompressMode(pattern->image, &src)) {
        return VGU_OPERATION_FAILED;
    }

    src.vir_addr = (td_uchar *)(uintptr_t)src.phy_addr;
    src.argb_order = EXT_GFX_FMT_ORDER_ARGB;

    src.rect.x = 0;
    src.rect.y = 0;
    src.rect.width = src.width;
    src.rect.height = src.height;

    ext_vau_pattern_attr vauPattern = { 0 };
    vauPattern.argb = pattern->image->buffer.color;
    vauPattern.mode = pattern->wrapx;
    src.pattern = &vauPattern;

    ext_vau_matrix vauMatrix = { 0 };
    VGUMatrix3 *matrix = &pattern->image->matrix;
    if (VauEqual(matrix->m[2][2], 1.0f)) { // 2: col & rol
        ext_vau_matrix mat = { { { matrix->m[0][0], matrix->m[0][1], matrix->m[0][2] },
                                 { matrix->m[1][0], matrix->m[1][1], matrix->m[1][2] },
                                 { matrix->m[2][0], matrix->m[2][1], matrix->m[2][2] } } };
        vauMatrix = mat;
        src.matrix = &vauMatrix;
    } else {
        ext_vau_matrix mat = { { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } } };
        vauMatrix = mat;
        src.matrix = &vauMatrix;
    }

    surfaceList.src_surface = &src;
    surfaceList.src_surface_cnt = 1;

    uint32_t result = (uint32_t)(uapi_vau_render(handle, &surfaceList));
    if (result != VGU_SUCCESS) {
        DISP_LOGE("uapi_vau_render failure, ret = 0x%x!\n", result);
        goto ERROR;
    }

    memset_s(&g_path, sizeof(ext_vau_path), 0, sizeof(ext_vau_path));
    g_renderQuality = 0;
    return VGU_SUCCESS;
ERROR:
    uapi_vau_cancel(handle);
    g_hdl = INVALID_HDL;
    memset_s(&g_path, sizeof(ext_vau_path), 0, sizeof(ext_vau_path));
    g_renderQuality = 0;
    return result;
}

static VGUResult BlendImgToSurface(VGUImage *src, VGUSurface *dst,
    ext_vau_blend_cmd cmd, td_handle handle)
{
    ext_vau_surface srcSurface[2] = {0};
    if (Convert2VauSurface(&dst->buffer, &srcSurface[0]) != VGU_SUCCESS) {
        return VGU_OPERATION_FAILED;
    }
    ext_rect dstRect = {dst->clipRects[0].x, dst->clipRects[0].y, dst->clipRects[0].w, dst->clipRects[0].h};
    srcSurface[0].rect = dstRect;
    srcSurface[0].vir_addr = (td_uchar *)(uintptr_t)srcSurface[0].phy_addr;

    if (Convert2VauSurface(&src->buffer, &srcSurface[1]) != VGU_SUCCESS) {
        return VGU_OPERATION_FAILED;
    }
    HandleCompressMode(src, &srcSurface[1]);

    ext_rect srcRect = { src->rect.x, src->rect.y, src->rect.w, src->rect.h };
    srcSurface[1].rect = srcRect;

    ext_vau_matrix mat = {{{src->matrix.m[0][0], src->matrix.m[0][1], src->matrix.m[0][2]},
                           {src->matrix.m[1][0], src->matrix.m[1][1], src->matrix.m[1][2]},
                           {src->matrix.m[2][0], src->matrix.m[2][1], src->matrix.m[2][2]}}};

#if MATRIX_TRANSFORM_SPACE_WORLD
    Local2WorldMatrix(&mat, &src->rect);
#endif

    srcSurface[1].matrix = &mat;
    srcSurface[1].vir_addr = (td_uchar *)(uintptr_t)srcSurface[1].phy_addr;
    srcSurface[1].alpha_info.premult_en = TD_TRUE;
    srcSurface[1].alpha_info.pixel_alpha_en = TD_TRUE;
    srcSurface[1].alpha_info.global_alpha_en = TD_FALSE;
    srcSurface[1].alpha_info.global_alpha = 0XFF;

    ext_vau_surface_list surfaceList = { 0 };
    ext_vau_blit_opt blitOpt = { 0 };
    blitOpt.blend_opt.blend_enable = TD_TRUE;
    blitOpt.blend_opt.blend_cmd = cmd;
    blitOpt.blend_opt.out_alpha_mode = EXT_VAU_OUTALPHA_FROM_NORM;
    surfaceList.opt = &blitOpt;
    surfaceList.src_surface_cnt = 2; // 2: src surface count
    surfaceList.src_surface = srcSurface;
    surfaceList.dst_surface = &srcSurface[0];

    int32_t ret = uapi_vau_render(handle, &surfaceList);
    if (ret != VGU_SUCCESS) {
        uapi_vau_cancel(handle);
        g_hdl = INVALID_HDL;
        return VGU_OPERATION_FAILED;
    }

    return VGU_SUCCESS;
}

static VGUResult BlendToSurface(VGUSurface *src, VGUSurface *dst, ext_vau_blend_cmd cmd, td_handle handle)
{
    ext_vau_surface srcSurface[2] = {0};
    if (Convert2VauSurface(&dst->buffer, &srcSurface[0]) != VGU_SUCCESS) {
        return VGU_OPERATION_FAILED;
    }
    ext_rect dstRect = {dst->clipRects[0].x, dst->clipRects[0].y, dst->clipRects[0].w, dst->clipRects[0].h};
    srcSurface[0].rect = dstRect;
    srcSurface[0].vir_addr = (td_uchar *)(uintptr_t)srcSurface[0].phy_addr;

    if (Convert2VauSurface(&src->buffer, &srcSurface[1]) != VGU_SUCCESS) {
        return VGU_OPERATION_FAILED;
    }
    ext_rect srcRect = { 0, 0, src->buffer.width, src->buffer.height };
    srcSurface[1].rect = srcRect;
    ext_vau_matrix mat = { { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } } };
    srcSurface[1].matrix = &mat;
    srcSurface[1].vir_addr = (td_uchar *)(uintptr_t)srcSurface[1].phy_addr;
    srcSurface[1].alpha_info.premult_en = TD_TRUE;
    srcSurface[1].alpha_info.pixel_alpha_en = TD_TRUE;
    srcSurface[1].alpha_info.global_alpha_en = TD_FALSE;
    srcSurface[1].alpha_info.global_alpha = 0XFF;

    ext_vau_surface_list surfaceList = { 0 };
    ext_vau_blit_opt blitOpt = { 0 };
    blitOpt.blend_opt.blend_enable = TD_TRUE;
    blitOpt.blend_opt.blend_cmd = cmd;
    blitOpt.blend_opt.out_alpha_mode = EXT_VAU_OUTALPHA_FROM_NORM;
    surfaceList.opt = &blitOpt;
    surfaceList.src_surface_cnt = 2; // 2: src surface count
    surfaceList.src_surface = srcSurface;
    surfaceList.dst_surface = &srcSurface[0];

    int32_t ret = uapi_vau_render(handle, &surfaceList);
    if (ret != VGU_SUCCESS) {
        uapi_vau_cancel(handle);
        g_hdl = INVALID_HDL;
        return VGU_OPERATION_FAILED;
    }

    return VGU_SUCCESS;
}

static bool Intersect(ext_rect* dst, const ext_rect* rect1, const ext_rect* rect2)
{
    int32_t x1 = VauMin(rect1->x + (int32_t)(rect1->width) - 1, rect2->x + (int32_t)(rect2->width) - 1);
    int32_t y1 = VauMin(rect1->y + (int32_t)(rect1->height) - 1, rect2->y + (int32_t)(rect2->height) - 1);
    dst->x = VauMax(rect1->x, rect2->x);
    dst->y = VauMax(rect1->y, rect2->y);
    dst->width = (td_u32)(x1 - dst->x + 1);
    dst->height = (td_u32)(y1 - dst->y + 1);

    if ((dst->x < x1) && (dst->y < y1)) {
        return true;
    }

    return false;
}

static VGUResult BlendPatternToSurface(VGUSurface *target, const VGUMatrix3 *matrix, const VGUFillAttr *fillAttr,
    const VGUStrokeAttr *strokeAttr, VGUPattern *pattern)
{
    VGUResult result = VGU_SUCCESS;
    ext_rect rect = {target->clipRects[0].x, target->clipRects[0].y, target->clipRects[0].w, target->clipRects[0].h};
    bool isWarpDisable = (pattern->wrapx == VGU_WRAP_PATTERN_DISABLE && pattern->wrapy == VGU_WRAP_PATTERN_DISABLE);
    VGUSurface tmpTarget1 = *target;
    tmpTarget1.buffer.pixelFormat = PIXEL_FMT_RGBA_8888;
    tmpTarget1.buffer.stride = CalcStride(target->buffer.width, PIXEL_FMT_RGBA_8888,
        target->buffer.compressMode);
    uint32_t datasize = CalcSize(tmpTarget1.buffer.stride, target->buffer.height, target->buffer.compressMode);
    tmpTarget1.buffer.memHandle = (uint64_t)(uintptr_t)DispMalloc(datasize);
    if (tmpTarget1.buffer.memHandle == 0) {
        DISP_LOGE("[%d] DispMalloc for tmpTarget1 failed\n", __LINE__);
        result = VGU_OPERATION_FAILED;
        goto OUT;
    }
    RenderClearRect(&tmpTarget1, NULL, 0x0);
    VGUSurface tmpTarget2 = tmpTarget1;

    if (RenderPathInternal(&tmpTarget1, g_hdl, matrix, fillAttr, strokeAttr) != VGU_SUCCESS) {
        DISP_LOGE("[%d] RenderPathInternal failed\n", __LINE__);
        result = VGU_OPERATION_FAILED;
        goto OUT;
    }

    if (isWarpDisable) {
        pattern->image->rect.x = 0;
        pattern->image->rect.y = 0;
        pattern->image->rect.w = pattern->image->buffer.width;
        pattern->image->rect.h = pattern->image->buffer.height;
        if (BlendImgToSurface(pattern->image, &tmpTarget1, EXT_VAU_BLENDCMD_SRCIN, g_hdl) != VGU_SUCCESS) {
            DISP_LOGE("[%d] BlendToSurface failed\n", __LINE__);
            result = VGU_OPERATION_FAILED;
            goto OUT;
        }
    } else {
        tmpTarget2.buffer.memHandle = (uint64_t)(uintptr_t)DispMalloc(datasize);
        if (tmpTarget2.buffer.memHandle == 0) {
            DISP_LOGE("[%d] DispMalloc for tmpTarget2 failed\n", __LINE__);
            result = VGU_OPERATION_FAILED;
            goto OUT;
        }
        RenderClearRect(&tmpTarget2, NULL, 0x0);

        if (RenderPatternInternal(&tmpTarget2, rect, pattern, g_hdl) != VGU_SUCCESS) {
            DISP_LOGE("[%d] RenderPatternInternal failed\n", __LINE__);
            result = VGU_OPERATION_FAILED;
            goto OUT;
        }
        if (BlendToSurface(&tmpTarget2, &tmpTarget1, EXT_VAU_BLENDCMD_SRCIN, g_hdl) != VGU_SUCCESS) {
            DISP_LOGE("[%d] BlendToSurface failed\n", __LINE__);
            result = VGU_OPERATION_FAILED;
            goto OUT;
        }
    }

    if (BlendToSurface(&tmpTarget1, target, EXT_VAU_BLENDCMD_SRCOVER, g_hdl) != VGU_SUCCESS) {
        DISP_LOGE("[%d] BlendToSurface failed\n", __LINE__);
        result = VGU_OPERATION_FAILED;
    }
OUT:
    if (result == VGU_OPERATION_FAILED) {
        DispFree((void *)(uintptr_t)tmpTarget1.buffer.memHandle);
        if (!isWarpDisable) {
            DispFree((void *)(uintptr_t)tmpTarget2.buffer.memHandle);
        }
        return result;
    }
    UpdateBufferHdlState((void *)(uintptr_t)tmpTarget1.buffer.memHandle, BUFFER_HDL_RENDER);
    if (!isWarpDisable) {
        UpdateBufferHdlState((void *)(uintptr_t)tmpTarget2.buffer.memHandle, BUFFER_HDL_RENDER);
    }
    return VGU_SUCCESS;
}

static VGUResult RenderFillPattern(VGUSurface *target, const VGUMatrix3 *matrix, const VGUFillAttr *fillAttr)
{
    VGUResult result = VGU_SUCCESS;
    ext_rect pathRect = { 0, 0, 0, 0 };
    if (VauEqual(matrix->m[0][1], 0.0f) && VauEqual(matrix->m[1][0], 0.0f) &&
        (CheckAndGetPathRect(&pathRect) == VGU_SUCCESS)) {
        pathRect.x = (int32_t)(pathRect.x + matrix->m[0][2]); // 2: row & col
        pathRect.y = (int32_t)(pathRect.y + matrix->m[1][2]); // 2: row & col
        pathRect.width = (td_u32)(pathRect.width * matrix->m[0][0]);
        pathRect.height = (td_u32)(pathRect.height * matrix->m[1][1]);

        ext_rect clipRect = {target->clipRects[0].x, target->clipRects[0].y,
            target->clipRects[0].w, target->clipRects[0].h};
        if (!Intersect(&pathRect, &pathRect, &clipRect)) {
            return VGU_SUCCESS;
        }

        result = RenderPatternInternal(target, pathRect, fillAttr->style->pattern, g_hdl);
    } else {
        result = BlendPatternToSurface(target, matrix, fillAttr, NULL, fillAttr->style->pattern);
    }
    return result;
}

VGUResult RenderPath(VGUSurface *target, uintptr_t path, const VGUMatrix3 *matrix, const VGUFillAttr *fillAttr,
    const VGUStrokeAttr *strokeAttr)
{
    CHECK_NULLPOINTER_RETURN_VALUE(target, VGU_INVALID_PARAMETER);
    if (path != g_hdl) {
        DISP_LOGE("RenderPath with invalid path.\n");
        return VGU_INVALID_PARAMETER;
    }

    if ((fillAttr == NULL) && (strokeAttr == NULL)) {
        DISP_LOGE("One of fill and stroke in path attr should be set!\n");
        return VGU_INVALID_PARAMETER;
    }

    VGUResult result = VGU_SUCCESS;
    if ((fillAttr != NULL) && (fillAttr->style->type == VGU_PAINT_PATTERN)) {
        result = RenderFillPattern(target, matrix, fillAttr);
        return result;
    };

    if ((strokeAttr != NULL) && (strokeAttr->style->type == VGU_PAINT_PATTERN)) {
        result = BlendPatternToSurface(target, matrix, NULL, strokeAttr, strokeAttr->style->pattern);
        return result;
    };

    result = RenderPathInternal(target, g_hdl, matrix, fillAttr, strokeAttr);
    return result;
}

VGUResult ClipImgByPath(VGUSurface *target, const VGUImage *src, uintptr_t path,
    const VGUMatrix3 *matrix, const VGUFillAttr *fillAttr)
{
    RenderClearRect(target, NULL, 0x0);
    if (RenderPathInternal(target, path, matrix, fillAttr, NULL) != VGU_SUCCESS) {
        DISP_LOGE("[%d] RenderPathInternal failed\n", __LINE__);
        return VGU_OPERATION_FAILED;
    }
    if (BlendImgToSurface(src, target, EXT_VAU_BLENDCMD_SRCIN, g_hdl) != VGU_SUCCESS) {
        DISP_LOGE("[%d] BlendImgToSurface failed\n", __LINE__);
        return VGU_OPERATION_FAILED;
    }
    return VGU_SUCCESS;
}

uintptr_t VGUPathInit(VGUPathDataType type, bool enAlias)
{
    if (g_cacheStatus == 0) {
        CreateHandleIfNull();
    }
    if (enAlias) {
        g_renderQuality = EXT_VAU_RENDER_QUALITY_BETTER;
    } else {
        g_renderQuality = EXT_VAU_RENDER_QUALITY_NONE_AA;
    }

    return g_hdl;
}

VGUResult VGUPathAppendPath(uintptr_t path, uintptr_t subPath)
{
    return VGU_NO_SUPPORT;
}

VGUResult VGUPathClear(uintptr_t path)
{
    return VGU_NO_SUPPORT;
}

bool IsCacheRenderSupport(void)
{
    return uapi_vau_is_off_list_support();
}

uint32_t InitRenderCache(int32_t renderNum)
{
    if (renderNum <= 0) {
        DISP_LOGE("[%d] invalid renderNum:%d\n", __LINE__, renderNum);
        return 0;
    }

    RenderNodeCache *renderCache = (RenderNodeCache *)DispMalloc(sizeof(RenderNodeCache));
    if (renderCache == NULL) {
        DISP_LOGE("[%d] DispMalloc for RenderNodeCache failed\n", __LINE__);
        return 0;
    }
    memset_s(renderCache, sizeof(RenderNodeCache), 0, sizeof(RenderNodeCache));

    renderCache->nodeSize = uapi_vau_get_node_size();
    if (renderCache->nodeSize <= 0) {
        DISP_LOGE("[%d] uapi_vau_get_node_size failed\n", __LINE__);
        DispFree(renderCache);
        return 0;
    }

    renderCache->nodeCaches = (uint8_t *)DispMalloc(renderCache->nodeSize * renderNum);
    if (renderCache->nodeCaches == NULL) {
        DISP_LOGE("[%d] DispMalloc %d bytes for cache path failed\n", __LINE__, renderCache->nodeSize * renderNum);
        DispFree(renderCache);
        return 0;
    }

    renderCache->renderNum = (uint32_t)renderNum;
    return (uintptr_t)(void *)renderCache;
}

void DeinitRenderCache(uint32_t cacheKey)
{
    if (!DispCheckAddr((void *)(uintptr_t)cacheKey)) {
        DISP_LOGE("[%d] invalid addr \n", __LINE__);
        return;
    }
    RenderNodeCache *renderCache = (RenderNodeCache *)(void *)(uintptr_t)cacheKey;
    if (renderCache->nodeCaches) {
        for (uint32_t i = 0; i < renderCache->totalCachedNum; i++) {
            uint8_t *node = renderCache->nodeCaches + i * renderCache->nodeSize;
            uapi_vau_unset_node(node);
        }
        DispFree(renderCache->nodeCaches);
    }
    DispFree(renderCache);
}

static next_subnode_mask ShareTypeToSubNodeMask(VGURenderGroupShareType type)
{
    switch (type) {
        case VGU_RENDER_GROUP_SHARE_ALTER:
            return LOAD_REG_MODE_BASE;
            break;
        case VGU_RENDER_GROUP_SHARE_GRADIENT:
            return LOAD_REG_MODE_ALTER;
            break;
        case VGU_RENDER_GROUP_SHARE_BASE:
            return LOAD_REG_MODE_GRADIENT;
            break;
        default:
            return LOAD_REG_MODE_FULL;
    }
}

VGUResult CacheRenderGroupBegin(uint32_t cacheKey,
    VGURenderGroupShareType preShareType, VGURenderGroupShareType subShareType)
{
    if (!DispCheckAddr((void *)(uintptr_t)cacheKey)) {
        DISP_LOGE("[%d] invalid addr\n", __LINE__);
        return VGU_INVALID_PARAMETER;
    }
    RenderNodeCache *renderCache = (RenderNodeCache *)(void *)(uintptr_t)cacheKey;
    if (renderCache->totalCachedNum >= renderCache->renderNum) {
        DISP_LOGE("render cache is full.\n");
        return VGU_OUT_OF_MEMORY;
    }
    if (renderCache->totalCachedNum > 0) {
        uint8_t *curNode = renderCache->nodeCaches + (renderCache->totalCachedNum - 1) * renderCache->nodeSize;
        VGUResult result = (VGUResult)(uapi_vau_set_node_nxt_subnode_mask(curNode,
            ShareTypeToSubNodeMask(preShareType)));
        if (result != VGU_SUCCESS) {
            DISP_LOGE("uapi_vau_node_set_nxt_subnode_mask failure, ret = 0x%x!\n", result);
            return VGU_OPERATION_FAILED;
        }
    }
    renderCache->tmpCachedNum = 0;
    renderCache->subShareType = subShareType;
    g_cacheStatus = 1;

    return VGU_SUCCESS;
}

uint16_t CacheRenderGroupEnd(uint32_t cacheKey)
{
    if (!DispCheckAddr((void *)(uintptr_t)cacheKey)) {
        DISP_LOGE("[%d] invalid addr\n", __LINE__);
        return 0;
    }
    RenderNodeCache *renderCache = (RenderNodeCache *)(void *)(uintptr_t)cacheKey;
    g_cacheStatus = 0;
    return renderCache->tmpCachedNum;
}

static VGUResult PathCacheSetFullNode(RenderNodeCache *renderCache, uint8_t *node, VGUSurface *target,
    const VGUMatrix3 *matrix, const VGUFillAttr *fillAttr, const VGUStrokeAttr *strokeAttr)
{
    ext_vau_surface_list surfaceList = {0};
    SurfaceListParams params = {0};
    VGUResult result = InitPathSurfaceList(&surfaceList, &params, target, matrix, fillAttr, strokeAttr);
    if (result != VGU_SUCCESS) {
        DISP_LOGE("InitPathSurfaceList failure, ret = 0x%x!\n", result);
        return result;
    }

    result = (VGUResult)(uapi_vau_set_full_node(node, &surfaceList, renderCache->renderNum));
    if (result != VGU_SUCCESS) {
        DISP_LOGE("uapi_vau_set_full_node failure, ret = 0x%x!\n", result);
    }

    return result;
}

static VGUResult PathCacheAppendNode(RenderNodeCache *renderCache, uint8_t *curNode, VGUSurface *target,
    const VGUMatrix3 *matrix, const VGUFillAttr *fillAttr, const VGUStrokeAttr *strokeAttr)
{
    ext_vau_draw_attr drawAttr = {0};
    DrawAttrParams drawParams = {0};
    ext_vau_matrix dstMatrix, *dstMixPtr = NULL;
    VGUResult result = VGU_OPERATION_FAILED;
    if (fillAttr) {
        result = InitPathDrawAttr(&drawAttr, &drawParams, fillAttr, NULL);
        if (result != VGU_SUCCESS) {
            DISP_LOGE("InitPathDrawAttr failure, ret = 0x%x!\n", result);
            return result;
        }
    } else {
        result = InitPathDrawAttr(&drawAttr, &drawParams, NULL, strokeAttr);
        if (result != VGU_SUCCESS) {
            DISP_LOGE("InitPathDrawAttr failure, ret = 0x%x!\n", result);
            return result;
        }
    }

    if (renderCache->subShareType <= VGU_RENDER_GROUP_SHARE_GRADIENT) {
        InitMatrix(&dstMixPtr, &dstMatrix, matrix);
    }
    uint8_t *preNode = curNode - renderCache->nodeSize;
    result = (VGUResult)(uapi_vau_append_node_with_path(curNode, preNode, &drawAttr, dstMixPtr));
    if (result != VGU_SUCCESS) {
        DISP_LOGE("uapi_vau_append_node_with_path failure, ret = 0x%x!\n", result);
        return result;
    }

    result = (VGUResult)(uapi_vau_set_node_nxt_subnode_mask(preNode,
        ShareTypeToSubNodeMask(renderCache->subShareType)));
    if (result != VGU_SUCCESS) {
        DISP_LOGE("uapi_vau_node_set_nxt_subnode_mask failure, ret = 0x%x!\n", result);
        return result;
    }

    return result;
}

static VGUResult CacheOnePathInternal(RenderNodeCache *renderCache, VGUSurface *target, const VGUMatrix3 *matrix,
    const VGUFillAttr *fillAttr, const VGUStrokeAttr *strokeAttr)
{
    if (renderCache->totalCachedNum >= renderCache->renderNum) {
        DISP_LOGE("render cache is full.\n");
        return VGU_OUT_OF_MEMORY;
    }

    if (fillAttr && strokeAttr) {
        DISP_LOGE("Adding both stroke and fill nodes together is not supported.\n");
        return VGU_NO_SUPPORT;
    }
    VGUResult result = VGU_OPERATION_FAILED;
    uint8_t *curNode = renderCache->nodeCaches + renderCache->totalCachedNum * renderCache->nodeSize;
    if (renderCache->tmpCachedNum == 0) {
        result = PathCacheSetFullNode(renderCache, curNode, target, matrix, fillAttr, strokeAttr);
        if (result != VGU_SUCCESS) {
            DISP_LOGE("PathCacheSetFullNode failure, ret = 0x%x.\n", result);
            return result;
        }
    } else {
        result = PathCacheAppendNode(renderCache, curNode, target, matrix, fillAttr, strokeAttr);
        if (result != VGU_SUCCESS) {
            DISP_LOGE("PathCacheAppendNode failure, ret = 0x%x.\n", result);
            return result;
        }
    }
    result = (VGUResult)(uapi_vau_set_node_id(curNode, renderCache->totalCachedNum));
    if (result != VGU_SUCCESS) {
        DISP_LOGE("uapi_vau_set_node_id failure, ret = 0x%x.\n", result);
        return result;
    }
    renderCache->tmpCachedNum++;
    renderCache->totalCachedNum++;
    return VGU_SUCCESS;
}

VGUResult CacheOnePath(uint32_t cacheKey, VGUSurface *target, const VGUMatrix3 *matrix,
    const VGUFillAttr *fillAttr, const VGUStrokeAttr *strokeAttr)
{
    CHECK_NULLPOINTER_RETURN_VALUE(target, VGU_INVALID_PARAMETER);
    if (!DispCheckAddr((void *)(uintptr_t)cacheKey)) {
        DISP_LOGE("[%d] invalid addr\n", __LINE__);
        return VGU_INVALID_PARAMETER;
    }
    RenderNodeCache *renderCache = (RenderNodeCache *)(void *)(uintptr_t)cacheKey;
    if (g_cacheStatus != 1) {
        DISP_LOGE("cache render not begin!\n");
        return VGU_OPERATION_FAILED;
    }

    if ((fillAttr == NULL) && (strokeAttr == NULL)) {
        DISP_LOGE("One of fill and stroke in path attr should be set!\n");
        return VGU_INVALID_PARAMETER;
    }
    VGUResult result = VGU_OPERATION_FAILED;
    if (fillAttr) {
        result = CacheOnePathInternal(renderCache, target, matrix, fillAttr, NULL);
        if (result != VGU_SUCCESS) {
            DISP_LOGE("CacheOnePathInternal by fillAttr failure, ret = 0x%x!\n", result);
            goto SAFE_EXIT;
        }
    }
    if (strokeAttr) {
        result = CacheOnePathInternal(renderCache, target, matrix, NULL, strokeAttr);
        if (result != VGU_SUCCESS) {
            DISP_LOGE("CacheOnePathInternal by strokeAttr failure, ret = 0x%x!\n", result);
            goto SAFE_EXIT;
        }
    }
    renderCache->cacheAdded = true;

SAFE_EXIT:
    memset_s(&g_path, sizeof(ext_vau_path), 0, sizeof(ext_vau_path));
    g_renderQuality = 0;
    return result;
}

VGUResult SetGroupRenderBufferAddr(uint32_t cacheKey, uint32_t nodeId, uint64_t memHandle)
{
    if (!DispCheckAddr((void *)(uintptr_t)cacheKey)) {
        DISP_LOGE("[%d] invalid addr\n", __LINE__);
        return VGU_INVALID_PARAMETER;
    }
    RenderNodeCache *renderCache = (RenderNodeCache *)(void *)(uintptr_t)cacheKey;
    if (nodeId >= renderCache->renderNum) {
        DISP_LOGE("Invalid node id %u.\n", nodeId);
        return VGU_INVALID_PARAMETER;
    }
    uint8_t *node = renderCache->nodeCaches + nodeId * renderCache->nodeSize;
    VGUResult result = (VGUResult)(uapi_vau_set_node_buffer_addr(node, (td_u32)(memHandle)));
    if (result != VGU_SUCCESS) {
        DISP_LOGE("uapi_vau_set_node_buffer failure, ret = 0x%x!\n", result);
    }
    return result;
}

VGUResult SetGroupRenderBuffer(uint32_t cacheKey, uint32_t nodeId, VGUBuffer *buffer, VGURect *clipRect)
{
    if (buffer == NULL) {
        return VGU_INVALID_PARAMETER;
    }
    if (!DispCheckAddr((void *)(uintptr_t)cacheKey)) {
        DISP_LOGE("[%d] invalid addr\n", __LINE__);
        return VGU_INVALID_PARAMETER;
    }
    RenderNodeCache *renderCache = (RenderNodeCache *)(void *)(uintptr_t)cacheKey;
    if (nodeId >= renderCache->renderNum) {
        DISP_LOGE("Invalid node id %u.\n", nodeId);
        return VGU_INVALID_PARAMETER;
    }
    ext_vau_surface dst = {0};
    if (Convert2VauSurface(buffer, &dst) != VGU_SUCCESS) {
        return VGU_OPERATION_FAILED;
    }
    ext_rect dstRect = {0, 0, dst.width, dst.height};
    if (clipRect != NULL) {
        dstRect.x = (td_s32)clipRect->x;
        dstRect.y = (td_s32)clipRect->y;
        dstRect.width = (td_u32)clipRect->w;
        dstRect.height = (td_u32)clipRect->h;
    }
    dst.rect = dstRect;
    uint8_t *node = renderCache->nodeCaches + nodeId * renderCache->nodeSize;
    VGUResult result = (VGUResult)(uapi_vau_set_node_buffer(node, &dst));
    if (result != VGU_SUCCESS) {
        DISP_LOGE("uapi_vau_set_node_buffer failure, ret = 0x%x!\n", result);
    }
    return result;
}

VGUResult SetGroupRenderRect(uint32_t cacheKey, uint32_t nodeId, VGURect *clipRect)
{
    CHECK_NULLPOINTER_RETURN_VALUE(clipRect, VGU_INVALID_PARAMETER);
    if (!DispCheckAddr((void *)(uintptr_t)cacheKey)) {
        DISP_LOGE("[%d] invalid addr\n", __LINE__);
        return VGU_INVALID_PARAMETER;
    }
    RenderNodeCache *renderCache = (RenderNodeCache *)(void *)(uintptr_t)cacheKey;
    if (nodeId >= renderCache->renderNum) {
        DISP_LOGE("Invalid node id %u.\n", nodeId);
        return VGU_INVALID_PARAMETER;
    }
    ext_rect rect;
    rect.x = clipRect->x;
    rect.y = clipRect->y;
    rect.width = clipRect->w;
    rect.height = clipRect->h;
    uint8_t *node = renderCache->nodeCaches + nodeId * renderCache->nodeSize;
    VGUResult result = (VGUResult)(uapi_vau_set_node_rect(node, &rect));
    if (result != VGU_SUCCESS) {
        DISP_LOGE("uapi_vau_set_node_rect failure, ret = 0x%x!\n", result);
    }
    return result;
}

VGUResult SetGroupRenderMatrix(uint32_t cacheKey, uint32_t nodeId, const VGUMatrix3 *matrix)
{
    CHECK_NULLPOINTER_RETURN_VALUE(matrix, VGU_INVALID_PARAMETER);
    if (!DispCheckAddr((void *)(uintptr_t)cacheKey)) {
        DISP_LOGE("[%d] invalid addr\n", __LINE__);
        return VGU_INVALID_PARAMETER;
    }
    RenderNodeCache *renderCache = (RenderNodeCache *)(void *)(uintptr_t)cacheKey;
    if (nodeId >= renderCache->renderNum) {
        DISP_LOGE("Invalid node id %u.\n", nodeId);
        return VGU_INVALID_PARAMETER;
    }
    ext_vau_matrix vauMatrix  = {{{matrix->m[0][0], matrix->m[0][1], matrix->m[0][2]},
        {matrix->m[1][0], matrix->m[1][1], matrix->m[1][2]},
        {matrix->m[2][0], matrix->m[2][1], matrix->m[2][2]}}};
    uint8_t *node = renderCache->nodeCaches + nodeId * renderCache->nodeSize;
    VGUResult result = (VGUResult)(uapi_vau_set_node_matrix(node, &vauMatrix));
    if (result != VGU_SUCCESS) {
        DISP_LOGE("uapi_vau_set_node_matrix failure, ret = 0x%x!\n", result);
        return result;
    }
    result = (VGUResult)(uapi_vau_set_node_stroke_rnd(node, &vauMatrix));
    if (result != VGU_SUCCESS) {
        DISP_LOGE("uapi_vau_set_node_stroke_rnd failure, ret = 0x%x!\n", result);
    }
    return result;
}

VGUResult TranslateRenderNode(uint32_t cacheKey, uint32_t nodeId, int16_t offsetX, int16_t offsetY)
{
    if (!DispCheckAddr((void *)(uintptr_t)cacheKey)) {
        DISP_LOGE("[%d] invalid addr\n", __LINE__);
        return VGU_INVALID_PARAMETER;
    }
    RenderNodeCache *renderCache = (RenderNodeCache *)(void *)(uintptr_t)cacheKey;
    if (nodeId >= renderCache->renderNum) {
        DISP_LOGE("Invalid node id %u.\n", nodeId);
        return VGU_INVALID_PARAMETER;
    }
    uint8_t *node = renderCache->nodeCaches + nodeId * renderCache->nodeSize;
    VGUResult result = (VGUResult)(uapi_vau_set_node_translate(node, offsetX, offsetY));
    if (result != VGU_SUCCESS) {
        DISP_LOGE("uapi_vau_set_node_translate failure, ret = 0x%x!\n", result);
    }
    return result;
}

VGUResult SetRenderNodeOpa(uint32_t cacheKey, uint32_t nodeId, uint8_t opa)
{
    if (!DispCheckAddr((void *)(uintptr_t)cacheKey)) {
        DISP_LOGE("[%d] invalid addr\n", __LINE__);
        return VGU_INVALID_PARAMETER;
    }
    RenderNodeCache *renderCache = (RenderNodeCache *)(void *)(uintptr_t)cacheKey;
    if (nodeId >= renderCache->renderNum) {
        DISP_LOGE("Invalid node id %u.\n", nodeId);
        return VGU_INVALID_PARAMETER;
    }
    uint8_t *node = renderCache->nodeCaches + nodeId * renderCache->nodeSize;
    VGUResult result = (VGUResult)(uapi_vau_set_node_opa(node, opa));
    if (result != VGU_SUCCESS) {
        DISP_LOGE("uapi_vau_set_node_opa failure, ret = 0x%x!\n", result);
    }
    return result;
}

VGUResult DoRenderByCache(uint32_t cacheKey)
{
    if (!DispCheckAddr((void *)(uintptr_t)cacheKey)) {
        DISP_LOGE("[%d] invalid addr\n", __LINE__);
        return VGU_INVALID_PARAMETER;
    }
    RenderNodeCache *renderCache = (RenderNodeCache *)(void *)(uintptr_t)cacheKey;
    if (renderCache->renderNum > renderCache->totalCachedNum && renderCache->cacheAdded) {
        DISP_LOGW("renderNum %u greater than totalCachedNum %u!\n",
            renderCache->renderNum, renderCache->totalCachedNum);
        for (uint32_t i = 0; i < renderCache->totalCachedNum; i++) {
            uint8_t *node = renderCache->nodeCaches + i * renderCache->nodeSize;
            uapi_vau_set_node_subnode_num(node, renderCache->totalCachedNum);
        }
        renderCache->cacheAdded = false;
    }

    VGUResult result = (VGUResult)(uapi_vau_render_offline_nodes(renderCache->nodeCaches, true));
    if (result != VGU_SUCCESS) {
        DISP_LOGE("uapi_vau_render_offline_nodes failure, ret = 0x%x!\n", result);
    }
    return result;
}