#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
#===============================================================================


if(${CONFIG_SUPPORT_TA} STREQUAL "y")
add_subdirectory_if_exist(application/boxa)
add_subdirectory_if_exist(application/boxb)
add_subdirectory_if_exist(common/usrdrvlib)
add_subdirectory_if_exist(common/usrsyslib)
endif()

add_subdirectory_if_exist(target/${CHIP})
add_subdirectory_if_exist(common/mem)
add_subdirectory_if_exist(kernel)
add_subdirectory_if_exist(kernel/base/sec_storage)
add_subdirectory_if_exist(kernel/service/psa/crypto)
add_subdirectory_if_exist(kernel/service/psa/attestation)
add_subdirectory_if_exist(kernel/service/psa/pake)
add_subdirectory_if_exist(kernel/service/psa/storage)
add_subdirectory_if_exist(kernel/service/psa/ioctl)
add_subdirectory_if_exist(drivers)
add_subdirectory_if_exist(test)
