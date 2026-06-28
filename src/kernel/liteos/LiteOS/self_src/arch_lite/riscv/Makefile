include $(RTOSLITETOPDIR)/config.mk

MODULE_NAME := $(LOSCFG_ARCH_CPU)

LOCAL_INCLUDE := \
        -I $(LITEOSTOPDIR)/kernel_lite/extended/include \
        -I $(LITEOSTOPDIR)/kernel_lite/extended/dynload/include \
        -I ${LITEOSTOPDIR}/kernel_lite/extended/perf

LOCAL_SRCS_y := src/cache.c src/canary.c src/fault.c src/task.c src/stack.S src/fault_ccause.c

LOCAL_SRCS_$(LOSCFG_LASLR) += src/aslr/aslr_vector.S src/aslr/aslr.c src/aslr/symbol.S
LOCAL_SRCS_$(LOSCFG_FPB_ENABLE) += src/fpb.c
LOCAL_SRCS_$(LOSCFG_PERF_HW_PMU) += src/pmu.c
LOCAL_SRCS_$(LOSCFG_APC_ENABLE) += src/pmp.c
LOCAL_SRCS_$(LOSCFG_KERNEL_DYNLOAD) += src/dynload.c
LOCAL_SRCS_$(LOSCFG_ARCH_VECTOR_ENABLE) += src/vpu.S
LOCAL_SRCS_$(LOSCFG_TRIGGER_ENABLE) += src/trigger.c
LOCAL_SRCS_$(LOSCFG_EXC_RECOVER_ENABLE) += src/exc_recover.c
LOCAL_SRCS_$(LOSCFG_KERNEL_ISOLATION) += src/isolation.S

ifeq (${LOSCFG_RISCV_COMPILER_OPTIONS_LCMP_GEN}, y)
    LOCAL_SRCS_y += src/intrinsics/dispatch.c src/intrinsics/reset.c src/intrinsics/trap.c
else
    LOCAL_SRCS_y += src/exception.S src/dispatch.S src/reset_vector.S src/trap.S
endif

ifneq ($(LOSCFG_ARCH_INTERRUPT_TAKEOVER), y)
    LOCAL_SRCS_y := $(filter-out src/exception.S src/fault.c, $(LOCAL_SRCS_y))
endif

ifeq ($(LOSCFG_USING_BOARD_RESET_VECTOR), y)
    LOCAL_SRCS_y := $(filter-out src/reset_vector.S src/stack.S, $(LOCAL_SRCS_y))
endif

ifeq ($(LOSCFG_KERNEL_DEEPSLEEP), y)
    LOCAL_SRCS_y += src/runstop.S
endif

# add customized architecture directory here.
ifeq ($(LOSCFG_ARCH_LINXCORE_131), y)
    LITEOS_ARCH_RISCV := linx131
endif

ifneq ($(LITEOS_ARCH_RISCV), )
    -include src/$(LITEOS_ARCH_RISCV)/custom.mk
endif

LOCAL_SRCS = $(LOCAL_SRCS_y)
LOCAL_SRCS_NEED_SORT := y
LOCAL_FLAGS := $(LOCAL_INCLUDE) $(LITEOS_GCOV_OPTS) $(LITEOS_NON_SECURE_LOCAL_OPTS)

# aslr should avoid generating jump tables for switch statements even it would be more efficient.
ifeq ($(LOSCFG_LASLR), y)
    LOCAL_FLAGS += -fno-jump-tables
endif
include $(MODULE)
