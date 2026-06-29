#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hal_memory_static")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/commonlibrary/utils_lite/memory/include
    ${OHOS_DIR}/commonlibrary/utils_lite/include
)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ohos_mem_pool.c
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
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

set(COMPONENT_NAME "mem_util")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${OHOS_DIR}/foundation/arkui/ace_engine_lite/interfaces/inner_api/builtin/base
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/kits/hilog_lite
    ${ROOT_DIR}/drivers/drivers/driver/ipc/ipc_old/acore
    ${ROOT_DIR}/drivers/drivers/driver/ipc/ipc_old/shared
    ${ROOT_DIR}/include/middleware/utils
    ${ROOT_DIR}/kernel/dpal/include
    ${ROOT_DIR}/kernel/dpal/device/include
    ${ROOT_DIR}/include/osal
    ${ROOT_DIR}/include/osal/atomic
    ${ROOT_DIR}/include/osal/debug
    ${ROOT_DIR}/include/osal/device
    ${ROOT_DIR}/include/osal/drvbox
    ${ROOT_DIR}/include/osal/event
    ${ROOT_DIR}/include/osal/fileops
    ${ROOT_DIR}/include/osal/interrupt
    ${ROOT_DIR}/include/osal/lock
    ${ROOT_DIR}/include/osal/math
    ${ROOT_DIR}/include/osal/memory
    ${ROOT_DIR}/include/osal/msgqueue
    ${ROOT_DIR}/include/osal/proc
    ${ROOT_DIR}/include/osal/schedule
    ${ROOT_DIR}/include/osal/semaphore
    ${ROOT_DIR}/include/osal/string
    ${ROOT_DIR}/include/osal/time
    ${ROOT_DIR}/kernel/osal/adapt/include
    ${ROOT_DIR}/kernel/osal/test/alios
    ${ROOT_DIR}/kernel/osal/test/src
    ${ROOT_DIR}/middleware/services/gui/hal/common/include
    ${OHOS_DIR}/foundation/arkui/ui_lite/wearable/interfaces/kits
    ${ROOT_DIR}/middleware/utils/common_headers/drivers/gpu
    ${ROOT_DIR}/middleware/utils/common_headers/native
    ${ROOT_DIR}/middleware/utils/common_headers/osal
    ${ROOT_DIR}/middleware/utils/dfx/dfx_os/dfx_include
    ${ROOT_DIR}/middleware/utils/dfx/dfx_os/include
    ${ROOT_DIR}/middleware/utils/dfx/write_interface/include
    ${ROOT_DIR}/middleware/utils/dfx/diag
    ${ROOT_DIR}/middleware/utils/dfx/diag/dfx_include
    ${ROOT_DIR}/middleware/utils/dfx/diag/include
    ${ROOT_DIR}/middleware/utils/dfx/diag/diag_cmd_router/include
    ${ROOT_DIR}/middleware/utils/dfx/diag_transmit/include
    ${ROOT_DIR}/middleware/utils/dfx/diag_transmit/dfx_include
    ${ROOT_DIR}/middleware/utils/dfx/diag_transmit
    ${ROOT_DIR}/middleware/utils/dfx/log/include
    ${ROOT_DIR}/middleware/utils/dfx/log_reader/public
    ${ROOT_DIR}/middleware/utils/dfx/log_buffer
)

if ("${CHIP}" STREQUAL "brandy")
    list(APPEND PRIVATE_HEADER 
        ${ROOT_DIR}/middleware/chips/brandy/dfx/include
        ${ROOT_DIR}/middleware/chips/brandy/dfx
        ${ROOT_DIR}/drivers/boards/brandy_evb/board_config
        ${ROOT_DIR}/drivers/boards/brandy_evb/memory_config/include
        ${ROOT_DIR}/drivers/boards/brandy_evb/product
        ${ROOT_DIR}/drivers/chips/brandy/arch/include
        ${ROOT_DIR}/drivers/chips/brandy/arch/riscv/riscv70
        ${ROOT_DIR}/drivers/chips/brandy/arch/riscv/riscv_common
        ${ROOT_DIR}/drivers/chips/brandy/arch/riscv
        ${ROOT_DIR}/drivers/chips/brandy/config
        ${ROOT_DIR}/drivers/chips/brandy/interrupt
        ${ROOT_DIR}/drivers/chips/brandy/porting/include
        ${ROOT_DIR}/drivers/chips/brandy/porting/arch/riscv
    )
elseif ("${CHIP}" STREQUAL "3322")
    list(APPEND PRIVATE_HEADER 
        ${ROOT_DIR}/middleware/chips/3322/dfx/include
        ${ROOT_DIR}/middleware/chips/3322/dfx
        ${ROOT_DIR}/drivers/boards/3322_fpga/board_config
        ${ROOT_DIR}/drivers/boards/3322_fpga/memory_config/include
        ${ROOT_DIR}/drivers/boards/3322_fpga/product
        ${ROOT_DIR}/drivers/chips/3322/arch/include
        ${ROOT_DIR}/drivers/chips/3322/arch/riscv/riscv70
        ${ROOT_DIR}/drivers/chips/3322/arch/riscv/riscv_common
        ${ROOT_DIR}/drivers/chips/3322/arch/riscv
        ${ROOT_DIR}/drivers/chips/3322/config
        ${ROOT_DIR}/drivers/chips/3322/interrupt
        ${ROOT_DIR}/drivers/chips/3322/porting/include
        ${ROOT_DIR}/drivers/chips/3322/porting/arch/riscv
    )
endif()
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/mem_util.cpp
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -std=c++11
    -Wno-error
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

