include $(RTOSLITETOPDIR)/config.mk

MODULE_NAME := $(notdir $(shell pwd))
# function
LOCAL_SRCS_$(LOSCFG_KERNEL_DEEPSLEEP) := deepsleep/los_deepsleep.c
LOCAL_SRCS_$(LOSCFG_KERNEL_TICKLESS) += tickless/los_tickless.c
LOCAL_SRCS_$(LOSCFG_KERNEL_RUNSTOP) += runstop/los_runstop.c
# debug
LOCAL_SRCS_$(LOSCFG_LOWPOWER_SLEEP_DEBUG) += debug/los_lowpower_debug.c
# base
LOCAL_SRCS = $(LOCAL_SRCS_y)
LOCAL_SRCS += $(wildcard *.c)
LOCAL_SRCS_NEED_SORT := y

LOCAL_INCLUDE := \
        -I $(LITEOSTOPDIR)/kernel_lite/extended/include \
        -I $(LITEOSTOPDIR)/kernel_lite/extended/include/lowpower

LOCAL_FLAGS := $(LOCAL_INCLUDE) $(LITEOS_GCOV_OPTS) $(LITEOS_NON_SECURE_LOCAL_OPTS)

include $(MODULE)
