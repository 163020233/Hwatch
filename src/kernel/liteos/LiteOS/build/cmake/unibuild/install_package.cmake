string(TOUPPER ${PLAT_TYPE} PLAT_TYPE_UP)
string(TOLOWER ${PLAT_TYPE} PLAT_TYPE_LOW)
string(TOUPPER ${CPU_TYPE} CPU_TYPE_UP)
string(TOLOWER ${CPU_TYPE} CPU_TYPE_LOW)

set(TARGET_LIB_LIST ${LITEOS_DEP_LIBS_INT})
include(${RTOS_LITE_TOP_DIR}/build/libck_cmake/functions/libck_functions.cmake)

##### remove unnecessary libs #####
include(${RTOS_LITE_TOP_DIR}/build/cmake/unibuild/trim_lib_list.cmake)
foreach(LIB IN LISTS TRIM_LIB_LIST)
    list(REMOVE_ITEM TARGET_LIB_LIST ${LIB})
endforeach()

set(FILTERED_LIB_LIST)
foreach(LIB IN LISTS TARGET_LIB_LIST)
    if(NOT LIB MATCHES "^[Tt][Ee][Ss][Tt]_")
        list(APPEND FILTERED_LIB_LIST ${LIB})
    endif()
endforeach()
set(TARGET_LIB_LIST ${FILTERED_LIB_LIST})

set(EXPORT_NAME modules)

set(INSTALL_BASE_DIR               .)
set(INSTALL_INCLUDE_DIR            RTOS_Lite/include)
set(INSTALL_ARCHIVE_DIR            RTOS_Lite/lib/${PLAT_TYPE_LOW}/${CPU_TYPE_LOW})
set(INSTALL_ARCHIVE_CONFIG_DIR     RTOS_Lite/config)
set(INSTALL_TOOLS_DIR              RTOS_Lite/tools)
set(INSTALL_CONFIG_DIR             cmake/${PLAT_TYPE_LOW}_${CPU_TYPE_LOW})

include(CMakePackageConfigHelpers)
configure_package_config_file(${RTOS_LITE_TOP_DIR}/build/cmake/unibuild/common-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/rtoslite-${PLAT_TYPE_LOW}-${CPU_TYPE_LOW}-config.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIG_DIR}
    INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/os_${TARGET}
    PATH_VARS
    INSTALL_BASE_DIR
    INSTALL_INCLUDE_DIR
    INSTALL_ARCHIVE_DIR
    INSTALL_ARCHIVE_CONFIG_DIR
    INSTALL_TOOLS_DIR
    INSTALL_CONFIG_DIR
)

########################### cmake ###########################
install(EXPORT ${EXPORT_NAME}
    NAMESPACE RTOS_Lite::
    FILE rtoslite-${PLAT_TYPE_LOW}-${CPU_TYPE_LOW}-targets.cmake
    DESTINATION ${INSTALL_CONFIG_DIR}
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/rtoslite-${PLAT_TYPE_LOW}-${CPU_TYPE_LOW}-config.cmake
    DESTINATION ${INSTALL_CONFIG_DIR}
)

install(TARGETS ${TARGET_LIB_LIST}
    EXPORT ${EXPORT_NAME}
    ARCHIVE DESTINATION ${INSTALL_ARCHIVE_DIR}
)

########################### buildef ###########################
install(FILES
    ${CMAKE_BINARY_DIR}/include/os_buildef.h
    DESTINATION ${INSTALL_ARCHIVE_CONFIG_DIR}/${PLAT_TYPE_LOW}/config_${CPU_TYPE_LOW}
)
if(CONFIG_OS_OPTION_WL_BIN_SAME)
    install(FILES
        ${CMAKE_BINARY_DIR}/include/sre_buildef.h
        DESTINATION ${INSTALL_ARCHIVE_CONFIG_DIR}/${PLAT_TYPE_LOW}/config_${CPU_TYPE_LOW}
    )
endif()

########################### common header files ###########################
file(GLOB common_include_files  ${RTOS_LITE_TOP_DIR}/self_src/include/uapi/common/base/*.h)
if("${CONFIG_OS_OPTION_WL_BIN_SAME}" STREQUAL "")
    list(REMOVE_ITEM common_include_files "${RTOS_LITE_TOP_DIR}/self_src/include/uapi/common/base/os_buildef.h")
endif()
install(FILES
    ${common_include_files}
    DESTINATION ${INSTALL_INCLUDE_DIR}/common/base
)

file(GLOB common_include_files  ${RTOS_LITE_TOP_DIR}/self_src/include/uapi/common/extend/*.h)
install(FILES
    ${common_include_files}
    DESTINATION ${INSTALL_INCLUDE_DIR}/common/extend
)

if(CONFIG_OS_OPTION_FUSION_INTEGRATE_API)
    file(GLOB include_files  ${RTOS_LITE_TOP_DIR}/self_src/include/uapi/mdrv/*.h)
    install(FILES
        ${include_files}
        DESTINATION ${INSTALL_INCLUDE_DIR}/mdrv
    )
    install_cpu_headers(hw_cpu_include_files ${RTOS_LITE_TOP_DIR}/self_src/include/uapi/hw/cpu ${INSTALL_INCLUDE_DIR}/mdrv)
else()
    file(GLOB hw_board_include_files  ${RTOS_LITE_TOP_DIR}/self_src/include/uapi/hw/board/*)
    install(FILES
        ${hw_board_include_files}
        DESTINATION ${INSTALL_INCLUDE_DIR}/hw/board
    )
    install_cpu_headers(hw_cpu_include_files ${RTOS_LITE_TOP_DIR}/self_src/include/uapi/hw/cpu ${INSTALL_INCLUDE_DIR})

    file(GLOB hw_drv_include_files  ${RTOS_LITE_TOP_DIR}/self_src/include/uapi/hw/drv/*)
    install(FILES
        ${hw_drv_include_files}
        DESTINATION ${INSTALL_INCLUDE_DIR}/hw/drv
    )

    file(GLOB include_files  ${RTOS_LITE_TOP_DIR}/self_src/include/uapi/drb/base/*.h)
    install(FILES
        ${include_files}
        DESTINATION ${INSTALL_INCLUDE_DIR}/drb/base
    )

    file(GLOB include_files  ${RTOS_LITE_TOP_DIR}/self_src/include/uapi/drb/extend/*.h)
    install(FILES
        ${include_files}
        DESTINATION ${INSTALL_INCLUDE_DIR}/drb/extend
    )
endif()

if(CONFIG_OS_OPTION_LOS)
    install(FILES
        ${RTOS_LITE_TOP_DIR}/self_src/config/drbconfig/drb_target_symboldef.c
        RENAME los_target_symboldef.c
        DESTINATION ${INSTALL_ARCHIVE_CONFIG_DIR}/symbol
    )

    install(FILES
        ${RTOS_LITE_TOP_DIR}/self_src/config/drbconfig/drb_config.c
        RENAME los_config.c
        DESTINATION ${INSTALL_ARCHIVE_CONFIG_DIR}
    )
    install(FILES
        ${RTOS_LITE_TOP_DIR}/self_src/config/drbconfig/drb_config_internal.h
        RENAME los_config_internal.h
        DESTINATION ${INSTALL_ARCHIVE_CONFIG_DIR}
    )
    install(FILES
        ${RTOS_LITE_TOP_DIR}/self_src/config/drbconfig/config/drb_config.h
        RENAME los_config.h
        DESTINATION ${INSTALL_ARCHIVE_CONFIG_DIR}
    )

    file(GLOB los_include_files ${RTOS_LITE_TOP_DIR}/self_src/include/uapi/los/*)
    install(FILES
        ${los_include_files}
        DESTINATION ${INSTALL_INCLUDE_DIR}/los
    )
else()
    install(FILES
        ${RTOS_LITE_TOP_DIR}/self_src/config/drbconfig/drb_target_symboldef.c
        DESTINATION ${INSTALL_ARCHIVE_CONFIG_DIR}/symbol
    )

    install(FILES
        ${RTOS_LITE_TOP_DIR}/self_src/config/drbconfig/drb_config.c
        ${RTOS_LITE_TOP_DIR}/self_src/config/drbconfig/drb_config_internal.h
        ${RTOS_LITE_TOP_DIR}/self_src/config/drbconfig/config/drb_config.h
        DESTINATION ${INSTALL_ARCHIVE_CONFIG_DIR}
    )
endif()

if(CONFIG_OS_OPTION_COMPAT_CMSIS)
    file(GLOB cmsis_include_files  ${RTOS_LITE_TOP_DIR}/self_src/compat/cmsis/*.h)
    install(FILES
        ${cmsis_include_files}
        DESTINATION ${INSTALL_INCLUDE_DIR}/cmsis
    )
endif()

if(CONFIG_OS_OPTION_VOS)
    file(GLOB include_files  ${RTOS_LITE_TOP_DIR}/self_src/include/uapi/vos/*.h)
    install(FILES
        ${include_files}
        DESTINATION ${INSTALL_INCLUDE_DIR}/vos
    )
endif()

if(CONFIG_OS_OPTION_POSIX OR CONFIG_OS_OPTION_FS_LITEVFS)
    file(GLOB include_files  ${RTOS_LITE_TOP_DIR}/self_src/include/uapi/posix/*.h)
    install(FILES
        ${include_files}
        DESTINATION ${INSTALL_INCLUDE_DIR}/posix
    )

    file(GLOB include_files  ${RTOS_LITE_TOP_DIR}/self_src/include/uapi/posix/sys/*.h)
    install(FILES
        ${include_files}
        DESTINATION ${INSTALL_INCLUDE_DIR}/posix/sys
    )
endif()

include(${RTOS_LITE_TOP_DIR}/build/cmake/unibuild/special_install.cmake)
