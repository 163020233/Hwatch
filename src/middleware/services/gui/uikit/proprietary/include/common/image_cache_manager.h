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

/**
 * @addtogroup UI_Common
 * @{
 *
 * @brief Defines common UI capabilities, such as image and text processing.
 *
 */

/**
 * @file image_cache_manager.h
 *
 * @brief Declares the <b>ImageCacheManager</b> class of the graphics module, which provides functions for image load
 * and cache manager.
 *
 */

#ifndef IMAGE_CACHE_MANAGER_H
#define IMAGE_CACHE_MANAGER_H
#include <string>
#include <cstdint>
#include <unordered_map>
#include "securec.h"
#include "gfx_utils/image_info.h"
#include "gfx_utils/heap_base.h"
#include "gfx_utils/vector.h"
#include "graphic_config.h"
#include "hals/gralloc_engines.h"
#include "graphic_mutex.h"

#ifdef ERROR
#undef ERROR
#endif

#if ENABLE_IMAGE_PACKER
namespace OHOS {
/**
 * @brief Declares the <b>ImageCacheManager</b> class of the graphics module.
 *        This class provides functions for image load and cache manager.
 *
 */
class ImageCacheManager : public HeapBase {
public:
    /**
     * @brief Get <b>ImageCacheManager</b> instance.
     */
    static ImageCacheManager& GetInstance();
    /**
     * @brief Load all images from a packed resource file. And then use LoadOneInMultiRes to get the actual image.
     * @param file: the path of the image binary.
     *        userFP: file pointer(witch opend outside), if userFP is not nullptr, we will use it firstly.
     *        isLongTerm: whether the image should be kept in long term.
     *        offset: the offset of the multi image start point.
     */
    bool LoadAllInMultiRes(const std::string& file, FILE* userFP = nullptr, bool isLongTerm = false, int offset = 0);

    /**
     * @brief Load Specified Number of images from a packed resource file. And then use LoadOneInMultiRes to get the actual image.
     * @param file: the path of the image binary.
     *        startResId: the resId of start image
     *        num: the number of image to load
     *        userFP: file pointer(witch opend outside), if userFP is not nullptr, we will use it firstly.
     *        isLongTerm: whether the image should be kept in long term.
     *        offset: the offset of the multi image start point.
     */
    bool LoadPartialInMultiRes(const std::string& file, uint32_t startResId, uint32_t num, FILE* userFP = nullptr,
        bool isLongTerm = false, uint32_t offset = 0);

    /**
     * @brief Load all at a time from input buffer. And then use LoadOneInMultiRes to get the actual image.
     * @param buf: address of the image binary in the memory.
     */
    bool LoadAllInMultiRes(uint8_t* buf);

    /**
     * @brief Load Specified Number of images from input buffer. And then use LoadOneInMultiRes to get the actual image.
     * @param buf: address of the image binary in the memory.
     *        startResId: the resId of start image
     *        num: the number of image to load
     */
    bool LoadPartialInMultiRes(uint8_t* buf, uint32_t startResId, uint32_t num);

    /**
     * @brief Load one image (appoint by resId). If you has loaded the image before,
     *        then it will return the buffer directly .
     * @param resId: the actual image's resource id.
     *        others: the same with LoadAllInMultiRes().
     */
    ImageInfo* LoadOneInMultiRes(uint32_t resId, const std::string& file, FILE* userFP = nullptr,
        bool isLongTerm = false, int offset = 0);
    ImageInfo* LoadOneInMultiRes(uint8_t* buf, uint32_t resId);

    /**
     * @brief Unload the target image, witch is appoint by resId.
     */
    bool UnloadOneInMultiRes(uint32_t resId, const std::string& file);
    bool UnloadOneInMultiRes(uint8_t* buf, uint32_t resId);

    /**
     * @brief Unload multiple target images, which is appoint by resIdArray
     * @param resIdArray: the Array of resId.
     * @param arrayLen: the length of param resIdArray
     * @param file: name of file to unload
     */
    bool UnloadPartialInMultiRes(const uint32_t* resIdArray, uint32_t arrayLen, const std::string& file);

    /**
     * @brief Unload multiple target images, which is appoint by resIdArray
     * @param buf: buf to unload.
     * @param resIdArray: the Array of resId.
     * @param arrayLen: the length of param resIdArray
     */
    bool UnloadPartialInMultiRes(uint8_t* buf, const uint32_t* resIdArray, uint32_t arrayLen);

    /**
     * @brief Unload all images of the file/buf.
     */
    bool UnloadAllInMultiRes(const std::string& file);
    bool UnloadAllInMultiRes(uint8_t* buf);

    /**
     * @brief Load image for single res file (only has one image in the binary).
     */
    ImageInfo* LoadSingleRes(const std::string& file, bool isLongTerm = false);

    /**
     * @brief Unload image for single res file.
     */
    bool UnloadSingleRes(const std::string& file);

    /**
     * @brief Check if image is released, if it is, we need to reload it. Otherwise, update the usage info for it.
     */
    void UpdateImageInfoIfNecessary(ImageInfo& info);

    bool TryToFreeImage();

    /**
     * @brief Dump image information.
     */
    void Dump();

    void EnterAod();
    void ExitAod();
    bool IsInAod() const
    {
        return isInAod_;
    }

private:
    struct ImageNode;
    struct MultiImageInfo {
        uint32_t dirId = 0;
        uint32_t count = 0;
        uint32_t loadedCount = 0;
        uint32_t offset = 0;
        bool isPath = false;
        bool isSingle = false;
        std::string file;
        ImageNode** node = nullptr;
    };

    struct ImageNode {
        bool isLongTerm;
        bool isLoaded;
        uint32_t resId;
        ImageInfo info;
        ImageNode* prev;
        ImageNode* next;
        MultiImageInfo* multiImgInfo;
    };

    struct BinHeader {
        uint32_t dirId;
        uint32_t count;
    };

    struct ImgRange {
        uint32_t startIndex;
        uint32_t num;
    };

    std::unordered_map<std::string, MultiImageInfo*> pathMap_;
    std::unordered_map<uint8_t*, MultiImageInfo*> bufMap_;
    ImageNode longTermList_;
    ImageNode shortTermList_;
    GraphicMutex listMutex_;

    ImageCacheManager()
    {
        memset_s(&longTermList_, sizeof(longTermList_), 0, sizeof(longTermList_));
        memset_s(&shortTermList_, sizeof(shortTermList_), 0, sizeof(shortTermList_));
        longTermList_.prev = &longTermList_;
        longTermList_.next = &longTermList_;
        shortTermList_.prev = &shortTermList_;
        shortTermList_.next = &shortTermList_;
    }
    ~ImageCacheManager() {}
    bool InitMultiImgInfo(BinHeader& header, MultiImageInfo*& multiImgInfo);
    bool LoadAllImgInfo(FILE* fp, MultiImageInfo& multiImgInfo, int offset, bool longTermFlag);
    void FreeInfo(MultiImageInfo& multiImgInfo);
    ImageInfo* LoadSingleImgInfo(FILE* fp, MultiImageInfo& multiImgInfo, uint32_t imgIndex, int offset,
        bool isLongTerm);
    bool LoadImgInfoInRange(FILE* fp, MultiImageInfo& multiImgInfo, const ImgRange& range,
        uint32_t offset, bool isLongTerm, const std::string& file);
    bool LoadImgInfoInRange(uint8_t* buf, MultiImageInfo& multiImgInfo, const ImgRange& range);
    bool LoadImgInfoInRangeWithNewMultiInfo(const std::string& file, FILE* fp,
        uint32_t startResId, uint32_t num, uint32_t offset, bool isLongTerm);
    bool LoadImgInfoInRangeWithRecoredMultiInfo(const std::string& file, FILE* fp,
        MultiImageInfo* multiImgInfo, uint32_t startResId, uint32_t num, uint32_t offset, bool isLongTerm);
    void ProcessLoadRange(Graphic::Vector<ImgRange>& loadRangeVec, Graphic::Vector<ImgRange>& updateRangeVec,
        MultiImageInfo* multiImgInfo, uint32_t startIndex, uint32_t num, bool isBuffer);
    bool InitializeImageNode(MultiImageInfo& multiImgInfo, uint32_t imgIndex, bool isLongTerm,
        uint32_t dataSize, bool& isNewNode);
    void UpdateNodeInRange(const ImgRange& range, MultiImageInfo& multiImgInfo);
    void CancelNodeInRange(const ImgRange &range, MultiImageInfo &multiImgInfo);
    void RollbackLoadedNode(Graphic::Vector<ImgRange>& imgRangeVec, uint32_t nowIndex,
        MultiImageInfo& multiImgInfo);
    ImageInfo* LoadOneImgFromBuf(uint8_t* buf, MultiImageInfo& multiImgInfo, uint32_t imgIndex);
    ImageCacheManager::ImageNode* CreateImageNode(MultiImageInfo* multiImgInfo, uint32_t size, bool isLongTerm,
        uint32_t imgId);
    void DestoryImageNode(ImageNode*& node);

    void ListAdd(ImageNode* head, ImageNode* node)
    {
        head->next->prev = node;
        node->next = head->next;
        node->prev = head;
        head->next = node;
    }

    void ListRemove(ImageNode* node)
    {
        node->next->prev = node->prev;
        node->prev->next = node->next;
    }

    void ListUpdate(ImageNode* node)
    {
        ImageNode* head = node->isLongTerm ? &longTermList_ : &shortTermList_;
        ListRemove(node);
        ListAdd(head, node);
    }

    void CancelNode(ImageNode* node, bool isNewNode)
    {
        if (isNewNode) {
            DestoryImageNode(node);
            return;
        }
        ImageCacheFree(node->info);
        if (node->multiImgInfo->isPath) {
            listMutex_.Lock();
            ListRemove(node);
            listMutex_.Unlock();
        }
    }

    void RecordAodFile(const std::string &file);
    bool IsRecordedAodFile(const std::string &file);
    Graphic::Vector<std::string> recordedAodFiles_;
    bool isInAod_ = false;
};
} // OHOS
#endif // ENABLE_IMAGE_PACKER
#endif // IMAGE_CACHE_MANAGER_H
/**
 * @}
 */
