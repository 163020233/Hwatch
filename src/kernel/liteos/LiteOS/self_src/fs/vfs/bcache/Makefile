include $(RTOSLITETOPDIR)/config.mk

MODULE_NAME := $(notdir $(shell pwd))

LOCAL_SRCS_y := src/bcache.c
LOCAL_SRCS_$(LOSCFG_FS_FAT_CACHE_PROC) += src/bcache_proc.c

LOCAL_SRCS     = $(LOCAL_SRCS_y)
LOCAL_INCLUDE := -I $(LITEOSTOPDIR)/fs/vfs/include/disk \
                 -I $(LITEOSTOPDIR)/fs/vfs/include/bcache

LOCAL_FLAGS   := $(LOCAL_INCLUDE) $(LITEOS_GCOV_OPTS) $(LITEOS_NON_SECURE_LOCAL_OPTS)
LOCAL_SRCS_NEED_SORT := y
LOCAL_FLAGS += -fno-strict-aliasing

include $(MODULE)

