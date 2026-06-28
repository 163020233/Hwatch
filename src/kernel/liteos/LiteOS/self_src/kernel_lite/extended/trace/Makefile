include $(RTOSLITETOPDIR)/config.mk

MODULE_NAME := $(notdir $(shell pwd))

LOCAL_SRCS := $(wildcard *.c)

ifeq ($(LOSCFG_TRACE_CLIENT_INTERACT), y)
LOCAL_SRCS += $(wildcard pipeline/*.c)
endif

ifeq ($(LOSCFG_TRACE_PIPELINE_SERIAL), y)
LOCAL_SRCS += $(wildcard pipeline/serial/*.c)
endif
LOCAL_SRCS_NEED_SORT := y

LOCAL_INCLUDE := \
        -I $(LITEOSTOPDIR)/kernel_lite/base/include \
        -I $(LITEOSTOPDIR)/kernel_lite/extended/include \
        -I $(LITEOSTOPDIR)/kernel_lite/extended/trace/pipeline

ifeq ($(LOSCFG_TRACE_PIPELINE_SERIAL), y)
LOCAL_INCLUDE += -I $(LITEOSTOPDIR)/kernel_lite/extended/trace/pipeline/serial
endif

LOCAL_FLAGS := $(LOCAL_INCLUDE) $(LITEOS_GCOV_OPTS) $(LITEOS_NON_SECURE_LOCAL_OPTS)

include $(MODULE)

