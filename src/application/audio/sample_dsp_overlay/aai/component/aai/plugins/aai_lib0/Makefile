CUR_DIR := $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)
IDP_TOP_DIR := $(realpath $(CUR_DIR)/../../../..)

include $(IDP_TOP_DIR)/sap_idp.mak
include $(IDP_BUILD_DIR)/xtensa_cfg.mak
#===============================================================================
ifdef BUILD_LIB
TARGET := libaai_lib0

CFLAGS_EXT :=
INCLUDES := -Iinclude -I$(IDP_TOP_DIR)/include
LIB_INSTALL_DIR := lib

SRCFILE := $(wildcard src/*.c)

#===============================================================================
else
TARGET := aai_lib0

CFLAGS_EXT :=
INCLUDES := -Iinclude -I$(IDP_TOP_DIR)/include
LIBS := -l:lib/libaai_lib0.a

SRCFILE := xaaai_lib0_comp.c

endif
#===============================================================================

#===============================================================================
include ../rules.mak
#===============================================================================
