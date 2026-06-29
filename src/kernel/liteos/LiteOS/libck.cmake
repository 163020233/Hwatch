############################## 外部传入变量 ##############################
set(CPU_TYPE "$ENV{CPU_TYPE}" ) #类似sd6185
set(PLAM_TYPE "$ENV{PLAM_TYPE}" ) #类似hiavpv200
set(LIB_TYPE "$ENV{LIB_TYPE}"  ) #类似SRE
set(VERSION_ID "$ENV{VERSION_ID}" ) #version id
set(LIBCK_PACKING_PATH "$ENV{LIBCK_PACKING_PATH}" ) #LIBCK_PACKING_PATH
set(COMPILE_OPTION "$ENV{COMPILE_OPTION}" ) #编译选项 空为默认全编译
set(CONFIG_FILE_PATH "$ENV{CONFIG_FILE_PATH}" ) #defconfig及其头文件所在目录
string(TOUPPER ${PLAM_TYPE} PLAM_TYPE_UP) #转大写 HIAVPV200
set(XT_SYSTEM "$ENV{XT_SYSTEM}" )
set(XT_CORE "$ENV{XT_CORE}" )
set(LIB_RUN_TYPE "$ENV{LIB_RUN_TYPE}" )
set(HOME_PATH "$ENV{HOME_PATH}" )
set(TOOLCHAIN_DIR "$ENV{HCC_PATH}")
set(BUILD_MACHINE_PLATFORM "$ENV{BUILD_MACHINE_PLATFORM}" )
set(LIBCK_BINARY_DIR "$ENV{LIBCK_BINARY_DIR}" )
set(COMPILE_MODE "$ENV{COMPILE_MODE}" )
set(FULL_BUILD_HUAWEI_SECURE_C_PATH "$ENV{FULL_BUILD_HUAWEI_SECURE_C_PATH}" )
set(LIBCK_INSTALL_FILE_OPTION "$ENV{LIBCK_INSTALL_FILE_OPTION}" )##LIBCK_INSTALL_FILE_OPTION="SUPER_BUILD";
#用于保存所有的对象库
set(ALL_OBJECT_LIBRARYS "" CACHE STRING INTERNAL)


if("${LIBCK_BINARY_DIR}" STREQUAL "")
  # this branch will be taken
  set(LIBCK_BINARY_DIR ${PROJECT_SOURCE_DIR}/build/output) #类似libck
else()
  set(LIBCK_BINARY_DIR ${LIBCK_BINARY_DIR}/LibCK/tmp/output) #类似libck
endif()

if("${FULL_BUILD_HUAWEI_SECURE_C_PATH}" STREQUAL "")
  # 默认的安全c路径
  set(FULL_BUILD_HUAWEI_SECURE_C_PATH ${HUAWEI_SECURE_C_DIR})
endif()

message("LibCK BINARY_DIR=${LIBCK_BINARY_DIR}")

######该参数的判断逻辑脚本来实现一个XCache的开关######
if (CACHE)
    find_program(CACHE_FOUND ${CACHE})
    set(CACHE_TOOL "${CACHE}")
endif()
if(CACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CACHE_TOOL})
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ${CACHE_TOOL})
endif()

############################## 公共函数定义导入 ##############################
include(${HOME_PATH}/build/libck_cmake/functions/libck_functions.cmake)

#支持加入ASM支持汇编代码   ENABLE_LANGUAGE(ASM)
#进行相应的配置 set(CMAKE_ASM_SOURCE_FILE_EXTENSIONS "s;S;asm;ASM")
############################## 编译参数 ##################################
#下述这些参数的设置要放在项目设置之后！
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${LIBCK_BINARY_DIR}/${CPU_TYPE}/${PLAM_TYPE}/${LIB_RUN_TYPE}") #库文件输出路径
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

############################## 基础信息打印 ##############################
message("CMAKE_C_COMPILER is ${CMAKE_C_COMPILER}")
message("CMAKE_AR is ${CMAKE_AR}")
message("CMAKE_LINKER is ${CMAKE_LINKER}")
message("CMAKE_INSTALL_PREFIX is ${CMAKE_INSTALL_PREFIX}")
message("BUILD_MACHINE_PLATFORM= ${BUILD_MACHINE_PLATFORM}")
message("CONFIG_FILE_PATH= ${CONFIG_FILE_PATH}")

#编译选项开关,会把各步骤实际调用的命令、参数一一列出 (正常默认关闭) set(CMAKE_VERBOSE_MAKEFILE on )
if (${COMPILE_MODE} STREQUAL "dbg") #编译模式：支持dbg、compile_config、dbg_log、Release
add_compile_options("-g") #编译 -g 选项，打开后可以看到调试信息
elseif (${COMPILE_MODE} STREQUAL "dbg_log")
add_compile_options("-DOS_DBG")  #定义源码中OS_DBG宏,用于一些疑难问题定位
add_compile_options("-g")
endif()
message("=============== COMPILE_MODE is ${COMPILE_MODE} ===============")
############################## 通过.config文件引入编译宏 ##################
# #函数import_kconfig把.config文件中的定义转换为cmake变量，用于后续子文件夹中决定是否编译某库
import_kconfig(${CONFIG_FILE_PATH}/defconfig)

include(${HOME_PATH}/build/libck_cmake/tool_chain/rtosk_tool_chain.cmake)
##############################include 头文件##############################
#指向cmake运行的根目录，比如build/sd6185目录  include_directories("${PROJECT_BINARY_DIR}")
#可能和平台相关
include_directories(
	${CONFIG_FILE_PATH}
	${FULL_BUILD_HUAWEI_SECURE_C_PATH}/include
)

###定义MODULE支持模块编译脚本兼容
set(MODULE ${HOME_PATH}/build/libck_cmake/functions/libck_module.cmake)
set(MODULE_LIBC ${HOME_PATH}/build/libck_cmake/functions/libck_module.cmake)
include(${MODULE})

###添加源码目录的cmakelists
add_subdirectory(self_src)
##############################不同平台，根据需要链接出不同的lib库############
include(./build/libck_cmake/tool_chain/${CPU_TYPE}_${PLAM_TYPE}.cmake)