set(MODULE_NAME m)


# add global include, compile macro and options to public target #
set(MODULE_INCLUDE_PUB)
set(MODULE_CXXINCLUDE_PUB)
set(MODULE_COPTS_PUB)
set(MODULE_ASOPTS_PUB)
set(MODULE_CXXOPTS_PUB)
set(MODULE_CMACRO_PUB)
set(MODULE_ASMACRO_PUB)
set(MODULE_CXXMACRO_PUB)


# add local include, compile macro and options to private target #
set(MODULE_INCLUDE_PRI
    ${LITEOS_LIBC_PATH}/src/include
    ${LITEOS_LIBC_PATH}/src/internal
)
set(MODULE_COPTS_PRI)
set(MODULE_ASOPTS_PRI)
set(MODULE_CXXOPTS_PRI)
set(MODULE_CMACRO_PRI)
set(MODULE_ASMACRO_PRI)
set(MODULE_CXXMACRO_PRI)

list(APPEND MODULE_COPTS_PRI -Wno-parentheses -Wno-strict-prototypes)
if(LOSCFG_COMPILER_CLANG)
    list(APPEND MODULE_COPTS_PRI -Wno-uninitialized -Wno-ignored-pragmas)
    if (${VERSION_NUM} VERSION_GREATER_EQUAL 13.0.0)
        list(APPEND MODULE_COPTS_PRI -Wno-unused-but-set-variable)
    endif()
else()
    list(APPEND MODULE_COPTS_PRI
         -Wno-maybe-uninitialized
         -Wno-unknown-pragmas
         -Wno-unused-but-set-variable
    )
endif()

# add srcs to private target #
aux_source_directory(${LIBC_DIR_PATH}/src/math LOCAL_SRCS_y)

set(AARCH64_MATH_ASM_REMOVE
    ${LIBC_DIR_PATH}/src/math/ceil.c
    ${LIBC_DIR_PATH}/src/math/ceilf.c
    ${LIBC_DIR_PATH}/src/math/fabs.c
    ${LIBC_DIR_PATH}/src/math/fabsf.c
    ${LIBC_DIR_PATH}/src/math/floor.c
    ${LIBC_DIR_PATH}/src/math/floorf.c
    ${LIBC_DIR_PATH}/src/math/fma.c
    ${LIBC_DIR_PATH}/src/math/fmaf.c
    ${LIBC_DIR_PATH}/src/math/fmax.c
    ${LIBC_DIR_PATH}/src/math/fmaxf.c
    ${LIBC_DIR_PATH}/src/math/fmin.c
    ${LIBC_DIR_PATH}/src/math/fminf.c
    ${LIBC_DIR_PATH}/src/math/llrint.c
    ${LIBC_DIR_PATH}/src/math/llrintf.c
    ${LIBC_DIR_PATH}/src/math/llround.c
    ${LIBC_DIR_PATH}/src/math/llroundf.c
    ${LIBC_DIR_PATH}/src/math/lrint.c
    ${LIBC_DIR_PATH}/src/math/lrintf.c
    ${LIBC_DIR_PATH}/src/math/lround.c
    ${LIBC_DIR_PATH}/src/math/lroundf.c
    ${LIBC_DIR_PATH}/src/math/nearbyint.c
    ${LIBC_DIR_PATH}/src/math/nearbyintf.c
    ${LIBC_DIR_PATH}/src/math/rint.c
    ${LIBC_DIR_PATH}/src/math/rintf.c
    ${LIBC_DIR_PATH}/src/math/round.c
    ${LIBC_DIR_PATH}/src/math/roundf.c
    ${LIBC_DIR_PATH}/src/math/sqrt.c
    ${LIBC_DIR_PATH}/src/math/sqrtf.c
    ${LIBC_DIR_PATH}/src/math/trunc.c
    ${LIBC_DIR_PATH}/src/math/truncf.c)


# C++ library symbols in the compiler conflict with those in the LiteOS libm #
if(LOSCFG_COMPILER_ARM_NONE_EABI AND LOSCFG_KERNEL_CPPSUPPORT)
    list(REMOVE_ITEM LOCAL_SRCS_y ${LIBC_DIR_PATH}/src/math/frexpl.c ${LIBC_DIR_PATH}/src/math/ceill.c)
endif()

if(LOSCFG_ARCH_ARM_AARCH64)
aux_source_directory(${LIBC_DIR_PATH}/src/math/aarch64 LOCAL_SRCS_y)
list(REMOVE_ITEM LOCAL_SRCS_y ${AARCH64_MATH_ASM_REMOVE})
endif()

set(LOCAL_SRCS ${LOCAL_SRCS_y})
include(${MODULE_LIBC})
