#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

add_subdirectory(primary)
add_subdirectory(bluetooth)
if (${CHIP} MATCHES "brandy|3322")
    add_subdirectory(modem)
elseif (${CHIP} MATCHES "socmn1|socmn2")
    add_subdirectory(usb)
endif()
add_subdirectory(common)
