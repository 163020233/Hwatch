include $(RTOSLITETOPDIR)/config.mk

MODULE_NAME := $(LOSCFG_ARCH_CPU)

LOCAL_SRCS_y := $(wildcard src/canary.c) $(wildcard src/cpu.c) \
                $(wildcard src/fault.c) $(wildcard src/task.c)

LOCAL_SRCS_$(LOSCFG_ARCH_MMU_ENABLE) += $(wildcard src/mmu.c)
LOCAL_SRCS_$(LOSCFG_KERNEL_DYNLOAD) += $(wildcard src/dynload.c)
LOCAL_SRCS_$(LOSCFG_ARCH_LINXCORE_610) += $(wildcard src/cache_lx.c)

LOCAL_INCLUDE := \
        -I $(LITEOSTOPDIR)/kernel_lite/extended/include \
        -I $(LITEOSTOPDIR)/kernel_lite/extended/dynload/include

ifeq ($(LOSCFG_PERF_HW_PMU), y)
    LOCAL_INCLUDE += -I $(LITEOSTOPDIR)/kernel_lite/extended/perf
    LOCAL_SRCS_y += $(wildcard src/pmu.c)
endif

ASSRCS  := $(wildcard src/*.S)
ASSRCS  := $(filter-out src/spinlock.S src/spinlock_lse.S, $(ASSRCS))
ifeq ($(LOSCFG_ARCH_ARM_AARCH64_LSE_ATOMIC),y)
    ASSRCS += src/spinlock_lse.S
else
    ASSRCS += src/spinlock.S
endif

LOCAL_SRCS_y += $(ASSRCS)

LOCAL_SRCS = $(LOCAL_SRCS_y)
LOCAL_FLAGS := $(LOCAL_INCLUDE) $(LITEOS_GCOV_OPTS) $(LITEOS_NON_SECURE_LOCAL_OPTS)
LOCAL_SRCS_NEED_SORT := y

include $(MODULE)
