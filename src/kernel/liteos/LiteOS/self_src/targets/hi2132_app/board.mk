USER_SRC =  \
        $(LITEOS_PLATFORM)/vendor/src/startup.S \
        $(LITEOS_PLATFORM)/vendor/src/hal_pinctrl_cat1.c \
        $(LITEOS_PLATFORM)/vendor/src/hal_uart_v151.c \
        $(LITEOS_PLATFORM)/vendor/src/pinctrl.c \
        $(LITEOS_PLATFORM)/vendor/src/uart_porting.c \
        $(LITEOS_PLATFORM)/vendor/src/uart.c \
        $(LITEOS_PLATFORM)/vendor/src/tick_timer.c \
        $(LITEOS_PLATFORM)/vendor/src/mpu.c \
        $(LITEOS_PLATFORM)/vendor/src/hal_watchdog_v151.c \
        $(LITEOS_PLATFORM)/vendor/src/hal_watchdog.c \
        $(LITEOS_PLATFORM)/vendor/src/watchdog_porting.c \
        $(LITEOS_PLATFORM)/vendor/src/watchdog.c

USER_INC = \
    -I $(LITEOS_PLATFORM)/vendor/include

USER_DEF = -DSW_UART_CHIP_DEFINE

LOCAL_SRCS    += $(USER_SRC)
LOCAL_INCLUDE += $(USER_INC)
LOCAL_DEFS    += $(USER_DEF)
LOCAL_FLAGS += $(LOCAL_INCLUDE) $(LITEOS_NON_SECURE_LOCAL_OPTS) $(LOCAL_DEFS)
