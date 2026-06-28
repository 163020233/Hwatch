#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "modem_service")
 
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/modem_service.c
    ${CMAKE_CURRENT_SOURCE_DIR}/modem_service_msg.c
)
 
set(PRIVATE_HEADER
)
 
set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/
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
 
install_sdk(${CMAKE_CURRENT_SOURCE_DIR} "*.h")
build_component()