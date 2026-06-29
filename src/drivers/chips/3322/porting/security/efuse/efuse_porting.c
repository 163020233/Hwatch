/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides efuse port template \n
 *
 * History: \n
 * 2023-3-4， Create file. \n
 */
#include "hal_efuse_v151.h"
#include "chip_io.h"
#include "securec.h"
#include "hal_efuse.h"
#include "memory_config.h"
#include "otp_map.h"
#include "soc_reg.h"
#include "tcxo.h"
#include "efuse.h"
#if defined(CONFIG_TEE_EFUSE_SUPPORT) && !defined(BUILD_SELITEOS)
#include "ree_tee_comm.h"
#include "chips_porting_ioctl.h"
#include "pm_clocks.h"
#endif
#include "efuse_porting.h"

#define EFUSE_BASE_ADDR              0x57027000

#define HAL_EFUSE0_CTRL_ADDR         (EFUSE_BASE_ADDR + 0x30)
#define HAL_EFUSE1_CTRL_ADDR         (EFUSE_BASE_ADDR + 0x100)

#define HAL_EFUSE0_READ_BASE_ADDR    (EFUSE_BASE_ADDR + 0x800)
#define HAL_EFUSE1_READ_BASE_ADDR    (EFUSE_BASE_ADDR + 0x1000)

#define HAL_EFUSE0_WRITE_BASE_ADDR   (EFUSE_BASE_ADDR + 0x800)
#define HAL_EFUSE1_WRITE_BASE_ADDR   (EFUSE_BASE_ADDR + 0x1000)

#define BIT_SIZE_OF_BYTE 8

uint32_t g_efuse_boot_done_addr = EFUSE_BASE_ADDR + 0x2c;

uint32_t g_efuse_base_addr[EFUSE_REGION_NUM] = { HAL_EFUSE0_CTRL_ADDR, HAL_EFUSE1_CTRL_ADDR };
uint32_t g_efuse_region_read_address[EFUSE_REGION_NUM] = { HAL_EFUSE0_READ_BASE_ADDR, HAL_EFUSE1_READ_BASE_ADDR };
uint32_t g_efuse_region_write_address[EFUSE_REGION_NUM] = { HAL_EFUSE0_WRITE_BASE_ADDR, HAL_EFUSE1_WRITE_BASE_ADDR };

typedef struct {
    uint16_t id_start_bit;
    uint16_t id_size;
    uint8_t attr;
} efuse_stru;

static efuse_stru g_efuse_cfg[EFUSE_IDX_MAX] = {
    { 352,  128, EFUSE_IDX_RW }, /* die id */
    { 504,  8,   EFUSE_IDX_RW }, /* chip id */
    { 1728, 256, EFUSE_IDX_RW }, /* oem hash root public key */
    { 1664, 32,  EFUSE_IDX_RW }, /* msid */
    { 1644, 1,   EFUSE_IDX_RW }, /* oem_hash_root_public_key lock */
    { 1728, 256, EFUSE_IDX_RW }, /* tee hash root public key, reuse oem */
    { 1644, 1,   EFUSE_IDX_RW }, /* tee hash root public key lock, reuse oem */
    { 1652, 8,   EFUSE_IDX_RW }, /* tee boot ver id, reuse flash boot ver id */
    { 2240, 8,   EFUSE_IDX_RW }, /* tee ver id */
    { 1634, 4,   EFUSE_IDX_RW }, /* chip lifecycle status */
    { 715,  4,   EFUSE_IDX_RW }, /* sec verify enable */
    { 1248, 128, EFUSE_IDX_RW }, /* sec mrk oem */
    { 1645, 1,   EFUSE_IDX_RW }, /* sec mrk oem lock */
    { 159,  16,  EFUSE_IDX_RW }, /* xotrim0 value */
};

void efuse_port_power_on(bool power_on)
{
    // only 0p9v can write efuse
    unused(power_on);
    reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_72_REG, LP_CTL_GLB_RB_D_PMU_SW1P8_2_EN_OFFSET);
    uapi_tcxo_delay_us(200UL);
}

#if defined(CONFIG_TEE_EFUSE_SUPPORT) && !defined(BUILD_SELITEOS)
static errcode_t tesvec_efuse_get_die_id(uint8_t *buffer, uint16_t length)
{
    efuse_get_die_id_arg_t local_arg = {.buffer = buffer, .length = length};

    return ree_tee_comm_unify_entry(CMD_EFUSE_GET_DIE_ID, &local_arg);
}

static errcode_t tesvec_efuse_read_byte(uint32_t byte_address, uint8_t *value)
{
    efuse_read_byte_arg_t local_arg = {.byte_address = byte_address, .value = *value};

    errcode_t ret = ree_tee_comm_unify_entry(CMD_EFUSE_READ_BYTE, &local_arg);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    *value = local_arg.value;
    return ret;
}

static errcode_t tesvec_efuse_write_op(uint32_t byte_address, uint8_t value, uint32_t region)
{
    efuse_write_op_arg_t local_arg = {.byte_address = byte_address, .value = value, .region = region};

    uapi_clocks_config_mode(PM_ID_EFUSE, PM_MODE_PERFORMACE);
    errcode_t ret = ree_tee_comm_unify_entry(CMD_EFUSE_WRITE_OP, &local_arg);
    uapi_clocks_config_mode(PM_ID_EFUSE, PM_MODE_NORMAL);
    return ret;
}

static errcode_t tesvec_efuse_write_buffer_op(uint32_t address, const uint8_t *buffer, uint16_t length)
{
    efuse_write_buffer_op_arg_t local_arg = {.address = address, .buffer = (uint8_t *)buffer, .length = length};

    uapi_clocks_config_mode(PM_ID_EFUSE, PM_MODE_PERFORMACE);
    errcode_t ret = ree_tee_comm_unify_entry(CMD_EFUSE_WRITE_BUFFER_OP, &local_arg);
    uapi_clocks_config_mode(PM_ID_EFUSE, PM_MODE_NORMAL);
    return ret;
}

static errcode_t tesvec_efuse_init(void)
{
    return ERRCODE_SUCC;
}

hal_efuse_funcs_t g_hal_tee_efuse_funcs = {
    .init = tesvec_efuse_init,
    .deinit = NULL,
    .flush_write = NULL,
    .refresh_read = NULL,
    .read_byte = tesvec_efuse_read_byte,
    .write_byte = NULL,
    .clear = NULL,
    .write_op = tesvec_efuse_write_op,
    .write_buffer_op = tesvec_efuse_write_buffer_op,
    .get_die_id = tesvec_efuse_get_die_id,
    .get_chip_id = NULL
};

hal_efuse_funcs_t *hal_tee_efuse_funcs_get(void)
{
    return &g_hal_tee_efuse_funcs;
}

void efuse_port_register_hal_funcs(void)
{
    hal_efuse_register_funcs(hal_tee_efuse_funcs_get());
}
#else
void efuse_port_register_hal_funcs(void)
{
    hal_efuse_register_funcs(hal_efuse_funcs_get());
}
#endif

void efuse_port_unregister_hal_funcs(void)
{
    hal_efuse_unregister_funcs();
}

hal_efuse_region_t hal_efuse_get_region(uint32_t byte_addr)
{
    return (hal_efuse_region_t)(byte_addr / EFUSE_REGION_MAX_BYTES);
}

uint16_t hal_efuse_get_byte_offset(uint32_t byte_addr)
{
    return byte_addr % EFUSE_REGION_MAX_BYTES;
}

uint32_t efuse_read_item(efuse_idx efuse_id, uint8_t *data, uint16_t data_len)
{
    uint16_t item_data_len;
    uint16_t bit_index;
    uint32_t byte_index;
    uint8_t bit_offset;
    uint8_t value;
    uint16_t id_start_bit;
    uint16_t id_size;
    uint16_t result_index;
    uint32_t ret;
    if ((data == NULL) || (efuse_id >= EFUSE_IDX_MAX)) {
        return ERRCODE_FAIL;
    }
    id_start_bit = g_efuse_cfg[efuse_id].id_start_bit;
    id_size = g_efuse_cfg[efuse_id].id_size;
    item_data_len = (id_size + BIT_SIZE_OF_BYTE - 1) / BIT_SIZE_OF_BYTE;
    if (item_data_len > data_len) {
        return ERRCODE_FAIL;
    }
    memset_s(data, data_len, 0, data_len);
    if ((id_size % BIT_SIZE_OF_BYTE) == 0 && (id_start_bit % BIT_SIZE_OF_BYTE) == 0) {
        return uapi_efuse_read_buffer(data, (id_start_bit / BIT_SIZE_OF_BYTE), item_data_len);
    } else {
        for (bit_index = id_start_bit; bit_index < (id_start_bit + id_size); bit_index++) {
            byte_index = bit_index / BIT_SIZE_OF_BYTE;
            bit_offset = bit_index % BIT_SIZE_OF_BYTE;
            ret = uapi_efuse_read_bit(&value, byte_index, bit_offset);
            if (ret != ERRCODE_SUCC) {
                return ret;
            }
            if (value != 0) {
                result_index = (bit_index - id_start_bit) / BIT_SIZE_OF_BYTE;
                data[result_index] = data[result_index] |
                    (1 << ((uint16_t)((bit_index - id_start_bit) % BIT_SIZE_OF_BYTE)));
            }
        }
    }
    return ERRCODE_SUCC;
}

errcode_t check_verify_enable(uint32_t image_owner)
{
    unused(image_owner);

    uint8_t verify_enable;
    uint8_t life_cycle;

    uint32_t ret = efuse_read_item(EFUSE_SEC_VERIFY_ENABLE, &verify_enable, (uint16_t)sizeof(verify_enable));
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    ret = efuse_read_item(EFUSE_CHIP_LIFECYCLE_STS, &life_cycle, (uint16_t)sizeof(life_cycle));
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    if (((verify_enable == 0x0) && (life_cycle < 0x2)) || (verify_enable == 0xA)) { // 空白状态
        return ERRCODE_NOT_SUPPORT;
    }

    return ERRCODE_SUCC;
}

#ifdef CONFIG_EFUSE_READ_TO_RAM
#if defined(CONFIG_TEE_EFUSE_SUPPORT) && defined(BUILD_SELITEOS)
uint8_t g_efuse_save_buffer[EFUSE_MAX_BYTES] = {0};
uint8_t *efuse_porting_get_save_addr(uint32_t byte_addr)
{
    hal_efuse_region_t region = hal_efuse_get_region(byte_addr);
    if (region == HAL_EFUSE_REGION_0) {
        return g_efuse_save_buffer;
    }
    return (uint8_t *)(g_efuse_save_buffer + EFUSE_REGION_MAX_BYTES);
}
#else
uint8_t *efuse_porting_get_save_addr(uint32_t byte_addr)
{
    hal_efuse_region_t region = hal_efuse_get_region(byte_addr);
    if (region == HAL_EFUSE_REGION_0) {
        return (uint8_t *)EFUSE_SAVE_VAL;
    }
    return (uint8_t *)(EFUSE_SAVE_VAL + EFUSE_REGION_MAX_BYTES);
}
#endif
#endif

errcode_t efuse_port_read_buffer_by_reload(uint8_t *buffer, uint32_t byte_number, uint16_t length)
{
    return uapi_efuse_read_buffer(buffer, byte_number, length);
}

errcode_t uapi_efuse_get_chip_version(uint8_t *buffer, uint16_t length)
{
#define CHIP_VERSION_LEN                        6
#define CHIP_VERSION_EFFUSE_0_OFFSET            1638
#define CHIP_VERSION_EFFUSE_1_OFFSET            1639

    if ((length < CHIP_VERSION_LEN) || (buffer == NULL)) {
        return ERRCODE_FAIL;
    }
    uint16_t chip_name = readw(GLB_CTL_M_RB_GLB_CTL_CHIP_NAME_REG);
    uint8_t i = 0;
    buffer[i++] = ((chip_name & 0xF000) >> 0xC) + '0';
    buffer[i++] = ((chip_name & 0x0F00) >> 0x8) + '0';
    buffer[i++] = ((chip_name & 0x00F0) >> 0x4) + '0';
    buffer[i++] = ((chip_name & 0x000F) >> 0x0) + '0';

    uint8_t value = 0;
    uapi_efuse_read_bit(&value, CHIP_VERSION_EFFUSE_0_OFFSET / 0x8, CHIP_VERSION_EFFUSE_0_OFFSET % 0x8);
    buffer[i++] = value + '0';
    uapi_efuse_read_bit(&value, CHIP_VERSION_EFFUSE_1_OFFSET / 0x8, CHIP_VERSION_EFFUSE_1_OFFSET % 0x8);
    buffer[i++] = value + '0';

    return ERRCODE_SUCC;
}