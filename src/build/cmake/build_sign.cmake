#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
if (EXISTS ${ROOT_DIR}/build/config/target_config/${CHIP}/cmake/${CHIP}_sign.cmake)
    include("${ROOT_DIR}/build/config/target_config/${CHIP}/cmake/${CHIP}_sign.cmake")
endif()

# 需要签名的镜像匹配规则
set(TARGETS_LIST "^ate_debug$" "^ate$" "^tee$" "^upgrade$" "application.*" "protocol.*" "^bt.*" "^wifi.*" "seliteos.*" "ccore.*" "sensor.*" "gnss.*" "ssb.*" "^recovery$") # ^xxx$ :精准匹配正则
string(JOIN "|" COMBINED_REGEX ${TARGETS_LIST})

if (${CHIP} STREQUAL "bs27a" AND ${TARGET_NAME} MATCHES "^tee$|protocol.*|loaderboot")
    if (${TARGET_NAME} STREQUAL "protocol")
        set(BIN_TYPE ${TARGET_NAME})
    else()
        set(BIN_TYPE "boot_bin")
    endif()
    string(REPLACE "_" "-" TARGET_DIR ${BUILD_TARGET_NAME})
    if (NOT EXISTS ${ROOT_DIR}/interim_binary/${CHIP}/bin/${BIN_TYPE}/${TARGET_DIR})
        file(MAKE_DIRECTORY ${ROOT_DIR}/interim_binary/${CHIP}/bin/${BIN_TYPE}/${TARGET_DIR})
    endif()
    if(TARGET GENERAT_ROM_PATCH)
        set(DEPENDENCY_TARGET GENERAT_ROM_PATCH)
    else()
        set(DEPENDENCY_TARGET GENERAT_BIN)
    endif()
    add_custom_target(COPY_BIN ALL
        COMMAND ${CP} ${ROOT_DIR}/output/${CHIP}/${CORE}/${TARGET_DIR}/${TARGET_NAME}.bin ${ROOT_DIR}/interim_binary/${CHIP}/bin/${BIN_TYPE}/${TARGET_DIR}/${TARGET_NAME}.bin
        COMMENT "copy bin to interim binary"
        DEPENDS ${DEPENDENCY_TARGET}
    )
endif()

if (${TARGET_NAME} STREQUAL "flashboot")
if (EXISTS ${ROOT_DIR}/build/config/target_config/${CHIP}/sign_config/${BUILD_TARGET_NAME}.cfg)
    if (NOT ${SEC_BOOT} STREQUAL "")
        # 如果使能了secboot
        if (${SEC_BOOT_VERIFY})
            # 如果使能了secboot的验签功能
            if(EXISTS ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin/${SEC_BOOT}/sec_boot_sign.bin)
                # 存在secboot sign则不签名secboot，只签名flashboot
                add_custom_target(GENERAT_SIGNBIN ALL
                    COMMAND ${SIGN_TOOL} 0 ${ROOT_DIR}/build/config/target_config/${CHIP}/sign_config/${BUILD_TARGET_NAME}.cfg 1>nul 2>nul
                    COMMENT "sign file:gen boot sign file"
                    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
                    DEPENDS GENERAT_BIN
                )
            else()
                # 不存在secboot sign则签名secboot和flashboot
                string(REPLACE "-" "_" SEC_BOOT_CFG ${SEC_BOOT})
                add_custom_target(GENERAT_SECBOOT_SIGNBIN ALL
                    COMMAND ${SIGN_TOOL} 0 ${ROOT_DIR}/build/config/target_config/${CHIP}/sign_config/${SEC_BOOT_CFG}.cfg 1>nul 2>nul
                    COMMENT "sign file:gen sec boot sign file"
                    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
                    DEPENDS GENERAT_BIN
                )
                add_custom_target(GENERAT_SIGNBIN ALL
                    COMMAND ${SIGN_TOOL} 0 ${ROOT_DIR}/build/config/target_config/${CHIP}/sign_config/${BUILD_TARGET_NAME}.cfg 1>nul 2>nul
                    COMMENT "sign file:gen boot sign file"
                    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
                    DEPENDS GENERAT_SECBOOT_SIGNBIN
                )
            endif()
            # 组合secboot与flashboot
            add_custom_target(CONCAT_BIN ALL
                COMMAND ${Python3_EXECUTABLE} ${CONCAT_TOOL} ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin/${SEC_BOOT}/sec_boot_sign.bin ${PROJECT_BINARY_DIR}/flashboot_sign.bin ${SEC_BOOT_SIZE} ${PROJECT_BINARY_DIR}/flashboot_sign_a.bin &&
                        ${Python3_EXECUTABLE} ${CONCAT_TOOL} ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin/${SEC_BOOT}/sec_boot_sign.bin ${PROJECT_BINARY_DIR}/flashboot_sign.bin ${SEC_BOOT_SIZE} ${PROJECT_BINARY_DIR}/flashboot_sign_b.bin
                COMMENT "concat bin"
                WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
                DEPENDS GENERAT_SIGNBIN
            )
        else()
            # 如果没使能了secboot的验签功能
            # 组合secboot与flashboot
            add_custom_target(CONCAT_BIN ALL
                COMMAND ${Python3_EXECUTABLE} ${CONCAT_TOOL} ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin/${SEC_BOOT}/sec_boot.bin ${PROJECT_BINARY_DIR}/flashboot.bin ${SEC_BOOT_SIZE} ${PROJECT_BINARY_DIR}/flashboot.bin
                COMMENT "concat bin"
                WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
                DEPENDS GENERAT_BIN
            )
            # 签名合并后flashboot
            add_custom_target(GENERAT_SIGNBIN ALL
                COMMAND ${SIGN_TOOL} 0 ${ROOT_DIR}/build/config/target_config/${CHIP}/sign_config/${BUILD_TARGET_NAME}.cfg 1>nul 2>nul &&
                        ${CP} ${PROJECT_BINARY_DIR}/flashboot_sign_a.bin ${PROJECT_BINARY_DIR}/flashboot_sign_b.bin
                COMMENT "sign file:gen boot sign file"
                WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
                DEPENDS CONCAT_BIN
            )
        endif()
    elseif(NOT ${USE_CUSTOM_SIGN}) # 如果没有使能使用自定义的签名方式，则执行下面的签名内容，使能则需要自定义实现 GENERAT_SIGNBIN
        # 预留concat bin命令，后续使用.
        add_custom_target(CONCAT_BIN ALL
            COMMENT "concat bin"
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
            DEPENDS GENERAT_BIN
        )
        # 非secboot方案，只签名flashboot，生成flashboot a, flashboot b.
        add_custom_target(GENERAT_SIGNBIN ALL
            COMMAND ${SIGN_TOOL} 0 ${ROOT_DIR}/build/config/target_config/${CHIP}/sign_config/${BUILD_TARGET_NAME}.cfg 1>nul 2>nul &&
                    ${CP} ${PROJECT_BINARY_DIR}/flashboot_sign_a.bin ${PROJECT_BINARY_DIR}/flashboot_sign_b.bin
            COMMENT "sign file:gen boot sign file"
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
            DEPENDS GENERAT_BIN
        )
    endif()
    if (${UPDATE_BIN})
        # 如果使能了自动更新flashboot到二进制目录，判断二进制目录有效性，不存在则创建
        string(REPLACE "_" "-" TARGET_DIR ${BUILD_TARGET_NAME})
        if (NOT EXISTS ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin)
            file(MAKE_DIRECTORY ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin)
        endif()
        if (NOT EXISTS ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin/${TARGET_DIR})
            file(MAKE_DIRECTORY ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin/${TARGET_DIR})
        endif()
        # flashboot镜像拷贝到二进制目录
        add_custom_target(COPY_SIGNBIN ALL
            COMMAND ${CP} ${ROOT_DIR}/output/${CHIP}/acore/${TARGET_DIR}/flashboot_sign_a.bin ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin/${TARGET_DIR}/flashboot_sign_a.bin  &&
                    ${CP} ${ROOT_DIR}/output/${CHIP}/acore/${TARGET_DIR}/flashboot_sign_a.bin ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin/${TARGET_DIR}/flashboot_sign_b.bin
            COMMENT "copy bin file"
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
            DEPENDS CONCAT_BIN GENERAT_SIGNBIN
        )
    endif()
endif()
elseif (${TARGET_NAME} STREQUAL "loaderboot")
set(BUILD_TARGET_NAME_CFG ${ROOT_DIR}/build/config/target_config/${CHIP}/sign_config/${BUILD_TARGET_NAME}.cfg)
if (EXISTS ${BUILD_TARGET_NAME_CFG})
    message(STATUS "BUILD_TARGET_NAME_CFG existed")
    # 创建临时配置文件，替换 {SDK_DIR} 为实际路径
    set(TEMP_BUILD_TARGET_NAME_CFG ${PROJECT_BINARY_DIR}/${BUILD_TARGET_NAME}_temp.cfg)
    file(READ ${BUILD_TARGET_NAME_CFG} BUILD_TARGET_NAME_CFG_CONTENT)
    string(REPLACE "{SDK_DIR}" "${ROOT_DIR}" BUILD_TARGET_NAME_CFG_CONTENT "${BUILD_TARGET_NAME_CFG_CONTENT}")
    file(WRITE ${TEMP_BUILD_TARGET_NAME_CFG} "${BUILD_TARGET_NAME_CFG_CONTENT}")

    add_custom_target(GENERAT_SIGNBIN ALL
        COMMAND ${SIGN_TOOL} 0 ${TEMP_BUILD_TARGET_NAME_CFG} 1>nul 2>nul
        COMMENT "sign file:gen boot sign file 00"
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        DEPENDS GENERAT_BIN
    )
    if (TARGET COPY_BIN)
        add_dependencies(GENERAT_SIGNBIN COPY_BIN)
    endif()

    set(ROOT_PUB_KEY_CFG ${ROOT_DIR}/build/config/target_config/${CHIP}/sign_config/root_pubk.cfg)
    set(ADD_KEY_HEADER ${ROOT_DIR}/build/config/target_config/${CHIP}/script/add_key_header.py)
    if (EXISTS ${ROOT_PUB_KEY_CFG} AND EXISTS ${ADD_KEY_HEADER})
        # 创建临时配置文件，替换 {SDK_DIR} 为实际路径
        set(TEMP_ROOT_PUB_KEY_CFG ${PROJECT_BINARY_DIR}/root_pubk_temp.cfg)
        file(READ ${ROOT_PUB_KEY_CFG} ROOT_PUB_KEY_CFG_CONTENT)
        string(REPLACE "{SDK_DIR}" "${ROOT_DIR}" ROOT_PUB_KEY_CFG_CONTENT "${ROOT_PUB_KEY_CFG_CONTENT}")
        file(WRITE ${TEMP_ROOT_PUB_KEY_CFG} "${ROOT_PUB_KEY_CFG_CONTENT}")

        add_custom_target(GENERAT_KEYBIN ALL
            COMMAND ${SIGN_TOOL} 1 ${TEMP_ROOT_PUB_KEY_CFG}
            COMMAND ${Python3_EXECUTABLE} ${ADD_KEY_HEADER} ${TEMP_ROOT_PUB_KEY_CFG} ${TEMP_BUILD_TARGET_NAME_CFG}
            COMMENT "add root key:gen boot sign file 11"
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
            DEPENDS GENERAT_SIGNBIN
        )
    endif()
    if (NOT EXISTS ${ROOT_PUB_KEY_CFG} AND EXISTS ${ADD_KEY_HEADER})
        add_custom_target(GENERAT_KEYBIN ALL
            COMMAND ${Python3_EXECUTABLE} ${ADD_KEY_HEADER} ${ROOT_PUB_KEY_CFG} ${TEMP_BUILD_TARGET_NAME_CFG}
            COMMENT "add root key:gen boot sign file"
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
            DEPENDS GENERAT_SIGNBIN
        )
    endif()
    if (${UPDATE_BIN})
        string(REPLACE "_" "-" TARGET_DIR ${BUILD_TARGET_NAME})
        if (NOT EXISTS ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin)
            file(MAKE_DIRECTORY ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin)
        endif()
        if (NOT EXISTS ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin/${TARGET_DIR})
            file(MAKE_DIRECTORY ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin/${TARGET_DIR})
        endif()
        add_custom_target(COPY_SIGNBIN ALL
            COMMAND ${CP} ${ROOT_DIR}/output/${CHIP}/acore/${TARGET_DIR}/loaderboot_sign.bin ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin/${TARGET_DIR}/loaderboot_sign.bin
            COMMENT "copy sign bin file"
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
            DEPENDS GENERAT_SIGNBIN
        )
    endif()
endif()
elseif (${TARGET_NAME} MATCHES "(${COMBINED_REGEX})")
    if (EXISTS ${ROOT_DIR}/build/config/target_config/${CHIP}/sign_config/${BUILD_TARGET_NAME}.cfg)
        set(BUILD_TARGET_NAME_CFG ${ROOT_DIR}/build/config/target_config/${CHIP}/sign_config/${BUILD_TARGET_NAME}.cfg)
        # 创建临时配置文件，替换 {SDK_DIR} 为实际路径
        set(TEMP_BUILD_TARGET_NAME_CFG ${PROJECT_BINARY_DIR}/${BUILD_TARGET_NAME}_temp.cfg)
        file(READ ${BUILD_TARGET_NAME_CFG} BUILD_TARGET_NAME_CFG_CONTENT)
        string(REPLACE "{SDK_DIR}" "${ROOT_DIR}" BUILD_TARGET_NAME_CFG_CONTENT "${BUILD_TARGET_NAME_CFG_CONTENT}")
        file(WRITE ${TEMP_BUILD_TARGET_NAME_CFG} "${BUILD_TARGET_NAME_CFG_CONTENT}")

        add_custom_target(GENERAT_SIGNBIN ALL
            COMMAND ${SIGN_TOOL} 0 ${TEMP_BUILD_TARGET_NAME_CFG} 1>nul 2>nul
            COMMENT "sign file:gen boot sign file"
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
            DEPENDS GENERAT_BIN
        )
        if (TARGET COPY_BIN)
            add_dependencies(GENERAT_SIGNBIN COPY_BIN)
        endif()
    endif()
endif()
if (${CHIP} STREQUAL "ws63")
add_custom_target(WS63_GENERAT_SIGNBIN ALL
    COMMAND ${Python3_EXECUTABLE} ${ROOT_DIR}/build/config/target_config/${CHIP}/sign_config/params_and_bin_sign.py ${TARGET_NAME}
    COMMENT "ws63 image sign"
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    DEPENDS GENERAT_BIN
)

if(TARGET GENERAT_ROM_PATCH)
    add_dependencies(WS63_GENERAT_SIGNBIN GENERAT_ROM_PATCH)
endif()
endif()

if (${CHIP} STREQUAL "ws53")
    set(PACK_CCORE_NAME "default")
    if (${USE_KCONFIG} STREQUAL "True" AND ${BUILD_TARGET_NAME} STREQUAL "ws53_liteos_app")
        set(MCU_MODE_2_SEARCH_TEXT "#define CONFIG_MCU_MODE_2 1")
        file(READ ${PROJECT_BINARY_DIR}/mconfig.h MCONFIG_H_CONTENT)
        if(MCONFIG_H_CONTENT MATCHES ${MCU_MODE_2_SEARCH_TEXT})
            set(PACK_CCORE_NAME "control_ws53_lite.bin")
        endif()
    endif()
add_custom_target(WS53_GENERAT_SIGNBIN ALL
    COMMAND ${Python3_EXECUTABLE} ${ROOT_DIR}/build/config/target_config/${CHIP}/sign_config/params_and_bin_sign.py ${BUILD_TARGET_NAME} ${BIN_NAME} ${PACK_CCORE_NAME}
    COMMENT "ws53 image sign"
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    DEPENDS GENERAT_BIN
)

if(TARGET GENERAT_ROM_PATCH)
    add_dependencies(WS53_GENERAT_SIGNBIN GENERAT_ROM_PATCH)
endif()
endif()

if(TARGET GENERAT_ROM_PATCH AND TARGET GENERAT_SIGNBIN)
    add_dependencies(GENERAT_SIGNBIN GENERAT_ROM_PATCH)
endif()

if (${TARGET_NAME} STREQUAL "sec_boot")
    if (${UPDATE_BIN})
        string(REPLACE "_" "-" TARGET_DIR ${BUILD_TARGET_NAME})
        if (NOT EXISTS ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin)
            file(MAKE_DIRECTORY ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin)
        endif()
        if (NOT EXISTS ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin/${TARGET_DIR})
            file(MAKE_DIRECTORY ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin/${TARGET_DIR})
        endif()
        add_custom_target(COPY_SEC_BOOTBIN ALL
            COMMAND ${CP} ${ROOT_DIR}/output/${CHIP}/acore/${TARGET_DIR}/sec_boot.bin ${ROOT_DIR}/interim_binary/${CHIP}/bin/boot_bin/${TARGET_DIR}/sec_boot.bin
            COMMENT "copy bin file"
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
            DEPENDS GENERAT_BIN
        )
    endif()
endif()

if(${GEN_SEC_BIN} AND ${GEN_SEC_BIN} STREQUAL "True")
    if(${BUILD_PLATFORM} MATCHES "linux")
        if (EXISTS ${COMPILER_ROOT}/lib/sec/${IMAGE_VERSION}/libsec_image.so)
            execute_process(COMMAND ${CP} ${COMPILER_ROOT}/lib/sec/${IMAGE_VERSION}/libsec_image.so ${COMPILER_ROOT}/lib/sec/libsec_image.so)
        endif()
        elseif(${BUILD_PLATFORM} MATCHES "windows")
        if (EXISTS ${COMPILER_ROOT}/lib/sec/${IMAGE_VERSION}/libsec_image.pyd)
            execute_process(COMMAND ${CP} ${COMPILER_ROOT}/lib/sec/${IMAGE_VERSION}/libsec_image.pyd ${COMPILER_ROOT}/lib/sec/libsec_image.pyd)
        endif()
    endif()
    if(${CORE} STREQUAL "acore")
        if (TARGET GENERAT_SIGNBIN)
            add_custom_target(GENERAT_SEC_IMAGE ALL
                COMMAND ${CMAKE_OBJCOPY} --enable_sec ${PROJECT_BINARY_DIR}/${BIN_NAME}_sign.bin
                WORKING_DIRECTORY ${COMPILER_ROOT}/bin
                DEPENDS GENERAT_SIGNBIN
            )
        else()
            add_custom_target(GENERAT_SEC_IMAGE ALL
                COMMAND ${CMAKE_OBJCOPY} --enable_sec ${PROJECT_BINARY_DIR}/${BIN_NAME}.bin
                WORKING_DIRECTORY ${COMPILER_ROOT}/bin
                DEPENDS GENERAT_BIN
            )
        endif()
    elseif(${CORE} STREQUAL "bt_core")
        add_custom_target(GENERAT_SEC_IMAGE ALL
            COMMAND ${SEC_OBJCPY_TOOL} --enable_sec ${PROJECT_BINARY_DIR}/${BIN_NAME}.bin
            WORKING_DIRECTORY ${SEC_TOOL_DIR}
            DEPENDS GENERAT_ROM_PATCH
        )
    endif()
endif()

