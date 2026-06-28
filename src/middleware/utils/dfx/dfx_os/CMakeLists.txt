#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
if("SUPPORT_DIAG_V2_PROTOCOL" IN_LIST DEFINES)
    set(COMPONENT_NAME "dfx_common")

    if(${OS} STREQUAL "liteos")
        set(SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/liteos_v1/dfx_mem.c
            ${CMAKE_CURRENT_SOURCE_DIR}/liteos_v1/dfx_task.c
            ${CMAKE_CURRENT_SOURCE_DIR}/liteos_v1/dfx_res.c
            ${CMAKE_CURRENT_SOURCE_DIR}/liteos_v1/dfx_cpup.c
            ${CMAKE_CURRENT_SOURCE_DIR}/liteos_v1/dfx_trace.c
        )
        elseif(${OS} STREQUAL "alios")
        set(SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/alios/dfx_mem.c
            ${CMAKE_CURRENT_SOURCE_DIR}/alios/dfx_task.c
        )
        elseif(${OS} STREQUAL "freertos")
        set(SOURCES
            ${CMAKE_CURRENT_SOURCE_DIR}/freertos/dfx_mem.c
            ${CMAKE_CURRENT_SOURCE_DIR}/freertos/dfx_task.c
        )
    endif()

    set(PUBLIC_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/dfx_include
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${ROOT_DIR}/kernel/osal/adapt/include/
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

    install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/dfx_include/" "*.h")
    install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/include/" "*.h")

    build_component()
endif()