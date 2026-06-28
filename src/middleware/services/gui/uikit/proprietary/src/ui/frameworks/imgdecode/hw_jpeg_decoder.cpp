/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <climits>
#include "imgdecode/hw_jpeg_decoder.h"
#if CAPABILITY_HW_JPEG_DECODE
#include "gfx_utils/file.h"
#include "gfx_utils/graphic_log.h"
#include "gfx_utils/mem_api.h"
#include "hals/gralloc_engines.h"
#include "soc_jpeg_api.h"
#include "securec.h"

namespace OHOS {
static uint32_t g_fileLen = 0;
namespace {
class HWJpegDecoderImplement : public HWJpegDecoder {
public:
    HWJpegDecoderImplement() = default;

    ~HWJpegDecoderImplement() override = default;

    RetCode Open(ImgResDsc& dsc) override;

    RetCode GetHeader(ImgResDsc& dsc) override;

    RetCode ReadToCache(ImgResDsc& dsc) override;

    RetCode DecodeJPEG(ImageInfo &imageInfo, const td_u8 *fileData, uint32_t fileLength) override;

private:
    uint8_t* filedata_ {nullptr};
    struct jpeg_decompress_struct dinfo_ = {0};
    struct jpeg_decompress_struct dinfo1_ = {0};

    RetCode CreateDecoder(ImgResDsc& dsc);
    RetCode CreateDecoder(const td_u8 *fileData, uint32_t fileLength, jpeg_decompress_struct *dinfo);
    void DestroyDecoder(void);
    RetCode DecodeJPEG(ImgResDsc& dsc);
    RetCode DecodeSDRJPEG(ImgResDsc& dsc);
    RetCode DecodeHDRJPEG(ImgResDsc& dsc);
    RetCode DecodeHDRJPEGLayers(ext_vau_surface *srcSurfaces, ext_hdr_attr *hdrAttrs, ImageInfo *hdrImages);
    RetCode ConvertYUVToRGB(ImageInfo& imageInfo, jpeg_decompress_struct &dinfo, jpeg_dst_struct* dst);

    HWJpegDecoderImplement(const HWJpegDecoderImplement&) = delete;
    HWJpegDecoderImplement& operator=(const HWJpegDecoderImplement&) = delete;
    HWJpegDecoderImplement(HWJpegDecoderImplement&&) = delete;
    HWJpegDecoderImplement& operator=(HWJpegDecoderImplement&&) = delete;
    static constexpr uint8_t HDR_STD_LAYER_CNT = 2;
};


RetCode HWJpegDecoderImplement::Open(ImgResDsc& dsc)
{
    char path[PATH_MAX] = {0};
    if (realpath(dsc.path, path) == nullptr) {
        GRAPHIC_LOGE("Invalid path.");
        return RetCode::FAIL;
    }
    FILE* fp = fopen(path, "rb");
    if (fp == nullptr) {
        return RetCode::FAIL;
    }
    dsc.fd = 0;
    dsc.imgInfo.data = nullptr;
    dsc.imgInfo.phyAddr = nullptr;
    dsc.inCache_ = false;

    fseek(fp, 0, SEEK_END);
    g_fileLen = static_cast<uint32_t>(ftell(fp));
    filedata_ = GrallocEngines::GetInstance()->AllocBufferBySize(g_fileLen,
        HBM_USE_ASSIGN_SIZE | HBM_USE_MEM_JPEG_DATA);
    if (filedata_ == nullptr) {
        fclose(fp);
        GRAPHIC_LOGE("Alloc buffer for data failed.");
        return RetCode::FAIL;
    }

    fseek(fp, 0, SEEK_SET);
    if (fread(filedata_, 1, g_fileLen, fp) != g_fileLen) {
        fclose(fp);
        GrallocEngines::GetInstance()->FreeBuffer(filedata_);
        filedata_ = nullptr;
        GRAPHIC_LOGE("Read data failed.");
        return RetCode::FAIL;
    }
    fclose(fp);

    return RetCode::OK;
}

RetCode HWJpegDecoderImplement::CreateDecoder(ImgResDsc& dsc)
{
    if (filedata_ == nullptr) {
        return RetCode::FAIL;
    }

    memset_s(&dinfo_, sizeof(jpeg_decompress_struct), 0, sizeof(jpeg_decompress_struct));
    struct jpeg_source_struct src = {0};

    src.vir_buf = reinterpret_cast<td_char *>(filedata_);
    src.phy_buf = reinterpret_cast<td_u32>(uintptr_t(src.vir_buf));
    src.size = g_fileLen;
    dinfo_.scale = JPEG_SCALE_1; // suggest to customize the scaling ratio based on the file name.
    dinfo_.output_yuv420sp = true;

    int ret = uapi_jpeg_decomress_create(&dinfo_, &src, true);
    if (ret != 0) {
        GrallocEngines::GetInstance()->FreeBuffer(filedata_);
        filedata_ = nullptr;
        return RetCode::FAIL;
    }
    return RetCode::OK;
}

RetCode HWJpegDecoderImplement::CreateDecoder(const td_u8 *fileData, uint32_t fileLength, jpeg_decompress_struct *dinfo)
{
    struct jpeg_source_struct src = {0};

    src.vir_buf = (td_char *)fileData;
    src.phy_buf = reinterpret_cast<uint32_t>(src.vir_buf);
    src.size = fileLength;
    dinfo->scale = JPEG_SCALE_1; // suggest to customize the scaling ratio based on the file name.
    dinfo->output_yuv420sp = true;

    int ret = uapi_jpeg_decomress_create(dinfo, &src, true);
    if (ret != 0) {
        return RetCode::FAIL;
    }
    return RetCode::OK;
}

RetCode HWJpegDecoderImplement::GetHeader(ImgResDsc& dsc)
{
    if (CreateDecoder(dsc) != RetCode::OK) {
        return RetCode::FAIL;
    }

    dsc.imgInfo.header.width = dinfo_.output_width;
    dsc.imgInfo.header.height = dinfo_.output_height;
    dsc.imgInfo.header.colorMode = RGB888;
    dsc.imgInfo.header.compressMode = 0;
    dsc.imgInfo.header.reserved = 0;

    dsc.imgInfo.isHdr = false;
    if (dinfo_.hdr_attr.image_cnt == HDR_STD_LAYER_CNT) {
        dsc.imgInfo.isHdr = true;
    }

    return RetCode::OK;
}

RetCode HWJpegDecoderImplement::DecodeSDRJPEG(ImgResDsc& dsc)
{
    struct jpeg_dst_struct dst = {0};
    dst.stride = dinfo_.output.stride[0];
    dst.uv_stride = dinfo_.output.stride[1];
    dst.uv_offset = dinfo_.output.size[0];
    dst.size = dinfo_.output.size[0] + dinfo_.output.size[1];

    ImageInfo temp = {{0}};
    temp.dataSize = dst.size;
    dst.phy_buf = reinterpret_cast<td_u32>((uintptr_t)ImageCacheMalloc(temp));
    if (dst.phy_buf == 0) {
        return RetCode::FAIL;
    }
    dst.vir_buf = reinterpret_cast<td_char *>((uintptr_t)dst.phy_buf);
    uapi_jpeg_decompress_start(&dinfo_, &dst);
    ConvertYUVToRGB(dsc.imgInfo, dinfo_, &dst);
    uapi_jpeg_decompress_destroy(&dinfo_);
    ImageCacheFree(temp);
    return RetCode::OK;
}

RetCode HWJpegDecoderImplement::DecodeHDRJPEGLayers(ext_vau_surface *srcSurfaces, ext_hdr_attr *hdrAttrs, ImageInfo *hdrImages)
{
    struct jpeg_dst_struct dst = {0};
    for (td_u32 i = 0; i < HDR_STD_LAYER_CNT; i++) {
        ImageInfo &temp = hdrImages[i];
        if (i > 0) {
            struct jpeg_source_struct src = {0};
            src.vir_buf = reinterpret_cast<td_char *>(filedata_);
            src.phy_buf = reinterpret_cast<td_u32>((uintptr_t)src.vir_buf);
            src.size = g_fileLen;
            uapi_jpeg_decomress_create(&dinfo_, &src, true);
        }
        dst.stride = dinfo_.output.stride[0];
        dst.uv_stride = dinfo_.output.stride[1];
        dst.uv_offset = dinfo_.output.size[0];
        dst.size = dinfo_.output.size[0] + dinfo_.output.size[1];

        temp.dataSize = dst.size;
        dst.phy_buf = reinterpret_cast<td_u32>((uintptr_t)ImageCacheMalloc(temp));
        if (dst.phy_buf == 0) {
            for (td_u32 j = 0; j < i; j++) {
                ImageCacheFree(hdrImages[j]);
            }
            return RetCode::FAIL;
        }
        dst.vir_buf = reinterpret_cast<td_char *>((uintptr_t)dst.phy_buf);
        uapi_jpeg_decompress_start(&dinfo_, &dst);
        uapi_jpeg_decompress_destroy(&dinfo_);

        td_u32 index = ((i == 0) ? 1 : 0);
        srcSurfaces[index].phy_addr      = dst.phy_buf;
        srcSurfaces[index].vir_addr      = (td_uchar *)dst.vir_buf;
        srcSurfaces[index].width         = dinfo_.output.width[0];
        srcSurfaces[index].height        = dinfo_.output.height[0];
        srcSurfaces[index].stride        = dinfo_.output.stride[0];
        srcSurfaces[index].color_fmt     = (index == 1) ? EXT_GFX_FMT_YUV420SP : EXT_GFX_FMT_YUV400SP;
        srcSurfaces[index].compress_mode = EXT_GFX_COMPRESS_NONE;
        srcSurfaces[index].rect.x        = 0;
        srcSurfaces[index].rect.y        = 0;
        srcSurfaces[index].rect.width    = srcSurfaces[index].width;
        srcSurfaces[index].rect.height   = srcSurfaces[index].height;
        srcSurfaces[index].uv_offset     = dinfo_.output.size[0];

        hdrAttrs[index] = dinfo_.hdr_attr;
        srcSurfaces[index].hdr_attr = &hdrAttrs[index];
        dinfo_.gainmap_decoding_en = TD_TRUE;
    }
    return RetCode::OK;
}

static void RenderHdrImageToCache(ext_vau_surface_list *surface_list)
{
    td_s32 ret;
    td_handle handle;

    handle = uapi_vau_create();
    if (handle <= 0) {
        GRAPHIC_LOGE("uapi_vau_create failed");
        return;
    }

    ret = uapi_vau_render_hdr(handle, surface_list);
    if (ret != EXT_SUCCESS) {
        uapi_vau_cancel(handle);
        GRAPHIC_LOGE("uapi_vau_render_hdr failed");
        return;
    }

    ret = uapi_vau_submit(handle, TD_TRUE, 1000); /* 1000 ms timeout */
    if (ret != EXT_SUCCESS) {
        uapi_vau_cancel(handle);
        GRAPHIC_LOGE("uapi_vau_submit failed");
        return;
    }

    uapi_vau_destroy(handle);
    return;
}

RetCode HWJpegDecoderImplement::DecodeHDRJPEG(ImgResDsc& dsc)
{
    ext_hdr_attr hdrAttrs[2] = {0};       /* 2:index */
    ext_vau_surface srcSurfaces[2] = {0}; /* 2:index */
    ext_vau_surface dstSurface = {0};
    ext_vau_surface_list surfaceList = {0};
    ext_vau_blit_opt opt = {0};
    RetCode ret = RetCode::OK;

    ImageInfo hdrImages[HDR_STD_LAYER_CNT] = {{0}};

    if (DecodeHDRJPEGLayers(srcSurfaces, hdrAttrs, hdrImages) != RetCode::OK) {
        return RetCode::FAIL;
    }

    dstSurface.width = srcSurfaces[1].width;
    dstSurface.height = srcSurfaces[1].height;
    dstSurface.color_fmt = EXT_GFX_FMT_RGB888;
    dstSurface.compress_mode = EXT_GFX_COMPRESS_NONE;
    dstSurface.argb_order = EXT_GFX_FMT_ORDER_ARGB;
    dstSurface.rect.x = 0;
    dstSurface.rect.y = 0;
    dstSurface.rect.width = srcSurfaces[1].width;
    dstSurface.rect.height = srcSurfaces[1].height;
    dstSurface.stride = uapi_byte_align(dstSurface.width, BYTE_ALIGNMENT) * 3; // 3: bpp of RGB888
    ImageInfo dstImageInfo = {{0}};
    dstImageInfo.dataSize = dstSurface.stride * dstSurface.rect.height;
    dstSurface.phy_addr = reinterpret_cast<td_u32>((uintptr_t)ImageCacheMalloc(dstImageInfo));
    if (dstSurface.phy_addr == 0) {
        ret = RetCode::FAIL;
        goto QUIT;
    }

    surfaceList.src_surface_cnt = 2; /* 2:index */
    surfaceList.src_surface = srcSurfaces;
    surfaceList.dst_surface = &dstSurface;
    surfaceList.opt = &opt;

    RenderHdrImageToCache(&surfaceList);

    dsc.imgInfo.data = (uint8_t *)(uintptr_t)dstSurface.phy_addr;
    dsc.imgInfo.phyAddr = (uint8_t *)(uintptr_t)dstSurface.phy_addr;
    dsc.imgInfo.dataSize = dstImageInfo.dataSize;
QUIT:
    for (td_u32 i = 0; i < HDR_STD_LAYER_CNT; i++) {
        ImageInfo &temp = hdrImages[i];
        ImageCacheFree(temp);
    }
    return ret;
}

RetCode HWJpegDecoderImplement::DecodeJPEG(ImgResDsc& dsc)
{
    RetCode ret;
    if (dsc.imgInfo.isHdr) {
        ret = DecodeHDRJPEG(dsc);
    } else {
        ret = DecodeSDRJPEG(dsc);
    }
    if (ret == RetCode::OK) {
        GrallocEngines::GetInstance()->FreeBuffer(filedata_);
        filedata_ = nullptr;
        dsc.inCache_ = true;
        dsc.fd = -1;
    }
    return ret;
}

static td_s32 VauRenderYuvToRgb(ext_vau_surface* srcSurface, ext_vau_surface* dstSurface)
{
    td_handle handle = uapi_vau_create();
    if (handle <= 0) {
        GRAPHIC_LOGE("uapi_vau_create failed");
        return TD_FALSE;
    }

    ext_vau_surface_list surfaceList = {0};
    surfaceList.src_surface_cnt = 1;
    surfaceList.src_surface = srcSurface;
    surfaceList.dst_surface = dstSurface;

    td_s32 ret = uapi_vau_render(handle, &surfaceList);
    if (ret != EXT_SUCCESS) {
        GRAPHIC_LOGE("uapi_vau_render failed");
        uapi_vau_cancel(handle);
        uapi_vau_destroy(handle);
        return TD_FALSE;
    }

    ret = uapi_vau_submit(handle, TD_TRUE, 1000); /* 1000 ms timeout */
    if (ret != EXT_SUCCESS) {
        GRAPHIC_LOGE("uapi_vau_submit failed");
        uapi_vau_cancel(handle);
        uapi_vau_destroy(handle);
        return TD_FALSE;
    }

    uapi_vau_destroy(handle);
    return EXT_SUCCESS;
}

static void InitVauSrcSurface(ext_vau_surface* surface, jpeg_dst_struct* dst, uint16_t width, uint16_t height)
{
    surface->phy_addr = dst->phy_buf;
    surface->vir_addr = reinterpret_cast<td_uchar*>(dst->vir_buf);
    surface->width = width;
    surface->height = height;
    surface->stride = dst->stride;
    surface->color_fmt = EXT_GFX_FMT_YUV420SP;
    surface->compress_mode = EXT_GFX_COMPRESS_NONE;
    surface->rect = {0, 0, width, height};
    surface->uv_offset = dst->uv_offset;
}

static void InitVauDstSurface(ext_vau_surface* surface, uint8_t* rgbBuf, uint16_t width, uint16_t height, uint16_t stride)
{
    surface->phy_addr = reinterpret_cast<td_u32>((uintptr_t)rgbBuf);
    surface->vir_addr = rgbBuf;
    surface->width = width;
    surface->height = height;
    surface->stride = stride;
    surface->color_fmt = EXT_GFX_FMT_RGB888;
    surface->compress_mode = EXT_GFX_COMPRESS_NONE;
    surface->argb_order = EXT_GFX_FMT_ORDER_ARGB;
    surface->rect = {0, 0, width, height};
}

RetCode HWJpegDecoderImplement::ConvertYUVToRGB(ImageInfo& imageInfo, jpeg_decompress_struct &dinfo, jpeg_dst_struct* dst)
{
    uint16_t imgWidth = dinfo.output_width;
    uint16_t imgHeight = dinfo.output_height;
    uint16_t alignedImgWidth = ALIGN_BYTE(imgWidth, BYTE_ALIGNMENT);
    uint32_t rgbSize = alignedImgWidth * imgHeight * 3; // 3: bpp of RGB888

    ImageInfo tempImgInfo = {{0}};
    tempImgInfo.dataSize = rgbSize;
    uint8_t* rgbAddr = reinterpret_cast<uint8_t*>(ImageCacheMalloc(tempImgInfo));
    if (rgbAddr == nullptr) {
        GRAPHIC_LOGE("ImageCacheMalloc for rgb failed.");
        return RetCode::FAIL;
    }

    ext_vau_surface srcSurface = {0};
    InitVauSrcSurface(&srcSurface, dst, imgWidth, imgHeight);

    ext_vau_surface dstSurface = {0};
    InitVauDstSurface(&dstSurface, rgbAddr, imgWidth, imgHeight, alignedImgWidth * 3); // 3: bpp of RGB888

    if (VauRenderYuvToRgb(&srcSurface, &dstSurface) != EXT_SUCCESS) {
        ImageCacheFree(tempImgInfo);
        return RetCode::FAIL;
    }

    imageInfo.data = rgbAddr;
    imageInfo.phyAddr = rgbAddr;
    imageInfo.dataSize = rgbSize;
    return RetCode::OK;
}

RetCode HWJpegDecoderImplement::ReadToCache(ImgResDsc& dsc)
{
    if (dsc.inCache_) {
        return RetCode::OK;
    }

    DecodeJPEG(dsc);
    return RetCode::OK;
}

RetCode HWJpegDecoderImplement::DecodeJPEG(ImageInfo &imageInfo, const td_u8 *fileData, uint32_t fileLength)
{
    jpeg_decompress_struct dinfo = {0};
    if (CreateDecoder(fileData, fileLength, &dinfo) != RetCode::OK) {
        GRAPHIC_LOGE("CreateDecoder failed.");
        return RetCode::FAIL;
    }
    imageInfo.header.width = dinfo.output_width;
    imageInfo.header.height = dinfo.output_height;
    imageInfo.header.colorMode = RGB888;
#ifdef VERSION_IOT
    imageInfo.header.compressMode = 0;
#endif

    RetCode ret = RetCode::OK;
    struct jpeg_dst_struct dst = {0};
    dst.stride = dinfo.output.stride[0];
    dst.uv_stride = dinfo.output.stride[1];
    dst.uv_offset = dinfo.output.size[0];
    dst.size = dinfo.output.size[0] + dinfo.output.size[1];

    ImageInfo temp = {{0}};
    temp.dataSize = dst.size;
    dst.phy_buf = reinterpret_cast<uint32_t>(ImageCacheMalloc(temp));
    dst.vir_buf = reinterpret_cast<td_char *>(dst.phy_buf);
    uapi_jpeg_decompress_start(&dinfo, &dst);
    if (ConvertYUVToRGB(imageInfo, dinfo, &dst) != RetCode::OK) {
        GRAPHIC_LOGE("ConvertYUVToRGB failed.");
        ret = RetCode::FAIL;
    }
    uapi_jpeg_decompress_destroy(&dinfo);
    ImageCacheFree(temp);
    return ret;
}
} // namespace

HWJpegDecoder& HWJpegDecoder::GetInstance()
{
    static HWJpegDecoderImplement instance;
    return instance;
}
} // namespace OHOS
#endif
