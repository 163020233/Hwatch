#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

set(UIKIT_ROOT_DIR ${PROJECT_ROOT_DIR}/uikit)
set(GPU_CODE_DIR   ${ROOT_DIR}/middleware/services/gpu)
add_compile_options(-D_FORTIFY_SOURCE=2 -O2)
add_compile_options(-ftrapv)
set(COMPONENT_NAME "display_gfx")

if("${CHIP}" STREQUAL "socmn2")
set(SOURCES )
else()
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/yuv_to_rgb.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/display_gmmu.c
    ${PROJECT_ROOT_DIR}/hal/common/src/barcode128.c
    ${PROJECT_ROOT_DIR}/hal/common/src/graphic_hardware_config.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/display_vau/display_vau_init.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/display_vau/display_vau_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/display_vau/display_vau_path.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/display_vau/display_vau_blit.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/display_vau/display_vau_blur.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/display_vau/display_vau_hsv.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/display_vau/display_vau_light.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/display_vau/display_vau_rain.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/display_vau/display_vau_3d.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vau_render_offline_list.c
)
endif()

set(PUBLIC_HEADER
    ${PROJECT_ROOT_DIR}/hal/common/include
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/include/osal
    ${ROOT_DIR}/include/osal/memory
    ${ROOT_DIR}/kernel/osal/adapt/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${OHOS_DIR}/drivers/peripheral/display/interfaces/include
    ${UIKIT_ROOT_DIR}/proprietary/src/utils/innerkits
    ${GPU_CODE_DIR}/drv/vau/hal/v2
    ${GPU_CODE_DIR}/drv/vau/hal
    ${GPU_CODE_DIR}/drv/vau/list
    ${GPU_CODE_DIR}/drv/vau/node
    ${GPU_CODE_DIR}/drv/comm/crg
    ${GPU_CODE_DIR}/rm/vau/inc
    ${GPU_CODE_DIR}/rm/vau/inc/math
    ${GPU_CODE_DIR}/api/vau/matrix
)

set(PRIVATE_DEFINES
    ${CCDEFINES}
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(CFLAG ${ccflags} -nostdlibinc)
list(APPEND CFLAG -Wno-missing-field-initializers)
set(COMPONENT_CCFLAGS
    ${CFLAG}
    -Wno-int-conversion
    -Wno-sign-compare
    -Wno-enum-conversion
    -Wno-unused-parameter
    -Wno-unused-function
    -Wno-incompatible-pointer-types
    -Wno-incompatible-pointer-types-discards-qualifiers
    -Werror=return-type
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(AUTO_ADD_PATH
    false
)

build_component()

#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "display_gralloc")

set(SOURCES_GRALLOC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/display_gralloc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mempool/src/mem_pool_los.c
    )
set(SOURCES
    ${SOURCES_GRALLOC}
)

set(PUBLIC_HEADER
    ${PROJECT_ROOT_DIR}/hal/common/include
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/mempool/include
    ${OHOS_DIR}/drivers/peripheral/display/interfaces/include
    ${UIKIT_ROOT_DIR}/proprietary/src/utils/innerkits
)

set(PRIVATE_DEFINES
    ${CCDEFINES}
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(CFLAG ${ccflags} -nostdlibinc)
list(APPEND CFLAG -Wno-missing-field-initializers)
set(COMPONENT_CCFLAGS
    ${CFLAG}
    -Wno-sign-compare
    -Werror=return-type
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(AUTO_ADD_PATH
    false
)

build_component()

#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "display_layer")

if("${CHIP}" STREQUAL "socmn2")
set(SOURCES_LAYER ${CMAKE_CURRENT_SOURCE_DIR}/src/display_layer_qspi.c)
else()
set(SOURCES_LAYER ${CMAKE_CURRENT_SOURCE_DIR}/src/display_layer.c)
endif()
set(SOURCES
    ${SOURCES_LAYER}
)

set(PUBLIC_HEADER
    ${PROJECT_ROOT_DIR}/hal/common/include
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/drivers/peripheral/display/interfaces/include
    ${UIKIT_ROOT_DIR}/proprietary/src/utils/innerkits
)

set(PRIVATE_DEFINES
    ${CCDEFINES}
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(CFLAG ${ccflags} -nostdlibinc)
set(COMPONENT_CCFLAGS
    ${CFLAG}
    -Wno-unused-parameter
    -Wno-unused-function
    -Werror=return-type
    -Wno-tautological-pointer-compare
    -Wno-int-conversion
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

