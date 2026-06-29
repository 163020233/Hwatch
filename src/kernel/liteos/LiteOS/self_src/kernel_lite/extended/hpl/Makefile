include $(RTOSLITETOPDIR)/config.mk

MODULE_NAME := $(notdir $(shell pwd))

# make is only supported for fuzz test
LOCAL_SRCS := src/los_hpl_logger_common.c src/los_hpl_c_preprocessor.c

ifeq ($(LOSCFG_KERNEL_HPL_TASK_LOCK_FREE), y)
    LOCAL_SRCS += src/task_lock_free/los_hpl_ringbuf.c \
                  src/task_lock_free/los_hpl_logger.c \
                  src/task_lock_free/los_hpl_compression.c
else
    LOCAL_SRCS += src/task_locked/los_hpl_logger.c
endif

LOCAL_SRCS_NEED_SORT := y

LOCAL_INCLUDE := -I $(LITEOSTOPDIR)/kernel_lite/include \
                 -I $(LITEOSTOPDIR)/kernel_lite/extended/include \
                 -I $(LITEOSTOPDIR)/kernel_lite/extended/hpl/include

LOCAL_FLAGS := $(LOCAL_INCLUDE) $(LITEOS_NON_SECURE_LOCAL_OPTS)

LOCAL_CFLAGS := $(LITEOS_GCOV_OPTS)

include $(MODULE)
