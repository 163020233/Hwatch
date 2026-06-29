#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "zlib_static")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/third_party/zlib
)

set(SOURCES
    ${OHOS_DIR}/third_party/zlib/adler32.c
    ${OHOS_DIR}/third_party/zlib/compress.c
    ${OHOS_DIR}/third_party/zlib/crc32.c
    ${OHOS_DIR}/third_party/zlib/deflate.c
    ${OHOS_DIR}/third_party/zlib/gzclose.c
    ${OHOS_DIR}/third_party/zlib/gzlib.c
    ${OHOS_DIR}/third_party/zlib/gzread.c
    ${OHOS_DIR}/third_party/zlib/gzwrite.c
    ${OHOS_DIR}/third_party/zlib/infback.c
    ${OHOS_DIR}/third_party/zlib/inffast.c
    ${OHOS_DIR}/third_party/zlib/inflate.c
    ${OHOS_DIR}/third_party/zlib/inftrees.c
    ${OHOS_DIR}/third_party/zlib/trees.c
    ${OHOS_DIR}/third_party/zlib/uncompr.c
    ${OHOS_DIR}/third_party/zlib/zutil.c
    ${OHOS_DIR}/third_party/zlib/contrib/minizip/ioapi.c
    ${OHOS_DIR}/third_party/zlib/contrib/minizip/unzip.c
    ${OHOS_DIR}/third_party/zlib/contrib/minizip/zip.c
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
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

