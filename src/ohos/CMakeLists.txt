#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

if("param_client_lite" IN_LIST RAM_COMPONENT)
    set(INSTALL_PARAM_H
        ${Python3_EXECUTABLE}
        ${OHOS_DIR}/base/startup/init/scripts/param_cfg_to_code.py
        --source
        ${OHOS_DIR}/base/startup/init/services/etc_lite/param/ohos_const/ohos.para
        --source
        ${OHOS_DIR}/vendor/hisilicon/${OHOS_PRODUCT_NAME}/hals/utils/sys_param/vendor.para
        --source
        ${OHOS_DIR}/base/startup/init/services/etc/param/ohos.para
        --dest_dir
        ${PROJECT_BINARY_DIR}/gen/init/
    )
    string(JOIN " " INSTALL_PARAM_H_STR ${INSTALL_PARAM_H})
    message(STATUS "INSTALL_PARAM_H = ${INSTALL_PARAM_H_STR}")
    file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/gen/init")
    execute_process(
        COMMAND ${INSTALL_PARAM_H}
        OUTPUT_VARIABLE output
        RESULT_VARIABLE result
    )
endif()

if("freetype_static" IN_LIST RAM_COMPONENT)
    set(INSTALL_FREETYPE
        ${Python3_EXECUTABLE}
        ${OHOS_DIR}/third_party/freetype/install.py
        --gen-dir
        ${PROJECT_BINARY_DIR}/gen/third_party/freetype
        --source-dir
        ${OHOS_DIR}/third_party/freetype
    )
    string(JOIN " " INSTALL_FREETYPE_STR ${INSTALL_FREETYPE})
    message(STATUS "INSTALL_FREETYPE = ${INSTALL_FREETYPE_STR}")
    file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/gen/third_party/freetype/freetype")
    execute_process(
        COMMAND ${INSTALL_FREETYPE}
        OUTPUT_VARIABLE output
        RESULT_VARIABLE result
    )
endif()

if("harfbuzz" IN_LIST RAM_COMPONENT)
    set(INSTALL_HARFBUZZ
        ${Python3_EXECUTABLE}
        ${OHOS_DIR}/third_party/harfbuzz/install.py
        --gen-dir
        ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz
        --source-dir
        ${OHOS_DIR}/third_party/harfbuzz
    )
    string(JOIN " " INSTALL_HARFBUZZ_STR ${INSTALL_HARFBUZZ})
    message(STATUS "INSTALL_HARFBUZZ = ${INSTALL_HARFBUZZ_STR}")
    file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz")
    execute_process(
        COMMAND ${INSTALL_HARFBUZZ}
        OUTPUT_VARIABLE output
        RESULT_VARIABLE result
    )
endif()

if("png_static" IN_LIST RAM_COMPONENT)
    set(INSTALL_LIBPNG
        ${Python3_EXECUTABLE}
        ${OHOS_DIR}/third_party/libpng/install.py
        --gen-dir
        ${PROJECT_BINARY_DIR}/gen/third_party/libpng
        --source-dir
        ${OHOS_DIR}/third_party/libpng
    )
    string(JOIN " " INSTALL_LIBPNG_STR ${INSTALL_LIBPNG})
    message(STATUS "INSTALL_LIBPNG = ${INSTALL_LIBPNG_STR}")
    file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/gen/third_party/libpng")
    execute_process(
        COMMAND ${INSTALL_LIBPNG}
        OUTPUT_VARIABLE output
        RESULT_VARIABLE result
    )
endif()

if("jpeg_static" IN_LIST RAM_COMPONENT)
    set(INSTALL_LIBJPEG
        ${Python3_EXECUTABLE}
        ${OHOS_DIR}/third_party/libjpeg-turbo/install.py
        --gen-dir
        ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo
        --source-dir
        ${OHOS_DIR}/third_party/libjpeg-turbo
    )
    string(JOIN " " INSTALL_LIBJPEG_STR ${INSTALL_LIBJPEG})
    message(STATUS "INSTALL_LIBJPEG = ${INSTALL_LIBJPEG_STR}")
    file(MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo")
    execute_process(
        COMMAND ${INSTALL_LIBJPEG}
        OUTPUT_VARIABLE output
        RESULT_VARIABLE result
    )
endif()
add_subdirectory_with_alias_if_exist(third_party/freetype ${PROJECT_BINARY_DIR}/freetype)
if (DEFINES MATCHES "ENABLE_UIKIT")
    set(ROOTDIR ${ROOT_DIR})
    add_subdirectory_with_alias_if_exist(foundation/arkui/ui_lite ${PROJECT_BINARY_DIR}/ohos/uikit/ui_lite)
    add_subdirectory_with_alias_if_exist(foundation/graphic/graphic_utils_lite ${PROJECT_BINARY_DIR}/ohos/uikit/utils)
    add_subdirectory_with_alias_if_exist(third_party/cJSON ${PROJECT_BINARY_DIR}/cJSON)
    add_subdirectory_with_alias_if_exist(third_party/qrcodegen ${PROJECT_BINARY_DIR}/qrcodegen)
    add_subdirectory_with_alias_if_exist(third_party/icu/icu4c/source/common ${PROJECT_BINARY_DIR}/icu)
    add_subdirectory_with_alias_if_exist(third_party/harfbuzz ${PROJECT_BINARY_DIR}/harfbuzz)
elseif (DEFINES MATCHES "CFG_NORFLASH_ONLY")
    add_subdirectory_with_alias_if_exist(third_party/cJSON ${PROJECT_BINARY_DIR}/cJSON)
endif()

if (DEFINES MATCHES "XTS_SUPPORT")
    add_subdirectory_with_alias_if_exist(third_party/libjpeg ${PROJECT_BINARY_DIR}/libjpeg)
endif()

add_subdirectory_with_alias_if_exist(base/startup/bootstrap_lite/services/source ${PROJECT_BINARY_DIR}/ohos/bootstrap)
add_subdirectory_with_alias_if_exist(third_party/libpng ${PROJECT_BINARY_DIR}/libpng)
add_subdirectory_with_alias_if_exist(third_party/libjpeg-turbo ${PROJECT_BINARY_DIR}/libjpeg-turbo)
# add_subdirectory_with_alias_if_exist(base/startup/syspara_lite ${PROJECT_BINARY_DIR}/ohos/syspara)

add_subdirectory_with_alias_if_exist(base/startup/init/interfaces/hals/utils/sys_param ${PROJECT_BINARY_DIR}/ohos/syspara/hal_sys_param)
add_subdirectory_with_alias_if_exist(base/startup/init/simulator ${PROJECT_BINARY_DIR}/ohos/syspara/simulator)
add_subdirectory_with_alias_if_exist(base/startup/init/services/log ${PROJECT_BINARY_DIR}/ohos/bootstrap/log)
add_subdirectory_with_alias_if_exist(base/startup/init/services/param/liteos ${PROJECT_BINARY_DIR}/ohos/bootstrap/param)
add_subdirectory_with_alias_if_exist(base/startup/init/services/utils ${PROJECT_BINARY_DIR}/ohos/bootstrap/utils)
# add_subdirectory_with_alias_if_exist(foundation/distributedschedule/samgr_lite ${PROJECT_BINARY_DIR}/ohos/samgr)
add_subdirectory_with_alias_if_exist(foundation/systemabilitymgr/samgr_lite/communication/broadcast ${PROJECT_BINARY_DIR}/ohos/samgr/broadcast)
add_subdirectory_with_alias_if_exist(foundation/systemabilitymgr/samgr_lite/samgr/adapter ${PROJECT_BINARY_DIR}/ohos/samgr/adapter)
add_subdirectory_with_alias_if_exist(foundation/systemabilitymgr/samgr_lite/samgr ${PROJECT_BINARY_DIR}/ohos/samgr/samgr)
add_subdirectory_with_alias_if_exist(foundation/systemabilitymgr/samgr_lite/samgr/source ${PROJECT_BINARY_DIR}/ohos/samgr/source)

# add_subdirectory_with_alias_if_exist(foundation/aafwk/aafwk_lite ${PROJECT_BINARY_DIR}/ohos/aafwk)
add_subdirectory_with_alias_if_exist(foundation/ability/ability_lite/frameworks/ability_lite ${PROJECT_BINARY_DIR}/ohos/aafwk/ability)
add_subdirectory_with_alias_if_exist(foundation/ability/ability_lite/frameworks/abilitymgr_lite ${PROJECT_BINARY_DIR}/ohos/aafwk/abilitymanager)
add_subdirectory_with_alias_if_exist(foundation/ability/ability_lite/frameworks/want_lite ${PROJECT_BINARY_DIR}/ohos/aafwk/want)
add_subdirectory_with_alias_if_exist(foundation/ability/ability_lite/services/abilitymgr_lite ${PROJECT_BINARY_DIR}/ohos/aafwk/abilityms)

# add_subdirectory_with_alias_if_exist(foundation/appexecfwk/appexecfwk_lite ${PROJECT_BINARY_DIR}/ohos/appexecfwk)
add_subdirectory_with_alias_if_exist(foundation/bundlemanager/bundle_framework_lite/frameworks/bundle_lite ${PROJECT_BINARY_DIR}/ohos/appexecfwk/bundle)
add_subdirectory_with_alias_if_exist(foundation/bundlemanager/bundle_framework_lite/services/bundlemgr_lite ${PROJECT_BINARY_DIR}/ohos/appexecfwk/bundlems)
add_subdirectory_with_alias_if_exist(foundation/bundlemanager/bundle_framework_lite/services/bundlemgr_lite/tools/signmode ${PROJECT_BINARY_DIR}/ohos/appexecfwk/bms_sign_mode)
add_subdirectory_with_alias_if_exist(third_party/zlib ${PROJECT_BINARY_DIR}/zlib)

# add_subdirectory_with_alias_if_exist(foundation/ace/ace_engine_lite ${PROJECT_BINARY_DIR}/ohos/ace_engine)

add_subdirectory_with_alias_if_exist(foundation/arkui/ace_engine_lite/frameworks ${PROJECT_BINARY_DIR}/ohos/ace_engine_lite/ace_lite)
add_subdirectory_with_alias_if_exist(foundation/arkui/ace_engine_lite/frameworks/common ${PROJECT_BINARY_DIR}/ohos/ace_engine_lite/ace_common)
add_subdirectory_with_alias_if_exist(foundation/arkui/ace_engine_lite/frameworks/module_manager ${PROJECT_BINARY_DIR}/ohos/ace_engine_lite/ace_module_manager)
add_subdirectory_with_alias_if_exist(foundation/arkui/ace_engine_lite/frameworks/native_engine ${PROJECT_BINARY_DIR}/ohos/ace_engine_lite/ace_native_engine)
add_subdirectory_with_alias_if_exist(foundation/arkui/ace_engine_lite/frameworks/src/core/components/baidumap ${PROJECT_BINARY_DIR}/ohos/ace_engine_lite/baidu_components)
add_subdirectory_with_alias_if_exist(foundation/arkui/ace_engine_lite/frameworks/src/core/modules/baidumap ${PROJECT_BINARY_DIR}/ohos/ace_engine_lite/baidu_map)
add_subdirectory_with_alias_if_exist(foundation/arkui/ace_engine_lite/frameworks/src/core/modules/battery ${PROJECT_BINARY_DIR}/ohos/ace_engine_lite/battery)
add_subdirectory_with_alias_if_exist(foundation/arkui/ace_engine_lite/frameworks/src/core/modules/brightness ${PROJECT_BINARY_DIR}/ohos/ace_engine_lite/ace_brightness)
add_subdirectory_with_alias_if_exist(foundation/arkui/ace_engine_lite/frameworks/src/core/modules/keyinput ${PROJECT_BINARY_DIR}/ohos/ace_engine_lite/key_input)
add_subdirectory_with_alias_if_exist(foundation/arkui/ace_engine_lite/frameworks/src/core/modules/network ${PROJECT_BINARY_DIR}/ohos/ace_engine_lite/network)
add_subdirectory_with_alias_if_exist(foundation/arkui/ace_engine_lite/frameworks/src/core/modules/nfc ${PROJECT_BINARY_DIR}/ohos/ace_engine_lite/nfc)
add_subdirectory_with_alias_if_exist(foundation/arkui/ace_engine_lite/frameworks/src/core/modules/calendarManager ${PROJECT_BINARY_DIR}/ohos/ace_engine_lite/calendarManager)
add_subdirectory_with_alias_if_exist(foundation/arkui/ace_engine_lite/frameworks/src/core/modules/vibrator ${PROJECT_BINARY_DIR}/ohos/ace_engine_lite/vibrator)
add_subdirectory_with_alias_if_exist(foundation/arkui/ace_engine_lite/frameworks/src/core/modules/deviceinfo ${PROJECT_BINARY_DIR}/ohos/ace_engine_lite/device)
add_subdirectory_with_alias_if_exist(foundation/arkui/ace_engine_lite/frameworks/src/core/modules/locale ${PROJECT_BINARY_DIR}/ohos/ace_engine_lite/locale)
add_subdirectory_with_alias_if_exist(foundation/arkui/ace_engine_lite/frameworks/src/core/modules/geolocation ${PROJECT_BINARY_DIR}/ohos/ace_engine_lite/geolocation)
add_subdirectory_with_alias_if_exist(foundation/arkui/ace_engine_lite/frameworks/src/core/modules/sensor ${PROJECT_BINARY_DIR}/ohos/ace_engine_lite/sensor)
add_subdirectory_with_alias_if_exist(foundation/arkui/ace_engine_lite/frameworks/src/core/modules/netstack/http ${PROJECT_BINARY_DIR}/ohos/ace_engine_lite/netstack_http)
add_subdirectory_with_alias_if_exist(foundation/arkui/ace_engine_lite/frameworks/src/core/modules/http ${PROJECT_BINARY_DIR}/ohos/ace_engine_lite/http)
add_subdirectory_with_alias_if_exist(foundation/arkui/ace_engine_lite/frameworks/src/core/modules/websocket ${PROJECT_BINARY_DIR}/ohos/ace_engine_lite/websocket)
add_subdirectory_with_alias_if_exist(foundation/arkui/ace_engine_lite/frameworks/src/core/modules/bluetooth ${PROJECT_BINARY_DIR}/ohos/ace_engine_lite/bluetooth)
add_subdirectory_with_alias_if_exist(foundation/arkui/ace_engine_lite/frameworks/src/core/modules/socket ${PROJECT_BINARY_DIR}/ohos/ace_engine_lite/socket)
add_subdirectory_with_alias_if_exist(foundation/arkui/ace_engine_lite/frameworks/src/core/ace_ext ${PROJECT_BINARY_DIR}/ohos/ace_engine_lite/ace_ext)
if (DEFINES MATCHES "CONFIG_MAP_DEMO")
    add_subdirectory_with_alias_if_exist(foundation/arkui/ace_engine_lite/frameworks/src/core/map_fwk ${PROJECT_BINARY_DIR}/ohos/ace_engine_lite/map_fwk)
endif()

add_subdirectory_with_alias_if_exist(foundation/multimedia/camera_lite ${PROJECT_BINARY_DIR}/ohos/camera_lite)

add_subdirectory_with_alias_if_exist(foundation/communication/dsoftbus ${PROJECT_BINARY_DIR}/ohos/dsoftbus)
# add_subdirectory_with_alias_if_exist(base/security/permission ${PROJECT_BINARY_DIR}/ohos/permission)
add_subdirectory_with_alias_if_exist(base/security/permission_lite/services/pms ${PROJECT_BINARY_DIR}/ohos/permission/pms)

add_subdirectory_with_alias_if_exist(device/soc/hisilicon/common/hardware/litewearable_base/pms_hal ${PROJECT_BINARY_DIR}/ohos/permission/hal)
add_subdirectory_with_alias_if_exist(device/soc/hisilicon/common/hardware/litewearable_base/utils_hal/token ${PROJECT_BINARY_DIR}/ohos/hal_token)

# add_subdirectory_with_alias_if_exist(base/security/appverify ${PROJECT_BINARY_DIR}/ohos/appverify)
add_subdirectory_with_alias_if_exist(base/security/appverify/interfaces/innerkits/appverify_lite/products/ipcamera ${PROJECT_BINARY_DIR}/ohos/appverify/verify_base)
add_subdirectory_with_alias_if_exist(base/security/appverify/interfaces/innerkits/appverify_lite ${PROJECT_BINARY_DIR}/ohos/appverify/verify)
add_subdirectory_with_alias_if_exist(base/security/huks ${PROJECT_BINARY_DIR}/ohos/huks/huks_sdk)
add_subdirectory_with_alias_if_exist(base/security/huks/frameworks/crypto_lite/cipher ${PROJECT_BINARY_DIR}/ohos/huks/cipher)
add_subdirectory_with_alias_if_exist(base/security/huks/frameworks/crypto_lite/js/builtin ${PROJECT_BINARY_DIR}/ohos/huks/ace_kit_cipher_static)
# add_subdirectory_with_alias_if_exist(third_party/jerryscript ${PROJECT_BINARY_DIR}/ohos/jerryscript)

add_subdirectory_with_alias_if_exist(third_party/jerryscript/jerry-core ${PROJECT_BINARY_DIR}/ohos/jerryscript/core)
add_subdirectory_with_alias_if_exist(third_party/jerryscript/jerry-ext ${PROJECT_BINARY_DIR}/ohos/jerryscript/ext)
add_subdirectory_with_alias_if_exist(third_party/jerryscript/jerry-port/default/ ${PROJECT_BINARY_DIR}/ohos/jerryscript/port)
add_subdirectory_with_alias_if_exist(third_party/lwip ${PROJECT_BINARY_DIR}/ohos/lwip)
# add_subdirectory_with_alias_if_exist(base/global/i18n_lite ${PROJECT_BINARY_DIR}/ohos/i18n)
# add_subdirectory_with_alias_if_exist(base/global/resmgr_lite ${PROJECT_BINARY_DIR}/ohos/resmgr)
add_subdirectory_with_alias_if_exist(base/global/i18n_lite/frameworks/i18n ${PROJECT_BINARY_DIR}/ohos/i18n)
add_subdirectory_with_alias_if_exist(base/global/resource_management_lite/frameworks/resmgr_lite ${PROJECT_BINARY_DIR}/ohos/resmgr)
add_subdirectory_with_alias_if_exist(base/location/location_lite ${PROJECT_BINARY_DIR}/ohos/location/location_lite)
add_subdirectory_with_alias_if_exist(drivers/peripheral/location ${PROJECT_BINARY_DIR}/ohos/location_hdi)
add_subdirectory_with_alias_if_exist(base/sensors/sensor_lite ${PROJECT_BINARY_DIR}/ohos/sensors/sensor_lite)
add_subdirectory_with_alias_if_exist(drivers/peripheral/sensor/ ${PROJECT_BINARY_DIR}/ohos/sensor_hdi)

# add_subdirectory_with_alias_if_exist(utils/native/lite/file ${PROJECT_BINARY_DIR}/ohos/file)
# add_subdirectory_with_alias_if_exist(utils/native/lite/kv_store ${PROJECT_BINARY_DIR}/ohos/kv_store)
# add_subdirectory_with_alias_if_exist(utils/native/lite/js/builtin ${PROJECT_BINARY_DIR}/ohos/storage_jsi)

add_subdirectory_with_alias_if_exist(commonlibrary/utils_lite/file ${PROJECT_BINARY_DIR}/ohos/file/file)
add_subdirectory_with_alias_if_exist(commonlibrary/utils_lite/hals/file ${PROJECT_BINARY_DIR}/ohos/file/hal)
add_subdirectory_with_alias_if_exist(commonlibrary/utils_lite/js/builtin/filekit ${PROJECT_BINARY_DIR}/ohos/storage_jsi/filekit)
add_subdirectory_with_alias_if_exist(commonlibrary/utils_lite/js/builtin/common ${PROJECT_BINARY_DIR}/ohos/storage_jsi/common)
add_subdirectory_with_alias_if_exist(commonlibrary/utils_lite/js/builtin/kvstorekit ${PROJECT_BINARY_DIR}/ohos/storage_jsi/kvstorekit)
add_subdirectory_with_alias_if_exist(foundation/distributeddatamgr/kv_store/interfaces/inner_api/kv_store ${PROJECT_BINARY_DIR}/ohos/kv_store)

add_subdirectory_with_alias_if_exist(device/soc/hisilicon/common/hardware/litewearable_base/mem_hal ${PROJECT_BINARY_DIR}/ohos/memory)
add_subdirectory_with_alias_if_exist(base/hiviewdfx/hiview_lite ${PROJECT_BINARY_DIR}/ohos/hiview)
add_subdirectory_with_alias_if_exist(base/hiviewdfx/hievent_lite ${PROJECT_BINARY_DIR}/ohos/hievent)
add_subdirectory_with_alias_if_exist(base/hiviewdfx/hilog_lite/frameworks/mini ${PROJECT_BINARY_DIR}/ohos/hilog)

#add_subdirectory_with_alias_if_exist(third_party/quickjs ${PROJECT_BINARY_DIR}/ohos/quickjs)

add_subdirectory_with_alias_if_exist(test/xts/tools/lite/hctest ${PROJECT_BINARY_DIR}/xts_test/hctest)
add_subdirectory_with_alias_if_exist(test/xts/acts/security_lite/huks/liteos_m_adapter ${PROJECT_BINARY_DIR}/xts_test/huks_adapter)
add_subdirectory_with_alias_if_exist(test/xts/acts/security_lite/huks/common ${PROJECT_BINARY_DIR}/xts_test/huks_common)
add_subdirectory_with_alias_if_exist(test/xts/acts/ability_lite/ability_hal ${PROJECT_BINARY_DIR}/xts_test/ability)
add_subdirectory_with_alias_if_exist(test/xts/acts/appexecfwk_lite/appexecfwk_hal ${PROJECT_BINARY_DIR}/xts_test/appexecfwk)
add_subdirectory_with_alias_if_exist(test/xts/acts/distributeddatamgr_lite/kv_store_hal ${PROJECT_BINARY_DIR}/xts_test/kv_store)
add_subdirectory_with_alias_if_exist(test/xts/acts/hiviewdfx_lite/hievent_hal ${PROJECT_BINARY_DIR}/xts_test/hievent)
add_subdirectory_with_alias_if_exist(test/xts/acts/hiviewdfx_lite/hilog_hal ${PROJECT_BINARY_DIR}/xts_test/hilog)
add_subdirectory_with_alias_if_exist(test/xts/acts/iothardware_lite/peripheral_hal ${PROJECT_BINARY_DIR}/xts_test/peripheral)
add_subdirectory_with_alias_if_exist(test/xts/acts/distributed_schedule_lite/system_ability_manager_hal ${PROJECT_BINARY_DIR}/xts_test/system_ability_manager)
add_subdirectory_with_alias_if_exist(test/xts/acts/startup_lite/syspara_hal ${PROJECT_BINARY_DIR}/xts_test/syspara)
add_subdirectory_with_alias_if_exist(test/xts/acts/startup_lite/bootstrap_hal ${PROJECT_BINARY_DIR}/xts_test/bootstrap)
add_subdirectory_with_alias_if_exist(test/xts/acts/kernel_lite/kernelcmsis_hal ${PROJECT_BINARY_DIR}/xts_test/kernelcmsis)
add_subdirectory_with_alias_if_exist(test/xts/acts/commonlibrary_lite/file_hal ${PROJECT_BINARY_DIR}/xts_test/file)
add_subdirectory_with_alias_if_exist(test/xts/device_attest_lite/framework ${PROJECT_BINARY_DIR}/xts_test/devattest_sdk)
add_subdirectory_with_alias_if_exist(test/xts/device_attest_lite/services/core ${PROJECT_BINARY_DIR}/xts_test/devattest_core)
add_subdirectory_with_alias_if_exist(base/security/device_auth ${PROJECT_BINARY_DIR}/ohos/deviceauth)
if("ogg" IN_LIST RAM_COMPONENT)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../build/cmake/open_source/libogg.cmake)
endif()