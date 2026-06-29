include $(RTOSLITETOPDIR)/config.mk

MODULE_NAME := $(LOSCFG_ARCH_CPU)

ifeq ($(LOSCFG_SECURE_REGION), y)
    LOCAL_SRCS = $(wildcard src/secure/*.c) $(wildcard src/secure/*.S)
    LOCAL_SRCS += src/canary.c
else

    LOCAL_SRCS := $(wildcard src/*.c) $(wildcard src/*.S)

    ifneq ($(LOSCFG_KERNEL_LOWPOWER), y)
        LOCAL_SRCS := $(filter-out src/intermit.S, $(LOCAL_SRCS))
    endif
    ifneq ($(LOSCFG_FPB_ENABLE), y)
        LOCAL_SRCS := $(filter-out src/fpb.c, $(LOCAL_SRCS))
    endif

    ifneq ($(LOSCFG_APC_ENABLE), y)
        LOCAL_SRCS := $(filter-out src/mpu.c, $(LOCAL_SRCS))
    endif
    ifneq ($(LOSCFG_PERF_HW_PMU), y)
        LOCAL_SRCS := $(filter-out src/pmu.c, $(LOCAL_SRCS))
    endif
endif

LOCAL_SRCS += $(wildcard ../common/thumb_backtrace.c)

LOCAL_INCLUDE := \
    -I $(LITEOSTOPDIR)/kernel_lite/base/include \
    -I $(LITEOSTOPDIR)/kernel_lite/extended/include \
    -I $(LITEOSTOPDIR)/kernel_lite/extended/dynload/include

ifeq ($(LOSCFG_PERF_HW_PMU), y)
    LOCAL_INCLUDE += -I $(LITEOSTOPDIR)/kernel_lite/extended/perf
endif

LOCAL_SRCS_NEED_SORT := y
LOCAL_FLAGS := $(LOCAL_INCLUDE) $(LITEOS_GCOV_OPTS) $(LITEOS_NON_SECURE_LOCAL_OPTS)

ifeq ($(LOSCFG_GDB), y)
LOCAL_FLAGS += $(AS_OBJS_LIBC_FLAGS)
endif
include $(MODULE)
