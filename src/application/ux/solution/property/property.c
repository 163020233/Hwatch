/**
  Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
*
* Description: Provide property interface that base on KV \n
*/

#include <string.h>
#include <common_def.h>
#include <nv.h>

#include <property.h>

int32_t property_get_int(uint16_t key, int32_t dval)
{
    uint16_t len = 0;
    int32_t val = 0;
    errcode_t err = uapi_nv_read(key, (uint16_t)sizeof(val), &len, (uint8_t *)&val);
    if (err == ERRCODE_SUCC) {
        return val;
    }
    return dval;
}

errcode_t property_set_int(uint16_t key, int32_t value)
{
    return uapi_nv_write(key, (const uint8_t *)&value, sizeof(value));
}

bool property_get_bool(uint16_t key, bool dval)
{
    uint16_t len = 0;
    uint8_t val = 0;

    errcode_t err = uapi_nv_read(key, (uint16_t)sizeof(val), &len, (uint8_t *)&val);
    if (err == ERRCODE_SUCC) {
        if (val == 0) {
            return false;
        }
        return true;
    }
    return dval;
}

errcode_t property_set_bool(uint16_t key, bool value)
{
    return uapi_nv_write(key, (const uint8_t *)&value, sizeof(value));
}

uint8_t *property_get_bytes(uint16_t key, uint8_t *values, uint16_t vlen)
{
    uint16_t len = 0;
    errcode_t err = uapi_nv_read(key, vlen, &len, values);
    if (err == ERRCODE_SUCC) {
        return values;
    }
    return NULL;
}

errcode_t property_set_bytes(uint16_t key, uint8_t *values, uint16_t vlen)
{
    return uapi_nv_write(key, values, vlen);
}

errcode_t property_set_bytes_with_notify(uint16_t key, uint8_t *values, uint16_t vlen)
{
    /* Caller want to make notify after data change, same with property_set_bytes at this moment. */
    return property_set_bytes(key, values, vlen);
}

errcode_t property_set_encrypted_bytes(uint16_t key, uint8_t *values, uint16_t vlen)
{
    nv_key_attr_t attr = { false, true, true, 0 };
    return uapi_nv_write_with_attr(key, values, vlen, &attr, NULL);
}

errcode_t property_set_non_upgrade_bytes(uint16_t key, uint8_t *values, uint16_t vlen)
{
    nv_key_attr_t attr = { false, false, true, 0 };
    return uapi_nv_write_with_attr(key, values, vlen, &attr, NULL);
}

errcode_t property_set_non_upgrade_int(uint16_t key, int32_t value)
{
    nv_key_attr_t attr = { false, false, true, 0 };
    return uapi_nv_write_with_attr(key, (uint8_t*)&value, sizeof(value), &attr, NULL);
}

errcode_t property_set_non_upgrade_bool(uint16_t key, bool value)
{
    nv_key_attr_t attr = { false, false, true, 0 };
    return uapi_nv_write_with_attr(key, (uint8_t*)&value, sizeof(value), &attr, NULL);
}

uint8_t *property_get_encrypted_bytes(uint16_t key, uint8_t *values, uint16_t vlen)
{
    nv_key_attr_t attr = { false, true, true, 0 };
    uint16_t len = 0;
    errcode_t err = uapi_nv_read_with_attr(key, vlen, &len, values, &attr);
    if (err == ERRCODE_SUCC) {
        return values;
    }
    return NULL;
}

errcode_t property_reset(void)
{
    return ERRCODE_SUCC;
}
