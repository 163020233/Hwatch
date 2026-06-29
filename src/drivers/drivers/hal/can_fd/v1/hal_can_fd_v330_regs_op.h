/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides v330 CAN-FD register opreation api \n
 *
 * History: \n
 * 2023-02-25, Create file. \n
 */
#ifndef HAL_CAN_FD_V330_REGS_OP_H
#define HAL_CAN_FD_V330_REGS_OP_H

#include <stdint.h>
#include "errcode.h"
#include "hal_can_fd_v330_regs_def.h"
#include "hal_can.h"
#include "can_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_can_fd_regs_op CAN-FD v330 regs opreation
 * @ingroup  drivers_hal_can
 * @{
 */

#define CAN_FD_IGNORED_BIT  0x2
#define CAN_FD_ENABLE_ALL_LINE 0x3

extern uintptr_t g_can_regs;
extern uintptr_t g_can_fd_sram;
extern uintptr_t g_can_fd_std_filter;
extern uintptr_t g_can_fd_ex_filter;
extern uintptr_t g_can_fd_rx_fifo_0;
extern uintptr_t g_can_fd_rx_fifo_1;
extern uintptr_t g_can_fd_tx_fifo;
extern uintptr_t g_can_fd_tx_event_buffer;

/**
 * @brief  Disable the initialization state and enable the write protection. Start working on CAN-Bus.
 * @param  [in]  mode CAN controller opreation mode.
 */
void hal_can_fd_regs_configuration_change_disable(uint32_t mode);

/**
 * @brief  Add the CAN frame structure to the Tx-FIFO.
 * @param  [in]  put_idx Insert index of the FIFO.
 * @param  [in]  msg Expected CAN frame structure to be transmitted.
 * @retval ERRCODE_SUCC   成功。
 * @retval Other          失败，参考 @ref errcode_t
 */
errcode_t hal_can_fd_set_tx_element(uint32_t put_idx, hal_can_msg_t *msg, uint8_t *data, uint32_t length);

/**
 * @brief  Get the CAN frame structure from the Rx-FIFO
 * @param  [in]  get_idx Index of the first element in the Rx-FIFO.
 * @param  [out]  msg Obtained CAN frame.
 * @retval The CAN frame structure got from the Rx-FIFO.
 */
void hal_can_fd_get_rx_0_element(uint32_t get_idx, hal_can_msg_t *msg, uint8_t *data, uint32_t *length);

/**
 * @brief  Get the CAN frame structure from the Rx-FIFO
 * @param  [in]  get_idx Index of the first element in the Rx-FIFO.
 * @param  [out]  msg Obtained CAN frame.
 * @retval The CAN frame structure got from the Rx-FIFO.
 */
void hal_can_fd_get_rx_1_element(uint32_t get_idx, hal_can_msg_t *msg, uint8_t *data, uint32_t *length);

/**
 * @brief  Set the filter array configuration.
 */
void hal_can_fd_regs_set_filter_configuration(void);

/**
 * @brief  Set the Rx-FIFO configuration.
 */
void hal_can_fd_regs_set_rx_fifo_configuration(void);

/**
 * @brief  Set the Tx-FIFO configuration.
 */
void hal_can_fd_regs_set_tx_fifo_configuration(void);

/**
 * @brief  Enable the initialization state and disable the write protection.
 */
static inline void hal_can_fd_regs_configuration_change_enable(void)
{
    cccr_data_t cccr;
    cccr.d32 = ((can_fd_regs_t *)g_can_regs)->cccr;
    cccr.b.init = 1;
    ((can_fd_regs_t *)g_can_regs)->cccr = cccr.d32;
    while (((can_fd_regs_t *)g_can_regs)->cccr != cccr.d32) {}
    cccr.b.cce = 1;
    ((can_fd_regs_t *)g_can_regs)->cccr = cccr.d32;
    while (((can_fd_regs_t *)g_can_regs)->cccr != cccr.d32) {}
}

static inline void hal_can_fd_regs_configuration_disable(void)
{
    cccr_data_t cccr;
    cccr.d32 = ((can_fd_regs_t *)g_can_regs)->cccr;
    cccr.b.init = 0;
    cccr.b.cce = 0;
    ((can_fd_regs_t *)g_can_regs)->cccr = cccr.d32;
}

/**
 * @brief  Set the nominal time of the arbitration segment.
 * @param  [in]  nsjw Nominal resyncronization jump width.
 * @param  [in]  nbrp Nominal bit rate prescaler.
 * @param  [in]  ntseg1 Nominal time segment before sample point.
 * @param  [in]  ntseg2 Nominal time segment after sample point.
 */
static inline void hal_can_fd_regs_set_nbtp(uint32_t nsjw, uint32_t nbrp, uint32_t ntseg1, uint32_t ntseg2)
{
    nbtp_data_t nbtp;
    nbtp.d32 = ((can_fd_regs_t *)g_can_regs)->nbtp;
    nbtp.b.nsjw = nsjw;
    nbtp.b.nbrp = nbrp;
    nbtp.b.ntseg1 = ntseg1;
    nbtp.b.ntseg2 = ntseg2;
    ((can_fd_regs_t *)g_can_regs)->nbtp = nbtp.d32;
}

/**
 * @brief  Set the nominal time of the data segment.
 * @param  [in]  dsjw Data resyncronization jump width.
 * @param  [in]  dbrp Data bit rate prescaler.
 * @param  [in]  dtseg1 Data time segment before sample point.
 * @param  [in]  dtseg2 Data time segment after sample point.
 */
static inline void hal_can_fd_regs_set_dbtp(uint32_t dsjw, uint32_t dbrp, uint32_t dtseg1, uint32_t dtseg2)
{
    dbtp_data_t dbtp;
    dbtp.d32 = ((can_fd_regs_t *)g_can_regs)->dbtp;
    dbtp.b.dsjw = dsjw;
    dbtp.b.dbrp = dbrp;
    dbtp.b.dtseg1 = dtseg1;
    dbtp.b.dtseg2 = dtseg2;
    dbtp.b.tdc = 1;
    ((can_fd_regs_t *)g_can_regs)->dbtp = dbtp.d32;
}

static inline void hal_can_fd_regs_set_tdc_enable(uint8_t enable)
{
    dbtp_data_t dbtp;
    dbtp.d32 = ((can_fd_regs_t *)g_can_regs)->dbtp;
    dbtp.b.tdc = enable;
    ((can_fd_regs_t *)g_can_regs)->dbtp = dbtp.d32;
}

static inline void hal_can_fd_regs_set_tdcr(uint32_t offset)
{
    tdcr_data_t tdcr;
    tdcr.d32 = ((can_fd_regs_t *)g_can_regs)->tdcr;
    tdcr.b.tdco = offset;
    tdcr.b.tdcf = offset;
    ((can_fd_regs_t *)g_can_regs)->tdcr = tdcr.d32;
}

/**
 * @brief  Set the standard filter.
 * @param  [in]  index Index of the standard filter array.
 * @param  [in]  std_filter Standard filter expected to be set.
 */
static inline void hal_can_fd_regs_set_std_filter(uint32_t index, hal_can_filter_t std_filter)
{
    standard_filter_config_t std_filter_element;
    std_filter_element.d32 = ((std_filter_array_t *)g_can_fd_std_filter)->element[index];
    std_filter_element.b.sfec = std_filter.fifo_id + 1;
    std_filter_element.b.sft = std_filter.filter_type;
    std_filter_element.b.sfid1 = std_filter.fid1;
    std_filter_element.b.sfid2 = std_filter.fid2;
    ((std_filter_array_t *)g_can_fd_std_filter)->element[index] = std_filter_element.d32;
}

/**
 * @brief  Set the extended filter.
 * @param  [in]  index Index of the extended filter array.
 * @param  [in]  ex_filter Extended filter expected to be set.
 */
static inline void hal_can_fd_regs_set_ex_filter(uint32_t index, hal_can_filter_t ex_filter)
{
    extend_filter_config_t ex_filter_element;
    ex_filter_element.d64 = ((ex_filter_array_t *)g_can_fd_ex_filter)->element[index];
    ex_filter_element.b.efec = ex_filter.fifo_id + 1;
    ex_filter_element.b.eft = ex_filter.filter_type;
    ex_filter_element.b.efid1 = ex_filter.fid1;
    ex_filter_element.b.efid2 = ex_filter.fid2;
    ((ex_filter_array_t *)g_can_fd_ex_filter)->element[index] = ex_filter_element.d64;
}

/**
 * @brief  Set the CAN controller interrupt configuration.
 */
static inline void hal_can_fd_regs_set_int_configuration(void)
{
    ir_data_t ie;
    ir_data_t ils;
    ie.d32 = ((can_fd_regs_t *)g_can_regs)->ie;
    ils.d32 = ((can_fd_regs_t *)g_can_regs)->ils;
    ie.b.rf0n = 1;
    ie.b.rf1n = 1;
    ils.b.rf1n = 1;
    ((can_fd_regs_t *)g_can_regs)->ie = ie.d32;
    ((can_fd_regs_t *)g_can_regs)->ils = ils.d32;
    ((can_fd_regs_t *)g_can_regs)->ile = 0x3;
}

static inline void hal_can_fd_regs_int_enable(uint32_t int_mask)
{
    ir_data_t ie;
    ie.d32 = ((can_fd_regs_t *)g_can_regs)->ie;
    ie.d32 = int_mask | ie.d32;
    ((can_fd_regs_t *)g_can_regs)->ie = ie.d32;
}

static inline void hal_can_fd_regs_int_disable(uint32_t int_mask)
{
    ir_data_t ie;
    ie.d32 = ((can_fd_regs_t *)g_can_regs)->ie;
    ie.d32 = ie.d32 & ~(int_mask);
    ((can_fd_regs_t *)g_can_regs)->ie = ie.d32;
}

/**
 * @brief  Get the insert index of the Tx-FIFO.
 */
static inline uint32_t hal_can_fd_regs_get_tx_put_idx(void)
{
    txfqs_data_t txfqs;
    txfqs.d32 = ((can_fd_regs_t *)g_can_regs)->txfqs;
    return txfqs.b.tfqpi;
}

/**
 * @brief  Add CAN frame transmission request.
 * @param  [in]  request_mask Send request mask.
 */
static inline void hal_can_fd_regs_add_request(uint32_t request_mask)
{
    ((can_fd_regs_t *)g_can_regs)->txbar = request_mask;
}

/**
 * @brief  Get transmission occurred mask.
 */
static inline uint32_t hal_can_fd_regs_get_txbto(void)
{
    return ((can_fd_regs_t *)g_can_regs)->txbto;
}

/**
 * @brief  Get transmission pending mask.
 */
static inline uint32_t hal_can_fd_regs_get_pending(void)
{
    return ((can_fd_regs_t *)g_can_regs)->txbrp;
}

/**
 * @brief  Cancel the CAN frame transmission request.
 * @param  [in]  cancel_mask Request cancel mask.
 */
static inline void hal_can_fd_regs_cancel(uint32_t cancel_mask)
{
    ((can_fd_regs_t *)g_can_regs)->txbcr = cancel_mask;
}

/**
 * @brief  Get the index of the first element in the Rx-FIFO0.
 */
static inline uint32_t hal_can_fd_regs_get_rx_fifo_0_get_idx(void)
{
    rxfs_data_t rxf0s;
    rxf0s.d32 = ((can_fd_regs_t *)g_can_regs)->rxf0s;
    return rxf0s.b.fgi;
}

/**
 * @brief  Get the index of the first element in the Rx-FIFO1.
 */
static inline uint32_t hal_can_fd_regs_get_rx_fifo_1_get_idx(void)
{
    rxfs_data_t rxf1s;
    rxf1s.d32 = ((can_fd_regs_t *)g_can_regs)->rxf1s;
    return rxf1s.b.fgi;
}

/**
 * @brief  Get the fill level of the Rx-FIFO0.
 */
static inline uint32_t hal_can_fd_regs_get_rx_fifo_0_fill_level(void)
{
    rxfs_data_t rxf0s;
    rxf0s.d32 = ((can_fd_regs_t *)g_can_regs)->rxf0s;
    return rxf0s.b.ffl;
}

/**
 * @brief  Get the fill level of the Rx-FIFO1.
 */
static inline uint32_t hal_can_fd_regs_get_rx_fifo_1_fill_level(void)
{
    rxfs_data_t rxf1s;
    rxf1s.d32 = ((can_fd_regs_t *)g_can_regs)->rxf1s;
    return rxf1s.b.ffl;
}

/**
 * @brief  Acknowledgment of receiving CAN frames in Rx-FIFO0.
 * @param  [in]  idx Index to be acknowledged.
 */
static inline void hal_can_fd_regs_ack_fifo_0_element(uint32_t idx)
{
    rxfa_data_t rxfa_data;
    rxfa_data.d32 = ((can_fd_regs_t *)g_can_regs)->rxf0a;
    rxfa_data.b.fai = idx;
    ((can_fd_regs_t *)g_can_regs)->rxf0a = rxfa_data.d32;
}

/**
 * @brief  Acknowledgment of receiving CAN frames in Rx-FIFO1.
 * @param  [in]  idx Index to be acknowledged.
 */
static inline void hal_can_fd_regs_ack_fifo_1_element(uint32_t idx)
{
    rxfa_data_t rxfa_data;
    rxfa_data.d32 = ((can_fd_regs_t *)g_can_regs)->rxf1a;
    rxfa_data.b.fai = idx;
    ((can_fd_regs_t *)g_can_regs)->rxf1a = rxfa_data.d32;
}

/**
 * @brief  Clear the interrupt register.
 * @param  [in]  val Interrupt register mask.
 */
static inline void hal_can_fd_regs_clr_ir(uint32_t val)
{
    ir_data_t ir;
    ir.d32 = ((can_fd_regs_t *)g_can_regs)->ir;
    ir.d32 = ir.d32 & val;
    ((can_fd_regs_t *)g_can_regs)->ir = ir.d32;
}

static inline uint32_t hal_can_fd_regs_get_ir(void)
{
    return ((can_fd_regs_t *)g_can_regs)->ir;
}

static inline uint32_t hal_can_fd_regs_get_ie(void)
{
    return ((can_fd_regs_t *)g_can_regs)->ie;
}

static inline uint32_t hal_can_fd_regs_get_ils(void)
{
    return ((can_fd_regs_t *)g_can_regs)->ils;
}

static inline void hal_can_fd_regs_int_select_line_1(uint32_t val)
{
    ir_data_t ils;
    ils.d32 = ((can_fd_regs_t *)g_can_regs)->ils;
    ils.d32 = val | ils.d32;
    ((can_fd_regs_t *)g_can_regs)->ils = ils.d32;
}

static inline void hal_can_fd_regs_int_select_line_0(uint32_t val)
{
    ir_data_t ils;
    ils.d32 = ((can_fd_regs_t *)g_can_regs)->ils;
    ils.d32 = ils.d32 & (~val);
    ((can_fd_regs_t *)g_can_regs)->ils = ils.d32;
}

static inline void hal_can_fd_enable_all_int_line(void)
{
    ((can_fd_regs_t *)g_can_regs)->ile = CAN_FD_ENABLE_ALL_LINE;
}

static inline uint32_t hal_can_fd_regs_get_error_passive_sts(void)
{
    psr_data_t psr;
    psr.d32 = ((can_fd_regs_t *)g_can_regs)->psr;
    return psr.b.ep;
}

static inline uint32_t hal_can_fd_regs_get_bus_off_sts(void)
{
    psr_data_t psr;
    psr.d32 = ((can_fd_regs_t *)g_can_regs)->psr;
    return psr.b.bo;
}

/**
 * @brief  Set the buffer size in CAN-FD mode.
 */
static inline void hal_can_fd_set_buffer_configuration(void)
{
    ((can_fd_regs_t *)g_can_regs)->txesc = 0x7;
    ((can_fd_regs_t *)g_can_regs)->rxesc = 0x777;
}

static inline uint8_t hal_can_fd_regs_get_tec(void)
{
    ecr_data_t ecr;
    ecr.d32 = ((can_fd_regs_t *)g_can_regs)->ecr;
    return ecr.b.tec;
}

static inline uint8_t hal_can_fd_regs_get_rec(void)
{
    ecr_data_t ecr;
    ecr.d32 = ((can_fd_regs_t *)g_can_regs)->ecr;
    return ecr.b.rec;
}

static inline uint32_t hal_can_fd_regs_get_txbcf(void)
{
    return ((can_fd_regs_t *)g_can_regs)->txbcf;
}

static inline uint32_t hal_can_fd_regs_get_rx_sts(void)
{
    test_data_t test;
    test.d32 = ((can_fd_regs_t *)g_can_regs)->test;
    return test.b.rx;
}

static inline uint32_t hal_can_fd_regs_get_dbtp(void)
{
    return ((can_fd_regs_t *)g_can_regs)->dbtp;
}

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif