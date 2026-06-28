#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "security_unified_port")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/security_unified_porting.c
    ${CMAKE_CURRENT_SOURCE_DIR}/security_env_init.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    # fix 3322 multi-thread problem only, remove this macro later.
    CONFIG_MULTI_THREAD_SUPPORT
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

if(("flashboot" IN_LIST APPLICATION) OR ("ssb" IN_LIST APPLICATION) OR ("loaderboot" IN_LIST APPLICATION))
    set(CONFIG_BUILD_ENV "boot")
    list(APPEND PRIVATE_DEFINES
        -DCONFIG_BUILD_ENV_BOOT
    )
elseif("seliteos" IN_LIST APPLICATION)
    set(CONFIG_BUILD_ENV "seliteos")
    list(APPEND PRIVATE_DEFINES
        -DCONFIG_BUILD_ENV_SELITEOS
    )
elseif("UT_TEST" IN_LIST DEFINES OR "FUZZ_TEST" IN_LIST DEFINES)
    set(CONFIG_BUILD_ENV "test")
    list(APPEND PRIVATE_DEFINES
        -DCONFIG_BUILD_ENV_TEST
    )
else()
    set(CONFIG_BUILD_ENV "liteos")
    list(APPEND PRIVATE_DEFINES
        -DCONFIG_BUILD_ENV_LITEOS
    )
endif()

if(${CONFIG_BUILD_ENV} STREQUAL "boot")
    list(APPEND PUBLIC_DEFINES
        # need to move to related to kconfig later.
        -DCONFIG_HASH_SUPPORT_SHA256
    )
endif()

build_component()
