include $(RTOSLITETOPDIR)/config.mk

MODULE_NAME := $(notdir $(shell pwd))

LOCAL_SRCS := src/ftl.c src/fblock.c

ifeq ($(LOSCFG_FS_FAT_FTL_COMPRESS), y)
LOCAL_SRCS += src/ftl_compress.c
endif

LOCAL_INCLUDE += -I $(LITEOSTOPDIR)/fs/vfs/include/ftl

LOCAL_FLAGS := $(LOCAL_INCLUDE) $(LITEOS_GCOV_OPTS) $(LITEOS_NON_SECURE_LOCAL_OPTS)
LOCAL_FLAGS += -fno-strict-aliasing

include $(MODULE)
