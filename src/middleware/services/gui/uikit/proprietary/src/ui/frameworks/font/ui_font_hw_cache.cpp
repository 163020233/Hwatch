/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies Co., Ltd. All rights reserved
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

#include "font/ui_font_hw_cache.h"
#include <cstddef>
#include "gfx_utils/graphic_log.h"
#include "hals/gralloc_engines.h"

namespace OHOS {
UIFontHWCache::UIFontHWCache()
{
    ListInit(lruList_);
}

UIFontHWCache::~UIFontHWCache() {}

UIFontHWCache::FontInfo* UIFontHWCache::SearchFontInfo(uint16_t fontId)
{
    for (auto iter : fontInfos_) {
        if (iter->fontId == fontId) {
            return iter;
        }
    }
    FontInfo* fontInfo = new FontInfo;
    fontInfo->fontId = fontId;
    fontInfos_.push_back(fontInfo);
    return fontInfo;
}

bool UIFontHWCache::TryToFreeFontCache()
{
    if ((lruList_.prev == &lruList_) && (lruList_.next == &lruList_)) {
        GRAPHIC_LOGE("Nothing in font cache can be freed!");
        return false;
    }
    CacheData* toFree = reinterpret_cast<struct CacheData *>(reinterpret_cast<uint8_t *>(lruList_.prev) -
        offsetof(struct CacheData, lruHead));
    FontInfo* fontInfo = SearchFontInfo(toFree->fontId);
    auto it = fontInfo->caches.find(toFree->unicode);
    if (it != fontInfo->caches.end()) {
        fontInfo->caches.erase(it);
    } else {
        GRAPHIC_LOGE("unicode %u was not found in font %u", toFree->unicode, toFree->fontId);
    }
    ListDel(toFree->lruHead);
    GrallocEngines::GetInstance()->FreeBuffer(reinterpret_cast<uint8_t*>(toFree));
    return true;
}

uint8_t* UIFontHWCache::GetSpace(uint16_t fontId, uint32_t unicode, uint32_t size)
{
    CacheData* cache = reinterpret_cast<CacheData*>(GrallocEngines::GetInstance()->AllocBufferBySize(
        sizeof(CacheData) + size, HBM_USE_ASSIGN_SIZE | HBM_USE_MEM_FONT));
    if (cache == nullptr) {
        GRAPHIC_LOGE("GetSpace AllocBuffer failed.");
        return nullptr;
    }

    cache->fontId = fontId;
    cache->unicode = unicode;

    ListInit(cache->lruHead);
    ListAdd(cache->lruHead, lruList_);
    FontInfo* fontInfo = SearchFontInfo(fontId);
    fontInfo->caches.emplace(std::make_pair(unicode, cache));
    return reinterpret_cast<uint8_t*>(cache->data);
}

void UIFontHWCache::PutSpace(uint8_t* addr)
{
    if (addr == nullptr) {
        return;
    }
    CacheData* cache = reinterpret_cast<CacheData *>(addr  - sizeof(CacheData));
    FontInfo* fontInfo = SearchFontInfo(cache->fontId);
    auto it = fontInfo->caches.find(cache->unicode);
    if (it == fontInfo->caches.end()) {
        fontInfo->caches.erase(it);
    } else {
        GRAPHIC_LOGE("unicode %u was not found in font %u", cache->unicode, cache->fontId);
        return;
    }
    ListDel(cache->lruHead);
    GrallocEngines::GetInstance()->FreeBuffer(reinterpret_cast<uint8_t*>(addr));
}

uint8_t* UIFontHWCache::GetCacheData(uint16_t fontId, uint32_t unicode)
{
    FontInfo* fontInfo = SearchFontInfo(fontId);
    auto it = fontInfo->caches.find(unicode);
    if (it == fontInfo->caches.end()) {
        return nullptr;
    }

    CacheData* cache = reinterpret_cast<CacheData *>(it->second);
    // update lru list
    ListDel(cache->lruHead);
    ListInit(cache->lruHead);
    ListAdd(cache->lruHead, lruList_);
    return cache->data;
}
} // namespace OHOS
