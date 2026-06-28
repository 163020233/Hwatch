/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SlicePageFactory
 * Create: 2025-11
 */

#ifndef SLICE_PAGE_FACTORY_H
#define SLICE_PAGE_FACTORY_H
#include <unordered_map>
#include "SlicePage.h"

namespace OHOS {

class SlicePageProxyInterface : public HeapBase {
public:
    virtual SlicePageBase* CreateSlicePage(uint16_t pageId) = 0;
};

class SlicePageFactory : public HeapBase {
public:
    static SlicePageFactory& GetInstance()
    {
        static SlicePageFactory instance;
        return instance;
    }

    bool IsSlicePageExist(uint16_t sliceId, uint16_t pageId);

    SlicePageBase* CreateSlicePage(uint16_t sliceId, uint16_t pageId);

    SlicePageBase* GetDefaultSlicePage(uint16_t sliceId, uint16_t& defaultPageId);

    void RegisterPage(uint16_t sliceId, uint16_t pageId, SlicePageProxyInterface* proxy, bool isMainPage);

    void SetMainPage(uint16_t sliceId, uint16_t pageId);

private:
    SlicePageFactory() {};
    ~SlicePageFactory() {};
    SlicePageFactory(const SlicePageFactory &) = delete;
    SlicePageFactory &operator=(const SlicePageFactory &) = delete;
    std::unordered_map<uint32_t, SlicePageProxyInterface*> pageProxyMap_;
    std::unordered_map<uint16_t, SlicePageProxyInterface*> mainPageMap_;
    std::unordered_map<uint16_t, uint16_t> mainPageIdMap_;
};


class SlicePageRegister : public HeapBase {
public:
    SlicePageRegister(uint16_t sliceId, uint16_t pageId, SlicePageProxyInterface* proxy, bool isMainPage);
};

template<class V>
class SlicePageProxy : public SlicePageProxyInterface {
public:
    SlicePageBase* CreateSlicePage(uint16_t pageId) override
    {
        V* v = new V();
        SlicePageBase* page = dynamic_cast<SlicePageBase*>(v);
        if (page == nullptr) {
            delete v;
        } else {
            page->pageId_ = pageId;
        }
        return page;
    }
};
}

#define REGIST_SLICE_PAGE(sliceId, pageId, V, isMainPage) \
    const static OHOS::SlicePageRegister (STATIC_VAR##sliceId)(sliceId, pageId, new OHOS::SlicePageProxy<V>(), isMainPage)
#endif