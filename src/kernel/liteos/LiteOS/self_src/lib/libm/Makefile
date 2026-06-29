include $(RTOSLITETOPDIR)/config.mk
LIBC_RELATIVE_PATH := $(LITEOS_OPENSOURCE_PATHNAME)/musl

MODULE_NAME := m

LOCAL_SRCS = $(wildcard $(LIBC_RELATIVE_PATH)/src/math/*.c)

LOCAL_INCLUDE := \
        -I $(LITEOS_LIBC_PATH)/src/include \
        -I $(LITEOS_LIBC_PATH)/src/internal

LOCAL_FLAGS := $(LOCAL_INCLUDE) $(LITEOS_GCOV_OPTS)
LOCAL_CFLAGS += -Wno-parentheses -Wno-strict-prototypes
ifeq ($(LOSCFG_COMPILER_CLANG), y)
    LOCAL_CFLAGS += -Wno-uninitialized -Wno-ignored-pragmas
    ifeq ($(shell awk 'BEGIN{print ("$(VERSION_NUM)" >= "13.0.0") ? "1" : "0"}'),1)
        LOCAL_CFLAGS += -Wno-unused-but-set-variable
    endif
else
    LOCAL_CFLAGS += -Wno-maybe-uninitialized -Wno-unknown-pragmas \
                    -Wno-unused-but-set-variable
endif

AARCH64_MATH_ASM_REMOVE = \
    $(LIBC_RELATIVE_PATH)/src/math/ceil.c \
    $(LIBC_RELATIVE_PATH)/src/math/ceilf.c \
    $(LIBC_RELATIVE_PATH)/src/math/fabs.c \
    $(LIBC_RELATIVE_PATH)/src/math/fabsf.c \
    $(LIBC_RELATIVE_PATH)/src/math/floor.c \
    $(LIBC_RELATIVE_PATH)/src/math/floorf.c \
    $(LIBC_RELATIVE_PATH)/src/math/fma.c \
    $(LIBC_RELATIVE_PATH)/src/math/fmaf.c \
    $(LIBC_RELATIVE_PATH)/src/math/fmax.c \
    $(LIBC_RELATIVE_PATH)/src/math/fmaxf.c \
    $(LIBC_RELATIVE_PATH)/src/math/fmin.c \
    $(LIBC_RELATIVE_PATH)/src/math/fminf.c \
    $(LIBC_RELATIVE_PATH)/src/math/llrint.c \
    $(LIBC_RELATIVE_PATH)/src/math/llrintf.c \
    $(LIBC_RELATIVE_PATH)/src/math/llround.c \
    $(LIBC_RELATIVE_PATH)/src/math/llroundf.c \
    $(LIBC_RELATIVE_PATH)/src/math/lrint.c \
    $(LIBC_RELATIVE_PATH)/src/math/lrintf.c \
    $(LIBC_RELATIVE_PATH)/src/math/lround.c \
    $(LIBC_RELATIVE_PATH)/src/math/lroundf.c \
    $(LIBC_RELATIVE_PATH)/src/math/nearbyint.c \
    $(LIBC_RELATIVE_PATH)/src/math/nearbyintf.c \
    $(LIBC_RELATIVE_PATH)/src/math/rint.c \
    $(LIBC_RELATIVE_PATH)/src/math/rintf.c \
    $(LIBC_RELATIVE_PATH)/src/math/round.c \
    $(LIBC_RELATIVE_PATH)/src/math/roundf.c \
    $(LIBC_RELATIVE_PATH)/src/math/sqrt.c \
    $(LIBC_RELATIVE_PATH)/src/math/sqrtf.c \
    $(LIBC_RELATIVE_PATH)/src/math/trunc.c \
    $(LIBC_RELATIVE_PATH)/src/math/truncf.c


# C++ library symbols in the compiler conflict with those in the LiteOS libm #
ifeq ($(LOSCFG_COMPILER_ARM_NONE_EABI)$(LOSCFG_KERNEL_CPPSUPPORT), yy)
FILTER_FILES += $(wildcard $(LIBC_RELATIVE_PATH)/src/math/frexpl.c) \
                $(wildcard $(LIBC_RELATIVE_PATH)/src/math/ceill.c)

LOCAL_SRCS := $(filter-out $(FILTER_FILES), $(LOCAL_SRCS))
endif

ifeq ($(LOSCFG_ARCH_ARM_AARCH64), y)
LOCAL_SRCS := $(filter-out $(AARCH64_MATH_ASM_REMOVE), $(LOCAL_SRCS))
LOCAL_SRCS += $(wildcard $(LIBC_RELATIVE_PATH)/src/math/aarch64/*.c)
endif

include $(MODULE_LIBC)
