#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
enable_language(CXX)
set(COMPONENT_NAME "location_sample")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/location_sample.cpp
)

set(PUBLIC_HEADER
)

set(OPENMG_DIR ${ROOT_DIR}/ohos)

set(PRIVATE_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${ROOT_DIR}/ohos/base/location/location_lite/interfaces/kits/include
    ${ROOT_DIR}/ohos/base/hiviewdfx/hilog_lite/interfaces/native/kits/hilog_lite
)

set(CCDEFINES ${CCDEFINES} "_GNU_SOURCE" "_XOPEN_SOURCE=700" "LOCAL_VOICE_CALL")
list(REMOVE_ITEM CCDEFINES VERSION_STANDARD)
set(PRIVATE_DEFINES
    ${CCDEFINES}
)

set(PUBLIC_DEFINES
    SUPPORT_AT_LOCATION
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_FLAGS)
list(REMOVE_ITEM COMPONENT_FLAGS -std=gnu99 -std=c99 -std=gnu11 -std=c11)
list(APPEND COMPONENT_FLAGS -std=c++11 -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables -nostdlibinc)
set(COMPONENT_CCFLAGS
    ${COMPONENT_FLAGS}
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
