#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "touch_drv")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/touch_ctrl.c
)

if(DEFINED CONFIG_INPUT_USING_TPTYPE_ZTW523)
set(SOURCES ${SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/ztw523_touch/touch_ztw523.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ztw523_touch/touch_ztw523_drv.c

)
elseif(DEFINED CONFIG_INPUT_USING_TPTYPE_CST820)
set(SOURCES ${SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/cst820_touch/touch_cst820.c
    ${CMAKE_CURRENT_SOURCE_DIR}/cst820_touch/touch_cst820_drv.c
)
else()
set(SOURCES ${SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/ztw622_touch/touch_ztw622.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ztw622_touch/touch_ztw622_drv.c

)
endif()

if(DEFINED CONFIG_INPUT_USING_TPTYPE_ZTW523)
set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/ztw523_touch
)
elseif(DEFINED CONFIG_INPUT_USING_TPTYPE_CST820)
set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/cst820_touch
)
else()
set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/ztw622_touch
)
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
-Wno-unused-function
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

