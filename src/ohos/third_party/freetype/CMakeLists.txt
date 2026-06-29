#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "freetype_static")
add_compile_options(-D_FORTIFY_SOURCE=2 -O2)
add_compile_options(-ftrapv)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${PROJECT_BINARY_DIR}/third_party/freetype/freetype/include
    ${PROJECT_BINARY_DIR}/gen/third_party/freetype/freetype/include
    ${PROJECT_BINARY_DIR}/gen/third_party/freetype/freetype/src/base
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${PROJECT_BINARY_DIR}/gen/third_party/libpng/libpng-1.6.38
    ${OHOS_DIR}/third_party/zlib
)

set(SOURCES
    ${PROJECT_BINARY_DIR}/gen/third_party/freetype/freetype/src/base/ftbase.c
    ${PROJECT_BINARY_DIR}/gen/third_party/freetype/freetype/src/base/ftbbox.c
    ${PROJECT_BINARY_DIR}/gen/third_party/freetype/freetype/src/base/ftbitmap.c
    ${PROJECT_BINARY_DIR}/gen/third_party/freetype/freetype/src/base/ftdebug.c
    ${PROJECT_BINARY_DIR}/gen/third_party/freetype/freetype/src/base/ftglyph.c
    ${PROJECT_BINARY_DIR}/gen/third_party/freetype/freetype/src/base/ftinit.c
    ${PROJECT_BINARY_DIR}/gen/third_party/freetype/freetype/src/base/ftsystem.c
    ${PROJECT_BINARY_DIR}/gen/third_party/freetype/freetype/src/gzip/ftgzip.c
    ${PROJECT_BINARY_DIR}/gen/third_party/freetype/freetype/src/sfnt/sfnt.c
    ${PROJECT_BINARY_DIR}/gen/third_party/freetype/freetype/src/smooth/smooth.c
    ${PROJECT_BINARY_DIR}/gen/third_party/freetype/freetype/src/truetype/truetype.c
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-unused-parameter
)

set(COMPONENT_CXXCFLAGS
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
    FT2_BUILD_LIBRARY
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

install_sdk_by_sh(${CMAKE_CURRENT_SOURCE_DIR}/include "*")
install_sdk_by_sh(${CMAKE_CURRENT_SOURCE_DIR}/ "*")
