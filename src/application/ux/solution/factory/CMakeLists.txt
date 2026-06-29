#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
add_subdirectory_if_exist(audio)
add_subdirectory_if_exist(fac_tlv)

set(COMPONENT_NAME "ux_factory")

set(SOURCES
    ${UX_FACTORY_LIST}
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${UX_INTERNAL_HEADERS}
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    SUPPORT_UX_FACTORY
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

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/audio" "*.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/fac_tlv" "*.h")

build_component()
