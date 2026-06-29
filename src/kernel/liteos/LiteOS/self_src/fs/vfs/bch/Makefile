include $(RTOSLITETOPDIR)/config.mk
NUTTX_FS_VFS_BCH_PATH := $(LITEOS_OPENSOURCE_PATHNAME)/incubator-nuttx/liteos/fs/vfs/bch

MODULE_NAME := $(notdir $(shell pwd))

LOCAL_SRCS :=  $(wildcard *.c)
LOCAL_SRCS +=  $(wildcard $(NUTTX_FS_VFS_BCH_PATH)/src/*.c)

LOCAL_INCLUDE := \
	-I $(LITEOSTOPDIR)/compat/nuttx/include \
	-I $(LITEOSTOPDIR)/fs/compat \
	-I $(LITEOSTOPDIR)/fs/vfs/include/disk \
	-I $(LITEOSTOPDIR)/fs/vfs/include/bch \
	-I $(NUTTX_FS_VFS_BCH_PATH)/include \

ifeq ($(LOSCFG_FS_FAT_CACHE), y)
    LOCAL_INCLUDE += -I $(LITEOSTOPDIR)/fs/vfs/include/bcache
endif

LOCAL_FLAGS := $(LOCAL_INCLUDE) $(LITEOS_GCOV_OPTS)
LOCAL_FLAGS += -DCONFIG_DISABLE_POLL -DCONFIG_BCH_EXTEND
LOCAL_SRCS_NEED_SORT := y

include $(MODULE)
