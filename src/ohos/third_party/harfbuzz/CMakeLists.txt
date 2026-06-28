#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "harfbuzz")

set(PUBLIC_HEADER
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src
    ${OHOS_DIR}/third_party/harfbuzz/include
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src
)

set(SOURCES
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-aat-layout.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-aat-map.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-blob.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-buffer-serialize.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-buffer.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-common.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-face.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-fallback-shape.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-font.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-map.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-number.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-ot-cff1-table.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-ot-cff2-table.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-ot-face.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-ot-font.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-ot-layout.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-ot-map.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-ot-math.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-ot-meta.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-ot-metrics.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-ot-name.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-ot-shape-complex-arabic.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-ot-shape-complex-default.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-ot-shape-complex-hangul.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-ot-shape-complex-hebrew.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-ot-shape-complex-indic-table.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-ot-shape-complex-indic.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-ot-shape-complex-khmer.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-ot-shape-complex-myanmar.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-ot-shape-complex-syllabic.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-ot-shape-complex-thai.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-ot-shape-complex-use.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-ot-shape-complex-vowel-constraints.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-ot-shape-fallback.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-ot-shape-normalize.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-ot-shape.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-ot-tag.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-ot-var.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-set.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-shape-plan.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-shape.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-shaper.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-static.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-subset-cff2.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-subset-cff-common.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-ucd.cc
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src/hb-unicode.cc
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(COMPONENT_CXXCFLAGS
    -std=c++11
    -Wno-macro-redefined
    -Wno-unused-variable
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
    HB_TINY
    __STDC_VERSION__
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
install_sdk(${CMAKE_CURRENT_SOURCE_DIR} "*")
