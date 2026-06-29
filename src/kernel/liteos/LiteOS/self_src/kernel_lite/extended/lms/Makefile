include $(RTOSLITETOPDIR)/config.mk

MODULE_NAME := $(notdir $(shell pwd))

LOCAL_SRCS := los_lms.c

ifeq ($(LOSCFG_LMS_LIBC_HIGH_FREQ_FUNC_CHECK), y)
LOCAL_SRCS += lms_libc.c
endif

LOCAL_SRCS_NEED_SORT := y

LOCAL_INCLUDE := -I $(LITEOSTOPDIR)/kernel_lite/include \
                 -I $(LITEOSTOPDIR)/kernel_lite/extended/include

ifeq ($(LOSCFG_KERNEL_MEM_BESTFIT), y)
LOCAL_INCLUDE += -I $(LITEOSTOPDIR)/kernel_lite/base/mem/bestfit
endif

LOCAL_FLAGS := $(LOCAL_INCLUDE) $(LITEOS_NON_SECURE_LOCAL_OPTS)

LOCAL_CFLAGS := $(LITEOS_GCOV_OPTS)

include $(MODULE)
