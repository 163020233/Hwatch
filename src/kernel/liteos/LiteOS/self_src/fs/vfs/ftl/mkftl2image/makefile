#===============================================================================
# @brief    compiler file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2025. All rights reserved.
#===============================================================================

FATFS_DIR  := $(OPENSOURCEDIR)/FatFs/source
ZLIB_DIR   := $(OPENSOURCEDIR)/zlib
FTL_DIR    := ../src
KERNEL_DIR := ../../../../kernel_lite/include
CRC_DIR    := ../../../../compat/linux
SEC_DIR    := $(HUAWEI_SECURE_C_DIR)

-include ../../../../../.config
INC_DIR := -I .
INC_DIR += -I $(FATFS_DIR) -I $(ZLIB_DIR) -I $(KERNEL_DIR) -I $(CRC_DIR)/include -I $(SEC_DIR)/include
INC_DIR += -I ../../include/ftl

SEC_SRCS := memcpy_s.c memset_s.c strcat_s.c strcpy_s.c

SEC_SRCS := $(addprefix $(SEC_DIR)/src/, $(SEC_SRCS))

ZLIB_SRCS := adler32.c compress.c crc32.c deflate.c gzclose.c gzlib.c \
    gzread.c gzwrite.c infback.c inffast.c inflate.c inftrees.c \
    trees.c uncompr.c zutil.c

ZLIB_SRCS := $(addprefix $(ZLIB_DIR)/, $(ZLIB_SRCS))

SRC = $(wildcard *.c)
SRC += $(FATFS_DIR)/ff.c 
SRC += $(FATFS_DIR)/ffunicode.c
SRC += $(ZLIB_SRCS)
SRC += $(SEC_SRCS)
SRC += $(FTL_DIR)/ftl_compress.c
SRC += $(CRC_DIR)/src/crc32.c

OBJ = $(patsubst %.c, %.o, $(SRC))

CROSS_COMPILE ?=

GCC := gcc 

APP_TARGET = mkfs.ftl

CFLAGS += $(INC_DIR) -DFATFS_EXTEND_CONFIG -DZ_HAVE_UNISTD_H -DSECUREC_HAVE_WCTOMB=0 -DSECUREC_ENABLE_SPRINTF_FLOAT=0
CFLAGS += -O2 -Wall -Werror
CFLAGS += -Wshadow -Wpointer-arith -Wwrite-strings -Wstrict-prototypes
CFLAGS += -Wredundant-decls -Wnested-externs -Winline
CFLAGS += -fstack-protector-strong

ifeq ($(LOSCFG_FS_FAT_FTL_COMPRESS), y)
CFLAGS += -DLOSCFG_FS_FAT_FTL_COMPRESS
endif

%.o:%.c
	$(GCC) -c $(CFLAGS) $^ -o $@

SRCOBJ = $(SRC:.c=.o)

all:$(APP_TARGET)
$(APP_TARGET): $(SRCOBJ)
	$(GCC) $(SRCOBJ) $(CFLAGS) -o $(APP_TARGET)

clean:
	rm -f $(APP_TARGET) $(SRCOBJ)
