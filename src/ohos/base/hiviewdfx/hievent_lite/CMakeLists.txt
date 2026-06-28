#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hievent_lite")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/base/hiviewdfx/hievent_lite/interfaces/native/innerkits
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/kits/hilog_lite
    ${OHOS_DIR}/base/hiviewdfx/hiview_lite
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/interfaces/kits/samgr
)

set(SOURCES
    ${OHOS_DIR}/base/hiviewdfx/hievent_lite/frameworks/hiview_event.c
    ${OHOS_DIR}/base/hiviewdfx/hievent_lite/frameworks/hiview_output_event.c
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(COMPONENT_CXXCFLAGS
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
    FAULT_EVENT_FILE_SIZE=1024
    UE_EVENT_FILE_SIZE=1024
    STAT_EVENT_FILE_SIZE=1024
    EVENT_CACHE_SIZE=256
    HIVIEW_HIEVENT_FILE_BUF_SIZE=128
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

