/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 */
#include "string.h"
#include "stdlib.h"
#include "types.h"
#include "tcxo.h"
#include "securec.h"
#include "chip_io.h"
#include "kal_mem.h"
#include "nand_ids.h"
#include "nand_scan.h"
#include "nand_ops.h"
#include "debug_print.h"
#include "nand_common.h"
#include "hifmc_union.h"
#include "hifmc_regs.h"
#include "hifmc_cmds.h"
#include "hifmc_common.h"
#include "nand.h"
#include "hifmc100.h"

static struct nand_info *g_nand_info;
static struct nand_host *g_nand_host;

inline uintptr_t dpal_align(uintptr_t addr, uint32_t boundary)
{
    return (addr + boundary - 1) & ~((uintptr_t)(boundary - 1));
}

void dpal_free(void *ptr)
{
    if (ptr == NULL) {
        return;
    }
    free(ptr);
}

void *dpal_zalloc(size_t size)
{
    if (size == 0) {
        return NULL;
    }

    void *ptr = (void *)memalign(CACHE_ALIGNED_SIZE, size);
    if (ptr != NULL) {
        (void)memset_s(ptr, size, 0, size);
    }
    return ptr;
}

static void fmc_reg_write(uint32_t offset, uint32_t val)
{
    writel((uint32_t)(uintptr_t)g_nand_host->regbase + offset, val);
}

static uint32_t fmc_reg_read(uint32_t offset)
{
    return readl((uint32_t)(uintptr_t)g_nand_host->regbase + offset);
}

static uint8_t fmc100_read_byte(struct nand_info *nand, int32_t offset)
{
    struct nand_host *nand_host = nand->priv;
    return dpal_readb((uint8_t *)(nand_host->membase) + offset);
}

static uint16_t fmc100_read_word(struct nand_info *nand, int32_t offset)
{
    struct nand_host *nand_host = nand->priv;
    return dpal_readw(nand_host->membase + offset);
}

static void fmc100_write_buf(struct nand_info *nand, const uint8_t *buf, int32_t len, int32_t offset)
{
    struct nand_host *nand_host = nand->priv;
    struct nand_flash_info *nand_dev = (struct nand_flash_info *)(nand->dev.priv);
    memset_s((void *)(nand_host->dma_data + nand_host->pagesize), nand_host->oobsize, 0xff, nand_host->oobsize);
    memcpy_s((void *)(nand_host->dma_data + offset), len, (void *)buf, len);
    if (offset >= (int32_t)nand_host->pagesize && nand_dev->oob_format != NULL) {
        nand_dev->oob_format(1, nand_host->dma_oob, nand_host->oobsize);
    }
}

static void fmc100_read_buf(struct nand_info *nand, const uint8_t *buf, int32_t len, int32_t offset)
{
    struct nand_host *nand_host = nand->priv;
    struct nand_flash_info *nand_dev = (struct nand_flash_info *)(nand->dev.priv);
    if (offset >= (int32_t)nand_host->pagesize && nand_dev->oob_format != NULL) {
        nand_dev->oob_format(0, nand_host->dma_oob, nand_host->oobsize);
    }
    memcpy_s((void *)buf, len, (void *)(nand_host->dma_data + offset), len);
}

static int32_t fmc_status_check(int32_t flag)
{
    reg_fmc_op fmc_op;
    reg_fmc_op_ctrl fmc_op_ctrl;

    for (int32_t i = 0; i < HIFMC_TIME_OUT; i++) {
        if (flag == HIFMC_DMA) {
            fmc_op_ctrl.value = fmc_reg_read(FMC_OP_CTRL);
            if (fmc_op_ctrl.bits.dma_op_ready == 0x0) {
                return NAND_STATUS_SUCCESS;
            }
        } else {
            fmc_op.value = fmc_reg_read(FMC_OP);
            if (fmc_op.bits.reg_op_start == 0x0) {
                return NAND_STATUS_SUCCESS;
            }
        }
        uapi_tcxo_delay_us(5);  /* 5us */
    }

    return NAND_STATUS_FAIL;
}

static int32_t fmc100_reset(struct nand_info *nand)
{
    reg_fmc_cmd fmc_cmd;
    reg_fmc_op_cfg  fmc_op_cfg;
    reg_fmc_op fmc_op;

    fmc_cmd.value = 0;
    fmc_cmd.bits.cmd1 = RESET;
    fmc_reg_write(FMC_CMD, fmc_cmd.value);

    fmc_op_cfg.value = 0;
    fmc_op_cfg.bits.fm_cs = nand->cur_cs;
    fmc_reg_write(FMC_OP_CFG, fmc_op_cfg.value);

    fmc_op.value = 0;
    fmc_op.bits.reg_op_start = 0x1;
    fmc_op.bits.cmd1_en = 0x1;
    fmc_reg_write(FMC_OP, fmc_op.value);

    return fmc_status_check(HIFMC_REG);
}

static int32_t fmc100_erase(struct nand_info *nand, uint32_t page)
{
    reg_fmc_cmd fmc_cmd;
    reg_fmc_addrh fmc_addrh;
    reg_fmc_addrl fmc_addrl;
    reg_fmc_op_cfg fmc_op_cfg;
    reg_fmc_op fmc_op;

    fmc_cmd.value = 0;
    fmc_cmd.bits.cmd1 = WRITE_ENABLE;
    fmc_reg_write(FMC_CMD, fmc_cmd.value);

    fmc_op.value = 0;
    fmc_op.bits.reg_op_start = 0x1;
    fmc_op.bits.cmd1_en = 0x1;
    fmc_reg_write(FMC_OP, fmc_op.value);

    int32_t ret = fmc_status_check(HIFMC_REG);
    if (ret) {
        PRINT("fmc erase fail!\n");
        return ret;
    }

    fmc_cmd.value = 0;
    fmc_cmd.bits.cmd1 = BLOCK_ERASE;
    fmc_reg_write(FMC_CMD, fmc_cmd.value);

    fmc_addrh.value = 0x0;
    fmc_reg_write(FMC_ADDRH, fmc_addrh.value);

    fmc_addrl.value = 0;
    fmc_addrl.bits.addrl = page;
    fmc_reg_write(FMC_ADDRL, fmc_addrl.value);

    fmc_op_cfg.value = 0;
    fmc_op_cfg.bits.addr_num = 0x3;
    fmc_op_cfg.bits.fm_cs = nand->cur_cs;
    fmc_reg_write(FMC_OP_CFG, fmc_op_cfg.value);

    fmc_op.value = 0;
    fmc_op.bits.reg_op_start = 0x1;
    fmc_op.bits.addr_en = 0x1;
    fmc_op.bits.cmd1_en = 0x1;
    fmc_reg_write(FMC_OP, fmc_op.value);

    ret = fmc_status_check(HIFMC_REG);
    if (ret) {
        PRINT("fmc erase fail!\n");
        return ret;
    }

    return ret;
}

static int32_t fmc100_dma_transfer(struct nand_info *nand, uint32_t page, uint32_t cmd)
{
    struct nand_host *nand_host = nand->priv;
    reg_fmc_addrh fmc_addrh;
    reg_fmc_addrl fmc_addrl;
    reg_fmc_dma_saddr_d0 fmc_dma_saddr_d0;
    reg_fmc_dma_saddr_oob fmc_dma_saddr_oob;
    reg_fmc_dma_len fmc_dma_len;
    reg_fmc_op_cfg fmc_op_cfg;
    reg_fmc_op_ctrl fmc_op_ctrl;
    uint32_t page_addrh = 0x0;
    uint32_t page_addrl = page;

    if (page > 0xFFFF) {
        page_addrh = (page & 0xFFFF0000);
        page_addrl = (page & 0xFFFF);
    }

    fmc_addrh.value = 0x0;
    fmc_addrh.bits.addrh = (page_addrh >> 16); // right 16
    fmc_reg_write(FMC_ADDRH, fmc_addrh.value);

    fmc_addrl.value = 0;
    fmc_addrl.bits.addrl = (page_addrl << 16); // left 16
    fmc_reg_write(FMC_ADDRL, fmc_addrl.value);

    fmc_dma_saddr_d0.value = 0;
    fmc_dma_saddr_d0.bits.dma_mem_saddr_d0 = (uint32_t)(uintptr_t)nand_host->dma_data;
    fmc_reg_write(FMC_DMA_SADDR_D0, fmc_dma_saddr_d0.value);

    fmc_dma_saddr_oob.value = 0;
    fmc_dma_saddr_oob.bits.dma_mem_saddr_oob = (uint32_t)(uintptr_t)nand_host->dma_oob;
    fmc_reg_write(FMC_DMA_SADDR_OOB, fmc_dma_saddr_oob.value);

    if (nand->dev.ecctype == NAND_ECC_0BIT) {
        fmc_dma_len.value = 0;
        fmc_dma_len.bits.dma_len = nand->dev.oobsize;
        fmc_reg_write(FMC_DMA_LEN, fmc_dma_len.value);
    }

    fmc_op_cfg.value = 0;
    if ((cmd == READ_FROM_CACHE4) || (cmd == READ_FROM_CACHE)) {
        fmc_op_cfg.bits.dummy_num = 0x1;
    }
    fmc_op_cfg.bits.addr_num = 0x2;
    fmc_op_cfg.bits.fm_cs = nand->cur_cs;
    fmc_op_cfg.bits.mem_if_type = 0x3; /* PROGRAM_LOAD4/READ_FROM_CACHE4 */
    if ((cmd == PROGRAM_LOAD) || (cmd == READ_FROM_CACHE)) {
        fmc_op_cfg.bits.mem_if_type = 0x0;
    }
    fmc_reg_write(FMC_OP_CFG, fmc_op_cfg.value);

    fmc_op_ctrl.value = 0;
    fmc_op_ctrl.bits.dma_op_ready = 0x1;
    if ((cmd == READ_FROM_CACHE4) || (cmd == READ_FROM_CACHE)) {
        fmc_op_ctrl.bits.rd_opcode = cmd;
    } else if (cmd == PROGRAM_LOAD4 || (cmd == PROGRAM_LOAD)) {
        fmc_op_ctrl.bits.rw_op = 0x1;
        fmc_op_ctrl.bits.wr_opcode = cmd;
    }
    fmc_reg_write(FMC_OP_CTRL, fmc_op_ctrl.value);

    return fmc_status_check(HIFMC_DMA);
}

static int32_t fmc100_program(struct nand_info *nand, uint32_t page)
{
    return fmc100_dma_transfer(nand, page, PROGRAM_LOAD4);
}

static int32_t fmc100_read(struct nand_info *nand, uint32_t page)
{
    return fmc100_dma_transfer(nand, page, READ_FROM_CACHE4);
}

static int32_t fmc100_feature_op(struct nand_info *nand, uint32_t cmd,
    uint8_t addr, uint8_t w_val, uint8_t *r_val)
{
    struct nand_host *nand_host = nand->priv;
    reg_fmc_cmd fmc_cmd;
    reg_fmc_addrh fmc_addrh;
    reg_fmc_addrl fmc_addrl;
    reg_fmc_op_cfg fmc_op_cfg;
    reg_fmc_data_num fmc_data_num;
    reg_fmc_op fmc_op;
    int32_t ret;

    fmc_cmd.value = 0;
    fmc_cmd.bits.cmd1 = cmd;
    fmc_reg_write(FMC_CMD, fmc_cmd.value);
    fmc_addrh.value = 0x0;
    fmc_reg_write(FMC_ADDRH, fmc_addrh.value);

    fmc_addrl.value = 0;
    fmc_addrl.bits.addrl = addr;
    fmc_reg_write(FMC_ADDRL, fmc_addrl.value);

    fmc_op_cfg.value = 0;
    fmc_op_cfg.bits.addr_num = 0x1;
    fmc_op_cfg.bits.fm_cs = nand->cur_cs;
    fmc_reg_write(FMC_OP_CFG, fmc_op_cfg.value);

    fmc_data_num.value = 0;
    fmc_data_num.bits.op_data_num = 0x1;
    fmc_reg_write(FMC_DATA_NUM, fmc_data_num.value);

    fmc_op.value = 0;
    fmc_op.bits.reg_op_start = 0x1;
    fmc_op.bits.addr_en = 0x1;
    fmc_op.bits.cmd1_en = 0x1;
    if (cmd == SET_FEATURE) {
        fmc_op.bits.write_data_en = 0x1;
        writeb((uint32_t)(uintptr_t)nand_host->membase, w_val);
    } else {
        fmc_op.bits.read_data_en = 0x1;
    }
    fmc_reg_write(FMC_OP, fmc_op.value);
    ret = fmc_status_check(HIFMC_REG);
    if (ret) {
        PRINT("fmc op fail!\n");
        return ret;
    }

    if (cmd == GET_FEATURE) {
        *r_val = readb((uint32_t)(uintptr_t)nand_host->membase);
    }

    return ret;
}

static int32_t fmc100_read_status(struct nand_info *nand)
{
    uint8_t status;
    int32_t ret;

    for (int32_t i = 0; i < HIFMC_TIME_OUT; i++) {
        nand->feature_op(nand, GET_FEATURE, DEVICE_STATUS, 0, &status);
        status = status & 0xff;
        switch (nand->op_state) {
            case NAND_PROGING:
                ret = ((status & STATUS_OIP_MASK) || (status & STATUS_P_FAIL_MASK)) ? \
                NAND_STATUS_FAIL : NAND_STATUS_SUCCESS;
                break;
            case NAND_ERASING:
                ret = ((status & STATUS_OIP_MASK) || (status & STATUS_E_FAIL_MASK)) ? \
                NAND_STATUS_FAIL : NAND_STATUS_SUCCESS;
                break;
            case NAND_READING:
                ret = (status & STATUS_OIP_MASK) ? NAND_STATUS_FAIL : NAND_STATUS_SUCCESS;
                break;
            default:
                ret = NAND_STATUS_SUCCESS;
        }
        uapi_tcxo_delay_us(5); /* 5us */
        if (ret == NAND_STATUS_SUCCESS) {
            return ret;
        }
    }
    return NAND_STATUS_FAIL;
}

static int32_t fmc100_read_id(struct nand_info *nand, uint8_t *id)
{
    struct nand_host *nand_host = nand->priv;
    reg_fmc_cmd fmc_cmd;
    reg_fmc_data_num fmc_data_num;
    reg_fmc_addrl fmc_addrl;
    reg_fmc_op_cfg  fmc_op_cfg;
    reg_fmc_op fmc_op;
    int32_t ret;

    memset_s((uint8_t *)(nand_host->membase), MAX_SPI_NAND_ID_LEN, 0, MAX_SPI_NAND_ID_LEN);

    fmc_cmd.value = 0;
    fmc_cmd.bits.cmd1 = READ_ID;
    fmc_reg_write(FMC_CMD, fmc_cmd.value);

    fmc_data_num.value = 0;
    fmc_data_num.bits.op_data_num = MAX_SPI_NAND_ID_LEN;
    fmc_reg_write(FMC_DATA_NUM, fmc_data_num.value);

    fmc_addrl.value = 0;
    fmc_addrl.bits.addrl = 0x0;
    fmc_reg_write(FMC_ADDRL, fmc_addrl.value);

    fmc_op_cfg.value = 0;
    fmc_op_cfg.bits.dummy_num = 0x1;
    fmc_op_cfg.bits.fm_cs = nand->cur_cs;
    fmc_reg_write(FMC_OP_CFG, fmc_op_cfg.value);

    fmc_op.value = 0;
    fmc_op.bits.reg_op_start = 0x1;
    fmc_op.bits.read_data_en = 0x1;
    fmc_op.bits.cmd1_en = 0x1;
    fmc_op.bits.dummy_en = 0x1;
    fmc_reg_write(FMC_OP, fmc_op.value);

    ret = fmc_status_check(HIFMC_REG);
    if (ret) {
        PRINT("fmc read id fail!\n");
        return ret;
    }

    memcpy_s((void *)id, MAX_SPI_NAND_ID_LEN, (void *)nand_host->membase, MAX_SPI_NAND_ID_LEN);

    return ret;
}

static char nand_tmp_buffer[0x1200] = {0}; // can not malloc 0x1080 use global mem
static int32_t fmc100_oob_resize(struct nand_info *nand)
{
    uint32_t reg;
    reg_fmc_cfg fmc_cfg;
    struct nand_host *nand_host = nand->priv;

    reg = fmc_reg_read(FMC_CFG);
    PRINT("fmc_cfg = 0x%x.\n", reg);

    fmc_cfg.value = reg;
    fmc_cfg.bits.page_size = (uint32_t)nandpage_size2type(nand->dev.pagesize);
    fmc_cfg.bits.block_size = (uint32_t)nandblock_size2type(nand->dev.blocksize / nand->dev.pagesize);
    fmc_reg_write(FMC_CFG, fmc_cfg.value);
    PRINT("fmc_cfg = 0x%x.\n", fmc_cfg.value);
    nand_host->cfg = fmc_cfg.value;
    nand_host->oobsize = nand->dev.oobsize;
    nand_host->pagesize = nand->dev.pagesize;
    nand_host->buforg = nand_tmp_buffer;
    nand_host->buffer = nand_host->buforg;
    nand_host->dma_data = nand_host->buffer;
    nand_host->dma_oob = nand_host->dma_data + nand_host->pagesize;
    nand_host->bbm = (nand_host->buffer + nand_host->pagesize + NAND_BAD_BLOCK_POS);
    return 0;
}

int spinand_general_qe_enable(struct spi *spi)
{
    uint8_t value = 0;
    struct hifmc_host *fmc_host = (struct hifmc_host *)(spi->host);
    struct nand_host *nand_host = (struct nand_host *)(fmc_host->host);
    struct nand_info *nand = nand_host->nand;

    int32_t ret = nand->feature_op(nand, GET_FEATURE, DEVICE_OTP, 0x0, &value);
    unused(ret);
    PRINT("get [0x%x]feature: 0x%x ret = %d\n", DEVICE_OTP, value, ret);

    value |= FEATURE_QE_ENABLE;
    ret = nand->feature_op(nand, SET_FEATURE, DEVICE_OTP, value, NULL);
    PRINT("set [0x%x]feature: 0x%x ret = %d\n", DEVICE_OTP, value, ret);
    
    ret = nand->feature_op(nand, GET_FEATURE, DEVICE_OTP, 0x0, &value);
    PRINT("get [0x%x]feature: 0x%x ret = %d\n", DEVICE_OTP, value, ret);
    return 0;
}

int spinand_buf_enable(struct spi *spi)
{
    uint8_t value = 0;
    struct hifmc_host *fmc_host = (struct hifmc_host *)(spi->host);
    struct nand_host *nand_host = (struct nand_host *)(fmc_host->host);
    struct nand_info *nand = nand_host->nand;

    int32_t ret = nand->feature_op(nand, GET_FEATURE, DEVICE_OTP, 0x0, &value);
    unused(ret);
    PRINT("get [0x%x]feature: 0x%x ret = %d\n", DEVICE_OTP, value, ret);

    value = 0x8;  // 0x8 enable buf
    ret = nand->feature_op(nand, SET_FEATURE, DEVICE_OTP, value, NULL);
    PRINT("set [0x%x]feature: 0x%x ret = %d\n", DEVICE_OTP, value, ret);

    ret = nand->feature_op(nand, GET_FEATURE, DEVICE_OTP, 0x0, &value);
    PRINT("get [0x%x]feature: 0x%x ret = %d\n", DEVICE_OTP, value, ret);
    return 0;
}

static void fmc100_ids_probe(struct nand_info *nand)
{
    struct nand_host *nand_host = nand->priv;
    struct hifmc_host *fmc_host = nand_host->priv;
    struct spi *spi = fmc_host->spi;
    struct nand_flash_info *spi_dev = (struct nand_flash_info *)(nand->dev.priv);

    spi->host = fmc_host;
    spi->name = spi_dev->name;
    spi->driver = spi_dev->driver;
    spi->driver->qe_enable(spi);

    /* Disable write protection */
    uint8_t value = 0;
    int32_t ret = nand->feature_op(nand, GET_FEATURE, DEVICE_BLOCK_LOCK, 0, &value);
    unused(ret);
    PRINT("get [0x%x]feature: 0x%x ret = %d\n", DEVICE_BLOCK_LOCK, value, ret);

    value = 0;
    ret = nand->feature_op(nand, SET_FEATURE, DEVICE_BLOCK_LOCK, value, NULL);
    PRINT("set [0x%x]feature: 0x%x ret = %d\n", DEVICE_BLOCK_LOCK, value, ret);

    ret = nand->feature_op(nand, GET_FEATURE, DEVICE_BLOCK_LOCK, 0x0, &value);
    PRINT("get [0x%x]feature: 0x%x ret = %d\n", DEVICE_BLOCK_LOCK, value, ret);
}

static void fmc100_nand_init(struct nand_info *nand)
{
    nand->erase = fmc100_erase;
    nand->program = fmc100_program;
    nand->read = fmc100_read;

    nand->read_id = fmc100_read_id;
    nand->read_status = fmc100_read_status;
    nand->reset = fmc100_reset;
    nand->feature_op = fmc100_feature_op;
    nand->ids_probe = fmc100_ids_probe;

    nand->read_byte = fmc100_read_byte;
    nand->read_word = fmc100_read_word;
    nand->read_buf = fmc100_read_buf;
    nand->write_buf = fmc100_write_buf;

    nand->oob_resize = fmc100_oob_resize;

    nand->cur_cs = HIFMC100_NAND_CS_NUM;
}

static int32_t fmc100_host_init(struct nand_host *nand_host)
{
    reg_fmc_cfg fmc_cfg;

    struct hifmc_host *fmc_host = dpal_zalloc(sizeof(struct hifmc_host));
    if (!fmc_host) {
        PRINT("no mem for hifmc_host!\n");
        return -1;
    }
    fmc_host->host = nand_host;
    nand_host->priv = fmc_host;

    fmc_cfg.value = 0;
    fmc_cfg.bits.op_mode = 0x1;
    fmc_cfg.bits.flash_sel = 0x1;
    fmc_cfg.bits.spi_nand_sel = 0x3;        /* enable plane select */
    fmc_reg_write(FMC_CFG, fmc_cfg.value);
    return 0;
}

static int32_t fmc100_resource_build(struct dpal_platform_device *dev)
{
    struct dpal_resource *res = NULL;

    g_nand_mtd = (struct mtd_info *)dpal_zalloc(sizeof(struct mtd_info));
    if (!g_nand_mtd) {
        PRINT("no mem for mtd_info!\n");
        return -1;
    }
    /* nand chip init */
    g_nand_info = (struct nand_info *)dpal_zalloc(sizeof(struct nand_info));
    if (!g_nand_info) {
        PRINT("no mem for nand_info!\n");
        goto err;
    }
    g_nand_mtd->priv = g_nand_info;
    g_nand_host = (struct nand_host *)dpal_zalloc(sizeof(struct nand_host));
    if (!g_nand_host) {
        PRINT("no mem for nand_host!\n");
        goto err1;
    }
    res = get_hifmc100_resources(0);
    if (!res) {
        goto err2;
    }

    g_nand_host->regbase = (char *)(uintptr_t)res->start;
    PRINT("[%s] regbase=0x%x\n", __FUNCTION__, (uint32_t)(uintptr_t)g_nand_host->regbase);

    res = get_hifmc100_resources(1);
    if (!res) {
        goto err2;
    }

    g_nand_host->membase = (char *)(uintptr_t)res->start;
    PRINT("[%s] membase=0x%x\n", __FUNCTION__, (uint32_t)(uintptr_t)g_nand_host->membase);

    g_nand_host->nand = g_nand_info;
    g_nand_info->priv = g_nand_host;
    return 0;

err2:
    dpal_free(g_nand_host);
    g_nand_host = NULL;
err1:
    dpal_free(g_nand_info);
    g_nand_info = NULL;
err:
    dpal_free(g_nand_mtd);
    g_nand_mtd = NULL;
    return -1;
}

static int32_t fmc100_probe(struct dpal_platform_device *dev)
{
    if (fmc100_resource_build(dev)) {
        return -1;
    }
    fmc100_nand_init(g_nand_info);
    if (fmc100_host_init(g_nand_host)) {
        PRINT("fmc100_host_init failed\n");
        goto err;
    }
    /* scan nand device entry */
    if (nand_scan(g_nand_mtd)) {
        PRINT("nand scan fail!\n");
        goto err;
    }
    /* nand register */
    nand_register(g_nand_mtd);
    return 0;

err:
    dpal_free(g_nand_host);
    dpal_free(g_nand_info);
    dpal_free(g_nand_mtd);
    g_nand_host = NULL;
    g_nand_info = NULL;
    g_nand_mtd = NULL;

    return -1;
}

static int32_t fmc100_suspend(struct device_dpal *dev)
{
    (void)dev;

    return 0;
}

static int32_t fmc100_resume(struct device_dpal *dev)
{
    (void)dev;

    return 0;
}

static const struct dpal_dev_pm_op fmc100_dev_pm_ops = {
    .suspend = fmc100_suspend,
    .resume = fmc100_resume,
};

static int32_t fmc100_remove(struct dpal_platform_device *dev)
{
    (void)dev;

    return 0;
}

static struct dpal_platform_driver fmc100_driver = {
    .probe      = fmc100_probe,
    .remove     = fmc100_remove,
    .drv     = {
        .name   = "fmc100_nand",
        .pm = &fmc100_dev_pm_ops,
    },
};

int32_t nand_fmc100_init(void)
{
    return fmc100_driver.probe(get_device_hifmc100_nand());
}

void nand_fmc100_exit(void)
{
}

