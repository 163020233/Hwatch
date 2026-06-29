#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "param_client_lite")

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${OHOS_DIR}/base/hiviewdfx/hilog_lite/interfaces/native/kits
    ${OHOS_DIR}/base/startup/init/interfaces/innerkits/include/syspara
    ${OHOS_DIR}/base/startup/init/services/modules/init_hook
    ${OHOS_DIR}/base/startup/init/services/param/include
    ${OHOS_DIR}/base/startup/init/services/param/adapter
    ${OHOS_DIR}/base/startup/init/services/param/base
    ${OHOS_DIR}/base/startup/init/interfaces/innerkits/include/param
    ${OHOS_DIR}/base/startup/init/interfaces/innerkits/include
    ${OHOS_DIR}/base/startup/init/services/init/include
    ${OHOS_DIR}/base/startup/init/services/log
    ${OHOS_DIR}/third_party/cJSON
    ${OHOS_DIR}/commonlibrary/utils_lite/include
    ${PROJECT_BINARY_DIR}/gen/init
)

set(SOURCES
    ${OHOS_DIR}/base/startup/init/services/param/base/param_base.c
    ${OHOS_DIR}/base/startup/init/services/param/base/param_comm.c
    ${OHOS_DIR}/base/startup/init/services/param/base/param_trie.c
    ${OHOS_DIR}/base/startup/init/services/param/liteos/param_client.c
    ${OHOS_DIR}/base/startup/init/services/param/liteos/param_litedac.c
    ${OHOS_DIR}/base/startup/init/services/param/liteos/param_osadp.c
    ${OHOS_DIR}/base/startup/init/services/param/manager/param_manager.c
    ${OHOS_DIR}/base/startup/init/services/param/manager/param_persist.c
    ${OHOS_DIR}/base/startup/init/services/utils/init_hashmap.c
    ${OHOS_DIR}/base/startup/init/services/utils/list.c
    ${OHOS_DIR}/base/startup/init/services/log/init_commlog.c
    ${OHOS_DIR}/base/startup/init/services/param/liteos/param_hal.c
    ${OHOS_DIR}/base/startup/init/services/param/liteos/param_service.c
    ${OHOS_DIR}/base/startup/init/services/param/manager/param_server.c
)

set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-typedef-redefinition
    -Wno-strict-prototypes
    -Wno-error=unused-function
    -Wno-error=sign-compare
    -Wno-error=unused-but-set-variable
)

set(COMPONENT_CXXCFLAGS
)

set(PUBLIC_DEFINES
)

set(PRIVATE_DEFINES
    _GNU_SOURCE
    INCREMENTAL_VERSION="OpenHarmony\ 4.0\ Beta1"
    BUILD_TYPE="debug"
    BUILD_USER="jenkins"
    BUILD_TIME=""
    BUILD_HOST="linux"
    BUILD_ROOTHASH=""
    WORKSPACE_AREA_NEED_MUTEX
    PARAM_PERSIST_SAVE_MUTEX
    PARAMWORKSPACE_NEED_MUTEX
    DATA_PATH="/"
    PARAM_LOAD_CFG_FROM_CODE
    PARAM_WORKSPACE_MAX=5120
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
install_sdk(${OHOS_DIR}/base/startup/init/scripts/ "*")
install_sdk(${OHOS_DIR}/base/startup/init/services/etc_lite/param/ohos_const/ "*")
install_sdk(${OHOS_DIR}/base/startup/init/services/etc/param/ "*")
