include $(RTOSLITETOPDIR)/config.mk

MODULE_NAME     := $(notdir $(shell pwd))

LOCAL_SRCS_y    :=
LOCAL_SRCS_$(LOSCFG_SHARED_STACK) += $(RTOSLITETOPDIR)/self_src/kernel_lite/keysrc/los_shared_stack.c
LOCAL_SRCS_$(LOSCFG_RISCV_HIMIDEERV200_PLIC) += ${LITEOSTOPDIR}/drivers/interrupt/riscv_himideerv200_key.c

LOCAL_SRCS    = $(LOCAL_SRCS_y)
LOCAL_SRCS_NEED_SORT := y

LOCAL_FLAGS := $(LOCAL_INCLUDE) $(LITEOS_GCOV_OPTS) $(LITEOS_NON_SECURE_LOCAL_OPTS)

include $(MODULE)