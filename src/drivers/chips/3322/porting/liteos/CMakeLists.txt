#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "liteos_port")

if(${OS} MATCHES "liteos")
    if(${ARCH} MATCHES "riscv70")
        set(SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/riscv70/idle_config.c
            ${CMAKE_CURRENT_SOURCE_DIR}/riscv70/tick_timer.c
            ${CMAKE_CURRENT_SOURCE_DIR}/riscv70/os_dfx.c
        )
        set(PUBLIC_HEADER
            ${CMAKE_CURRENT_SOURCE_DIR}/riscv70
        )
    elseif(${ARCH} MATCHES "riscv32")
        set(SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/riscv32/idle_config.c
            ${CMAKE_CURRENT_SOURCE_DIR}/riscv32/tick_timer.c
        )
        set(PUBLIC_HEADER
            ${CMAKE_CURRENT_SOURCE_DIR}/riscv32
        )
    endif()
endif()

set(PRIVATE_HEADER
)


set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-implicit-function-declaration
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
