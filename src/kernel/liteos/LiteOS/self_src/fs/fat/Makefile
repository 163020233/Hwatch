include $(RTOSLITETOPDIR)/config.mk
LITEOS_FATFS_PATH := $(LITEOS_OPENSOURCE_PATHNAME)/FatFs

MODULE_NAME := $(notdir $(shell pwd))

LOCAL_SRCS := $(wildcard os_adapt/*.c) $(wildcard src/*.c)

LOCAL_SRCS += $(wildcard $(LITEOS_FATFS_PATH)/source/ff.c)
LOCAL_SRCS += $(wildcard $(LITEOS_FATFS_PATH)/source/ffunicode.c)

ifeq ($(LOSCFG_FS_FAT_DCACHE), y)
LOCAL_SRCS += $(wildcard dcache/src/*.c)
endif

LOCAL_INCLUDE := \
	-I $(LITEOSTOPDIR)/fs/fat/include \
	-I $(LITEOSTOPDIR)/fs/fat/virpart/include \
	-I $(LITEOSTOPDIR)/fs/fat/dcache/include \
	-I $(LITEOS_FATFS_PATH)/source \
	-I $(LITEOSTOPDIR)/fs/vfs/include/utrim

ifeq ($(LOSCFG_FS_VFS), y)
    LOCAL_INCLUDE += -I $(LITEOSTOPDIR)/fs/vfs/include/disk
endif

ifeq ($(LOSCFG_FS_FAT_CACHE), y)
    LOCAL_INCLUDE += -I $(LITEOSTOPDIR)/fs/vfs/include/bcache
endif

LOCAL_FLAGS := $(LOCAL_INCLUDE) $(LITEOS_GCOV_OPTS) -DFATFS_EXTEND_CONFIG -DCONFIG_PRIVATE_DIRENT_STRUCT
LOCAL_SRCS_NEED_SORT := y
LOCAL_FLAGS += -fno-strict-aliasing

include $(MODULE)

