/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: TEE efuse function
 */

#ifdef CONFIG_TEE_EFUSE_SUPPORT
#include "tee_efuse.h"
#include "chips_porting.h"
#include "chips_porting_ioctl.h"
#include "efuse_porting.h"
#include "errcode.h"
#include "hal_efuse.h"
#include "los_base.h"
#include "3322_glb_ctl_m_rb_reg_offset.h"

errcode_t tee_efuse_get_lifecycle(uint8_t *lifecycle)
{
    if (lifecycle == NULL) {
        return ERRCODE_FAIL;
    }
    hal_efuse_funcs_t *hal_funcs = hal_efuse_get_funcs();
    if (hal_funcs->read_byte(EFUSE_LIFECYCLE_ADDR, &lifecycle[0]) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    *lifecycle = (*lifecycle & 0x3c) >> 2; /* 0x3c: bit2 ~ bit5 is lifecycle_sts */
    return ERRCODE_SUCC;
}

#define CHIP_VERSION_LEN 6

errcode_t tee_efuse_get_chip_version(uint8_t *buffer, uint16_t length)
{
    uint16_t chip_name;
    hal_efuse_funcs_t *hal_funcs;
    uint8_t i = 0;
    uint8_t value = 0;

    if (buffer == NULL || length < CHIP_VERSION_LEN) {
        return ERRCODE_FAIL;
    }

    chip_name = *(uint16_t*)(uintptr_t)GLB_CTL_M_RB_GLB_CTL_CHIP_NAME_REG;
    buffer[i++] = ((chip_name & 0xF000) >> 0xC) + '0'; /* 0xF000: bit12 ~ bit15, 0xC: right shift 12 bit */
    buffer[i++] = ((chip_name & 0x0F00) >> 0x8) + '0'; /* 0x0F00: bit8 ~ bit11, 0x8: right shift 8 bit */
    buffer[i++] = ((chip_name & 0x00F0) >> 0x4) + '0'; /* 0x00F0: bit4 ~ bit7, 0x4: right shift 4 bit */
    buffer[i++] = ((chip_name & 0x000F) >> 0x0) + '0'; /* 0x000F: bit0 ~ bit3 */

    hal_funcs = hal_efuse_get_funcs();
    if (hal_funcs->read_byte(EFUSE_LIFECYCLE_ADDR, &value) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    buffer[i++] = ((value & 0x40) >> 6) + '0'; /* 0x40: bit6 is chip_version 0 */
    buffer[i++] = ((value & 0x80) >> 7) + '0'; /* 0x80: bit7 is chip_version 1 */

    return ERRCODE_SUCC;
}

errcode_t tee_efuse_get_die_id(uint8_t *buffer, uint16_t length)
{
    uint32_t data_length = length > EFUSE_DIE_ID_LENGTH ? EFUSE_DIE_ID_LENGTH : length;

    if ((length == 0) || (buffer == NULL)) {
        return ERRCODE_FAIL;
    }
    hal_efuse_funcs_t *hal_funcs = hal_efuse_get_funcs();
    errcode_t ret = hal_funcs->get_die_id(buffer, data_length);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    return ERRCODE_SUCC;
}

void tee_efuse_init(void)
{
    efuse_port_register_hal_funcs();
    hal_efuse_funcs_t *hal_funcs = hal_efuse_get_funcs();
    hal_funcs->init();
}
#endif
