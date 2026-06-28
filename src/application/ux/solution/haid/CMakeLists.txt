#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
add_subdirectory_if_exist(service)
add_subdirectory_if_exist(fitting)
add_subdirectory_if_exist(commu)
add_subdirectory_if_exist(media)
add_subdirectory_if_exist(statistic)

set(COMPONENT_NAME "hearing_aid_service")

set(SOURCES
    ${HEARING_SERVICE_LIST}
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${UX_INTERNAL_HEADERS}
    ${CMAKE_CURRENT_SOURCE_DIR}/media
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    SUPPORT_UX_HAID
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -fstack-protector-strong
    -Werror=all
    -Wconversion
    -Werror=incompatible-pointer-types
    -Werror=unused-parameter
    -Werror=enum-conversion
    -Werror=sign-compare
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/service" "*.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/media" "*.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/commu" "*.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/fitting" "*.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/statistic" "*.h")

build_component()
