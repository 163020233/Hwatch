include $(RTOSLITETOPDIR)/config.mk
LITEOS_LITTLEFS_PATH := $(LITEOS_OPENSOURCE_PATHNAME)/littlefs

MODULE_NAME := $(notdir $(shell pwd))

LOCAL_SRCS := $(wildcard os_adapt/*.c)

LOCAL_SRCS += $(wildcard $(LITEOS_LITTLEFS_PATH)/*.c)

LOCAL_INCLUDE := \
    -I $(LITEOSTOPDIR)/fs/littlefs/include \
    -I $(LITEOS_LITTLEFS_PATH) \

ifeq ($(CONFIG_OS_OPTION_FS_LITEVFS),y)
    LOCAL_SRCS := $(filter-out os_adapt/vfs_littlefs.c,$(LOCAL_SRCS))
else ifeq($(LOSCFG_FS_LITTLEFS),y)
    LOCAL_SRCS := $(filter-out os_adapt/litevfs_littlefs.c,$(LOCAL_SRCS))
endif

LOCAL_FLAGS := $(CFLAGS) $(LOCAL_INCLUDE) $(LITEOS_GCOV_OPTS) -DLFS_NO_ASSERT -DLFS_THREADSAFE

# The open-source code of littlefs contains self-assignment statements. #
ifeq ($(LOSCFG_COMPILER_RISCV_CLANG_MUSL), y)
LOCAL_FLAGS += -Wno-self-assign
endif

LOCAL_SRCS_NEED_SORT := y

include $(MODULE)

