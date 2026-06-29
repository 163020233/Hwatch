include $(RTOSLITETOPDIR)/config.mk

MODULE_$(LOSCFG_LIB_LIBSEC) += libsec
MODULE_$(LOSCFG_LIB_LIBC) += libc
ifneq ($(LOSCFG_KERNEL_CPPSUPPORT)$(LOSCFG_COMPAT_POSIX),)
MODULE_y += liteos_libc
endif
MODULE_$(LOSCFG_LIB_LIBM) += libm
MODULE_$(LOSCFG_LIB_ZLIB) += zlib

ifneq ($(findstring $(LOSCFG_ARCH_ARM_V7A)$(LOSCFG_ARCH_ARM_V7R)$(LOSCFG_ARCH_CORTEX_R52), y y y),)
MODULE_$(LOSCFG_LIB_LIBC) += liteos_libc/arch/$(ARCH_LOCAL)/armv7
else ifneq ($(findstring $(LOSCFG_ARCH_RISCV32), y),)
MODULE_$(LOSCFG_LIB_LIBC) += liteos_libc/arch/$(ARCH_LOCAL)
else ifneq ($(findstring $(LOSCFG_ARCH_ARM_AARCH64), y),)
MODULE_$(LOSCFG_LIB_LIBC) += liteos_libc/arch/$(ARCH_LOCAL)/generic
endif

include $(MODULE)
