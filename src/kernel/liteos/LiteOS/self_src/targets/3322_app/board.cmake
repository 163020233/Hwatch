# C sources
set(CFBB_DRV_INCLUDE
    ${ROOT_DIR}/drivers/chips/${CHIP}/include/shared/
    ${ROOT_DIR}/drivers/chips/${CHIP}/include/
    ${ROOT_DIR}/middleware/utils/common_headers/
    ${ROOT_DIR}/include
)

list(APPEND LOCAL_SRCS_y ${USER_SRC_TEMP})
list(APPEND MODULE_INCLUDE_PUB ${CMAKE_CURRENT_SOURCE_DIR}/include)
list(APPEND MODULE_INCLUDE_PUB ${CFBB_DRV_INCLUDE})

list(APPEND MODULE_INCLUDE_PUB ${ROOT_DIR}/ohos/third_party/lwip/lwip_v2.1.3/src/include)
list(APPEND MODULE_INCLUDE_PUB ${ROOT_DIR}/ohos/third_party/lwip/lwip_adapter/liteos/src/include)

list(APPEND MODULE_INCLUDE_PUB ${ROOT_DIR}/include/osal/lock/)
list(APPEND MODULE_INCLUDE_PUB ${ROOT_DIR}/include/osal/schedule/)
list(APPEND MODULE_CMACRO_PUB LWIP_CONFIG_FILE=\"lwip/lwipopts_default.h\")

string(FIND "${TARGET_COMMAND}" "diting" IS_DITING_TARGET)
string(FIND "${TARGET_COMMAND}" "xts" IS_XTS_TARGET)
string(FIND "${TARGET_COMMAND}" "native-js" IS_NATIVE_JS_TARGET)
if(NOT ${IS_DITING_TARGET} EQUAL -1)
    if(NOT ${IS_XTS_TARGET} EQUAL -1 OR NOT ${IS_NATIVE_JS_TARGET} EQUAL -1)
    else()
        message(STATUS "Adding LWIP_SHARED_CONFIG_FILE MACRO")
        list(APPEND MODULE_CMACRO_PUB LWIP_SHARED_CONFIG_FILE=\"lwip/lwipopts_shared.h\")
    endif()
endif()
