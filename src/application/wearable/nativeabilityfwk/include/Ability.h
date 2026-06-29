/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Ability
 * Create: 2025-11
 */

#ifndef NATIVEABILITYFWK_ABILITY_H
#define NATIVEABILITYFWK_ABILITY_H
#include <cstdint>
#include "wearable_log.h"
#include "TransitionType.h"
#include "soc_osal.h"
#include "gfx_utils/vector.h"
#include "graphic_mutex.h"
#include "Slice.h"
#include "SliceProxy.h"
#include "SlicePageManager.h"
#include "RotationManager.h"

namespace OHOS {
const static uint16_t gSliceInvalid = 0;
const static uint16_t gPageInvalid = 0;
const static uint16_t gSliceDefaultPriority = 4;  // 页面默认是最低优先级

struct ExtralConfig {
    uint16_t pageId;
    TransitionType type;
    bool enableSlideBack;
    bool enableCache;
};

// 定义默认配置
constexpr ExtralConfig DEFAULT_CONF = {gPageInvalid, TransitionType::TRANSITION_INVALID, false, false};

class BaseAbility : public OnRotationChangedListener {
public:
    void AddSliceProxy(uint16_t sliceId, SliceProxy* proxy);

    /**
     * @brief Switch to specified page in the current slice, target page will be created, and current slice will
     *         not be recreated. if slices or pages is being switched, this operation will be canceled
     * @param pageId Indicates the specified id of the page in the current slice, it should not be zero
     * @param type Indicates Transition animation type, which is declared in TransitionType.h. The matched
     *        Transitioncallback should have params: enableCurrentSnapshot_ = false enableTargetSnapshot_ = false
     * @param canBack Indicates whether current page will be cached or destroyed. if cached, can back from target page
     * @param data Indicates the data which will be transfered to next page, default value is nullptr,
     *        data can only be native C structures or basic data types
     * @param dataLength Indicates the length of data
     * @return Returns <b>true</b> if the pages is switched successfully, returns <b>false</b> otherwise.
     * @note It is recommended to call this method in the UI thread, and the event will be post into graphic event queue
     */
    bool SwitchPageInSlice(uint16_t pageId, TransitionType type = TransitionType::TRANSITION_INVALID,
                           bool canBack = false, void* data = nullptr, uint16_t dataLength = 0);

    /**
     * @brief Navigates back to the previously cached page in the current slice.
     *        The current page will be destroyed, and the previous cached page will be resumed.
     *        If slices or pages are being switched, this operation will be canceled.
     * @param type Transition type, defaults to TransitionType::TRANSITION_INVALID.
     * @return Returns <b>true</b> if back to previously cached page successfully, returns <b>false</b> otherwise.
     * @note It is recommended to call this method in the UI thread.
     *       Otherwise, the event will be posted into the graphics event queue.
     */
    bool BackToPrevSlicePage(TransitionType type = TransitionType::TRANSITION_INVALID);

    /**
     * @brief Navigates back to the target cached page in the current slice.
     *        The current page and the cached pages on the top of target cached page will be destroyed.
     *        The target cached page will be resumed.
     *        If slices or pages are being switched, this operation will be canceled.
     * @param pageId Indicates the target page id in cached pages, it should not be zero.
     * @return Returns <b>true</b> if back to target cached page successfully, returns <b>false</b> otherwise.
     * @note It is recommended to call this method in the UI thread.
     *       Otherwise, the event will be posted into the graphics event queue.
     */
    bool BackToCachedSlicePage(uint16_t pageId, TransitionType type = TransitionType::TRANSITION_INVALID);

    /**
     * @brief Switch to specified slice. matched slice will be created.
     *         1. If pageId is valid or default page exists, matched page will also be created
     *         2. If slices is being switched, this operation will be cached and run later.
     * @param sliceId Indicates the specified id of the slice
     * @param pageId Indicates the specified id of the slice, if pageId is invalid, default page will be used.
     * @param type Indicates Transition animation type, which is declared in TransitionType.h
     * @param enableSlideBack Indicates whether can return to current slice from target slice by sliding
     */
    void SwitchSlice(uint16_t sliceId, uint16_t pageId = gPageInvalid,
        TransitionType type = TransitionType::TRANSITION_INVALID, bool enableSlideBack = false);

    /**
     * @brief Switch to specified slice, but pre slice don't destory. Matched slice will be created.
     *         1. If pageId is valid or default page exists, matched page will also be created
     *         2. If slices is being switched, this operation will be cached and run later.
     * @param sliceId Indicates the specified id of the slice
     * @param pageId Indicates the specified id of the slice, if pageId is invalid, default page will be used.
     * @param type Indicates Transition animation type, which is declared in TransitionType.h
     * @param enableSlideBack Indicates whether can return to current slice from target slice by sliding
     */
    void SwitchSliceWithCache(uint16_t sliceId, uint16_t pageId = gPageInvalid,
        TransitionType type = TransitionType::TRANSITION_INVALID, bool enableSlideBack = false);

    /**
     * @brief Switch to specified slice. matched slice will be created.
     *         1. If pageId is valid or default page exists, matched page will also be created
     *         2. If slices is being switched, this operation will be cached and run later.
     * @param config Indicates the extern config of the slice.
     */
    void SwitchSliceWithData(uint16_t sliceId, void* data, uint16_t dataLength,
        const ExtralConfig& config = DEFAULT_CONF);

    /**
     * @brief Switch to specified slice. matched slice will be created.
     *         1. If default page is exist, default page will also be created
     *         2. If slices is being switched, this operation will be cached and run later.
     *        Note: Method SwitchSlice is recommended, this method is reserved for compatibility.
     * @param sliceId Indicates the specified id of the slice
     * @param type Indicates Transition animation type, which is declared in TransitionType.h
     * @param priority Indicates the priority of slice, default value is used currently. if needed, config it in
     *        XXXPresenter::OnResume using method SetCurSlicePriority
     * @param enableSlideBack Indicates whether can return to current slice from target slice by sliding
     */
    void ChangeSlice(uint16_t sliceId, TransitionType type = TransitionType::TRANSITION_INVALID,
        uint16_t priority = gSliceDefaultPriority, bool enableSlideBack = false);

    void ChangeSliceToApplist();

    /**
     * @brief Get SlicePageManager of currrent slice. you can Use SlicePageManager to get page info in the slice.
     * @return Returns <b>nullptr</b> if slices is being switched, or js ability is running
     */
    SlicePageManager* GetCurSlicePageManager();

    void SetDefaultSliceId(uint16_t defaultSliceId, uint16_t pageId = gPageInvalid)
    {
        defaultTargetId_ = defaultSliceId | (static_cast<uint32_t>(pageId) << PAGE_OFFSET);
    }

    /**
     * @brief Get current slice id
     * @return Returns <b>0</b> if js ability is running, detail refer to comments of variable allSlices_
     */
    uint16_t GetCurSliceId(void)
    {
        if (allSlices_.IsEmpty()) {
            return gSliceInvalid;
        }
        return static_cast<uint16_t>(allSlices_[allSlices_.Size() - 1] & SLICE_MASK);
    }

    /**
     * @brief Get prevoius slice id in the slice vector, detail refer to comments of variable allSlices_
     */
    uint16_t GetPreSliceId(void)
    {
        if (allSlices_.Size() < 2) { // 2, minimum count if contains previous target
            return gSliceInvalid;
        }
        return static_cast<uint16_t>(allSlices_[allSlices_.Size() - 2] & SLICE_MASK); // 2 ：换算
    }

    void UpdateCurSlicePageId(uint16_t pageId);

    uint32_t GetCurTargetId(void)
    {
        if (allSlices_.IsEmpty()) {
            return gSliceInvalid;
        }

        return allSlices_[allSlices_.Size() - 1];
    }

    uint32_t GetPreTargetId(void)
    {
        if (allSlices_.Size() < 2) { // 2, minimum count if contains previous target
            return gSliceInvalid;
        }

        return allSlices_[allSlices_.Size() - 2]; // 2, index offset
    }

    void SetCurSlicePriority(uint16_t priority)
    {
        curSlicePriority_ = priority;
    }

    uint16_t GetCurSlicePriority(void)
    {
        return curSlicePriority_;
    }

    void ResumeSlice();
    void PauseSlice(PauseReason reason = PauseReason::CHANGE_SLICE);
    void StopSliceTransition();

    /**
    * @brief Navigates back to the previous slice(a composition of Slice and Page) in stack.
    * @param type The transition type, defaults to TRANSITION_INVALID.
    */
    void BackToPreSlice(TransitionType type = TransitionType::TRANSITION_INVALID);

    /**
    * @brief Return to a slice(a composition of Slice and Page) in stack. Other on the top will be popped.
    * @param targetId A composition of SliceId and PageId to be returned.
    * @param type Transition type, defaults to TRANSITION_INVALID.
    */
    void BackToSliceInStack(uint32_t targetId, TransitionType type = TransitionType::TRANSITION_INVALID);

    void EnterAod();
    void ExitAod(uint32_t targetId);

    /**
    * @brief Check if the specified targetId(a composition of SliceId and PageId) in stack, and thus could be returned.
    * @param targetId A composition of SliceId and PageId to check
    * @return True if the slice in stack, false otherwise.
    */
    bool IsSliceInStack(uint32_t targetId);

    /**
    * @brief Check if the specified page is cached in the current slice.
    * @param pageId The ID of the page to be checked.
    * @return Returns true if the page is cached in the current slice; otherwise, returns false.
    */
    bool IsCachedSlicePage(uint16_t pageId);

    /**
    * @brief Get previous slice ID before a transition.
    * @return uint16_t Returns previous slice ID before the transition.
    *         If no valid ID exists, it returns the invalid value gSliceInvalid.
    */
    uint16_t GetTransitionPrevSliceId(void)
    {
        if (transitionPrevTargetId_ == gSliceInvalid) {
            return gSliceInvalid;
        }
        return (transitionPrevTargetId_ & SLICE_MASK);
    }

    /**
    * @brief Get the prev target ID before transition. This ID is a composition of SliceId and PageId.
    * @return uint32_t Returns the prev Target ID before transition.
    */
    uint32_t GetTransitionPrevTargetId(void)
    {
        return transitionPrevTargetId_;
    }

    void OnRotationChanged() override;

    bool IsAodSlice(uint32_t targetId) const;

protected:
    // targetId consists of slice id and page id
    void StartSlice(uint32_t targetId, void* data = nullptr);
    void ResumeSliceWithoutJS(bool needCacheSlice);
    void StopSlice();
    BaseAbility() = default;
    virtual ~BaseAbility() = default;
    void ClearCacheSlice();
    void UpdateCacheSlice();
    struct ProxyStruct {
        uint16_t sliceId;
        SliceProxy* proxy;
    };

    struct SliceStruct {
        uint16_t sliceId = 0;
        Slice* slice = nullptr;
    };

    // 预加载的所有slices
    Graphic::Vector<ProxyStruct> allSlicesProxy_{16};

    /** Processing rule of allSlices_, its item value consists of slice and page. slice | page << 16.
     *    1. When switch slice A to main slice, clear slice vector, and then push back main slice into slice vector
     *    2. When switch slice A to slice B (not main), push back slice B into slice vector
     *    3. When switch js to slice A, clear slice vector, and then push back slice A into slice vector
     *    4. When switch slice A to js, push back 0 into slice vector. You Can get slice A by method GetPreSliceId()
     */
    Graphic::Vector<uint32_t> allSlices_;

    SliceStruct prevSlice_;
    SliceStruct curSlice_;
    SliceStruct nextSlice_;
    SliceStruct prevAodSlice_;
    uint32_t targetIdFromJS_ = 0;
    uint32_t defaultTargetId_ = gSliceInvalid;
    uint16_t curSlicePriority_ = gSliceDefaultPriority;
    void PostChangeSlice(uint32_t targetId, TransitionType type, void* data, bool needCacheSlice);
    BaseAbility::ProxyStruct* Find(const uint16_t sliceId);
    int16_t PopToTarget(uint32_t targetId);
    uint32_t transitionPrevTargetId_ = gSliceInvalid;
    bool isSameRotation_ = true;
};
}
#endif // NATIVEABILITYFWK_ABILITY_H
