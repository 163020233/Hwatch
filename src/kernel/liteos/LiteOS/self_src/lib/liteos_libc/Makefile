include $(RTOSLITETOPDIR)/config.mk
LIBC_RELATIVE_PATH := $(LITEOS_OPENSOURCE_PATHNAME)/musl
include ../libc/libc_src.mk

MODULE_NAME := posix

LOCAL_DIRS_y :=

# liteos_libc path
LOCAL_DIRS_LITEOS_LIBC := sched misc env
LOCAL_DIRS_LITEOS_LIBC += mq/nonisolated ldso pthread/nonisolated
# musl_libc path
LOCAL_SRCS_LIBC := $(LIBC_PRNG_SRC) $(LIBC_LINUX_SRC) $(LIBC_THREAD_SRC)

# posix configured module path
LOCAL_SRCS_$(LOSCFG_COMPAT_POSIX) += time/timer.c pthread/pthread_attr.c
LOCAL_SRCS_$(LOSCFG_COMPAT_POSIX) += $(LOCAL_SRCS_LIBC)
LOCAL_DIRS_$(LOSCFG_COMPAT_POSIX) += $(LOCAL_DIRS_LITEOS_LIBC)

LOCAL_SRCS_y += $(foreach dir, $(LOCAL_DIRS_y), $(wildcard $(dir)/*.c))

LOCAL_SRCS    = $(LOCAL_SRCS_y)

LOCAL_SRCS := $(filter-out misc/getentropy.c, $(LOCAL_SRCS))

ifeq ($(LOSCFG_KERNEL_CPPSUPPORT)_$(LOSCFG_COMPAT_POSIX), y_)
LOCAL_SRCS += misc/stub.c
endif

LOCAL_INCLUDE := -I $(LITEOSTOPDIR)/compat \
                 -I $(LITEOSTOPDIR)/lib/liteos_libc/pthread \
                 -I $(LITEOSTOPDIR)/lib/liteos_libc/mq \
                 -I $(LITEOSTOPDIR)/lib/liteos_libc/errno \
                 -I $(LITEOSTOPDIR)/lib/liteos_libc/tzdst \
                 -I $(LITEOSTOPDIR)/lib/liteos_libc/time \
                 -I $(LITEOSTOPDIR)/lib/liteos_libc/lock \
                 -I $(LITEOSTOPDIR)/lib/liteos_libc/init \
                 -I $(LITEOS_LIBC_PATH)/src/include \
                 -I $(LITEOS_LIBC_PATH)/src/internal \
                 -I $(LITEOS_LIBC_PATH)/src/time

LOCAL_FLAGS := $(LOCAL_INCLUDE) $(LITEOS_GCOV_OPTS) $(LITEOS_NON_SECURE_LOCAL_OPTS)
LOCAL_CFLAGS += -Wno-parentheses -Wno-strict-prototypes

ifeq ($(LOSCFG_COMPILER_GCC), y)
    ifeq ($(shell awk 'BEGIN{print ("$(VERSION_NUM)" >= "13.0.0") ? "1" : "0"}'), 1)
        LOCAL_CFLAGS += -Wno-array-parameter
    endif
endif

LOCAL_FLAGS := $(filter-out -Wshadow, $(LOCAL_FLAGS))

include $(MODULE_LIBC)
