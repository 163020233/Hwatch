HPL_INC_DIR  := ../include
KERNEL_DIR := ../../../../kernel_lite/include
SEC_DIR    := $(HUAWEI_SECURE_C_DIR)

INC_DIR := -I$(HPL_INC_DIR) -I$(KERNEL_DIR) -I$(SEC_DIR)/include -include menuconfig.h

HPL_SRCS = ../src/los_hpl_decompression.c
INC_DIR += -I../../../../kernel_lite/extended/hpl/include

SEC_SRCS := memcpy_s.c memset_s.c strcat_s.c strcpy_s.c vsnprintf_s.c snprintf_s.c secureprintoutput_a.c strtok_s.c
SEC_SRCS := $(addprefix $(SEC_DIR)/src/, $(SEC_SRCS))

SRC += $(HPL_SRCS) $(SEC_SRCS)

GCC := gcc

APP_TARGET = decompressor

CFLAGS += $(INC_DIR)
CFLAGS += -O2 -static -Wall -Werror -Wextra -s -std=c11 -fsigned-char -Wtrampolines -Wformat=2
CFLAGS += -Wdate-time -Wfloat-equal -Wswitch-default -Wvla -Wunused -fno-common -pipe
CFLAGS += -Wshadow -Wpointer-arith -Wwrite-strings -Wstrict-prototypes
CFLAGS += -Wredundant-decls -Wnested-externs -Winline -fstack-protector-strong

all:$(APP_TARGET)
$(APP_TARGET): $(SRC)
	$(GCC) $(SRC) $(CFLAGS) -o $(APP_TARGET)
