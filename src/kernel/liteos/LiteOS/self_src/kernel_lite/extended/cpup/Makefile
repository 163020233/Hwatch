include $(RTOSLITETOPDIR)/config.mk

MODULE_NAME := $(notdir $(shell pwd))

LOCAL_SRCS_y := los_cpup.c
ifneq ($(LOSCFG_SHELL)_$(LOSCFG_SHELL_ADAPT_AT)_, __)
LOCAL_SRCS_y += cpup_shellcmd.c
endif
LOCAL_SRCS    = $(LOCAL_SRCS_y)
LOCAL_INCLUDE := \
	-I $(LITEOSTOPDIR)/kernel_lite/extended/include

LOCAL_FLAGS := $(LOCAL_INCLUDE) $(LITEOS_GCOV_OPTS) $(LITEOS_NON_SECURE_LOCAL_OPTS)

include $(MODULE)
