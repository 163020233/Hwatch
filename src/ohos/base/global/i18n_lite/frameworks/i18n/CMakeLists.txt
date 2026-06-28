#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "i18n")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/base/global/i18n_lite/interfaces/kits/i18n/include
    ${OHOS_DIR}/base/global/i18n_lite/frameworks/i18n/include
)

set(SOURCES
    ${OHOS_DIR}/base/global/i18n_lite/frameworks/i18n/src/data_resource.cpp
    ${OHOS_DIR}/base/global/i18n_lite/frameworks/i18n/src/date_time_data.cpp
    ${OHOS_DIR}/base/global/i18n_lite/frameworks/i18n/src/date_time_format.cpp
    ${OHOS_DIR}/base/global/i18n_lite/frameworks/i18n/src/date_time_format_impl.cpp
    ${OHOS_DIR}/base/global/i18n_lite/frameworks/i18n/src/locale_info.cpp
    ${OHOS_DIR}/base/global/i18n_lite/frameworks/i18n/src/measure_format.cpp
    ${OHOS_DIR}/base/global/i18n_lite/frameworks/i18n/src/measure_format_impl.cpp
    ${OHOS_DIR}/base/global/i18n_lite/frameworks/i18n/src/number_data.cpp
    ${OHOS_DIR}/base/global/i18n_lite/frameworks/i18n/src/number_format.cpp
    ${OHOS_DIR}/base/global/i18n_lite/frameworks/i18n/src/number_format_impl.cpp
    ${OHOS_DIR}/base/global/i18n_lite/frameworks/i18n/src/plural_format.cpp
    ${OHOS_DIR}/base/global/i18n_lite/frameworks/i18n/src/plural_format_impl.cpp
    ${OHOS_DIR}/base/global/i18n_lite/frameworks/i18n/src/plural_rules.cpp
    ${OHOS_DIR}/base/global/i18n_lite/frameworks/i18n/src/str_util.cpp
    ${OHOS_DIR}/base/global/i18n_lite/frameworks/i18n/src/week_info.cpp
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-missing-field-initializers
    -Wno-error=sign-compare
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

