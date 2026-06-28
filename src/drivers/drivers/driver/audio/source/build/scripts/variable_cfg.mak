## 删除cfg文件中传入的引号
CFG_SAP_DSP_VERSION := $(shell echo $(CFG_SAP_DSP_VERSION) | sed s/\"//g)

CFG_SAP_CHIP_TYPE := $(shell echo $(CFG_SAP_CHIP_TYPE) | sed s/\"//g)
CFG_SAP_CHIP_REVERSION := $(shell echo $(CFG_SAP_CHIP_REVERSION) | sed s/\"//g)
CFG_SAP_BOARD_TYPE := $(shell echo $(CFG_SAP_BOARD_TYPE) | sed s/\"//g)

CFG_SAP_CPU_ARCH := $(shell echo $(CFG_SAP_CPU_ARCH) | sed s/\"//g)
CFG_SAP_CPU_OS := $(shell echo $(CFG_SAP_CPU_OS) | sed s/\"//g)
CFG_SAP_DSP_ARCH := $(shell echo $(CFG_SAP_DSP_ARCH) | sed s/\"//g)
CFG_SAP_DSP_OS := $(shell echo $(CFG_SAP_DSP_OS) | sed s/\"//g)
CFG_SAP_DSP_FRIMWARE_NAME := $(shell echo $(CFG_SAP_DSP_FRIMWARE_NAME) | sed s/\"//g)
CFG_SAP_DRV_OVERLAY_LOAD_FROM := $(shell echo $(CFG_SAP_DRV_OVERLAY_LOAD_FROM) | sed s/\"//g)

CFG_SAP_IPC_DRV_VERSION := $(shell echo $(CFG_SAP_IPC_DRV_VERSION) | sed s/\"//g)

CFG_SAP_DSP_LSP_MAIN := $(shell echo $(CFG_SAP_DSP_LSP_MAIN) | sed s/\"//g)
CFG_SAP_DSP_LSP_MAIN1 := $(shell echo $(CFG_SAP_DSP_LSP_MAIN1) | sed s/\"//g)
CFG_SAP_DSP_LSP_SEA := $(shell echo $(CFG_SAP_DSP_LSP_SEA) | sed s/\"//g)
CFG_SAP_DSP_LSP_AAI := $(shell echo $(CFG_SAP_DSP_LSP_AAI) | sed s/\"//g)
CFG_SAP_DSP_LSP_AEF := $(shell echo $(CFG_SAP_DSP_LSP_AEF) | sed s/\"//g)
CFG_SAP_DSP_LSP_AEF_PRE := $(shell echo $(CFG_SAP_DSP_LSP_AEF_PRE) | sed s/\"//g)
CFG_SAP_DSP_LSP_HAID := $(shell echo $(CFG_SAP_DSP_LSP_HAID) | sed s/\"//g)
CFG_SAP_DSP_LSP_CODEC := $(shell echo $(CFG_SAP_DSP_LSP_CODEC) | sed s/\"//g)
CFG_SAP_DSP_LSP_ROM := $(shell echo $(CFG_SAP_DSP_LSP_ROM) | sed s/\"//g)

CFG_SAP_SYNC_HAL_VERSION := $(shell echo $(CFG_SAP_SYNC_HAL_VERSION) | sed s/\"//g)
CFG_SAP_SYNC_TOG_TYPE := $(shell echo $(CFG_SAP_SYNC_TOG_TYPE) | sed s/\"//g)

CFG_SAP_AFE_HAL_VERSION := $(shell echo $(CFG_SAP_AFE_HAL_VERSION) | sed s/\"//g)
CFG_SAP_MAD_HAL_VERSION := $(shell echo $(CFG_SAP_MAD_HAL_VERSION) | sed s/\"//g)
