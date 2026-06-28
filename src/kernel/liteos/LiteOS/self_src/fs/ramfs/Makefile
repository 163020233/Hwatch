include $(RTOSLITETOPDIR)/config.mk
NUTTX_RAMFS_PATH := $(LITEOS_OPENSOURCE_PATHNAME)/incubator-nuttx/liteos/fs/ramfs

MODULE_NAME := $(notdir $(shell pwd))

LOCAL_SRCS := $(wildcard *.c)
LOCAL_SRCS += $(wildcard $(NUTTX_RAMFS_PATH)/src/*.c)

LOCAL_INCLUDE := \
	-I $(NUTTX_RAMFS_PATH)/include \
	-I $(LITEOSTOPDIR)/fs/compat

LOCAL_FLAGS := $(LOCAL_INCLUDE) $(LITEOS_GCOV_OPTS)
LOCAL_SRCS_NEED_SORT := y

include $(MODULE)
