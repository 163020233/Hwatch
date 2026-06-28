#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "icu4c")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/third_party/icu/icu4c/source/common
)

set(SOURCES
    ${OHOS_DIR}/third_party/icu/icu4c/source/common/cmemory.cpp
    ${OHOS_DIR}/third_party/icu/icu4c/source/common/cstring.cpp
    ${OHOS_DIR}/third_party/icu/icu4c/source/common/ubidi.cpp
    ${OHOS_DIR}/third_party/icu/icu4c/source/common/ubidiln.cpp
    ${OHOS_DIR}/third_party/icu/icu4c/source/common/ubidiwrt.cpp
    ${OHOS_DIR}/third_party/icu/icu4c/source/common/ubidi_props.cpp
    ${OHOS_DIR}/third_party/icu/icu4c/source/common/uchar.cpp
    ${OHOS_DIR}/third_party/icu/icu4c/source/common/udataswp.cpp
    ${OHOS_DIR}/third_party/icu/icu4c/source/common/uinvchar.cpp
    ${OHOS_DIR}/third_party/icu/icu4c/source/common/ustring.cpp
    ${OHOS_DIR}/third_party/icu/icu4c/source/common/utf_impl.cpp
    ${OHOS_DIR}/third_party/icu/icu4c/source/common/utrie2.cpp
    ${OHOS_DIR}/third_party/icu/icu4c/source/common/ucptrie.cpp
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(COMPONENT_CXXCFLAGS
    -std=c++11
    -Wno-macro-redefined
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

