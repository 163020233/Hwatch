/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: DialBinParserV3
 * Author:
 * Create: 2025-02-06
 */

#ifndef GRAPHIC_LITE_DIAL_BIN_PARSER_V3_H
#define GRAPHIC_LITE_DIAL_BIN_PARSER_V3_H
#include "main/DialBinParserManager.h"
#include "main/dial/DialModelTime.h"
#include "components/ui_abstract_progress.h"
#include "main/dial/DialViewContainer.h"
#include "main/dial/DialDataType.h"
#include "main/dial/DialOHSliceId.h"

namespace OHOS {
class DialBinParserV3 : public DialBinParserInterface {
public:
    DialBinParserV3();
    ~DialBinParserV3() override;
    DialBinParserV3(const DialBinParserV3&) = delete;
    DialBinParserV3& operator=(const DialBinParserV3&) = delete;
    bool UpdateDial(const std::string& file, FILE* fp, DialViewGroup* group, DisplayState state) override;
    bool GetDialPreviewInfo(const std::string& file, FILE* fp, DialPreviewInfo* info) override;
    void SetEncryptData(uint8_t *data, uint32_t len) override final;

protected:
    bool ParseDialHeader(FILE* file);
    bool ParseDialStaticImg(ContainerInterface &parent);
    bool ParseDialSequenceImg(ContainerInterface &parent);
    bool ParseDialRotateImg(ContainerInterface &parent);
    bool ParseDialOptionImg(ContainerInterface &parent);
    bool ParseDialDigitalImg(ContainerInterface &parent);
    bool ParseDialLabel(ContainerInterface &parent);
    bool ParseDialArcLabel(ContainerInterface &parent);
    bool ParseDialBoxProgress(ContainerInterface &parent);
    bool ParseDialArcProgress(ContainerInterface &parent);
    bool ParseDialVideo(ContainerInterface &parent);
    bool ParseDialClick(ContainerInterface &parent);
    bool ParseDialKaleidoscope(ContainerInterface &parent);
    bool ParseDialContainer(ContainerInterface &parent);
    bool ParseDialOptions(ContainerInterface &parent);
    bool ParseDialLottie(ContainerInterface &parent);

protected:
    virtual bool IsTypeValid(uint8_t type);
    void* MallocAndReadBuf(FILE* file, uint32_t readSize);
    bool SetProgressType(FILE* fp, UIAbstractProgress* view, DialProgressType data);

    static constexpr uint8_t DIAL_BIN_PARSER_FUNC_NUM = 32; // 32: func max value
    bool (DialBinParserV3::*funcArray_[DIAL_BIN_PARSER_FUNC_NUM])(ContainerInterface&);
    void AddToParentWithView(ContainerInterface &parent, uint8_t displayMode, UIView *view);
    void AddToParentWithDialView(ContainerInterface &parent, uint8_t displayMode,
        DialView *view, DialDataType bindData, bool isPeriodic);
    struct {
        uint8_t *viewType;
        uint8_t dialViewCount;
        FILE *file;
        uint8_t curIndex;
    } curViewType_;
    bool ParseTimeIntervals(FILE* fp, uint8_t cnt, TimeChangeListener *listener);
    bool ParseViewType(uint8_t *viewType, uint8_t dialViewCount, FILE *file);
    bool ParseContainerSubviews(DialViewContainer *container, uint8_t num);
    bool ParseOptionsSubviews(ContainerInterface &parent);
    DialHeader *GetHeader(FILE *fp);
    uint8_t *GetVeiwType(FILE *fp, DialHeader *dialHeader);

    DialViewGroup* dialGroup_ = nullptr;
    std::string file_;
    const uint32_t dirId_ = 0xfef << 20; // 20: offset
    uint32_t imageOffset_ = 0;
    DisplayState state_ = DisplayState::NORMAL;
    uint8_t *encryptedData_ = nullptr;
    uint32_t encrpytedLen_ = 0;

    class ScopePtr {
    public:
        explicit ScopePtr(void* ptr) : ptr_(ptr) {}
        ~ScopePtr()
        {
            if (ptr_ != nullptr) {
                UIFree(ptr_);
            }
        }
        ScopePtr& operator=(const ScopePtr&) = delete;
        ScopePtr(const ScopePtr&) = delete;

    private:
        void *ptr_;
    };

    class ScopeConditional {
    public:
        explicit ScopeConditional(UIView *view, bool &isSuccess) : view_(view), isSuccess_(isSuccess) {}
        ~ScopeConditional()
        {
            if (!isSuccess_ && view_ != nullptr) {
                delete view_;
            }
        }
        ScopeConditional(const ScopeConditional&) = delete;
        ScopeConditional& operator=(const ScopeConditional&) = delete;

    private:
        UIView *view_ = nullptr;
        bool &isSuccess_;
    };
}; // class DialBinParserV3
} // namespace OHOS

#endif // GRAPHIC_LITE_DIAL_BIN_PARSER_V3_H