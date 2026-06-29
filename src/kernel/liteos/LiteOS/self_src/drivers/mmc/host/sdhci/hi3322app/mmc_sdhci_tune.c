/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description:  PMU DRIVER Source
 */

#include "sdhci.h"
#include "sdhci_private.h"
#include "hci_helper.h"

#define SAMP_PHASE_DEF       15
#define TUNE_CORASE_MAX      8  /* cu tiao 8 */
#define TUNE_FINE_MAX        16 /* xi tiao 16 */
#define TUNE_EDGE_MAX        (TUNE_CORASE_MAX * TUNE_FINE_MAX) /* Active phase scale: 0 ~ TUNE_EDGE_PHASES-1 */
int g_tuning_result_arr[TUNE_EDGE_MAX] = {0};
#define TUNE_EDGE_NOT_FUND    TUNE_EDGE_MAX

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

#ifdef LOFCFG_DRIVERS_MMC_CUSTOM_TUNING
static bool print_tuning_process = false;
static device_t g_emmc_reqdev = NULL;
void do_emmc_tuning(void)
{
    if (g_emmc_reqdev == NULL) {
        dprintf("[%s] failed ! \n", __func__);
        return;
    }
    print_tuning_process = true;
    tune_edge(NULL, g_emmc_reqdev);
    print_tuning_process = false;
}

#else

void do_emmc_tuning(void)
{
    dprintf("[%s] do nothing ! \n", __func__);
}
#endif

static int find_longest_success_sequence(tune_result *t)
{
    int max_len = 0;
    int current_len = 0;
    int current_start = -1;
    int start_index = -1;
    for (int i = 0; i < TUNE_EDGE_MAX; i++) {
        if (g_tuning_result_arr[i] == 0) {
            if (current_len == 0) {
                current_start = i;
            }
            current_len++;
        } else {
            if (current_len > max_len) {
                max_len = current_len;
                start_index = current_start;
            }
            current_len = 0;
        }
    }
    // 最后再检查一次，因为最长连续0可能在数组的末尾
    if (current_len > max_len) {
        max_len = current_len;
        start_index = current_start;
    }
    if  (max_len != 0) {
        t->left = start_index;
        t->right = start_index + (max_len - 1);
        t->phase = (t->left + t->right) >> 1 ;
        return 0;
    }
    return -1;
}

static void start_tune_cmd(struct sdhci_slot *slot,  struct mmc_command *tune_cmd, int index)
{
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
        uint32_t ret = LOS_EventRead(&slot->req_event, SDHCI_ACQUIRE_HOST,
            LOS_WAITMODE_AND | LOS_WAITMODE_CLR, (LOSCFG_BASE_CORE_TICK_PER_SECOND / 5));
        if (ret == LOS_ERRNO_EVENT_READ_TIMEOUT) {
            tune_cmd->error = MMC_ERR_NO_REP;
            slot_printf(slot, "tuning timeout\n");
            sdhci_dump_regs(slot);
            SDHCI_RESET(slot->bus, slot, SDHCI_RESET_CMD | SDHCI_RESET_DATA);
            slot->req = NULL;
        }
    }
    if (tune_cmd->error) {
        g_tuning_result_arr[index] = tune_cmd->error;
    }
}

static int tune_basic(struct sdhci_slot *slot, tune_result *t)
{
    struct hci_softc *sc = device_get_softc(slot->bus);
    struct mmc_data *tune_data;
    struct mmc_request *tune_req;
    struct mmc_command *tune_cmd;
    uint8_t tune_data_buf[MMC_TUNING_LEN_HS200];
    const void *cmp_data_buf = NULL;
    struct mmc_ios *ios = &slot->host.ios;
    WR4(slot, SDHCI_INT_ENABLE, slot->intmask | SDHCI_INT_DATA_AVAIL);
    WR4(slot, SDHCI_SIGNAL_ENABLE, slot->intmask | SDHCI_INT_DATA_AVAIL);
    uint16_t host_ctrl2 = RD2(slot, SDHCI_HOST_CONTROL2);
    host_ctrl2 &= ~SDHCI_CTRL2_SAMPLING_CLOCK;
    WR2(slot, SDHCI_HOST_CONTROL2, host_ctrl2);
    uint32_t reg = RD4(slot, SDHCI_AUTO_TUNING_CTRL);
    reg |= SDHCI_SW_TUNING_EN;
    WR4(slot, SDHCI_AUTO_TUNING_CTRL, reg);
    for (uint32_t i = 0; i < TUNE_EDGE_MAX; i++) {
        if (slot->ops->select_sample_phase != NULL) {
            slot->ops->select_sample_phase(sc, i);
        }
        tune_cmd = slot->tune_cmd;
        uint32_t opcode = tune_cmd->opcode;
        (void)memset_s(tune_cmd, sizeof(*tune_cmd), 0, sizeof(*tune_cmd));
        tune_cmd->opcode = opcode;
        tune_cmd->flags = MMC_RSP_R1 | MMC_CMD_ADTC;
        tune_data = slot->tune_data;
        tune_cmd->data = slot->tune_data;
        (void)memset_s(tune_data, sizeof(*tune_data), 0, sizeof(*tune_data));
        tune_data->len = (opcode == MMC_SEND_TUNING_BLOCK_HS200 &&
            ios->bus_width == bus_width_8) ? MMC_TUNING_LEN_HS200 : MMC_TUNING_LEN;
        cmp_data_buf = (opcode == MMC_SEND_TUNING_BLOCK_HS200 &&
            ios->bus_width == bus_width_8) ? (const void *)tune_blk_8bits : (const void *)tune_blk_4bits;
        tune_data->data = (void *)tune_data_buf;
        tune_data->flags = MMC_DATA_READ;
        tune_req = slot->tune_req;
        (void)memset_s(tune_req, sizeof(*tune_req), 0, sizeof(*tune_req));
        tune_req->cmd = tune_cmd;
        tune_req->done = sdhci_req_wakeup;
        tune_req->done_data = slot;
        slot->req = tune_req;
        slot->flags = 0;
        start_tune_cmd(slot, tune_cmd, i);
        if (memcmp(cmp_data_buf, (void *)tune_data_buf, tune_data->len) != 0) {
            g_tuning_result_arr[i] = -EIO;
        }
    }
    return find_longest_success_sequence(t);
}

static int cfg_tune_val(struct hci_softc *sc, struct sdhci_slot *slot, tune_result *t)
{
    int status = tune_basic(slot, t);
    if (status < 0) {
        dev_loge(slot->bus, "Tuning failed, using fixed sampling clock\n");
        sc->tuning_phase = SAMP_PHASE_DEF;
    } else {
        dev_loge(slot->bus, "tuning done, phase[%u, %u] final:%u\n", t->left, t->right, t->phase);
        sc->tuning_phase = t->phase;
    }
    /* Tell the host it's tuning successful, we can do it in the future */
    slot->opt |= SDHCI_TUNING_ENABLED;
    if (slot->ops->select_sample_phase != NULL) {
        slot->ops->select_sample_phase(sc, sc->tuning_phase);
    }
    uint16_t ctrl = RD2(slot, SDHCI_HOST_CONTROL2);
    ctrl |= SDHCI_CTRL2_SAMPLING_CLOCK; /* 0: fixed clock, 1: tuned clock for samling */
    WR2(slot, SDHCI_HOST_CONTROL2, ctrl);
    slot->intmask |= (SDHCI_INT_RESPONSE | SDHCI_INT_DMA_END);
    WR4(slot, SDHCI_INT_ENABLE, slot->intmask);
    WR4(slot, SDHCI_SIGNAL_ENABLE, slot->intmask);
    return status;
}

int tune_edge(device_t brdev, device_t reqdev)
{
    tune_result t = {0};
    struct sdhci_slot *slot = device_get_ivars(reqdev);
    struct hci_softc *sc = device_get_softc(slot->bus);
    struct mmc_ios *ios = &slot->host.ios;
    if (!(slot->opt & SDHCI_TUNING_SUPPORTED)) {
        return (0);
    }
#ifdef LOFCFG_DRIVERS_MMC_CUSTOM_TUNING
    g_emmc_reqdev = reqdev;
#endif
    SDHCI_LOCK(slot);
    slot->retune_ticks = slot->retune_count * SDHCI_HZ;
    /* Tell the host it's tuning failed by default */
    slot->opt &= ~SDHCI_TUNING_ENABLED;
    slot->tune_cmd->opcode = MMC_SEND_TUNING_BLOCK;
    switch (ios->timing) {
        case bus_timing_mmc_hs400:
            SDHCI_UNLOCK(slot);
            return (EINVAL);
        case bus_timing_mmc_hs200:
            slot->tune_cmd->opcode = MMC_SEND_TUNING_BLOCK_HS200;
            break;
        case bus_timing_uhs_ddr50:
        case bus_timing_uhs_sdr104:
            break;
        case bus_timing_uhs_sdr50:
            if (slot->opt & SDHCI_SDR50_NEEDS_TUNING) {
                break;
            }
        default:
            SDHCI_UNLOCK(slot);
            return (0);
    }
    int status = cfg_tune_val(sc, slot, &t);
    SDHCI_UNLOCK(slot);
    return (status);
}
