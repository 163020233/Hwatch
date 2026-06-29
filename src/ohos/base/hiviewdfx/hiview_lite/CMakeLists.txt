#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hiview_lite")

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
    ${OHOS_DIR}/base/hiviewdfx/hiview_lite/hiview_cache.c
    ${OHOS_DIR}/base/hiviewdfx/hiview_lite/hiview_config.c
    ${OHOS_DIR}/base/hiviewdfx/hiview_lite/hiview_file.c
    ${OHOS_DIR}/base/hiviewdfx/hiview_lite/hiview_service.c
    ${OHOS_DIR}/base/hiviewdfx/hiview_lite/hiview_util.c
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-typedef-redefinition
    -Wno-error=strict-prototypes
)

set(COMPONENT_CXXCFLAGS
)

set(PUBLIC_DEFINES
    HIVIEW_FILE_DIR=""
)

set(PRIVATE_DEFINES
    OUTPUT_OPTION=1
    HILOG_LITE_SWITCH=1
    DUMP_LITE_SWITCH=0
    HIEVENT_LITE_SWITCH=1
    LOG_OUTPUT_MODULE=-1
    HIVIEW_STACK_SIZE=4096
    OUTPUT_LEVEL=1
    CHIP_VER_Hi3321
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

