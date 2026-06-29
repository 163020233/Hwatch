include $(RTOSLITETOPDIR)/config.mk

MODULE_NAME     := $(notdir $(shell pwd))

LOCAL_SRCS_y :=
LOCAL_DIRS_y :=
LOCAL_SRCS_$(LOSCFG_KERNEL_MIN)  += los_bitmap.c los_err.c los_misc.c los_mp.c \
                                    los_percpu.c los_sortlink.c los_stackinfo.c \
                                    los_task.c los_tick.c los_exc.c

LOCAL_SRCS_$(LOSCFG_ARCH_INTERRUPT_TAKEOVER) += los_hwi.c
LOCAL_SRCS_$(LOSCFG_BASE_IPC_QUEUE) += los_queue.c
LOCAL_SRCS_$(LOSCFG_BASE_IPC_EVENT) += los_event.c
LOCAL_SRCS_$(LOSCFG_BASE_IPC_MUX) += los_mux.c
LOCAL_SRCS_$(LOSCFG_BASE_IPC_SEM) += los_sem.c
LOCAL_SRCS_$(LOSCFG_BASE_IPC_RWSEM) += los_rwsem.c
LOCAL_SRCS_$(LOSCFG_BASE_CORE_SWTMR) += los_swtmr.c
LOCAL_SRCS_$(LOSCFG_KERNEL_SPINDEP) += los_spinlock.c
LOCAL_SRCS_$(LOSCFG_DEBUG_RESOURCE_INFO) += los_resource.c
LOCAL_SRCS_$(LOSCFG_KERNEL_ISOLATION) += los_isolation.c
LOCAL_SRCS_y += los_hook.c

# printf
LOCAL_SRCS_$(LOSCFG_KERNEL_PRINTF) += los_printf.c

# ringbuf
LOCAL_SRCS_$(LOSCFG_KERNEL_RINGBUF) += los_ringbuf.c

# debug
ifneq ($(LOSCFG_DEBUG_TASK)_$(LOSCFG_SHELL)_$(LOSCFG_SHELL_ADAPT_AT)_$(LOSCFG_EXC_SIMPLE_INFO), ___y)
LOCAL_SRCS_y += debug/los_task_debug.c
endif
LOCAL_SRCS_$(LOSCFG_DEBUG_HWI) += debug/los_hwi_debug.c
LOCAL_SRCS_$(LOSCFG_DEBUG_SWTMR) += debug/los_swtmr_debug.c
LOCAL_SRCS_$(LOSCFG_DEBUG_QUEUE) += debug/los_queue_debug.c
LOCAL_SRCS_$(LOSCFG_DEBUG_MUTEX) += debug/los_mux_debug.c
LOCAL_SRCS_$(LOSCFG_DEBUG_SEMAPHORE) += debug/los_sem_debug.c
LOCAL_SRCS_$(LOSCFG_DEBUG_SCHED_STATISTICS) += debug/los_sched_debug.c
LOCAL_SRCS_$(LOSCFG_KERNEL_LOCKDEP) += debug/los_lockdep_debug.c
LOCAL_SRCS_$(LOSCFG_PTHREAD_MUXDEP) += debug/pthread_mux_deadlock_debug.c
LOCAL_SRCS_$(LOSCFG_SHELL_ADAPT_AT) += debug/los_at.c
LOCAL_SRCS_$(LOSCFG_HWI_STACK_CHECK) += los_hwi_dfx.c

# Schedulers
LOCAL_DIRS_$(LOSCFG_SCHED_SQ) += sched/sched_sq
LOCAL_DIRS_$(LOSCFG_SCHED_MQ) += sched/sched_mq

# Memory Algorithms
LOCAL_DIRS_$(LOSCFG_KERNEL_MEM_BESTFIT) += mem/bestfit
LOCAL_DIRS_$(LOSCFG_KERNEL_MEM_BESTFIT_LITTLE) += mem/bestfit_little

# Memory Extension
LOCAL_DIRS_$(LOSCFG_KERNEL_MEM_SLAB_EXTENTION) += mem/slab
LOCAL_DIRS_$(LOSCFG_MEM_TASK_STAT) += mem/common/memstat

# Membox
LOCAL_SRCS_$(LOSCFG_KERNEL_MEMBOX_STATIC) += mem/membox/los_membox.c
LOCAL_SRCS_$(LOSCFG_KERNEL_MEMBOX_DYNAMIC) += mem/membox/los_membox_dyn.c

# Memory Debug
LOCAL_DIRS_$(LOSCFG_MEM_MUL_POOL) += mem/common/multipool
LOCAL_SRCS_y += los_mem_dfx.c
LOCAL_SRCS_$(LOSCFG_MEMFRAG_DFX_SHOW_INFO) += los_memfrag_dfx.c
ifneq ($(LOSCFG_MEM_DFX_SHOW_CALLER_RA)_$(LOSCFG_MEM_DFX_DOUBLE_FREE_CHECK)_, __)
LOCAL_SRCS_y += los_wrap_memory.c
endif

# Shellcmd
ifneq ($(LOSCFG_SHELL)_$(LOSCFG_SHELL_ADAPT_AT)_, __)
ifeq ($(LOSCFG_ARCH_INTERRUPT_TAKEOVER)$(LOSCFG_DEBUG_HWI), yy)
LOCAL_SRCS_y += shellcmd/hwi_shellcmd.c
endif
ifeq ($(LOSCFG_DEBUG_SWTMR), y)
LOCAL_SRCS_y += shellcmd/swtmr_shellcmd.c
endif
ifeq ($(LOSCFG_DEBUG_XBUG), y)
LOCAL_SRCS_y += shellcmd/global_debug_shellcmd.c
endif
LOCAL_SRCS_$(LOSCFG_LOWPOWER_SLEEP_DEBUG) += shellcmd/lpm_shellcmd.c
ifneq ($(LOSCFG_SHELL_COMMAND_TRIM), y)
LOCAL_SRCS_y += shellcmd/mempt_shellcmd.c
LOCAL_SRCS_y += shellcmd/sysinfo_shellcmd.c
LOCAL_SRCS_y += shellcmd/stack_shellcmd.c
LOCAL_SRCS_y += shellcmd/task_shellcmd.c
LOCAL_SRCS_y += shellcmd/extend_dfx_shellcmd.c
endif
endif
ifeq ($(LOSCFG_DEBUG_MEM_MWMD), y)
LOCAL_SRCS_y += shellcmd/mwmd_shellcmd.c
endif

LOCAL_SRCS_y += $(foreach dir, $(LOCAL_DIRS_y), $(wildcard $(dir)/*.c))
LOCAL_SRCS    = $(LOCAL_SRCS_y)
LOCAL_SRCS_NEED_SORT := y

LOCAL_INCLUDE   := \
                -I $(LITEOSTOPDIR)/kernel_lite/extended/include \
                -I $(LITEOSTOPDIR)/kernel_lite/extended/include/lowpower \
                -I $(LITEOSTOPDIR)/kernel_lite/base/mem \
                -I $(OPENSOURCEDIR)/lz4
ifeq ($(LOSCFG_KERNEL_MEM_BESTFIT_LITTLE), y)
LOCAL_INCLUDE += -I $(LITEOSTOPDIR)/kernel_lite/base/mem/bestfit_little
endif
ifeq ($(LOSCFG_KERNEL_MEM_BESTFIT), y)
LOCAL_INCLUDE += -I $(LITEOSTOPDIR)/kernel_lite/base/mem/bestfit
endif

LOCAL_FLAGS := $(LOCAL_INCLUDE) $(LITEOS_GCOV_OPTS) $(LITEOS_NON_SECURE_LOCAL_OPTS)

include $(MODULE)
