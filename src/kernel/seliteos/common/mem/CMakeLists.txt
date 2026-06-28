#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "seliteos_mem")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/bestfit_little/los_heap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/bestfit_little/los_memory.c
    ${CMAKE_CURRENT_SOURCE_DIR}/los_mux.c
)

set(TA_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/los_usr_mem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/los_drvbox_mem.c
)

if(${CONFIG_SUPPORT_TA} STREQUAL "y")
    list(APPEND SOURCES ${TA_SOURCES})
endif()

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/kernel/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/kernel/base/include
    ${CMAKE_CURRENT_SOURCE_DIR}/bestfit_little
)

set(PRIVATE_DEFINES
    -DLOSCFG_MEM_BESTFIT_LITTLE
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
