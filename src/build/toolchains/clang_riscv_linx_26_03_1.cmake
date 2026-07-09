#===============================================================================
# @brief    cmake toolchains
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

function(find_env_exe var full_path exe)
    if(USE_LOCAL_COMPILER)
        file(TO_CMAKE_PATH ${full_path} full_path)
        set(${var} ${full_path}/${exe} CACHE PATH "" FORCE)
    else()
        find_program(${var} ${exe})
    endif()
endfunction(find_env_exe)

# 此版本编译器默认使用lld链接
set(USE_LLD 0)
if(DEFINED LINKFLAGS)
    # 使用字符串查找方法检查 -fuse-ld 是否在 LINKFLAGS 中
    string(FIND "${LINKFLAGS}" "-fuse-ld" FUSE_LD_INDEX)
    if(FUSE_LD_INDEX EQUAL -1)
        set(USE_LLD 1)
    endif()
else()
    set(USE_LLD 1)
endif()

set(CMAKE_CC clang)

# Windows cmd-line limit is 32767 chars; compile commands with hundreds of
# -D/-I flags exceed it.  Force Ninja to use response files (@file) instead.
if(CMAKE_HOST_WIN32)
    set(CMAKE_NINJA_FORCE_RESPONSE_FILE ON CACHE BOOL "" FORCE)
endif()
# Bare-metal cross-compile: cmake's ABI-detection links a test executable and
# infers implicit libs (e.g. -lc -lgloss libSclang_rt) that duplicate what
# STD_LIBS already provides inside --start-group/--end-group, causing
# duplicate-symbol errors with lld.  Use STATIC_LIBRARY mode so cmake cannot
# link a test executable and therefore detects no implicit libs.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
if(USE_LLD)
set(CROSS_COMPILE_PREFIX "llvm")
else()
set(CROSS_COMPILE_PREFIX "riscv32/riscv32-linux-musl")
endif()

# Self-locate the SDK root from this toolchain file's own path so the
# compiler resolves correctly in both in-tree and out-of-tree builds.
# clang_riscv_linx_26_03_1.cmake lives at <SDK>/build/toolchains/
if(NOT DEFINED ROOT_DIR)
    get_filename_component(ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
endif()
set(_COMPILER_BASE ${ROOT_DIR}/tools/bin/compiler/bisheng_26_03_1)
set(COMPILER_ROOT     ${_COMPILER_BASE}/BiSheng-llvm-15.0.4-riscv-x86-linux-26.03.1)
set(COMPILER_ROOT_WIN ${_COMPILER_BASE}/BiSheng-llvm-15.0.4-riscv-x86-win-26.03.1)

set(OBJDUMP_PATH ${COMPILER_ROOT}/bin/riscv32)

if((EXISTS ${COMPILER_ROOT}) OR (EXISTS ${COMPILER_ROOT_WIN}))
    set(USE_LOCAL_COMPILER TRUE)
else()
    set(USE_LOCAL_COMPILER FALSE)
endif()

if(${BUILD_PLATFORM} MATCHES "linux")
    MESSAGE(STATUS "BUILD_PLATFORM: " ${BUILD_PLATFORM})
    find_env_exe(CMAKE_C_COMPILER           ${COMPILER_ROOT}/bin           clang)
    find_env_exe(CMAKE_ASM_COMPILER         ${COMPILER_ROOT}/bin           clang)
    find_env_exe(CMAKE_CXX_COMPILER         ${COMPILER_ROOT}/bin           clang++)
    find_env_exe(CMAKE_LINKER               ${COMPILER_ROOT}/bin           ld.lld)
    find_env_exe(CMAKE_AR                   ${COMPILER_ROOT}/bin           ${CROSS_COMPILE_PREFIX}-ar)
    find_env_exe(CMAKE_C_COMPILER_AR        ${COMPILER_ROOT}/bin           ${CROSS_COMPILE_PREFIX}-ar)
    find_env_exe(CMAKE_ASM_COMPILER_AR      ${COMPILER_ROOT}/bin           ${CROSS_COMPILE_PREFIX}-ar)

    find_env_exe(CMAKE_RANLIB               ${COMPILER_ROOT}/bin           ${CROSS_COMPILE_PREFIX}-ranlib)
    find_env_exe(CMAKE_C_COMPILER_RANLIB    ${COMPILER_ROOT}/bin           ${CROSS_COMPILE_PREFIX}-ranlib)
    find_env_exe(CMAKE_ASM_COMPILER_RANLIB  ${COMPILER_ROOT}/bin           ${CROSS_COMPILE_PREFIX}-ranlib)

    find_env_exe(CMAKE_STRIP                ${COMPILER_ROOT}/bin           ${CROSS_COMPILE_PREFIX}-strip)
    find_env_exe(CMAKE_NM                   ${COMPILER_ROOT}/bin           ${CROSS_COMPILE_PREFIX}-nm)
    find_env_exe(CMAKE_OBJCOPY              ${COMPILER_ROOT}/bin           ${CROSS_COMPILE_PREFIX}-objcopy)
    find_env_exe(CMAKE_OBJDUMP              ${COMPILER_ROOT}/bin           ${CROSS_COMPILE_PREFIX}-objdump)
    find_env_exe(CMAKE_READELF              ${COMPILER_ROOT}/bin           ${CROSS_COMPILE_PREFIX}-readelf)
    find_env_exe(CMAKE_DWARFDUMP            ${COMPILER_ROOT}/bin           ${CROSS_COMPILE_PREFIX}-dwarfdump)
elseif(${BUILD_PLATFORM} MATCHES "windows")
    MESSAGE(STATUS "BUILD_PLATFORM: " ${BUILD_PLATFORM})
    set(COMPILER_ROOT  ${COMPILER_ROOT_WIN})
    # lld.exe exists in the Win toolchain but clang looks for ld.lld.exe by name;
    # use -fuse-ld=lld so clang finds lld.exe directly.
    if(USE_LLD)
        set(CMAKE_EXE_LINKER_FLAGS "-fuse-ld=lld" CACHE STRING "" FORCE)
    endif()
    find_env_exe(CMAKE_C_COMPILER           ${COMPILER_ROOT}/bin           clang.exe)
    find_env_exe(CMAKE_ASM_COMPILER         ${COMPILER_ROOT}/bin           clang.exe)
    find_env_exe(CMAKE_CXX_COMPILER         ${COMPILER_ROOT}/bin           clang++.exe)
    find_env_exe(CMAKE_LINKER               ${COMPILER_ROOT}/bin           ${CROSS_COMPILE_PREFIX}-ld.exe)
    find_env_exe(CMAKE_AR                   ${COMPILER_ROOT}/bin           ${CROSS_COMPILE_PREFIX}-ar.exe)
    find_env_exe(CMAKE_C_COMPILER_AR        ${COMPILER_ROOT}/bin           ${CROSS_COMPILE_PREFIX}-ar.exe)
    find_env_exe(CMAKE_ASM_COMPILER_AR      ${COMPILER_ROOT}/bin           ${CROSS_COMPILE_PREFIX}-ar.exe)

    find_env_exe(CMAKE_RANLIB               ${COMPILER_ROOT}/bin           ${CROSS_COMPILE_PREFIX}-ranlib.exe)
    find_env_exe(CMAKE_C_COMPILER_RANLIB    ${COMPILER_ROOT}/bin           ${CROSS_COMPILE_PREFIX}-ranlib.exe)
    find_env_exe(CMAKE_ASM_COMPILER_RANLIB  ${COMPILER_ROOT}/bin           ${CROSS_COMPILE_PREFIX}-ranlib.exe)

    find_env_exe(CMAKE_STRIP                ${COMPILER_ROOT}/bin           ${CROSS_COMPILE_PREFIX}-strip.exe)
    find_env_exe(CMAKE_NM                   ${COMPILER_ROOT}/bin           ${CROSS_COMPILE_PREFIX}-nm.exe)
    find_env_exe(CMAKE_OBJCOPY              ${COMPILER_ROOT}/bin           ${CROSS_COMPILE_PREFIX}-objcopy.exe)
    find_env_exe(CMAKE_OBJDUMP              ${COMPILER_ROOT}/bin           ${CROSS_COMPILE_PREFIX}-objdump.exe)
    find_env_exe(CMAKE_READELF              ${COMPILER_ROOT}/bin           ${CROSS_COMPILE_PREFIX}-readelf.exe)
    find_env_exe(CMAKE_DWARFDUMP            ${COMPILER_ROOT}/bin           ${CROSS_COMPILE_PREFIX}-dwarfdump.exe)
    set(LIB_GCC "" CACHE PATH "" FORCE)
endif()

# When no vendored compiler is found under ROOT_DIR, find_env_exe() falls back
# to find_program() and resolves the compiler from PATH (e.g. an fbb/hs-fbb
# managed toolchain under ~/.hs-fbb). COMPILER_ROOT/COMPILER_ROOT_WIN above
# still point at the (nonexistent) vendored path in that case, which breaks
# anything derived from them (e.g. LITEOS_COMPILER_GCC_INCLUDE's C++ stdlib
# include path). Re-derive COMPILER_ROOT from where the compiler actually was
# found so those downstream paths stay correct.
if(NOT USE_LOCAL_COMPILER)
    get_filename_component(_ACTUAL_COMPILER_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
    get_filename_component(COMPILER_ROOT "${_ACTUAL_COMPILER_BIN_DIR}/.." ABSOLUTE)
    if(${BUILD_PLATFORM} MATCHES "windows")
        set(COMPILER_ROOT_WIN ${COMPILER_ROOT})
    endif()
    message(STATUS "Local vendored compiler not found; using resolved system compiler root: ${COMPILER_ROOT}, compiler: ${CMAKE_C_COMPILER}")
endif()

function(correct_multilib in_var out_var)
    #[[ 通过-print-libgcc-file-name选项获取rt库类型，存在不准确的情况，性能和codesize存在差异, 这里进行手动修正
        -march=rv32imcxlinxma_xlinxmb_xlinxmc -mabi=ilp32获取的是rv32imc_ilp32，修正为rv32imcxlinxma_xlinxmb_xlinxmc_xlinxmd_ilp32
        -march=rv32imfcxlinxma_xlinxmb_xlinxmc -mabi=ilp32获取的是rv32imfc_ilp32，修正为rv32imfcxlinxma_xlinxmb_xlinxmc_xlinxmd_ilp32
        -march=rv32imfcxlinxma_xlinxmb_xlinxmc -mabi=ilp32f获取的是rv32imfc_ilp32f, 修正为rv32imfcxlinxma_xlinxmb_xlinxmc_xlinxmd_ilp32f
        -march=rv32imfdcxlinxma_xlinxmb_xlinxmc -mabi=ilp32d获取的是rv32imfdc_ilp32d，修正为rv32imfdcxlinxma_xlinxmb_xlinxmc_xlinxmd_ilp32d
    ]]
    if (in_var MATCHES "^(rv32imc_ilp32|rv32imfc_ilp32|rv32imfc_ilp32f|rv32imfdc_ilp32d)$")
        string(REPLACE "_ilp32" "xlinxma_xlinxmb_xlinxmc_xlinxmd_ilp32" tmp "${in_var}")
    else()
        set(tmp "${in_var}")
    endif()

    string(REGEX MATCHALL "-(march|mabi)=[a-zA-Z0-9_]+" ARCH_CFLAGS "${CCFLAGS}")
    if(WIN32)
        set(NULL_DEVICE NUL)
    else()
        set(NULL_DEVICE /dev/null)
    endif()
    execute_process(
        COMMAND ${CMAKE_C_COMPILER} ${ARCH_CFLAGS} -E -dM -xc ${NULL_DEVICE}
        OUTPUT_VARIABLE PREDEF_MACROS
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(PREDEF_MACROS MATCHES "#define __riscv_b ([0-9]+)")
        if(CMAKE_MATCH_1 GREATER_EQUAL 1000000)
            string(REGEX REPLACE "^(rv32im[fdc]+)" "\\1b" tmp "${tmp}")
        endif()
    endif()
    set(${out_var} "${tmp}" PARENT_SCOPE)
endfunction()

if(DEFINED CMAKE_C_COMPILER)
    execute_process(
        COMMAND ${CMAKE_C_COMPILER} ${CCFLAGS} --rtlib=compiler-rt -print-libgcc-file-name
        OUTPUT_VARIABLE LIBGCC_PATH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    get_filename_component(MULTILIB_DIR "${LIBGCC_PATH}" DIRECTORY) # 自动获取编译路径
    get_filename_component(MULTILIB_NAME "${MULTILIB_DIR}" NAME)
    get_filename_component(SECOND_DIR "${MULTILIB_DIR}" DIRECTORY)
    get_filename_component(LIB_NAME "${SECOND_DIR}" NAME)  # 获取lib或lib-lld
    STRING(REGEX REPLACE "(.+)/.+" "\\1" CMAKE_C_COMPILER_PATH ${CMAKE_C_COMPILER})
    correct_multilib("${MULTILIB_NAME}" out_var)
    if (out_var)
        set(MULTILIB_NAME ${out_var})
    endif()
    
    set(LIB_C ${CMAKE_C_COMPILER_PATH}/../riscv32-elf/${LIB_NAME}/${MULTILIB_NAME} CACHE PATH "" FORCE)
endif()
