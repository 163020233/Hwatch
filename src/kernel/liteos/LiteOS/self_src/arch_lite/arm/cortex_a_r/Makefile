include $(RTOSLITETOPDIR)/config.mk

MODULE_NAME := $(LOSCFG_ARCH_CPU)

LOCAL_SRCS_y := $(wildcard src/canary.c) $(wildcard src/cpu.c) \
                $(wildcard src/fault.c) $(wildcard src/mmu.c)  \
                $(wildcard src/task.c) $(wildcard src/*.S)

LOCAL_SRCS_$(LOSCFG_KERNEL_DYNLOAD) += $(wildcard src/dynload.c)
LOCAL_SRCS_$(LOSCFG_LASLR) += $(wildcard src/aslr/aslr.c src/aslr/symbol.S)

LOCAL_INCLUDE := \
	-I $(LITEOSTOPDIR)/kernel_lite/extended/include \
	-I $(LITEOSTOPDIR)/kernel_lite/extended/dynload/include \

ifeq ($(LITEOS_ARM_ARCH), -march=armv7-a)
LOCAL_SRCS_y += $(wildcard src/armv7/*.S)
else ifeq ($(LITEOS_ARM_ARCH), -march=armv7-r)
LOCAL_SRCS_y += $(wildcard src/armv7/cache.S)
endif

ifneq ($(LOSCFG_APC_ENABLE), y)
LOCAL_SRCS_y := $(filter-out src/mmu.c, $(LOCAL_SRCS_y))
endif

ifeq ($(LOSCFG_ARCH_ARM_WITH_THUMB_INSTRUCTION), y)
LOCAL_SRCS_y += $(wildcard ../common/thumb_backtrace.c)
endif

ifeq ($(LOSCFG_PERF_HW_PMU), y)
LOCAL_SRCS_y += $(wildcard src/pmu.c)
LOCAL_INCLUDE += -I $(LITEOSTOPDIR)/kernel_lite/extended/perf
endif

ifneq ($(LOSCFG_KERNEL_LOWPOWER), y)
LOCAL_SRCS_y := $(filter-out src/runstop.S, $(LOCAL_SRCS_y))
endif

LOCAL_SRCS = $(LOCAL_SRCS_y)
LOCAL_SRCS_NEED_SORT := y
LOCAL_FLAGS := $(LOCAL_INCLUDE) $(LITEOS_GCOV_OPTS) $(LITEOS_NON_SECURE_LOCAL_OPTS)

ifeq ($(LOSCFG_GDB), y)
LOCAL_FLAGS += $(AS_OBJS_LIBC_FLAGS)
endif
include $(MODULE)
