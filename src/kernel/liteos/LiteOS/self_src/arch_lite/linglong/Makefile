include $(RTOSLITETOPDIR)/config.mk

MODULE_NAME := $(LOSCFG_ARCH_CPU)

LOCAL_SRCS_y := $(wildcard src/canary.c) $(wildcard src/cpu.c) \
                $(wildcard src/fault.c) $(wildcard src/task.c) \
                $(wildcard src/atomic.c) $(wildcard src/canary.c)

ASSRCS = $(wildcard src/dispatch.S) $(wildcard src/exception.S) \
         $(wildcard src/reset_vector.S) $(wildcard src/vector.S)

ifeq ($(LOSCFG_KERNEL_SMP), y)
ASSRCS += $(wildcard src/spinlock.S)
endif
ifeq ($(LOSCFG_KERNEL_LOWPOWER), y)
ASSRCS += $(wildcard src/deepsleep.S)
endif

LOCAL_INCLUDE := \
    -I $(LITEOSTOPDIR)/kernel_lite/extended/include \
    -I $(LITEOSTOPDIR)/kernel_lite/extended/perf \
    -I $(LITEOSTOPDIR)/kernel_lite/extended/dynload/include

ifeq ($(LOSCFG_PERF_HW_PMU), y)
LOCAL_SRCS_y += $(wildcard src/pmu.c)
endif

ifeq ($(LOSCFG_KERNEL_DYNLOAD), y)
LOCAL_SRCS_y += $(wildcard src/dynload.c)
endif

ifeq ($(LOSCFG_LASLR), y)
LOCAL_SRCS_y += $(wildcard aslr/aslr.c)
ASSRCS       += $(wildcard aslr/aslr_vector.S) \
                $(wildcard aslr/symbol.S)
endif

LOCAL_FLAGS := $(LOCAL_INCLUDE) $(LITEOS_NON_SECURE_LOCAL_OPTS)

LOCAL_SRCS_y += $(ASSRCS)

LOCAL_SRCS = $(LOCAL_SRCS_y)
LOCAL_SRCS_NEED_SORT := y

include $(MODULE)
