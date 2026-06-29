#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "non_os")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/debug/non_os_debug.c
    ${CMAKE_CURRENT_SOURCE_DIR}/debug/non_os_nmi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/non_os_utils.c
)

set(CORE_LIST "control_core" "bcore")
if(${CORE} IN_LIST CORE_LIST)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/osal_addr.c
        ${CMAKE_CURRENT_SOURCE_DIR}/osal_atomic.c
        ${CMAKE_CURRENT_SOURCE_DIR}/osal_debug.c
        ${CMAKE_CURRENT_SOURCE_DIR}/osal_mutex.c
        ${CMAKE_CURRENT_SOURCE_DIR}/osal_spinlock.c
        ${CMAKE_CURRENT_SOURCE_DIR}/osal_task.c
        ${CMAKE_CURRENT_SOURCE_DIR}/osal_timer.c
        ${CMAKE_CURRENT_SOURCE_DIR}/osal_wait.c
        ${CMAKE_CURRENT_SOURCE_DIR}/osal_semaphore.c
        ${CMAKE_CURRENT_SOURCE_DIR}/osal_msgqueue.c
    )
endif()

# non os使用osal内存操作接口
if("SUPPORT_OSAL_MEM" IN_LIST DEFINES)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/osal_addr.c
    )
endif()

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/debug
)

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
    -Wno-error=unused-parameter
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}" "non_os.h")

build_component()
