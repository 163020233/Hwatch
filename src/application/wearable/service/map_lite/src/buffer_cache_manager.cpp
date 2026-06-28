/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Create Date : 2025-11
 */

#include "buffer_cache_manager.h"
#include <cstring>
#include <cstdio>
#include "securec.h"
#include "gfx_utils/mem_api.h"
#include "amap_log_adapter.h"

namespace OHOS {
// 构造函数
BufferCache::BufferCache()
{
    for (int i = 0; i < CACHE_MAX_ITEMS; ++i) {
        m_pool[i].key[0] = '\0';  // 初始化为空字符串
        m_pool[i].data = nullptr;
        m_pool[i].freeFunc = nullptr;
        m_pool[i].width = 0;
        m_pool[i].height = 0;
        m_pool[i].bytesPerPixel = 0;
        m_pool[i].used = false;
        m_pool[i].valid = false;
    }
}

// 析构函数：释放所有堆内存
BufferCache::~BufferCache()
{
    FreeAllRes();
}

bool BufferCache::SetSlot(const char *key, const BufferDesc *info)
{
    if (info == nullptr || key == nullptr || strlen(key) >= KEY_MAX_LEN || info->data == nullptr) {
        AMAP_LOGE("input err info=%p", info);
        return false;
    }

    int32_t idx = FindValidSlot(key);
    if (idx < 0) {
        // 不存在，找空闲槽
        idx = FindFreeSlot();
        if (idx < 0) {
            AMAP_LOGE("out of buf");
            return false;
        }
        // 分配新 key
        strcpy_s(m_pool[idx].key, KEY_MAX_LEN, key);
    }

    // 设置其他属性
    m_pool[idx].data = info->data;
    m_pool[idx].freeFunc = info->freeFunc;
    m_pool[idx].width = info->width;
    m_pool[idx].height = info->height;
    m_pool[idx].bytesPerPixel = info->bytesPerPixel;
    m_pool[idx].used = true;
    m_pool[idx].valid = true;
    return true;
}

void BufferCache::ReleaseUsed()
{
    for (int i = 0; i < CACHE_MAX_ITEMS; ++i) {
        if (m_pool[i].used) {
            // 保留，但重置 used 标记供下一轮
            m_pool[i].used = false;
        }
    }
}

void BufferCache::DumpValidSlot() const
{
    for (int i = 0; i < CACHE_MAX_ITEMS; ++i) {
        if (m_pool[i].valid) {
            AMAP_LOGD("i = %02d, data = %p, key = %s\n", i, m_pool[i].data, m_pool[i].key);
        }
    }
}

bool BufferCache::GetSlot(const char *key, BufferDesc *info)
{
    if (!info) {
        AMAP_LOGE("input is nul");
        return false;
    }
    int32_t idx = FindValidSlot(key);
    if (idx >= 0) {
        m_pool[idx].used = true;  // 标记为当前页使用
        info->width = m_pool[idx].width;
        info->height = m_pool[idx].height;
        info->bytesPerPixel = m_pool[idx].bytesPerPixel;
        info->freeFunc = m_pool[idx].freeFunc;
        info->data = m_pool[idx].data;
        return true;
    }
    return false;
}

int32_t BufferCache::FindValidSlot(const char *key) const
{
    if (!key) {
        return -1;
    }
    for (int i = 0; i < CACHE_MAX_ITEMS; ++i) {
        if (m_pool[i].valid && strcmp(m_pool[i].key, key) == 0) {
            return i;
        }
    }
    return -1;
}

int32_t BufferCache::FindFreeSlot()
{
    for (int i = 0; i < CACHE_MAX_ITEMS; ++i) {
        if (!m_pool[i].valid) {
            return i;
        }
    }
    // 全部已使用，只能老化之前数据
    for (int j = 0; j < CACHE_MAX_ITEMS; j++) {
        if (m_pool[j].used) {
            continue;
        }
        if (m_pool[j].freeFunc) {
            m_pool[j].freeFunc(m_pool[j].data);
            m_pool[j].data = nullptr;
        }
        m_pool[j].valid = false;
        return j;
    }
    return -1;
}

void BufferCache::FreeAllRes()
{
    for (int j = 0; j < CACHE_MAX_ITEMS; j++) {
        if (!m_pool[j].valid) {
            continue;
        }
        if (m_pool[j].freeFunc) {
            m_pool[j].freeFunc(m_pool[j].data);
            m_pool[j].data = nullptr;
            AMAP_LOGD("free idx = %d", j);
        }
        m_pool[j].valid = false;
    }
}
}  // namespace OHOS
