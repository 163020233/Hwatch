include $(RTOSLITETOPDIR)/config.mk

MODULE_NAME := $(notdir $(shell pwd))

LOCAL_SRCS := $(wildcard src/*.c)
LOCAL_SRCS_NEED_SORT := y

LOCAL_INCLUDE := \
              -I $(LITEOSTOPDIR)/kernel_lite/extended/include \
              -I $(LITEOSTOPDIR)/kernel_lite/extended/dynload/include


LOCAL_FLAGS := $(LOCAL_INCLUDE) $(LITEOS_GCOV_OPTS) $(LITEOS_NON_SECURE_LOCAL_OPTS)

include $(MODULE)
