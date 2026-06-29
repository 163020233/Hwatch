#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "qcbor")

set(SOURCES
    ${ROOT_DIR}/open_source/t_cose/QCBOR_v1.2/src/ieee754.c
    ${ROOT_DIR}/open_source/t_cose/QCBOR_v1.2/src/qcbor_decode.c
    ${ROOT_DIR}/open_source/t_cose/QCBOR_v1.2/src/qcbor_encode.c
    ${ROOT_DIR}/open_source/t_cose/QCBOR_v1.2/src/qcbor_err_to_str.c
    ${ROOT_DIR}/open_source/t_cose/QCBOR_v1.2/src/UsefulBuf.c
)

set(PUBLIC_HEADER
    ${ROOT_DIR}/open_source/t_cose/QCBOR_v1.2/inc
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/open_source/t_cose/QCBOR_v1.2/src
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()