get_filename_component(HOME_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)

set(RTOSLITETOPDIR ${HOME_PATH})

if(NOT DEFINED OS_CONFIG_PATH)
    set(OS_CONFIG_PATH ".config")
else()
    message(FATAL_ERROR "OS_CONFIG_PATH = ${OS_CONFIG_PATH}")
endif()

# #################### 公共函数导入 #################
include(${RTOSLITETOPDIR}/build/libck_cmake/functions/libck_functions.cmake)
import_kconfig(${OS_CONFIG_PATH})

###定义MODULE支持模块编译脚本兼容
set(MODULE ${HOME_PATH}/build/libck_cmake/functions/libck_module.cmake)
set(MODULE_LIBC ${HOME_PATH}/build/libck_cmake/functions/libck_module.cmake)

# 编译器库
set(LIBCK_DEP_LIBS_EXT)
# 源码库
set(LIBCK_DEP_OBJS_INT)

# 使用libck kconfigto head.h
set(OS_PLATFORM_MENUCONFIG_H ${CMAKE_CURRENT_BINARY_DIR}/sre_buildef.h)
execute_process(
    COMMAND python3 ${RTOSLITETOPDIR}/build/libck_lib/kconfig_to_macro.py -f ${OS_CONFIG_PATH} -o
            ${OS_PLATFORM_MENUCONFIG_H} WORKING_DIRECTORY ${RTOSLITETOPDIR}
)

# 生成test_switch.h
set(TEST_CONNFIG ${HOME_PATH}/test_Debug/BMTestProject/converge/build/config/test_config/linx170_sd5091linx_defconfig)
set(TEST_SWITCH_H ${HOME_PATH}/test_Debug/BMTestProject/converge/include/test_switch.h)
execute_process(
    COMMAND python3 ${RTOSLITETOPDIR}/build/libck_lib/kconfig_to_macro.py -f ${TEST_CONNFIG} -o ${TEST_SWITCH_H}
    WORKING_DIRECTORY ${RTOSLITETOPDIR}
)

if("${FULL_BUILD_HUAWEI_SECURE_C_PATH}" STREQUAL "")
    # 默认的安全c路径
    set(FULL_BUILD_HUAWEI_SECURE_C_PATH ${HUAWEI_SECURE_C_DIR})
endif()
# 头文件
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${FULL_BUILD_HUAWEI_SECURE_C_PATH}/include)
include_directories(
    ${RTOSLITETOPDIR}/self_src/include/uapi ${RTOSLITETOPDIR}/self_src/include/uapi/common/base
    ${RTOSLITETOPDIR}/self_src/include/uapi/common/extend ${RTOSLITETOPDIR}/self_src/include/uapi/drb/base
    ${RTOSLITETOPDIR}/self_src/include/uapi/drb/extend ${RTOSLITETOPDIR}/self_src/include/uapi/los
)

set(LITEOS_TARGET vs_server)
set(ENV{CPU_TYPE} sd5091linx)
set(CPU_TYPE sd5091linx)
set(ENV{PLAM_TYPE} linx170)
set(PLAM_TYPE linx170)
set(ENV{HOME_PATH} ${RTOSLITETOPDIR})
set(TOOLCHAIN_DIR /opt/buildtools/bisheng_mobile_mcu_std-202.1.0.b016/linx-llvm-binary-release-musl/bin)

# 源码
include(${RTOSLITETOPDIR}/build/libck_cmake/tool_chain/rtosk_tool_chain.cmake)
set(COMPILE_MODE dbg)
add_subdirectory(self_src)
add_subdirectory(test_Debug/BMTestProject/converge)

# 安全函数库
add_subdirectory(build/libck_image/libsec)

# #############################不同平台，根据需要链接出不同的lib库############
include(${RTOSLITETOPDIR}/build/libck_cmake/tool_chain/${CPU_TYPE}_${PLAM_TYPE}.cmake)

# 链接阶段
set(LD_FILE ${RTOSLITETOPDIR}/CI/Script/make_image/makefile/${CPU_TYPE}.ld.S)
set(LD_COMMAND ${CMAKE_LINKER} -nostdlib -EL -d -static -nostdlib)
set(LIB_PATH -L${COMPILE_LIB_PATH} -L${CMAKE_CURRENT_BINARY_DIR} -L${CMAKE_CURRENT_BINARY_DIR}/build/libck_image/libsec
             -L${CMAKE_CURRENT_BINARY_DIR}/test_Debug/BMTestProject/converge
)
message(LIBCK_DEP_LIBS_EXT = ${LIBCK_DEP_LIBS_EXT})
message(LIBCK_DEP_OBJS_INT = ${LIBCK_DEP_OBJS_INT})

set(LD_COMMAND_LIB ${LIBCK_DEP_OBJS_INT} ${LIBCK_DEP_LIBS_EXT})

list(TRANSFORM LD_COMMAND_LIB PREPEND -l)

set(BUILD_LINKER_PROCESS
    COMMAND ${LD_COMMAND} ${LIB_PATH} -T${LD_FILE} -o ${LITEOS_TARGET} 
    --start-group ${LD_COMMAND_LIB} --end-group
    -Map=${CMAKE_CURRENT_BINARY_DIR}/${LITEOS_TARGET}.map
    COMMAND ${CMAKE_OBJCOPY} -O binary ${CMAKE_CURRENT_BINARY_DIR}/${LITEOS_TARGET} ${CMAKE_CURRENT_BINARY_DIR}/${LITEOS_TARGET}.bin
    COMMAND ${CMAKE_OBJCOPY} --only-keep-debug ${CMAKE_CURRENT_BINARY_DIR}/${LITEOS_TARGET} ${CMAKE_CURRENT_BINARY_DIR}/${LITEOS_TARGET}.dbg
    COMMAND ${CMAKE_OBJDUMP} -d -h ${CMAKE_CURRENT_BINARY_DIR}/${LITEOS_TARGET} > ${CMAKE_CURRENT_BINARY_DIR}/${LITEOS_TARGET}.asm
)

add_custom_target(${LITEOS_TARGET} DEPENDS ${LIBCK_DEP_OBJS_INT} ${BUILD_LINKER_PROCESS})
