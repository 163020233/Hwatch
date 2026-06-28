/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides V330 CAN register. \n
 *
 * History: \n
 * 2023-02-21, Create file. \n
 */
#ifndef HAL_CAN_FD_V330_REGS_DEF_H
#define HAL_CAN_FD_V330_REGS_DEF_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_can_fd_v330_regs_def CAN-FD V330 Registers Defination.
 * @ingroup  drivers_hal_can_fd
 * @{
 */

#define ELEMENT_MAXIMUM_SIZE   64

/**
 * @brief  This union represents the bit fields in the DBTP. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements. \n
 *         This register is valid only when CCCR.FDOE and CCCR.brse are set.
 */
typedef union dbtp_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t dsjw            :   4; /*!< Data resyncronization jump width. */
        uint32_t dtseg2          :   4; /*!< Data time segment after sample point. */
        uint32_t dtseg1          :   5; /*!< Data time segment before sample point. */
        uint32_t reserved0       :   3; /*!< Reserved. */
        uint32_t dbrp            :   5; /*!< Data bit rate prescaler. */
        uint32_t reserved1       :   2; /*!< Reserved. */
        uint32_t tdc             :   1; /*!< Transmitter delay conpensation. */
        uint32_t reserved2       :   8; /*!< Reserved. */
    } b;                                        /*!< Register bits. */
} dbtp_data_t;

/**
 * @brief  This union represents the bit fields in the TEST. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements. \n
 *         This register is valid only when CCCR.TEST is set.
 */
typedef union test_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t tam             :   1; /*!< Test ASC multplexer control. */
        uint32_t tat             :   1; /*!< Test ASC transmit control. */
        uint32_t cam             :   1; /*!< Check ASC mutiplexer control. */
        uint32_t cat             :   1; /*!< Check ASC transmit control. */
        uint32_t lbck            :   1; /*!< Loop back mode enable. */
        uint32_t tx              :   2; /*!< Control of transmit pin.
                                            -00 controlled by the CAN Core.
                                            -01 Sample Point can be monitored at pin tx.
                                            -10 Dominant level(0) at pin tx.
                                            -11 Recessive(1) at pin tx. */
        uint32_t rx              :   1; /*!< Control of receive pin. */
        uint32_t txbnp           :   5; /*!< Tx Buffer number prepared. Valid when PVAL is set. */
        uint32_t pval            :   1; /*!< Prepare valid. */
        uint32_t reserved0       :   2; /*!< Reserved. */
        uint32_t txbns           :   5; /*!< Tx Buffer number started. */
        uint32_t sval            :   1; /*!< Region txbns enable. */
        uint32_t reserved1       :  10; /*!< Reserved. */
    } b;                                        /*!< Register bits. */
} test_data_t;

/**
 * @brief  This union represents the bit fields in the CC Control Register(CCCR). \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union cccr_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t init            :   1; /*!< Initialization Bit.
                                             -0 Can Controller Enable.
                                             -1 Can Controller Initialization is started. */
        uint32_t cce             :   1; /*!< Configuration change enable. */
        uint32_t rme             :   1; /*!< Restricted opreation mode enable. */
        uint32_t csa             :   1; /*!< Clock stop acknowledge. */
        uint32_t csr             :   1; /*!< Clock stop request. */
        uint32_t mon             :   1; /*!< Bus monitoring mode enable. */
        uint32_t dar             :   1; /*!< Disable automic retransmission. */
        uint32_t test            :   1; /*!< Test mode enable. */
        uint32_t fdoe            :   1; /*!< FD opreation enable. */
        uint32_t brse            :   1; /*!< Bit rate switch enable. */
        uint32_t utsu            :   1; /*!< Use timestamping unit. */
        uint32_t wmm             :   1; /*!< Wide message marker enable. */
        uint32_t pxhd            :   1; /*!< Protocol exception handling disable. */
        uint32_t efbi            :   1; /*!< Edge filtering during Bus Integration. */
        uint32_t txp             :   1; /*!< Transmit pause. */
        uint32_t niso            :   1; /*!< Non ISO Opreation. */
        uint32_t reserved        :  16; /*!< Reserved. */
    } b;                                        /*!< Register bits. */
} cccr_data_t;

/**
 * @brief  This union represents the bit fields in the NBTP. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union nbtp_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t ntseg2          :   7; /*!< Nominal time segment after sample point. */
        uint32_t reserved0       :   1; /*!< Reserved. */
        uint32_t ntseg1          :   8; /*!< Nominal time segment before sample point. */
        uint32_t nbrp            :   9; /*!< Nominal bit rate prescaler. */
        uint32_t nsjw            :   7; /*!< Nominal resyncronization jump width. */
    } b;                                        /*!< Register bits. */
} nbtp_data_t;

/**
 * @brief  This union represents the bit fields in the TSCC. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union tscc_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t tss             :   1; /*!< Timestamp select.
                                            -00 Always 0x0000.
                                            -01 Incremented according to TCP.
                                            -10 External timestamp counter used.
                                            -11 Reserved. Same as 0x00. */
        uint32_t reserved0       :  14; /*!< Reserved. */
        uint32_t tcp             :   4; /*!< Timestamp counter prescaler. */
        uint32_t reserved1       :  12; /*!< Reserved. */
    } b;                                        /*!< Register bits. */
} tscc_data_t;

/**
 * @brief  This union represents the bit fields in the TOCC. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union tocc_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t etoc            :   1; /*!< Timeout counter enable. */
        uint32_t tos             :   2; /*!< Timeout select.
                                            -00 Continuout opreation.
                                            -01 Controlled by Tx Event FIFO.
                                            -10 Controlled by Rx FIFO 0.
                                            -11 Controlled by Rx FIFO 1. */
        uint32_t reserved        :  13; /*!< Reserved. */
        uint32_t top             :  16; /*!< Start value of the timeout counter(down-counter). */
    } b;                                        /*!< Register bits. */
} tocc_data_t;

/**
 * @brief  This union represents the bit fields in the Interrupt register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union ir_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t rf0n            :   1; /*!< Rx FIFO 0 new message. */
        uint32_t rf0w            :   1; /*!< Rx FIFO 0 watermark reached. */
        uint32_t rf0f            :   1; /*!< Rx FIFO 0 full. */
        uint32_t rf0l            :   1; /*!< Rx FIFO 0 message lost. */
        uint32_t rf1n            :   1; /*!< Rx FIFO 1 new message. */
        uint32_t rf1w            :   1; /*!< Rx FIFO 1 watermark reached. */
        uint32_t rf1f            :   1; /*!< Rx FIFO 1 full. */
        uint32_t rf1l            :   1; /*!< Rx FIFO 1 message lost. */
        uint32_t hpm             :   1; /*!< High priority message received. */
        uint32_t tc              :   1; /*!< Transmission completed. */
        uint32_t tcf             :   1; /*!< Transmission cancellation finished. */
        uint32_t tfe             :   1; /*!< Tx FIFO Empty. */
        uint32_t tefn            :   1; /*!< Tx event FIFO new entry. */
        uint32_t tefw            :   1; /*!< Tx event FIFO watermark reached. */
        uint32_t teff            :   1; /*!< Tx event FIFO full. */
        uint32_t tefl            :   1; /*!< Tx event FIFO element lost. */
        uint32_t tsw             :   1; /*!< Timestamp wraparound. */
        uint32_t mraf            :   1; /*!< Message RAM access Failure. */
        uint32_t too             :   1; /*!< Timeout occurred. */
        uint32_t drx             :   1; /*!< Message stored to dedicated Rx Buffer. */
        uint32_t bec             :   1; /*!< Bit error corrected. */
        uint32_t beu             :   1; /*!< Bit error uncorrected. */
        uint32_t elo             :   1; /*!< Error logging overflow. */
        uint32_t ep              :   1; /*!< Error passive status. */
        uint32_t ew              :   1; /*!< Error warning status. */
        uint32_t bo              :   1; /*!< Error bus_off status. */
        uint32_t wdi             :   1; /*!< Watchdog interrupt. */
        uint32_t pea             :   1; /*!< Protocol error in arbitration phase. */
        uint32_t ped             :   1; /*!< Protocol error in data phase. */
        uint32_t ara             :   1; /*!< Access to reserved address */
        uint32_t rsvd            :   2; /*!< Reserved. */
    } b;                                        /*!< Register bits. */
} ir_data_t;

/**
 * @brief  This union represents the bit fields in the Error count register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union ecr_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t tec            :   8; /*!< Transmit error counter. */
        uint32_t rec            :   7; /*!< Receive error counter. */
        uint32_t rp             :   1; /*!< Receive error passive flag. */
        uint32_t cel            :   8; /*!< CAN Error logging. */
        uint32_t rsvd           :   8; /*!< Reserved. */
    } b;                                        /*!< Register bits. */
} ecr_data_t;

/**
 * @brief  This union represents the bit fields in the Protocol status register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union psr_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t lec              :   3; /*!< Last Error Code
                                              --0 No Error
                                              --1 Stuff Error
                                              --2 Format Error
                                              --3 ACK Error
                                              --4 Bit1 Error
                                              --5 Bit0 Error
                                              --6 CRC Error
                                              --7 No Change */
        uint32_t act              :   2; /*!< Acticity
                                              --00 Synchronizing - node is synchronizing on CAN communication
                                              --01 Idle - node is neither receiver nor transmitter
                                              --10 Receiver - node is operating as receiver
                                              --11 Transmitter - node is operating as transmitter */
        uint32_t ep               :   1; /*!< Error Passive */
        uint32_t ew               :   1; /*!< Error Warning */
        uint32_t bo               :   1; /*!< Bus_off status */
        uint32_t dlec             :   3; /*!< Data Phase Last Error Code */
        uint32_t resi             :   1; /*!< ESI flag of last received CAN FD Message */
        uint32_t rbrs             :   1; /*!< BRS flag of last received CAN FD Message */
        uint32_t rfdf             :   1; /*!< Received a CAN FD Message */
        uint32_t pxe              :   1; /*!< Protocol Exception Event */
        uint32_t resv0            :   1; /*!< Reserved */
        uint32_t tdcv             :   7; /*!< Transmitter Delay Compensation Value */
        uint32_t rsvd1            :   9; /*!< Reserved */
    } b;                                        /*!< Register bits. */
} psr_data_t;

/**
 * @brief  This union represents the bit fields in the Transmitter Delay Compensation Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union tdcr_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t tdcf            :   7;
        uint32_t rsvd7           :   1;
        uint32_t tdco            :   7;
        uint32_t rsvd            :   17;
    } b;                                        /*!< Register bits. */
} tdcr_data_t;

/**
 * @brief  This union represents the bit fields in the global filter configuration. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union gfc_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t rrfe            :   1; /*!< Reject all remote frames with 29-bit extended ID. */
        uint32_t rrfs            :   1; /*!< Reject all remote frames with 11-bit standard ID. */
        uint32_t anfe            :   2; /*!< Accept Non-matching frames with 29-bit extended ID.
                                            -00 Accept in Rx FIFO 0.
                                            -01 Accept in Rx FIFO 1.
                                            -10 & 11 Reject. */
        uint32_t anfs            :   2; /*!< Accept Non-matching frames with 11-bit standard ID.
                                            -00 Accept in Rx FIFO 0.
                                            -01 Accept in Rx FIFO 1.
                                            -10 & 11 Reject. */
        uint32_t reserved        :  26; /*!< Reserved. */
    } b;                                        /*!< Register bits. */
} gfc_data_t;

/**
 * @brief  This union represents the bit fields in the standard id filter configuration. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union sidfc_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t reserved0       :   2; /*!< Reserved. */
        uint32_t flssa           :  14; /*!< Start address of standard Message Id filter. */
        uint32_t lss             :   8; /*!< Filter list size. The maximum value is 128. */
        uint32_t reserved1       :   8; /*!< Reserved. */
    } b;                                        /*!< Register bits. */
} sidfc_data_t;

/**
 * @brief  This union represents the bit fields in the extended id filter configuration. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union xidfc_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t reserved0       :   2; /*!< Reserved. */
        uint32_t flesa           :  14; /*!< Start address of standard Message Id filter. */
        uint32_t lse             :   7; /*!< Filter list size. The maximum value is 64. */
        uint32_t reserved1       :   9; /*!< Reserved. */
    } b;                                        /*!< Register bits. */
} xidfc_data_t;

/**
 * @brief  This union represents the bit fields in the High priority message status. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union hpms_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t bidx            :   6; /*!< Index of Rx FIFO element to which the message was stored. Only valid when
                                             MSI = 10 or MSO = 11. */
        uint32_t msi             :   2; /*!< Message Storage Indicator.
                                            -00 No FIFO selected.
                                            -01 FIFO message lost.
                                            -10 Message stored in FIFO 0.
                                            -11 Message stored in FIFO 1. */
        uint32_t fidx            :   7; /*!< Index of matching filter element. */
        uint32_t flst            :   1; /*!< Indicates the filter list of the matching filter element. */
        uint32_t reserved        :  16; /*!< Reserved. */
    } b;                                        /*!< Register bits. */
} hpms_data_t;

/**
 * @brief  This union represents the bit fields in the Rx FIFO configuration. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union rxfc_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t reserved0       :   2; /*!< Reserved. */
        uint32_t fsa             :  14; /*!< Rx FIFO start address. */
        uint32_t fs              :   7; /*!< Number of Rx FIFO elements. The maximum value is 64 */
        uint32_t reserved1       :   1; /*!< Reserved. */
        uint32_t fwm             :   7; /*!< Level for Rx FIFO watermark interrupt. */
        uint32_t fom             :   1; /*!< FIFO Opreation Mode
                                            -0 FIFO blocking mode. When an Rx FIFO full condition is reached
                                            (RXFnS.FnPI = RXFnS.FnGI), no further messages are written to the
                                            corresponding Rx FIFO until at least one message has been
                                            read out and the Rx FIFO Get Index has been incremented.  */
    } b;                                        /*!< Register bits. */
} rxfc_data_t;

/**
 * @brief  This union represents the bit fields in the Rx FIFO Status. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union rxfs_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t ffl             :   7; /*!< Number of elements stored in Rx FIFO. */
        uint32_t reserved0       :   1; /*!< Reserved. */
        uint32_t fgi             :   6; /*!< Rx FIFO read index pointer. */
        uint32_t reserved1       :   2; /*!< Reserved. */
        uint32_t fpi             :   6; /*!< Rx FIFO write index pointer. */
        uint32_t reserved2       :   2; /*!< Reserved. */
        uint32_t ff              :   1; /*!< Rx FIFO full. */
        uint32_t rfl             :   1; /*!< A copy of interrupt flag IR.RFxL.
                                             When IR.RFxL is reset, this bit is also reset. */
        uint32_t reserved3       :   6; /*!< Reserved. */
    } b;                                        /*!< Register bits. */
} rxfs_data_t;

/**
 * @brief  This union represents the bit fields in the Rx FIFO acknowledge. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union rxfa_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t fai             :   6; /*!< After the Host has read a message or a sequence of
                                             messages from Rx FIFO it has to write the buffer
                                             index of the last element read from Rx FIFO to FAI. */
        uint32_t reserved        :  26; /*!< Reserved. */
    } b;                                        /*!< Register bits. */
} rxfa_data_t;

/**
 * @brief  This union represents the bit fields in the Rx buffer configuration. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union rxbc_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t reserved0       :   2; /*!< Reserved. */
        uint32_t rbsa            :  14; /*!< Rx Buffer Start Address. */
        uint32_t reserved1       :  16; /*!< Reserved. */
    } b;                                        /*!< Register bits. */
} rxbc_data_t;

/**
 * @brief  This union represents the bit fields in the Rx buffer/FIFO element suze configuration. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union rxesc_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t f0ds            :   3; /*!< Rx FIFO 0 Data Field Size.
                                            -000 8 byte data field.
                                            -001 12 byte data field.
                                            -010 16 byte data field.
                                            -011 20 byte data field.
                                            -100 24 byte data field.
                                            -101 32 byte data field.
                                            -110 48 byte data field.
                                            -111 64 byte data field. */
        uint32_t reserved0       :   1; /*!< Reserved. */
        uint32_t f1ds            :   3; /*!< Rx FIFO 1 Data Field Size.
                                            -000 8 byte data field.
                                            -001 12 byte data field.
                                            -010 16 byte data field.
                                            -011 20 byte data field.
                                            -100 24 byte data field.
                                            -101 32 byte data field.
                                            -110 48 byte data field.
                                            -111 64 byte data field. */
        uint32_t reserved1       :   1; /*!< Reserved. */
        uint32_t rbds            :   3; /*!< Rx buffer Data Field Size.
                                            -000 8 byte data field.
                                            -001 12 byte data field.
                                            -010 16 byte data field.
                                            -011 20 byte data field.
                                            -100 24 byte data field.
                                            -101 32 byte data field.
                                            -110 48 byte data field.
                                            -111 64 byte data field. */
        uint32_t reserved2       :  21; /*!< Reserved. */
    } b;                                        /*!< Register bits. */
} rxesc_data_t;

/**
 * @brief  This union represents the bit fields in the Tx Buffer configuration. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union txbc_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t reserved0       :   2; /*!< Reserved. */
        uint32_t tbsa            :  14; /*!< Tx buffers start address. */
        uint32_t ndtb            :   6; /*!< Number of Dedicated Transmit Buffers. The maximum value is 32. */
        uint32_t reserved1       :   2; /*!< Reserved. */
        uint32_t tfqs            :   6; /*!< Transmit FIFO/Queue Size. */
        uint32_t tfqm            :   1; /*!< Tx FIFO/Queue mode. */
        uint32_t reserved2       :   1; /*!< Reserved. */
    } b;                                        /*!< Register bits. */
} txbc_data_t;

/**
 * @brief  This union represents the bit fields in the Tx FIFO/Queue status. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union txfqs_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t tffl            :   6; /*!< Number of consecutive free Tx FIFO elements starting from TFGI. */
        uint32_t reserved0       :   2; /*!< Reserved. */
        uint32_t tfgi            :   5; /*!< Tx FIFO read index pointer. */
        uint32_t reserved1       :   3; /*!< Reserved. */
        uint32_t tfqpi           :   5; /*!< Tx FIFO/Queue write index pointer. */
        uint32_t tfqf            :   1; /*!< Tx FIFO/Queue Full. */
        uint32_t reserved2       :  10; /*!< Reserved. */
    } b;                                        /*!< Register bits. */
} txfqs_data_t;

/**
 * @brief  This union represents the bit fields in the Tx buffer element size. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union txesc_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t tbds            :   3; /*!< Tx Buffer Data Field Size. */
        uint32_t reserved        :  29; /*!< Reserved. */
    } b;                                        /*!< Register bits. */
} txesc_data_t;

/**
 * @brief  This union represents the bit fields in the Tx event FIFO configuration. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union txefc_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t reserved0       :   2; /*!< Reserved. */
        uint32_t efsa            :  14; /*!< Event FIFO Start Address. */
        uint32_t efs             :   6; /*!< Event FIFO size. */
        uint32_t reserved1       :   2; /*!< Reserved. */
        uint32_t efwm            :   6; /*!< Event FIFO watermark. */
        uint32_t reserved2       :   2; /*!< Reserved. */
    } b;                                        /*!< Register bits. */
} txefc_data_t;

/**
 * @brief  This union represents the bit fields in the Tx event FIFO status. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union txefs_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t effl            :   6; /*!< Number of consecutive free Tx FIFO elements starting from TFGI. */
        uint32_t reserved0       :   2; /*!< Reserved. */
        uint32_t efgi            :   5; /*!< Tx FIFO read index pointer. */
        uint32_t reserved1       :   3; /*!< Reserved. */
        uint32_t efpi            :   5; /*!< Tx FIFO/Queue write index pointer. */
        uint32_t reserved2       :   3; /*!< Reserved. */
        uint32_t eff             :   1; /*!< Tx FIFO/Queue Full. */
        uint32_t reserved3       :   6; /*!< Reserved. */
    } b;                                        /*!< Register bits. */
} txefs_data_t;

/**
 * @brief  This union represents the bit fields in the Tx event FIFO acknowledge. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union txefa_data {
    uint32_t d32;                               /*!< Raw register data. */
    struct {
        uint32_t efai            :   5; /*!< After the Host has read an element or a sequence of elements
                                             from the Tx Event FIFO it has to write the index of the last
                                             element read from Tx Event FIFO to EFAI. */
        uint32_t reserved        :  27; /*!< Reserved. */
    } b;                                        /*!< Register bits. */
} txefa_data_t;

/**
 * @brief  This union represents the bit fields in the Rx Buffer and FIFO Element config. \n
 *         Read the SRAM into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union rx_element_config {
    uint64_t d64;                               /*!< Raw SRAM data. */
    struct {
        uint32_t extend_id       :  18; /*!< Region to be used for extended identifier. */
        uint32_t standard_id     :  11; /*!< Region to be used for extended identifier. */
        uint32_t rtr             :   1; /*!< Remote transmission request. */
        uint32_t xtd             :   1; /*!< Extended identifier flag. */
        uint32_t esi             :   1; /*!< Error type of Transmitting node. */
        uint32_t rxts            :  16; /*!< Timestamp Counter value captured on start of frame reception. */
        uint32_t dlc             :   4; /*!< Data length. In the CAN protocol, a value greater than 8 is considered 8.
                                             -0~8 0~8 bytes, valid on both CAN and CAN-FD frames.
                                             -9  12 data bytes.
                                             -10 16 data bytes.
                                             -11 20 data bytes.
                                             -12 24 data bytes.
                                             -13 32 data bytes.
                                             -14 48 data bytes.
                                             -15 64 data bytes. */
        uint32_t brs             :   1; /*!< Bit rate switch flag. */
        uint32_t fdf             :   1; /*!< FD format flag. */
        uint32_t fidx            :   7; /*!< Index of matching Rx acceptance filter element. */
        uint32_t anmf            :   1; /*!< Accepted Non-maching Frame. */
    } b;                                        /*!< SRAM bits. */
} rx_element_config_t;

/**
 * @brief  Rx FIFO Element.
 */
typedef struct rx_fifo_data {
    rx_element_config_t config;
    uint8_t data[ELEMENT_MAXIMUM_SIZE];
} rx_fifo_data_t;

/**
 * @brief  This union represents the bit fields in the Tx Buffer Element config. \n
 *         Read the SRAM into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union tx_element_config {
    uint64_t d64;                               /*!< Raw SRAM data. */
    struct {
        uint32_t extend_id       :  18; /*!< Region to be used for extended identifier. */
        uint32_t standard_id     :  11; /*!< Region to be used for extended identifier. */
        uint32_t rtr             :   1; /*!< Remote transmission request. */
        uint32_t xtd             :   1; /*!< Extended identifier flag. */
        uint32_t esi             :   1; /*!< Error type of Transmitting node. */
        uint32_t reserved        :   8; /*!< Reserved. */
        uint32_t mmh             :   8; /*!< High byte of Wide Message Marker, written by CPU . */
        uint32_t dlc             :   4; /*!< Data length. In the CAN protocol, a value greater than 8 is considered 8.
                                             -0~8 0~8 bytes, valid on both CAN and CAN-FD frames.
                                             -9  12 data bytes.
                                             -10 16 data bytes.
                                             -11 20 data bytes.
                                             -12 24 data bytes.
                                             -13 32 data bytes.
                                             -14 48 data bytes.
                                             -15 64 data bytes. */
        uint32_t brs             :   1; /*!< Bit rate switch flag. */
        uint32_t fdf             :   1; /*!< FD format flag. */
        uint32_t tsce            :   1; /*!< Time Stamp Capture Enable for TSU. */
        uint32_t efc             :   1; /*!< Event FIFO Control. Set this bit to enable event recording. */
        uint32_t mml             :   8; /*!< Written by CPU during Tx Buffer configuration.  */
    } b;                                        /*!< SRAM bits. */
} tx_element_config_t;

/**
 * @brief  Tx Buffer Element.
 */
typedef struct tx_buffer_data {
    tx_element_config_t config;
    uint8_t data[ELEMENT_MAXIMUM_SIZE];
} tx_buffer_data_t;

/**
 * @brief  This union represents the bit fields in the Tx event FIFO Element data. \n
 *         Read the SRAM into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union tx_event_data {
    uint64_t d64;                               /*!< Raw SRAM data. */
    struct {
        uint32_t extend_id       :  18; /*!< Region to be used for extended identifier. */
        uint32_t standard_id     :  11; /*!< Region to be used for extended identifier. */
        uint32_t rtr             :   1; /*!< Remote transmission request. */
        uint32_t xtd             :   1; /*!< Extended identifier flag. */
        uint32_t esi             :   1; /*!< Error type of Transmitting node. */
        uint32_t txts            :  16; /*!< Timestamp Counter value captured on start of frame transmission.  */
        uint32_t dlc             :   4; /*!< Data length. In the CAN protocol, a value greater than 8 is considered 8.
                                             -0~8 0~8 bytes, valid on both CAN and CAN-FD frames.
                                             -9  12 data bytes.
                                             -10 16 data bytes.
                                             -11 20 data bytes.
                                             -12 24 data bytes.
                                             -13 32 data bytes.
                                             -14 48 data bytes.
                                             -15 64 data bytes. */
        uint32_t brs             :   1; /*!< Bit rate switch flag. */
        uint32_t fdf             :   1; /*!< FD format flag. */
        uint32_t et              :   2; /*!< Event type
                                            -00 Reserved.
                                            -01 Tx Event.
                                            -10 Transmission in spite of cancellation.
                                            -11 Reserved. */
        uint32_t mm              :   8; /*!< Written by CPU during Tx Buffer configuration. */
    } b;                                        /*!< SRAM bits. */
} tx_event_data_t;

/**
 * @brief  This union represents the bit fields in the Standard ID filter config. \n
 *         Read the SRAM into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union standard_filter_config {
    uint32_t d32;                               /*!< Raw SRAM data. */
    struct {
        uint32_t sfid2           :  11; /*!< Second ID of standard ID filter element. */
        uint32_t reserved        :   4; /*!< Reserved. */
        uint32_t ssync           :   1; /*!< Standard Sync Message. valued when CCCR.UTSU = 1. */
        uint32_t sfid1           :  11; /*!< First ID of standard ID filter element. */
        uint32_t sfec            :   3; /*!< Standard Filter Element Configuration.
                                            -000 Disable filter element.
                                            -001 Store in Rx FIFO 0.
                                            -010 Store in Rx FIFO 1.
                                            -011 Reject if filter matches.
                                            -100 Reject if filter matches. Set priority.
                                            -101 Set priority and store in Rx FIFO 0.
                                            -110 Set priority and store in Rx FIFO 1.
                                            -111 Store into Rx Buffer. */
        uint32_t sft             :   2; /*!< Standard filter type.
                                            -00 Range filter from SFID1 to SFID2.
                                            -01 Dual ID filter for SFID1 or SFID2.
                                            -10 Classic Filter.
                                            -11 Disabled. */
    } b;                                        /*!< SRAM bits. */
} standard_filter_config_t;

/**
 * @brief  This union represents the bit fields in the Extend ID filter config. \n
 *         Read the SRAM into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union extend_filter_config {
    uint64_t d64;                               /*!< Raw SRAM data. */
    struct {
        uint32_t efid1           :  29; /*!< First ID of extend ID filter element. */
        uint32_t efec            :   3; /*!< Extend Filter Element Configuration.
                                            -000 Disable filter element.
                                            -001 Store in Rx FIFO 0.
                                            -010 Store in Rx FIFO 1.
                                            -011 Reject if filter matches.
                                            -100 Reject if filter matches. Set priority.
                                            -101 Set priority and store in Rx FIFO 0.
                                            -110 Set priority and store in Rx FIFO 1.
                                            -111 Store into Rx Buffer. */
        uint32_t efid2           :  29; /*!< First ID of extend ID filter element. */
        uint32_t esync           :   1; /*!< Extend Sync Message. valued when CCCR.UTSU = 1. */
        uint32_t eft             :   2; /*!< Extend filter type.
                                            -00 Range filter from SFID1 to SFID2.
                                            -01 Dual ID filter for SFID1 or SFID2.
                                            -10 Classic Filter.
                                            -11 Disabled. */
    } b;                                        /*!< SRAM bits. */
} extend_filter_config_t;

/**
 * @brief  Registers associated with CAN-FD.
 */
typedef struct can_fd_regs {
    volatile uint32_t rsvd_0_8[3];              /*!< Reserved: 0x00 ~ 0x08. */
    volatile uint32_t dbtp;                     /*!< Data Bit Timing & Prescaler Register. <i>Offset: 0x00C</i>. */
    volatile uint32_t test;                     /*!< Test register. <i>Offset: 0x010</i>. */
    volatile uint32_t rwd;                      /*!< RAM Watchdog register. <i>Offset: 0x014</i>. */
    volatile uint32_t cccr;                     /*!< CC Control register. <i>Offset: 0x018</i>. */
    volatile uint32_t nbtp;                     /*!< Nominal bit timing & prescaler register. <i>Offset: 0x01C</i>. */
    volatile uint32_t tscc;                     /*!< Timestamp counter configuration register. <i>Offset: 0x020</i>. */
    volatile uint32_t tscv;                     /*!< Timestamp counter value register. <i>Offset: 0x024</i>. */
    volatile uint32_t tocc;                     /*!< Timeout counter configuration register. <i>Offset: 0x028</i>. */
    volatile uint32_t tocv;                     /*!< Timeout counter value register. <i>Offset: 0x02C</i>. */
    volatile uint32_t rsvd_30_3c[4];            /*!< Reserved: 0x30 ~ 0x3C. */
    volatile uint32_t ecr;                      /*!< Error counter register. <i>Offset: 0x040</i>. */
    volatile uint32_t psr;                      /*!< Protocol status register. <i>Offset: 0x044</i>. */
    volatile uint32_t tdcr;                     /*!< Transmitter delay compensation register. <i>Offset: 0x048</i>. */
    volatile uint32_t rsvd_4c;               /*!< Reserved: 0x4C. */
    volatile uint32_t ir;                       /*!< Interrupt register. <i>Offset: 0x050</i>. */
    volatile uint32_t ie;                       /*!< Interrupt enable register. <i>Offset: 0x054</i>. */
    volatile uint32_t ils;                      /*!< Interrupt line select register. <i>Offset: 0x058</i>. */
    volatile uint32_t ile;                      /*!< Interrupt line enable register. <i>Offset: 0x05C</i>. */
    volatile uint32_t rsvd_60_7c[8];            /*!< Reserved: 0x60 ~ 0x7C. */
    volatile uint32_t gfc;                      /*!< Global filter configuration register. <i>Offset: 0x080</i>. */
    volatile uint32_t sidfc;                    /*!< Standard filter configuration register. <i>Offset: 0x084</i>. */
    volatile uint32_t xidfc;                    /*!< extended filter configuration register. <i>Offset: 0x088</i>. */
    volatile uint32_t rsvd_8c;                  /*!< Reserved: 0x8C. */
    volatile uint32_t xidam;                    /*!< extended id and mask register. <i>Offset: 0x090</i>. */
    volatile uint32_t hpms;                     /*!< High priority message status register. <i>Offset: 0x094</i>. */
    volatile uint32_t ndat1;                    /*!< New data 1 register. <i>Offset: 0x098</i>. */
    volatile uint32_t ndat2;                    /*!< New data 2 register. <i>Offset: 0x09C</i>. */
    volatile uint32_t rxf0c;                    /*!< Rx FIFO 0 Configuration register . <i>Offset: 0x0A0</i>. */
    volatile uint32_t rxf0s;                    /*!< Rx FIFO 0 status register. <i>Offset: 0x0A4</i>. */
    volatile uint32_t rxf0a;                    /*!< Rx FIFO 0 acknowledge register . <i>Offset: 0x0A8</i>. */
    volatile uint32_t rxbc;                     /*!< Rx buffer configuration register. <i>Offset: 0x0AC</i>. */
    volatile uint32_t rxf1c;                    /*!< Rx FIFO 1 configuration register. <i>Offset: 0x0B0</i>. */
    volatile uint32_t rxf1s;                    /*!< Rx FIFO 1 status register. <i>Offset: 0x0B4</i>. */
    volatile uint32_t rxf1a;                    /*!< Rx FIFO 1 acknowledge register. <i>Offset: 0x0B8</i>. */
    volatile uint32_t rxesc;                    /*!< Rx element size configuration register. <i>Offset: 0x0BC</i>. */
    volatile uint32_t txbc;                     /*!< Tx buffer configuration register. <i>Offset: 0x0C0</i>. */
    volatile uint32_t txfqs;                    /*!< Tx FIFO/Queue status register. <i>Offset: 0x0C4</i>. */
    volatile uint32_t txesc;                    /*!< Tx element size configuration register. <i>Offset: 0x0C8</i>. */
    volatile uint32_t txbrp;                    /*!< Tx buffer request pending register. <i>Offset: 0x0CC</i>. */
    volatile uint32_t txbar;                    /*!< Tx buffer add request register. <i>Offset: 0x0D0</i>. */
    volatile uint32_t txbcr;                    /*!< Tx buffer cancellation request register. <i>Offset: 0x0D4</i>. */
    volatile uint32_t txbto;                    /*!< Tx buffer transmission occured register. <i>Offset: 0x0D8</i>. */
    volatile uint32_t txbcf;                    /*!< Tx buffer cancellation finished register. <i>Offset: 0x0DC</i>. */
    volatile uint32_t txbtie;                   /*!< Tx transmission interrupt register. <i>Offset: 0x0E0</i>. */
    volatile uint32_t txbcie;                   /*!< Tx cancellation interrupt register. <i>Offset: 0x0E4</i>. */
    volatile uint32_t rsvd_e8_ec[2];            /*!< Reserved : 0x0E8~ 0x0EC. */
    volatile uint32_t txefc;                    /*!< Tx event FIFO configuration register. <i>Offset: 0x0F0</i>. */
    volatile uint32_t txefs;                    /*!< Tx event FIFO status register. <i>Offset: 0x0F4</i>. */
    volatile uint32_t txefa;                    /*!< Tx event FIFO acknowledge register. <i>Offset: 0x0F8</i>. */
} can_fd_regs_t;

/**
 * @brief  SRAM associated with Tx buffer.
 */
typedef struct tx_buffer {
    volatile tx_buffer_data_t element[CONFIG_CAN_TX_BUFFER_SIZE];
} tx_buffer_t;

/**
 * @brief  SRAM associated with Tx buffer.
 */
typedef struct tx_event_buffer {
    volatile tx_event_data_t element[CONFIG_CAN_TX_BUFFER_SIZE];
} tx_event_buffer_t;


/**
 * @brief  SRAM associated with Rx FIFO 0.
 */
typedef struct rx_fifo_0 {
    volatile rx_fifo_data_t element[CONFIG_CAN_RX_FIFO0_SIZE];
} rx_fifo_0_t;

/**
 * @brief  SRAM associated with Rx FIFO 1.
 */
typedef struct rx_fifo_1 {
    volatile rx_fifo_data_t element[CONFIG_CAN_RX_FIFO1_SIZE];
} rx_fifo_1_t;

/**
 * @brief  SRAM associated with standard filter.
 */
typedef struct std_flilter_array {
    volatile uint32_t element[CONFIG_STANDARD_CAN_FILTER_NUM];
} std_filter_array_t;

/**
 * @brief  SRAM associated with standard filter.
 */
typedef struct ex_flilter_array {
    volatile uint64_t element[CONFIG_EXTENDED_CAN_FILTER_NUM];
} ex_filter_array_t;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif