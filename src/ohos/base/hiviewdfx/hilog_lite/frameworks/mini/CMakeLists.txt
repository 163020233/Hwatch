#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hilog_lite")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/base/hiviewdfx/hiview_lite
    ${OHOS_DIR}/foundation/systemabilitymgr/samgr_lite/interfaces/kits/samgr
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/kits/hilog_lite
    ${OHOS_DIR}/base/hiviewdfx/hievent_lite/interfaces/native/innerkits
)

set(SOURCES
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/frameworks/mini/hiview_log.c
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/frameworks/mini/hiview_log_limit.c
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/frameworks/mini/hiview_output_log.c
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(COMPONENT_CXXCFLAGS
)

set(PUBLIC_DEFINES
    HILOG_COMPILE_LEVEL=2
)

set(PRIVATE_DEFINES
    HIVIEW_LOG_FILE_SIZE=8192
    LOG_LIMIT_DEFAULT=30
    LOG_STATIC_CACHE_SIZE=1024
    HIVIEW_HILOG_FILE_BUF_SIZE=512
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

