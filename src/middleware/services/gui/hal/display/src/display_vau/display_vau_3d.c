/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: display vau hal layer
 * Author: Hisi Graphic Team
 */

#include "display_vau_internal.h"

static void Init3dAttr(ext_vau_3d_attr* threed_attr)
{
    threed_attr->enable        = TD_TRUE;
    threed_attr->bg_color      = 0XFF000000;
    threed_attr->shadow_color  = 0XFF000000;
    threed_attr->mapping_color = EXT_SAMPLING_BILINEAR;
    threed_attr->mapping_alpha = EXT_SAMPLING_BILINEAR;
    threed_attr->model_pos     = EXT_SAMPLING_BILINEAR;
    threed_attr->model_flag    = EXT_SAMPLING_BILINEAR;
}

static void ConvertModelSurface(ext_vau_model_surface* modelSurface, VGUModelSurface *model)
{
    modelSurface->width = model->width;
    modelSurface->height = model->height;
    modelSurface->stride = model->stride;
    modelSurface->phy_addr = model->memHandle;
    modelSurface->vir_addr = (td_uchar *)(uintptr_t)model->memHandle;
}

VGUResult Render3DMesh(VGUSurface *target, VGUImage *texture,
    VGUModelSurface *model, VGUMeshAttr *mesh)
{
    CHECK_NULLPOINTER_RETURN_VALUE(target, VGU_INVALID_PARAMETER);
    CHECK_NULLPOINTER_RETURN_VALUE(texture, VGU_INVALID_PARAMETER);
    CHECK_NULLPOINTER_RETURN_VALUE(model, VGU_INVALID_PARAMETER);
    CHECK_NULLPOINTER_RETURN_VALUE(mesh, VGU_INVALID_PARAMETER);

    CreateHandleIfNull();

    ext_vau_surface_list surfaceList = {0};

    ext_vau_surface dst = {0};
    surfaceList.dst_surface = &dst;
    if (Convert2VauSurface(&target->buffer, &dst) != VGU_SUCCESS) {
        DISP_LOGE("Render Mesh failed! Convert2VauSurface failed!\n");
        return VGU_OPERATION_FAILED;
    }

    ext_vau_surface src = {0};
    if (ConvertVGUImg2VauSurface(texture, &src) != VGU_SUCCESS) {
        DISP_LOGE("Render Mesh failed! ConvertVGUImg2VauSurface failed!\n");
        return VGU_OPERATION_FAILED;
    }

    ext_rect dstRect = {texture->dstRect.x, texture->dstRect.y, texture->dstRect.w, texture->dstRect.h};
    dst.rect = dstRect;

    surfaceList.src_surface = &src;
    surfaceList.src_surface_cnt = 1;
    surfaceList.opt = NULL;
    src.mapping_attr.mapping_en = TD_TRUE;
    src.mapping_attr.mapping_mode = EXT_MAPPING_OPT_MESH;

    // pattern configuration
    ext_vau_pattern_attr pattern = {0};
    src.pattern = &pattern;
    src.pattern->mode = EXT_VAU_PATTERN_PAD;

    // init 3d attributes
    ext_vau_3d_attr threed_attr = {0};
    src.threed_attr = &threed_attr;
    Init3dAttr(&threed_attr);

    threed_attr.mesh_attr.animation_trigger = mesh->animValue;
    threed_attr.mesh_attr.type  = mesh->type;
    threed_attr.mesh_attr.x_pos = mesh->x;
    threed_attr.mesh_attr.y_pos = mesh->y;

    // model surface
    ConvertModelSurface(&threed_attr.model_sur, model);

    int32_t ret = uapi_vau_render(g_hdl, &surfaceList);
    if (ret != VGU_SUCCESS) {
        DISP_LOGE("uapi_vau_render failure, ret = 0x%x!\n", ret);
        uapi_vau_cancel(g_hdl);
        g_hdl = INVALID_HDL;
        return VGU_OPERATION_FAILED;
    }
    return VGU_SUCCESS;
}

static void ConvertCameraAttr(const VGUCameraAttr* vguCam, ext_vau_camera_attr* vauCam)
{
    vauCam->angle  = vguCam->angle;
    vauCam->v_fov = vguCam->vFov;
    vauCam->aspect_ratio = vguCam->aspectRatio;
    vauCam->z_near = vguCam->nearPlane;
    vauCam->z_far = vguCam->farPlane;
    vauCam->axis.vector[0] = vguCam->axis.x;
    vauCam->axis.vector[1] = vguCam->axis.y;
    vauCam->axis.vector[2] = vguCam->axis.z; // 2: index
    vauCam->scale.vector[0] = vguCam->scale.x;
    vauCam->scale.vector[1] = vguCam->scale.y;
    vauCam->scale.vector[2] = vguCam->scale.z; // 2: index
    vauCam->translation.vector[0] = vguCam->translation.x;
    vauCam->translation.vector[1] = vguCam->translation.y;
    vauCam->translation.vector[2] = vguCam->translation.z; // 2: index
    vauCam->look_from.vector[0] = vguCam->cameraPos.x;
    vauCam->look_from.vector[1] = vguCam->cameraPos.y;
    vauCam->look_from.vector[2] = vguCam->cameraPos.z; // 2: index
}

VGUResult Render3DCylinder(VGUSurface *target, VGUImage *texture,
    VGUModelSurface *model, VGUCameraAttr* cam)
{
    CHECK_NULLPOINTER_RETURN_VALUE(target, VGU_INVALID_PARAMETER);
    CHECK_NULLPOINTER_RETURN_VALUE(texture, VGU_INVALID_PARAMETER);
    CHECK_NULLPOINTER_RETURN_VALUE(model, VGU_INVALID_PARAMETER);
    CHECK_NULLPOINTER_RETURN_VALUE(cam, VGU_INVALID_PARAMETER);

    CreateHandleIfNull();

    ext_vau_surface_list surfaceList = {0};

    ext_vau_surface dst = {0};
    surfaceList.dst_surface = &dst;
    if (Convert2VauSurface(&target->buffer, &dst) != VGU_SUCCESS) {
        DISP_LOGE("Render Plane failed! Convert2VauSurface failed!\n");
        return VGU_OPERATION_FAILED;
    }
    ext_rect dstRect = {texture->dstRect.x, texture->dstRect.y, texture->dstRect.w, texture->dstRect.h};
    dst.rect = dstRect;

    ext_vau_surface src = {0};
    if (ConvertVGUImg2VauSurface(texture, &src) != VGU_SUCCESS) {
        DISP_LOGE("Render Plane failed! ConvertVGUImg2VauSurface failed!\n");
        return VGU_OPERATION_FAILED;
    }

    surfaceList.src_surface = &src;
    surfaceList.src_surface_cnt = 1;
    surfaceList.opt = NULL;
    src.mapping_attr.mapping_en = TD_TRUE;
    src.mapping_attr.mapping_mode = EXT_MAPPING_OPT_PLANE;

    ext_vau_pattern_attr pattern = {0};
    src.pattern = &pattern;
    src.pattern->mode = EXT_VAU_PATTERN_FILL;

    // init 3d attributes
    ext_vau_3d_attr threed_attr = {0};
    src.threed_attr = &threed_attr;
    Init3dAttr(&threed_attr);

    ConvertCameraAttr(cam, &threed_attr.camera_attr);
    ConvertModelSurface(&threed_attr.model_sur, model);

    int32_t ret = uapi_vau_render(g_hdl, &surfaceList);
    if (ret != VGU_SUCCESS) {
        DISP_LOGE("uapi_vau_render failure, ret = 0x%x!\n", ret);
        uapi_vau_cancel(g_hdl);
        g_hdl = INVALID_HDL;
        return VGU_OPERATION_FAILED;
    }
    return VGU_SUCCESS;
}

VGUResult Render3DSphere(VGUSurface *target, VGUImage *texture, VGUModelSurface *model,
    VGUSphereAttr *sphere, const VGUImage *mask)
{
    CHECK_NULLPOINTER_RETURN_VALUE(target, VGU_INVALID_PARAMETER);
    CHECK_NULLPOINTER_RETURN_VALUE(texture, VGU_INVALID_PARAMETER);
    CHECK_NULLPOINTER_RETURN_VALUE(model, VGU_INVALID_PARAMETER);
    CHECK_NULLPOINTER_RETURN_VALUE(sphere, VGU_INVALID_PARAMETER);
    CHECK_NULLPOINTER_RETURN_VALUE(mask, VGU_INVALID_PARAMETER);

    CreateHandleIfNull();

    ext_vau_surface_list surfaceList = {0};

    ext_vau_surface dst = {0};
    surfaceList.dst_surface = &dst;
    if (Convert2VauSurface(&target->buffer, &dst) != VGU_SUCCESS) {
        DISP_LOGE("Convert2VauSurface failed!\n");
        return VGU_OPERATION_FAILED;
    }
    ext_rect dstRect = {texture->dstRect.x, texture->dstRect.y, texture->dstRect.w, texture->dstRect.h};
    dst.rect = dstRect;

    ext_vau_surface src[2] = {0};

    if (ConvertVGUImg2VauSurface(mask, &src[0]) != VGU_SUCCESS) {
        DISP_LOGE("ConvertVGUImg2VauSurface failed\n");
        return VGU_OPERATION_FAILED;
    }
    if (ConvertVGUImg2VauSurface(texture, &src[1]) != VGU_SUCCESS) {
        DISP_LOGE("ConvertVGUImg2VauSurface failed!\n");
        return VGU_OPERATION_FAILED;
    }

    ext_vau_matrix mat = {{0}};
    uapi_vau_get_sphere_matrix(&mat, (float)model->radius, sphere->xAngle, sphere->yAngle, sphere->zAngle);
    src[1].matrix = &mat;

    surfaceList.src_surface = &src;
    surfaceList.src_surface_cnt = 2; // 2: surface cnt

    ext_vau_blit_opt blitOpt = {0};
    blitOpt.blend_opt.blend_enable = TD_TRUE;
    blitOpt.blend_opt.blend_cmd = EXT_VAU_BLENDCMD_SRCATOP;
    blitOpt.blend_opt.out_alpha_mode = EXT_VAU_OUTALPHA_FROM_NORM;
    surfaceList.opt = &blitOpt;

    src[1].mapping_attr.mapping_en = TD_TRUE;
    src[1].mapping_attr.mapping_mode = EXT_MAPPING_OPT_SPHERE;

    // init 3d attributes
    ext_vau_3d_attr threed_attr = {0};
    src[1].threed_attr = &threed_attr;
    Init3dAttr(&threed_attr);

    ConvertModelSurface(&threed_attr.model_sur, model);

    int32_t ret = uapi_vau_render(g_hdl, &surfaceList);
    if (ret != VGU_SUCCESS) {
        DISP_LOGE("uapi_vau_render failed! ret = 0x%x!\n", ret);
        uapi_vau_cancel(g_hdl);
        g_hdl = INVALID_HDL;
        return VGU_OPERATION_FAILED;
    }
    return VGU_SUCCESS;
}