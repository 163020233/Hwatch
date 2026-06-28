# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.

ifeq ($(LOSCFG_FS_VFS), y)
    LITEOS_VFS_INCLUDE   += -I $(LITEOSTOPDIR)/fs/include
    LITEOS_VFS_INCLUDE   += -I $(LITEOS_OPENSOURCE_PATHNAME)/incubator-nuttx/liteos/fs/include
    LITEOS_VFS_INCLUDE   += -I $(LITEOSTOPDIR)/fs/vfs/include/operation

endif

LITEOS_FS_INCLUDE          := $(LITEOS_VFS_INCLUDE)
