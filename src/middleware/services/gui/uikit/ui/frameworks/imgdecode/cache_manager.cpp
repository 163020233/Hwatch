/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "imgdecode/cache_manager.h"
#include "gfx_utils/graphic_log.h"
#include "hal_tick.h"
#include "securec.h"
#if (ENABLE_JPEG_AND_PNG && CACHE_JPEG_AND_PNG)
#include "common/image.h"
#include "imgdecode/jpeg_img_decoder.h"
#include "imgdecode/png_img_decoder.h"
#endif
#if CAPABILITY_HW_JPEG_DECODE
#include "imgdecode/hw_jpeg_decoder.h"
#include "common/image.h"
#endif
#if (ENABLE_JPEG_AND_PNG && CACHE_JPEG_AND_PNG && CAPABILITY_SW_JPEG_DECODE)
#include "jpeglib.h"
#endif

namespace OHOS {
namespace {
#if (ENABLE_JPEG_AND_PNG && CACHE_JPEG_AND_PNG && CAPABILITY_SW_JPEG_DECODE)
/**
 * JPEG 类型枚举
 */
enum class JpegType {
    Baseline   = 0,  // 基线 JPEG
    Progressive = 1, // 渐进式 JPEG
    Other      = 2   // 文件打开失败、非 JPEG 格式等
};

/**
 * 获取 JPEG 图片的类型。
 * @param filename  JPEG 文件路径
 * @return JpegType 枚举值
 */
JpegType GetJpegType(const char *filename)
{
    FILE *infile = fopen(filename, "rb");
    if (!infile) {
        return JpegType::Other;
    }

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    jpeg_stdio_src(&cinfo, infile);

    // 解析 JPEG 头（不会解码像素数据）
    if (jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK) {
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        return JpegType::Other;
    }

    JpegType type = JpegType::Other;
    if (cinfo.progressive_mode) {
        type = JpegType::Progressive;
    } else {
        type = JpegType::Baseline;
    }

    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return type;
}
#endif
}

const uint8_t* CacheEntry::GetImgData() const
{
    return dsc_.imgInfo.data;
}

RetCode CacheEntry::ReadLine(const Point& start, int16_t len, uint8_t* buf)
{
    RetCode ret;
    if (dsc_.decoder != nullptr) {
        ret = dsc_.decoder->ReadLine(dsc_, start, len, buf);
    } else {
        ret = RetCode::FAIL;
    }

    return ret;
}

void CacheEntry::Clear()
{
    if (dsc_.decoder != nullptr) {
        dsc_.decoder->Close(dsc_);
    }

    dsc_.decoder = nullptr;
    ClearSrc();
    dsc_.imgInfo.data = nullptr;
    dsc_.fd = -1;
    dsc_.srcType = IMG_SRC_UNKNOWN;
    life_ = 0;
}

void CacheEntry::ClearSrc()
{
    if (dsc_.srcType == IMG_SRC_FILE) {
        UIFree(const_cast<char*>(dsc_.path));
    }
    dsc_.path = nullptr;
}

RetCode CacheEntry::SetSrc(const char* path)
{
    ClearSrc();
    if (dsc_.srcType == IMG_SRC_FILE) {
        size_t strLen = strlen(path);
        if (strLen > MAX_SRC_LENGTH) {
            return RetCode::FAIL;
        }
        char* newStr = static_cast<char*>(UIMalloc(static_cast<uint32_t>(strLen) + 1));
        if (newStr == nullptr) {
            return RetCode::FAIL;
        }
        if (memcpy_s(newStr, strLen + 1, path, strLen) != EOK) {
            UIFree(reinterpret_cast<void*>(newStr));
            newStr = nullptr;
            return RetCode::FAIL;
        }
        newStr[strLen] = '\0';
        dsc_.path = newStr;
    } else {
        dsc_.path = path;
    }
    return RetCode::OK;
}

CacheManager& CacheManager::GetInstance()
{
    static CacheManager instance;
    return instance;
}

RetCode CacheManager::Init(uint16_t size)
{
    if ((size == 0) || (size > DEFAULT_MAX_CACHE_ENTRY_NUM)) {
        return RetCode::FAIL;
    }
    Reset();
    if (entryArr_ != nullptr) {
        UIFree(reinterpret_cast<void*>(entryArr_));
    }

    uint32_t tmpCacheSize = size * sizeof(CacheEntry);
    entryArr_ = static_cast<CacheEntry*>(UIMalloc(tmpCacheSize));
    if (entryArr_ == nullptr) {
        size_ = 0;
        return RetCode::FAIL;
    }

    if (memset_s(entryArr_, tmpCacheSize, 0, tmpCacheSize) != EOK) {
        UIFree(reinterpret_cast<void*>(entryArr_));
        entryArr_ = nullptr;
        return RetCode::FAIL;
    }

    size_ = size;
    return RetCode::OK;
}

RetCode CacheManager::Open(const char* path, const Style& style, CacheEntry& entry)
{
    if ((path == nullptr) || (GetSize() <= 0)) {
        return RetCode::FAIL;
    }

    AgingAll();
    uint16_t indexHitted = 0;
    RetCode ret = GetIndex(path, indexHitted);
    if (ret == RetCode::OK) {
        entry = entryArr_[indexHitted];
        return RetCode::OK;
    }

    SelectEntryToReplace(indexHitted);
    if ((entryArr_[indexHitted].dsc_.path != nullptr) && (entryArr_[indexHitted].dsc_.decoder != nullptr)) {
        entryArr_[indexHitted].dsc_.decoder->Close(entryArr_[indexHitted].dsc_);
    }

    uint32_t startTime = HALTick::GetInstance().GetTime();
    entryArr_[indexHitted].life_ = 0;

    ret = TryDecode(path, style, entryArr_[indexHitted]);
    if (ret != RetCode::OK) {
        GRAPHIC_LOGE("CacheManager TryDecode %s failed\n", path);
        return ret;
    }
    ret = ReadToCache(entryArr_[indexHitted]);
    if (ret != RetCode::OK) {
        GRAPHIC_LOGE("CacheManager ReadToCache %s failed\n", path);
        Clear(entryArr_[indexHitted]);
        return ret;
    }
    entryArr_[indexHitted].life_ = HALTick::GetInstance().GetElapseTime(startTime);
    entry = entryArr_[indexHitted];
    return RetCode::OK;
}

RetCode CacheManager::Close(const char* path)
{
    if (path == nullptr) {
        return RetCode::FAIL;
    }

    for (uint16_t index = 0; index < GetSize(); index++) {
        if (entryArr_[index].dsc_.srcType == IMG_SRC_FILE) {
            if (entryArr_[index].dsc_.path == nullptr) {
                continue;
            }
            if (strcmp(entryArr_[index].dsc_.path, path) == 0) {
                Clear(entryArr_[index]);
                break;
            }
        } else {
            if (entryArr_[index].dsc_.path == path) {
                Clear(entryArr_[index]);
                break;
            }
        }
    }

    return RetCode::OK;
}

bool CacheManager::GetImageHeader(const char* path, ImageHeader& header)
{
    CacheEntry entry;
    Style useless;
    RetCode ret = Open(path, useless, entry);
    if (ret != RetCode::OK) {
        GRAPHIC_LOGW("CacheManager::GetImageHeader Image get info found unknown src type\n");
        return false;
    }

    header = entry.GetImgHeader();
    return true;
}

RetCode CacheManager::Reset()
{
    if (entryArr_ == nullptr) {
        return RetCode::OK;
    }

    for (uint16_t index = 0; index < GetSize(); index++) {
        if (entryArr_[index].dsc_.path != nullptr) {
            Clear(entryArr_[index]);
        }
    }

    return RetCode::OK;
}

RetCode CacheManager::ReadToCache(CacheEntry& entry)
{
    if (entry.dsc_.decoder == nullptr) {
        return RetCode::FAIL;
    }
    return entry.dsc_.decoder->ReadToCache(entry.dsc_);
}

void CacheManager::Clear(CacheEntry& entry)
{
    entry.Clear();
}

void CacheManager::AgingAll(int32_t time)
{
    for (uint16_t index = 0; index < GetSize(); index++) {
        if (entryArr_[index].life_ > INT32_MIN + AGING_INTERVAL) {
            entryArr_[index].life_ -= time;
        }
    }
}

RetCode CacheManager::GetIndex(const char* path, uint16_t& hittedIndex)
{
    for (uint16_t index = 0; index < GetSize(); index++) {
        if (entryArr_[index].dsc_.srcType == IMG_SRC_FILE) {
            if ((entryArr_[index].dsc_.path != nullptr) && !strcmp(path, entryArr_[index].dsc_.path)) {
                entryArr_[index].life_ += entryArr_[index].dsc_.timeToOpen * LIFE_GAIN_INTERVAL;
                if (entryArr_[index].life_ > LIFE_LIMIT) {
                    entryArr_[index].life_ = LIFE_LIMIT;
                }
                hittedIndex = index;
                return RetCode::OK;
            }
        } else {
            ImageInfo* imgDsc = reinterpret_cast<ImageInfo*>(const_cast<char*>(path));
            if ((entryArr_[index].dsc_.path == path) && (entryArr_[index].dsc_.imgInfo.data == imgDsc->data)) {
                entryArr_[index].life_ += entryArr_[index].dsc_.timeToOpen * LIFE_GAIN_INTERVAL;
                if (entryArr_[index].life_ > LIFE_LIMIT) {
                    entryArr_[index].life_ = LIFE_LIMIT;
                }
                hittedIndex = index;
                return RetCode::OK;
            }
        }
    }

    return RetCode::FAIL;
}

RetCode CacheManager::SelectEntryToReplace(uint16_t& selectedIndex)
{
    selectedIndex = 0;
    for (uint16_t index = 0; index < GetSize(); index++) {
        if (entryArr_[index].life_ < entryArr_[selectedIndex].life_) {
            selectedIndex = index;
        }
    }

    return RetCode::OK;
}

RetCode CacheManager::TryDecode(const char* path, const Style& style, CacheEntry& entry)
{
    entry.dsc_.srcType = IMG_SRC_FILE;
    RetCode ret = entry.SetSrc(path);
    if (ret != RetCode::OK) {
        Clear(entry);
        return ret;
    }
    FileImgDecoder* decoder = &(FileImgDecoder::GetInstance());
#if ENABLE_JPEG_AND_PNG || CAPABILITY_HW_JPEG_DECODE
    Image::ImageType type = Image::CheckImgType(path);
    if (type == Image::IMG_JPEG) {
#if (ENABLE_JPEG_AND_PNG && CACHE_JPEG_AND_PNG && CAPABILITY_SW_JPEG_DECODE) && CAPABILITY_HW_JPEG_DECODE
        if (GetJpegType(path) == JpegType::Baseline) {
            decoder = &(HWJpegDecoder::GetInstance());
        } else {
            decoder = &(JpegImgDecoder::GetInstance());
        }
#elif CAPABILITY_HW_JPEG_DECODE
        decoder = &(HWJpegDecoder::GetInstance());
#elif (ENABLE_JPEG_AND_PNG && CACHE_JPEG_AND_PNG && CAPABILITY_SW_JPEG_DECODE)
        decoder = &(JpegImgDecoder::GetInstance());
#else
        decoder = nullptr;
#endif
    } else if (type == Image::IMG_PNG) {
#if (ENABLE_JPEG_AND_PNG && CACHE_JPEG_AND_PNG)
        decoder = &(PngImgDecoder::GetInstance());
#endif
    }
#endif
    if (decoder == nullptr) {
        Clear(entry);
        return RetCode::FAIL;
    }
    entry.dsc_.decoder = decoder;

    ret = entry.dsc_.decoder->Open(entry.dsc_);
    if (ret != RetCode::OK) {
        Clear(entry);
        return ret;
    }

    ret = entry.dsc_.decoder->GetHeader(entry.dsc_);
    if (ret != RetCode::OK) {
        Clear(entry);
        return ret;
    }

    return ret;
}
} // namespace OHOS
