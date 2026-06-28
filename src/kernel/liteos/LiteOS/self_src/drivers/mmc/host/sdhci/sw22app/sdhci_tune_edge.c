/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: LiteOS MMC Sdhci Tune Edge Implementation
 * Author: Huawei LiteOS Team
 * Create: 2019-06-23
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */

#include "sdhci.h"
#include "sdhci_private.h"
#include "mmc_adapt.h"
#include "soc/mmc.h"

#define TUNE_EDGE_MAX        32
#define TUNE_EDGE_PHASES_TIMES    4

#define TUNE_EDGE_SIZE (TUNE_EDGE_MAX / TUNE_EDGE_PHASES_TIMES)

#define TUNE_RESULT_POS 1
#define TUNE_RESULT_ZRO 0

typedef struct {
    uint32_t left;
    uint32_t right;
    uint32_t phase;
} tune_result;

static const uint32_t tune_blk_4bits[] = {
    0x00ff0fff, 0xccc3ccff, 0xffcc3cc3, 0xeffefffe,
    0xddffdfff, 0xfbfffbff, 0xff7fffbf, 0xefbdf777,
    0xf0fff0ff, 0x3cccfc0f, 0xcfcc33cc, 0xeeffefff,
    0xfdfffdff, 0xffbfffdf, 0xfff7ffbb, 0xde7b7ff7
};

static const uint32_t tune_blk_8bits[] = {
    0xff00ffff, 0x0000ffff, 0xccccffff, 0xcccc33cc,
    0xcc3333cc, 0xffffcccc, 0xffffeeff, 0xffeeeeff,
    0xffddffff, 0xddddffff, 0xbbffffff, 0xbbffffff,
    0xffffffbb, 0xffffff77, 0x77ff7777, 0xffeeddbb,
    0x00ffffff, 0x00ffffff, 0xccffff00, 0xcc33cccc,
    0x3333cccc, 0xffcccccc, 0xffeeffff, 0xeeeeffff,
    0xddffffff, 0xddffffff, 0xffffffdd, 0xffffffbb,
    0xffffbbbb, 0xffff77ff, 0xff7777ff, 0xeeddbb77
};

static void
tune_posttune(struct sdhci_slot *slot)
{
    struct hci_softc *sc = device_get_softc(slot->bus);

    if (slot->ops->select_sample_phase != NULL) {
        slot->ops->select_sample_phase(sc, sc->tuning_phase);
    } else {
        slot_printf(slot, "cannot set tune, post tune call back is missing\n");
    }

    /*
     * Maybe SDHCI_INT_RESPONSE has been cleared by function sdhci_start_command,
     * so we restore interrupts here
     */
    slot->intmask |= (SDHCI_INT_RESPONSE | SDHCI_INT_DMA_END);
    WR4(slot, SDHCI_INT_ENABLE, slot->intmask);
    WR4(slot, SDHCI_SIGNAL_ENABLE, slot->intmask);

    hci_dev_clearl(slot->bus, SDHCI_INT_ENABLE, SDHCI_INT_DATA_AVAIL);
    hci_dev_clearl(slot->bus, SDHCI_SIGNAL_ENABLE, SDHCI_INT_DATA_AVAIL);
}

static void
tune_enable_edge_tune(const struct sdhci_slot *slot)
{
    unsigned int reg;
    struct hci_softc *sc = device_get_softc(slot->bus);

    reg = hci_readl(sc, SDHCI_EDGE_DETECT_CTRL);
    reg |= SDHCI_SD_EDGE_DETECT_EN;
    hci_writel(sc, SDHCI_EDGE_DETECT_CTRL, reg);
}

static void
tune_disable_edge_tune(const struct sdhci_slot *slot)
{
    hci_dev_clearl(slot->bus, SDHCI_EDGE_DETECT_CTRL, SDHCI_SD_EDGE_DETECT_EN);
}

static int
tune_request(struct sdhci_slot *slot)
{
#define SDHCI_TUNINT_REQ_TIMEOUT  (LOSCFG_BASE_CORE_TICK_PER_SECOND / 5)
    struct mmc_ios *ios = &slot->host.ios;
    struct mmc_command *tune_cmd;
    struct mmc_data *tune_data;
    struct mmc_request *tune_req;
    uint8_t dest_buf[MMC_TUNING_LEN_HS200] = {0};
    const void *src_buf = NULL;
    uint32_t opcode;
    uint32_t ret;
    slot->retune_ticks = slot->retune_count * SDHCI_HZ;

    tune_cmd = slot->tune_cmd;
    opcode = tune_cmd->opcode;

    (void)memset_s(tune_cmd, sizeof(*tune_cmd), 0, sizeof(*tune_cmd));

    tune_cmd->opcode = opcode;

    tune_cmd->flags = MMC_RSP_R1 | MMC_CMD_ADTC;
    tune_data = slot->tune_data;
    tune_cmd->data = slot->tune_data;
    (void)memset_s(tune_data, sizeof(*tune_data), 0, sizeof(*tune_data));

    tune_data->len = (ios->bus_width == bus_width_8) ? MMC_TUNING_LEN_HS200 :
        MMC_TUNING_LEN;
    src_buf = (ios->bus_width == bus_width_8) ? (const void *)tune_blk_8bits :
        (const void *)tune_blk_4bits;

    tune_data->data = (void *)dest_buf;

    tune_data->flags = MMC_DATA_READ;

    tune_req = slot->tune_req;
    (void)memset_s(tune_req, sizeof(*tune_req), 0, sizeof(*tune_req));
    tune_req->cmd = tune_cmd;
    tune_req->done = sdhci_req_wakeup;
    tune_req->done_data = slot;
    slot->req = tune_req;
    slot->flags = 0;
    sdhci_start(slot);

#ifdef LOSCFG_DRIVERS_MMC_RW_IN_BOOT_AND_EXC
    if (!OS_SCHEDULER_ACTIVE) {
        if (slot->req != NULL) {
            tune_cmd->error = MMC_ERR_NO_REP;
            slot_printf(slot, "tuning timeout\n");
            sdhci_dump_regs(slot);
            SDHCI_RESET(slot->bus, slot, SDHCI_RESET_CMD | SDHCI_RESET_DATA);
            slot->req = NULL;
        }
    } else
#endif
    {
        ret = LOS_EventRead(&slot->req_event, SDHCI_ACQUIRE_HOST,
            LOS_WAITMODE_AND | LOS_WAITMODE_CLR, SDHCI_TUNINT_REQ_TIMEOUT);
        if (ret == LOS_ERRNO_EVENT_READ_TIMEOUT) {
            tune_cmd->error = MMC_ERR_NO_REP;
            slot_printf(slot, "tuning timeout\n");
            sdhci_dump_regs(slot);
            SDHCI_RESET(slot->bus, slot, SDHCI_RESET_CMD | SDHCI_RESET_DATA);
            slot->req = NULL;
        }
    }

    if (tune_cmd->error) {
        dev_logt(slot->bus, "tuning error:0x%x\n", tune_cmd->error);
        return (tune_cmd->error);
    }

    if (memcmp(src_buf, (void *)dest_buf, tune_data->len) != 0) {
        dev_logt(slot->bus, "tuning EIO\n");
        return (-EIO);
    }

    return (0);
}

static int
tune_strict(struct sdhci_slot *slot, tune_result *t)
{
    int tune_result;
    uint32_t i, left, right;
    bool found, found_left;
    struct hci_softc *sc = device_get_softc(slot->bus);

    left = t->left;
    right = t->right;

    found_left = false;

    /*
     * For strict tuning, "holes" may occur in some cases (such as unstable signals).
     * To find the first rising edge and the last falling edge, extra logic processing
     * is required than basic tuning.
     */
    for (i = t->left; i <= t->right; i++) {
        if (slot->ops->select_sample_phase != NULL) {
            slot->ops->select_sample_phase(sc, i % TUNE_EDGE_MAX);
        }

        tune_result = tune_request(slot);
        if (tune_result != 0) {
            found = true;
        } else {
            found = false;
        }

        /* The left boundary is the first found */
        if (found && !found_left) {
            found_left = true;
            left = i;
        }

        /* The right boundary is the last found */
        if (found) {
            right = i;
        }

    }

    t->phase = (((left + right) / 2) + (TUNE_EDGE_MAX / 2)) % TUNE_EDGE_MAX;
    slot_printf(slot, "tuning done, phase[%u, %u] final:%u\n", left % TUNE_EDGE_MAX,
        right % TUNE_EDGE_MAX, t->phase);

    return (0);
}

static int
tuning(struct sdhci_slot *slot, uint32_t parse)
{
    int status;
    uint32_t value;
    struct hci_softc *sc = device_get_softc(slot->bus);

    if (slot->ops->select_sample_phase != NULL) {
        slot->ops->select_sample_phase(sc, parse);
    }

    status = tune_request(slot);
    if (status == 0) {
        value = RD4(slot, SDHCI_EDGE_DETECT_STAT);
        return ((value & SDHCI_FOUND_EDGE_FINAL) ? TUNE_RESULT_POS : TUNE_RESULT_ZRO);
    } else {
        return (TUNE_RESULT_POS); /* It's valid and OK, not TUNE_RESULT_ZRO */
    }
}

static int
tune_basic(struct sdhci_slot *slot, tune_result *t)
{
    int tune_result;
    int32_t i, left, right;
    bool prev_found, found;

    left = -1;
    right = TUNE_EDGE_SIZE + 1;
    prev_found = false;

    for (i = 0; i <= TUNE_EDGE_SIZE; i++) {
        tune_result = tuning(slot, i * TUNE_EDGE_PHASES_TIMES);
        dev_logt(slot->bus, "tune_result %d\n", tune_result);
        if (tune_result == TUNE_RESULT_POS) {
            found = true;
        } else {
            found = false;
        }

        /* It indicates that tune in a falling edge(left boundary) */
        if (!prev_found && found) {
            left = i;
        /* It indicates that tune in a rising edge(right boundary) */
        } else if (prev_found && !found) {
            right = i;
        }

        if (left != -1 && right != TUNE_EDGE_SIZE + 1) { /* Find edge */
            break;
        }

        prev_found = found;
    }

    if (left == 0 && right == TUNE_EDGE_SIZE) {
        dev_loge(slot->bus, "tune not find edge \n");
        return (-1);
    }

    if (right <= left) {
        right += TUNE_EDGE_SIZE;
    }

    t->left = left * TUNE_EDGE_PHASES_TIMES;
    t->right = right * TUNE_EDGE_PHASES_TIMES;
    dprintf("tune_basic t->left:%d, t->left:%d.\r\n", t->left, t->right);
    return (0);
}

static int
tune_get_cmd(struct sdhci_slot *slot)
{
    struct mmc_ios *ios = &slot->host.ios;

    slot->tune_cmd->opcode = MMC_SEND_TUNING_BLOCK;
    switch (ios->timing) {
        case bus_timing_mmc_hs400:
            slot_printf(slot, "HS400 must be tuned in HS200 mode\n");
            return (EINVAL);
        case bus_timing_mmc_hs200:
            slot->tune_cmd->opcode = MMC_SEND_TUNING_BLOCK_HS200;
            break;
        case bus_timing_uhs_ddr50:  /* FALLTHROUGH, no break */
        case bus_timing_uhs_sdr104: /* FALLTHROUGH, no break */
        case bus_timing_uhs_sdr50:  /* FALLTHROUGH, no break */
        default:
            break;
    }
    return (0);
}

int
tune_edge(device_t brdev, device_t reqdev)
{
    int status;
    tune_result t = {0};
    struct sdhci_slot *slot = device_get_ivars(reqdev);
    struct hci_softc *sc = device_get_softc(brdev);

    if (!(slot->opt & SDHCI_TUNING_SUPPORTED)) {
        return (0);
    }

    SDHCI_LOCK(slot);
    /* Tell the host it's tuning failed by default */
    slot->opt &= ~SDHCI_TUNING_ENABLED;
    status = tune_get_cmd(slot);
    if (status < 0) {
        goto out2;
    }

    hci_dev_setl(slot->bus, SDHCI_INT_ENABLE, SDHCI_INT_DATA_AVAIL);
    hci_dev_setl(slot->bus, SDHCI_SIGNAL_ENABLE, SDHCI_INT_DATA_AVAIL);
    if (slot->ops->enable_drv_sample != NULL) {
        slot->ops->enable_drv_sample(sc, 1);
    }

    tune_enable_edge_tune(slot);
    status = tune_basic(slot, &t);
    if (status < 0) {
        mmc_printf("tune basic failure.\n");
        tune_disable_edge_tune(slot);
        goto out1;
    }

    tune_disable_edge_tune(slot);
    status = tune_strict(slot, &t);
    if (status < 0) {
        goto out1;
    }
    sc->tuning_phase = t.phase;
out1:
    /* Tell the host it's tuning successful, we can do it in the future */
    tune_posttune(slot);
out2:
    slot->opt |= SDHCI_TUNING_ENABLED;
    SDHCI_UNLOCK(slot);

    return (status);
}

void do_emmc_tuning(void)
{
    dprintf("[%s] do nothing ! \n", __func__);
}