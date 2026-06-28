/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies Co., Ltd..
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

#include "img_decompress.h"
#include "gfx_utils/image_info.h"
#include "gfx_utils/graphic_math.h"
#ifdef _WIN32

namespace OHOS {
ImgDecompress& ImgDecompress::GetInstance()
{
    static ImgDecompress instance;
    return instance;
}

int ImgDecompress::GetBufferandSize(ext_vau_cmp_header header, unsigned char* &imageBuffer,
                                    uint32_t &dataSize, auto_dcmp_output* &outPut)
{
    if (header.pixel_format == HI_GFX_FMT_RGB565) {
        dataSize = sizeof(unsigned char) * ALIGN_BYTE(header.width, BYTE_ALIGNMENT) * 2  * header.height;  // 2 bytes
        imageBuffer = (unsigned char*)malloc(dataSize);
    } else if ((header.pixel_format == HI_GFX_FMT_ARGB8888)) {
        dataSize = sizeof(unsigned char) * ALIGN_BYTE(header.width, BYTE_ALIGNMENT) * 4 * header.height;  // 4 bytes
        imageBuffer = (unsigned char*)malloc(dataSize);
    } else if ((header.pixel_format == HI_GFX_FMT_RGB888)) {
        dataSize = sizeof(unsigned char) * ALIGN_BYTE(header.width, BYTE_ALIGNMENT) * 3 * header.height;  // 3 bytes
        imageBuffer = (unsigned char*)malloc(dataSize);
    }

    if (imageBuffer == nullptr) {
        printf("malloc failed ! size : %d\n", dataSize);
        return -1;
    }

    for (uint32_t i = 0; i < header.height; i++) {
        for (uint32_t j = 0; j < header.width; j++) {
            uint32_t index = i * ALIGN_BYTE(header.width, BYTE_ALIGNMENT) + j;
            uint32_t argbIndex = i * header.width + j;
            if ((header.pixel_format == HI_GFX_FMT_ARGB8888)) {
                uint32_t offset = index * 4;   // 4 bytes
                *(imageBuffer + offset) = *(outPut->rec_out_data[0]->blue_channel + argbIndex);
                *(imageBuffer + offset + 1) = *(outPut->rec_out_data[0]->green_channel + argbIndex);     // 1:index
                *(imageBuffer + offset + 2) = *(outPut->rec_out_data[0]->red_channel + argbIndex);       // 2:index
                *(imageBuffer + offset + 3) = *(outPut->rec_out_data[0]->alpha_channel + argbIndex);     // 3:index
            } else if (header.pixel_format == HI_GFX_FMT_RGB888) {
                uint32_t offset = index * 3;   // 3 bytes
                *(imageBuffer + offset) = *(outPut->rec_out_data[0]->blue_channel + argbIndex);
                *(imageBuffer + offset + 1) = *(outPut->rec_out_data[0]->green_channel + argbIndex);     // 1:index
                *(imageBuffer + offset + 2) = *(outPut->rec_out_data[0]->red_channel + argbIndex);       // 2:index
            } else if (header.pixel_format == HI_GFX_FMT_RGB565) {
                uint32_t offset = index * 2;   // 2 bytes
                // 5:green_channel offset
                // 11:red_channel offset
                uint16_t tmp = (((outPut->rec_out_data[0]->blue_channel[argbIndex]) & 0x1f) |
                               ((outPut->rec_out_data[0]->green_channel[argbIndex] & 0x3f) << 5) |
                               ((outPut->rec_out_data[0]->red_channel[argbIndex] & 0x1f) << 11));
                uint8_t *buffer = reinterpret_cast<uint8_t*>(&tmp);
                *(imageBuffer + offset) = buffer[0];
                *(imageBuffer + offset + 1) = buffer[1];
            }
        }
    }
    return 0;
}

void ImgDecompress::FreeCmpBuffer(auto_cmp_output* &info, auto_dcmp_output* &outPut)
{
    if ((info == nullptr) || (info->cmp_bs[0] == nullptr)) {
        return;
    }

    delete info->cmp_bs[0];
    info->cmp_bs[0] = nullptr;
    delete info;
    info = nullptr;

    if ((outPut == nullptr) || (outPut->rec_out_data[0] == nullptr) ||
        (outPut->rec_out_data[0]->red_channel == nullptr) ||
        (outPut->rec_out_data[0]->green_channel == nullptr) ||
        (outPut->rec_out_data[0]->blue_channel == nullptr) ||
        (outPut->rec_out_data[0]->alpha_channel == nullptr)) {
        return;
    }

    delete outPut->rec_out_data[0]->red_channel;
    outPut->rec_out_data[0]->red_channel = nullptr;
    delete outPut->rec_out_data[0]->green_channel;
    outPut->rec_out_data[0]->green_channel = nullptr;
    delete outPut->rec_out_data[0]->blue_channel;
    outPut->rec_out_data[0]->blue_channel = nullptr;
    delete outPut->rec_out_data[0]->alpha_channel;
    outPut->rec_out_data[0]->alpha_channel = nullptr;
    delete outPut->rec_out_data[0];
    outPut->rec_out_data[0] = nullptr;
    delete outPut;
    outPut = nullptr;
}

void ImgDecompress::Decompress(ImageInfo& img, bool isAssigned)
{
    if (img.header.compressMode == COMPRESS_MODE_NONE) {
        return;
    }
    ext_vau_cmp_header header = {0};
    uapi_vau_parse_cmp_header((td_u8*)img.data, 0, &header);

    auto_cmp_output *info = new auto_cmp_output;
    info->cmp_bs[0] = new BS_DATA;
    info->cmp_bs[0]->data_size = img.dataSize;
    info->cmp_bs[0]->bs_data = (unsigned char*)img.data;
    info->cmp_bs[0]->para.width = header.width;
    info->cmp_bs[0]->para.height =  header.height;
    info->cmp_bs[0]->para.stride =  header.stride;
    int channelLen = header.height * header.width;

    auto_dcmp_output *outPut = new auto_dcmp_output;
    outPut->rec_out_data[0] = new REC_DATA;
    outPut->rec_out_data[0]->red_channel = new unsigned char[channelLen];
    outPut->rec_out_data[0]->green_channel = new unsigned char[channelLen];
    outPut->rec_out_data[0]->blue_channel = new unsigned char[channelLen];
    outPut->rec_out_data[0]->alpha_channel = new unsigned char[channelLen];
    if (ice_v5r1_block_htc_auto_dcmp(info, outPut) == 0) {
        FreeCmpBuffer(info, outPut);
        return;
    }

    unsigned char* imageData = nullptr;
    if (GetBufferandSize(header, imageData, img.dataSize, outPut) < 0) {
        printf("Get buffer failed !\n");
        return;
    }

    if (isAssigned) {
        ImageCacheFree(img);
    }

    if (ImageCacheMalloc(img) == nullptr) {
        printf("img malloc failed ! size : %d\n", img.dataSize);
        return;
    }
    memcpy_s((void *)img.data, img.dataSize, imageData, img.dataSize);
    img.header.compressMode = COMPRESS_MODE_NONE;

    FreeCmpBuffer(info, outPut);
    free(imageData);
    imageData = nullptr;
}
}
#endif