#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
#===============================================================================

if(NOT "CONFIG_NPU_LIB" IN_LIST DEFINES)
    return()
endif()

if("CONFIG_NPU_DEBUG" IN_LIST DEFINES)
set(NPU_LIB_TYPE "debug")
else()
set(NPU_LIB_TYPE "release")
endif()

set(ACL_LIB_DIR ${BIN_DIR}/${CHIP}/libs/npu/${TARGET_COMMAND}/turing)
set(NPU_LIB_DIR ${BIN_DIR}/${CHIP}/libs/npu/${TARGET_COMMAND})
set(NPU_LIB_OUT_DIR ${BIN_DIR}/${CHIP}/libs/npu)

set(NPU_DIR ${CMAKE_BINARY_DIR}/../../../../protocol/npu)
set(ACL_LIB_OUT_DIR ${CMAKE_BINARY_DIR}/../../../../protocol/npu/turing/C11/output/lib64)

if (${BOARD} STREQUAL "evb")
set(NPU_BOARD "asic")
else()
set(NPU_BOARD "fpga")
endif()

set(CFG_DIR ${CMAKE_BINARY_DIR}/../../../../build/config/target_config/3322/npu_cfg)

install_sdk(${BIN_DIR}/${CHIP}/libs/npu/${TARGET_COMMAND}/turing "*.a")
install_sdk(${BIN_DIR}/${CHIP}/libs/npu/${TARGET_COMMAND}/nano_test "*.a")
install_sdk(${BIN_DIR}/${CHIP}/libs/npu/${TARGET_COMMAND}/driver/${NPU_LIB_TYPE}/${NPU_BOARD} "*.a")

if (EXISTS ${NPU_DIR}/npu_nano/scripts)
    add_custom_target(npu_nano_lib
        COMMAND bash build.sh npu ${CFG_DIR}/product.config ${NPU_BOARD} ${NPU_LIB_TYPE}
        COMMAND ${CMAKE_COMMAND} -E rm -rf "${NPU_LIB_DIR}/driver"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${NPU_LIB_DIR}/driver"
        COMMAND ${CMAKE_COMMAND} -E rename "${NPU_LIB_OUT_DIR}/driver" "${NPU_LIB_DIR}/driver"
        WORKING_DIRECTORY ${NPU_DIR}/npu_nano/scripts)
    add_dependencies(${TARGET_NAME} npu_nano_lib)
endif()

if (${NPU_LIB_TYPE} STREQUAL "debug")
if (EXISTS ${NPU_DIR}/npu_test/build.sh)
    add_custom_target(npu_test_lib
        COMMAND bash build.sh non_linux ${CFG_DIR}/product.config
        COMMAND ${CMAKE_COMMAND} -E rm -rf "${NPU_LIB_DIR}/nano_test"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${NPU_LIB_DIR}/nano_test"
        COMMAND ${CMAKE_COMMAND} -E rename "${NPU_LIB_OUT_DIR}/nano_test" "${NPU_LIB_DIR}/nano_test"
        WORKING_DIRECTORY ${NPU_DIR}/npu_test)
    add_dependencies(${TARGET_NAME} npu_test_lib)
endif()
endif()

if (EXISTS ${NPU_DIR}/npu_nano/scripts)
if (EXISTS ${NPU_DIR}/turing/master/cmake OR EXISTS ${NPU_DIR}/turing/C11/cmake)
    add_custom_target(npu_turing_lib
        COMMAND bash build.sh turing ${CFG_DIR}/product.config
        COMMAND ${CMAKE_COMMAND} -E rm -rf "${ACL_LIB_DIR}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${ACL_LIB_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy "${ACL_LIB_OUT_DIR}/*.a" "${ACL_LIB_DIR}"
        WORKING_DIRECTORY ${NPU_DIR}/npu_nano/scripts)
    add_dependencies(${TARGET_NAME} npu_turing_lib)
endif()
endif()

set(NPU_LIBS
    ${ACL_LIB_DIR}/libascendcl.a
    $<IF:$<STREQUAL:${NPU_LIB_TYPE},debug>,${ACL_LIB_DIR}/libge_dbg.a,${ACL_LIB_DIR}/libge_dbg_stub.a>
    $<$<STREQUAL:${NPU_LIB_TYPE},debug>:${ACL_LIB_DIR}/libdatadump.a>
    $<$<STREQUAL:${NPU_LIB_TYPE},debug>:${ACL_LIB_DIR}/libprofcore.a>
    $<$<STREQUAL:${NPU_LIB_TYPE},debug>:${NPU_LIB_DIR}/nano_test/libnano_test.a>
    ${NPU_LIB_DIR}/driver/${NPU_LIB_TYPE}/${NPU_BOARD}/libascend_hal.a
)

target_link_libraries(${TARGET_NAME}
PRIVATE
    -Wl,--start-group
    ${NPU_LIBS}
    -Wl,--end-group
)

