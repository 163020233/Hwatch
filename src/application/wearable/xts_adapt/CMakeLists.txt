#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "xts_adapt")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/cjson_adapt.c
)

set(PUBLIC_HEADER
)

if(DEFINED CONFIG_SUPPORT_OH)
    list(APPEND SOURCES
	    ${CMAKE_CURRENT_SOURCE_DIR}/cmsis_adapt.c
    )
elseif("ENABLE_UIKIT" IN_LIST DEFINES)
elseif("CFG_NORFLASH_ONLY" IN_LIST DEFINES)
else()
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/hal_file.c
    )
    set(PRIVATE_HEADER
        ${OHOS_DIR}/utils/native/lite/hals/file
    )
endif()

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    UNITY_OUTPUT_CHAR=xts_test_output_char
    OHOS_DEBUG
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
