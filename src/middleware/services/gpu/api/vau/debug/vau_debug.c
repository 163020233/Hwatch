/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description : vau debug file
 */

#include "vau_debug.h"
#include "vau_dfx.h"


STATIC td_void vau_debug_print_surface_opt(const ext_vau_blit_opt *opt)
{
    if (opt == TD_NULL) {
        return;
    }
    gpu_print("rop_opt:enable[%u] alpha_mode[%u] color_mode[%u]\n",
              opt->rop_opt.rop_enable, opt->rop_opt.alpha_rop_mode, opt->rop_opt.color_rop_mode);

    gpu_print("colorkey_opt:enable[%u] mode[%u]\n",
              opt->colorkey_opt.colorkey_enable, opt->colorkey_opt.colorkey_mode);

    gpu_print("blend_opt:enable[%u] cmd[%u] back_mode[%u] fore_mode[%u] out_alpha_mode[%u]\n",
              opt->blend_opt.blend_enable, opt->blend_opt.blend_cmd, opt->blend_opt.background_blend_mode,
              opt->blend_opt.foreground_blend_mode, opt->blend_opt.out_alpha_mode);

    gpu_print("blur_opt:enable[%u] alpha_en[%u] sigmaX/Y[%f / %f] area[%d] blur_quality[%d]\n",
              opt->blur_opt.blur_en, opt->blur_opt.alpha_en, opt->blur_opt.sigma_x,
              opt->blur_opt.sigma_y, opt->blur_opt.area, opt->blur_opt.blur_quality);
}

STATIC td_void vau_debug_print_path(const ext_vau_path *path)
{
    if (path == TD_NULL) {
        return;
    }

    if (path->cmd_num != 0) {
        gpu_print("cmd[%u] = { ", path->cmd_num);
        for (td_u32 i = 0; i < path->cmd_num; i++) {
            gpu_print("%u, ", (td_u32)path->cmds[i]);
        }
        gpu_print("};\n");
    }

    if (path->data_num != 0) {
        gpu_print("data[%u] = { ", path->data_num);
        for (td_u32 i = 0; i < path->data_num; i++) {
            gpu_print("%.4f, ", path->datas[i]);
        }
        gpu_print("};\n");
    }
}

STATIC td_void vau_debug_print_paint_attr(const ext_vau_paint_attr *paint_attr)
{
    gpu_print("paint_type:%u\n", paint_attr->paint_type);
    if (paint_attr->paint_type <= EXT_VAU_PAINT_TYPE_COLOR_ALPHA_FF) {
        gpu_print("color: %#x\n", paint_attr->solid.color);
        return;
    }
    if (paint_attr->paint_type <= EXT_VAU_PAINT_TYPE_SWEEP_GRADIENT &&
        paint_attr->paint_type >= EXT_VAU_PAINT_TYPE_LINEAR_GRADIENT) {
        gpu_print("gradient_tile_mode: %u, stops:colors_offsets:\n",
                  paint_attr->gradient.tile_mode);
        for (td_u32 i = 0; i < paint_attr->gradient.stops.num; i++) {
            gpu_print("[%#x, %.4f]", paint_attr->gradient.stops.colors[i],
                      paint_attr->gradient.stops.offsets[i]);
        }
        if (paint_attr->paint_type == EXT_VAU_PAINT_TYPE_LINEAR_GRADIENT) {
            gpu_print("\nLINEAR: startXY:[%d, %d], endXY:[%d, %d]\n", paint_attr->gradient.linear.start_point.x,
                      paint_attr->gradient.linear.start_point.y,
                      paint_attr->gradient.linear.end_point.x, paint_attr->gradient.linear.end_point.y);
        } else if (paint_attr->paint_type == EXT_VAU_PAINT_TYPE_RADIAL_GRADIENT) {
            gpu_print("\nRADIAL: radius:%u, center[%d, %d]\n", paint_attr->gradient.radial.radius,
                      paint_attr->gradient.radial.center.x, paint_attr->gradient.radial.center.y);
        } else {
            gpu_print("\nSWEEP: start_angle:%u, center[%d, %d]\n", paint_attr->gradient.sweep.start_angle,
                      paint_attr->gradient.sweep.center.x, paint_attr->gradient.sweep.center.y);
        }
    }
}

STATIC td_void vau_debug_print_paint(const ext_vau_paint *paint)
{
    if (paint == TD_NULL) {
        return;
    }

    gpu_print("render_quality[%u] paint_mode[%u]\n", paint->render_cfg.render_quality, paint->paint_mode);
    if (paint->fill_attr != TD_NULL) {
        gpu_print("fill:-- type: %u, ", paint->fill_attr->type);
        vau_debug_print_paint_attr(&paint->fill_attr->paint_attr);
    }

    if (paint->stroke_attr != TD_NULL) {
        gpu_print("stroke:--width[%.4f] limit[%.4f] cap[%u] join[%u]\n",
                  paint->stroke_attr->width,
                  paint->stroke_attr->miter_limit,
                  paint->stroke_attr->cap, paint->stroke_attr->join);

        vau_debug_print_paint_attr(&paint->stroke_attr->paint_attr);
        if (paint->stroke_attr->dash.enable == TD_TRUE) {
            gpu_print("dash:count-offset-reset[%d, %.4f, %d], array: \n",
                      paint->stroke_attr->dash.count, paint->stroke_attr->dash.offset, paint->stroke_attr->dash.reset);
            for (td_u32 i = 0; i < paint->stroke_attr->dash.count; i++) {
                gpu_print("%.4f, ", paint->stroke_attr->dash.array[i]);
            }
            gpu_print("\n");
        }
    }
}

STATIC td_void vau_debug_print_hdr(const ext_hdr_attr *hdr_attr)
{
    gpu_print("image_cnt[%u], is_base[%d], is_nonstand[%d]\n",
              hdr_attr->image_cnt, hdr_attr->is_base_image, hdr_attr->is_nonstandard);
    gpu_print("cicp_info[%d, %d, %d]\n", hdr_attr->cicp_info.color_primaries,
              hdr_attr->cicp_info.transfer_characteristics, hdr_attr->cicp_info.matrix_coefficients);
}

STATIC td_void vau_debug_print_3d(const ext_vau_3d_attr *threed_attr)
{
    gpu_print("enable[%d] bg_color[%#x] shadow_color[%#x]\n", threed_attr->enable,
              threed_attr->bg_color, threed_attr->shadow_color);
    gpu_print("mapping_color[%u] mapping_alpha[%u] model_pos[%u] model_flag[%u]\n",
              threed_attr->mapping_color, threed_attr->mapping_alpha,
              threed_attr->model_pos, threed_attr->model_flag);
    gpu_print("camera_attr:angle[%f] v_fov[%f] aspect_ratio[%f] z_near[%f] z_far[%f]\n",
              threed_attr->camera_attr.angle, threed_attr->camera_attr.v_fov,
              threed_attr->camera_attr.aspect_ratio, threed_attr->camera_attr.z_near,
              threed_attr->camera_attr.z_far);
    gpu_print("camera_axis[%.4f, %.4f, %.4f] camera_scale[%.4f, %.4f, %.4f]\n",
              threed_attr->camera_attr.axis.vector[0], threed_attr->camera_attr.axis.vector[1],
              threed_attr->camera_attr.axis.vector[2], threed_attr->camera_attr.scale.vector[0], // 2 index
              threed_attr->camera_attr.scale.vector[1], threed_attr->camera_attr.scale.vector[2]); // 2 index
    gpu_print("camera_trans[%.4f, %.4f, %.4f] camera_look_from[%.4f, %.4f, %.4f]\n",
              threed_attr->camera_attr.translation.vector[0], threed_attr->camera_attr.translation.vector[1],
              threed_attr->camera_attr.translation.vector[2], threed_attr->camera_attr.look_from.vector[0], // 2 index
              threed_attr->camera_attr.look_from.vector[1], threed_attr->camera_attr.look_from.vector[2]); // 2 index
    gpu_print("model_sur:whs[%u %u %u] phy[%#x]\n",
              threed_attr->model_sur.width, threed_attr->model_sur.height,
              threed_attr->model_sur.stride, threed_attr->model_sur.phy_addr);
    gpu_print("mesh_attr:type[%d] xy_pos[%u %u] animation[%u]\n",
              threed_attr->mesh_attr.type, threed_attr->mesh_attr.x_pos,
              threed_attr->mesh_attr.y_pos, threed_attr->mesh_attr.animation_trigger);
}

STATIC td_void vau_debug_print_sdf(const ext_vau_sdf_attr *sdf_attr)
{
    gpu_print("sdf_attr:enable[%d] is_first_frame[%d] phy_addr[%#x]\n", sdf_attr->enable, sdf_attr->is_first_frame,
        sdf_attr->phy_addr);

    if (sdf_attr->cam_info != TD_NULL) {
        gpu_print("sdf_cam_info:cam_pos[%.4f, %.4f, %.4f] cam_target[%.4f, %.4f, %.4f]\n",
                  sdf_attr->cam_info->cam_pos.vector[0], sdf_attr->cam_info->cam_pos.vector[1],
                  sdf_attr->cam_info->cam_pos.vector[2], // 2 index
                  sdf_attr->cam_info->cam_target.vector[0], sdf_attr->cam_info->cam_target.vector[1],
                  sdf_attr->cam_info->cam_target.vector[2]); // 2 index
    }

    if (sdf_attr->light_info != TD_NULL) {
        gpu_print("sdf_light_info:light_self[%.4f]\n", sdf_attr->light_info->light_self);
        for (td_u32 i = 0; i < 4; i++) { // 4 cnt
            gpu_print("light[%u]:pos[%.4f, %.4f, %.4f] color[%.4f, %.4f, %.4f] power[%.4f]\n",
                      i,
                      sdf_attr->light_info->light_pos[i].vector[0],
                      sdf_attr->light_info->light_pos[i].vector[1],
                      sdf_attr->light_info->light_pos[i].vector[2],  // 2 index
                      sdf_attr->light_info->light_color[i].vector[0],
                      sdf_attr->light_info->light_color[i].vector[1],
                      sdf_attr->light_info->light_color[i].vector[2], // 2 index
                      sdf_attr->light_info->light_power[i]);
        }
    }

    if (sdf_attr->press_info != TD_NULL) {
        gpu_print("sdf_press_info:press_point[%d, %d] press_strength[%.4f]\n",
                  sdf_attr->press_info->press_point.x, sdf_attr->press_info->press_point.y,
                  sdf_attr->press_info->press_strength);
    }

    if (sdf_attr->face_attr != TD_NULL) {
        gpu_print("sdf_face_attr:width[%u] height[%u] valid_face_flag[%u] memoji_mode[%u] scale_3d[%.4f]\n",
                  sdf_attr->face_attr->width, sdf_attr->face_attr->height,
                  sdf_attr->face_attr->valid_face_flag, sdf_attr->face_attr->memoji_mode,
                  sdf_attr->face_attr->scale_3d);
        if ((sdf_attr->face_attr->valid_face_flag == TD_TRUE) && (sdf_attr->face_attr->face_lmk != TD_NULL)) {
            gpu_print("sdf_face_lmk:");
            for (td_u32 i = 0; i < 71; i++) { // 71: face lmk count
                gpu_print("[%.4f, %.4f] ",
                          sdf_attr->face_attr->face_lmk[i * 2], sdf_attr->face_attr->face_lmk[i * 2 + 1]); // 2 num
            }
            gpu_print("\n");
        }
    }
}

STATIC td_void vau_debug_print_surface(const ext_vau_surface *surface)
{
    gpu_print("addr[%#x] wh[%ux%u] stride[%u] fmt[%u] order[%d] is_color[%d] color[%#x] cmp_mode[%u]\n",
              surface->phy_addr, surface->width, surface->height, surface->stride, surface->color_fmt,
              surface->argb_order, surface->is_color_surface, surface->color, surface->compress_mode);

    gpu_print("rect[%d %d %u %u] out_rect[%d %d %u %u]\n",
              surface->rect.x, surface->rect.y, surface->rect.width, surface->rect.height,
              surface->out_rect.x, surface->out_rect.y, surface->out_rect.width, surface->out_rect.height);

    gpu_print("g_alpha[%u] g_premult_en[%d], g_alpha_en[%d] premult_en[%d], pix_alpha_en[%d] uv_offset[%u]\n",
              surface->alpha_info.global_alpha, surface->alpha_info.golbal_premult_en,
              surface->alpha_info.global_alpha_en,
              surface->alpha_info.premult_en, surface->alpha_info.pixel_alpha_en, surface->uv_offset);

    if (surface->matrix != TD_NULL) {
        gpu_print("matrix:\n %.4f %.4f %.4f\n %.4f %.4f %.4f\n %.4f %.4f %.4f\n",
                  (surface->matrix->matrix[0][0]),
                  (surface->matrix->matrix[0][1]),
                  (surface->matrix->matrix[0][2]),  /* 2:index */
                  (surface->matrix->matrix[1][0]),
                  (surface->matrix->matrix[1][1]),
                  (surface->matrix->matrix[1][2]),  /* 2:index */
                  (surface->matrix->matrix[2][0]),  /* 2:index */
                  (surface->matrix->matrix[2][1]),  /* 2:index */
                  (surface->matrix->matrix[2][2])); /* 2:index */
    }

    if (surface->pattern != TD_NULL) {
        gpu_print("pattern:mode[%u], argb[%u]\n", surface->pattern->mode, surface->pattern->argb);
    }

    gpu_print("mapping_attr:mapping_en[%d] mapping_mode[%u]\n",
              surface->mapping_attr.mapping_en, surface->mapping_attr.mapping_mode);

    if (surface->draw != TD_NULL) {
        vau_debug_print_path(surface->draw->path);
        vau_debug_print_paint(surface->draw->paint);
    }

    if (surface->hdr_attr != TD_NULL) {
        vau_debug_print_hdr(surface->hdr_attr);
    }

    if (surface->threed_attr != TD_NULL) {
        vau_debug_print_3d(surface->threed_attr);
    }

    if (surface->sdf_attr != TD_NULL) {
        vau_debug_print_sdf(surface->sdf_attr);
    }
}

STATIC td_void vau_debug_print_drv_surface(const drv_vau_surface *surface)
{
    gpu_print("addr[%#x] wh[%ux%u] stride[%u] fmt[%u] order[%d] is_color[%d] \
               color[%#x] cmp_mode[%u] cmp_tile_width[%u]\n",
              surface->phy_addr, surface->width, surface->height, surface->stride,
              surface->fmt, surface->argb_order, surface->is_color_surface, surface->color,
              surface->compress_info.compress_mode, surface->compress_info.tile_width);

    gpu_print("rect[%d %d %u %u] out_rect[%d %d %u %u]\n",
              surface->rect.x, surface->rect.y, surface->rect.width, surface->rect.height,
              surface->out_rect.x, surface->out_rect.y, surface->out_rect.width, surface->out_rect.height);

    gpu_print("g_alpha[%u] g_premult_en[%d], g_alpha_en[%d] premult_en[%d], pix_alpha_en[%d]\n",
              surface->alpha_info.global_alpha, surface->alpha_info.golbal_premult_en,
              surface->alpha_info.global_alpha_en,
              surface->alpha_info.premult_en, surface->alpha_info.pixel_alpha_en);

    if (surface->attr.affine.affine_en == TD_TRUE) {
        gpu_print("matrix:\n %.4f %.4f %.4f\n %.4f %.4f %.4f\n %.4f %.4f %.4f\n",
                  (surface->attr.affine.matrix[0][0]),
                  (surface->attr.affine.matrix[0][1]),
                  (surface->attr.affine.matrix[0][2]),  /* 2:index */
                  (surface->attr.affine.matrix[1][0]),
                  (surface->attr.affine.matrix[1][1]),
                  (surface->attr.affine.matrix[1][2]),  /* 2:index */
                  (surface->attr.affine.matrix[2][0]),  /* 2:index */
                  (surface->attr.affine.matrix[2][1]),  /* 2:index */
                  (surface->attr.affine.matrix[2][2])); /* 2:index */
    }
}

td_void vau_debug_print_surface_list(const ext_vau_surface_list *surface_list)
{
    if (vau_dfx_check_mask(VAU_DFX_MASK_SURFACE) == 0) {
        return;
    }

    if (surface_list == TD_NULL) {
        return;
    }

    gpu_print("surface_list:src_surface_cnt[%u]\n", surface_list->src_surface_cnt);

    if (surface_list->src_surface != NULL) {
        for (td_u32 i = 0; i < surface_list->src_surface_cnt; i++) {
            gpu_print("src[%u]--------------------\n", i);
            vau_debug_print_surface(&surface_list->src_surface[i]);
        }
    }

    if (surface_list->dst_surface != NULL) {
        gpu_print("dst-----------------------\n");
        vau_debug_print_surface(surface_list->dst_surface);
    }

    if (surface_list->opt != NULL) {
        gpu_print("opt-----------------------\n");
        vau_debug_print_surface_opt(surface_list->opt);
    }
}

td_void vau_debug_print_drv_surface_list(const drv_vau_surface_list *surface_list)
{
    if (vau_dfx_check_mask(VAU_DFX_MASK_SURFACE) == 0) {
        return;
    }

    if (surface_list == TD_NULL) {
        return;
    }

    if (surface_list->src_surface != NULL) {
        for (td_u32 i = 0; i < surface_list->src_surface_cnt; i++) {
            gpu_print("drv src[%u]--------------------\n", i);
            vau_debug_print_drv_surface(&surface_list->src_surface[i]);
        }
    }

    if (surface_list->dst_surface != NULL) {
        gpu_print("drv dst-----------------------\n");
        vau_debug_print_drv_surface(surface_list->dst_surface);
    }
}