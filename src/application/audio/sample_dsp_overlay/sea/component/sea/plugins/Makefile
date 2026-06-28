CUR_DIR := $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)
IDP_TOP_DIR := $(realpath $(CUR_DIR)/../../..)

include $(IDP_TOP_DIR)/sap_idp.mak
include $(IDP_BUILD_DIR)/xtensa_cfg.mak
AT ?= @
#===============================================================================
sea_libx_y :=
sea_libx_$(CFG_SAP_SEA_LIB0_SUPPORT) += sea_lib0
#===============================================================================
sea_libx := $(sea_libx_y)
sea_libx_clean := $(addsuffix _clean,$(sea_libx))
sea_libx_txt := $(addsuffix _txt,$(sea_libx))

# sea_libx pre-build lib filter
sea_libx_lib_filter :=
#===============================================================================
#===============================================================================
.PHONY: all clean txt
#===============================================================================
all: $(sea_libx)
clean: $(sea_libx_clean)
txt: $(sea_libx_txt)
#===============================================================================
.PHONY: $(sea_libx) $(sea_libx_clean)
#===============================================================================
$(sea_libx):
	$(AT)$(XTENSA_MAKE) -C $@ all

$(sea_libx_clean):
	$(AT)$(XTENSA_MAKE) -C $(patsubst %_clean,%,$@) clean

$(sea_libx_txt):
	$(AT)$(XTENSA_MAKE) -C $(patsubst %_txt,%,$@) BUILD_LIB=y txt
	$(AT)$(XTENSA_MAKE) -C $(patsubst %_txt,%,$@) txt
#===============================================================================
lib := $(addsuffix _lib,$(filter-out $(sea_libx_lib_filter),$(sea_libx)))
lib_clean := $(addsuffix _clean,$(lib))
#===============================================================================
.PHONY: $(lib) $(lib_clean)
#===============================================================================
$(lib):
	$(AT)$(XTENSA_MAKE) -C $(patsubst %_lib,%,$@) BUILD_LIB=y lib

$(lib_clean):
	$(AT)$(XTENSA_MAKE) -C $(patsubst %_lib_clean,%,$@) BUILD_LIB=y lib_clean
#===============================================================================
.PHONY: lib lib_clean
#===============================================================================
lib: $(lib)
lib_clean: $(lib_clean)
#===============================================================================
