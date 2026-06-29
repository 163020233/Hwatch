# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.

if(CMAKE_TOOLCHAIN_FILE)
    message("CMAKE_TOOLCHAIN_FILE is defined ahead, skip auto-config compiler")
    if(CMAKE_HOST_WIN32)
        execute_process(
            COMMAND ${CMAKE_C_COMPILER} --version
            OUTPUT_VARIABLE _CLANG_VER_OUT
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
        )
        string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" CLANG_VERISON_OUT "${_CLANG_VER_OUT}")
        if(NOT CLANG_VERISON_OUT)
            message(FATAL_ERROR "Get clang version failed.")
        endif()
        set(CLANG_VERISON_RET 0)
    else()
    execute_process(
    COMMAND bash -c "${RTOSLITETOPDIR}/build/scripts/get_compiler_version.sh ${CMAKE_C_COMPILER}"
    RESULT_VARIABLE CLANG_VERISON_RET
    OUTPUT_VARIABLE CLANG_VERISON_OUT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    if(CLANG_VERISON_RET)
        message(FATAL_ERROR "Get clang version failed.")
        endif()
    endif()
    set(VERSION_NUM ${CLANG_VERISON_OUT} CACHE STRING "CLANG VERSION" FORCE)
    return()
endif()

if(LOSCFG_COMPILER_LLVM_BINUTILS)
    set(CROSS_COMPILE llvm-)
    set(CROSS_LINKER ld.lld)
else()
    if(LOSCFG_COMPILER_TOOLCHAIN_MUSL)
        set(CROSS_COMPILE riscv32-linux-musl-)
    elseif(LOSCFG_COMPILER_TOOLCHAIN_UNKNOWN)
        set(CROSS_COMPILE riscv32-unknown-elf-)
    elseif(LOSCFG_COMPILER_LINGLONG)
        set(CROSS_COMPILE linglong-elf-)
    endif()
    set(CROSS_LINKER ${CROSS_COMPILE}ld)
endif()

set(CC       clang)
set(GPP      clang++)
set(AS       ${CROSS_COMPILE}as)
set(AR       ${CROSS_COMPILE}ar)
set(LD       ${CROSS_LINKER})
set(OBJCOPY  ${CROSS_COMPILE}objcopy)
set(OBJDUMP  ${CROSS_COMPILE}objdump)
set(READELF  ${CROSS_COMPILE}readelf)
set(SIZE     ${CROSS_COMPILE}size)
set(NM       ${CROSS_COMPILE}nm)

set(CMAKE_AR             ${CROSS_COMPILE}ar)
set(CMAKE_C_COMPILER     clang)
set(CMAKE_ASM_COMPILER   ${CROSS_COMPILE}as)
set(CMAKE_OBJCOPY        ${CROSS_COMPILE}objcopy)
set(CMAKE_OBJDUMP        ${CROSS_COMPILE}objdump)
set(CMAKE_READELF        ${CROSS_COMPILE}readelf)
set(CMAKE_CXX_COMPILER   clang++)
set(CMAKE_LINKER         ${CROSS_LINKER})
set(STRIP                ${CROSS_COMPILE}STRIP)

execute_process(
    COMMAND bash -c "sh ${RTOSLITETOPDIR}/build/scripts/get_compiler_version.sh ${CC}"
    RESULT_VARIABLE CLANG_VERISON_RET
    OUTPUT_VARIABLE CLANG_VERISON_OUT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
if(CLANG_VERISON_RET)
    message(FATAL_ERROR "Get clang version failed.")
endif()
set(VERSION_NUM ${CLANG_VERISON_OUT} CACHE STRING "CLANG VERSION" FORCE)

# Check if input compiler is availible
execute_process(
    COMMAND bash -c "which ${CC}"
    RESULT_VARIABLE IS_CC_NON_AVAIL
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
if(IS_CC_NON_AVAIL)
    message(FATAL_ERROR "compiler ${COMPILE_NAME} is not in the ENV")
endif()

# Check readelf
check_program_exists(${READELF} READELF_IS_EXISTED)

# Get compiler path
execute_process(
    COMMAND bash -c "sh ${RTOSLITETOPDIR}/build/scripts/get_compiler_path.sh ${CC}"
    RESULT_VARIABLE CLANG_PATH_RET
    OUTPUT_VARIABLE CLANG_PATH_OUT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    WORKING_DIRECTORY ${LITEOSTOPDIR}
)
if(CLANG_PATH_RET)
    message(FATAL_ERROR "Get clang path failed.")
endif()

set(LITEOS_COMPILER_PATH ${CLANG_PATH_OUT} CACHE STRING "CLANG PATH" FORCE)

# Note that the path to the library may vary depending on the compiler version.
# The latest compiler version is 1.2.0.B038, the path to the library is as follows:

if(LOSCFG_COMPILER_LINGLONG)
    set(CLANG_GCCLIB_PATH ${LITEOS_COMPILER_PATH}/linglong-elf/lib)
    set(CLANG_GXXLIB_PATH ${LITEOS_COMPILER_PATH}/linglong-elf/lib)
    set(CLANG_CXXINCLUDE ${LITEOS_COMPILER_PATH}/include/c++/v1 ${LITEOS_COMPILER_PATH}/linglong-elf/include)
elseif(LOSCFG_COMPILER_LLVM_BINUTILS AND CONFIG_INTERNAL_OS_CORTEX_R5)
    set(CLANG_GCCLIB_PATH ${LITEOS_COMPILER_PATH}/armv7-r-none-eabi/lib)
    set(CLANG_GXXLIB_PATH ${LITEOS_COMPILER_PATH}/armv7-r-none-eabi/lib)
    set(CLANG_CXXINCLUDE ${LITEOS_COMPILER_PATH}/armv7-r-none-eabi/include)
elseif(LOSCFG_ARCH_ARM_AARCH32)
    set(CLANG_GXXLIB_PATH ${LITEOS_COMPILER_PATH}/newlib/arm-none-eabi/lib)
    set(CLANG_GCCLIB_PATH ${LITEOS_COMPILER_PATH}/newlib/arm-none-eabi/lib)
    set(CLANG_CXXINCLUDE ${LITEOS_COMPILER_PATH}/newlib/arm-none-eabi/include)
    if(LOSCFG_COMPILER_LLVM_BINUTILS)
        # Set target emulation
        list(APPEND LITEOS_LD_OPTS -marmelf)
    endif()
elseif(LOSCFG_COMPILER_LLVM_CLANG)
    if (LOSCFG_ARCH_ARM_AARCH64)
        set(CLANG_GXXLIB_PATH ${LITEOS_COMPILER_PATH}/lib/clang/${VERSION_NUM}/lib/aarch64-none-linux-elf)
        set(CLANG_GCCLIB_PATH ${LITEOS_COMPILER_PATH}/lib/clang/${VERSION_NUM}/lib/aarch64-none-linux-elf)
        set(CLANG_CXXINCLUDE ${LITEOS_COMPILER_PATH}/lib/clang/${VERSION_NUM}/include)
    endif()
else()
    if(LOSCFG_ARCH_FPU_ENABLE)
        if (LOSCFG_ARCH_FPU_DOUBLE)
            if(LOSCFG_LCMP_CUSTOM_INST16_EXTENSIONS AND LOSCFG_LCMP_CUSTOM_INST16_SUB_EXTENSIONS AND LOSCFG_LCMP_CUSTOM_INST32_EXTENSIONS)
                if ("${VERSION_NUM}" VERSION_GREATER "12.0.1")
                    set(CLANG_GCCLIB_PATH ${LITEOS_COMPILER_PATH}/lib/clang/${VERSION_NUM}/lib/rv32imfdcxlinxma_xlinxmb_xlinxmc_xlinxmd_ilp32d)
                    set(CLANG_GXXLIB_PATH ${LITEOS_COMPILER_PATH}/riscv32-elf/lib/rv32imfdcxlinxma_xlinxmb_xlinxmc_xlinxmd_ilp32d)
                else()
                    set(CLANG_GCCLIB_PATH ${LITEOS_COMPILER_PATH}/lib/clang/${VERSION_NUM}/lib/rv32imafdcbxlinxma_xlinxmb_xlinxmc_xlinxmd_ilp32d)
                    set(CLANG_GXXLIB_PATH ${LITEOS_COMPILER_PATH}/riscv32-elf/lib/rv32imafdcbxlinxma_xlinxmb_xlinxmc_xlinxmd_ilp32d)
                endif()
            else()
                set(CLANG_GCCLIB_PATH ${LITEOS_COMPILER_PATH}/lib/clang/${VERSION_NUM}/lib/rv32imfdc_ilp32d)
                set(CLANG_GXXLIB_PATH ${LITEOS_COMPILER_PATH}/riscv32-elf/lib/rv32imfdc_ilp32d)
            endif()
        else()
            if(LOSCFG_LCMP_CUSTOM_INST16_EXTENSIONS AND LOSCFG_LCMP_CUSTOM_INST16_SUB_EXTENSIONS AND LOSCFG_LCMP_CUSTOM_INST32_EXTENSIONS)
                if ("${VERSION_NUM}" VERSION_GREATER "12.0.1")
                    set(CLANG_GCCLIB_PATH ${LITEOS_COMPILER_PATH}/lib/clang/${VERSION_NUM}/lib/rv32imfcxlinxma_xlinxmb_xlinxmc_xlinxmd_ilp32f)
                    set(CLANG_GXXLIB_PATH ${LITEOS_COMPILER_PATH}/riscv32-elf/lib/rv32imfcxlinxma_xlinxmb_xlinxmc_xlinxmd_ilp32f)
                else()
                    set(CLANG_GCCLIB_PATH ${LITEOS_COMPILER_PATH}/lib/clang/${VERSION_NUM}/lib/rv32imafcbxlinxma_xlinxmb_xlinxmc_xlinxmd_ilp32f)
                    set(CLANG_GXXLIB_PATH ${LITEOS_COMPILER_PATH}/riscv32-elf/lib/rv32imafcbxlinxma_xlinxmb_xlinxmc_xlinxmd_ilp32f)
                endif()
            else()
                set(CLANG_GCCLIB_PATH ${LITEOS_COMPILER_PATH}/lib/clang/${VERSION_NUM}/lib/rv32imfc_ilp32f)
                set(CLANG_GXXLIB_PATH ${LITEOS_COMPILER_PATH}/riscv32-elf/lib/rv32imfc_ilp32f)
            endif()
        endif()
    else()
        set(CLANG_GCCLIB_PATH ${LITEOS_COMPILER_PATH}/lib/clang/${VERSION_NUM}/lib/rv32imc_ilp32)
        set(CLANG_GXXLIB_PATH ${LITEOS_COMPILER_PATH}/riscv32-elf/lib/rv32imc_ilp32)
    endif()
    set(CLANG_CXXINCLUDE )
    list(APPEND CLANG_CXXINCLUDE ${LITEOS_COMPILER_PATH}/riscv32-elf/include/c++/v1/)
    list(APPEND CLANG_CXXINCLUDE ${LITEOS_COMPILER_PATH}/riscv32-elf/include/c++/v1/ext/)
endif()

set(LITEOS_COMPILER_GCC_INCLUDE ${LITEOS_COMPILER_PATH}/lib/clang/${VERSION_NUM}/include)
set(LITEOS_COMPILER_CXXLIB_PATH ${CLANG_GXXLIB_PATH})
set(LITEOS_COMPILER_GCCLIB_PATH ${CLANG_GCCLIB_PATH})

list(APPEND LITEOS_CXXINCLUDE
    ${CLANG_CXXINCLUDE}
    ${LITEOS_COMPILER_PATH}/lib/clang/${VERSION_NUM}/include
)
if(LOSCFG_COMPILER_RISCV_CLANG_MUSL)
    string(REGEX REPLACE "([0-9]+)\\.([0-9]+)\\.([0-9]+)" "\\1" COMPILE_MAJOR_VERSION ${VERSION_NUM})
    string(REGEX REPLACE "([0-9]+)\\.([0-9]+)\\.([0-9]+)" "\\3" COMPILE_PATCH_VERSION ${VERSION_NUM})
    math(EXPR COMPILE_MAJOR_VERSION "${COMPILE_MAJOR_VERSION}")
    math(EXPR COMPILE_PATCH_VERSION "${COMPILE_PATCH_VERSION}")
    list(APPEND LITEOS_CMACRO __COMPILE_MAJOR_VERSION__=${COMPILE_MAJOR_VERSION})
    list(APPEND LITEOS_CMACRO __COMPILE_PATCH_VERSION__=${COMPILE_PATCH_VERSION})
endif()

# Check include path is avaiable for now.
# CXX paths to be added. Since LiteOS support for C++ is based on whether those libs
# are compiled for LiteOS. C++ support needs to configured with Kconfigs.
if(NOT EXISTS ${LITEOS_COMPILER_GCC_INCLUDE})
    message(FATAL_ERROR "clang path ${LITEOS_COMPILER_GCC_INCLUDE} not exist.")
endif()

# Check GCC lib path is avaiable for now.
# CXX paths to be added. Since LiteOS support for C++ is based on whether those libs
# are compiled for LiteOS. C++ support needs to configured with Kconfigs.
if(NOT EXISTS ${LITEOS_COMPILER_GCCLIB_PATH})
    message(FATAL_ERROR "gcc path ${LITEOS_COMPILER_GCCLIB_PATH} not exist.")
endif()

