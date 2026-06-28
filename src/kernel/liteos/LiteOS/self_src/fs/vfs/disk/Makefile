include $(RTOSLITETOPDIR)/config.mk

MODULE_NAME := $(notdir $(shell pwd))

LOCAL_SRCS :=  $(wildcard *.c)

LOCAL_INCLUDE := -I $(LITEOSTOPDIR)/drivers/mmc/core
LOCAL_INCLUDE += -I $(LITEOSTOPDIR)/fs/vfs/include/utrim \
                 -I $(LITEOSTOPDIR)/fs/vfs/include/disk

ifeq ($(LOSCFG_FS_FAT_FTL), y)
LOCAL_INCLUDE += -I $(LITEOSTOPDIR)/fs/vfs/include/ftl
endif

ifeq ($(LOSCFG_FS_FAT_CACHE), y)
    LOCAL_INCLUDE += -I $(LITEOSTOPDIR)/fs/vfs/include/bcache
endif

LOCAL_FLAGS := $(LOCAL_INCLUDE) $(LITEOS_GCOV_OPTS) $(LITEOS_NON_SECURE_LOCAL_OPTS)
LOCAL_SRCS_NEED_SORT := y
LOCAL_FLAGS += -fno-strict-aliasing

include $(MODULE)
