#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ace_common")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/interfaces/innerkits
    ${OHOS_DIR}/drivers/peripheral/display/interfaces/include
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/common/log
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/common/memory
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/common/memory/cache
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/common/utils
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/interfaces/inner_api/builtin/base
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/kits
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/targets/liteos_m
    ${ROOT_DIR}/middleware/services/gui/uikit/config/wearable
    ${ROOT_DIR}/middleware/services/gui/hal/common/include
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/wearable/interfaces/kits
    ${ROOT_DIR}/middleware/services/gui/uikit/utils/wearable/interfaces/innerkits
    ${OHOS_DIR}/third_party/freetype/include
    ${OHOS_DIR}/base/hiviewdfx/hiview_lite
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/interfaces/kits/samgr
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/kits/hilog_lite
    ${OHOS_DIR}/base/hiviewdfx/hievent_lite/interfaces/native/innerkits
)

set(SOURCES
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/common/log/ace_log.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/common/memory/ace_mem_base.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/common/memory/cache/cache_manager.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/common/memory/mem_proc.cpp
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/frameworks/common/memory/memory_heap.cpp
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(COMPONENT_CXXCFLAGS
    -std=c++11
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

