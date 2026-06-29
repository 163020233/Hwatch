CUR_DIR := $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)
IDP_TOP_DIR := $(realpath $(CUR_DIR)/../../..)

include $(IDP_TOP_DIR)/sap_idp.mak
include $(IDP_BUILD_DIR)/xtensa_cfg.mak
AT ?= @
#===============================================================================
lhdc_libx_y :=
lhdc_libx_$(CFG_SAP_LHDC_LIB0_SUPPORT) += lhdc_lib0
#===============================================================================
lhdc_libx := $(lhdc_libx_y)
lhdc_libx_clean := $(addsuffix _clean,$(lhdc_libx))
lhdc_libx_txt := $(addsuffix _txt,$(lhdc_libx))

# lhdc_libx pre-build lib filter
lhdc_libx_lib_filter :=
#===============================================================================
#===============================================================================
.PHONY: all clean txt
#===============================================================================
all: $(lhdc_libx)
clean: $(lhdc_libx_clean)
txt: $(lhdc_libx_txt)
#===============================================================================
.PHONY: $(lhdc_libx) $(lhdc_libx_clean)
#===============================================================================
$(lhdc_libx):
	$(AT)$(XTENSA_MAKE) -C $@ all

$(lhdc_libx_clean):
	$(AT)$(XTENSA_MAKE) -C $(patsubst %_clean,%,$@) clean

$(lhdc_libx_txt):
	$(AT)$(XTENSA_MAKE) -C $(patsubst %_txt,%,$@) BUILD_LIB=y txt
	$(AT)$(XTENSA_MAKE) -C $(patsubst %_txt,%,$@) txt
#===============================================================================
lib := $(addsuffix _lib,$(filter-out $(lhdc_libx_lib_filter),$(lhdc_libx)))
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
