#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "jpeg_static")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1
)

set(SOURCES
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jcapimin.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jcapistd.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jccoefct.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jccolor.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jcdctmgr.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jchuff.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jcicc.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jcinit.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jcmainct.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jcmarker.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jcmaster.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jcomapi.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jcparam.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jcphuff.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jcprepct.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jcsample.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jctrans.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jdapimin.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jdapistd.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jdatadst.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jdatasrc.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jdcoefct.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jdcolor.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jddctmgr.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jdhuff.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jdicc.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jdinput.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jdmainct.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jdmarker.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jdmaster.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jdmerge.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jdphuff.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jdpostct.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jdsample.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jdtrans.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jerror.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jfdctflt.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jfdctfst.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jfdctint.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jidctflt.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jidctfst.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jidctint.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jidctred.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jmemmgr.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jmemnobs.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jquant1.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jquant2.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jutils.c
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1/jsimd_none.c
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-typedef-redefinition
)

set(COMPONENT_CXXCFLAGS
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
    HAVE_UNSIGNED_SHORT=1
    NO_PUTENV
    NO_GETENV
    _LOS_COMPILER_H
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

