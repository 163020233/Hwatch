include $(RTOSLITETOPDIR)/config.mk
NUTTX_COMPAT_LINUX_PATH := $(LITEOS_OPENSOURCE_PATHNAME)/incubator-nuttx/liteos/compat/linux
FREEBSD_COMPAT_LINUX_PATH := $(LITEOS_OPENSOURCE_PATHNAME)/freebsd/liteos/compat/linux

MODULE_NAME := $(notdir $(shell pwd))

LOCAL_SRCS :=  src/adp.c src/time.c

ifeq ($(LOSCFG_COMPAT_LINUX_MODULE), y)
    LOCAL_SRCS += src/module.c
endif

ifeq ($(LOSCFG_COMPAT_LINUX_USED), y)
    LOCAL_SRCS += src/atomic.c
endif

ifeq ($(LOSCFG_COMPAT_LINUX_CHECKSUM), y)
ifeq ($(LOSCFG_ARCH_ARM_AARCH64), y)
    LOCAL_FLAGS := -D_LITTLE_ENDIAN=1234 -D_BYTE_ORDER=1234
    LOCAL_SRCS +=  $(wildcard $(FREEBSD_COMPAT_LINUX_PATH)/src/arm-arch/a53/*.c)
else ifeq ($(LOSCFG_ARCH_ARM_AARCH32)_$(LOSCFG_ARCH_ARM_CORTEX_M), y_)
    LOCAL_SRCS +=  $(wildcard $(FREEBSD_COMPAT_LINUX_PATH)/src/arm-arch/common/*.S)
    LOCAL_SRCS +=  $(wildcard $(FREEBSD_COMPAT_LINUX_PATH)/src/arm-arch/common/*.c)
endif
endif

LOCAL_SRCS_$(LOSCFG_COMPAT_LINUX_PENDLIST)      += src/common/pendlist.c
LOCAL_SRCS_$(LOSCFG_COMPAT_LINUX_TIMER)         += src/timer.c
LOCAL_SRCS_$(LOSCFG_COMPAT_LINUX_HRTIMER)       += src/hrtimer.c
LOCAL_SRCS_$(LOSCFG_COMPAT_LINUX_IRQ)           += src/interrupt.c
LOCAL_SRCS_$(LOSCFG_COMPAT_LINUX_SEMAPHORE)     += src/semaphore.c
LOCAL_SRCS_$(LOSCFG_COMPAT_LINUX_COMPLETION)    += src/completion.c
LOCAL_SRCS_$(LOSCFG_COMPAT_LINUX_WAITQUEUE)     += src/wait.c
LOCAL_SRCS_$(LOSCFG_COMPAT_LINUX_WORKQUEUE)     += src/workqueue.c
LOCAL_SRCS_$(LOSCFG_COMPAT_LINUX_DRIVER_BASE)   += src/driver.c
LOCAL_SRCS_$(LOSCFG_COMPAT_LINUX_WAKELOCK)      += src/wakelock.c
LOCAL_SRCS_$(LOSCFG_COMPAT_LINUX_RW_SEMAPHORE)  += src/rwsem.c
LOCAL_SRCS_$(LOSCFG_COMPAT_LINUX_RBTREE)        += src/rbtree.c
LOCAL_SRCS_$(LOSCFG_COMPAT_LINUX_CRC32)         += src/crc32.c
LOCAL_SRCS_$(LOSCFG_COMPAT_LINUX_MTD_LIST)      += src/mtd_list.c

LOCAL_SRCS += $(LOCAL_SRCS_y)
LOCAL_INCLUDE += -I $(LITEOSTOPDIR)/lib/libc/tzcode
LOCAL_FLAGS += $(LOCAL_INCLUDE) $(LITEOS_GCOV_OPTS) $(LITEOS_NON_SECURE_LOCAL_OPTS)
LOCAL_SRCS_NEED_SORT := y
ifeq ($(LOSCFG_COMPAT_LINUX_RBTREE), y)
LOCAL_FLAGS += -fno-strict-aliasing
endif

include $(MODULE)
