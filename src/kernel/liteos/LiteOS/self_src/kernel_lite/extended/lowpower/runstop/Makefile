include $(RTOSTOPDIR)/config.mk

MODULE_NAME := $(notdir $(shell pwd))

LOCAL_SRCS := $(wildcard src/*.c) $(wildcard src/*.s)
LOCAL_SRCS_NEED_SORT := y

LOCAL_INCLUDE := \
	-I $(LITEOSTOPDIR)/kernel_lite/extended/include \
	-I $(LITEOSTOPDIR)/kernel_lite/extended/include/lowpower
LOCAL_FLAGS := $(LOCAL_INCLUDE)

LOCAL_CFLAGS := $(LITEOS_GCOV_OPTS)

include $(MODULE)
