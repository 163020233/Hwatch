#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "png_static")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${PROJECT_BINARY_DIR}/gen/third_party/libpng/libpng-1.6.38
    ${OHOS_DIR}/third_party/zlib
)

set(SOURCES
    ${PROJECT_BINARY_DIR}/gen/third_party/libpng/libpng-1.6.38/png.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libpng/libpng-1.6.38/pngerror.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libpng/libpng-1.6.38/pngget.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libpng/libpng-1.6.38/pngmem.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libpng/libpng-1.6.38/pngpread.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libpng/libpng-1.6.38/pngread.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libpng/libpng-1.6.38/pngrio.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libpng/libpng-1.6.38/pngrtran.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libpng/libpng-1.6.38/pngrutil.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libpng/libpng-1.6.38/pngset.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libpng/libpng-1.6.38/pngtrans.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libpng/libpng-1.6.38/pngwio.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libpng/libpng-1.6.38/pngwrite.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libpng/libpng-1.6.38/pngwtran.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libpng/libpng-1.6.38/pngwutil.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libpng/libpng-1.6.38/arm/arm_init.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libpng/libpng-1.6.38/arm/filter_neon_intrinsics.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libpng/libpng-1.6.38/arm/palette_neon_intrinsics.c
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-error=null-pointer-subtraction
    -Wno-error=unused-but-set-variable
    -Wno-error=unused-parameter
)

set(COMPONENT_CXXCFLAGS
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

install_sdk_by_sh(${CMAKE_CURRENT_SOURCE_DIR}/ "*")