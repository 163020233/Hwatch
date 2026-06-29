STRING( REGEX REPLACE ".*/(.*)" "\\1" CURRENT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} )
set(MODULE_NAME ${CURRENT_FOLDER})

set(MODULE_CMACRO_PRI)
list(APPEND MODULE_CMACRO_PRI LOSCFG_MMCSD_SG_SUPPORT)

set(OS_FREEBSD_SRC ${LITEOS_OPENSOURCE_PATHNAME}/freebsd)
set(OS_FREEBSD_INCLUDE_PATH ${LITEOS_OPENSOURCE_PATHNAME}/freebsd)
set(MMC_INCLUDE_PATH ${LITEOSTOPDIR}/drivers/mmc)

set(MMC_FREEBSD_SRC ${OS_FREEBSD_SRC}/liteos/drivers/mmc)
set(MMC_FREEBSD_INCLUDE_PATH ${OS_FREEBSD_INCLUDE_PATH}/liteos/drivers/mmc)

# add global include, compile macro and options to public target #
set(MODULE_INCLUDE_PUB)
set(MODULE_CXXINCLUDE_PUB)
set(MODULE_COPTS_PUB)
set(MODULE_ASOPTS_PUB)
set(MODULE_CXXOPTS_PUB)
set(MODULE_CMACRO_PUB)
set(MODULE_ASMACRO_PUB)
set(MODULE_CXXMACRO_PUB)

# add local include, compile macro and options to private target #
set(MODULE_INCLUDE_PRI)

set(MODULE_COPTS_PRI)
set(MODULE_ASOPTS_PRI)
set(MODULE_CXXOPTS_PRI)
set(MODULE_ASMACRO_PRI)
set(MODULE_CXXMACRO_PRI)

# add srcs to private target #
set(LOCAL_SRCS_y)
set(LOCAL_DIRS_y)

if(LOSCFG_PLATFORM_SUQIN_APP)
    set(MMC_HOST_DIR sdhci)
    list(APPEND LOCAL_DIRS_y host/${MMC_HOST_DIR}/hi3321v100)
    list(APPEND LOCAL_SRCS_y host/${MMC_HOST_DIR}/tune/sdhci_tune.c)
endif()

if(LOSCFG_PLATFORM_HI3516DV300)
    set(MMC_HOST_DIR sdmci)
    list(APPEND LOCAL_DIRS_y host/${MMC_HOST_DIR}/hi3556v200_series)
endif()

if(LOSCFG_PLATFORM_HI3556V200)
    set(MMC_HOST_DIR sdmci)
    list(APPEND LOCAL_DIRS_y host/${MMC_HOST_DIR}/hi3556v200_series)
endif()

if(LOSCFG_PLATFORM_HI3559V200)
    set(MMC_HOST_DIR sdmci)
    list(APPEND LOCAL_DIRS_y host/${MMC_HOST_DIR}/hi3556v200_series)
endif()

if(LOSCFG_PLATFORM_HI3556V200_DCG)
    set(MMC_HOST_DIR sdmci)
    list(APPEND LOCAL_DIRS_y host/${MMC_HOST_DIR}/hi3556v200_series)
endif()

if(LOSCFG_PLATFORM_HI3556AV100)
    set(MMC_HOST_DIR sdmci)
    list(APPEND LOCAL_DIRS_y host/${MMC_HOST_DIR}/hi3556av100)
endif()

if(LOSCFG_PLATFORM_HI3516EV200)
    set(MMC_HOST_DIR sdhci)
    list(APPEND LOCAL_SRCS_y host/${MMC_HOST_DIR}/tune/sdhci_tune_edge.c)
    list(APPEND LOCAL_DIRS_y host/${MMC_HOST_DIR}/hi3518ev20x)
endif()

if(LOSCFG_PLATFORM_HI3518EV300)
    set(MMC_HOST_DIR sdhci)
    list(APPEND LOCAL_SRCS_y host/${MMC_HOST_DIR}/tune/sdhci_tune_edge.c)
    list(APPEND LOCAL_DIRS_y host/${MMC_HOST_DIR}/hi3518ev20x)
endif()

if(LOSCFG_PLATFORM_HI3516EV300)
    set(MMC_HOST_DIR sdhci)
    list(APPEND LOCAL_SRCS_y host/${MMC_HOST_DIR}/tune/sdhci_tune_edge.c)
    list(APPEND LOCAL_DIRS_y host/${MMC_HOST_DIR}/hi3518ev20x)
endif()

if(LOSCFG_PLATFORM_HI3892WV100ES_APP)
    set(MMC_HOST_DIR sdhci)
    list(APPEND LOCAL_SRCS_y host/${MMC_HOST_DIR}/tune/sdhci_tune_edge.c)
    list(APPEND LOCAL_DIRS_y host/${MMC_HOST_DIR}/hi3892v100)
endif()

if(LOSCFG_PLATFORM_HI3559AV100)
    set(MMC_HOST_DIR sdhci)
    list(APPEND LOCAL_SRCS_y host/${MMC_HOST_DIR}/tune/sdhci_tune.c)
    list(APPEND LOCAL_DIRS_y host/${MMC_HOST_DIR}/hi3559av100)
endif()

if(LOSCFG_PLATFORM_SW21_APP)
    set(MMC_HOST_DIR sdhci)
    list(APPEND LOCAL_SRCS_y host/${MMC_HOST_DIR}/tune/sdhci_tune.c)
    list(APPEND LOCAL_DIRS_y host/${MMC_HOST_DIR}/sw21app)
    add_definitions(-DCFG_SDIO_EMMC_COEXIST)
    add_definitions(-DLOFCFG_DRIVERS_MMC_CUSTOM_TUNING)
endif()

if(LOSCFG_PLATFORM_SW21_APP_SE)
    set(MMC_HOST_DIR sdhci)
    list(APPEND LOCAL_SRCS_y host/${MMC_HOST_DIR}/tune/sdhci_tune.c)
    list(APPEND LOCAL_DIRS_y host/${MMC_HOST_DIR}/sw21app)
endif()

if(LOSCFG_PLATFORM_SW22_APP)
    set(MMC_HOST_DIR sdhci)
    add_definitions(-DCFG_SDIO_EMMC_COEXIST)
    list(APPEND LOCAL_DIRS_y host/${MMC_HOST_DIR}/sw22app)
endif()

if(LOSCFG_PLATFORM_HI3372_APP)
    set(MMC_HOST_DIR sdhci)
    list(APPEND LOCAL_SRCS_y host/${MMC_HOST_DIR}/tune/sdhci_tune.c)
    list(APPEND LOCAL_DIRS_y host/${MMC_HOST_DIR}/hi3372)
endif()

if(LOSCFG_PLATFORM_HI3322_APP)
    set(MMC_HOST_DIR sdhci)
    list(APPEND LOCAL_DIRS_y host/${MMC_HOST_DIR}/hi3322app)
    add_definitions(-DCFG_SDIO_EMMC_COEXIST) # sdio && emmc 共存，可以同时运行
    add_definitions(-DLOFCFG_DRIVERS_MMC_CUSTOM_TUNING) # 自定义tuning策略
endif()

if(LOSCFG_PLATFORM_HI1A72_APP)
    set(MMC_HOST_DIR sdhci)
    list(APPEND LOCAL_SRCS_y host/${MMC_HOST_DIR}/tune/sdhci_tune.c)
    list(APPEND LOCAL_DIRS_y host/${MMC_HOST_DIR}/hi1a72)
endif()

if(${MMC_HOST_DIR} MATCHES "sdmci")
    list(APPEND LOCAL_DIRS_y host/${MMC_HOST_DIR}/tune)
    list(APPEND MODULE_INCLUDE_PRI
        ${MMC_INCLUDE_PATH}/host/${MMC_HOST_DIR}/tune
        ${MMC_FREEBSD_INCLUDE_PATH}/host/${MMC_HOST_DIR}/tune
    )
endif()

list(APPEND MODULE_INCLUDE_PRI
    ${MMC_INCLUDE_PATH}/host/${MMC_HOST_DIR}
    ${MMC_FREEBSD_INCLUDE_PATH}/host/${MMC_HOST_DIR}
    ${LITEOSTOPDIR}/arch_lite/riscv/include/arch
)

list(APPEND LOCAL_DIRS_y host/${MMC_HOST_DIR})
list(APPEND LOCAL_DIRS_y ${MMC_FREEBSD_SRC}/host/${MMC_HOST_DIR})

list(APPEND LOCAL_DIRS_y core)
list(APPEND LOCAL_DIRS_y host)
list(APPEND LOCAL_DIRS_y ${MMC_FREEBSD_SRC}/host)
list(APPEND LOCAL_DIRS_y ${MMC_FREEBSD_SRC}/core)

if(LOSCFG_FS_PROC)
    list(APPEND LOCAL_DIRS_y proc)
endif()

list(APPEND MODULE_INCLUDE_PRI
    ${MMC_INCLUDE_PATH}/proc
    ${MMC_INCLUDE_PATH}/core
    ${MMC_INCLUDE_PATH}/host
    ${MMC_INCLUDE_PATH}/adapt_liteos
    ${MMC_FREEBSD_INCLUDE_PATH}/core
    ${MMC_FREEBSD_INCLUDE_PATH}/host
)

FOREACH(CUR_DIR ${LOCAL_DIRS_y})
    aux_source_directory(${CUR_DIR} LOCAL_SRCS_y)
ENDFOREACH(CUR_DIR)

set(LOCAL_SRCS ${LOCAL_SRCS_y})  # module.cmake will sort LOCAL_SRCS files

include(${MODULE})
