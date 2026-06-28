#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(vdec_proc_support "yes")
set(vdec_print_support "yes")
set(vdec_mjpeg_support "yes")
set(vdec_debug_support "yes")
set(vfmw_over_spec_support "no")
set(vfmw_ipt_support "no")

if(${CHIP} MATCHES "sw22")
set(vdec_heif_support "yes")
set(VDEC_DEFINE_LIST
    VDEC_MAX_CHAN_NUM=3
)
else()
set(vdec_heif_support "no")
set(VDEC_DEFINE_LIST
    VDEC_MAX_CHAN_NUM=2
    VDEC_LOW_POWER_SUPPORT
)
endif()

add_subdirectory(api)
add_subdirectory(drv)

set(COMPONENT_NAME "vdec")

set(SOURCES
    ${VDEC_SRC_LIST}
)
set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${VDEC_INC_LIST}
    ${ROOT_DIR}/middleware/utils/common_headers/vdec/external
    ${ROOT_DIR}/middleware/utils/common_headers/vdec/internal
)

if(${vfmw_over_spec_support} STREQUAL "yes")
set(OVER_DEFINES
    VFMW_OVER_SPEC_SUPPORT
)
endif()

if(${vfmw_ipt_support} STREQUAL "yes")
set(IPT_DEFINES
    VFMW_IPT_SUPPORT
    VFMW_OVER_SPEC_SUPPORT
)
endif()

if(${vdec_proc_support} STREQUAL "yes")
set(PROC_DEFINES
    VDEC_PROC_SUPPORT
)
endif()

if(${vdec_debug_support} STREQUAL "yes")
set(DEBUG_DEFINES
    VDEC_DEBUG_SUPPORT
)
endif()

if(${vdec_print_support} STREQUAL "no")
set(PRINT_DEFINES
    VDEC_NO_DEBUG_LOG_SUPPORT
)
endif()

if(${vdec_mjpeg_support} STREQUAL "yes")
set(MJPEG_DEFINES
    EXT_VDEC_MJPEG_SUPPORT
)
endif()

if(${vdec_heif_support} STREQUAL "yes")
set(HEIF_DEFINES
    VDEC_HEIF_SUPPORT
)
endif()

set(PUBLIC_DEFINES

)

set(PRIVATE_DEFINES
	"${MJPEG_DEFINES}"
	"${HEIF_DEFINES}"
    "${PRINT_DEFINES}"
    "${OVER_DEFINES}"
    "${PROC_DEFINES}"
    "${DEBUG_DEFINES}"
    "${VDEC_DEFINE_LIST}"
    "${IPT_DEFINES}"
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    "-Werror=out-of-line-declaration"
    "-Werror=uninitialized"
    "-Werror=int-conversion"
    "-Werror=enum-conversion"
    "-Werror=long-long"
    "-Werror=unused-variable"
    "-Werror=unused-value"
    "-Werror=unused-function"
    "-Werror=unused-but-set-variable"
    "-Werror=sign-compare"
    "-Werror=unused-parameter"
    "-Werror=tautological-compare"
    "-Werror=bad-function-cast"
    "-Werror=incompatible-pointer-types"
    "-Werror=implicit-function-declaration"
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(LIBRARY_OUTPUT_PATH ${BIN_DIR}/${CHIP}/libs/vdec/${TARGET_COMMAND})
build_component()
install_sdk(${BIN_DIR}/${CHIP}/libs/vdec/${TARGET_COMMAND} "*.a")
