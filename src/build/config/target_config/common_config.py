#!/usr/bin/env python3
# encoding=utf-8
# ============================================================================
# @brief    common config of build system
# Copyright HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
# ============================================================================


component_set = {
    'boot_rom':  ['hw_sec', 'non_os_libc', 'non_os_isr', 'non_os_osal'],
    'libra_public': ['libra_device'],
    'std_common_lib': [
        'app_version', 'build_version', 'testsuite', 'common_headers', 'connectivity',
        'cpu_load', 'cpu_utils', 'irmalloc', 'lib_utils', 'dfx_log', 'dfx_log_reader',
        'dfx_panic', 'dfx_preserve',
    ],
    'ssb_libs': ['codeloader_ssb', 'lib_utils', 'jlink_load', 'algorithm'],
    'boot_common_lib': [
        'bootloader', 'codeloader', 'build_version', 'common_headers', 'app_version',
        'cpu_utils', 'irmalloc', 'dfx_panic', 'dfx_preserve', 'error_code',
    ],
    'sec': ['hal_sec_common', 'hal_sec_sha', 'hal_sec_trng2', 'hal_sectors',
               'sec_common', 'sec_rsa', 'sec_sha', 'sec_trng', 'sec_port', 'sec_rsa_port',],
    'security': ['hal_hash', 'hash', 'hash_port', 'hal_trng', 'trng', 'trng_port', 'hal_pke',
                    'pke', 'pke_port', 'hal_cipher', 'cipher', 'cipher_port', 'hal_km', 'km', 'km_port'],
    'mem': ['memory_core'],
    'mem_monitor': ['mem_monitor', 'hal_mem_monitor', 'mem_monitor_port'],
    'cpu': ['hal_cpu_core', 'reboot', 'hal_reg_config', 'hal_reboot', 'reboot_port'],
    'can': ['can', 'hal_can', 'can_port'],
    'watchdog': ['watchdog', 'hal_watchdog', 'watchdog_port'],
    'reboot_set':['reboot', 'hal_reboot', 'reboot_port'],
    'uart': ['hal_uart', 'uart', 'uart_port'],
    'xip_os': ['hal_xip', 'xip_port'],
    'xip_nonos': ['hal_xip', 'xip_port'],
    'adc': ['hal_adc', 'adc', 'adc_port'],
    'capsense': ['hal_capsense', 'capsense', 'capsense_port'],
    'ipc': ['hal_ipc_v0', 'ipc_v0'],
    'ipc_v1': ['hal_ipc', 'ipc', 'ipc_porting'],
    'spi_os': ['hal_spi', 'spi_cmsis'],
    'spi_nonos': ['hal_spi', 'spi_nonos'],
    'sfc_flash_old': ['sfc_old', 'hal_sfc_old'],
    'sfc_flash': ['sfc', 'hal_sfc'],
    'sfc_flash_v2': ['sfc_v2', 'hal_sfc_v2'],
    'dma': ['hal_dma_v100', 'dma', 'dma_port'],
    'dmav151': ['hal_dma_v151', 'dma', 'dma_port'],
    'dmav120': ['hal_dma_v120', 'dma', 'dma_port'],
    'udma': ['udma', 'hal_udma', 'udma_port'],
    'button': ['button', 'button_port'],
    'epmu': ['epmu_driver', 'epmu_port', 'hal_epmu'],
    'spi': ['spi', 'hal_spi', 'spi_porting'],
    'lpc': ['hal_lpc', 'lpc'],
    'otp': ['hal_otp', 'otp'],
    'i2c': ['hal_i2c', 'i2c', 'i2c_porting'],
    'pwm': ['hal_pwm', 'pwm', 'pwm_port',],
    'pwm_led': ['hal_pwm_led', 'pwm_led', 'pwm_led_port',],
    'mmc': ['hal_mmc', 'mmc'],
    'gpio_v150': ['hal_gpio_v150', 'gpio', 'gpio_port'],
    'gpio_v100': ['hal_gpio_v100', 'gpio', 'gpio_port'],
    'lpb': ['hal_lpb', 'lpb', 'lpb_port'],
    'qspi': ['hal_qspi', 'qspi'],
    'hso_log': ['dfx_log', 'dfx_log_reader'],
    'qdec' : ['hal_qdec', 'qdec', 'qdec_port'],
    'keyscan' : ['hal_keyscan', 'keyscan', 'keyscan_port'],
    'usb' : ['usb', 'hal_usb', 'usb_port'],
    'usb_unified' : ['usb_unified', 'usb_unified_open', 'usb_unified_port'],
    'pdm' : ['pdm', 'hal_pdm', 'pdm_porting', 'test_pdm'],
    'embed_flash' : ['eflash', 'hal_eflash', 'eflash_port'],
    'i2s' : ['i2s'],
    'flash' : ['flash', 'flash_porting'],
    'sio_v150' : ['hal_sio', 'sio_port'],
    'sio_v151' : ['hal_sio', 'sio_port'],
    'cpu_trace': ['hal_cpu_trace', 'cpu_trace'],
    'cpu_trace_v2': ['hal_cpu_trace_v100', 'cpu_trace_v2', 'cpu_trace_port'],
    'sdio_slave': ['sdio', 'hal_sdio', 'sdio_port'],
    'time_set': ['systick', 'tcxo', 'drv_timer', 'hal_systick', 'hal_tcxo', 'hal_timer', 'rtc_unified', 'hal_rtc_unified', 'rtc_unified_port', 'timer_port', 'systick_port', 'tcxo_port'],
    'systick_set':['systick', 'hal_systick', 'systick_port'],
    'tcxo_set':['tcxo', 'hal_tcxo', 'tcxo_port'],
    'rtc_unified': ['rtc_unified', 'hal_rtc_unified', 'rtc_unified_port'],
    'crc_hw': ['crc_hw', 'hal_crc', 'crc_port'],
    'local_stdlib': ['c', 'm', 'posix'],
    'audio': ['audio_api', 'audio_core', 'audio_drv', 'audio_sample', 'audio_vendor'],
    'bgh': ['bt_host_brandy_service', 'bt_host_brandy_sample', 'bt_host_sdk'],
    'bgh-master': ['bt_app', 'bth_sample', 'bg_common', 'bth_audio_manager_wrapper'],
    'bgh_audio': ['bt_host_audio_manager_wrapper'],
    'media': ['audio_capture', 'audio_manager', 'audio_service', 'audio_stream', 'audio_utils',
              'media_sample_wrapper', 'player', 'media_common', 'audio_bluetooth_port', 'audio_hw',
              'audio_primary_port','format_hw', 'hmf_demuxer', 'media_hal_common', 'plugin_demuxer_hmf',
              'plugin_demuxer_mjpeg', 'plugin_protocol_https', 'audio_port_common', 'codec_hw', 'codec_plugin_vdec', 'ogg',
              'camera_codec_hw', 'hdi_camera_intf', 'plugin_demuxer_raw', 'audio_modem_port', 'audio_decrypt', 'asr_engine', 'tts_engine',
              'circle_buffer', 'buffer_raw_demuxer', 'stream_source_common', 'media_chr',
              'a2dp_sink_service', 'audio_manager_c_service', 'audio_player_service', 'video_player_service', 'stream_source_player_service'],
    'media_tx': ['audio_manager', 'audio_service', 'audio_stream', 'audio_utils', 'audio_manager_service',
              'media_common', 'format_hw', 'audio_bluetooth_port', 'audio_hw', 'plugin_muxer_wav', 'dtcf', 'audio_recorder',
              'audio_primary_port', 'media_hal_common', 'audio_port_common', 'media_chr'],
    'media_rx': ['audio_manager', 'audio_service', 'audio_stream', 'audio_utils', 'audio_manager_service',
              'format_hw', 'media_common', 'audio_bluetooth_port', 'audio_hw', 'audio_recorder', 'audio_usb_port',
              'audio_primary_port', 'media_hal_common', 'audio_port_common', 'media_chr'],
    'media_earphone': ['audio_manager', 'audio_service', 'audio_stream', 'audio_utils', 'audio_manager_service',
              'media_common', 'audio_bluetooth_port', 'audio_hw',
              'audio_recorder', 'plugin_demuxer_raw', 'format_hw', 'audio_usb_port',
              'audio_primary_port', 'media_hal_common', 'audio_port_common', 'lite_player', 'haid_manager', 'tws_manager', 'media_chr', 'anc_manager'],
    'media_hearingaid': ['audio_manager', 'audio_service', 'audio_stream', 'audio_utils', 'audio_manager_service',
              'media_common', 'audio_bluetooth_port', 'audio_hw',
              'audio_recorder', 'plugin_demuxer_raw', 'format_hw',
              'audio_primary_port', 'media_hal_common', 'audio_port_common', 'lite_player', 'haid_manager', 'tws_manager', 'media_chr', 'anc_manager'],
    'media_soundbox': ['audio_manager', 'audio_service', 'audio_stream', 'audio_utils', 'audio_manager_service',
              'media_common', 'audio_bluetooth_port', 'audio_hw', 'plugin_demuxer_hmf', 'hmf_demuxer',
              'audio_recorder', 'plugin_demuxer_raw', 'format_hw', 'audio_primary_port', 'media_hal_common', 'audio_port_common', 'lite_player', 'media_chr'],
    'media_socmn2': ['audio_manager', 'audio_service', 'audio_stream', 'audio_utils', 'audio_manager_service',
              'media_common', 'audio_bluetooth_port', 'audio_hw', 'plugin_demuxer_hmf',
              'plugin_demuxer_raw', 'format_hw', 'hmf_demuxer', 'plugin_muxer_wav', 'dtcf', 'audio_recorder',
              'audio_primary_port', 'media_hal_common', 'audio_port_common', 'haid_manager', 'lite_player', 'tws_manager', 'media_chr'],
    'media_melody': ['audio_manager', 'audio_service', 'audio_stream', 'audio_utils',
              'media_common', 'audio_bluetooth_port', 'audio_hw',
              'audio_primary_port', 'media_hal_common', 'audio_port_common', 'haid_manager', 'tone_player', 'tws_manager', 'media_chr'],
    'media_mini': ['audio_capture', 'audio_manager', 'audio_service', 'audio_stream', 'audio_utils',
              'media_sample_wrapper', 'player', 'media_common', 'audio_bluetooth_port', 'audio_hw', 'a2dp_sink_service',
              'audio_primary_port','format_hw', 'hmf_demuxer', 'media_hal_common', 'plugin_demuxer_hmf',
              'audio_port_common', 'plugin_demuxer_raw', 'media_chr', 'audio_manager_c_service'],
    'media_target4': ['audio_manager', 'audio_service', 'audio_stream', 'audio_utils', 'media_common', 'audio_hw',
              'audio_primary_port', 'audio_bluetooth_port', 'audio_modem_port', 'media_hal_common',
              'audio_port_common', 'media_sample_wrapper', 'audio_decrypt', 'media_chr'],
    'media_hmf': ['hmf_demuxer'],
    'gpu': ['gfx_rm', 'gpu'],
    'gpu_test': ['test_gpu'],
    'graphic_uikit': ['cjson_static', 'display_gfx', 'display_gralloc', 'display_layer', 'graphic_input', 'graphic_service',
                'freetype_static', 'graphic_ui', 'graphic_utils', 'qrcodegen', 'icu4c', 'harfbuzz', 'uikit_ext', 'dfx_massdata',
                'graphics_ui_traceui', 'jpeg_static'],
    'graphic_lvgl': ['display_gfx', 'display_gralloc', 'display_layer', 'graphic_input', 'freetype_static',
                'lvgl', 'lvgl_sample', 'lvgl_demos', 'lvgl_proprietary', 'lvgl_service'],
    'graphic_lvgl_mini': ['display_gfx', 'display_gralloc', 'display_layer', 'graphic_input', 'freetype_static',
                'lvgl', 'lvgl_sample', 'lvgl_demos', 'lvgl_proprietary_mini', 'lvgl_service'],
    'graphic_test': ['uikit_test'],
    'socmn2_ui_sample': ['live_mic'],
    'calendar': ['calendar', 'hal_calendar', 'calendar_port'],
    'update_app': ['update_common', 'update_storage', 'update_local', 'update_storage_brandy', 'update_common_brandy', 'lzma_21.07'],
    'update_recovery': ['update_common', 'update_local', 'update_common_brandy', 'lzma_21.07'],
    'pinctrl': ['pinctrl', 'hal_pinctrl', 'port_pinctrl'],
    'pmp_set': ['pmp', 'hal_pmp', 'pmp_port'],
    'sema': ['sema', 'hal_sema', 'port_sema'],
    'mips': ['mips', 'hal_mips'],
    'dfx_set': ['dfx_diag', 'diag_dfx_cmd', 'dfx_panic', 'dfx_preserve', 'dfx_log', 'dfx_log_reader', 'dfx_exception', 'log_file', 'soc_log', 'dfx_massdata', 'dfx_massdata_reader', 'dfx_common'],
    'app_ux': ['ux_audio', 'ux_bt', 'ux_commu', 'ux_sensor', 'ux_spp', 'ux_cmn_header', 'ux_common'],
    'pm_set': ['pm', 'pm_porting', 'pm_sleep', 'pm_sleep_porting', 'pm_veto', 'pm_veto_porting', 'pm_dev', 'pm_dev_porting', 'pm_fsm', 'pm_fsm_porting'],
    'pm_pmu_set': ['pm_pmu', 'pm_pmu_porting'],
    'pm_clock_set': ['pm_clock', 'pm_clock_porting'],
    'ux_manager': ['fsm_box', 'fsm_master', 'fsm_pairing', 'ux_event', 'ux_earphone', 'ux_timer', 'ux_tone_config', 'ux_tone_service', 'ux_led_porting', 'ux_led_config',
                   'ux_led_frame', 'ux_master', 'at_ux_manager', 'at_ux_register', 'ux_voiceassistant', 'ux_feature'],
    'ux_module': ['ux_box', 'ux_button', 'ux_pairing', 'ux_touchpad', 'ux_charge'],
    'ux_framework': ['ux_task', 'ux_event_socmn2', 'fsm_box_socmn2', 'fsm_master_socmn2', 'fsm_pairing_socmn2', 'ux_framework_headers', 'ux_broadcast'],
    '3322_ux': ['ux_hal_memory', 'ux_common_headers', 'ux_task','ux_activity_manager', 'ux_solution_headers', 'ux_system_config', 'ux_property',
                'ux_feature', 'ux_watch_bg_activity', 'ux_bt', 'ux_device_mgr', 'ux_adapter_power', 'ux_power',
                'ux_system_mgr', 'ux_timer_socmn2', 'ux_power_customer', 'ux_bt_hfp', 'ux_watch_bg_bt', 'ux_watch_audio_sco', 'ux_utils',
                'ux_recovery', 'ux_watch_audio_a2dp', 'ux_bt_a2dp', 'ux_bt_avrcp', 'ux_adapter_bg', 'ux_ble','ux_spp', 'ux_bt_pbap'],
    'ux_at_set': ['at_srv', 'ux_cmd', 'ux_3322_at', 'ux_at'],
    'rom_security_unified': ['rom_hal_security_unified', 'rom_drv_security_unified', 'rom_security_unified_port'],
    'flash_security_unified': ['service_security_unified', 'security_unified_header', 'security_unified_port'],
    'security_unified': ['hal_security_unified', 'drv_security_unified', 'security_unified_port', 'security_unified_header'],
    'mbedtls_harden': ['mbedtls', 'mbedtls_port', 'mbedtls_harden'],
    'mbedtls_soft': ['mbedtls', 'mbedtls_port'],
    'mbedtls_v3.6.0_soft': ['mbedtls_v3.6.0', 'mbedtls_port'],
    'mbedtls_v3.6.0_harden': ['mbedtls_v3.6.0', 'mbedtls_port', 'mbedtls_harden_ex'],
    'mbedtls_v3.6.5_soft': ['mbedtls_v3.6.5', 'mbedtls_port'],
    'mbedtls_v3.6.5_harden': ['mbedtls_v3.6.5', 'mbedtls_port', 'mbedtls_harden_ex'],
    'rom_security_verify_boot': ['rom_secure_verify_boot', 'rom_security_verify_boot_port'],
    'security_verify_boot': ['common_boot', 'security_verify_boot_port'],
    'efuse_v151': ['efuse_port', 'hal_efuse_v151', 'efuse'],
    'efuse_v120': ['efuse', 'hal_efuse_v120', 'efuse_porting'],
    'efuse_v100': ['efuse', 'hal_efuse_v100', 'efuse_porting'],
    'efuse_v130': ['efuse', 'hal_efuse_v130', 'efuse_port'],
    'codec_set': ['l2hc_dec_16k', 'l2hc_dec_48k_10ms', 'l2hc_dec_48k_5ms',
                  'opus_dec_half_itcm', 'opus_dec_half_flash', 'opus_dec_all_itcm', 'opus_dec_all_flash',
                  'opus_enc_half_itcm', 'opus_enc_half_flash', 'opus_enc_all_itcm', 'opus_enc_all_flash',
                  'opus_enc_2.5ms_itcm', 'opus_enc_2.5ms_flash', 'l2hc_enc_16k', 'l2hc_enc_48k_10ms','l2hc_enc_48k_5ms',
                  'sbc_dec_half_itcm', 'sbc_dec_half_flash', 'sbc_dec_all_itcm', 'sbc_dec_all_flash',
                  'sbc_enc_half_itcm', 'sbc_enc_half_flash', 'sbc_enc_all_itcm', 'sbc_enc_all_flash',
                  'sbc_enc_all_flash_108k', 'sbc_enc_all_itcm_108k'],
    'ohos_set' : ['bootstrap', 'samgr', 'samgr_adapter', 'samgr_source', 'samgr_broadcast', 'ability', 'abilitymanager', 'want', 'abilityms', 'bundle', 'bundlems', 'bms_sign_mode','zlib_static',
                    'ace_lite', 'ace_lite_adapt', 'ace_common', 'ace_module_manager', 'ace_native_engine',
                    'permission', 'permission_hal', 'appverify', 'appverify_base',
                    'jerryscript','jerry-ext_static', 'jerry-port-default_static', 'baidu_components', 'i18n', 'ace_keyinput', 'ace_vibrator', 'ace_nfc', 'ace_device', 'ace_baidu_map', 'ace_battery', 'ace_network',
                    'resmgr', 'sysparam_simulator', 'syspara', 'param_client_lite', 'utils_kv_store', 'native_file', 'storage_jsapi','static_hal_file','ace_kit_common', 'ace_kit_kvstore',
                    'hiview_lite', 'hievent_lite', 'hilog_lite', 'hal_memory_static', 'mem_util','init_log',  'init_utils', 'png_static', 'js_player', 'js_audio_capturer',
                    'sensor_client', 'sensor_service', 'sensor_hdi', 'sensor_sample', "sensor_hdi_acc_alg_simulator", "sensor_hdi_acc_simulator", "sensor_hdi_barometer", "sensor_hdi_gyroscope",
                    "sensor_hdi_hr_counter", "sensor_hdi_pedometer", "sensor_hdi_wear_detection", 'netstack_http', 'websocket', 'ohos_mqtt',
                    'location_common', 'location_utils', 'locator_sdk', 'location_locator', 'location_gnss', 'location_gnss_hdi', 'location_gnss_utils', 'location_gnss_emulator', 'location_sample',
                    'ace_brightness', 'ace_locale', 'ace_sensor', 'ace_geolocation', 'ace_calendar', 'ace_http', 'ace_websocket', 'ace_bluetooth', 'ace_socket', 'ohos_lwip', 'lwip_ext','ace_ext',
                    'cipher_static', 'ace_kit_cipher_static', 'svr_dynload'],
    'ohos_bike_set' : ['bootstrap', 'samgr', 'samgr_adapter', 'samgr_source', 'samgr_broadcast', 'ability', 'abilitymanager', 'want', 'abilityms', 'bundle', 'bundlems', 'bms_sign_mode','zlib_static',
                    'ace_lite', 'ace_lite_adapt', 'ace_common', 'ace_module_manager', 'ace_native_engine', 'permission', 'permission_hal', 'appverify', 'appverify_base',
                    'jerryscript','jerry-ext_static', 'jerry-port-default_static', 'i18n', 'ace_keyinput', 'ace_vibrator', 'ace_device', 'ace_battery', 'ace_network',
                    'resmgr', 'syspara', 'param_client_lite', 'utils_kv_store', 'native_file', 'storage_jsapi','static_hal_file','ace_kit_common', 'ace_kit_kvstore',
                    'hiview_lite', 'hievent_lite', 'hilog_lite', 'hal_memory_static', 'mem_util','init_log',  'init_utils', 'png_static', 'js_player', 'js_audio_capturer',
                    'ace_brightness', 'ace_locale', 'ace_geolocation', 'ace_calendar',
                    'netstack_http', 'websocket', 'ohos_mqtt','ace_http', 'ace_websocket', 'ace_bluetooth', 'ace_socket', 'ohos_lwip', 'lwip_ext','ace_ext',
                    'cipher_static', 'ace_kit_cipher_static', 'svr_dynload'
                    #, 'ace_baidu_map', 'baidu_components'
                    #,'location_common', 'location_utils', 'locator_sdk', 'location_locator', 'location_gnss', 'location_gnss_hdi', 'location_gnss_utils', 'location_gnss_emulator', 'location_sample'
                    ],
    'png_set' : ['png_static','zlib_static'],
    'wearable_set' : ['nativeabilityfwk', 'nativelauncher', 'ohos_deps', 'ohos_startup', 'uikit_fwk', 'wearable_dial',
                      'msg_center','msg_center_adapt', 'xiaodu_navi', 'xiaodu_voice', 'xiaodu_navi_vector'],
    'map_set' : ['amap', 'amap_os_adapter', 'amap_controller', 'map_lite', 'amap_render_adapter', 'map_fwk'],
    'xts_set' : ['xts_adapt', 'ohos_drv', 'png_static', 'hal_token_static', 'huks_sdk', 'deviceauth'],
    'xts_test_set' : [  "dsoftbus", "hctest",
                        "module_ActsHuksHalFunctionTest", "huks_test_common",
    					"module_ActsAbilityMgrTest", "module_ActsBundleMgrTest",
                        "module_ActsKvStoreTest",
                        "module_ActsUtilsFileTest",
                        "module_ActsHieventLiteTest", "module_ActsDfxFuncTest",
                        "module_ActsWifiIotTest",
                        "module_ActsBootstrapTest",
                        "module_ActsSamgrTest",
                        "module_ActsParameterTest",
                        "module_ActsCMSISTest",
                        "devattest_sdk", "devattest_core",
                        "ohos_adapt"
                    ],
    'ohos_mini_set' : ['bootstrap', 'samgr', 'syspara', 'utils_kv_store', 'native_file', 'ohos_startup', 'sensor_lite', 'sensor_hdi', 'sensor_hdi_acc_simulator', 'sensor_hdi_acc_alg_simulator'],
    'xts_mini_set' : ['xts_adapt', 'ohos_drv',
                      'hal_file', 'hiview_lite', 'hievent_lite', 'hilog_lite', 'huks_sdk', 'deviceauth'],
    'xts_test_mini_set' : ['cjson_static', 'dsoftbus', 'hctest', 'ActsHuksHalFunctionTest', 'huks_test_common', 'utils_lite_header',
                           'ActsKvStoreTest', 'ActsUtilsFileTest', 'ActsHieventLiteTest', 'ActsDfxFuncTest', 'ActsWifiIotTest',
                           'ActsBootstrapTest', 'ActsSamgrTest', 'ActsParameterTest', 'ActsCMSISTest'],
    'alipay_set' : ['alipay_soc', 'alipay_soc_adapt', 'alipay_service', 'alipay_hal', 'hal_csi_rv70_llvm',
                    'app_setting_rv70_llvm', 'app_aid_code_rv70_llvm', 'app_bind_ble_rv70_llvm', 'app_pay_code_rv70_llvm',  'app_trans_code_rv70_llvm',
                    'fwk_bind_status_rv70_llvm', 'fwk_ble_communicate_rv70_llvm', 'fwk_common_rv70_llvm', 'fwk_crypt_rv70_llvm', 'fwk_secure_mode_se_rv70_llvm',
                    'fwk_device_certificate_rv70_llvm', 'fwk_network_rv70_llvm', 'fwk_sm2_rv70_llvm', 'fwk_quickjs_adapt_rv70_llvm', 'fwk_js_api_rv70_llvm',
                    '3rdparty_mbedtls_rv70_llvm', '3rdparty_uEcc_rv70_llvm', '3rdparty_sm3_rv70_llvm', '3rdparty_cJSON_rv70_llvm', '3rdparty_protobuf-c_rv70_llvm',
                    'quickjs', 'app_antchain_rv70_llvm', 'app_provision_rv70_llvm', 'alipay_antchain', 'fwk_storage_rv70_llvm', 'fwk_crypt_ext_rv70_llvm'],
    'alipay_base_set' : ["alipay_soc_adapt", "alipay_service", "app_aid_code_rv70_llvm", "app_pay_code_rv70_llvm", "app_setting_rv70_llvm", "app_trans_code_rv70_llvm",
                          "fwk_bind_status_rv70_llvm", "fwk_ble_communicate_rv70_llvm", "fwk_common_rv70_llvm", "fwk_crypt_rv70_llvm", "fwk_crypt_ext_rv70_llvm",
                           "fwk_device_certificate_rv70_llvm", "fwk_js_api_rv70_llvm", "fwk_network_rv70_llvm", "fwk_quickjs_adapt_rv70_llvm", "fwk_secure_mode_se_rv70_llvm", "fwk_sm2_rv70_llvm", "fwk_storage_rv70_llvm",
                           "3rdparty_mbedtls_rv70_llvm", "3rdparty_uEcc_rv70_llvm", "3rdparty_sm3_rv70_llvm", "3rdparty_cJSON_rv70_llvm", "3rdparty_protobuf-c_rv70_llvm"],
    'alipay_sec_set' : ["alipay_soc", "alipay_hal", "hal_csi_rv70_llvm", "alipay_antchain", "quickjs", "app_antchain_rv70_llvm", "app_provision_rv70_llvm"],
    'alipay_hsc32i1_set' : ["alipay_hal_os", "alipay_hal_device_info", "alipay_hal_bluetooth", "alipay_hal_network", "alipay_hal_file", "alipay_hal_se_v1",
                        "app_nfc_offline_pay_rv70_llvm", "app_pay_result_rv70_llvm", "app_scan_pay_rv70_llvm",
                        "3rdparty_bitmap_rv70_llvm", "3rdparty_quickjs_rv70_llvm", "3rdparty_regex_rv70_llvm", "share_common_rv70_llvm"],
    'alipay_ble_set' : ["app_bind_ble_rv70_llvm"],
    'alipay_net_set' : ["app_bind_net_rv70_llvm"],
    'msg_center_service' : ['xiaodu_navi', 'xiaodu_voice', 'xiaodu_navi_vector', 'hc_demo_msg','app_store_msg',
                            'device_msg', 'remote_msg', 'auto_ota_msg'],
    'wifi_set': ['wpa_supplicant',
            'wifi_drv_ws53', 'wifi_service', 'wifi_driver_tcm', 'wifi_hal_tcm', 'wifi_hal_ws53',
            'mac_addr_ws53', 'tsensor_port', 'pm_veto_porting', 'version_port',
            'ws53_host', 'osal_adapt', 'wifi_fe', 'hcc_host', 'app_init',
            'wifi_alg_txbf', 'wifi_alg_temp_protect', 'wifi_alg_anti_interference',
            'wifi_alg_edca_opt', 'wifi_alg_cca_opt',
            'wifi_csa', 'wifi_frag', 'wifi_mbo', 'wifi_bsrp_nfrp', 'wifi_slp',
            'wifi_apf', 'wifi_11k', 'wifi_11v', 'wifi_auto_adjust_freq', 'wifi_11r',
            'wifi_repeater', 'wifi_csi', 'wifi_m2u', 'wifi_wps', 'wifi_blacklist',
            'wifi_sdp', 'wifi_latency', 'wifi_tx_amsdu', 'wifi_uapsd_sta',
            'wifi_roam', 'wifi_btcoex', 'wifi_uapsd_ap', 'wifi_twt',
            'wifi_dnb', 'wifi_sr', 'wifi_pk_txrx',],
    'wifi_set_pure': ['wpa_supplicant2.11',
            'wifi_drv_ws53', 'wifi_service', 'wifi_driver_tcm', 'wifi_hal_tcm', 'wifi_hal_ws53',
            'mac_addr_ws53', 'tsensor_port', 'pm_veto_porting', 'version_port',
            'ws53_host', 'osal_adapt', 'wifi_fe', 'hcc_host', 'app_init',
            'wifi_alg_txbf', 'wifi_alg_temp_protect', 'wifi_alg_anti_interference',
            'wifi_alg_edca_opt', 'wifi_alg_cca_opt',
            'wifi_csa', 'wifi_frag', 'wifi_mbo', 'wifi_bsrp_nfrp', 'wifi_slp',
            'wifi_apf', 'wifi_11k', 'wifi_11v', 'wifi_auto_adjust_freq', 'wifi_11r',
            'wifi_repeater', 'wifi_csi', 'wifi_m2u', 'wifi_wps', 'wifi_blacklist',
            'wifi_sdp', 'wifi_latency', 'wifi_tx_amsdu', 'wifi_uapsd_sta',
            'wifi_roam', 'wifi_btcoex', 'wifi_uapsd_ap', 'wifi_twt',
            'wifi_dnb', 'wifi_sr', 'wifi_pk_txrx',],
    'nv_encrypt' : ['drv_hitls', 'security_unified_header'],
    'gnss_set': ['gnss_header', ],
    'gnss_agnss_set': ['supl_113x_ohos', 'hiasn1'],
    'tiot_set': ['tiot_driver'],
    'volte_set' : ['ims'],
    'nfc_set' : ['nfc_middleware_src', 'nfc_header', 'samples'],
}

defines_set = {
    'libsec_defines': [
        'SECUREC_HAVE_WCTOMB=0', 'SECUREC_HAVE_MBTOWC=0',
        'SECUREC_ENABLE_SCANF_FLOAT=0', 'SECUREC_ENABLE_SPRINTF_FLOAT=0'],
    'chip_defines': [
        'BS25_CHIP_FPGA=0', 'BS25_CHIP_V100=0', 'BRANDY_CHIP_FPGA=0', 'BRANDY_CHIP_V100=0', 'WUXING_CHIP_FPGA=0', 'WUXING_CHIP_V100=0', 'SOCMN1_CHIP_FPGA=0',
        'SOCMN1_CHIP_V100=0', 'SOCMN1_CHIP_V200=0', 'LIBRA_CHIP_CS=0', 'LIBRA_CHIP_FPGA=0', 'SW39_CHIP_FPGA=0', 'SOCMN2_CHIP_FPGA=0',
        'TARGET_CHIP_BS25=0', 'TARGET_CHIP_BRANDY=0', 'TARGET_CHIP_WUXING=0', 'TARGET_CHIP_SOCMN1=0', 'TARGET_CHIP_SW39=0', 'TARGET_CHIP_SOCMN2=0',
        'TARGET_CHIP_LIBRA=0', 'CHIP=1', 'CHIP_VERSION=1', 'CHIP_WS63=0', 'SW21_CHIP_FPGA=0',
        'TARGET_CHIP_SW21=0', 'CHIP_WS53=0', 'CONFIG_UART_SUPPORT_TX', 'CONFIG_UART_SUPPORT_RX',
        'CHIP_BS20=0', 'CHIP_BS21=0', 'CHIP_BS21A=0', 'CHIP_BS21E=0', 'CHIP_BS22=0', 'CHIP_BS26=0', 'CHIP_BS27A=0',
        'SW39_CHIP_V100=0', 'HI3322_CHIP_FPGA=0', 'TARGET_CHIP_3322=0', 'SW22_CHIP_FPGA=1', 'TARGET_CHIP_SW22=0', 'TARGET_CHIP_1A72=0','TARGET_CHIP_1A71E=0'
        ],
    'version_defines': [],
    'libepmu_defines':['EPMU_BUCK_LDO_EN', 'EPMU_RTC_EN', 'EPMU_PWR_KEY_EN', 'EPMU_GPIO_EN', 'EPMU_WDT_EN', 'EPMU_EXTEND_EN'],
    'libepmu_defines_new':['CONFIG_EPMU_BUCK_LDO_EN', 'CONFIG_EPMU_RTC_EN', 'CONFIG_EPMU_PWR_KEY_EN',
        'CONFIG_EPMU_GPIO_EN', 'CONFIG_EPMU_WDT_EN', 'CONFIG_EPMU_EXTEND_EN'],
}

common_ccflags = [
    '-std=gnu99',
    '-Wall',
    '-Werror',
    '-Wextra',
    '-Winit-self',
    '-Wpointer-arith',
    '-Wstrict-prototypes',
    '-Wno-type-limits',
    '-fno-strict-aliasing',
    '-Os',
    '-fno-unwind-tables',
]

common_linkflags = [
    '-Wl,--gc-section',
    '-nostdlib',
    '-static',
]

arch_config = {
    'riscv70': {
        'ccflags': [
            '--target=riscv32',
            '-march=rv32imfcxlinxma_xlinxmb_xlinxmc',
            '-mabi=ilp32f',
            '-ffunction-sections',
            '-fdata-sections',
            '-fno-common',
            '-fsigned-char',
            '-fstack-protector-strong',
            '-g',
            '-Wno-unused-command-line-argument', # 关闭未使用的命令行参数的警告,无用宏定义多需屏蔽
        ],
        'linkflags': [
            '-Wl,--gc-section,--cref,-M',
            '-static',
            '-nostdlib',
            '--target=riscv32',
        ],
        'rom_ccflags': [
            '--target=riscv32',
            '-march=rv32imfcxlinxma_xlinxmb_xlinxmc',
            '-mabi=ilp32f',
            '-ffunction-sections',
            '-fdata-sections',
            '-fno-common',
            '-fsigned-char',
            '-Wno-error=main',
            '-Wno-error=out-of-line-declaration',
            '-Wno-error=uninitialized',
            '-Wno-error=int-conversion',
            '-Wno-error=enum-conversion',
            '-Wno-error=long-long',
            '-Wno-unused-variable',
            '-Wno-error=unused-variable',
            '-Wno-error=unused-value',
            '-Wno-error=unused-function',
            '-Wno-error=sign-compare',
            '-Wno-error=unused-parameter',
            '-Wno-error=tautological-compare',
            '-Wno-error=bad-function-cast',
            '-Wno-error=incompatible-pointer-types',
            '-Wno-error=implicit-function-declaration',
            '-Wno-missing-braces',
            '-Wno-main-return-type',
            '-Wno-unused-command-line-argument',
            '-Wno-undef',
            '-g'
        ],
        'std_libs': [
        ]
    },

    'riscv32': {
        'ccflags': [
            '--target=riscv32',
            '-march=rv32imc_xlinxma_xlinxmb_xlinxmc',
            '-mabi=ilp32',
            '-ffunction-sections',
            '-fdata-sections',
            '-fno-common',
            '-fsigned-char',
            '-fstack-protector-strong',
            '-Wno-error=out-of-line-declaration',
            '-Wno-error=uninitialized',
            '-Wno-error=int-conversion',
            '-Wno-error=enum-conversion',
            '-Wno-error=long-long',
            '-Wno-unused-variable',
            '-Wno-error=unused-variable',
            '-Wno-error=unused-value',
            '-Wno-error=unused-function',
            '-Wno-error=sign-compare',
            '-Wno-error=unused-parameter',
            '-Wno-error=tautological-compare',
            '-Wno-error=bad-function-cast',
            '-Wno-error=incompatible-pointer-types',
            '-Wno-error=implicit-function-declaration',
            '-Wno-missing-braces',
            '-Wno-main-return-type',
            '-Wno-unused-command-line-argument',
            '-Wno-undef',
            '-g',
        ],
        'linkflags': [
            '-Wl,--gc-section,--cref,-M', # 没实际用到的段，不链接
            '-static',
            '-nostdlib', # 不使用标准库
            '--target=riscv32',
            '-mrelax', # 利用链接器的优化特性优化code size
#            '-Wl,--enjal8m', # 大于1M地址长度的函数跳转，使用jal8m指令,有问题
#            '-Wl,--jal-relax', # 当跳转地址为+-2k时，将jar优化为c.jal,有问题
        ],
        'rom_ccflags': [
            '--target=riscv32',
            '-march=rv32imc_xlinxma_xlinxmb_xlinxmc',
            '-mabi=ilp32',
            '-ffunction-sections', # 把函数生成独立的段
            '-fdata-sections', # 把data生成独立的段
            '-fno-common', # 将未初始化的变量放在目标文件的bss段，如果两个编译单元定义了同一个变量，会报错
            '-fsigned-char', # 表示将char解释为signed char
#            # fno-inline # 定义后，使用inline修饰的函数会报编译错误，如定义需将inline 定义为空
            '-pipe', # 优化编译速度，磁盘存储改为内存存储
            '-Werror', # 将告警转为错误
            '-Wmissing-declarations', # 全局函数没有在任何头文件中声明
            '-Wmissing-include-dirs', # 包含的头文件不存在
            '-Wpointer-arith', # 对函数指针或void *指针进行算数操作时，给出告警
            '-Wstrict-prototypes', # 函数定义时，没有为参数指定类型
            '-Wfloat-equal', # 避免浮点数相等比较运算
            '-Wdate-time', # 避免使用时间宏，确保二进制一致性
            '-Wswitch-default', # switch中无default分支会报错
            '-fno-builtin', # 当自定义函数与内建函数冲突时，优先使用用户自定义函数
            '-ffreestanding', # 不检查标准c库和main是否存在
            '-nostdinc', # 不搜索标准系统包含目录
            '-fno-exceptions', # 编译器层面，关闭异常支持(try-catch),优化性能，针对c++
            '-fno-unwind-tables', # 关闭unwind调试信息(c++语言中backtrace功能会使用)
            '-fno-short-enums', # 不使用小枚举
            '-fno-strict-aliasing', # 当代码中存在较多的不同类型指针互转时，使用此选项关闭严格别名优化
            '-finline-hint-functions', # 优先内联加了static的函数，不加static的函数，实际测试不会内联
#            '-Wundef', # liteos 里使用了musl库，需整改后才可打开
#            '-Wno-unused-parameter', # 未使用函数参数告警，如整改太多，可考虑定义
            '-msmall-data-limit=0', # 将小于0字节的全局和静态变量放入特殊的段
            '-g' # 最终rom码点版本需去掉此选项
        ],
        'std_libs': [
        ]
    },
    'cortex_m7':{
        'ccflags': [
            '-fomit-frame-pointer',
            '-ffreestanding',
            '-fdata-sections',
            '-ffunction-sections',
            '-specs=nano.specs',
            '-nostdlib',
            '-pipe',
            '-mno-unaligned-access',
            '-fdebug-types-section',
            '-Wtrampolines',
            '-Wlogical-op',
            '-funsigned-char',
            '-Wformat=2',
            '-Wdate-time',
            #'-Wshadow',
            '-Wno-return-local-addr',
            #'-Wconversion',
            #'-Wcast-align',
            '-Wfloat-equal',
            #'-Wswitch-default',
            '-Wvla',
            '-Wjump-misses-init',
            '-march=armv7-m',
            '-mfpu=fpv5-d16',
            '-mthumb',
            '--specs=nosys.specs',
            '-Wno-incompatible-pointer-types',
            '-Wno-sign-compare',
            '-Wno-error=enum-conversion',

            '-Wno-error=uninitialized',
            '-Wno-error=int-conversion',
            '-Wno-error=long-long',
            '-Wno-unused-variable',
            '-Wno-error=unused-variable',
            '-Wno-error=unused-value',
            '-Wno-error=unused-function',
            '-Wno-error=sign-compare',
            '-Wno-error=unused-parameter',
            '-Wno-error=tautological-compare',
            '-Wno-error=bad-function-cast',
            '-Wno-error=cast-function-type',
            '-Wno-error=discarded-qualifiers',
            '-Wno-error=missing-field-initializers',
            '-Wno-error=incompatible-pointer-types',
            '-Wno-error=implicit-function-declaration',
            '-Wno-missing-braces',
            '-Wno-implicit-function-declaration'
        ],
        'linkflags': [
            '-Wl,--gc-section,--cref,--relax,-M',
            '-mthumb',
            '-flto=16',
            '-march=armv7-m',
            '-nostdlib'
        ],
        'rom_ccflags': [
            '-fomit-frame-pointer',
            '-ffreestanding',
            '-fdata-sections',
            '-ffunction-sections',
            '-specs=nano.specs',
            '-nostdlib',
            '-pipe',
            '-mno-unaligned-access',
            '-mthumb',
            '-fdebug-types-section',
            '-Wtrampolines',
            '-Wlogical-op',
            '-Wjump-misses-init',
            '-march=armv7-m',
            '-fno-inline-functions-called-once',
            '-fno-inline-small-functions',
            '--specs=nosys.specs',
        ],
        'std_libs': [
        ]
    },
    'riscv31': {
        'ccflags': [
            '-ffreestanding',
            '-fdata-sections',
            '-Wno-implicit-fallthrough',
            '-ffunction-sections',
            '-nostdlib',
            '-pipe',
            '-mabi=ilp32',
            '-march=rv32imc',
            '-fno-tree-scev-cprop',
            '-fno-common',
            '-mpush-pop',
            '-msmall-data-limit=0',
            '-fno-ipa-ra',
            '-Wtrampolines',
            '-Wlogical-op',
            '-Wjump-misses-init',
            '-Wa,-enable-c-lbu-sb',
            '-Wa,-enable-c-lhu-sh',
            '-fimm-compare',
            '-femit-muliadd',
            '-fmerge-immshf',
            '-femit-uxtb-uxth',
            '-femit-lli',
            '-femit-clz',
            '-fldm-stm-optimize',
            '-g',
        ],
        'linkflags': [
            '-Wl,--enjal16',
            '-nostdlib',
            '-g',
        ],
        'rom_ccflags': [
            '-ffreestanding',
            '-fdata-sections',
            '-Wno-implicit-fallthrough',
            '-ffunction-sections',
            '-nostdlib',
            '-pipe',
            '-mabi=ilp32',
            '-march=rv32imc',
            '-fno-tree-scev-cprop',
            '-fno-common',
            '-mpush-pop',
            '-msmall-data-limit=0',
            '-fno-ipa-ra',
            '-Wtrampolines',
            '-Wlogical-op',
            '-Wjump-misses-init',
            '-Wa,-enable-c-lbu-sb',
            '-Wa,-enable-c-lhu-sh',
            '-fimm-compare',
            '-femit-muliadd',
            '-fmerge-immshf',
            '-femit-uxtb-uxth',
            '-femit-lli',
            '-femit-clz',
            '-fldm-stm-optimize',
            '-g',
        ],
        'std_libs': [
            'gcc', 'm', 'c'
        ]
    },
}

arch_family = {
    'arm': [
        'cortex_m7', 'cortex_m3'
    ],
    'riscv': [
        'riscv70', 'riscv31', 'riscv32'
    ],
}


class CommonConfig:
    def __init__(self, arch):
        self.arch_config = arch_config[arch]
        self.arch_family = ''
        for arch_family_name in arch_family:
            if arch in arch_family[arch_family_name]:
                self.arch_family = arch_family_name
        if not self.arch_family:
            raise Exception(f'{arch} is not found in arch_family, please add it')

    def get_ram_ccflags(self):
        temp = []
        temp.extend(common_ccflags)
        temp.extend(self.arch_config['ccflags'])
        return temp

    def get_rom_ccflags(self):
        return common_ccflags + self.arch_config['rom_ccflags']

    def get_definse(self, set_name):
        try:
            return defines_set[set_name]
        except KeyError:
            print(f'{set_name} is not in defines_set!!')
            raise

    def get_linkflags(self):
        return common_linkflags + self.arch_config['linkflags']

    def get_std_libs(self):
        return self.arch_config['std_libs']

    def get_arch_family(self):
        return self.arch_family

    def get_component_set(self, set_name):
        try:
            return component_set[set_name]
        except KeyError:
            print(f'{set_name} is not in component_set!!')
            raise
