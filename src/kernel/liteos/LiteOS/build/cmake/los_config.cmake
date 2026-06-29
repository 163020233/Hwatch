## global prop define ##
set_property(GLOBAL PROPERTY LITEOS_MODULE_LIBS_PATH_PROP)

## var define ##
set(LITEOS_CMACRO)
set(LITEOS_ASMACRO)
set(LITEOS_CXXMACRO)
set(LITEOS_CXXINCLUDE)
set(LITEOS_DEP_LIBS_EXT)
set(LITEOS_U_INCLUDES)
set(LITEOS_K_INCLUDES)
if((${LOSCFG_PLATFORM_MP18_GNSS}) OR (${LOSCFG_PLATFORM_MP18_BGLE}) OR (${LOSCFG_PLATFORM_TG8_BGLE}))
set(HUAWEI_SECURE_C_DIR ${LITEOS_TOP_DIR}/platform/huawei_secure_c)
else()
set(HUAWEI_SECURE_C_DIR ${LITEOS_TOP_DIR}/../../../middleware/utils/hwsec_c)
endif()
## opensource path ##
set(LITEOS_OPENSOURCE_PATHNAME ${OPENSOURCEDIR})
set(LITEOS_ZLIB_PATH ${LITEOS_OPENSOURCE_PATHNAME}/zlib)
set(LITEOS_LIBC_PATH ${LITEOS_OPENSOURCE_PATHNAME}/musl)
set(LITEOS_CMSIS_PATH ${LITEOS_OPENSOURCE_PATHNAME}/CMSIS_6)

## LiteOS self-developed code compiler options ##
## please do not arbitrarily add -Wno-xxx suppression here ##
set(LITEOS_NEEDED_COMPILE_OPTS -Wextra -Wno-unused-parameter -Wno-missing-field-initializers)
set(LITEOS_SUGGESTED_COMPILE_OPTS -Wunused -Wswitch-default -fvisibility=hidden)
if(NOT LOSCFG_COMPILER_XTENSA_32)
    list(APPEND LITEOS_SUGGESTED_COMPILE_OPTS -Wvla -Wdate-time -Wshadow)
endif()

if (CONFIG_INTERNAL_OS_UNIFIED_COMPILE AND NOT CONFIG_LOSCFG_FS_LITTLEFS)
    list(APPEND LITEOS_SUGGESTED_COMPILE_OPTS -Wundef -Wcast-qual -Wcast-align -Wframe-larger-than=4096)
endif()

set(LITEOS_NON_SECURE_LOCAL_OPTS ${LITEOS_NEEDED_COMPILE_OPTS} ${LITEOS_SUGGESTED_COMPILE_OPTS})

############### configuring compiler ###############
# Introduced compiler binaries which might different from gcc compilers.
# Those compilers need to implement like ${CC} ${GPP} like gcc does.
# Meanwhile Introduced compiler includes and lib paths
# Export : ${LITEOS_COMPILER_GCC_INCLUDE}
#          ${LITEOS_CXXINCLUDE}
#          ${LITEOS_COMPILER_GCCLIB_PATH}
#          ${LITEOS_COMPILER_CXXLIB_PATH}

if(LOSCFG_SAFETY_KERNEL)
    set(KERNEL_PATH kernel_s)
    set(ARCH_PATH arch_s)
    set(TEST_PATH test_s)
else()
    set(KERNEL_PATH kernel_lite)
    set(ARCH_PATH arch_lite)
    set(TEST_PATH test)
endif()

include(${RTOS_LITE_TOP_DIR}/build/cmake/los_config_prelude.cmake)

if(LOSCFG_COMPILER_GCC)
    # Supported GCC Compilers
    # Also support "make CROSS_COMPILE=" to use third party compilers.
    include(${RTOS_LITE_TOP_DIR}/build/cmake/compiler/compiler_gcc_common.cmake)
elseif(LOSCFG_COMPILER_XTENSA_32)
    include(${RTOSLITETOPDIR}/build/cmake/compiler/compiler_xcc_common.cmake)
elseif(LOSCFG_COMPILER_CLANG)
    include(${RTOSLITETOPDIR}/build/cmake/compiler/compiler_clang_common.cmake)
endif()

## variable define ##
set(CUR_OS liteos)
set(PYTHON python -B)
set(PYTHON3 python3 -B)
set(OUT ${CMAKE_CURRENT_BINARY_DIR})
set(BUILD ${OUT}/obj)
set(MK_PATH ${RTOSLITETOPDIR}/build/cmake)
set(LITEOS_SCRIPTPATH ${RTOSLITETOPDIR}/build/scripts)
set(LITEOS_LIB_BIGODIR ${OUT}/lib/obj)
set(LOSCFG_ENTRY_SRC ${LITEOSTOPDIR}/${KERNEL_PATH}/init/los_init.c)
set(BOARD_LD_INCULDE_PATH)
set(BOARD_LD_OUTPUT_PATH ${OUT}/targets)
set(BOARD_LD_FILE ${BOARD_LD_OUTPUT_PATH}/board.ld)

set(BOARD_LD_S_FILE ${LITEOSTOPDIR}/targets/${LITEOS_PLATFORM}/board.ld.S)

### include variable
set(MODULE ${MK_PATH}/kmodule.cmake)
set(MODULE_LIBC ${MK_PATH}/module_libc.cmake)
set(UMODULE ${MK_PATH}/umodule.cmake)

if(LOSCFG_COMPILER_HIMIX_32)
    list(APPEND LITEOS_CMACRO __COMPILER_HUAWEILITEOS__)
elseif(LOSCFG_COMPILER_HIMIX210_64)
    list(APPEND LITEOS_CMACRO __COMPILER_HUAWEILITEOS__)
elseif(LOSCFG_COMPILER_HCC_64)
    list(APPEND LITEOS_CMACRO __COMPILER_HUAWEILITEOS__)
elseif(LOSCFG_COMPILER_GCC_64)
    list(APPEND LITEOS_CMACRO __COMPILER_HUAWEILITEOS__)
endif()

if (NOT CONFIG_INTERNAL_OS_UNIFIED_COMPILE)
    list(APPEND LITEOS_CMACRO __LITEOS__)
endif()

list(APPEND LITEOS_CMACRO SECUREC_IN_KERNEL=0 _ALL_SOURCE)
if(NOT LOSCFG_DEBUG_VERSION)
list(APPEND LITEOS_CMACRO NDEBUG)
endif()

# workround : the linker may use the math functions in libstdc++. put the libm in the front
#             to make sure the math functions in the libm will be used.
if(LOSCFG_LIB_LIBM)
    list(APPEND LITEOS_EXT_AHEAD_LIBS m)
endif()

if(LOSCFG_COMPILER_CLANG)
    if(LOSCFG_COMPILER_LINGLONG)
        list(APPEND LITEOS_DEP_LIBS_EXT clang_rt.builtins)
    elseif(LOSCFG_COMPILER_ARM_NONE_EABI_CLANG)
        if(LOSCFG_COMPILER_LLVM_BINUTILS AND CONFIG_INTERNAL_OS_CORTEX_R5)
            list(APPEND LITEOS_DEP_LIBS_EXT clang_rt.builtins-armv7)
        else()
            list(APPEND LITEOS_DEP_LIBS_EXT clang_rt.builtins-arm)
        endif()
    elseif(LOSCFG_COMPILER_RISCV_CLANG_UNKNOWN OR LOSCFG_COMPILER_RISCV_CLANG_MUSL)
        list(APPEND LITEOS_DEP_LIBS_EXT clang_rt.builtins-riscv32)
    elseif(LOSCFG_COMPILER_LLVM_CLANG)
        if (LOSCFG_ARCH_ARM_AARCH64)
            list(APPEND LITEOS_DEP_LIBS_EXT clang_rt.builtins-aarch64)
        endif()
    endif()
elseif(LOSCFG_USING_GCC_NOPIC_LIB)
    list(APPEND LITEOS_DEP_LIBS_EXT gcc-nopic)
else()
    list(APPEND LITEOS_DEP_LIBS_EXT gcc)
endif()

if(NOT LOSCFG_LIB_LIBC)
    list(APPEND LITEOS_DEP_LIBS_EXT c)
endif()

if(NOT LOSCFG_COMPILER_ARM_NONE_EABI AND
   NOT LOSCFG_COMPILER_AARCH64_NONE_ELF AND
   NOT LOSCFG_COMPILER_RISCV_GCC_UNKNOWN AND
   NOT LOSCFG_COMPILER_XTENSA_32 AND
   NOT LOSCFG_COMPILER_CLANG)
    list(APPEND LITEOS_DEP_LIBS_EXT gcc_eh)
endif()
set(AS_OBJS_LIBC_FLAGS __ASSEMBLY__ CACHE STRING "liteos asm flags" FORCE)

set(WARNING_AS_ERROR -Wall -Werror)
if(LOSCFG_COMPILER_RISCV_CLANG_UNKNOWN OR LOSCFG_COMPILER_RISCV_CLANG_MUSL OR LOSCFG_COMPILER_LINGLONG)
    list(APPEND WARNING_AS_ERROR -Wno-tautological-constant-out-of-range-compare -Wno-parentheses-equality -Wno-string-plus-int)
endif()

set(LIBC_FILTER_OPTS -Wfloat-equal)
## ATTENTION: api_inc.cmake is about external api header files' path, don't add other vars in it ##
if(LOSCFG_KERNEL_CPPSUPPORT)
    # origin: lib/api.mk
if(LOSCFG_COMPILER_CLANG)
    list(APPEND LITEOS_DEP_LIBS_EXT c++ c++abi unwind)
else()
    list(APPEND LITEOS_DEP_LIBS_EXT supc++ stdc++)
endif()
endif()

set(ALL_INC_FILE)
if(LOSCFG_KERNEL_DYNLOAD)
    list(APPEND ALL_INC_FILE ${OUT}/dynload_dir/build/dynload_ld.cmake)
endif()

if (NOT CONFIG_INTERNAL_OS_UNIFIED_COMPILE)
    list(APPEND ALL_INC_FILE ${LITEOSTOPDIR}/${ARCH_PATH}/api_inc.cmake)
    list(APPEND ALL_INC_FILE ${LITEOSTOPDIR}/${KERNEL_PATH}/api_inc.cmake)
    list(APPEND ALL_INC_FILE ${LITEOSTOPDIR}/lib/api_inc.cmake)
    list(APPEND ALL_INC_FILE ${LITEOSTOPDIR}/drivers/api_inc.cmake)
    list(APPEND ALL_INC_FILE ${LITEOSTOPDIR}/compat/api_inc.cmake)
    list(APPEND ALL_INC_FILE ${LITEOSTOPDIR}/fs/api_inc.cmake)
    list(APPEND ALL_INC_FILE ${LITEOSTOPDIR}/shell/api_inc.cmake)
    list(APPEND ALL_INC_FILE ${LITEOSTOPDIR}/net/api_inc.cmake)

    FOREACH(FILE ${ALL_INC_FILE})
        if(EXISTS ${FILE})
            include(${FILE})
        endif()
    ENDFOREACH(FILE)
endif()

# the following vars will be used by submodules CMakeLists.txt, so defined here
set(LOS_PUB_INTF_SUFFIX _intf_pub)
set(LOS_PRI_INTF_SUFFIX _intf_pri)
set(LOS_CC_PROP_INTF_PUB "LiteOS${LOS_PUB_INTF_SUFFIX}")
set(VENDOR_CC_PROP_INTF_PUB "Vendor${LOS_PUB_INTF_SUFFIX}")

if(LOSCFG_KERNEL_HPL_SCRIPT_PREPROCESSOR)
set(LOS_HPL_PRE_INTF_SUFFIX _intf_hpl_pre)
set(LOS_HPL_PRE_INTF "LITEOS${LOS_HPL_PRE_INTF_SUFFIX}")
set(VENDOR_HPL_PRE_INTF "Vendor${LOS_HPL_PRE_INTF_SUFFIX}")
set(LOS_HPL_PREPROCESS_OUT_PATH ${OUT}/hpl/preprocess)
if(NOT OS_HPL_LOG_MODULE_ID)
set(OS_HPL_LOG_MODULE_ID 0)
endif()
endif()

if (NOT CONFIG_INTERNAL_OS_UNIFIED_COMPILE)
    # Export APIs from these module.
    add_subdirectory(${LITEOSTOPDIR}/${ARCH_PATH} ${ARCH_PATH})
    add_subdirectory(${LITEOSTOPDIR}/targets targets)

    add_subdirectory(${LITEOSTOPDIR}/${KERNEL_PATH} ${KERNEL_PATH})
    add_subdirectory(${LITEOSTOPDIR}/lib lib)
    add_subdirectory(${LITEOSTOPDIR}/drivers drivers)
    add_subdirectory(${LITEOSTOPDIR}/litekey litekey)
    if(EXISTS ${LITEOSTOPDIR}/shell/CMakeLists.txt)
        add_subdirectory(${LITEOSTOPDIR}/shell shell)
    endif()

    if(EXISTS ${LITEOSTOPDIR}/compat/CMakeLists.txt)
        add_subdirectory(${LITEOSTOPDIR}/compat compat)
    endif()

    if(EXISTS ${LITEOSTOPDIR}/LiteOS_Testcase/CMakeLists.txt)
        add_subdirectory(${LITEOSTOPDIR}/LiteOS_Testcase LiteOS_Testcase)
    endif()

    if(EXISTS ${LITEOSTOPDIR}/fs/CMakeLists.txt)
        add_subdirectory(${LITEOSTOPDIR}/fs fs)
    endif()

    if(NOT LOSCFG_SAFETY_KERNEL)
        if(EXISTS ${LITEOSTOPDIR}/net/CMakeLists.txt)
            add_subdirectory(${LITEOSTOPDIR}/net net)
        endif()
    endif()

    if(LOSCFG_LZ4_MEMCPY)
        if(EXISTS ${OPENSOURCEDIR}/lz4/CMakeLists.txt)
            add_subdirectory(${OPENSOURCEDIR}/lz4)
        endif()
    endif()

else()
    # Include ck moudules if unified compile is enabled
    include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)
    include_directories(${HUAWEI_SECURE_C_DIR})

    add_subdirectory(${RTOSLITETOPDIR}/self_src RTOS_Lite/)

    set(LITEOS_KERNEL_INCLUDE
        ${LITEOSTOPDIR}/include/uapi
    )

    list(APPEND LITEOS_LIB_INCLUDE ${LITEOS_COMPILER_GCC_INCLUDE})

    add_subdirectory(${RTOSLITETOPDIR}/self_src/lib/libsec RTOS_Lite/libsec)
    include_directories(${LITEOS_KERNEL_INCLUDE})

    if(${CONFIG_OS_ARCH_ARMV7_M})
        list(APPEND LITEOS_DEP_LIBS_EXT nosys)
    endif()

    if(${CONFIG_OS_APPINIT_TESTSUIT} AND NOT UNI_INSTALL)
        if(${CONFIG_OS_TESTSUITE_BAREMETAL})
            add_subdirectory(test_Debug/bare_metal/base/platform)
        else()
            add_subdirectory(test_Debug/BMTestProject/converge)
        endif()
    endif()
endif()


############################# Tools && Debug Option Begin ##############################
if(LOSCFG_3RDPARTY)
    add_subdirectory(${LITEOSTOPDIR}/3rdParty 3rdParty)
endif()

if(LOSCFG_TEST)
    include(${LITEOSTOPDIR}/${TEST_PATH}/test.cmake)
    add_subdirectory(${LITEOSTOPDIR}/${TEST_PATH} ${TEST_PATH})
endif()

if(LOSCFG_NULL_ADDRESS_PROTECT)
    list(APPEND LITEOS_CMACRO LOSCFG_NULL_ADDRESS_PROTECT)
endif()
############################# Tools && Debug Option End #################################


## Optimize Options
if(LOSCFG_COMPILER_OPTIMIZE_NONE)
    set(LITEOS_COPTS_OPTIMIZE -O0)
elseif(LOSCFG_COMPILER_OPTIMIZE_SPEED)
    set(LITEOS_COPTS_OPTIMIZE -O2)
elseif(LOSCFG_COMPILER_OPTIMIZE_SIZE)
    set(LITEOS_COPTS_OPTIMIZE -Os)
endif()

if(LOSCFG_RISCV_COMPILER_CODESIZE_OPTIMIZE)
    list(APPEND LITEOS_COPTS_OPTIMIZE 
                "SHELL:-mllvm --jump-is-expensive=false"
                "SHELL:-mllvm --min-jump-table-entries=1000000"
                "SHELL:-mllvm --enable-oz-inline-threshold=true"
                "SHELL:-mllvm --enable-machine-outliner=always"
                "SHELL:-mllvm -machine-outliner-funcbytes=4"
                "SHELL:-mllvm -enable-rodata-sections=true"
                "SHELL:-mllvm -enable-sequence-abstract=true"
                "SHELL:-mllvm -enable-branch-imm-no-zero=true"
                "SHELL:-mllvm --riscv-enable-copyelim=true")
    if(LOSCFG_ARCH_LINXCORE_131)
        list(APPEND LITEOS_COPTS_OPTIMIZE 
                    "SHELL:-mllvm -imm-compare"
                    "SHELL:-mllvm -emit-muliadd"
                    "SHELL:-mllvm -merge-immshf"
                    "SHELL:-mllvm -emit-uxtb-uxth"
                    "SHELL:-mllvm -himideer-push-pop")
    elseif(LOSCFG_ARCH_LINXCORE_132 OR LOSCFG_ARCH_LINXCORE_170)
        list(APPEND LITEOS_COPTS_OPTIMIZE 
                    "SHELL:-mllvm -enable-lwgp=true"
                    "SHELL:-mllvm -enable-ldst-multiple-opt=true"
                    "SHELL:-mllvm -allow-unalign-ldst=true")
    endif()
endif()

if(LOSCFG_COMPILE_DEBUG)
    set(LITEOS_COPTS_OPTION -g -gdwarf-2)
endif()

## disable aotu vectorize
if(LOSCFG_ARCH_ARM_SVE2)
    list(APPEND LITEOS_COPTS_OPTIMIZE -fno-tree-vectorize)
endif()

set(LITEOS_COPTS_DEBUG ${LITEOS_COPTS_OPTION} ${LITEOS_COPTS_OPTIMIZE})
set(LITEOS_CXXOPTS_BASE ${LITEOS_COPTS_OPTION} ${LITEOS_COPTS_OPTIMIZE})
set(LITEOS_ASOPTS ${LITEOS_COPTS_OPTION})

## Security Options
if(LOSCFG_CC_STACKPROTECTOR)
    set(LITEOS_SSP -fstack-protector --param ssp-buffer-size=4)
elseif(LOSCFG_CC_STACKPROTECTOR_STRONG)
    set(LITEOS_SSP -fstack-protector-strong)
elseif(LOSCFG_CC_STACKPROTECTOR_ALL)
    set(LITEOS_SSP -fstack-protector-all)
endif()

# ASLR Options
if(LOSCFG_LASLR_GOT)
    set(LITEOS_ASLR_OPTS -fpie)
    set(LITEOS_ASLR_LD_OPTS -pie -z now)
    if (LOSCFG_COMPILER_LLVM_BINUTILS)
        list(APPEND LITEOS_ASLR_LD_OPTS -z notext --apply-dynamic-relocs)
    endif()
elseif(LOSCFG_LASLR_GSPR)
    set(LITEOS_ASLR_LD_OPTS --emit-relocs)
endif()


set(LITEOS_COMMON_OPTS)

if(LOSCFG_SHARED_LIB)
    list(APPEND LITEOS_COMMON_OPTS -fPIC)
elseif(NOT LOSCFG_LASLR)
    list(APPEND LITEOS_COMMON_OPTS -fno-pic)
endif()

if(CONFIG_COMPILER_UNWIND_TABLES)
    list(APPEND LITEOS_DEP_LIBS_EXT unwind)
    list(APPEND LITEOS_COMMON_OPTS -funwind-tables)
endif()

list(APPEND LITEOS_COMMON_OPTS
            -fno-builtin -funsigned-char
            -ffunction-sections -fdata-sections
            ${WARNING_AS_ERROR} ${LITEOS_SSP} ${LITEOS_ASLR_OPTS}
)

if(LOSCFG_ARCH_XTENSA_XEA3)
    # xea2 don't support shadow due to libc in compiler
    list(APPEND LITEOS_COMMON_OPTS -Wshadow)
endif()

list(APPEND LITEOS_COMMON_OPTS -Wformat=2)
if((NOT LOSCFG_COMPILER_CLANG) AND (NOT LOSCFG_COMPILER_XTENSA_32))
    list(APPEND LITEOS_COMMON_OPTS -freg-struct-return -Wtrampolines)
endif()

if(LOSCFG_LIB_LIBC)
    list(APPEND LITEOS_COMMON_OPTS -nostdinc -nostdlib)
endif()

set(LITEOS_COPTS_BASE ${LITEOS_COMMON_OPTS})

# Anonymous structs and unions are supported on c11; while gcc supports those features as extension
# which is turn on for default. As for Clang, if choose c99, -std=gnu99 should be used.
if(LOSCFG_COMPILER_CLANG)
    if (LOSCFG_COMPILER_LINGLONG)
        list(APPEND LITEOS_COPTS_BASE -std=c99)
    elseif(LOSCFG_COMPILER_LLVM_BINUTILS AND CONFIG_INTERNAL_OS_CORTEX_R5)
        list(APPEND LITEOS_COPTS_BASE -std=gnu11)
    else()
        list(APPEND LITEOS_COPTS_BASE -std=c11)
    endif()
elseif(LOSCFG_COMPILER_XTENSA_32)
    list(APPEND LITEOS_COPTS_BASE -std=gnu99)
else()
    if(CONFIG_INTERNAL_OS_UNIFIED_COMPILE)
        list(APPEND LITEOS_COPTS_BASE -std=gnu11)
    else()
        list(APPEND LITEOS_COPTS_BASE -std=c99)
    endif()
endif()


# -Wpointer-arith will treat the size of a void or of a function as 1.
# -Wstrict-prototypes will warn if a function is defined without specifying the argument types.
# -pipe will use pipes, save compilation time
if (NOT CONFIG_INTERNAL_OS_UNIFIED_COMPILE)
    list(APPEND LITEOS_COPTS_BASE -Wpointer-arith)
endif()

list(APPEND LITEOS_COPTS_BASE -Wstrict-prototypes -fno-exceptions -pipe -fno-common)
if(LOSCFG_COMPILER_GCC)
    list(APPEND LITEOS_COPTS_BASE -fno-aggressive-loop-optimizations)
endif()

# clang support -fno-omit-frame-pointer
if(LOSCFG_BACKTRACE_FP)
if(NOT LOSCFG_ARCH_ARM_CORTEX_M AND NOT LOSCFG_ARCH_XTENSA)
    list(APPEND LITEOS_COPTS_BASE -fno-omit-frame-pointer)
    list(APPEND LITEOS_CXXOPTS_BASE -fno-omit-frame-pointer)
endif()
endif()

if(NOT LOSCFG_COMPILER_XTENSA_32)
    list(APPEND LITEOS_COPTS_BASE -Winvalid-pch)
endif()

if((NOT LOSCFG_ARCH_RISCV32) AND (NOT LOSCFG_COMPILER_ARM_NONE_EABI) AND (NOT LOSCFG_COMPILER_AARCH64_NONE_ELF))
    list(APPEND LITEOS_COPTS_BASE -fno-short-enums)
endif()

list(APPEND LITEOS_CXXOPTS_BASE
    -std=c++11 -fpermissive -fno-use-cxa-atexit -frtti
    ${LITEOS_COMMON_OPTS}
)

# clang don't support cpp exception, add -fno-cxx-exceptions
if(LOSCFG_COMPILER_CLANG AND NOT LOSCFG_SAFETY_KERNEL)
    list(APPEND LITEOS_CXXOPTS_BASE -fno-cxx-exceptions)
else()
    list(APPEND LITEOS_CXXOPTS_BASE -fexceptions)
endif()

if(NOT LOSCFG_COMPILER_LINGLONG)
    list(APPEND LITEOS_CXXOPTS_BASE -nostdinc++)
endif()

if(LOSCFG_COMPILER_ARM_NONE_EABI OR LOSCFG_COMPILER_AARCH64_NONE_ELF)
    list(APPEND LITEOS_CXXOPTS_BASE -Wno-deprecated)
endif()

if(NOT LOSCFG_COMPILER_XTENSA_32)
    list(APPEND LITEOS_CXXOPTS_BASE -Winvalid-pch)
endif()

if(LOSCFG_LLTREPORT)
    # -fprofile-arcs may introduce false alarm on 'maybe-uninitialized'
    set(LITEOS_GCOV_OPTS -fprofile-arcs -ftest-coverage -Wno-maybe-uninitialized)
    list(APPEND LITEOS_DEP_LIBS_EXT gcov)
endif()

list(APPEND LITEOS_LD_OPTS -static --gc-sections ${LITEOS_ASLR_LD_OPTS})
if(LOSCFG_TRUSTZONE AND LOSCFG_SECURE_REGION)
    list(APPEND LITEOS_LD_OPTS --cmse-implib --out-implib=${CMAKE_CURRENT_BINARY_DIR}/CMSE_importLib.o)
elseif(LOSCFG_TRUSTZONE AND (NOT LOSCFG_SECURE_REGION))
    list(APPEND LITEOS_LD_OPTS ${CMAKE_CURRENT_BINARY_DIR}/../secure/CMSE_importLib.o)
endif()

if(LOSCFG_MEM_DFX_DOUBLE_FREE_CHECK)
    list(APPEND LITEOS_MEMORY_OPTS --wrap=LOS_MemFree)
endif()

if(LOSCFG_MEM_DFX_SHOW_CALLER_RA)
    list(APPEND LITEOS_MEMORY_OPTS --wrap=LOS_MemAlloc --wrap=LOS_MemAllocAlign --wrap=LOS_MemRealloc)

if(LOSCFG_MEM_MUL_MODULE AND LOSCFG_KERNEL_MEM_BESTFIT)
    list(APPEND LITEOS_MEMORY_OPTS --wrap=LOS_MemMalloc --wrap=LOS_MemMallocAlign --wrap=LOS_MemMrealloc)
endif()

if(LOSCFG_KERNEL_MEM_SLAB_EXTENTION)
    list(APPEND LITEOS_MEMORY_OPTS --wrap=OsSlabAllocatorNew)
endif()

if(LOSCFG_KERNEL_CPPSUPPORT OR LOSCFG_COMPAT_POSIX)
    list(APPEND LITEOS_MEMORY_OPTS --wrap=malloc --wrap=realloc --wrap=calloc --wrap=zalloc)
endif()

if(LOSCFG_KERNEL_MEMBOX_STATIC OR LOSCFG_KERNEL_MEMBOX_DYNAMIC)
    list(APPEND LITEOS_MEMORY_OPTS --wrap=LOS_MemboxAlloc)
endif()

if(LOSCFG_COMPAT_CMSIS_VER_1 STREQUAL "y")
    list(APPEND LITEOS_MEMORY_OPTS --wrap=osPoolAlloc --wrap=osPoolCAlloc)
endif()
endif()

list(APPEND LITEOS_LD_OPTS ${LITEOS_MEMORY_OPTS})

if("${CONFIG_OS_BYTE_ORDER}" STREQUAL "OS_BIG_ENDIAN")
    list(APPEND LITEOS_LD_OPTS -EB)
endif()

if(LOSCFG_COMPILER_XTENSA_32)
    include(${MK_PATH}/compiler/compiler_xcc_cpu.cmake)
elseif(LOSCFG_COMPILER_GCC)
    include(${MK_PATH}/compiler/compiler_gcc_cpu.cmake)
endif()

list(APPEND LITEOS_LD_OPTS ${LITEOS_DYNLOADOPTS})
set(LITEOS_LD_PATH
    -L${LITEOS_SCRIPTPATH}/ld -L${BOARD_LD_OUTPUT_PATH}
    -L${OUT}/lib -L${LITEOS_LIB_BIGODIR} -L${LITEOSTOPDIR}/tools/build
    -L${LITEOSTOPDIR}/targets/${LITEOS_PLATFORM}
)
FOREACH(CUR_DIR ${LITEOS_COMPILER_GCCLIB_PATH} ${LITEOS_COMPILER_CXXLIB_PATH})
    list(APPEND LITEOS_LD_PATH -L${CUR_DIR})
ENDFOREACH(CUR_DIR)

if(LOSCFG_VENDOR)
    list(APPEND LITEOS_LD_PATH -L${OUT}/lib/rdk -L${OUT}/lib/sdk -L${OUT}/lib/main_server)
endif()

if(LOSCFG_USING_BOARD_LD)
    if(LOSCFG_BOX)
        set(LITEOS_LD_SCRIPT -T${LITEOSTOPDIR}/targets/${LITEOS_PLATFORM}/box/liteos.ld)
    else()
        if(CONFIG_INTERNAL_OS_UNIFIED_COMPILE)
            set(LITEOS_LD_SCRIPT -T${BOARD_LD_FILE})
        else()
            set(LITEOS_LD_SCRIPT -T${LITEOSTOPDIR}/targets/${LITEOS_PLATFORM}/liteos.ld)
        endif()
    endif()
else()
    set(LITEOS_LD_SCRIPT -T${RTOSLITETOPDIR}/liteos.ld)
endif()

list(APPEND LITEOS_CXXINCLUDE
    ${LITEOS_COMPILER_GCC_INCLUDE}
    ${LITEOS_ARCH_INCLUDE}
    ${LITEOS_NET_INCLUDE}
    ${LITEOS_SECURITY_INCLUDE}
    ${LITEOS_KERNEL_INCLUDE}
    ${LITEOS_FS_INCLUDE}
    ${LITEOS_LIBC_INCLUDE}
    ${LITEOS_DRIVERS_INCLUDE}
    ${LITEOS_COMPAT_INCLUDE}
)
set(LOSCFG_TOOLS_DEBUG_INCLUDE ${LITEOS_SHELL_INCLUDE} ${LITEOS_UART_INCLUDE})
