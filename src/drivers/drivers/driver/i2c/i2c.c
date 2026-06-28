/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides i2c driver source \n
 *
 * History: \n
 * 2022-08-15, Create file. \n
 */

#include <stdbool.h>
#include "soc_osal.h"
#include "securec.h"
#include "common_def.h"
#include "tcxo.h"
#include "i2c.h"
#if defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1)
#include "dma.h"
#include "hal_dma.h"
#endif  /* CONFIG_I2C_SUPPORT_DMA */
#if defined(CONFIG_I2C_USE_POLL_IN_EXC_NON_SCHEDULE)
#include "los_sched_pri.h"
#include "los_exc.h"
#endif

#define I2C_DEFAULT_DEV_ADDRESS  0x0
#define I2C_TIMEOUT_TICK_PER_MS ((uint64_t)32000)
#ifndef I2C_TIMEOUT_SEM
#define I2C_TIMEOUT_SEM 10000
#endif

#if defined(CONFIG_I2C_SUPPORT_LPM)
static bool g_i2c_suspend_flag[I2C_BUS_MAX_NUMBER] = { false };
static uint32_t g_i2c_baudrate[I2C_BUS_MAX_NUMBER] = { 0 };
static uint8_t g_i2c_hscode[I2C_BUS_MAX_NUMBER] = { 0 };
#if defined(CONFIG_I2C_SUPPORT_SLAVE) && (CONFIG_I2C_SUPPORT_SLAVE == 1)
static uint16_t g_i2c_addr[I2C_BUS_MAX_NUMBER] = { 0 };
#endif
#endif  /* CONFIG_I2C_SUPPORT_LPM */

#if defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1)
typedef struct i2c_dma_trans_inf {
    bool trans_succ;
    bool read_flag;
    uint8_t read_channel;
    uint8_t write_channel;
    hal_i2c_prepare_config_t pre_cfg;
    uint16_t *data_cmd_buffer;
    uint32_t data_cmd_length;
} i2c_dma_trans_inf_t;
#endif  /* CONFIG_I2C_SUPPORT_DMA */

typedef struct i2c_ctrl_block_info {
    bool init_flag;              /* When the I2C bus is initialized, the flag is true. */
    bool master_flag;            /* when init_flag is True, the flag can be valid. */
    uint8_t hscode;              /* High speeed mode master code. */
    uint8_t operate_type;        /* I2C data operation type. */
    uint16_t addr;               /* I2C slave address, valid for slave mode. */
#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
    i2c_irq_callback_t  callback;
#endif
#if defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1)
    i2c_dma_trans_inf_t *dma_trans;
#endif
#if (defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)) || \
    defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1)
    osal_semaphore sem;
#endif
#if (defined(CONFIG_I2C_SUPPORT_SLAVE_INT) && (CONFIG_I2C_SUPPORT_SLAVE_INT == 1))
    i2c_slave_callback_t slave_cb;
#endif
} i2c_ctrl_block_info_t;

static i2c_ctrl_block_info_t g_i2c_ctrl[I2C_BUS_MAX_NUM];

#if defined(CONFIG_I2C_SUPPORT_CONCURRENCY) && (CONFIG_I2C_SUPPORT_CONCURRENCY == 1)
static osal_mutex g_i2c_mutex[I2C_BUS_NONE];
#endif  /* CONFIG_I2C_SUPPORT_CONCURRENCY */

#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
static hal_i2c_buffer_wrap_t g_i2c_buffer_wrap[I2C_BUS_MAX_NUM];
#endif  /* CONFIG_I2C_SUPPORT_INT */

#if defined(CONFIG_I2C_SUPPORT_LPM)
__attribute__((weak)) void i2c_port_clock_dynamic_enable(i2c_bus_t bus, bool en)
{
    unused(bus);
    unused(en);
}
#endif

#ifdef CONFIG_I2C_DMA_MEMORY_POOL
#define I2C_DMA_BUF_LENGTH 0x800
uint16_t g_i2c_dma_buffer[I2C_BUS_MAX_NUM][I2C_DMA_BUF_LENGTH];
void *i2c_port_get_dma_buffer(i2c_bus_t bus)
{
    if (bus >= I2C_BUS_MAX_NUM) {
        return NULL;
    }
    return &g_i2c_dma_buffer[bus];
}
#endif

static errcode_t i2c_bus_init_check(i2c_bus_t bus)
{
    if (bus >= I2C_BUS_MAX_NUMBER) {
        return ERRCODE_I2C_INVALID_PARAMETER;
    }

    i2c_ctrl_block_info_t *i2c_ctrl = &g_i2c_ctrl[bus];
    if (i2c_ctrl->init_flag == false) {
        return ERRCODE_I2C_NOT_INIT;
    }
#if defined(CONFIG_I2C_SUPPORT_LPM)
    i2c_port_clock_dynamic_enable(bus, true);
#endif  /* CONFIG_I2C_SUPPORT_LPM */
    return ERRCODE_SUCC;
}

static errcode_t i2c_write_pre_check(i2c_bus_t bus, const i2c_data_t *data)
{
    if ((data == NULL) || (data->send_buf == NULL) || (data->send_len == 0)) {
        return ERRCODE_I2C_SEND_PARAM_INVALID;
    }

    errcode_t ret = i2c_bus_init_check(bus);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    return ERRCODE_SUCC;
}

static errcode_t i2c_buffer_pre_check(i2c_bus_t bus, const hal_i2c_buffer_wrap_t *buffer_wrap)
{
    if ((buffer_wrap == NULL) || (buffer_wrap->buffer == NULL) || (buffer_wrap->len == 0)) {
        return ERRCODE_I2C_SEND_PARAM_INVALID;
    }

    errcode_t ret = i2c_bus_init_check(bus);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    return ERRCODE_SUCC;
}

static errcode_t i2c_read_pre_check(i2c_bus_t bus, const i2c_data_t *data)
{
    if ((data == NULL) || (data->receive_buf == NULL) || (data->receive_len == 0)) {
        return ERRCODE_I2C_RECEIVE_PARAM_INVALID;
    }

    errcode_t ret = i2c_bus_init_check(bus);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    return ERRCODE_SUCC;
}

static inline uint32_t i2c_ctrl_get_mask(hal_i2c_ctrl_id_t ctrl_id)
{
    return (1 << (uint32_t)ctrl_id);
}

static inline bool i2c_ctrl_get_id(uint32_t ctrl_mask, hal_i2c_ctrl_id_t ctrl_id)
{
    return (ctrl_mask >> (uint32_t)ctrl_id) & 1;
}

static errcode_t i2c_comm_ctrl_check(i2c_bus_t bus, hal_i2c_ctrl_id_t id)
{
    uint32_t flag = false;
    errcode_t ret = hal_i2c_ctrl(bus, id, (uintptr_t)&flag);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    if (flag == true) {
        return ERRCODE_SUCC;
    }
    return ERRCODE_I2C_WAIT_CONTINUE;
}

static errcode_t i2c_ctrl_wait(i2c_bus_t bus, uint32_t ctrl_mask, uint32_t timeout)
{
    errcode_t ret;
    uint32_t tmp_ctrl_mask = ctrl_mask;
    uint64_t start_time = uapi_tcxo_get_count();

    do {
        for (uint32_t loop = 0; loop <= (uint32_t)I2C_CTRL_NORMAL_MAX; loop++) {
            if (i2c_ctrl_get_id(tmp_ctrl_mask, loop) == false) {
                continue;
            }
            ret = i2c_comm_ctrl_check(bus, loop);
            if (ret == ERRCODE_SUCC) {
                tmp_ctrl_mask &= ~(1 << loop);
            } else if (ret == ERRCODE_I2C_WAIT_CONTINUE) {
                continue;
            }
        }
        ret = i2c_comm_ctrl_check(bus, I2C_CTRL_CHECK_TRANSMIT_ABRT);
        if (ret == ERRCODE_SUCC) {
            return ERRCODE_I2C_WAIT_EXCEPTION;
        }

        if ((uapi_tcxo_get_count() - start_time) > (timeout * I2C_TIMEOUT_TICK_PER_MS)) {
            return ERRCODE_I2C_TIMEOUT;
        }
    } while (tmp_ctrl_mask != 0);
    return ERRCODE_SUCC;
}

static errcode_t i2c_wait(i2c_bus_t bus, i2c_wait_condition_t *condition, uint32_t timeout)
{
    errcode_t ret = ERRCODE_SUCC;
    i2c_ctrl_block_info_t *i2c_ctrl = &g_i2c_ctrl[bus];

    if (i2c_ctrl->operate_type == I2C_DATA_OPERATION_TYPE_POLL) {
        ret = i2c_ctrl_wait(bus, condition->ctrl_mask, timeout);
    }
    return ret;
}

static errcode_t i2c_write(i2c_bus_t bus, hal_i2c_buffer_wrap_t *buffer_wrap)
{
    errcode_t ret = ERRCODE_SUCC;
    unsigned int irq_sts = i2c_porting_lock(bus);

    uint64_t start_time = uapi_tcxo_get_count();
    while (buffer_wrap->len != 0) {
        ret = hal_i2c_write(bus, buffer_wrap);
        if (ret != ERRCODE_SUCC) {
            goto end;
        }
        uint64_t time = uapi_tcxo_get_count() - start_time;
        if (time > CONFIG_I2C_WAIT_CONDITION_TIMEOUT * I2C_TIMEOUT_TICK_PER_MS) {
            ret = ERRCODE_I2C_TIMEOUT;
            goto end;
        }
    }

end:
    i2c_porting_unlock(bus, irq_sts);
    return ret;
}

static errcode_t i2c_read(i2c_bus_t bus, hal_i2c_buffer_wrap_t *buffer_wrap)
{
    errcode_t ret = ERRCODE_SUCC;
    unsigned int irq_sts = i2c_porting_lock(bus);

    uint64_t start_time = uapi_tcxo_get_count();
    while (buffer_wrap->len != 0) {
        ret = hal_i2c_ctrl(bus, I2C_CTRL_FLUSH_RX_FIFO, (uintptr_t)buffer_wrap);
        if (ret != ERRCODE_SUCC) {
            goto end;
        }
        ret = hal_i2c_read(bus, buffer_wrap);
        if (ret != ERRCODE_SUCC) {
            goto end;
        }
        uint64_t time = uapi_tcxo_get_count() - start_time;
        if (time >= CONFIG_I2C_WAIT_CONDITION_TIMEOUT * I2C_TIMEOUT_TICK_PER_MS) {
            ret = ERRCODE_I2C_TIMEOUT;
            goto end;
        }
    }

end:
    i2c_porting_unlock(bus, irq_sts);
    return ret;
}

static errcode_t i2c_poll_write(i2c_bus_t bus, uint16_t dev_addr, hal_i2c_buffer_wrap_t *data_cfg, bool end_flag)
{
    hal_i2c_prepare_config_t cfg = { 0 };
    i2c_ctrl_block_info_t *i2c_ctrl = &g_i2c_ctrl[bus];

    cfg.addr = dev_addr;
    cfg.operation_type = I2C_DATA_OPERATION_TYPE_POLL;
    i2c_ctrl->operate_type = cfg.operation_type;

    errcode_t ret = hal_i2c_ctrl(bus, I2C_CTRL_WRITE_PREPARE, (uintptr_t)&cfg);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    ret = i2c_write(bus, data_cfg);
    if (ret != ERRCODE_SUCC) {
        (void)hal_i2c_ctrl(bus, I2C_CTRL_WRITE_RESTORE, (uintptr_t)NULL);
        return ret;
    }

    i2c_wait_condition_t condition = { 0 };
    if (end_flag) {
        condition.ctrl_mask = i2c_ctrl_get_mask(I2C_CTRL_CHECK_TX_PROCESS_DONE);
    }
    ret = i2c_wait(bus, &condition, CONFIG_I2C_WAIT_CONDITION_TIMEOUT);

    (void)hal_i2c_ctrl(bus, I2C_CTRL_WRITE_RESTORE, (uintptr_t)NULL);
    return ret;
}

static errcode_t i2c_poll_read(i2c_bus_t bus, uint16_t dev_addr, hal_i2c_buffer_wrap_t *data_cfg, bool end_flag)
{
    unused(end_flag);
    hal_i2c_prepare_config_t cfg = { 0 };
    i2c_ctrl_block_info_t *i2c_ctrl = &g_i2c_ctrl[bus];

    cfg.addr = dev_addr;
    cfg.operation_type = I2C_DATA_OPERATION_TYPE_POLL;
    i2c_ctrl->operate_type = cfg.operation_type;

    errcode_t ret = hal_i2c_ctrl(bus, I2C_CTRL_READ_PREPARE, (uintptr_t)&cfg);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    ret = i2c_read(bus, data_cfg);
    if (ret != ERRCODE_SUCC) {
        (void)hal_i2c_ctrl(bus, I2C_CTRL_READ_RESTORE, (uintptr_t)NULL);
        return ret;
    }

    i2c_wait_condition_t condition = { 0 };
    condition.ctrl_mask = i2c_ctrl_get_mask(I2C_CTRL_CHECK_RX_PROCESS_DONE);

    ret = i2c_wait(bus, &condition, CONFIG_I2C_WAIT_CONDITION_TIMEOUT);

    (void)hal_i2c_ctrl(bus, I2C_CTRL_READ_RESTORE, (uintptr_t)NULL);
    return ret;
}

#if (defined(CONFIG_I2C_SUPPORT_DMA) || defined(CONFIG_I2C_SUPPORT_INT))
static int i2c_sem_down(osal_semaphore *sem, uint32_t timeout)
{
    unused(sem);
    int osal_value = OSAL_SUCCESS;
#if !defined(CONFIG_I2C_SUPPORT_IN_CHIP_LOOPBACK)
    osal_value = osal_sem_down_timeout(sem, timeout);
#endif
    return osal_value;
}

static void i2c_sem_up(osal_semaphore *sem)
{
    unused(sem);
#if !defined(CONFIG_I2C_SUPPORT_IN_CHIP_LOOPBACK)
    osal_sem_up(sem);
#endif
}
#endif

#if defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1)
static void i2c_dma_transfer_finish(i2c_bus_t bus)
{
    (void)hal_i2c_ctrl(bus, I2C_CTRL_WRITE_RESTORE, (uintptr_t)NULL);
    i2c_sem_up(&g_i2c_ctrl[bus].sem);
}

static void i2c_dma_isr(uint8_t int_type, uint8_t ch, uintptr_t arg)
{
    unused(arg);
    i2c_bus_t bus;
    for (bus = I2C_BUS_0; bus < I2C_BUS_NONE; bus++) {
        /* channel default value is 0, means not used. channel > 0 means used.
           So ch + 1 will not misjudgment with channel value 0. */
        if (!g_i2c_ctrl[bus].dma_trans) {
            continue;
        }
        if (g_i2c_ctrl[bus].dma_trans->read_channel == ch + 1) {
            break;
        }
    }
    if (bus != I2C_BUS_NONE) {
        g_i2c_ctrl[bus].dma_trans->trans_succ = int_type == 0 ? true : false;
        g_i2c_ctrl[bus].dma_trans->read_channel = 0;
        i2c_dma_transfer_finish(bus);
    }
}

static void i2c_write_by_dma_config(i2c_bus_t bus, uint16_t *buffer,
                                    uint32_t length,
                                    dma_ch_user_peripheral_config_t *user_cfg)
{
    uint32_t data_addr = 0;
    hal_i2c_ctrl(bus, I2C_CTRL_GET_DMA_DATA_ADDR, (uintptr_t)(&data_addr));

    user_cfg->src = (uint32_t)(uintptr_t)buffer;
    user_cfg->dest = (uint32_t)data_addr;
    user_cfg->transfer_num = (uint16_t)length;
    user_cfg->src_handshaking = 0;
    user_cfg->trans_type = HAL_DMA_TRANS_MEMORY_TO_PERIPHERAL_DMA;
    user_cfg->trans_dir = HAL_DMA_TRANSFER_DIR_MEM_TO_PERIPHERAL;
    user_cfg->priority = HAL_DMA_CH_PRIORITY_0;
    user_cfg->src_width = HAL_DMA_TRANSFER_WIDTH_16;
    user_cfg->dest_width = HAL_DMA_TRANSFER_WIDTH_16;
    user_cfg->burst_length = HAL_DMA_BURST_TRANSACTION_LENGTH_4;
    user_cfg->src_increment = HAL_DMA_ADDRESS_INC_INCREMENT;
    user_cfg->dest_increment = HAL_DMA_ADDRESS_INC_NO_CHANGE;
    user_cfg->protection = HAL_DMA_PROTECTION_CONTROL_PRIVILEGED;
    user_cfg->dest_handshaking = (uint16_t)i2c_port_get_dma_trans_dest_handshaking(bus);
}

static void i2c_read_by_dma_config(i2c_bus_t bus,
                                   uint8_t *buffer,
                                   uint32_t length,
                                   dma_ch_user_peripheral_config_t *user_cfg)
{
    uint32_t data_addr = 0;
    hal_i2c_ctrl(bus, I2C_CTRL_GET_DMA_DATA_ADDR, (uintptr_t)(&data_addr));

    user_cfg->src = (uint32_t)data_addr;
    user_cfg->dest = (uint32_t)(uintptr_t)buffer;
    user_cfg->transfer_num = (uint16_t)length;
    user_cfg->src_handshaking = (uint16_t)i2c_port_get_dma_trans_src_handshaking(bus);
    user_cfg->trans_type = HAL_DMA_TRANS_PERIPHERAL_TO_MEMORY_DMA;
    user_cfg->trans_dir = HAL_DMA_TRANSFER_DIR_PERIPHERAL_TO_MEM;
    user_cfg->priority = HAL_DMA_CH_PRIORITY_0;
    user_cfg->src_width = HAL_DMA_TRANSFER_WIDTH_8;
    user_cfg->dest_width = HAL_DMA_TRANSFER_WIDTH_8;
    user_cfg->burst_length = HAL_DMA_BURST_TRANSACTION_LENGTH_4;
    user_cfg->src_increment = HAL_DMA_ADDRESS_INC_NO_CHANGE;
    user_cfg->dest_increment = HAL_DMA_ADDRESS_INC_INCREMENT;
    user_cfg->protection = HAL_DMA_PROTECTION_CONTROL_BUFFERABLE;
    user_cfg->dest_handshaking = 0;
}

static errcode_t i2c_start_transmit_dma(i2c_bus_t bus, uint8_t dma_ch)
{
    g_i2c_ctrl[bus].dma_trans->write_channel = dma_ch + 1;
    if (uapi_dma_start_transfer(dma_ch) != ERRCODE_SUCC) {
        g_i2c_ctrl[bus].dma_trans->write_channel = 0;
        return ERRCODE_I2C_DMA_CONFIG_ERROR;
    }
    return ERRCODE_SUCC;
}

static errcode_t i2c_transmit_dma(i2c_bus_t bus, uint16_t *buffer, uint32_t len)
{
    uint8_t dma_ch;
    dma_ch_user_peripheral_config_t user_cfg = {0};

    i2c_write_by_dma_config(bus, buffer, len, &user_cfg);
    if (uapi_dma_configure_peripheral_transfer_single(&user_cfg, &dma_ch,
        NULL, (uintptr_t)NULL) != ERRCODE_SUCC) {
        return ERRCODE_I2C_DMA_CONFIG_ERROR;
    }

    return i2c_start_transmit_dma(bus, dma_ch);
}

static uint16_t* i2c_dma_data_cmd_prepare(i2c_bus_t bus, i2c_data_t *data)
{
#define I2C_READ_CMD 0x100
#define I2C_STOP_CMD 0x200
#define I2C_RESTART_CMD 0x400

    uint32_t w_len = data->send_len;
    uint8_t *w_data = data->send_buf;
    uint32_t r_len = data->receive_len;
    uint32_t len = w_len + r_len;

#ifdef CONFIG_I2C_DMA_MEMORY_POOL
    uint16_t *data_cmd_buffer = (uint16_t*)i2c_port_get_dma_buffer(bus);
#else
    unused(bus);
    uint16_t *data_cmd_buffer = (uint16_t*)osal_kmalloc_align(sizeof(uint16_t) * len, 0, 0x4);
#endif

    if (!data_cmd_buffer) {
        return NULL;
    }
    for (uint32_t i = 0; i != w_len; ++i) {
        data_cmd_buffer[i] = (uint16_t)w_data[i];
    }
    if (w_len != 0) {
        data_cmd_buffer[w_len - 1] |= I2C_STOP_CMD;
    }
    for (uint32_t i = w_len; i != len; ++i) {
        data_cmd_buffer[i] = I2C_READ_CMD;
    }
    data_cmd_buffer[len - 1] |= I2C_STOP_CMD;
    return data_cmd_buffer;
}

static errcode_t i2c_transmit_data_cmd_by_dma(i2c_bus_t bus, i2c_dma_trans_inf_t *dma_trans)
{
    uint32_t ret;
    ret = i2c_transmit_dma(bus, dma_trans->data_cmd_buffer, dma_trans->data_cmd_length);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_I2C_DMA_CONFIG_ERROR;
    }

    return ERRCODE_SUCC;
}

static errcode_t i2c_start_recv_dma(i2c_bus_t bus, uint8_t *buffer, uint32_t length)
{
    uint8_t dma_ch;
    dma_ch_user_peripheral_config_t user_cfg = {0};
    i2c_read_by_dma_config(bus, buffer, length, &user_cfg);

    if (uapi_dma_configure_peripheral_transfer_single(&user_cfg, &dma_ch,
        i2c_dma_isr, (uintptr_t)NULL) != ERRCODE_SUCC) {
        return ERRCODE_I2C_DMA_CONFIG_ERROR;
    }

    g_i2c_ctrl[bus].dma_trans->read_channel = dma_ch + 1;
    if (uapi_dma_start_transfer(dma_ch) != ERRCODE_SUCC) {
        g_i2c_ctrl[bus].dma_trans->read_channel = 0;
        return ERRCODE_I2C_DMA_CONFIG_ERROR;
    }
    return ERRCODE_SUCC;
}

static void i2c_dma_transfer_done(i2c_ctrl_block_info_t *ctrl)
{
    i2c_dma_trans_inf_t *dma_trans = ctrl->dma_trans;
#ifdef CONFIG_I2C_DMA_MEMORY_POOL
    memset_s(dma_trans->data_cmd_buffer, I2C_DMA_BUF_LENGTH*sizeof(uint16_t), 0, I2C_DMA_BUF_LENGTH*sizeof(uint16_t));
#else
    free(dma_trans->data_cmd_buffer);
#endif
    /* dma传输无需手动中止的情形会在对应的中断处理中标记为0 */
    if (dma_trans->read_channel != 0) {
        uapi_dma_end_transfer(dma_trans->read_channel - 1);
        dma_trans->read_channel = 0;
    }
    if (dma_trans->write_channel != 0) {
        uapi_dma_end_transfer(dma_trans->write_channel - 1);
        dma_trans->write_channel = 0;
    }
    if ((dma_trans->read_flag == true) && (dma_trans->trans_succ == true)) {
#ifdef CONFIG_SUPPORT_DATA_CACHE
        osal_flush_cache();
#endif
    }
    ctrl->dma_trans = NULL;
}

static errcode_t i2c_transmit_by_dma(i2c_bus_t bus, uint16_t dev_addr, i2c_data_t *data)
{
    errcode_t ret;
    i2c_dma_trans_inf_t dma_trans = {0};
    g_i2c_ctrl[bus].dma_trans = &dma_trans;
    dma_trans.pre_cfg.addr = dev_addr;
    dma_trans.pre_cfg.operation_type = I2C_DATA_OPERATION_TYPE_DMA;
    dma_trans.data_cmd_length = data->receive_len + data->send_len;
    dma_trans.read_flag = data->receive_len != 0 ? true : false;

    dma_trans.data_cmd_buffer = i2c_dma_data_cmd_prepare(bus, data);
    if (!dma_trans.data_cmd_buffer) {
        return ERRCODE_MALLOC;
    }

    hal_i2c_ctrl_id_t ctrl_id = dma_trans.read_flag ? I2C_CTRL_READ_PREPARE : I2C_CTRL_WRITE_PREPARE;
    ret = hal_i2c_ctrl(bus, ctrl_id, (uintptr_t)&dma_trans.pre_cfg);
    if (ret != ERRCODE_SUCC) {
        goto end;
    }

    if (dma_trans.read_flag) {
        ret = i2c_start_recv_dma(bus, data->receive_buf, data->receive_len);
        if (ret != ERRCODE_SUCC) {
            goto end;
        }
    }

    if (g_i2c_ctrl[bus].master_flag || !dma_trans.read_flag) {
        ret = i2c_transmit_data_cmd_by_dma(bus, &dma_trans);
        if (ret != ERRCODE_SUCC) {
            goto end;
        }
    }

    if (i2c_sem_down(&g_i2c_ctrl[bus].sem, I2C_TIMEOUT_SEM) != OSAL_SUCCESS) {
        (void)hal_i2c_ctrl(bus, I2C_CTRL_WRITE_RESTORE, (uintptr_t)NULL);
        ret = ERRCODE_I2C_DMA_TRANSFER_ERROR;
        goto end;
    }

    if (!dma_trans.trans_succ) {
        ret = ERRCODE_I2C_WAIT_EXCEPTION;
    }
end:
    i2c_dma_transfer_done(&g_i2c_ctrl[bus]);
    return ret;
}
#endif  /* CONFIG_I2C_SUPPORT_DMA */

#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
static void i2c_evt_busy_callback(i2c_bus_t bus)
{
    if (g_i2c_ctrl[bus].callback != NULL) {
        g_i2c_ctrl[bus].callback(bus, I2C_IRQ_EVT_I2C_BUSY);
    }
}

static void i2c_evt_tx_callback(i2c_bus_t bus)
{
#if defined(CONFIG_I2C_SUPPORT_SLAVE_INT) && (CONFIG_I2C_SUPPORT_SLAVE_INT == 1)
    if (g_i2c_ctrl[bus].master_flag == false) {
        uint8_t buffer;
        if (g_i2c_ctrl[bus].slave_cb.on_write != NULL) {
            g_i2c_ctrl[bus].slave_cb.on_write(&buffer, 1);
        }
        hal_i2c_buffer_wrap_t hal_wrap = {
            .buffer = &buffer,
            .len = 1,
        };
        hal_i2c_write(bus, &hal_wrap);
        return;
    }
#endif
    hal_i2c_buffer_wrap_t hal_wrap;
    uint32_t burst_len;

    /* 如果没有要写的数据了，就退出 */
    if (g_i2c_buffer_wrap[bus].len == 0) {
        (void)hal_i2c_ctrl(bus, I2C_CTRL_WRITE_RESTORE, (uintptr_t)NULL);
        if (g_i2c_ctrl[bus].callback != NULL) {
            g_i2c_ctrl[bus].callback(bus, I2C_IRQ_EVT_TX_DONE);
        }
        return;
    }

    hal_i2c_ctrl(bus, I2C_CTRL_GET_WRITE_NUM, (uintptr_t)&burst_len);
    uint32_t remain_len = g_i2c_buffer_wrap[bus].len;

    burst_len = burst_len < remain_len ? burst_len : remain_len;
    hal_wrap.len = burst_len;
    hal_wrap.buffer = g_i2c_buffer_wrap[bus].buffer;
    hal_wrap.restart_flag = g_i2c_buffer_wrap[bus].restart_flag;
    /* 只有在发送到末尾时，才发送停止位 */
    if (burst_len == g_i2c_buffer_wrap[bus].len) {
        hal_wrap.stop_flag = g_i2c_buffer_wrap[bus].stop_flag;
    } else {
        hal_wrap.stop_flag = false;
    }

    g_i2c_buffer_wrap[bus].len -= burst_len;
    g_i2c_buffer_wrap[bus].buffer += burst_len;

    hal_i2c_write(bus, &hal_wrap);
}

static void i2c_evt_rx_callback(i2c_bus_t bus)
{
#if defined(CONFIG_I2C_SUPPORT_SLAVE_INT) && (CONFIG_I2C_SUPPORT_SLAVE_INT == 1)
    if (g_i2c_ctrl[bus].master_flag == false) {
        uint8_t buffer[1];
        hal_i2c_buffer_wrap_t buffer_wrap = {
            .buffer = buffer,
            .len = 0,
        };
        buffer_wrap.len = 1;
        buffer_wrap.buffer = buffer;
        (void)hal_i2c_read(bus, &buffer_wrap);
        if (buffer_wrap.len == 0) {
            if (g_i2c_ctrl[bus].slave_cb.on_read != NULL) {
                g_i2c_ctrl[bus].slave_cb.on_read(buffer, 1);
            }
        }
        return;
    }
#endif
    hal_i2c_read(bus, &g_i2c_buffer_wrap[bus]);
    if (g_i2c_buffer_wrap[bus].len == 0) {
        hal_i2c_ctrl(bus, I2C_CTRL_READ_RESTORE, (uintptr_t)NULL);
        i2c_sem_up(&g_i2c_ctrl[bus].sem);
        if (g_i2c_ctrl[bus].callback != NULL) {
            g_i2c_ctrl[bus].callback(bus, I2C_IRQ_EVT_RX_DONE);
        }
        return;
    }

    hal_i2c_ctrl(bus, I2C_CTRL_FLUSH_RX_FIFO, (uintptr_t)&g_i2c_buffer_wrap[bus]);
}

static errcode_t i2c_write_int(i2c_bus_t bus, uint16_t dev_addr, hal_i2c_buffer_wrap_t *data_cfg)
{
    hal_i2c_prepare_config_t cfg = { 0 };
    i2c_ctrl_block_info_t *i2c_ctrl = &g_i2c_ctrl[bus];

    cfg.addr = dev_addr;
    cfg.operation_type = I2C_DATA_OPERATION_TYPE_INT;
    cfg.total_len = data_cfg->len;
    i2c_ctrl->operate_type = cfg.operation_type;

    g_i2c_buffer_wrap[bus].buffer = data_cfg->buffer;
    g_i2c_buffer_wrap[bus].len = data_cfg->len;
    g_i2c_buffer_wrap[bus].restart_flag = data_cfg->restart_flag;
    g_i2c_buffer_wrap[bus].stop_flag = data_cfg->stop_flag;

    errcode_t ret = hal_i2c_ctrl(bus, I2C_CTRL_WRITE_PREPARE, (uintptr_t)&cfg);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    if (i2c_sem_down(&g_i2c_ctrl[bus].sem, I2C_TIMEOUT_SEM) != OSAL_SUCCESS) {
        return ERRCODE_I2C_DMA_TRANSFER_ERROR;
    }

    return ret;
}

static errcode_t i2c_read_int(i2c_bus_t bus, uint16_t dev_addr, hal_i2c_buffer_wrap_t *data_cfg)
{
    hal_i2c_prepare_config_t cfg = { 0 };
    i2c_ctrl_block_info_t *i2c_ctrl = &g_i2c_ctrl[bus];

    cfg.addr = dev_addr;
    cfg.operation_type = I2C_DATA_OPERATION_TYPE_INT;
    cfg.total_len = data_cfg->len;
    i2c_ctrl->operate_type = cfg.operation_type;

    errcode_t ret = hal_i2c_ctrl(bus, I2C_CTRL_READ_PREPARE, (uintptr_t)&cfg);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    uint32_t burst_num;
    uint32_t len = data_cfg->len;

    hal_i2c_ctrl(bus, I2C_CTRL_GET_READ_NUM, (uintptr_t)&burst_num);
    burst_num = burst_num < len ? burst_num : len;
    g_i2c_buffer_wrap[bus].len = data_cfg->len;
    g_i2c_buffer_wrap[bus].buffer = data_cfg->buffer;
    g_i2c_buffer_wrap[bus].restart_flag = data_cfg->restart_flag;
    g_i2c_buffer_wrap[bus].stop_flag = data_cfg->stop_flag;
    g_i2c_buffer_wrap[bus].cmd_len = data_cfg->len;

    hal_i2c_ctrl(bus, I2C_CTRL_FLUSH_RX_FIFO, (uintptr_t)&g_i2c_buffer_wrap[bus]);

    if (i2c_sem_down(&g_i2c_ctrl[bus].sem, I2C_TIMEOUT_SEM) != OSAL_SUCCESS) {
        return ERRCODE_I2C_DMA_TRANSFER_ERROR;
    }

    return ret;
}

static void i2c_evt_callback_int(i2c_bus_t bus, hal_i2c_evt_id_t evt)
{
    switch (evt) {
        case I2C_EVT_TRANSMITION_DONE:
#if (defined(CONFIG_I2C_SUPPORT_SLAVE_INT) && (CONFIG_I2C_SUPPORT_SLAVE_INT == 1))
            if (g_i2c_ctrl[bus].slave_cb.on_event != NULL) {
                g_i2c_ctrl[bus].slave_cb.on_event(I2C_IRQ_EVT_STOP);
            }
#else
            i2c_sem_up(&g_i2c_ctrl[bus].sem);
#endif
            break;
        case I2C_EVT_TRANSMITION_ABRT:
            if (g_i2c_ctrl[bus].callback != NULL) {
                g_i2c_ctrl[bus].callback(bus, I2C_IRQ_EVT_I2C_ERR);
            }
            break;
        case I2C_EVT_TX_READY:
            i2c_evt_tx_callback(bus);
            break;
        case I2C_EVT_RX_READY:
            i2c_evt_rx_callback(bus);
            break;
        case I2C_EVT_TRANSMITION_BUSY:
            i2c_evt_busy_callback(bus);
            break;
        default:
            break;
    }
}
#endif  /* CONFIG_I2C_SUPPORT_INT */

#if (defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1))
static void i2c_evt_callback_dma(i2c_bus_t bus, hal_i2c_evt_id_t evt)
{
    switch (evt) {
        case I2C_EVT_TRANSMITION_DONE:
            g_i2c_ctrl[bus].dma_trans->trans_succ = true;
            break;
        case I2C_EVT_TRANSMITION_ABRT:
            g_i2c_ctrl[bus].dma_trans->trans_succ = false;
            break;
        default:
            break;
    }
    i2c_dma_transfer_finish(bus);
}
#endif

#if (defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)) || \
    (defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1))
static errcode_t i2c_evt_callback(i2c_bus_t bus, hal_i2c_evt_id_t evt, uintptr_t param)
{
    unused(param);
    unused(bus);
#if (defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1))
    if (g_i2c_ctrl[bus].dma_trans != NULL) {
        i2c_evt_callback_dma(bus, evt);
        return ERRCODE_SUCC;
    }
#endif
#if (defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1))
        i2c_evt_callback_int(bus, evt);
#endif
    return ERRCODE_SUCC;
}
#else
__attribute__((weak)) errcode_t i2c_evt_callback(i2c_bus_t bus, hal_i2c_evt_id_t evt, uintptr_t param);
#endif

#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
errcode_t uapi_i2c_set_irq_mode(i2c_bus_t bus, bool irq_en)
{
    if (bus >= I2C_BUS_NONE) {
        return ERRCODE_I2C_INVALID_PARAMETER;
    }
    if (irq_en) {
        g_i2c_ctrl[bus].operate_type = I2C_DATA_OPERATION_TYPE_INT;
    } else {
        g_i2c_ctrl[bus].operate_type = I2C_DATA_OPERATION_TYPE_POLL;
    }

    return ERRCODE_SUCC;
}

errcode_t uapi_i2c_register_irq_callback(i2c_bus_t bus, i2c_irq_callback_t callback)
{
    g_i2c_ctrl[bus].callback = callback;
    return ERRCODE_SUCC;
}

#if (defined(CONFIG_I2C_SUPPORT_SLAVE_INT) && (CONFIG_I2C_SUPPORT_SLAVE_INT == 1))
errcode_t uapi_i2c_slave_register_irq_callback(i2c_bus_t bus, i2c_slave_callback_t *callback)
{
    if ((bus >= I2C_BUS_NONE) ||
        (callback == NULL) ||
        (callback->on_event == NULL) ||
        (callback->on_read == NULL) ||
        (callback->on_write == NULL)) {
        return ERRCODE_FAIL;
    }
    g_i2c_ctrl[bus].slave_cb.on_event = callback->on_event;
    g_i2c_ctrl[bus].slave_cb.on_read = callback->on_read;
    g_i2c_ctrl[bus].slave_cb.on_write = callback->on_write;
    return ERRCODE_SUCC;
}
#endif

errcode_t uapi_i2c_unregister_irq_callback(i2c_bus_t bus)
{
    g_i2c_ctrl[bus].callback = NULL;
    return ERRCODE_SUCC;
}
#endif  /* CONFIG_I2C_SUPPORT_INT */

static void i2c_sem_init(i2c_bus_t bus)
{
    unused(bus);
#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1) || \
    defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1)
    (void)memset_s(&g_i2c_ctrl[bus].sem, sizeof(g_i2c_ctrl[bus].sem),
                   0, sizeof(g_i2c_ctrl[bus].sem));
#if !defined(CONFIG_I2C_SUPPORT_IN_CHIP_LOOPBACK)
    (void)osal_sem_binary_sem_init(&g_i2c_ctrl[bus].sem, 0);
#endif
#endif  /* CONFIG_I2C_SUPPORT_INT */
}

static void i2c_sem_deinit(i2c_bus_t bus)
{
    unused(bus);
#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1) || \
    defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1)
#if !defined(CONFIG_I2C_SUPPORT_IN_CHIP_LOOPBACK)
    osal_sem_destroy(&g_i2c_ctrl[bus].sem);
#endif
#endif  /* CONFIG_I2C_SUPPORT_INT */
}

#if defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1)
errcode_t uapi_i2c_set_dma_mode(i2c_bus_t bus, bool dma_en)
{
    if (bus >= I2C_BUS_NONE) {
        return ERRCODE_I2C_INVALID_PARAMETER;
    }
    if (dma_en) {
        g_i2c_ctrl[bus].operate_type = I2C_DATA_OPERATION_TYPE_DMA;
    } else {
        g_i2c_ctrl[bus].operate_type = I2C_DATA_OPERATION_TYPE_POLL;
    }
    return ERRCODE_SUCC;
}
#endif  /* CONFIG_I2C_SUPPORT_DMA */

#if defined(CONFIG_I2C_SUPPORT_CONCURRENCY) && (CONFIG_I2C_SUPPORT_CONCURRENCY == 1)
__attribute__((weak)) bool i2c_support_mutex_config(i2c_bus_t bus)
{
    unused(bus);
    return true;
}
#endif  /* CONFIG_I2C_SUPPORT_CONCURRENCY */

static void i2c_mutex_init(i2c_bus_t bus)
{
    unused(bus);
#if defined(CONFIG_I2C_SUPPORT_CONCURRENCY) && (CONFIG_I2C_SUPPORT_CONCURRENCY == 1)
    if (i2c_support_mutex_config(bus) == true) {
        (void)memset_s(&g_i2c_mutex[bus], sizeof(g_i2c_mutex[bus]), 0, sizeof(g_i2c_mutex[bus]));
        (void)osal_mutex_init(&g_i2c_mutex[bus]);
    }
#endif  /* CONFIG_I2C_SUPPORT_CONCURRENCY */
}

static void i2c_mutex_destroy(i2c_bus_t bus)
{
    unused(bus);
#if defined(CONFIG_I2C_SUPPORT_CONCURRENCY) && (CONFIG_I2C_SUPPORT_CONCURRENCY == 1)
    if (i2c_support_mutex_config(bus) == true) {
        osal_mutex_destroy(&g_i2c_mutex[bus]);
    }
#endif  /* CONFIG_I2C_SUPPORT_CONCURRENCY */
}

static void i2c_mutex_lock(i2c_bus_t bus)
{
    unused(bus);
#if defined(CONFIG_I2C_SUPPORT_CONCURRENCY) && (CONFIG_I2C_SUPPORT_CONCURRENCY == 1)
    if (i2c_support_mutex_config(bus) == true) {
        osal_mutex_lock(&g_i2c_mutex[bus]);
    }
#endif  /* CONFIG_I2C_SUPPORT_CONCURRENCY */
}

static void i2c_mutex_unlock(i2c_bus_t bus)
{
    unused(bus);
#if defined(CONFIG_I2C_SUPPORT_CONCURRENCY) && (CONFIG_I2C_SUPPORT_CONCURRENCY == 1)
    if (i2c_support_mutex_config(bus) == true) {
        osal_mutex_unlock(&g_i2c_mutex[bus]);
    }
#endif  /* CONFIG_I2C_SUPPORT_CONCURRENCY */

#if defined(CONFIG_I2C_SUPPORT_LPM)
    i2c_port_clock_dynamic_enable(bus, false);
#endif
}

static errcode_t i2c_init_check(i2c_bus_t bus, uint32_t baudrate)
{
    if (unlikely(bus >= I2C_BUS_MAX_NUM)) {
        return ERRCODE_I2C_INVALID_PARAMETER;
    }
    if (unlikely(baudrate > I2C_HS_MODE_BAUDRATE_HIGH_LIMIT || baudrate == 0)) {
        return ERRCODE_I2C_RATE_INVALID;
    }
    if (g_i2c_ctrl[bus].init_flag == true) {
        return ERRCODE_I2C_ALREADY_INIT;
    }
#if defined(CONFIG_I2C_SUPPORT_LPM)
    i2c_port_clock_dynamic_enable(bus, true);
#endif
    return ERRCODE_SUCC;
}

#if defined(CONFIG_I2C_SUPPORT_MASTER) && (CONFIG_I2C_SUPPORT_MASTER == 1)
static errcode_t i2c_master_init_check(i2c_bus_t bus, uint32_t baudrate, uint8_t hscode)
{
    errcode_t ret = i2c_init_check(bus, baudrate);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    if (unlikely(hscode > I2C_HS_MODE_MASTER_CODE_MAX)) {
#if defined(CONFIG_I2C_SUPPORT_LPM)
        i2c_port_clock_dynamic_enable(bus, false);
#endif
        return ERRCODE_I2C_INVALID_PARAMETER;
    }
    return ERRCODE_SUCC;
}

errcode_t uapi_i2c_master_init(i2c_bus_t bus, uint32_t baudrate, uint8_t hscode)
{
    errcode_t ret = i2c_master_init_check(bus, baudrate, hscode);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

#if defined(CONFIG_I2C_SUPPORT_LPM)
    g_i2c_baudrate[bus] = baudrate;
    g_i2c_hscode[bus] = hscode;
#endif  /* CONFIG_I2C_SUPPORT_LPM */

#if defined(CONFIG_I2C_SUPPORT_LPC)
    i2c_port_clock_enable(bus, true);
#endif

    i2c_mutex_init(bus);
    i2c_sem_init(bus);

    ret = hal_i2c_master_init(bus, baudrate, hscode, i2c_evt_callback);
    if (ret != ERRCODE_SUCC) {
        i2c_sem_deinit(bus);
        i2c_mutex_destroy(bus);
        return ret;
    }

#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1) || \
    defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1)
    i2c_port_register_irq(bus);
#endif

    i2c_ctrl_block_info_t *i2c_ctrl = &g_i2c_ctrl[bus];
    i2c_ctrl->init_flag = true;
    i2c_ctrl->master_flag = true;
    i2c_ctrl->hscode = hscode;
#if defined(CONFIG_I2C_SUPPORT_LPM)
    i2c_port_clock_dynamic_enable(bus, false);
#endif

    return ERRCODE_SUCC;
}

static errcode_t i2c_fifo_check(i2c_bus_t bus)
{
    uint64_t start_time = uapi_tcxo_get_count();
    while (hal_i2c_ctrl(bus, I2C_CTRL_CHECK_TX_FIFO_EMPTY, CONFIG_I2C_WAIT_CONDITION_TIMEOUT) != ERRCODE_SUCC) {
        uint64_t time = uapi_tcxo_get_count() - start_time;
        if (time > CONFIG_I2C_WAIT_CONDITION_TIMEOUT * I2C_TIMEOUT_TICK_PER_MS) {
#if defined(CONFIG_I2C_SUPPORT_LPM)
            i2c_port_clock_dynamic_enable(bus, false);
#endif  /* CONFIG_I2C_SUPPORT_LPM */
            return ERRCODE_I2C_TIMEOUT;
        }
    }

    return ERRCODE_SUCC;
}

static errcode_t i2c_master_write(i2c_bus_t bus, uint16_t dev_addr, hal_i2c_buffer_wrap_t *buffer_wrap)
{
    errcode_t ret = ERRCODE_SUCC;
    i2c_mutex_lock(bus);
#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
    if (g_i2c_ctrl[bus].operate_type == I2C_DATA_OPERATION_TYPE_INT) {
        ret = i2c_write_int(bus, dev_addr, buffer_wrap);
        i2c_mutex_unlock(bus);
        return ret;
    }
#endif  /* CONFIG_I2C_SUPPORT_INT */
    ret = i2c_poll_write(bus, dev_addr, buffer_wrap, true);
    i2c_mutex_unlock(bus);
    return ret;
}

errcode_t uapi_i2c_master_write(i2c_bus_t bus, uint16_t dev_addr, i2c_data_t *data)
{
    errcode_t ret = i2c_write_pre_check(bus, data);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    hal_i2c_buffer_wrap_t buffer_wrap = { 0 };
    buffer_wrap.buffer = data->send_buf;
    buffer_wrap.len = data->send_len;
    buffer_wrap.stop_flag = true;
    buffer_wrap.restart_flag = false;

    i2c_mutex_lock(bus);
#if defined(CONFIG_I2C_BUS_RECOVERY) && (CONFIG_I2C_BUS_RECOVERY == 1)
    hal_i2c_ctrl(bus, I2C_CTRL_CLEAN_ADDR_NACK, CONFIG_I2C_WAIT_CONDITION_TIMEOUT);
#endif
#if defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1)
#if defined(CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH) && (CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH == 1)
#if defined(CONFIG_I2C_USE_POLL_IN_EXC_NON_SCHEDULE)
    if (buffer_wrap.len > CONFIG_I2C_POLL_AND_DMA_AUTO_SWITCH_THRESHOLD && !OS_EXC_ACTIVE && OS_SCHEDULER_ACTIVE) {
#else
    if (buffer_wrap.len > CONFIG_I2C_POLL_AND_DMA_AUTO_SWITCH_THRESHOLD) {
#endif
#else
    if (g_i2c_ctrl[bus].operate_type == I2C_DATA_OPERATION_TYPE_DMA) {
#endif  /* CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH */
        ret = i2c_transmit_by_dma(bus, dev_addr, data);
        goto end;
    }
#endif  /* CONFIG_I2C_SUPPORT_DMA */
#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
    if (g_i2c_ctrl[bus].operate_type == I2C_DATA_OPERATION_TYPE_INT) {
        ret = i2c_write_int(bus, dev_addr, &buffer_wrap);
        goto end;
    }
#endif  /* CONFIG_I2C_SUPPORT_INT */
    ret = i2c_poll_write(bus, dev_addr, &buffer_wrap, true);
    goto end;
end:
    i2c_mutex_unlock(bus);
    return ret;
}

static errcode_t i2c_master_write_restart(i2c_bus_t bus, uint16_t dev_addr, i2c_data_t *data)
{
    errcode_t ret;
    hal_i2c_buffer_wrap_t buffer_wrap = { 0 };
    buffer_wrap.buffer = data->send_buf;
    buffer_wrap.len = data->send_len;
    buffer_wrap.stop_flag = false;
    buffer_wrap.restart_flag = true;

    i2c_mutex_lock(bus);
#if defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1)
#if defined(CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH) && (CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH == 1)
    #if defined(CONFIG_I2C_USE_POLL_IN_EXC_NON_SCHEDULE)
    if (buffer_wrap.len > CONFIG_I2C_POLL_AND_DMA_AUTO_SWITCH_THRESHOLD && !OS_EXC_ACTIVE && OS_SCHEDULER_ACTIVE) {
#else
    if (buffer_wrap.len > CONFIG_I2C_POLL_AND_DMA_AUTO_SWITCH_THRESHOLD) {
#endif
        uapi_i2c_set_dma_mode(bus, true);
        ret = i2c_transmit_by_dma(bus, dev_addr, data);
        i2c_mutex_unlock(bus);
        return ret;
    }
    uapi_i2c_set_dma_mode(bus, false);
#else
    if (g_i2c_ctrl[bus].operate_type == I2C_DATA_OPERATION_TYPE_DMA) {
        ret = i2c_transmit_by_dma(bus, dev_addr, data);
        i2c_mutex_unlock(bus);
        return ret;
    }
#endif  /* CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH */
#endif  /* CONFIG_I2C_SUPPORT_DMA */
#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
    if (g_i2c_ctrl[bus].operate_type == I2C_DATA_OPERATION_TYPE_INT) {
        ret = i2c_write_int(bus, dev_addr, &buffer_wrap);
        i2c_mutex_unlock(bus);
        return ret;
    }
#endif  /* CONFIG_I2C_SUPPORT_INT */
    ret = i2c_poll_write(bus, dev_addr, &buffer_wrap, false);
    i2c_mutex_unlock(bus);
    return ret;
}

static errcode_t i2c_master_read(i2c_bus_t bus, uint16_t dev_addr, hal_i2c_buffer_wrap_t *buffer_wrap)
{
    errcode_t ret = ERRCODE_SUCC;
    i2c_mutex_lock(bus);
#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
    if (g_i2c_ctrl[bus].operate_type == I2C_DATA_OPERATION_TYPE_INT) {
        ret = i2c_read_int(bus, dev_addr, buffer_wrap);
        i2c_mutex_unlock(bus);
        return ret;
    }
#endif  /* CONFIG_I2C_SUPPORT_INT */
    ret = i2c_poll_read(bus, dev_addr, buffer_wrap, true);
    i2c_mutex_unlock(bus);
    return ret;
}

errcode_t uapi_i2c_master_read(i2c_bus_t bus, uint16_t dev_addr, i2c_data_t *data)
{
    errcode_t ret = i2c_read_pre_check(bus, data);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    hal_i2c_buffer_wrap_t buffer_wrap = { 0 };
    buffer_wrap.buffer = data->receive_buf;
    buffer_wrap.len = data->receive_len;
    buffer_wrap.stop_flag = true;
    buffer_wrap.restart_flag = false;
    buffer_wrap.cmd_len = data->receive_len;

    i2c_mutex_lock(bus);
#if defined(CONFIG_I2C_BUS_RECOVERY) && (CONFIG_I2C_BUS_RECOVERY == 1)
    hal_i2c_ctrl(bus, I2C_CTRL_CLEAN_ADDR_NACK, CONFIG_I2C_WAIT_CONDITION_TIMEOUT);
#endif
#if defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1)
#if defined(CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH) && (CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH == 1)
#if defined(CONFIG_I2C_USE_POLL_IN_EXC_NON_SCHEDULE)
    if (data->receive_len > CONFIG_I2C_POLL_AND_DMA_AUTO_SWITCH_THRESHOLD && !OS_EXC_ACTIVE && OS_SCHEDULER_ACTIVE) {
#else
    if (data->receive_len > CONFIG_I2C_POLL_AND_DMA_AUTO_SWITCH_THRESHOLD) {
#endif
#else
    if (g_i2c_ctrl[bus].operate_type == I2C_DATA_OPERATION_TYPE_DMA) {
#endif  /* CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH */
        ret = i2c_transmit_by_dma(bus, dev_addr, data);
        goto end;
    }
#endif  /* CONFIG_I2C_SUPPORT_DMA */
#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
    if (g_i2c_ctrl[bus].operate_type == I2C_DATA_OPERATION_TYPE_INT) {
        ret = i2c_read_int(bus, dev_addr, &buffer_wrap);
        goto end;
    }
#endif  /* CONFIG_I2C_SUPPORT_INT */
    ret = i2c_poll_read(bus, dev_addr, &buffer_wrap, true);
    goto end;
end:
    i2c_mutex_unlock(bus);
    return ret;
}

errcode_t uapi_i2c_master_writeread(i2c_bus_t bus, uint16_t dev_addr, i2c_data_t *data)
{
    errcode_t ret = i2c_write_pre_check(bus, data);
    i2c_wait_condition_t condition = { 0 };
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    ret = i2c_read_pre_check(bus, data);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    i2c_mutex_lock(bus);
#if defined(CONFIG_I2C_BUS_RECOVERY) && (CONFIG_I2C_BUS_RECOVERY == 1)
    hal_i2c_ctrl(bus, I2C_CTRL_CLEAN_ADDR_NACK, CONFIG_I2C_WAIT_CONDITION_TIMEOUT);
#endif
#if defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1)
#if defined(CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH) && (CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH == 1)
    if (data->receive_len > CONFIG_I2C_POLL_AND_DMA_AUTO_SWITCH_THRESHOLD) {
#else
    if (g_i2c_ctrl[bus].operate_type == I2C_DATA_OPERATION_TYPE_DMA) {
#endif /* CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH */
        ret = i2c_transmit_by_dma(bus, dev_addr, data);
        goto end;
    }
#endif  /* CONFIG_I2C_SUPPORT_DMA */
    ret = uapi_i2c_master_write(bus, dev_addr, data);
    if (ret != ERRCODE_SUCC) {
        goto end;
    }
#if defined(CONFIG_I2C_SUPPORT_LPM)
    i2c_port_clock_dynamic_enable(bus, true);
#endif  /* CONFIG_I2C_SUPPORT_LPM */
    condition.ctrl_mask = i2c_ctrl_get_mask(I2C_CTRL_CHECK_RESTART_READY);
    (void)i2c_wait(bus, &condition, CONFIG_I2C_WAIT_CONDITION_TIMEOUT);
#if defined(CONFIG_I2C_SUPPORT_LPM)
    i2c_port_clock_dynamic_enable(bus, false);
#endif  /* CONFIG_I2C_SUPPORT_LPM */
    ret = uapi_i2c_master_read(bus, dev_addr, data);
    goto end;
end:
    i2c_mutex_unlock(bus);
    return ret;
}

errcode_t uapi_i2c_master_writeread_restart(i2c_bus_t bus, uint16_t dev_addr, i2c_data_t *data)
{
    errcode_t ret = i2c_write_pre_check(bus, data);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    ret = i2c_read_pre_check(bus, data);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    i2c_mutex_lock(bus);
    ret = i2c_fifo_check(bus);
    if (ret != ERRCODE_SUCC) {
        i2c_mutex_unlock(bus);
        return ret;
    }
#if defined(CONFIG_I2C_BUS_RECOVERY) && (CONFIG_I2C_BUS_RECOVERY == 1)
    hal_i2c_ctrl(bus, I2C_CTRL_CLEAN_ADDR_NACK, CONFIG_I2C_WAIT_CONDITION_TIMEOUT);
#endif
    ret = i2c_master_write_restart(bus, dev_addr, data);
    if (ret != ERRCODE_SUCC) {
        i2c_mutex_unlock(bus);
        return ret;
    }

#if defined(CONFIG_I2C_SUPPORT_LPM)
    i2c_port_clock_dynamic_enable(bus, true);
#endif  /* CONFIG_I2C_SUPPORT_LPM */
    (void)i2c_fifo_check(bus);
    i2c_wait_condition_t condition = { 0 };
    condition.ctrl_mask = i2c_ctrl_get_mask(I2C_CTRL_CHECK_RESTART_READY);
    (void)i2c_wait(bus, &condition, CONFIG_I2C_WAIT_CONDITION_TIMEOUT);
#if defined(CONFIG_I2C_SUPPORT_LPM)
    i2c_port_clock_dynamic_enable(bus, false);
#endif  /* CONFIG_I2C_SUPPORT_LPM */
    data->send_buf = NULL;
    data->send_len = 0;
    ret = uapi_i2c_master_read(bus, dev_addr, data);
    i2c_mutex_unlock(bus);
    return ret;
}

errcode_t uapi_i2c_master_writeread_with_flag(i2c_bus_t bus, uint16_t dev_addr, i2c_buffer_wrap_t *write_buffer,
                                              i2c_buffer_wrap_t *read_buffer)
{
    hal_i2c_buffer_wrap_t *write_data = (hal_i2c_buffer_wrap_t *)(uintptr_t)write_buffer;
    hal_i2c_buffer_wrap_t *read_data = (hal_i2c_buffer_wrap_t *)(uintptr_t)read_buffer;

    errcode_t ret = i2c_buffer_pre_check(bus, write_data);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    ret = i2c_buffer_pre_check(bus, read_data);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    ret = i2c_fifo_check(bus);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    i2c_mutex_lock(bus);
    ret = i2c_master_write(bus, dev_addr, write_data);
    if (ret != ERRCODE_SUCC) {
        i2c_mutex_unlock(bus);
        return ret;
    }

    i2c_wait_condition_t condition = { 0 };
    condition.ctrl_mask = i2c_ctrl_get_mask(I2C_CTRL_CHECK_RESTART_READY);
    (void)i2c_wait(bus, &condition, CONFIG_I2C_WAIT_CONDITION_TIMEOUT);

    ret = i2c_master_read(bus, dev_addr, read_data);
    i2c_mutex_unlock(bus);
    return ret;
}
#endif  /* CONFIG_I2C_SUPPORT_MASTER */

#if defined(CONFIG_I2C_SUPPORT_SLAVE) && (CONFIG_I2C_SUPPORT_SLAVE == 1)
errcode_t uapi_i2c_slave_init(i2c_bus_t bus, uint32_t baudrate, uint16_t addr)
{
    errcode_t ret = i2c_init_check(bus, baudrate);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

#if defined(CONFIG_I2C_SUPPORT_LPM)
    g_i2c_baudrate[bus] = baudrate;
    g_i2c_addr[bus] = addr;
#endif  /* CONFIG_I2C_SUPPORT_LPM */

#if defined(CONFIG_I2C_SUPPORT_LPC)
    i2c_port_clock_enable(bus, true);
#endif

    i2c_mutex_init(bus);
    i2c_sem_init(bus);

    ret = hal_i2c_slave_init(bus, baudrate, addr, i2c_evt_callback);
    if (ret != ERRCODE_SUCC) {
        i2c_sem_deinit(bus);
        i2c_mutex_destroy(bus);
        return ret;
    }

#if (defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)) || \
    (defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1))
    i2c_port_register_irq(bus);
#endif

    i2c_ctrl_block_info_t *i2c_ctrl = &g_i2c_ctrl[bus];
    i2c_ctrl->init_flag = true;
    i2c_ctrl->master_flag = false;
    i2c_ctrl->addr = addr;
#if defined(CONFIG_I2C_SUPPORT_LPM)
    i2c_port_clock_dynamic_enable(bus, false);
#endif
    return ERRCODE_SUCC;
}

errcode_t uapi_i2c_slave_write(i2c_bus_t bus, i2c_data_t *data)
{
    errcode_t ret = i2c_write_pre_check(bus, data);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    hal_i2c_buffer_wrap_t buffer_wrap = { 0 };
    buffer_wrap.buffer = data->send_buf;
    buffer_wrap.len = data->send_len;
    buffer_wrap.stop_flag = false;

    i2c_mutex_lock(bus);
#if defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1)
#if defined(CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH) && (CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH == 1)
#if defined(CONFIG_I2C_USE_POLL_IN_EXC_NON_SCHEDULE)
    if (buffer_wrap.len > CONFIG_I2C_POLL_AND_DMA_AUTO_SWITCH_THRESHOLD && !OS_EXC_ACTIVE && OS_SCHEDULER_ACTIVE) {
#else
    if (buffer_wrap.len > CONFIG_I2C_POLL_AND_DMA_AUTO_SWITCH_THRESHOLD) {
#endif
#else
    if (g_i2c_ctrl[bus].operate_type == I2C_DATA_OPERATION_TYPE_DMA) {
#endif  /* CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH */
        ret = i2c_transmit_by_dma(bus, I2C_DEFAULT_DEV_ADDRESS, data);
        goto end;
    }
#endif  /* CONFIG_I2C_SUPPORT_DMA */
#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
    if (g_i2c_ctrl[bus].operate_type == I2C_DATA_OPERATION_TYPE_INT) {
        ret = i2c_write_int(bus, I2C_DEFAULT_DEV_ADDRESS, &buffer_wrap);
        goto end;
    }
#endif  /* CONFIG_I2C_SUPPORT_INT */
    ret = i2c_poll_write(bus, I2C_DEFAULT_DEV_ADDRESS, &buffer_wrap, true);
    goto end;
end:
    i2c_mutex_unlock(bus);
    return ret;
}

errcode_t uapi_i2c_slave_read(i2c_bus_t bus, i2c_data_t *data)
{
    errcode_t ret = i2c_read_pre_check(bus, data);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    hal_i2c_buffer_wrap_t buffer_wrap;
    buffer_wrap.buffer = data->receive_buf;
    buffer_wrap.len = data->receive_len;
    buffer_wrap.stop_flag = false;
    buffer_wrap.restart_flag = false;

    i2c_mutex_lock(bus);
#if defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1)
    unused(buffer_wrap);
#if defined(CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH) && (CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH == 1)
#if defined(CONFIG_I2C_USE_POLL_IN_EXC_NON_SCHEDULE)
    if (data->receive_len > CONFIG_I2C_POLL_AND_DMA_AUTO_SWITCH_THRESHOLD && !OS_EXC_ACTIVE && OS_SCHEDULER_ACTIVE) {
#else
    if (data->receive_len > CONFIG_I2C_POLL_AND_DMA_AUTO_SWITCH_THRESHOLD) {
#endif
#else
    if (g_i2c_ctrl[bus].operate_type == I2C_DATA_OPERATION_TYPE_DMA) {
#endif  /* CONFIG_I2C_SUPPORT_POLL_AND_DMA_AUTO_SWITCH */
        ret = i2c_transmit_by_dma(bus, 0, data);
        goto end;
    }
#endif  /* CONFIG_I2C_SUPPORT_DMA */

#if defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)
    if (g_i2c_ctrl[bus].operate_type == I2C_DATA_OPERATION_TYPE_INT) {
        ret = i2c_read_int(bus, I2C_DEFAULT_DEV_ADDRESS, &buffer_wrap);
        goto end;
    }
#endif  /* CONFIG_I2C_SUPPORT_INT */
    ret = i2c_poll_read(bus, I2C_DEFAULT_DEV_ADDRESS, &buffer_wrap, true);
    goto end;
end:
    i2c_mutex_unlock(bus);
    return ret;
}
#endif  /* CONFIG_I2C_SUPPORT_SLAVE */

errcode_t uapi_i2c_deinit(i2c_bus_t bus)
{
    errcode_t ret = i2c_bus_init_check(bus);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    i2c_ctrl_block_info_t *i2c_ctrl = &g_i2c_ctrl[bus];

    hal_i2c_deinit(bus);
    i2c_sem_deinit(bus);
    i2c_mutex_destroy(bus);
#if (defined(CONFIG_I2C_SUPPORT_INT) && (CONFIG_I2C_SUPPORT_INT == 1)) || \
    defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1)
    i2c_port_unregister_irq(bus);
#endif
#if defined(CONFIG_I2C_SUPPORT_LPC)
    i2c_port_clock_enable(bus, false);
#endif
#if defined(CONFIG_I2C_SUPPORT_LPM)
    i2c_port_clock_dynamic_enable(bus, false);
#endif

    i2c_ctrl->init_flag = false;
    i2c_ctrl->master_flag = false;
    return ERRCODE_SUCC;
}

errcode_t uapi_i2c_set_baudrate(i2c_bus_t bus, uint32_t baudrate)
{
    if (baudrate > I2C_HS_MODE_BAUDRATE_HIGH_LIMIT || baudrate == 0) {
        return ERRCODE_I2C_RATE_INVALID;
    }
    errcode_t ret = i2c_bus_init_check(bus);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
#if defined(CONFIG_I2C_SUPPORT_LPM)
    g_i2c_baudrate[bus] = baudrate;
#endif  /* CONFIG_I2C_SUPPORT_LPM */
    i2c_ctrl_block_info_t *i2c_ctrl = &g_i2c_ctrl[bus];
    i2c_mutex_lock(bus);
    if (i2c_ctrl->master_flag == true) {
        ret = hal_i2c_master_init(bus, baudrate, i2c_ctrl->hscode, i2c_evt_callback);
    } else {
        ret = hal_i2c_slave_init(bus, baudrate, i2c_ctrl->addr, i2c_evt_callback);
    }
    i2c_mutex_unlock(bus);
    return ret;
}

#if defined(CONFIG_I2C_SUPPORT_LPM)
errcode_t uapi_i2c_suspend(uintptr_t arg)
{
    errcode_t ret = ERRCODE_SUCC;
    unused(arg);
    for (uint32_t i = 0; i < I2C_BUS_MAX_NUMBER; i++) {
        if (g_i2c_ctrl[i].init_flag == false) {
            continue;
        }
        g_i2c_suspend_flag[i] = true;
    }
    return ret;
}

errcode_t uapi_i2c_resume(uintptr_t arg)
{
    errcode_t ret = ERRCODE_SUCC;
    unused(arg);

    for (uint32_t i = 0; i < I2C_BUS_MAX_NUMBER; i++) {
        if (g_i2c_suspend_flag[i] == false) {
            continue;
        }
#if defined(CONFIG_I2C_SUPPORT_LPM)
        i2c_port_clock_dynamic_enable(i, true);
#endif  /* CONFIG_I2C_SUPPORT_LPM */
        hal_i2c_deinit(i);
        if (g_i2c_ctrl[i].master_flag == true) {
#if defined(CONFIG_I2C_SUPPORT_MASTER) && (CONFIG_I2C_SUPPORT_MASTER == 1)
            hal_i2c_master_init(i, g_i2c_baudrate[i], g_i2c_hscode[i], i2c_evt_callback);
#endif
        } else {
#if defined(CONFIG_I2C_SUPPORT_SLAVE) && (CONFIG_I2C_SUPPORT_SLAVE == 1)
            hal_i2c_slave_init(i, g_i2c_baudrate[i], g_i2c_addr[i], i2c_evt_callback);
#endif
        }
#if defined(CONFIG_I2C_SUPPORT_LPM)
        i2c_port_clock_dynamic_enable(i, false);
#endif  /* CONFIG_I2C_SUPPORT_LPM */
        g_i2c_suspend_flag[i] = false;
    }
    return ret;
}
#endif