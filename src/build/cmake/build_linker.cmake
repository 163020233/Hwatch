#===============================================================================
# @brief    cmake deal with link script
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

if("${LINK_SCRIPT}" STREQUAL "")
    MESSAGE(FATAL_ERROR "linkscript is not found!")
endif()

function(auto_gen_lds macro_name lib_list)
    file(APPEND ${CMAKE_BINARY_DIR}/auto_gen_libfunc.lds "#define ${macro_name}(func) \\\n")
    set(LIB_FUNC "${${lib_list}}")
    list(TRANSFORM LIB_FUNC PREPEND "    func(*lib")
    list(TRANSFORM LIB_FUNC APPEND ".a:*.o) \\\n")
    file(APPEND ${CMAKE_BINARY_DIR}/auto_gen_libfunc.lds ${LIB_FUNC} "\n")
endfunction()

function(check_target_has_flto target_name enable_lto)
    # 获取目标的所有编译选项
    get_target_property(target_flags ${target_name} COMPILE_OPTIONS)

    # 检查是否存在-flto
    if(target_flags AND "-flto" IN_LIST target_flags)
        set(${enable_lto} TRUE PARENT_SCOPE)
        message(STATUS "Target ${target_name} uses -flto")
    else()
        set(${enable_lto} FALSE PARENT_SCOPE)
        message(STATUS "Target ${target_name} does NOT use -flto")
    endif()
endfunction()

# 添加用于处理链接脚本用的编译宏
set(LDS_DEFINES ${DEFINES})
list(TRANSFORM LDS_DEFINES PREPEND "-D")

# 添加用于处理链接脚本用的编译头文件
if(DEFINED LDS_HEADER)
    list(REMOVE_ITEM LDS_HEADER "")
    list(TRANSFORM LDS_HEADER PREPEND "-I")
else()
    set(LDS_HEADER ${ALL_PUBLIC_HEADER})
    list(REMOVE_DUPLICATES LDS_HEADER)
    list(REMOVE_ITEM LDS_HEADER "")
    list(TRANSFORM LDS_HEADER PREPEND "-I")
endif()
list(APPEND LDS_HEADER -I${CMAKE_BINARY_DIR})
if(DEFINED ROM_RAM_CHECK)
    list(APPEND LDSDEFINES -DROM_BIN_CHECK)
endif()
list(TRANSFORM LDS_HEADER PREPEND "\"")
list(TRANSFORM LDS_HEADER APPEND "\"")
string(JOIN " " LDS_HEADER_STR ${LDS_HEADER})
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/linker_header.srp "${LDS_HEADER_STR}")
auto_gen_lds("LIB_RAM_FUNC" RAM_COMPONENT)
auto_gen_lds("LIB_ROM_FUNC" ROM_COMPONENT)
auto_gen_lds("LIB_LOS_FUNC" LOS_LIB)

if(DEFINED SDK_OUTPUT_PATH)
    string(REPLACE "${PROJECT_SOURCE_DIR}" "${SDK_OUTPUT_PATH}" dest ${LINK_SCRIPT})
    get_filename_component(dest ${dest} DIRECTORY)
    install(FILES ${LINK_SCRIPT} DESTINATION ${dest})
    install_sdk(${LINK_SCRIPT} "*.prelds")
endif()

if(NOT DEFINED ROM_CHECK)
    set(ROM_CHECK False)
endif()

if(${ROM_CHECK})
    list(APPEND LDS_DEFINES "-DROM_CHECK")
endif()

if(${BUILD_ROM_CALLBACK})
list(APPEND LDS_DEFINES "-DBUILD_ROM_CALLBACK")
endif()

if(USE_LLD)
    list(APPEND LDS_DEFINES "-DUSE_LLD")
endif()

if(NOT DEFINED BUILD_ROM_SYMLINK_CHECK)
    set(BUILD_ROM_SYMLINK_CHECK False)
endif()

if(BUILD_ROM_SYMLINK_CHECK)
    list(APPEND LDS_DEFINES "-DBUILD_ROM_SYMLINK_CHECK")
elseif(ROM_SYMLINK)
    list(APPEND LDS_DEFINES "-DROM_SYMLINK")
endif()

add_custom_command(TARGET ${TARGET_NAME} PRE_BUILD
    COMMAND ${CMAKE_C_COMPILER} -P -xc -E -o linker.lds @${CMAKE_CURRENT_BINARY_DIR}/linker_header.srp ${LDS_DEFINES} ${LINK_SCRIPT}
    COMMENT "Generating ${LINK_SCRIPT} -> ${CMAKE_BINARY_DIR}/linker.lds"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    VERBATIM
)

set(STD_LIB_PATH
    ${LIB_GCC}
    ${LIB_C}
    ${LIB_CXX}
)
list(TRANSFORM STD_LIB_PATH PREPEND "-L")

target_link_libraries(${TARGET_NAME}
PRIVATE
    -Wl,--start-group "${STD_LIBS}" -Wl,--end-group
)

target_link_options(${TARGET_NAME}
    PRIVATE
    ${STD_LIB_PATH}
    ${LINKFLAGS}
    -Wl,-T${CMAKE_BINARY_DIR}/linker.lds -nostartfiles
    -Wl,--gc-sections -Wl,-Map=${CMAKE_BINARY_DIR}/${BIN_NAME}.map
)

if(NOT USE_LLD)
target_link_options(${TARGET_NAME}
    PRIVATE
    -Wl,--print-memory-usage
)
else()
list(LENGTH LTO_SRC_LIST LTO_SRC_NUMBER)
check_target_has_flto(${TARGET_NAME} enable_lto)
if(LTO_COMPONENT OR LTO_SRC_NUMBER OR enable_lto)
target_link_options(${TARGET_NAME}
    PRIVATE
    -flto
    ${CCFLAGS}
)
endif()
endif()
