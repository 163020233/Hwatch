#===================================================================================
# @brief   Makefile
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===================================================================================
CFG_SAP_SDK_VERSION=SAP_SDKV100R001
PLATFORM ?= apps
TO_UPPER_CASE = $(shell echo '$1' | tr '[:lower:]' '[:upper:]')
#===================================================================================
ifeq ($(SAP_CFGFILE),)
SAP_CFGFILE=build/configs/brandy/brandy_fpga_liteos_cfg.mak
endif
export SAP_CFGFILE
include $(SAP_TOP_DIR)/$(SAP_CFGFILE)
include $(SAP_TOP_DIR)/build/scripts/variable_cfg.mak
include $(SAP_TOP_DIR)/build/configs/$(CFG_SAP_CHIP_TYPE)/$(CFG_SAP_CHIP_TYPE)_base.mak
#===================================================================================
ifeq ($(SECUREC_DIR),)
$(error "SECUREC_DIR is not specified")
endif
ifeq ($(SAP_OPEN_SOURCE_DIR),)
$(error "SAP_OPEN_SOURCE_DIR is not specified")
endif
#===================================================================================
SAP_MPI_DIR   := $(SAP_TOP_DIR)/api
SAP_DRV_DIR   := $(SAP_TOP_DIR)/drv
SAP_CORE_DIR  := $(SAP_TOP_DIR)/core
SAP_BUILD_DIR := $(SAP_TOP_DIR)/core/build
#===================================================================================
XTENSA_BUILD_DIR := $(SAP_BUILD_DIR)/$(CFG_SAP_DSP_OS)
LINUX_BUILD_DIR  := $(SAP_BUILD_DIR)/$(CFG_SAP_CPU_OS)
FIRMWARE_PREFIX := $(CFG_SAP_DSP_FRIMWARE_NAME)
#===================================================================================
SAP_FEATURE_CONFIGS += -DSAP_CHIP_TYPE=$(CFG_SAP_CHIP_TYPE)
SAP_FEATURE_CONFIGS += -DSAP_CODEC_CLK_DEF=$(CFG_SAP_CODEC_CLK_DEF)
#===================================================================================
SAP_FEATURE_CONFIGS += \
	-I$(SECUREC_INCLUDE) \
	-I$(SAP_TOP_DIR)/include \
	-I$(SAP_CORE_DIR)/include \
	-I$(SAP_CORE_DIR)/common/include
#===================================================================================
ifeq ($(PLATFORM), apps)
ifeq ($(CFG_SAP_PROC_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_PROC_SUPPORT
endif
ifeq ($(CFG_SAP_PROC_TYPE_DUMMY), y)
SAP_FEATURE_CONFIGS += -DSAP_PROC_TYPE_DUMMY
ifeq ($(CFG_SAP_PROC_OUTPUT_UART), y)
SAP_FEATURE_CONFIGS += -DSAP_PROC_OUTPUT_UART
endif
ifeq ($(CFG_SAP_PROC_OUTPUT_DIAG_SHELL), y)
SAP_FEATURE_CONFIGS += -DSAP_PROC_OUTPUT_DIAG_SHELL
endif
endif
endif
#===================================================================================
ifeq ($(CFG_SAP_DEBUG_MODULE_HANDLE), y)
SAP_FEATURE_CONFIGS += -DSAP_DEBUG_MODULE_HANDLE
endif
#===================================================================================
ifeq ($(CFG_SAP_DEBUG_BASIC_STATUS_INFO), y)
SAP_FEATURE_CONFIGS += -DSAP_DEBUG_BASIC_STATUS_INFO
endif
#===================================================================================
ifeq ($(CFG_SAP_DEBUG_RUN_STATUS_INFO), y)
SAP_FEATURE_CONFIGS += -DSAP_DEBUG_RUN_STATUS_INFO
endif
#===================================================================================
ifeq ($(CFG_SAP_SYNC_STATUS_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_SYNC_STATUS_SUPPORT
endif
#===================================================================================
ifeq ($(CFG_SAP_MEM_STATISTIC), y)
SAP_FEATURE_CONFIGS += -DSAP_MEM_STATISTIC
endif
#===================================================================================
ifeq ($(CFG_SAP_MCPS_STATISTIC), y)
SAP_FEATURE_CONFIGS += -DSAP_MCPS_STATISTIC
endif
#===================================================================================
ifeq ($(CFG_SAP_TIME_CONSUME), y)
SAP_FEATURE_CONFIGS += -DSAP_TIME_CONSUME
endif
#===================================================================================
ifeq ($(CFG_SAP_LPC_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_LPC_SUPPORT
endif
#===================================================================================
ifeq ($(CFG_SAP_FLASH_FIRMWARE_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_FLASH_FIRMWARE_SUPPORT
endif
#===================================================================================
ifeq ($(CFG_SAP_SHELL_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_SHELL_SUPPORT
endif
#===================================================================================
ifeq ($(CFG_SAP_HAID_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_HAID_SUPPORT
endif
#===================================================================================
ifeq ($(CFG_SAP_BINAURAL_SYNC_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_BINAURAL_SYNC_SUPPORT
endif
#===================================================================================
ifeq ($(CFG_SAP_BT_MUSIC_SNK_LOW_LATENCY_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_BT_MUSIC_SNK_LOW_LATENCY_SUPPORT
endif
#===================================================================================
ifeq ($(CFG_SAP_BT_MUSIC_SEND_MUTE_PACKET_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_BT_MUSIC_SEND_MUTE_PACKET_SUPPORT
endif
#===================================================================================
ifeq ($(CFG_SAP_LOWPOWER_MSG_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_LOWPOWER_MSG_SUPPORT
endif
#===================================================================================
ifeq ($(CFG_SAP_BT_MUSIC_OFFLOAD_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_BT_MUSIC_OFFLOAD_SUPPORT
endif
#===================================================================================
ifeq ($(CFG_SAP_HW_REF_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_HW_REF_SUPPORT
endif
#===================================================================================
ifeq ($(CFG_SAP_HAID_DMIC_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_HAID_DMIC_SUPPORT
endif
#===================================================================================
ifeq ($(CFG_SAP_DUMP_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_DUMP_SUPPORT
endif
#===================================================================================
ifeq ($(CFG_SAP_HOOK_DATA_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_HOOK_DATA_SUPPORT
endif
#===================================================================================
ifeq ($(CFG_SAP_HOOK_MIX_DATA_IN_SEA_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_HOOK_MIX_DATA_IN_SEA_SUPPORT
endif
#===================================================================================
ifeq ($(CFG_SAP_FPGA), y)
SAP_FEATURE_CONFIGS += -DSAP_FPGA
BOARD_TYPE = fpga
endif
#===================================================================================
ifneq ($(CFG_SAP_ES8311_0_I2C),)
SAP_FEATURE_CONFIGS += -DSAP_ES8311_0_I2C=$(CFG_SAP_ES8311_0_I2C)
SAP_FEATURE_CONFIGS += -DSAP_ES8311_0_I2C_DEV_ADDR=$(CFG_SAP_ES8311_0_I2C_DEV_ADDR)
endif
#===================================================================================
ifneq ($(CFG_SAP_ES8311_1_I2C),)
SAP_FEATURE_CONFIGS += -DSAP_ES8311_1_I2C=$(CFG_SAP_ES8311_1_I2C)
SAP_FEATURE_CONFIGS += -DSAP_ES8311_1_I2C_DEV_ADDR=$(CFG_SAP_ES8311_1_I2C_DEV_ADDR)
endif
#===================================================================================
ifeq ($(CFG_SAP_AIAO_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_AIAO_SUPPORT
endif
#===================================================================================
ifneq ($(CFG_SAP_DEFAULT_TX_PORT),)
SAP_FEATURE_CONFIGS += -DSAP_DEFAULT_TX_PORT=$(CFG_SAP_DEFAULT_TX_PORT)
endif

ifneq ($(CFG_SAP_DEFAULT_RX_PORT),)
SAP_FEATURE_CONFIGS += -DSAP_DEFAULT_RX_PORT=$(CFG_SAP_DEFAULT_RX_PORT)
endif
#===================================================================================
ifeq ($(CFG_SAP_IPC_LOG_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_IPC_LOG_SUPPORT
endif
ifeq ($(CFG_SAP_IPC_LOG_USE_DIAG_FRAME), y)
SAP_FEATURE_CONFIGS += -DSAP_IPC_LOG_USE_DIAG_FRAME
endif
ifeq ($(CFG_SAP_IPC_LOG_WITHOUT_HEADER), y)
SAP_FEATURE_CONFIGS += -DSAP_IPC_LOG_WITHOUT_HEADER
endif
ifeq ($(CFG_SAP_DIAG_LOG_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_DIAG_LOG_SUPPORT
endif
#===================================================================================
ifeq ($(CFG_SAP_VENDOR_DRIVER_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_VENDOR_DRIVER_SUPPORT
ifeq ($(CFG_SAP_VENDOR_DRIVER_ANC_CODEC_SUPPORT), y)
SAP_FEATURE_CONFIGS += \
    -DSAP_ANC_CODEC_I2C_BUS_ID=$(CFG_SAP_ANC_CODEC_I2C_BUS_ID) \
    -DSAP_ANC_CODEC_I2C_ADDR=$(CFG_SAP_ANC_CODEC_I2C_ADDR)
endif

ifeq ($(CFG_SAP_VENDOR_DRIVER_AW88262_SUPPORT), y)
SAP_FEATURE_CONFIGS += \
    -DSAP_AW88262_I2C_BUS_ID=$(CFG_SAP_AW88262_I2C_BUS_ID) \
    -DSAP_AW88262_I2C_ADDR=$(CFG_SAP_AW88262_I2C_ADDR)
endif

ifeq ($(CFG_SAP_VENDOR_DRIVER_AW85825_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_VENDOR_DRIVER_AW85825_SUPPORT
endif

ifeq ($(CFG_SAP_VENDOR_DRIVER_ACM8623_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_VENDOR_DRIVER_ACM8623_SUPPORT
ifeq ($(CFG_SAP_ACM8623_PBTL_MODE_ENABLE), y)
SAP_FEATURE_CONFIGS += -DSAP_ACM8623_PBTL_MODE_ENABLE
endif
endif

ifeq ($(CFG_SAP_VENDOR_DRIVER_ACM8625S_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_VENDOR_DRIVER_ACM8625S_SUPPORT
endif

ifeq ($(CFG_SAP_VENDOR_DRIVER_ACM8815_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_VENDOR_DRIVER_ACM8815_SUPPORT
endif

endif
#===================================================================================
ifeq ($(CFG_SAP_SEA_SEE_1MIC_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_SEA_SEE_1MIC_SUPPORT
ifeq ($(CFG_SAP_SEA_SEE_1MIC_WNR_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_SEA_SEE_1MIC_WNR_SUPPORT
endif
ifeq ($(CFG_SAP_SEA_SEE_1MIC_NNSPK_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_SEA_SEE_1MIC_NNSPK_SUPPORT
endif
ifeq ($(CFG_SAP_SEA_SEE_1MIC_EQ_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_SEA_SEE_1MIC_EQ_SUPPORT
endif
endif
#===================================================================================
ifeq ($(CFG_SAP_SEA_SEE_1MIC_PROTECT_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_SEA_SEE_1MIC_PROTECT_SUPPORT
endif
#===================================================================================
ifeq ($(CFG_SAP_SEA_KWS_GRU_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_SEA_KWS_GRU_SUPPORT
else ifeq ($(CFG_SAP_SEA_PHS_GRU_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_SEA_PHS_GRU_SUPPORT
else
endif
#===================================================================================
ifeq ($(CFG_SAP_SEA_SEE_PHS_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_SEA_SEE_PHS_SUPPORT
endif
#===================================================================================
SAP_FEATURE_CONFIGS += -DSAP_DSP_CLK_HZ=$(CFG_SAP_DSP_CLK_HZ)
#=============================BOARD_FPGA/BOARD_ASIC=================================
SAP_FEATURE_CONFIGS += -DBOARD_$(call TO_UPPER_CASE,$(CFG_SAP_BOARD_TYPE))
#=============================BOARD_ES/BOARD_CS=====================================
SAP_FEATURE_CONFIGS += -DBOARD_$(call TO_UPPER_CASE,$(CFG_SAP_CHIP_REVERSION))
#===================================================================================
SAP_FEATURE_CONFIGS += -DSAP_DSP_VERSION=\"$(CFG_SAP_DSP_VERSION)\"
#===================================================================================
ifeq ($(CFG_SAP_EFFECT_SWS_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_EFFECT_SWS_SUPPORT
else ifeq ($(CFG_SAP_EFFECT_SMARTPA_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_EFFECT_SMARTPA_SUPPORT
else ifeq ($(CFG_SAP_EFFECT_SMARTPA_V1_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_EFFECT_SMARTPA_V1_SUPPORT
else ifeq ($(CFG_SAP_EFFECT_WDRC_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_EFFECT_WDRC_SUPPORT
else ifeq ($(CFG_SAP_EFFECT_HVS_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_EFFECT_HVS_SUPPORT
else ifeq ($(CFG_SAP_EFFECT_CUSTOMER_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_EFFECT_CUSTOMER_SUPPORT
endif

ifeq ($(CFG_SAP_EFFECT_AEF_PRE_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_EFFECT_AEF_PRE_SUPPORT
endif

ifeq ($(CFG_SAP_EFFECT_DRC_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_EFFECT_DRC_SUPPORT
endif

ifeq ($(CFG_SAP_EFFECT_PEQ_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_EFFECT_PEQ_SUPPORT
endif

ifeq ($(CFG_SAP_EFFECT_SOFT_EQ_TYPE_SWITCH_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_EFFECT_SOFT_EQ_TYPE_SWITCH_SUPPORT
endif

ifeq ($(CFG_SAP_EFFECT_HW_EQ_V1_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_EFFECT_HW_EQ_V1_SUPPORT
endif
#===================================================================================
ifeq ($(CFG_SAP_AUDIO_SPEED_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_AUDIO_SPEED_SUPPORT
endif
#===================================================================================
ifeq ($(CFG_SAP_AENC_OUT_AFULL_EVENT_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_AENC_OUT_AFULL_EVENT_SUPPORT
endif
#===================================================================================
ifeq ($(CFG_SAP_ADEC_PSS_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_ADEC_PSS_SUPPORT
endif
#===================================================================================
ifeq ($(CFG_SAP_MUTE_DETECT_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_MUTE_DETECT_SUPPORT
endif
ifeq ($(CFG_SAP_MUTE_DETECT_CLAMP_NOISE_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_MUTE_DETECT_CLAMP_NOISE_SUPPORT
endif
#===================================================================================
ifeq ($(CFG_SAP_NATURE_DSP_SIGNAL_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_NATURE_DSP_SIGNAL_SUPPORT
endif
#===================================================================================
ifeq ($(CFG_SAP_NPU_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_NPU_SUPPORT
endif
#===================================================================================
ifeq ($(CFG_SAP_SUPPORT_RT_SMALL_MEM), y)
SAP_FEATURE_CONFIGS += -DSAP_SUPPORT_RT_SMALL_MEM
endif
#===================================================================================
ifeq ($(CFG_SAP_AUDIO_DSP_ROM_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_AUDIO_DSP_ROM_SUPPORT
endif
#===================================================================================
ifeq ($(CFG_SAP_LIVE_MIC_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_LIVE_MIC_SUPPORT
ifeq ($(CFG_SAP_BMIC_UAC_8_CHANNELS_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_BMIC_UAC_8_CHANNELS_SUPPORT
endif
endif
#===================================================================================
ifeq ($(CFG_SAP_CODEC_UNI), y)
SAP_FEATURE_CONFIGS += -DSAP_CODEC_UNI
endif
#===================================================================================
ifeq ($(CFG_SAP_SIGN_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_SIGN_SUPPORT
endif
#===================================================================================
ifeq ($(CFG_SAP_SIGN_CHECK_ONLY_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_SIGN_CHECK_ONLY_SUPPORT
endif
#===================================================================================
ifeq ($(CFG_SAP_ANC_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_ANC_SUPPORT
endif
#===================================================================================
ifneq ($(findstring $(CFG_SAP_CODEC_FLAC_DEC_SUPPORT),y m),)
SAP_FEATURE_CONFIGS += -DSAP_CODEC_FLAC_DEC_SUPPORT
SAP_FEATURE_CONFIGS += -DSAP_CODEC_FLAC_DEC_IN_FRAME_MAX_SIZE=$(CFG_SAP_CODEC_FLAC_DEC_IN_FRAME_MAX_SIZE)
endif

ifneq ($(findstring $(CFG_SAP_CODEC_L2HC_V2_DEC_SUPPORT),y m),)
SAP_FEATURE_CONFIGS += -DSAP_CODEC_L2HC_V2_DEC_SUPPORT
endif
ifneq ($(findstring $(CFG_SAP_CODEC_L2HC_V4_DEC_SUPPORT),y m),)
SAP_FEATURE_CONFIGS += -DSAP_CODEC_L2HC_V4_DEC_SUPPORT
endif
ifneq ($(findstring $(CFG_SAP_CODEC_L2HC_V4_ENC_SUPPORT),y m),)
SAP_FEATURE_CONFIGS += -DSAP_CODEC_L2HC_V4_ENC_SUPPORT
endif
#===================================================================================
ifeq ($(CFG_SAP_UNKNOWN_PRODUCT), y)
SAP_FEATURE_CONFIGS += -DSAP_UNKNOWN_PRODUCT
endif

ifeq ($(CFG_SAP_SMART_WATCH_PRODUCT), y)
SAP_FEATURE_CONFIGS += -DSAP_SMART_WATCH_PRODUCT
endif

ifeq ($(CFG_SAP_TWS_PRODUCT), y)
SAP_FEATURE_CONFIGS += -DSAP_TWS_PRODUCT
endif

ifeq ($(CFG_SAP_BMIC_TX_PRODUCT), y)
SAP_FEATURE_CONFIGS += -DSAP_BMIC_TX_PRODUCT
endif

ifeq ($(CFG_SAP_BMIC_RX_PRODUCT), y)
SAP_FEATURE_CONFIGS += -DSAP_BMIC_RX_PRODUCT
endif

ifeq ($(CFG_SAP_SOUND_BOX_PRODUCT), y)
SAP_FEATURE_CONFIGS += -DSAP_SOUND_BOX_PRODUCT
endif

ifeq ($(CFG_SAP_DONGLE_PRODUCT), y)
SAP_FEATURE_CONFIGS += -DSAP_DONGLE_PRODUCT
endif

ifeq ($(CFG_SAP_HA_PRODUCT), y)
SAP_FEATURE_CONFIGS += -DSAP_HA_PRODUCT
endif
#===================================================================================
ifeq ($(CFG_SAP_TEST_CORE_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_TEST_CORE_SUPPORT
endif

ifeq ($(CFG_SAP_BT_MUSIC_FLOW_CTRL_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_BT_MUSIC_FLOW_CTRL_SUPPORT
endif
#===================================================================================
ifeq ($(CFG_SAP_AUDIO_DSP_BSS_IN_DRAM_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_AUDIO_DSP_BSS_IN_DRAM_SUPPORT
endif

ifeq ($(CFG_SAP_MCU_PSRAM_DYNAMIC_VETO_SUPPORT), y)
SAP_FEATURE_CONFIGS += -DSAP_MCU_PSRAM_DYNAMIC_VETO_SUPPORT
endif
#===================================================================================
ifeq ($(CFG_SAP_BRANDY), y)
SAP_FEATURE_CONFIGS += -DSAP_BRANDY
endif

ifeq ($(CFG_SAP_SOCMN1), y)
SAP_FEATURE_CONFIGS += -DSAP_SOCMN1
endif

ifeq ($(CFG_SAP_SOCMN2), y)
SAP_FEATURE_CONFIGS += -DSAP_SOCMN2
endif

ifeq ($(CFG_SAP_SW21), y)
SAP_FEATURE_CONFIGS += -DSAP_SW21
endif

ifeq ($(CFG_SAP_SW72), y)
SAP_FEATURE_CONFIGS += -DSAP_SW72
endif

ifeq ($(CFG_SAP_3322), y)
SAP_FEATURE_CONFIGS += -DSAP_3322
endif

ifeq ($(CFG_SAP_SW22), y)
SAP_FEATURE_CONFIGS += -DSAP_SW22
endif
#===================================================================================
ifneq ($(CFG_SAP_DSP_LSP_SEA),)
SAP_FEATURE_CONFIGS += -DSAP_DSP_LSP_SEA
endif

ifneq ($(CFG_SAP_DSP_LSP_AAI),)
SAP_FEATURE_CONFIGS += -DSAP_DSP_LSP_AAI
endif

ifneq ($(CFG_SAP_DSP_LSP_AEF),)
SAP_FEATURE_CONFIGS += -DSAP_DSP_LSP_AEF
endif

ifneq ($(CFG_SAP_DSP_LSP_AEF_PRE),)
SAP_FEATURE_CONFIGS += -DSAP_DSP_LSP_AEF_PRE
endif

ifneq ($(CFG_SAP_DSP_LSP_CODEC),)
SAP_FEATURE_CONFIGS += -DSAP_DSP_LSP_CODEC
endif
