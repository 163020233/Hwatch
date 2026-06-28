#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
#===============================================================================

if(NOT "${SECTOR_CFG}" STREQUAL "")
    SET(SECTOR_JSON ${ROOT_DIR}/build/config/target_config/${CHIP}/flash_sector_config/${SECTOR_CFG}.json)
    SET(SECTOR_BIN_PATH ${ROOT_DIR}/interim_binary/${CHIP}/bin/partition/${PKG_TARGET_NAME})
    SET(REFLASH_JSON_PY ${ROOT_DIR}/build/config/target_config/${CHIP}/scripts/partition_auto_gen.py)

    # 定义 REFLASH_PARTITION 目标（如果脚本存在）
    if (EXISTS ${REFLASH_JSON_PY})
        add_custom_target(REFLASH_PARTITION ALL
            COMMENT "reflash partition"
            COMMAND ${Python3_EXECUTABLE} ${REFLASH_JSON_PY} ${SECTOR_JSON} ${BUILD_TARGET_NAME} ${BUILD_PLATFORM}
            WORKING_DIRECTORY ${ROOT_DIR}
            DEPENDS GENERAT_BIN
        )
    endif()

    # 确保 SECTOR_BIN_PATH 目录存在
    if (NOT EXISTS ${SECTOR_BIN_PATH})
        file(MAKE_DIRECTORY ${SECTOR_BIN_PATH})
    endif()

    # 定义 GENERAT_PARTITION 目标，并动态设置依赖关系
    add_custom_target(GENERAT_PARTITION ALL
        COMMAND ${Python3_EXECUTABLE} ${ROOT_DIR}/build/script/param_packet.py ${SECTOR_JSON} ${SECTOR_BIN_PATH}/partition.bin &&
                ${CP} ${SECTOR_BIN_PATH}/partition.bin ${PROJECT_BINARY_DIR}/partition.bin
        COMMENT "update partition bin"
        WORKING_DIRECTORY ${ROOT_DIR}
        DEPENDS GENERAT_BIN
    )

    # 如果 REFLASH_PARTITION 已定义，则添加依赖关系
    if (TARGET REFLASH_PARTITION)
        add_dependencies(GENERAT_PARTITION REFLASH_PARTITION)
    endif()
endif()
