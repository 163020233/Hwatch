include $(RTOSLITETOPDIR)/config.mk
NUTTX_VFS_PATH := $(LITEOS_OPENSOURCE_PATHNAME)/incubator-nuttx/liteos/fs/vfs
NUTTX_SCHED_PATH := ${LITEOS_OPENSOURCE_PATHNAME}/incubator-nuttx/liteos/sched
MODULE_NAME := $(notdir $(shell pwd))

LOCAL_SRCS := $(wildcard operation/*.c) $(wildcard vfs_cmd/*.c)

LOCAL_SRCS += $(wildcard $(NUTTX_VFS_PATH)/operation/*.c) \
              $(wildcard $(NUTTX_VFS_PATH)/inode/*.c) \
              $(wildcard $(NUTTX_VFS_PATH)/dirent/*.c) \
              $(wildcard $(NUTTX_VFS_PATH)/mount/*.c) \
              $(wildcard $(NUTTX_VFS_PATH)/driver/*.c) \
              $(wildcard $(NUTTX_VFS_PATH)/poll/*.c) \
              $(wildcard $(NUTTX_SCHED_PATH)/semaphore/*.c) \

ifneq ($(LOSCFG_COMPAT_LINUX), y)
    LOCAL_SRCS := $(filter-out $(NUTTX_VFS_PATH)/poll/*.c, $(LOCAL_SRCS))
    LOCAL_SRCS := $(filter-out operation/fs_poll.c, $(LOCAL_SRCS))
endif

ifneq ($(LOSCFG_FS_VFS_BLOCK_DEVICE), y)
    LOCAL_SRCS := $(filter-out ${NUTTX_VFS_PATH}/driver/fs_blockproxy.c, $(LOCAL_SRCS))
endif

LOCAL_INCLUDE := \
    -I $(LITEOSTOPDIR)/fs/vfs/include \
    -I $(LITEOSTOPDIR)/fs/compat \
    -I $(NUTTX_VFS_PATH)/include \
    -I $(NUTTX_SCHED_PATH)/include \
    -I $(LITEOSTOPDIR)/fs/vfs/include/multi_partition \
    -I $(LITEOSTOPDIR)/fs/vfs/include/disk \
    -I $(LITEOSTOPDIR)/kernel_lite/extended/include \
    -I $(LITEOSTOPDIR)/compat/nuttx/include/nuttx

ifeq ($(LOSCFG_FS_FAT), y)
    LOCAL_INCLUDE += -I $(LITEOSTOPDIR)/fs/fat/include
endif

ifeq ($(LOSCFG_FS_FAT_CACHE), y)
    LOCAL_INCLUDE += -I $(LITEOSTOPDIR)/fs/vfs/include/bcache
endif

LOCAL_FLAGS := $(LOCAL_INCLUDE) $(LITEOS_GCOV_OPTS) $(LITEOS_SUGGESTED_COMPILE_OPTS)

LOCAL_SRCS_NEED_SORT := y

include $(MODULE)
