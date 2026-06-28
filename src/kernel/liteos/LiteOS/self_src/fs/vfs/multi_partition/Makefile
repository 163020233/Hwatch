include $(RTOSLITETOPDIR)/config.mk

MODULE_NAME := $(notdir $(shell pwd))

LOCAL_SRCS :=  $(wildcard src/*.c)

LOCAL_INCLUDE := \
	-I $(LITEOSTOPDIR)/fs/jffs2/include \
	-I $(LITEOSTOPDIR)/fs/yaffs2/yaffs/include \
	-I $(LITEOSTOPDIR)/fs/yaffs2/yaffs/direct/include \
	-I $(LITEOSTOPDIR)/drivers/mtd/common/include \
	-I $(LITEOSTOPDIR)/fs/vfs/include/multi_partition

ifeq ($(LOSCFG_FS_FAT_FTL), y)
	LOCAL_INCLUDE += -I $(LITEOSTOPDIR)/fs/vfs/include/ftl
endif

LOCAL_FLAGS := $(LOCAL_INCLUDE) $(LITEOS_GCOV_OPTS) $(LITEOS_NON_SECURE_LOCAL_OPTS)
LOCAL_SRCS_NEED_SORT := y
LOCAL_FLAGS += -fno-strict-aliasing

include $(MODULE)
