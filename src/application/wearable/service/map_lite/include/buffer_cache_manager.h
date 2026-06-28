
/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Created: 2026-01-14
 */

#ifndef BUFFER_CACHE_MANAGER_H
#define BUFFER_CACHE_MANAGER_H

#include <cstdint>
#include <cstddef>

// ===== 配置参数（todo 待提供特征配置）=====
static constexpr int CACHE_MAX_ITEMS = 40;  // 缓存槽位数（建议 ≥ 20）
static constexpr int KEY_MAX_LEN = 32;      // 图片 ID 最大长度

// 函数指针类型: 释放资源的回调
typedef void (*FreeDataFunc)(void *arg);
typedef struct {
    uint8_t *data;
    FreeDataFunc freeFunc;
    uint32_t width;
    uint32_t height;
    uint32_t bytesPerPixel;
} BufferDesc;

namespace OHOS {
class BufferCache {
public:
    BufferCache();
    ~BufferCache();

    void FreeAllRes();

    // 设置缓存项（外部分配buffer，并存入相关图片buffer的宽高等信息）
    bool SetSlot(const char *key, const BufferDesc *info);

    // 获取缓存项的信息
    bool GetSlot(const char *key, BufferDesc *info);

    // 释放所有被标记为“当前页使用”的缓存项
    void ReleaseUsed();
    // 维测接口
    void DumpValidSlot() const;

private:
    struct CacheItem {
        char key[KEY_MAX_LEN + 1];  // 直接存储key，避免动态分配内存
        uint8_t *data;              // 外部分配的图像数据缓冲区
        FreeDataFunc freeFunc;      // 释放缓存区的方式
        uint32_t width;             // 图像宽度
        uint32_t height;            // 图像高度
        uint8_t bytesPerPixel;      // 每像素字节数
        bool used;                  // 当前帧是否使用
        bool valid;                 // 是否包含有效数据
    };

    CacheItem m_pool[CACHE_MAX_ITEMS];

    int32_t FindValidSlot(const char *key) const;
    // 如果不存在会老化不用的数据
    int32_t FindFreeSlot();
};
}  // namespace OHOS

#endif  // BUFFER_CACHE_MANAGER_H