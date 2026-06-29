/*
 * Copyright (c) @HiSilicon (Shanghai) Technologies Co., Ltd. 2024. All rights reserved.
 */

#ifndef UI_WATERFALL_LIST_H
#define UI_WATERFALL_LIST_H

#include "components/ui_view.h"
#include "components/ui_abstract_scroll.h"
#include "components/ui_scroll_view.h"
#include "gfx_utils/list.h"
#include "gfx_utils/vector.h"
#include "animator/animator.h"

namespace OHOS {
class UIWaterfallList : public UIScrollView, public AnimatorCallback {
public:
    UIWaterfallList();
    enum WaterfallListMode {
        NORMAL,
        GRID,
        GRID_FLOAT,
        NORMAL_TO_GRID,
        NORMAL_TO_GRID_FLOAT,
        GRID_TO_GRID_FLOAT,
        GRID_TO_NORMAL,
        ENTER,
    };

    ~UIWaterfallList() override {}

    /**
     * @brief Set origin image distance between the center of the two adjacent images
     *        imageDistance = oriImgDistance * globalScale
     *        globalScale: refer to function Scale
     *        LayoutChildren(true) must be invoked mannualy after invoking this method.
     *
     * @param distance Indicates image distance to set when the image in the center region
     */
    void SetImageDistance(uint16_t distance)
    {
        oriImgDistance_ = distance;
    }

    /**
     * @brief Set the size of Image or UITransformGroup.
     *        LayoutChildren(true) must be invoked mannualy after invoking this method.
     *
     * @param size Indicates size to set, when the Image or UITransformGroup is displayed in the center.
     *             It is best to keep child's size as same as this size.
     */
    void SetImageSizeInCenter(uint16_t size)
    {
        imgSizeInCenter_ = size;
    }

    /**
     * @brief Set origin image scale factor, scale strategy will follow the below rules
     *        if globalScale >= 1
     *           scaleFactor = oriScaleFactor
     *        elif globalScale > scaleThreshold
     *           scaleFactor = oriScaleFactor * (globalScale - scaleThreshold) / (1 - scaleThrethold)
     *        else
     *           scaleFactor = 0
     *        img size is inversely proportional to distance * scaleFactor
     *
     *        oriScaleFactor: refer to function SetScaleFactorByDistance
     *        scaleThreshold: refer to function SetScaleThesholdOfZeroFactor
     *        globalScale: refer to function Scale
     *
     * @param factor Indicates scale factor to set, which will decide scale value together with distance
     */
    void SetScaleFactorByDistance(float factor)
    {
        oriScaleFactor_ = factor;
    }

    float GetScaleFactorByDistance()
    {
        return oriScaleFactor_;
    }

    /**
     * @brief Set scale threshold, scale factor will decrease to 0 if global scale equal and less than scaleThreshold,
     *        default value is 0.6, more information can  refer to method SetScaleFactorByDistance
     *
     * @param threshold Indicates factor to set, the scale factor when the image is not in the center
     */
    void SetScaleThesholdOfZeroFactor(float threshold)
    {
        scaleThresholdOfZeroFactor_ = threshold;
    }

    float GetScaleThesholdOfZeroFactor()
    {
        return scaleThresholdOfZeroFactor_;
    }

    /**
     * @brief Adds UITransformGroup or UIImageView whose resize mode will set to be UIImageView::COVER.
     *        LayoutChildren(true) should be invoked mannualy after invoking this method.
     *        Note: View index was set for children inner in order of addition, so you can
     *        get its index by call method GetViewIndex()
     *
     * @param view Indicates the pointer to the child view to add.
     */
    void Add(UIView* view) override;

    /**
     * @brief Inserts UITransformGroup or UIImageView whose resize mode will set to be UIImageView::COVER.
     *        LayoutChildren(true) should be invoked mannualy after invoking this method.
     *        Note: View index was set for children inner in order of addition, so you can
     *        get its index by call method GetViewIndex()
     *
     * @param prevView Indicates the pointer to the child view, previous to the new child view.
     * @param view Indicates the pointer to the new child view to insert.
     */
    void Insert(UIView* prevView, UIView* insertView) override;

    bool OnDragEvent(const DragEvent& event) override;

    /**
     * @brief Layout all children views according to the Waterfall shape, it must be invoked after method:
     *        1. Add(UIView* view)
     *        2. Insert(UIView* prevView, UIView* insertView)
     *        3. void SetImageSizeInCenter(uint16_t size)
     *        4. void SetImageDistance(uint16_t distance)
     *
     * @param needInvalidate Specifies whether to refresh the invalidated area after the layout is complete.
     *                       Value <b>true</b> means to refresh the invalidated area after the layout is complete,
     *                       and <b>false</b> means the opposite.
     */
    void LayoutChildren(bool needInvalidate = false) override;

    /**
     * @brief Moves all child views.
     *
     * @param x Indicates the offset distance by which this view group is moved on the x-axis.
     * @param y Indicates the offset distance by which this view group is moved on the y-axis.
     */
    void MoveChildByOffset(int16_t offsetX, int16_t offsetY) override;

    /**
     * @brief Set global scale, and image with centerIndex will be focused
     *
     * @param scale Indicates global scale value both for x- and y- axes.
     */
    void Scale(float scale, int16_t centerIndex = -1);

    float GetGlobalScale()
    {
        return globalScale_;
    }

    /**
     * @brief Get the Index Of View object in this view group
     *
     * @param  index, Indicates the view to search
     * @return Returns the view, nullptr if not exsit
     */
    UIView* GetViewByIndex(uint8_t index);

    /**
     * @brief Get index of focused image, default 0, changed by scale and scroll

     *        When start Scale(scale, index), scale center index will set to be focus index
     * @return Returns index of focused image
     */
    int8_t GetFocusedImgIndex()
    {
        return focusImgIndex_;
    }

    /**
     * @brief Get center position of the special(index) image
     *        Note: the position is not actual position, it is position in regular
     *        hexagon without futher adjustment
     *        which include adjusting the img zoom and distance in order to improve display experience
     *
     * @return Returns postion
     */
    Vector2<int16_t> GetCurrentPosition(uint8_t index, WaterfallListMode mode = NORMAL);

    /**
     * @brief Get the row where the initial grid position of index is
     *
     * @return Returns the row
     */
    int16_t GetInitRow(uint8_t index);

    /**
     * @brief Get the column where the initial grid position of index is
     *
     * @return Returns the row
     */
    int16_t GetInitCol(uint8_t index);

    /**
     * @brief Get the initial grid position of the special(index) image
     *        Note: the position is not actual position, it is position in regular waterfall without futher adjustment
     *        which include adjusting the img zoom and distance in order to improve display experience
     *
     * @return Returns postion
     */
    Vector2<int16_t> GetInitPosition(uint8_t index);

    UIViewType GetViewType() const override
    {
        return UI_WATERFALL_LIST;
    }

    void ReMeasure() override;

    /**
     * @brief Simulate the positions of different views during the entry process of waterfall within a period of time
     *        Note: the position is not actual position, it is position in regular waterfall without futher adjustment
     *        which include adjusting the img zoom and distance in order to improve display experience
     *
     * @param  curTime, current time
     * @param  totalTime, total time
     */
    void EnterMove(float curTime, float totalTime);

    /**
     * @brief Get the rebound distance, which is the external interface of CalculateReboundDistance
     *        Note: the position in caculating is not actual position, it is
     *        position in regular waterfall without futher adjustment
     *        which include adjusting the img zoom and distance in order to improve display experience
     */
    Vector2<int16_t> GetReboundDistance();

    /**
     * @brief Gets the current waterfall list mode.
     * @return Returns the current waterfall list mode.
     */
    WaterfallListMode GetMode()
    {
        return mode_;
    }

    /**
     * @brief Enables or disables the edit feature.
     * @param enable Whether to enable the edit feature, true to enable, false to disable.
     */
    void EnableEdit(bool enable);

    /**
     * @brief Checks if the edit feature is enabled.
     * @return Returns true if the edit feature is enabled, false otherwise.
     */
    bool IsEnableEdit()
    {
        return enableEdit_;
    }

    /**
     * @brief Sets the scale ratio of icon size in grid mode compared to the centered icon size in normal mode.
     * @param scale Scale ratio for icon size in grid mode.
     */
    void SetGridScale(float scale);

    /**
     * @brief Sets the auto-scroll speed when dragging an icon to the top/bottom edge.
     * @param speed Scroll speed value, ranging from 0 to 255.
     */
    void SetEdgeScrollSpeed(uint8_t speed);

    /**
     * @brief Switches from grid mode to normal mode.
     */
    void SwitchGridToNormal();

    void SetEnterAnimFlag(bool flag);

    /**
     * @brief Removes the specified item from UIWaterfallList.
     * @param view A pointer to the subview to be removed.
     */
    void Remove(UIView* view) override;

    /**
     * @brief  Mode change listener.
     */
    class OnModeChangeListener : public HeapBase {
    public:
        /**
         * @brief Called when WaterfallListMode is changed.
         * @param view Pointer to the UIWaterfallList object representing the view that triggered the mode change.
         * @param targetMode Enum value representing the target mode of WaterfallListMode.
         */
        virtual void OnModeChange(UIWaterfallList* view, UIWaterfallList::WaterfallListMode targetMode) = 0;
        virtual ~OnModeChangeListener() {}
    };

    /**
     * @brief Get the current mode change listener.
     * @return Returns a constant pointer to the OnModeChangeListener.
     */
    const OnModeChangeListener* GetOnModeChangeListener()
    {
        return onModeChangeListener_;
    }

    /**
     * @brief Set a new mode change listener.
     * @param onModeChangeListener Pointer to the new OnModeChangeListener to be set.
     */
    void SetOnModeChangeListener(OnModeChangeListener* onModeChangeListener)
    {
        onModeChangeListener_ = onModeChangeListener;
    }

protected:
    bool DragXInner(int16_t distance) override;
    bool DragYInner(int16_t distance) override;
    bool OnClickEvent(const ClickEvent& event) override;
    bool OnLongPressEvent(const LongPressEvent& event) override;
    bool OnDragEndEvent(const DragEvent& event) override;

    float globalScale_{1.0f};
    float scaleThresholdOfZeroFactor_{0.75f};

private:
    using UIScrollView::Scale;
    void InitParametersAndImgIndex();
    void CalculateAdjustDistanceForRectScreen(Vector2<float>& position, float& imgScaleRatio);
    void CalculateReboundDistance(int16_t& dragDistanceX, int16_t& dragDistanceY) override;
    void ScaleBaseElement(UIView* view, float scaleX, float scaleY);
    Graphic::Vector<Vector2<int16_t>> imgPoints_{64};
    float imgDistance_{0};
    uint16_t oriImgDistance_{0};
    float oriScaleFactor_{0.23f};
    int16_t imgSizeInCenter_{0};
    int16_t focusImgIndex_{-1};
    float oriImgRadius_{0.0f};
    float imgRadius_{0.0f};
    float layoutWidth_{0.0f};
    float layoutHeight_{0.0f};
    int16_t centerX_{0};
    int16_t centerY_{0};

    struct ItemLayoutInfo {
        int16_t x;
        int16_t y;
        float scale;
    };

    Graphic::Vector<Vector2<int16_t>> imgPointsInGrid_{64};
    float gridScale_{0.75f};
    WaterfallListMode mode_ = NORMAL;
    UIView* floatView_ = nullptr;
    Animator transitionAnim_;
    bool isSetIndex_ = false;
    bool isLongPressed_ = false;
    bool enableEdit_ = false;

    Point floatViewStartPoint_ = {0, 0};
    Point floatViewEndPoint_ = {0, 0};
    float floatViewStartScale_ = 1.0f;
    float floatViewEndScale_ = 1.0f;

    void LayoutChildrenInGrid(bool needInvalidate = false);
    void AdjustViewInNormal(UIView& view, int16_t index);
    void AdjustViewInGrid(UIView& view, int16_t index);
    void SwitchNormalToGrid();

    UIView* GetItemAtPos(const Point& pos, WaterfallListMode mode);
    UIView* GetItemAtPosInNormal(const Point& pos);
    UIView* GetItemAtPosInGrid(const Point& pos);
    bool GetLayoutInfoInNormal(int16_t index, ItemLayoutInfo& info, const Vector2<int16_t>& position);

    int16_t GetFloatIndex(int16_t x, int16_t y);
    void UpdateIndexInGrid(int16_t oldIndex, int16_t newIndex);
    void ReorderElementByFloatPos(int16_t x, int16_t y);

    void FloatAnimCb();
    void TransitionAnimCb(bool isNormalToGrid);

    void EnterMoveRow1Or2(int16_t curTime, int16_t totalTime, UIView* view, int16_t index, uint8_t rowNum);
    void EnterMoveRestRow(int16_t curTime, int16_t totalTime, UIView* view, int16_t index, int16_t iconR);
    void ApplyLayoutAndScale(UIView* view, int index, float extraScaleCoef = 1.0f);

    void ClearFloatView();

    virtual void OnStop(UIView& view) override;
    virtual void Callback(UIView* view) override;

    class EdgeScrollCallback : public AnimatorCallback {
    public:
        EdgeScrollCallback() : scrollSpeed_(0) {}
        virtual ~EdgeScrollCallback() {}

        void SetScrollSpeed(int16_t speed)
        {
            scrollSpeed_ = speed;
        }

        int16_t GetScrollSpeed() const
        {
            return scrollSpeed_;
        }
        virtual void Callback(UIView* view) override;

    protected:
        int16_t scrollSpeed_ = 0;
    };

    Animator edgeScrollAnim_;
    EdgeScrollCallback edgeScrollCb_;
    uint8_t edgeScrollSpeed_ = 8;
    friend class EdgeScrollCallback;

    class ShakeAnimCallback : public AnimatorCallback {
    public:
        ShakeAnimCallback() {}
        virtual ~ShakeAnimCallback() {}

        void BindAnim(Animator* anim)
        {
            anim_ = anim;
        }
        virtual void Callback(UIView* view) override;
        virtual void OnStop(UIView& view) override;

    protected:
        void ShakeItem(UIView* view);
        Animator* anim_ = nullptr;
        uint32_t lastTime_ = 0;
    };

    Animator shakeAnim_;
    ShakeAnimCallback shakeAnimCb_;
    friend class ShakeAnimCallback;

    OnModeChangeListener* onModeChangeListener_ = nullptr;
    void SetModeInner(UIWaterfallList::WaterfallListMode mode);
};
}
#endif // UI_WATERFALL_LIST_H
