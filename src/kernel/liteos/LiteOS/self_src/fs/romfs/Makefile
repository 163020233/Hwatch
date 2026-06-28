include $(RTOSLITETOPDIR)/config.mk
NUTTX_ROMFS_PATH := $(LITEOS_OPENSOURCE_PATHNAME)/incubator-nuttx/liteos/fs/romfs

MODULE_NAME := $(notdir $(shell pwd))

LOCAL_SRCS := $(wildcard *.c)
LOCAL_SRCS += $(wildcard $(NUTTX_ROMFS_PATH)/src/*.c)

LOCAL_INCLUDE := \
	-I $(NUTTX_ROMFS_PATH)/include \
	-I $(LITEOSTOPDIR)/fs/compat

ifeq ($(LOSCFG_FS_FAT_CACHE), y)
    LOCAL_INCLUDE += -I $(LITEOSTOPDIR)/fs/vfs/include/bcache
endif

LOCAL_FLAGS := $(LOCAL_INCLUDE) $(LITEOS_GCOV_OPTS)
LOCAL_FLAGS += -DCONFIG_ROMFS_EXTEND
LOCAL_SRCS_NEED_SORT := y

include $(MODULE)
