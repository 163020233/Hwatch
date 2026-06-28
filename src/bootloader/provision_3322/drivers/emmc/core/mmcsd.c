/*-
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2006 Bernd Walter.  All rights reserved.
 * Copyright (c) 2006 M. Warner Losh <imp@FreeBSD.org>
 * Copyright (c) 2017 Marius Strobl <marius@FreeBSD.org>
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Portions of this software may have been developed with reference to
 * the SD Simplified Specification.  The following disclaimer may apply:
 *
 * The following conditions apply to the release of the simplified
 * specification ("Simplified Specification") by the SD Card Association and
 * the SD Group. The Simplified Specification is a subset of the complete SD
 * Specification which is owned by the SD Card Association and the SD
 * Group. This Simplified Specification is provided on a non-confidential
 * basis subject to the disclaimers below. Any implementation of the
 * Simplified Specification may require a license from the SD Card
 * Association, SD Group, SD-3C LLC or other third parties.
 *
 * Disclaimers:
 *
 * The information contained in the Simplified Specification is presented only
 * as a standard specification for SD Cards and SD Host/Ancillary products and
 * is provided "AS-IS" without any representations or warranties of any
 * kind. No responsibility is assumed by the SD Group, SD-3C LLC or the SD
 * Card Association for any damages, any infringements of patents or other
 * right of the SD Group, SD-3C LLC, the SD Card Association or any third
 * parties, which may result from its use. No license is granted by
 * implication, estoppel or otherwise under any patent or other rights of the
 * SD Group, SD-3C LLC, the SD Card Association or any third party. Nothing
 * herein shall be construed as an obligation by the SD Group, the SD-3C LLC
 * or the SD Card Association to disclose or distribute any technical
 * information, know-how or other confidential information to any third party.
 */

#include "kal_base.h"
#include "tcxo.h"
#include "block_private.h"
#include "mmc_pri.h"
#include "sdhci.h"
#include "mmcsd.h"

LITE_OS_SEC_TEXT uint32_t LOS_EventWrite(PEVENT_CB_S eventCB, uint32_t events)
{
    eventCB->uwEventID |= events;
    return LOS_OK;
}

static void biodone(struct bio *bp)
{
    bp->bio_flags |= BIO_DONE;
    if (!OS_EXC_ACTIVE) {
        (void)LOS_EventWrite(&bp->wait_event, REQUEST_EVENT_ANSWER);
    }
}

static void clear_req_queue(struct mmcsd_part *part, struct bio *bp)
{
    unused(part);
    unused(bp);
}

static int mmcsd_switch_part(device_t bus, device_t dev, uint16_t rca, unsigned int part)
{
    struct mmcsd_softc *sc;
    int err;
    uint8_t value;

    sc = device_get_softc(dev);

    if (sc->mode == mode_sd) {
        return (MMC_ERR_NONE);
    }

    /*
     * According to section "6.2.2 Command restrictions" of the eMMC
     * specification v5.1, CMD19/CMD21 aren't allowed to be used with
     * RPMB partitions.  So we pause re-tuning along with triggering
     * it up-front to decrease the likelihood of re-tuning becoming
     * necessary while accessing an RPMB partition.  Consequently, an
     * RPMB partition should immediately be switched away from again
     * after an access in order to allow for re-tuning to take place
     * anew.
     */
    if (part == EXT_CSD_PART_CONFIG_ACC_RPMB) {
        MMCBUS_RETUNE_PAUSE(sc->mmcbus, sc->dev, false);
    }

    if (sc->part_curr == part) {
        return (MMC_ERR_NONE);
    }

    value = (sc->ext_csd[EXT_CSD_PART_CONFIG] & ~EXT_CSD_PART_CONFIG_ACC_MASK) | part;
    /* Jump! */
    err = mmc_switch(bus, dev, rca, EXT_CSD_CMD_SET_NORMAL, EXT_CSD_PART_CONFIG, value, sc->part_time, true);
    if (err != MMC_ERR_NONE) {
        if (part == EXT_CSD_PART_CONFIG_ACC_RPMB) {
            MMCBUS_RETUNE_UNPAUSE(sc->mmcbus, sc->dev);
        }
        return (err);
    }

    sc->ext_csd[EXT_CSD_PART_CONFIG] = value;
    if (sc->part_curr == EXT_CSD_PART_CONFIG_ACC_RPMB) {
        MMCBUS_RETUNE_UNPAUSE(sc->mmcbus, sc->dev);
    }
    sc->part_curr = part;
    return (MMC_ERR_NONE);
}

int wait_card_ready(device_t dev, device_t mmcbus, bool in_exc)
{
#define MAX_WAIT_TIME 6000
    struct mmc_command cmd;
    struct mmc_request req;
    uint64_t time_cost;
    memset_s(&req, sizeof(req), 0, sizeof(req));
    memset_s(&cmd, sizeof(cmd), 0, sizeof(cmd));
    req.cmd = &cmd;
    cmd.opcode = MMC_SEND_STATUS;
    cmd.arg = (uint32_t)mmc_get_rca(dev) << 16;
    cmd.flags = MMC_RSP_R1 | MMC_CMD_AC;
    time_cost = uapi_tcxo_get_ms() + MAX_WAIT_TIME;

#ifdef LOSCFG_DRIVERS_MMC_RW_IN_BOOT_AND_EXC
    req.flags |= (!in_exc) ? 0 : MMC_REQ_IN_EXC;
#endif

    do {
        MMCBUS_WAIT_FOR_REQUEST(mmcbus, dev, &req);
        if ((req.cmd->error != MMC_ERR_NONE) || (cmd.resp[0] & STATUS_REG_ERR_MASK)) {
            device_printf(dev, "wait_card_ready Issuing command 13 failed %s ,status = 0x%x\n",
                          mmcsd_errmsg(req.cmd->error), cmd.resp[0]);
            return (req.cmd->error);
        }
        if (uapi_tcxo_get_ms() >= time_cost) {
            return (-ETIMEDOUT);
        }
    } while (!(cmd.resp[0] & R1_READY_FOR_DATA) || (R1_CURRENT_STATE(cmd.resp[0]) == R1_STATE_PRG));

    return (0);
}

int sent_stop(device_t mmcbus, device_t dev, uint16_t rca, uint32_t write_flag, uint32_t *status)
{
    struct mmc_command cmd = {0};
    int err;
    cmd.arg = (uint32_t)rca << 16;
    cmd.opcode = MMC_STOP_TRANSMISSION;
    if (write_flag) {
        cmd.flags = MMC_RSP_R1B | MMC_CMD_AC;
    } else {
        cmd.flags = MMC_RSP_R1 | MMC_CMD_AC;
    }
    err = mmc_wait_for_cmd(mmcbus, dev, &cmd, 3); /* retry three times*/
    if (err) {
        return err;
    }
    *status = cmd.resp[0];
    /* No need to check card status in case of READ. */
    if (write_flag == 0) {
        return 0;
    }
    return wait_card_ready(dev, mmcbus, false);
}

static int mmc_cmd_error_check(const struct mmc_command *cmd, uint32_t status, bool status_flag)
{
    if ((cmd->error == MMC_ERR_BADCRC) || (cmd->error == MMC_ERR_FIFO)) {
        mmc_err("%s,%d, response CRC or fifo error, cmd->error:0x%x\n", __FUNCTION__, __LINE__, cmd->error);
        return (MMC_ERR_NONE);
    } else if (cmd->error == MMC_ERR_TIMEOUT) {
        mmc_err("%s,%d, response timeout\n", __FUNCTION__, __LINE__);
        if (status_flag == false) {
            mmc_err("%s,%d, get card status failed, retry\n", __FUNCTION__, __LINE__);
            return (MMC_ERR_NONE);
        } else if (status & (R1_COM_CRC_ERROR | R1_ILLEGAL_COMMAND)) {
            mmc_err("%s,%d, get card status failed, status:0x%x\n", __FUNCTION__, __LINE__, status);
            return (MMC_ERR_NONE);
        }
    }
    return (MMC_ERR_FAILED);
}

static int card_recovery(device_t dev, device_t mmcbus, const struct mmc_command *cmd, const struct mmc_command *stop)
{
    int err;
    uint32_t status;
    uint32_t stop_status = 0;
    bool status_flag = true;
    uint32_t write_flag = 0;
    int i = 0;

    if (mmc_get_card_present(dev) == MMC_CAP_CARD_ABSENT) {
        mmc_err("card is absent!\n");
        return (MMC_ERR_FAILED);
    }

    if (!slot_proc_get_card_attach(device_get_parent(mmcbus))) {
        mmc_err("card is not attached!\n");
        return (MMC_ERR_FAILED);
    }

    /* 3: try three times to get card status */
    for (i = 0; i < 3; i++) {
        err = mmc_send_status(mmcbus, dev, mmc_get_rca(dev), &status);
        if (err == 0) {
            break;
        }
        status_flag = false;
    }

    if (err) {
        mmc_err("get card status fail!\n");
        goto hw_reset;
    }

    if (status & R1_CARD_ECC_FAILED) {
        mmc_err("cmd->resp[0] = 0x%x\n", cmd->resp[0]);
    }

    if (!(cmd->data->flags & MMC_DATA_READ)) {
        if ((status & R1_ERROR)) {
            mmc_err("cmd->resp[0] = 0x%x\n", cmd->resp[0]);
        }
    }
    /*
     * if card status at R1_STATE_DATA and R1_STATE_RCV status,
     * try to sent stop cmd
     */
    if (R1_CURRENT_STATE(status) == R1_STATE_DATA || R1_CURRENT_STATE(status) == R1_STATE_RCV) {
        if (!(cmd->data->flags & MMC_DATA_READ)) {
            write_flag = 1;
        }
        err = sent_stop(mmcbus, dev, 0, write_flag, &stop_status);
        if (err) {
            mmc_err("stop_status = 0x%x, err:0x%x\n", stop_status, err);
            goto hw_reset;
        }
        if (stop_status & R1_CARD_ECC_FAILED) {
            mmc_err("card ecc fail\n");
        }
    }
    if (cmd->error) {
        if (mmc_cmd_error_check(cmd, status, status_flag) == MMC_ERR_NONE) {
            return (MMC_ERR_NONE);
        }
    }
    if (stop->error) {
        if (mmc_cmd_error_check(stop, status, status_flag) == MMC_ERR_NONE) {
            return (MMC_ERR_NONE);
        }
    }

hw_reset:
    if (mmc_hw_reset(mmcbus, dev) == 0) {
        return (MMC_ERR_NONE);
    }
    return (MMC_ERR_FAILED);
}

unsigned long mmcsd_rw(struct mmcsd_part *part, struct bio *bp)
{
#define MMCSD_RW_RETRY_TIMES 3
    int status;
    unsigned long block, end, remain_blocks;
    struct mmc_command cmd;
    struct mmc_command stop;
    struct mmc_request req;
    struct mmc_data data;
    struct mmcsd_softc *sc = part->sc;
    device_t dev = sc->dev;
    device_t mmcbus = sc->mmcbus;
    unsigned int numblocks, sz;
    unsigned char *vaddr;
    uintptr_t mem = (uintptr_t)bp->bio_data;
    uint32_t bc = sc->flags & MMCSD_USE_CMD23;
    uint32_t align = part->align;
    uint32_t max_blk = sc->max_data;
    uint32_t retries = 0;

    if (align & mem) {
        max_blk = min(sc->max_data, (part->size / bp->sectorsize));
    }
    block = bp->sector_start;

    sz = bp->sectorsize;
    end = bp->sector_start + bp->numsectors;
    while (block < end) {
        vaddr = bp->bio_data + (block - bp->sector_start) * sz;
        mem = (uintptr_t)vaddr;
        remain_blocks = end - block;
        numblocks = min(remain_blocks, max_blk);
        memset_s(&req, sizeof(req), 0, sizeof(req));
        memset_s(&cmd, sizeof(cmd), 0, sizeof(cmd));
        memset_s(&stop, sizeof(stop), 0, sizeof(stop));
        req.cmd = &cmd;
        cmd.data = &data;
        if (bp->bio_cmd == BIO_READ) {
            if (numblocks > 1) {
                cmd.opcode = MMC_READ_MULTIPLE_BLOCK;
            } else {
                cmd.opcode = MMC_READ_SINGLE_BLOCK;
            }
        } else {
            if (numblocks > 1) {
                cmd.opcode = MMC_WRITE_MULTIPLE_BLOCK;
            } else {
                cmd.opcode = MMC_WRITE_BLOCK;
            }
        }
        cmd.arg = block;
        if (sc->high_cap == 0) {
            cmd.arg <<= 9;
        }
        cmd.flags = MMC_RSP_R1 | MMC_CMD_ADTC;
        data.data = vaddr;

        data.len = numblocks * sz;
        if (bp->bio_cmd == BIO_READ) {
            data.flags = MMC_DATA_READ;
            if (align & mem) {
                data.data = part->buffer;
            }
        } else {
            data.flags = MMC_DATA_WRITE;
            if (align & mem) {
                status = memcpy_s(part->buffer, part->size, vaddr, data.len);
                if (status != EOK) {
                    device_printf(dev, "Error indicated: copy mem %d\n", status);
                    break;
                }
                data.data = part->buffer;
            }
        }

#ifdef LOSCFG_DRIVERS_MMC_RW_IN_BOOT_AND_EXC
        if (bp->in_exc == true) {
            req.flags |= MMC_REQ_IN_EXC;
        }
#endif

        if (numblocks > 1) {
            data.flags |= MMC_DATA_MULTI;
            if (bc) {
                stop.opcode = MMC_SET_BLOCK_COUNT;
                stop.arg = numblocks;
                stop.flags = MMC_RSP_R1 | MMC_CMD_AC;
                req.bc = &stop;
            } else {
                stop.opcode = MMC_STOP_TRANSMISSION;
                stop.flags = MMC_RSP_R1B | MMC_CMD_AC;
                req.stop = &stop;
            }
        }
        if (MMCBUS_WAIT_FOR_REQUEST(mmcbus, dev, &req) != 0) {
            device_printf(dev, "wait for request error \n");
        }
        if (cmd.error != MMC_ERR_NONE || stop.error != MMC_ERR_NONE) {
            if (stop.error != MMC_ERR_NONE) {
                device_printf(dev, "Error indicated: %d %s\n", stop.error, mmcsd_errmsg(stop.error));
            } else {
                device_printf(dev, "Error indicated: %d %s\n", cmd.error, mmcsd_errmsg(cmd.error));
            }
            if ((card_recovery(dev, mmcbus, &cmd, &stop) == MMC_ERR_NONE) && (retries < MMCSD_RW_RETRY_TIMES)) {
                retries++;
                device_printf(dev, "card_recovery success, retry time:%u\n", retries);
                continue;
            }
            break;
        } else if ((bp->bio_cmd == BIO_READ) && (align & mem)) {
            status = memcpy_s(vaddr, remain_blocks * sz, part->buffer, data.len);
            if (status != EOK) {
                device_printf(dev, "copy mem error %d\n", status);
                break;
            }
        }
        block += numblocks;
        if (bp->bio_cmd == BIO_WRITE) {
            if (wait_card_ready(dev, mmcbus, bp->in_exc)) {
                return bp->sector_start;
            }
        }
    }
    return (block);
}

unsigned long mmcsd_delete(struct mmcsd_part *part, struct bio *bp)
{
    unsigned long block, end, start, stop;
    struct mmc_command cmd;
    struct mmc_request req;
    struct mmcsd_softc *sc;
    device_t dev, mmcbus;
    unsigned int erase_sector;
    int err;
    bool use_trim;
    uint64_t time_cost;

    sc = part->sc;
    dev = sc->dev;
    mmcbus = sc->mmcbus;

    block = bp->sector_start;
    end = bp->sector_start + bp->numsectors;
    use_trim = sc->flags & MMCSD_USE_TRIM;
    dev_logt(dev, "block:%lu, end:%lu\n", block, end);
    if (mmc_get_erase_cap(dev) == 0) {
        dev_loge(dev, "the present card dont support erase!\n");
        return (block);
    }
    if (use_trim == true) {
        start = block;
        stop = end;
    } else {
        /* Coalesce with the remainder of the previous request. */
        if (block > part->eblock && block <= part->eend) {
            block = part->eblock;
        }
        if (end >= part->eblock && end < part->eend) {
            end = part->eend;
        }
        /* Safely round to the erase sector boundaries. */
        erase_sector = sc->erase_sector;
        if (erase_sector == 0) {
            dev_loge(dev, "erase sector size is zero, erase operation will return fail!\n");
            return (block);
        }
        start = block + erase_sector - 1; /* Round up. */
        start -= start % erase_sector;
        stop = end; /* Round down. */
        stop -= end % erase_sector;
        /*
         * We can't erase an area smaller than an erase sector, so
         * store it for later.
         */
        if (start >= stop) {
            part->eblock = block;
            part->eend = end;
            return (end);
        }
    }

    if ((sc->flags & MMCSD_INAND_CMD38) != 0) {
        err = mmc_switch(mmcbus, dev, sc->rca, EXT_CSD_CMD_SET_NORMAL, EXT_CSD_INAND_CMD38,
                         use_trim == true ? EXT_CSD_INAND_CMD38_TRIM : EXT_CSD_INAND_CMD38_ERASE, sc->cmd6_time, true);
        if (err != MMC_ERR_NONE) {
            device_printf(dev, "Setting iNAND erase command failed %s\n", mmcsd_errmsg(err));
            return (block);
        }
    }

    /*
     * Pause re-tuning so it won't interfere with the order of erase
     * commands.  Note that these latter don't use the data lines, so
     * re-tuning shouldn't actually become necessary during erase.
     */
    MMCBUS_RETUNE_PAUSE(mmcbus, dev, false);
    /* Set erase start position. */
    memset_s(&req, sizeof(req), 0, sizeof(req));
    memset_s(&cmd, sizeof(cmd), 0, sizeof(cmd));
    req.cmd = &cmd;
    if (mmc_get_card_type(dev) == mode_sd) {
        cmd.opcode = SD_ERASE_WR_BLK_START;
    } else {
        cmd.opcode = MMC_ERASE_GROUP_START;
    }
    cmd.arg = start;
    if (sc->high_cap == 0) {
        cmd.arg <<= 9;
    }
    cmd.flags = MMC_RSP_R1 | MMC_CMD_AC;
    if (MMCBUS_WAIT_FOR_REQUEST(mmcbus, dev, &req) != 0) {
        device_printf(dev, "wait for request error \n");
    }
    if (req.cmd->error != MMC_ERR_NONE) {
        device_printf(dev, "Setting erase start position failed %s\n", mmcsd_errmsg(req.cmd->error));
        block = bp->sector_start;
        goto unpause;
    }
    /* Set erase stop position. */
    memset_s(&req, sizeof(req), 0, sizeof(req));
    memset_s(&cmd, sizeof(cmd), 0, sizeof(cmd));
    req.cmd = &cmd;
    if (mmc_get_card_type(dev) == mode_sd) {
        cmd.opcode = SD_ERASE_WR_BLK_END;
    } else {
        cmd.opcode = MMC_ERASE_GROUP_END;
    }
    cmd.arg = stop;
    if (sc->high_cap == 0) {
        cmd.arg <<= 9;
    }
    cmd.arg--;
    cmd.flags = MMC_RSP_R1 | MMC_CMD_AC;
    if (MMCBUS_WAIT_FOR_REQUEST(mmcbus, dev, &req) != 0) {
        device_printf(dev, "wait for request error \n");
    }
    if (req.cmd->error != MMC_ERR_NONE) {
        device_printf(dev, "Setting erase stop position failed %s\n", mmcsd_errmsg(req.cmd->error));
        block = bp->sector_start;
        goto unpause;
    }
    /* Erase range. */
    memset_s(&req, sizeof(req), 0, sizeof(req));
    memset_s(&cmd, sizeof(cmd), 0, sizeof(cmd));
    req.cmd = &cmd;
    cmd.opcode = MMC_ERASE;
    cmd.arg = use_trim == true ? MMC_ERASE_TRIM : MMC_ERASE_ERASE;
    cmd.flags = MMC_RSP_R1B | MMC_CMD_AC;
    if (MMCBUS_WAIT_FOR_REQUEST(mmcbus, dev, &req) != 0) {
        device_printf(dev, "wait for request error \n");
    }
    if (req.cmd->error != MMC_ERR_NONE) {
        device_printf(dev, "mmcsd_delete Issuing erase command failed %s\n", mmcsd_errmsg(req.cmd->error));
        block = bp->sector_start;
        goto unpause;
    }

    memset_s(&req, sizeof(req), 0, sizeof(req));
    memset_s(&cmd, sizeof(cmd), 0, sizeof(cmd));
    req.cmd = &cmd;
    cmd.opcode = MMC_SEND_STATUS;
    cmd.arg = (uint32_t)mmc_get_rca(dev) << 16;
    cmd.flags = MMC_RSP_R1 | MMC_CMD_AC;
    time_cost = uapi_tcxo_get_ms() + MAX_WAIT_TIME;
    do {
        if (MMCBUS_WAIT_FOR_REQUEST(mmcbus, dev, &req) != 0) {
            device_printf(dev, "wait for request error \n");
        }
        if (req.cmd->error != MMC_ERR_NONE || (cmd.resp[0] & STATUS_REG_ERR_MASK)) {
            device_printf(dev, "Issuing command 13 failed %s ,status = 0x%x\n", mmcsd_errmsg(req.cmd->error),
                          cmd.resp[0]);
            block = bp->sector_start;
            goto unpause;
        }
        if (uapi_tcxo_get_ms() >= time_cost) {
            block = bp->sector_start;
            goto unpause;
        }
    } while (!(cmd.resp[0] & R1_READY_FOR_DATA) || (R1_CURRENT_STATE(cmd.resp[0]) == R1_STATE_PRG));

    if (use_trim == false) {
        /* Store one of the remaining parts for the next call. */
        if (bp->sector_start >= part->eblock || block == start) {
            part->eblock = stop; /* Predict next forward. */
            part->eend = end;
        } else {
            part->eblock = block; /* Predict next backward. */
            part->eend = start;
        }
    }
    dev_logt(dev, "part->eblock:%lu,  part->eend:%lu\n", part->eblock, part->eend);
    block = end;
unpause:
    MMCBUS_RETUNE_UNPAUSE(mmcbus, dev);
    return (block);
}

void mmcsd_task_process(struct mmcsd_part *part, struct bio *bp)
{
    unsigned long block, end;
    struct mmcsd_softc *sc;
    device_t dev, mmcbus;
    int bio_error, err, sz;

    sc = part->sc;
    dev = sc->dev;
    mmcbus = sc->mmcbus;
    bio_error = 0;
    if (bp->bio_cmd != BIO_READ && part->ro) {
        bp->bio_error = EROFS;
        bp->bio_resid = (long)(bp->numsectors * bp->sectorsize);
        bp->bio_flags |= BIO_ERROR;
        biodone(bp);
        return;
    }
    if (bp->bio_cmd == BIO_CMD0) {
        clear_req_queue(part, bp);
        biodone(bp);
        return;
    }
    MMCBUS_ACQUIRE_BUS(mmcbus, dev);
    block = bp->sector_start;
    sz = (int)bp->sectorsize;
    end = bp->sector_start + bp->numsectors;
    err = mmcsd_switch_part(mmcbus, dev, sc->rca, part->type);
    if (err != MMC_ERR_NONE) {
        goto release;
    }
    if (bp->bio_cmd == BIO_READ || bp->bio_cmd == BIO_WRITE) {
        /* Access to the remaining erase block obsoletes it. */
        if (block < part->eend && end > part->eblock)
            part->eblock = part->eend = 0;
        block = mmcsd_rw(part, bp);
    } else if (bp->bio_cmd == BIO_DELETE) {
        block = mmcsd_delete(part, bp);
    } else {
        bio_error = EOPNOTSUPP;
    }
release:
    MMCBUS_RELEASE_BUS(mmcbus, dev);
    if (block < end) {
        bp->bio_error = (bio_error == 0) ? EIO : bio_error;
        bp->bio_resid = (end - block) * sz;
        bp->bio_flags |= BIO_ERROR;
    } else {
        bp->bio_resid = 0;
    }
    biodone(bp);
}