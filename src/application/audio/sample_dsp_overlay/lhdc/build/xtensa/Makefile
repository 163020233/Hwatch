CUR_DIR := $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)
IDP_TOP_DIR := $(realpath $(CUR_DIR)/../..)

include $(IDP_TOP_DIR)/sap_idp.mak
include xtensa_cfg.mak

AT ?= @
#===============================================================================
FILTER := $(IDP_OUT_DIR)/bin/dsp_overlay.bin

#===============================================================================
component := lhdc
component_clean := $(addsuffix _clean,$(component))
#===============================================================================
depends := lsp
depends_clean := $(addsuffix _clean,$(depends))
#===============================================================================
.PHONY: all clean
#===============================================================================
all: overlay
clean: overlay_clean
#===============================================================================
.PHONY: $(objects) $(objects_clean)
#===============================================================================
$(objects): $(depends)
	$(AT)$(XTENSA_MAKE) -f hifi_$@.mak all

$(objects_clean): $(depends_clean)
	$(AT)$(XTENSA_MAKE) -f hifi_$(patsubst %_clean,%,$@).mak clean
#===============================================================================
.PHONY: $(depends) $(depends_clean)
#===============================================================================
$(depends):
	$(AT)$(XTENSA_MAKE) -f hifi_$@.mak

$(depends_clean):
	$(AT)$(XTENSA_MAKE) -f hifi_$(patsubst %_clean,%,$@).mak clean
#===============================================================================
#                   lib and component lib
#===============================================================================
.PHONY: lib lib_clean
#===============================================================================
lib: $(component)

lib_clean: $(component_clean)
#===============================================================================
.PHONY: $(component) $(component_clean)
#===============================================================================
$(component):
	$(AT)$(XTENSA_MAKE) -C $(IDP_TOP_DIR)/component/$@ lib

$(component_clean):
	$(AT)$(XTENSA_MAKE) -C $(IDP_TOP_DIR)/component/$(patsubst %_clean,%,$@) lib_clean
#===============================================================================
.PHONY: lhdc_bin lhdc_bin_clean
#===============================================================================
lhdc_bin: $(depends)
	$(AT)$(XTENSA_MAKE) -C $(IDP_TOP_DIR)/component/lhdc/plugins

lhdc_bin_clean:
	$(AT)$(XTENSA_MAKE) -C $(IDP_TOP_DIR)/component/lhdc/plugins clean
#===============================================================================
.PHONY: overlay overlay_clean
#===============================================================================
overlay: lhdc_bin
	$(AT)$(XT_ELF_PACK) $(IDP_OUT_DIR)/bin/dsp_$@.bin $(filter-out $(FILTER), $(wildcard $(IDP_OUT_DIR)/bin/*.bin))
	$(AT)echo "Scan & Generate hso xml after compile finished"
	$(AT)$(XT_HSO_XML) $(TXT_DIR)/dsp_log.txt socmn2 unused process ${CFG_SAP_PRODUCT_TYPE} lhdc

overlay_clean: lhdc_bin_clean
	$(AT)rm -rf $(IDP_OUT_DIR)/bin/dsp_$@.bin
#===============================================================================
.PHONY: txt txt_clean
#===============================================================================
txt:
	$(AT)echo "Generate header file for FILE_ID definition"
	$(AT)$(XTENSA_MAKE) -C $(IDP_TOP_DIR)/component/lhdc/plugins txt
	$(AT)$(XT_HSO_XML) $(TXT_DIR)/dsp_log.txt socmn2 unused prepare ${CFG_SAP_PRODUCT_TYPE} lhdc

txt_clean:
	$(AT)echo "Clean header file for FILE_ID definition"
	$(AT)$(XT_HSO_XML) $(TXT_DIR)/dsp_log.txt socmn2 unused clean ${CFG_SAP_PRODUCT_TYPE} lhdc
#===============================================================================
