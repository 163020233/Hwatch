/*-
 * Copyright (c) 2013 Ian Lepore <ian@freebsd.org>
 * Copyright (c) 2011 Ben Gray <ben.r.gray@gmail.com>.
 * All rights reserved.
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2020. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <stdbool.h>
#include "tcxo.h"
#include "debug_print.h"
#include "soc_osal.h"
#include "sdhci.h"

#define SAMP_PHASE_DEF 15
#ifdef LOFCFG_DRIVERS_MMC_CUSTOM_TUNING
#define TUNE_CORASE_MAX 4                               /* 粗调 0 2 4 6 */
#define TUNE_FINE_MAX 11                                /* 细调 【0 ~ 10】 */
#define TUNE_EDGE_MAX (TUNE_CORASE_MAX * TUNE_FINE_MAX) /* Active phase scale: 0 ~ TUNE_EDGE_PHASES-1 */
int g_tuning_result_arr[TUNE_EDGE_MAX] = {0};
#else
#define TUNE_EDGE_MAX 64 /* Active phase scale: 0 ~ TUNE_EDGE_PHASES-1 */
#endif
#define TUNE_EDGE_NOT_FUND TUNE_EDGE_MAX

static hci_slot_ops slot_methods;

static const uint32_t tune_blk_4bits[] = {0x00ff0fff, 0xccc3ccff, 0xffcc3cc3, 0xeffefffe, 0xddffdfff, 0xfbfffbff,
                                          0xff7fffbf, 0xefbdf777, 0xf0fff0ff, 0x3cccfc0f, 0xcfcc33cc, 0xeeffefff,
                                          0xfdfffdff, 0xffbfffdf, 0xfff7ffbb, 0xde7b7ff7};

static const uint32_t tune_blk_8bits[] = {
    0xff00ffff, 0x0000ffff, 0xccccffff, 0xcccc33cc, 0xcc3333cc, 0xffffcccc, 0xffffeeff, 0xffeeeeff,
    0xffddffff, 0xddddffff, 0xbbffffff, 0xbbffffff, 0xffffffbb, 0xffffff77, 0x77ff7777, 0xffeeddbb,
    0x00ffffff, 0x00ffffff, 0xccffff00, 0xcc33cccc, 0x3333cccc, 0xffcccccc, 0xffeeffff, 0xeeeeffff,
    0xddffffff, 0xddffffff, 0xffffffdd, 0xffffffbb, 0xffffbbbb, 0xffff77ff, 0xff7777ff, 0xeeddbb77};

typedef struct {
    device_t *dev;
    uint32_t slots;
    uint32_t init;
    struct mmc_proc_dir_entry *root;
} mmc_dev;

/* mmc_devs.init will be inited as MMC_PROC_DEFAULT */
mmc_dev mmc_devs;
struct hci_softc *mmc_sus_res[MAX_MMC_NUM];

int emmc_task_count = 0;
int sdio_task_count = 0;

static uint8_t hi_sdhci_readb(device_t dev, uint32_t off)
{
    struct hci_softc *sc = device_get_softc(dev);
    return (hci_readb(sc, off));
}

static uint16_t hi_sdhci_readw(device_t dev, uint32_t off)
{
    struct hci_softc *sc = device_get_softc(dev);
    return (hci_readw(sc, off));
}

static uint32_t hi_sdhci_readl(device_t dev, uint32_t off)
{
    struct hci_softc *sc = device_get_softc(dev);
    uint32_t val32;

    val32 = hci_readl(sc, off);
    /* Force the card-present state if necessary. */
    if (off == SDHCI_PRESENT_STATE && sc->force_card_present) {
        val32 |= SDHCI_CARD_PRESENT;
    }
    return (val32);
}

static void hi_sdhci_writeb(device_t dev, uint32_t off, uint8_t val)
{
    struct hci_softc *sc = device_get_softc(dev);
    hci_writeb(sc, off, val);
}

static void hi_sdhci_writew(device_t dev, uint32_t off, uint16_t val)
{
    struct hci_softc *sc = device_get_softc(dev);
    hci_writew(sc, off, val);
}

static void hi_sdhci_writel(device_t dev, uint32_t off, uint32_t val)
{
    struct hci_softc *sc = device_get_softc(dev);
    hci_writel(sc, off, val);
}

static int hi_sdhci_get_ro(device_t brdev, device_t reqdev)
{
    (void)reqdev;
    struct hci_softc *sc = device_get_softc(brdev);
    struct sdhci_slot *slot = (struct sdhci_slot *)sc->slot;

    return (sdhci_generic_get_ro(slot->bus, slot->dev));
}

static void hi_set_uhs_timing(device_t brdev)
{
    uint16_t host_ctrl2;
    struct hci_softc *sc = device_get_softc(brdev);
    struct sdhci_slot *slot = (struct sdhci_slot *)sc->slot;
    struct mmc_ios *ios = &slot->host.ios;

    host_ctrl2 = hci_readw(sc, SDHCI_HOST_CONTROL2);
    /* Select Bus Speed Mode for host */
    host_ctrl2 &= ~SDHCI_CTRL2_UHS_MASK;
    if ((ios->timing == bus_timing_mmc_hs200) || (ios->timing == bus_timing_uhs_sdr104))
        host_ctrl2 |= SDHCI_CTRL2_UHS_SDR104;
    else if (ios->timing == bus_timing_uhs_sdr12)
        host_ctrl2 |= SDHCI_CTRL2_UHS_SDR12;
    else if (ios->timing == bus_timing_uhs_sdr25)
        host_ctrl2 |= SDHCI_CTRL2_UHS_SDR25;
    else if (ios->timing == bus_timing_uhs_sdr50)
        host_ctrl2 |= SDHCI_CTRL2_UHS_SDR50;
    else if ((ios->timing == bus_timing_uhs_ddr50) || (ios->timing == bus_timing_mmc_ddr52))
        host_ctrl2 |= SDHCI_CTRL2_UHS_DDR50;
    else if ((ios->timing == bus_timing_mmc_hs400) || (ios->timing == bus_timing_mmc_hs400es))
        host_ctrl2 |= SDHCI_CTRL2_MMC_HS400;
    else /* Legacy mode, no need to set SDHCI_HOST_CONTROL2 */
        return;

    hci_writew(sc, SDHCI_HOST_CONTROL2, host_ctrl2);
}

void sdhci_req_wakeup(struct mmc_request *req)
{
    unused(req);
    struct sdhci_slot *slot = req->done_data;
    slot->req_event.uwEventID |= SDHCI_ACQUIRE_HOST;
}

#ifdef LOFCFG_DRIVERS_MMC_CUSTOM_TUNING
static device_t g_emmc_reqdev = NULL;

static void find_coarse_result(tune_result *t)
{
    int start = -1;
    int count = 0;
    int max_start = -1;
    int max_count = 0;
    int32_t tmp_coarse_result[TUNE_CORASE_MAX] = {0};
    for (int i = 0; i < TUNE_CORASE_MAX; i++) {
        tmp_coarse_result[i] = g_tuning_result_arr[i * TUNE_FINE_MAX];
    }
    for (int i = 0; i < TUNE_CORASE_MAX * 2; i++) {
        if (tmp_coarse_result[i % TUNE_CORASE_MAX] == 0) {
            count++;
            if (count == 1) {
                start = i % TUNE_CORASE_MAX;
            }
        } else {
            if (count > max_count) {
                max_count = count;
                max_start = start;
            }
            count = 0;
        }
    }
    int index = ((max_start + (max_start + max_count - 1)) / 2) % TUNE_CORASE_MAX * TUNE_FINE_MAX;
    t->left = max_start;
    t->right = (max_start + max_count - 1) % TUNE_CORASE_MAX;
    t->phase = index;
    PRINT("corase = %d \r\n", index);
}

static void find_longest_fine_result(uint32_t index, int *start_succ, int *count)
{
    int start = -1;
    int end = -1;
    for (uint32_t fine_i = index; fine_i < index + TUNE_FINE_MAX; fine_i++) {
        if (g_tuning_result_arr[fine_i] == 0) {
            if (start == -1) {  // 找到第一个0，记录开始位置
                start = fine_i;
            }
        } else if (g_tuning_result_arr[fine_i] != 0) {  // 遇到1且已经找到开始位置
            if (start != -1) {
                end = fine_i - 1;  // 结束位置是前一个位置
            }
            break;
        }
    }
    if (start != -1 && end == -1) {
        end = start + TUNE_FINE_MAX - 1;
    }
    if (start != -1) {
        *start_succ = start;
        *count = end - start + 1;
    } else {
        *count = 0;
    }
    PRINT("start = %d , end = %d, count == %d \r\n", start, end, count);
}

static void find_longest_continuous_tune2(tune_result *t)
{
    int max_start = -1;
    int max_count = 0;
    int equal_count = 0;
    for (uint32_t corase_i = 0; corase_i < TUNE_CORASE_MAX; corase_i++) {
        int start = -1;
        int count = 0;
        find_longest_fine_result(corase_i * TUNE_FINE_MAX, &start, &count);
        if (count > max_count) {
            max_start = start;
            max_count = count;
        } else if (count == max_count) {
            equal_count++;
        }
    }
    int index = (max_start + (max_start + max_count)) / 2;
    if (equal_count == TUNE_CORASE_MAX - 1) {  // 所有细调区间相同 特殊处理
        index = TUNE_EDGE_MAX / 2;
        PRINT("edge result  cu = %d , xi = %d \r\n", (index / TUNE_FINE_MAX) * 2, index % TUNE_FINE_MAX);
    } else {
        PRINT("start = %d , max_count = %d \r\n", max_start, max_count);
        PRINT("edge result  cu = %d , xi = %d \r\n", (index / TUNE_FINE_MAX) * 2, index % TUNE_FINE_MAX);
    }
    t->left = max_start;
    t->right = max_start + max_count - 1;
    t->phase = index;
}
#endif

static int tune_basic(struct sdhci_slot *slot, tune_result *t)
{
    uint32_t i;
    int tune_result;
    uint32_t reg;
    uint16_t host_ctrl2;
    uint32_t opcode;
    uint32_t success_idx = 0;
    uint32_t fail_index = 0;
#ifndef LOFCFG_DRIVERS_MMC_CUSTOM_TUNING
    uint32_t succeeded_first_phase = TUNE_EDGE_NOT_FUND;
    uint32_t failed_first_phase = TUNE_EDGE_NOT_FUND;
#endif
    struct hci_softc *sc = device_get_softc(slot->bus);
    struct mmc_data *tune_data;
    struct mmc_request *tune_req;
    struct mmc_command *tune_cmd;
    uint8_t tune_data_buf[MMC_TUNING_LEN_HS200];
    const void *cmp_data_buf = NULL;
    struct mmc_ios *ios = &slot->host.ios;

    WR4(slot, SDHCI_INT_ENABLE, slot->intmask | SDHCI_INT_DATA_AVAIL);
    WR4(slot, SDHCI_SIGNAL_ENABLE, slot->intmask | SDHCI_INT_DATA_AVAIL);
    host_ctrl2 = RD2(slot, SDHCI_HOST_CONTROL2);
    host_ctrl2 &= ~SDHCI_CTRL2_SAMPLING_CLOCK;
    WR2(slot, SDHCI_HOST_CONTROL2, host_ctrl2);

    reg = RD4(slot, SDHCI_AUTO_TUNING_CTRL);
    reg |= SDHCI_SW_TUNING_EN;
    WR4(slot, SDHCI_AUTO_TUNING_CTRL, reg);

    for (i = 0; i < TUNE_EDGE_MAX; i++) {
        if (slot->ops->select_sample_phase != NULL) {
            slot->ops->select_sample_phase(sc, i);
        }
        tune_cmd = slot->tune_cmd;
        opcode = tune_cmd->opcode;
        (void)memset_s(tune_cmd, sizeof(*tune_cmd), 0, sizeof(*tune_cmd));
        tune_cmd->opcode = opcode;
        tune_cmd->flags = MMC_RSP_R1 | MMC_CMD_ADTC;

        tune_data = slot->tune_data;
        tune_cmd->data = slot->tune_data;
        (void)memset_s(tune_data, sizeof(*tune_data), 0, sizeof(*tune_data));

        tune_data->len = (opcode == MMC_SEND_TUNING_BLOCK_HS200 && ios->bus_width == bus_width_8) ?
                             MMC_TUNING_LEN_HS200 :
                             MMC_TUNING_LEN;
        cmp_data_buf = (opcode == MMC_SEND_TUNING_BLOCK_HS200 && ios->bus_width == bus_width_8) ?
                           (const void *)tune_blk_8bits :
                           (const void *)tune_blk_4bits;

        tune_data->data = (void *)tune_data_buf;
        tune_data->flags = MMC_DATA_READ;

        tune_req = slot->tune_req;
        (void)memset_s(tune_req, sizeof(*tune_req), 0, sizeof(*tune_req));
        tune_req->cmd = tune_cmd;
        tune_req->done = sdhci_req_wakeup;
        tune_req->done_data = slot;

        slot->req = tune_req;
        slot->flags = 0;
        tune_result = 0;

        sdhci_start(slot);

        if (!OS_SCHEDULER_ACTIVE) {
            if (slot->req != NULL) {
                tune_cmd->error = MMC_ERR_NO_REP;
                slot_printf(slot, "tuning timeout\n");
                sdhci_dump_regs(slot);
                SDHCI_RESET(slot->bus, slot, SDHCI_RESET_CMD | SDHCI_RESET_DATA);
                slot->req = NULL;
            }
        }
        if (tune_cmd->error) {
            tune_result = tune_cmd->error;
        }
        if (memcmp(cmp_data_buf, (void *)tune_data_buf, tune_data->len) != 0) {
            tune_result = -EIO;
        }
        PRINT("tuning index = %d, tune_result %d\n", i, tune_result);
#ifdef LOFCFG_DRIVERS_MMC_CUSTOM_TUNING
        g_tuning_result_arr[i] = tune_result;
        if (g_tuning_result_arr[i] == 0) {
            success_idx++;
        } else if ((i % TUNE_FINE_MAX) == 0) {
            fail_index++;
        }
    }
    if (success_idx == TUNE_EDGE_MAX) {  //  all success
        PRINT("all success \r\n");
        t->left = TUNE_EDGE_MAX / 2;
        t->right = TUNE_EDGE_MAX / 2;
        t->phase = TUNE_EDGE_MAX / 2;
        return 0;
    }
    if (fail_index == TUNE_CORASE_MAX) {  //  all fail
        PRINT("all fail \r\n");
        return -1;
    }
    if (fail_index == 1) {  // fail 1
        find_coarse_result(t);
        return 0;
    }
    find_longest_continuous_tune2(t);
    return 0;
#else
        if (tune_result == 0) {
            if (success_idx == 0)
                succeeded_first_phase = i;
            success_idx++;
        } else {
            fail_index++;
            if ((fail_index && success_idx) && succeeded_first_phase < i) {
                failed_first_phase = i;
                break;
            }
        }
    }

    if (success_idx > 0) {
        t->left = succeeded_first_phase;
        t->right = failed_first_phase - 1;
        t->phase = (t->left + t->right) >> 1;
        return (0);
    } else
        return (-1);
#endif
}

int tune_edge(device_t brdev, device_t reqdev)
{
    int status;
    uint16_t ctrl;
    tune_result t = {0};
    struct sdhci_slot *slot = device_get_ivars(reqdev);
    struct hci_softc *sc = device_get_softc(slot->bus);
    struct mmc_ios *ios = &slot->host.ios;
    if (!(slot->opt & SDHCI_TUNING_SUPPORTED))
        return (0);
#ifdef LOFCFG_DRIVERS_MMC_CUSTOM_TUNING
    g_emmc_reqdev = reqdev;
#endif
    slot->retune_ticks = slot->retune_count * SDHCI_HZ;

    /* Tell the host it's tuning failed by default */
    slot->opt &= ~SDHCI_TUNING_ENABLED;
    slot->tune_cmd->opcode = MMC_SEND_TUNING_BLOCK;
    switch (ios->timing) {
        case bus_timing_mmc_hs400:
            slot_printf(slot, "HS400 must be tuned in HS200 mode\n");
            return (EINVAL);
        case bus_timing_mmc_hs200:
            slot->tune_cmd->opcode = MMC_SEND_TUNING_BLOCK_HS200;
            break;
        case bus_timing_uhs_ddr50:
        case bus_timing_uhs_sdr104:
            break;
        case bus_timing_uhs_sdr50:
            if (slot->opt & SDHCI_SDR50_NEEDS_TUNING)
                break;
            /* fall-through */
        default:
            return (0);
    }

    status = tune_basic(slot, &t);
    if (status < 0) {
        dev_loge(slot->bus, "Tuning failed, using fixed sampling clock\n");
        sc->tuning_phase = SAMP_PHASE_DEF;
    } else {
        dev_loge(slot->bus, "tuning done, phase[%u, %u] final:%u\n", t.left, t.right, t.phase);
        sc->tuning_phase = t.phase;
    }

    /* Tell the host it's tuning successful, we can do it in the future */
    slot->opt |= SDHCI_TUNING_ENABLED;

    if (slot->ops->select_sample_phase != NULL) {
        slot->ops->select_sample_phase(sc, sc->tuning_phase);
    } else {
        dev_loge(slot->bus, "cannot set tune, post tune call back is missing\n");
    }

    ctrl = RD2(slot, SDHCI_HOST_CONTROL2);
    ctrl |= SDHCI_CTRL2_SAMPLING_CLOCK; /* 0: fixed clock, 1: tuned clock for samling */
    WR2(slot, SDHCI_HOST_CONTROL2, ctrl);

    /*
	 * Maybe SDHCI_INT_RESPONSE has been cleared by function sdhci_start_command,
	 * so we restore interrupts here
	 */
    dev_logt(slot->bus, "slot->intmask:0x%x, ctrl:0x%x,", slot->intmask, ctrl);
    slot->intmask |= (SDHCI_INT_RESPONSE | SDHCI_INT_DMA_END);
    WR4(slot, SDHCI_INT_ENABLE, slot->intmask);
    WR4(slot, SDHCI_SIGNAL_ENABLE, slot->intmask);

    return (status);
}

static uint32_t sdhci_get_card_busy(device_t brdev, device_t reqdev)
{
    (void)reqdev;
    uint32_t present_state;
    present_state = hi_sdhci_readl(brdev, SDHCI_PRESENT_STATE);
    /* Check whether DAT[0] is 0 */
    return (!(present_state & SDHCI_STATE_DAT0_MASK));
}

#define SDHCI_PLUG_STATE(sc) (hci_readl(sc, SDHCI_PRESENT_STATE) & SDHCI_CARD_PRESENT)

uint32_t sdhci_card_state(struct sdhci_slot *slot)
{
    struct hci_softc *sc = device_get_softc(slot->bus);
    if (slot->opt & SDHCI_NON_REMOVABLE) {
        return (MMC_CAP_CARD_PRESENT);
    }
    return (SDHCI_PLUG_STATE(sc) ? MMC_CAP_CARD_PRESENT : MMC_CAP_CARD_ABSENT);
}

static bool hi_sdhci_get_card_present(device_t brdev)
{
    struct hci_softc *sc = device_get_softc(brdev);
    struct sdhci_slot *slot = (struct sdhci_slot *)sc->slot;

    return (sdhci_card_state(slot) ? true : false);
}

static bool hi_sdhci_proc_get_card_present(device_t brdev)
{
    struct hci_softc *sc = device_get_softc(brdev);
    struct sdhci_slot *slot = (struct sdhci_slot *)sc->slot;
    if (slot->opt & SDHCI_NON_REMOVABLE) {
        return (slot->dev != NULL);
    }
    return (sdhci_card_state(slot) ? true : false);
}

static bool hi_sdhci_proc_card_attach(device_t brdev)
{
    struct hci_softc *sc = device_get_softc(brdev);
    struct sdhci_slot *slot = (struct sdhci_slot *)sc->slot;
    return (slot->host.card_present ? true : false);
}

void sdhci_set_card_present(device_t brdev, uint32_t p)
{
    struct hci_softc *sc = device_get_softc(brdev);
    struct sdhci_slot *slot = (struct sdhci_slot *)sc->slot;
    slot->host.card_present = p;
}

void sdhci_handle_error(device_t reqdev, const struct mmc_request *req)
{
    struct sdhci_slot *slot = device_get_ivars(reqdev);
    struct mmc_command *cur_cmd = slot->curcmd;
    (void)req;
    if (cur_cmd == NULL)
        return;

    if (cur_cmd->error == MMC_ERR_NO_REP) {
        dev_loge(slot->bus, "Controller timeout, cmd opcode:%u.\n", slot->curcmd->opcode);
        SDHCI_RESET(slot->bus, slot, SDHCI_RESET_CMD);
        SDHCI_RESET(slot->bus, slot, SDHCI_RESET_DATA);
        slot->req = NULL;
        slot->curcmd = NULL;
        sdhci_dump_regs(slot);
    }
}

void sdhci_enable_sdio_irq(device_t brdev)
{
    struct hci_softc *sc = device_get_softc(brdev);
    struct sdhci_slot *slot = (struct sdhci_slot *)sc->slot;
    slot->intmask |= SDHCI_INT_CARD_INT;
    hi_sdhci_writel(brdev, SDHCI_INT_ENABLE, slot->intmask);
    hi_sdhci_writel(brdev, SDHCI_SIGNAL_ENABLE, slot->intmask);
}

int hci_registor_sdio_irq(device_t brdev, sdio_irq_cb cb, void *arg)
{
    struct hci_softc *sc = device_get_softc(brdev);
    if (sc == NULL) {
        return (-EINVAL);
    }

    if ((sc->sdio_cb == NULL) && (sc->sdio_arg == NULL)) {
        sc->sdio_cb = cb;
        sc->sdio_arg = arg;
        return (0);
    } else if (cb == NULL) {
        sc->sdio_cb = NULL;
        sc->sdio_arg = NULL;
        return (0);
    } else
        return (-EACCES);
}

/* Methods for the SDHCI layer. */
static hci_ops hi_sdhci_methods = {
    /* Bus interface */
    .read_ivar = sdhci_generic_read_ivar,
    .write_ivar = sdhci_generic_write_ivar,

    /* MMC bridge interface */
    .update_ios = sdhci_generic_update_ios,
    .request = sdhci_generic_request,
    .get_ro = hi_sdhci_get_ro,
    .acquire_host = sdhci_generic_acquire_host,
    .release_host = sdhci_generic_release_host,
    .set_uhs_timing = hi_set_uhs_timing,
    .tune = tune_edge,
    .retune = sdhci_generic_retune,
    .switch_vccq = sdhci_generic_switch_vccq,
    .get_card_busy = sdhci_get_card_busy,
    /* SDHCI registers accessors */
    .readb = hi_sdhci_readb,
    .readw = hi_sdhci_readw,
    .readl = hi_sdhci_readl,
    .writeb = hi_sdhci_writeb,
    .writew = hi_sdhci_writew,
    .writel = hi_sdhci_writel,
    .get_card_present = hi_sdhci_get_card_present,
    .proc_get_card_present = hi_sdhci_proc_get_card_present,
    .proc_get_card_attach = hi_sdhci_proc_card_attach,
    .set_card_present = sdhci_set_card_present,
    .handle_error = sdhci_handle_error,
    .enable_sdio_irq = sdhci_enable_sdio_irq,
    .registor_sdio_irq = hci_registor_sdio_irq,
    .get_host_id = sdhci_get_host_id,
};

struct resource *platform_get_resource(struct platform_device *dev, unsigned int type, unsigned int num)
{
    uint32_t i, j;
    int found = 0;

    if ((dev == NULL) || (dev->resource == NULL)) {
        PRINT_ERR("platform_get_resource :the input in invalid\n");
        return NULL;
    }

    for (i = 0, j = 0; i < dev->num_resources; i++) {
        if (dev->resource[i].flags & type) {
            j++;
        }

        if (j == (num + 1)) {
            found = 1;
            break;
        }
    }

    if (found == 1) {
        return &dev->resource[i];
    } else {
        PRINT_ERR("can't found the resource\n");
        return NULL;
    }
}

int set_mmc_proc(device_t dev, uint32_t index)
{
    /*
	 * try to keep the dev in the private data
	 * Note: if mmc_devs is not initialized,
	 * it's must be that the proc is not enabled in menu-configuration.
	 * we just return OK in this case.
	 */
    if (mmc_devs.dev != NULL) {
        if (index >= mmc_devs.slots || mmc_devs.dev[index] != NULL) {
            return (-EINVAL);
        }

        mmc_devs.dev[index] = dev;
    }
    return (0);
}

int set_mmc_resource(struct hci_softc *sc, int index)
{
    if ((index < 0) || (index >= MAX_MMC_NUM)) {
        return (-EINVAL);
    }
    mmc_sus_res[index] = sc;
    return (0);
}

struct hci_softc *get_mmc_resource(int index)
{
    if ((index < 0) || (index >= MAX_MMC_NUM)) {
        return (NULL);
    }
    return (mmc_sus_res[index]);
}

void sdhci_task_entry(void)
{
#define WEIGHTED_TIMES 5
    static uint32_t i, j;
    static uint32_t state;
    static uint32_t tmp_state;
    static struct hci_softc *sc;
    static struct sdhci_slot *slot;
    for (i = 0; i < MAX_MMC_NUM; i++) {
        if (i == 1 && !OS_SCHEDULER_ACTIVE) {
            continue;
        }
        sc = get_mmc_resource(i);
        if (sc == NULL || sc->sdio_rescan_allow) {
            continue;
        }
        slot = sc->slot;
        if (slot == NULL) {
            continue;
        }
        tmp_state = state = sdhci_card_state(slot);
        if ((slot->opt & SDHCI_NON_REMOVABLE) && (state != slot->host.plugged_state)) {
            slot->host.plugged_state = state;
            sdhci_card_detect(sc, slot->host.plugged_state ? HCI_EVENT_PLUGGED : HCI_EVENT_UNPLUGGED);
            sc->sdio_rescan_allow = true;
            continue;
        }
        for (j = 1; j < WEIGHTED_TIMES; j++) {
            tmp_state += sdhci_card_state(slot);
            uapi_tcxo_delay_us(10); /* Deley 10us. */
        }
        if (slot->host.plugged_state != state && tmp_state == state * WEIGHTED_TIMES) {
            slot->host.plugged_state = state;
            sdhci_card_detect(sc, slot->host.plugged_state ? HCI_EVENT_PLUGGED : HCI_EVENT_UNPLUGGED);
        }
    }
}

static int hi_sdhci_attach(struct platform_device *pdev)
{
    int err;
    struct hci_softc *sc;
    struct sdhci_slot *slot;
    struct resource *res;

    sc = zalloc(sizeof(struct hci_softc));
    if (sc == NULL) {
        return (-ENOMEM);
    }
    memset_s(sc, sizeof(struct hci_softc), 0, sizeof(struct hci_softc));

    sc->slot = zalloc(sizeof(struct sdhci_slot));
    if (sc->slot == NULL) {
        err = -ENOMEM;
        goto free_sc;
    }
    memset_s(sc->slot, sizeof(struct sdhci_slot), 0, sizeof(struct sdhci_slot));

    slot = sc->slot;

    slot->ops = &slot_methods;
    slot->bus = &pdev->dev;
    slot->num = (uint32_t)pdev->id;
    init_slot_methods(slot->num, slot->ops);
    sc->dev = &pdev->dev;
    sc->sdio_rescan_allow = false;
    err = mmcdev_priv_create(sc->dev, &hi_sdhci_methods, sc, slot);
    if (err != 0)
        goto free_slot;

    /*
	* The chip can inherently do dual-voltage (1p8v, 3p3v) on the first
	* device, and only 3p3v on second devices for 16ev200/300 18ev300 dev-board.
	*
	* but it also depend on the hardware implementation, say if emmc is enabled
	* it should do 1p8v, and if SD mode is enabled, it should do 3p3v.
	*/
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (res == NULL) {
        dev_loge(sc->dev, "cannot found memory resource\n");
        err = -ENXIO;
        goto destroy_priv;
    }
    sc->base = (uintptr_t)res->start;

    /* Resource setup. */
    sc->irq_res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
    if (sc->irq_res == NULL) {
        dev_loge(sc->dev, "cannot found interrupt resource\n");
        err = -ENXIO;
        goto destroy_priv;
    }

    res = platform_get_resource(pdev, IORESOURCE_REG, 1);
    if (res == NULL) {
        device_printf(sc->dev, "cannot found register resource\n");
        err = -ENXIO;
        goto free_irq;
    }

    sc->peri_crg_reg = res->start;
    sc->num = (uint32_t)pdev->id;

    /* Board init */
    if (slot->ops->slot_init != NULL)
        slot->ops->slot_init(sc);

    /* Copy the low-level caps to the slot */
    slot->caps = sc->caps.caps;
    slot->caps2 = sc->caps.caps2;
    slot->min_clk = sc->caps.min_clk;
    slot->max_clk = sc->caps.max_clk;
    slot->quirks = sc->caps.quirks;

    /*
	 * Set up the hardware and go.  Note that this sets many of the
	 * slot.host.* fields, so we have to do this before overriding any of
	 * those values based on spec board, below.
	 */
    err = sdhci_init_slot(sc->dev, slot, pdev->id);
    if (err != 0)
        goto free_irq;

    err = set_mmc_proc(slot->bus, slot->num);
    if (err != 0)
        goto deinit_slot;

    err = set_mmc_resource(sc, slot->num);
    if (err != 0)
        goto clear_mmc_proc;

    if (slot->num == 0) {
        if (LOS_AtomicCmpXchg32bits(&emmc_task_count, 1, 0) == false) {
            int t = 40;
            while (t--) {
                sdhci_task_entry();
                uapi_tcxo_delay_us(50 * 1000);
            }
            if (err) {
                LOS_AtomicDec(&emmc_task_count);
                goto clear_mem_resource;
            }
        } else
            LOS_AtomicInc(&emmc_task_count);
    }
    if (slot->num == 1) {
        if (LOS_AtomicCmpXchg32bits(&sdio_task_count, 1, 0) == false) {
            int t = 40;
            while (t--) {
                sdhci_task_entry();
                uapi_tcxo_delay_us(50 * 1000);
            }
            if (err) {
                LOS_AtomicDec(&sdio_task_count);
                goto clear_mem_resource;
            }
        } else
            LOS_AtomicInc(&sdio_task_count);
    }
    return (0);

clear_mem_resource:
    (void)set_mmc_resource(NULL, slot->num);

clear_mmc_proc:
    (void)set_mmc_proc(NULL, slot->num);

deinit_slot:
    sdhci_deinit_slot(slot);

free_irq:

destroy_priv:
    mmcdev_priv_destroy(sc->dev);

free_slot:
    free(sc->slot);
    sc->slot = NULL;

free_sc:
    free(sc);
    return (err);
}

static int hi_sdhci_detach(struct platform_device *pdev)
{
    struct hci_softc *sc = device_get_softc(&pdev->dev);
    if (sc == NULL) {
        return (-EINVAL);
    }
    (void)sdhci_cleanup_slot((struct sdhci_slot *)sc->slot);
    free(sc->slot);
    sc->slot = NULL;
    mmcdev_priv_destroy(sc->dev);
    free(sc);
    return (0);
}

static struct platform_driver sd_mci_driver[2] = {{
                                                      .probe = hi_sdhci_attach,
                                                      .remove = hi_sdhci_detach,
                                                      .driver =
                                                          {
                                                              .name = "sdhci0",
                                                          },
                                                  },
                                                  {
                                                      .probe = hi_sdhci_attach,
                                                      .remove = hi_sdhci_detach,
                                                      .driver =
                                                          {
                                                              .name = "sdhci1",
                                                          },
                                                  }};

int MMC_HostInitById(unsigned int id)
{
    int status;
    unsigned int count = ARRAY_SIZE(sd_mci_driver);
    if (id >= count) {
        return (-EINVAL);
    }

    status = sd_mci_driver[id].probe(get_mmc_device(id));
    if (status != 0) {
        PRINT("platform driver register error, host id=%u, status=%d\n", id, status);
        return (status);
    }
    return (0);
}