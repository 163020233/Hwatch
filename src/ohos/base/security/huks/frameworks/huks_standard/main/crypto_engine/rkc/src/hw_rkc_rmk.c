/*
 * Copyright (c) 2020-2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef _CUT_AUTHENTICATE_

#include "rkc_rmk.h"
#include "hks_rkc.h"
#include "hks_crypto_hal.h"
#include "hks_get_udid.h"
#include "hks_log.h"
#include "hks_mem.h"
#include "hks_template.h"

// ohos begin
// Obtain the hardware key for key derivation during the upgrade to OH5.0.
#ifdef HKS_HARDWARE_ROOT_KEY
#include "hks_rkc_rw.h"
#include "security_km.h"
#include "security_symc.h"
#include "cipher.h"
#include "trng.h"

#define AES_SALT_LENGTH         28
#define AES_KEY_LENGTH          16
#define INVALID_HANDLE          (0xFFFFFFFF)

static int32_t RkcGetFixedMaterial(struct HksBlob *material)
{
    /* consistent with the old hks */
    const uint8_t fixedMaterial[HKS_RKC_MATERIAL_LEN] = {
        0xB2, 0xA1, 0x0C, 0x73, 0x52, 0x73, 0x76, 0xA1,
        0x60, 0x62, 0x2E, 0x08, 0x52, 0x08, 0x2E, 0xA9,
        0x60, 0xBC, 0x2E, 0x73, 0x52, 0x0B, 0x0C, 0xBC,
        0xEE, 0x0A, 0x2E, 0x08, 0x52, 0x9C, 0x76, 0xA9
    };
 
    if (memcpy_s(material->data, material->size, fixedMaterial, HKS_RKC_MATERIAL_LEN) != EOK) {
        HKS_LOG_E("Memcpy fiexd material failed!");
        return HKS_ERROR_INSUFFICIENT_MEMORY;
    }
 
    return HKS_SUCCESS;
}

static errcode_t create_keyslot(uint32_t *keyslot_handle, uint8_t *salt)
{
    errcode_t ret = ERRCODE_FAIL;

    ret = uapi_drv_keyslot_setup(keyslot_handle, UAPI_DRV_KEYSLOT_TYPE_MCIPHER);
    HKS_IF_NOT_SUCC_LOGE_RETURN(ret, ret, "keyslot setup failed! ret = 0x%" LOG_PUBLIC "X", ret)

    ret = uapi_drv_keyslot_set_hard_key(*keyslot_handle, UAPI_DRV_KEYSLOT_HARD_KEY_TYPE_USER_REE,
        UAPI_DRV_KLAD_ENGINE_AES, salt, AES_SALT_LENGTH, 16);  // 16: 128-bit
    if (ret != ERRCODE_SUCC) {
        HKS_LOG_E("send hard key failed!\n");
        goto error_keyslot_teardown;
    }

    return ret;
error_keyslot_teardown:
    (void)uapi_drv_keyslot_teardown(*keyslot_handle);
    return ret;
}
 
static int32_t hks_aes_cipher_process(const struct HksKsfDataRkc *ksfData, const struct HksKsfDataMk *ksfDataMk,
    uint8_t *src, uint8_t *dst, uint32_t len)
{
    errcode_t ret = ERRCODE_SUCC;
    uint32_t keyslot = INVALID_HANDLE;
    uint8_t iv[HKS_RKC_MK_IV_LEN] = {0};
 
    ret = create_keyslot(&keyslot, ksfData->rmkSalt);
    HKS_IF_NOT_SUCC_LOGE_RETURN(ret, ret, "create_keyslot failed! ret = 0x%" LOG_PUBLIC "X", ret)

    if (memcpy_s(iv, HKS_RKC_MK_IV_LEN, ksfDataMk->mkIv, HKS_RKC_MK_IV_LEN) != EOK) {
        HKS_LOG_E("memcpy_s failed!\n");
        return HKS_ERROR_BAD_STATE;
    }
 
    ret = uapi_drv_cipher_symc_crypt(UAPI_DRV_SYMC_AES_CBC_ALG, src, dst, len,
        iv, NULL, AES_KEY_LENGTH, keyslot, true);
    return ret;
}
#endif

int32_t RkcGetRmkRawKey(const struct HksKsfDataRkc *ksfDataRkc, const struct HksKsfDataMk *ksfDataMk,
    struct HksBlob *rawKey)
{
    uint8_t material3Data[HKS_RKC_MATERIAL_LEN] = {0};
    struct HksBlob material3 = { HKS_RKC_MATERIAL_LEN, material3Data };
 
    /* Get the fixed material */
    int32_t ret = RkcGetFixedMaterial(&material3);
    HKS_IF_NOT_SUCC_LOGE_RETURN(ret, ret, "Get fixed material failed! ret = 0x%" LOG_PUBLIC "X", ret)
 
    /* materials xor */
    for (uint32_t i = 0; i < HKS_RKC_MATERIAL_LEN; ++i) {
        rawKey->data[i] = ksfDataRkc->rkMaterial1[i] ^ ksfDataRkc->rkMaterial2[i] ^ material3.data[i];
    }
 
    /* append hardware UDID */
    ret = HksGetHardwareUdid(rawKey->data + HKS_RKC_MATERIAL_LEN, HKS_HARDWARE_UDID_LEN);
    HKS_IF_NOT_SUCC_LOGE_RETURN(ret, ret, "Get hardware udid failed! ret = 0x%" LOG_PUBLIC "X", ret)
 
    ret = hks_aes_cipher_process(ksfDataRkc, ksfDataMk, rawKey->data, rawKey->data, rawKey->size);
    HKS_IF_NOT_SUCC_LOGE_RETURN(ret, ret, "hks_aes_cipher_process failed! ret = 0x%" LOG_PUBLIC "X", ret)

    return HKS_SUCCESS;
}
// ohos end
#endif
