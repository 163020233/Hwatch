#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.

import os

tmp_path = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'tmp')

key_area_stru_ver          = 0x00010000
key_area_stru_len_ecc_sm2  = 0x100
key_area_key_len_ecc_sm2   = 64
key_area_sig_len_ecc_sm2   = 64
key_area_key_alg_ecc       = 0x2A13C812
key_area_ecc_curve         = 0x2A13C812
key_area_maintenance_mode  = 0x2A13C812
key_area_key_owner_id      = 0
key_area_key_id            = 0
key_area_key_version       = 0
key_area_key_version_mask  = 0
key_area_msid              = 0
key_area_msid_mask         = 0

code_info_stru_ver         = 0x00010000
code_info_stru_len_ecc_sm2 = 0x200
code_info_sig_len_ecc_sm2  = 64
code_info_msid             = 0
code_info_msid_mask        = 0
code_info_enc_flag_disable = 0x3C7896E1
code_info_enc_flag_enable  = 0xC387691E
code_info_compress_enable  = 0x3C7896E1
code_info_compress_disable = 0xC387691E

sign_params_root_key = {
    'image_id'     : ['4BA5C31E', '4BA5C31E'],
    'key_name'     : ['wearalbe_secboot_ecc_brainpool_rootkey', 'wearalbe_secboot_ecc_brainpool_rootkey']
}

sign_params_flashboot = {
    'image_id'     : ['4B1E3C1E', '4B1E3C2D'],
    'key_version'  : '00000000',
    'key_ver_mask' : '00000000',
    'version'      : '00000000',
    'version_mask' : '00000000',
    'key_name'     : ['wearalbe_secboot_ecc_brainpool_rootkey', 'wearalbe_secboot_ecc_brainpool_subkey']
}

sign_params_recovery = {
    'image_id'     : ['4B69871E', '4B69872D'],
    'key_version'  : '00000000',
    'key_ver_mask' : '00000000',
    'version'      : '00000000',
    'version_mask' : '00000000',
    'key_name'     : ['wearalbe_secboot_ecc_brainpool_rootkey', 'wearalbe_secboot_ecc_brainpool_subkey']
}

sign_params_provision = {
    'image_id'     : ['4BD2F01E', '4BD2F02D'],
    'key_version'  : '00000000',
    'key_ver_mask' : '00000000',
    'version'      : '00000000',
    'version_mask' : '00000000',
    'key_name'     : ['wearalbe_secboot_ecc_brainpool_rootkey', 'wearalbe_secboot_ecc_brainpool_subkey']
}

sign_params_param_areas = {
    'image_id'     : ['4B87A51E', '4B87A52D'],
    'key_version'  : '00000000',
    'key_ver_mask' : '00000000',
    'version'      : '00000000',
    'version_mask' : '00000000',
    'key_name'     : ['wearalbe_secboot_ecc_brainpool_rootkey', 'wearalbe_secboot_ecc_brainpool_subkey']
}

sign_params_tee_fw = {
    'image_id'     : ['4BE10F1E', '4BE10F2D'],
    'key_version'  : '00000000',
    'key_ver_mask' : '00000000',
    'version'      : '00000000',
    'version_mask' : '00000000',
    'key_name'     : ['wearalbe_secboot_ecc_brainpool_rootkey', 'wearalbe_secboot_ecc_brainpool_subkey']
}

sign_params_app_image = {
    'image_id'     : ['4B0F2D1E', '4B0F2D2D'],
    'key_version'  : '00000000',
    'key_ver_mask' : '00000000',
    'version'      : '00000000',
    'version_mask' : '00000000',
    'key_name'     : ['wearalbe_secboot_ecc_brainpool_rootkey', 'wearalbe_secboot_ecc_brainpool_subkey']
}

sign_params_bt_image = {
    'image_id'     : ['4BF01E1E', '4BF01E2D'],
    'key_version'  : '00000000',
    'key_ver_mask' : '00000000',
    'version'      : '00000000',
    'version_mask' : '00000000',
    'key_name'     : ['wearalbe_secboot_ecc_brainpool_rootkey', 'wearalbe_secboot_ecc_brainpool_subkey']
}

sign_params_wifi0_image = {
    'image_id'     : ['4B2D4B1E', '4B2D4B2D'],
    'key_version'  : '00000000',
    'key_ver_mask' : '00000000',
    'version'      : '00000000',
    'version_mask' : '00000000',
    'key_name'     : ['wearalbe_secboot_ecc_brainpool_rootkey', 'wearalbe_secboot_ecc_brainpool_subkey']
}

sign_params_wifi1_image = {
    'image_id'     : ['5AC3E11E', '5AC3E12D'],
    'key_version'  : '00000000',
    'key_ver_mask' : '00000000',
    'version'      : '00000000',
    'version_mask' : '00000000',
    'key_name'     : ['wearalbe_secboot_ecc_brainpool_rootkey', 'wearalbe_secboot_ecc_brainpool_subkey']
}

sign_params_wifi2_image = {
    'image_id'     : ['5AD2F01E', '5AD2F02D'],
    'key_version'  : '00000000',
    'key_ver_mask' : '00000000',
    'version'      : '00000000',
    'version_mask' : '00000000',
    'key_name'     : ['wearalbe_secboot_ecc_brainpool_rootkey', 'wearalbe_secboot_ecc_brainpool_subkey']
}

sign_params_wifi3_image = {
    'image_id'     : ['5AE10F1E', '5AE10F2D'],
    'key_version'  : '00000000',
    'key_ver_mask' : '00000000',
    'version'      : '00000000',
    'version_mask' : '00000000',
    'key_name'     : ['wearalbe_secboot_ecc_brainpool_rootkey', 'wearalbe_secboot_ecc_brainpool_subkey']
}

sign_params_sensor_image = {
    'image_id'     : ['4BC3E11E', '4BC3E12D'],
    'key_version'  : '00000000',
    'key_ver_mask' : '00000000',
    'version'      : '00000000',
    'version_mask' : '00000000',
    'key_name'     : ['wearalbe_secboot_ecc_brainpool_rootkey', 'wearalbe_secboot_ecc_brainpool_subkey']
}

sign_params_dsp_main = {
    'image_id'     : ['5A87A51E', '5A87A52D'],
    'key_version'  : '00000000',
    'key_ver_mask' : '00000000',
    'version'      : '00000000',
    'version_mask' : '00000000',
    'key_name'     : ['wearalbe_secboot_ecc_brainpool_rootkey', 'wearalbe_secboot_ecc_brainpool_subkey']
}

sign_params_dsp_overlay = {
    'image_id'     : ['5A96B41E', '5A96B42D'],
    'key_version'  : '00000000',
    'key_ver_mask' : '00000000',
    'version'      : '00000000',
    'version_mask' : '00000000',
    'key_name'     : ['wearalbe_secboot_ecc_brainpool_rootkey', 'wearalbe_secboot_ecc_brainpool_subkey']
}

sign_params_npu_image = {
    'image_id'     : ['5AA5C31E', '5AA5C32D'],
    'key_version'  : '00000000',
    'key_ver_mask' : '00000000',
    'version'      : '00000000',
    'version_mask' : '00000000',
    'key_name'     : ['wearalbe_secboot_ecc_brainpool_rootkey', 'wearalbe_secboot_ecc_brainpool_subkey']
}

sign_params_gnss_image = {
    'image_id'     : ['4B3C5A1E', '4B3C5A2D'],
    'key_version'  : '00000000',
    'key_ver_mask' : '00000000',
    'version'      : '00000000',
    'version_mask' : '00000000',
    'key_name'     : ['wearalbe_secboot_ecc_brainpool_rootkey', 'wearalbe_secboot_ecc_brainpool_subkey']
}

sign_params_debug_image = {
    'image_id'     : ['5AB4D21E', '5AB4D22D'],
    'key_version'  : '00000000',
    'key_ver_mask' : '00000000',
    'version'      : '00000000',
    'version_mask' : '00000000',
    'key_name'     : ['wearalbe_secboot_ecc_brainpool_rootkey', 'wearalbe_secboot_ecc_brainpool_devlprkey']
}

sign_params_fota_package = {
    'image_id'     : ['CB8D154E', 'CB8D154E'],
    'key_version'  : '00000000',
    'key_ver_mask' : '00000000',
    'version'      : '00000000',
    'version_mask' : '00000000',
    'key_name'     : ['wearalbe_secboot_ecc_brainpool_rootkey', 'wearalbe_secboot_ecc_brainpool_subkey']
}

sign_params = {
    'root_key'     : sign_params_root_key,
    'flashboot'    : sign_params_flashboot,
    'recovery'     : sign_params_recovery,
    'provision'    : sign_params_provision,
    'param_areas'  : sign_params_param_areas,
    'tee_fw'       : sign_params_tee_fw,
    'app_image'    : sign_params_app_image,
    'bt_image'     : sign_params_bt_image,
    'wifi0_image'  : sign_params_wifi0_image,
    'wifi1_image'  : sign_params_wifi1_image,
    'wifi2_image'  : sign_params_wifi2_image,
    'wifi3_image'  : sign_params_wifi3_image,
    'sensor_image' : sign_params_sensor_image,
    'dsp_main'     : sign_params_dsp_main,
    'dsp_overlay'  : sign_params_dsp_overlay,
    'npu_image'    : sign_params_npu_image,
    'gnss_image'   : sign_params_gnss_image,
    'debug_image'  : sign_params_debug_image,
    'fota_package' : sign_params_fota_package,
}
