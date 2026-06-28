#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ux_solution_headers")

set(PUBLIC_HEADER)

if(NOT "DEMO_WATCH" IN_LIST DEFINES)
list(APPEND PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/led/
    ${CMAKE_CURRENT_SOURCE_DIR}/protocol/
    ${CMAKE_CURRENT_SOURCE_DIR}/button/
    ${CMAKE_CURRENT_SOURCE_DIR}/display/
    ${CMAKE_CURRENT_SOURCE_DIR}/recovery/
    ${CMAKE_CURRENT_SOURCE_DIR}/update/
    ${CMAKE_CURRENT_SOURCE_DIR}/factory/
    ${CMAKE_CURRENT_SOURCE_DIR}/haid/
    ${CMAKE_CURRENT_SOURCE_DIR}/jack/
    ${CMAKE_CURRENT_SOURCE_DIR}/sensor/
)
endif()
list(APPEND PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/audio/
    ${CMAKE_CURRENT_SOURCE_DIR}/player/
    ${CMAKE_CURRENT_SOURCE_DIR}/usb/
    ${CMAKE_CURRENT_SOURCE_DIR}/power/
    ${CMAKE_CURRENT_SOURCE_DIR}/system/
    ${CMAKE_CURRENT_SOURCE_DIR}/bg/
    ${CMAKE_CURRENT_SOURCE_DIR}/box/
    ${CMAKE_CURRENT_SOURCE_DIR}/recovery/
    ${CMAKE_CURRENT_SOURCE_DIR}/jack/
)

if(NOT ("MELODY_DEMO_DONGLE" IN_LIST DEFINES OR "MELODY_DEMO_SOUNDBAR" IN_LIST DEFINES))
list(APPEND PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/charger/
)
endif()

if("CONFIG_EXT_MIC_SUPPORT" IN_LIST DEFINES)
list(APPEND PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/mic/
)

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/mic/" "*.h")
endif()

if(NOT "DEMO_WATCH" IN_LIST DEFINES)
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/led/" "*.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/protocol/" "*.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/button/" "*.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/display/" "*.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/recovery/" "*.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/update/" "*.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/factory/" "*.h")
endif()
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/audio/" "*.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/player/" "*.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/usb/" "*.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/power/" "*.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/system/" "*.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/bg/" "*.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/box/" "*.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/jack/" "*.h")

build_component()