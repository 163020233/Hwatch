
set(LOS_KERNEL_INCLUDE ${LITEOS_TOP_DIR}/kernel_lite/include)
set(LOS_SEC_DIR ${HUAWEI_SECURE_C_DIR})

# The kirin repository use follow path as secure lib path.
# set(LOS_SEC_DIR ${LITEOS_TOP_DIR}/../../../../open_source/bounds_checking_function)

set(SEC_SRCS memcpy_s.c memset_s.c strcat_s.c strcpy_s.c vsnprintf_s.c snprintf_s.c secureprintoutput_a.c strtok_s.c)
list(TRANSFORM SEC_SRCS PREPEND ${LOS_SEC_DIR}/src/)

set(HPL_SRCS ${SEC_SRCS} ${LITEOS_TOP_DIR}/kernel_lite/extended/hpl/src/los_hpl_decompression.c)

# Compile decompressor with gcc compiler, instead of cross-compile toolchain.
set(HPL_DECOMPRESSION_COMPILER gcc)
set(HPL_DECOMPRESSION_TARGET hpl_decompressor)
set(HPL_DECOMPRESSION_OUT ${OUT}/hpl/decompress)
file(MAKE_DIRECTORY ${HPL_DECOMPRESSION_OUT})

set(HPL_DECOMPRESSION_C_FLAG -I${LOS_KERNEL_INCLUDE} -I${LOS_SEC_DIR}/include)
list(APPEND HPL_DECOMPRESSION_C_FLAG -I${LITEOS_TOP_DIR}/kernel_lite/extended/hpl/include)
list(APPEND HPL_DECOMPRESSION_C_FLAG -O2 -static -Wall -Werror -Wextra -s -std=c11 -fsigned-char -Wtrampolines -Wformat=2)
list(APPEND HPL_DECOMPRESSION_C_FLAG -Wdate-time -Wfloat-equal -Wswitch-default -Wvla -Wunused -fno-common -pipe)
list(APPEND HPL_DECOMPRESSION_C_FLAG -Wshadow -Wpointer-arith -Wwrite-strings -Wstrict-prototypes)
list(APPEND HPL_DECOMPRESSION_C_FLAG -Wredundant-decls -Wnested-externs -Winline)
list(APPEND HPL_DECOMPRESSION_C_FLAG -fstack-protector-strong -include ${LITEOS_PLATFORM_MENUCONFIG_H})

add_custom_target(${HPL_DECOMPRESSION_TARGET} ALL
    BYPRODUCTS ${HPL_DECOMPRESSION_OUT}/decompressor
    COMMAND ${HPL_DECOMPRESSION_COMPILER} ${HPL_DECOMPRESSION_C_FLAG} ${HPL_SRCS} -o ${HPL_DECOMPRESSION_OUT}/decompressor
    VERBATIM
)
