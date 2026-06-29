#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
add_subdirectory_if_exist(bootrom)
add_subdirectory_if_exist(flashboot)
add_subdirectory_if_exist(provision)

if (${CHIP} MATCHES "bs20|bs21|bs21a|bs22|bs26|bs27a|ws53|ws63|ws75|sw39|cat1|sw21|socmn2|wuxing|3322|sw22")
add_subdirectory_if_exist(commonboot)
endif()

add_subdirectory_if_exist(provision_${CHIP})
add_subdirectory_if_exist(flashboot_${CHIP})
add_subdirectory_if_exist(bootrom_${CHIP})
