#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ipc_v0")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_v0/${CORE}/ipc_irq.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_v0/shared/ipc.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_v0/${CORE}
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc_v0/shared
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    SUPPORT_IPC
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

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/ipc_v0/shared" "*.h")

build_component()

set(COMPONENT_NAME "ipc")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc/ipc.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/ipc
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if(DEFINED CONFIG_IPC_USING_RING_BUFFER_MAILBOX)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/ipc/ipc_ring.c)
endif()

if(DEFINED CONFIG_IPC_USING_SW_MAILBOX)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/ipc/ipc_sw_mail.c)
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
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
