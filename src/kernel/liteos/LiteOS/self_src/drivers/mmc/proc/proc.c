/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: LiteOS MMC Proc Implementation
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

#include "sys/statfs.h"
#include "sys/mount.h"
#include "disk.h"
#include "bridge.h"
#include "mmc_private.h"
#include "mmcreg.h"
#include "mmcbrvar.h"
#include "mmcvar.h"
#include "mmc_adapt.h"
#include "proc.h"

#define	KHZ	1000
#define	MHZ	(KHZ * 1000)
#define	GHZ	(MHZ * 1000)

typedef struct {
	device_t *dev;
	uint32_t slots;
	uint32_t init;
	struct mmc_proc_dir_entry *root;
} mmc_dev;

/* mmc_devs.init will be inited as MMC_PROC_DEFAULT */
mmc_dev mmc_devs;

#define MMC_PROC_DEFAULT 	(0)
#define MMC_PROC_INITING	(1)

DEFINE_SPINLOCK(mmc_proc_init_lock);

static int
seq_add_head(struct mmc_seq_file *s, uint32_t slot, uint32_t inv)
{
	int status;
	if (inv != 0)
		status = mmc_seq_printf(s, "\nMCI%u: invalid\n", slot);
	else
		status = mmc_seq_printf(s, "\nMCI%u", slot);
	return (status);
}

static int
seq_add_plug_status(struct mmc_seq_file *s, device_t bus_dev, device_t bus)
{
	int status = 1;
	const struct mmc_softc *sc = NULL;

	if (slot_proc_get_card_present(bus) == MMC_CAP_CARD_ABSENT) {
		(void)mmc_seq_printf(s, ": unplugged_disconnected\n");
	} else if (!slot_proc_get_card_attach(bus)) {
		(void)mmc_seq_printf(s, ": plugged_disconnected\n");
	} else {
		(void)mmc_seq_printf(s, ": plugged");
		if (bus_dev == NULL) {
			(void)mmc_seq_printf(s, "_init_failed\n");
		} else {
			sc = device_get_softc(bus_dev);
			if (sc == NULL) {
				(void)mmc_seq_printf(s, "_disconnected\n");
				return (status);
			}
			if (sc->recovery == true) {
				(void)mmc_seq_printf(s, "_connected\n");
				status = 0;
				return (status);
			}
			if (sc->card_init_status == MMC_CARD_INIT) {
				(void)mmc_seq_printf(s, "_connected\n");
				status = 0;
			} else if (sc->card_init_status == MMC_CARD_INIT_FAIL) {
				(void)mmc_seq_printf(s, "_init_failed\n");
			} else {
				(void)mmc_seq_printf(s, "_disconnected\n");
			}
		}
	}
	return (status);
}

static int seq_add_card_type(struct mmc_seq_file *s, uint32_t mode)
{
	const char *strings = NULL;
	switch (mode) {
	case mode_mmc:
		strings = "MMC card";
		break;
	case mode_sd:
		strings = "SD card";
		break;
	case mode_sdio:
		strings = "SDIO card";
		break;
	case mode_combo:
		strings = "SD combo (IO+mem) card";
		break;
	default:
		strings = "unknown";
		break;
	}
	return (mmc_seq_printf(s, "\tType: %s", strings));
}

static int
seq_add_caps(struct mmc_seq_file *s, device_t bus_dev, device_t card)
{
	(void)bus_dev;
	const char *strings = NULL;

	if (mmc_get_high_cap(card)) {
		if (mmc_get_capacity(card) >= 8) { /* larger than 8GB */
			strings = "SDXC";
		} else {
			strings = "SDHC";
		}
	}
	if (strings != NULL) {
		return (mmc_seq_printf(s, "(%s)\n", strings));
	} else {
		return 0;
	}
}

static int
seq_add_timing(struct mmc_seq_file *s, device_t bus_dev)
{
	int t;
	const char *strings = "UHS ";
	const char *mode = "";

	t = mmcbr_get_timing(bus_dev);
	switch (t) {
	case bus_timing_mmc_hs:
	case bus_timing_sd_hs:
		strings = "HS ";
		break;
	case bus_timing_uhs_sdr12:
		mode = "SDR12";
		break;
	case bus_timing_uhs_sdr25:
		mode = "SDR25";
		break;
	case bus_timing_uhs_sdr50:
		mode = "SDR50";
		break;
	case bus_timing_uhs_ddr50:
		mode = "DDR50";
		break;
	case bus_timing_uhs_sdr104:
		mode = "SDR104";
		break;
	case bus_timing_mmc_ddr52:
		strings = "DDR ";
		break;
	case bus_timing_mmc_hs200:
		strings = "HS200 ";
		break;
	case bus_timing_mmc_hs400:
	case bus_timing_mmc_hs400es:
		strings = "HS400 ";
		break;
	default:
		strings = "";
		break;
	}

	return (mmc_seq_printf(s, "\tMode: %s%s\n", strings, mode));
}

static int
seq_add_speed(struct mmc_seq_file *s, device_t card)
{
	int status;
	uint32_t speed = mmc_get_sc(card);
	uint32_t grade = mmc_get_grade(card);
	const char *uhs = NULL;
	char class[] = "Reserved";

	/* speed equal 4 indicates card is class 10 */
	if (speed == 4) {
		status = strcpy_s(class, sizeof(class), "10");
		if (status != EOK) {
			return (-EINVAL);
		}
	} else if (speed < 4) {
		status = sprintf_s(class, sizeof(class), "%u", speed * 2);
		if (status < 0) {
			return (-EINVAL);
		}
	}

	if (grade == 0) {
		uhs = "Less than 10MB/sec(0h)";
	} else {
		uhs = "10MB/sec and above(1h)";
	}

	return (mmc_seq_printf(s, "\tSpeed Class: Class %s\n\tUhs Speed Grade: %s\n", class, uhs));
}

static int
seq_add_clk(struct mmc_seq_file *s, device_t bus_dev)
{
	const char *strings = NULL;
	int clk = mmcbr_get_clock(bus_dev);

	if (clk >= GHZ) {
		clk /= GHZ;
		strings = "GHz";
	} else if (clk >= MHZ) {
		clk /= MHZ;
		strings = "MHz";
	} else if (clk >= KHZ) {
		clk /= KHZ;
		strings = "KHz";
	} else {
		strings = "Hz";
	}
	return (mmc_seq_printf(s, "\tHost work clock: %d%s\n"
					    "\tCard support clock: %d%s\n"
					    "\tCard work clock: %d%s\n", clk, strings,
					    clk, strings, clk, strings));
}

static int
seq_add_error_cnt(struct mmc_seq_file *s, device_t bus_dev)
{
	uint32_t e = mmcbr_get_card_ec(bus_dev);
	return (mmc_seq_printf(s, "\tCard error count: %u\n", e));
}

static int
seq_add_card_name(struct mmc_seq_file *s, const char *pnm)
{
	return (mmc_seq_printf(s, "\tCard name: %s\n", pnm));
}

static int
seq_add_mid(struct mmc_seq_file *s, uint32_t mid)
{
	return (mmc_seq_printf(s, "\tCard mid: 0x%06x\n", mid));
}

static int
seq_add_oid(struct mmc_seq_file *s, uint16_t oid)
{
	return (mmc_seq_printf(s, "\tCard oid: 0x%04x\n", oid));
}

static inline int
seq_fmt_mid(struct mmc_seq_file *s, uint32_t mid)
{
	return (mmc_seq_printf(s, "%02x", mid));
}

static inline int
seq_fmt_oid(struct mmc_seq_file *s, uint16_t oid)
{
	return (mmc_seq_printf(s, "%04x", oid));
}

static inline int
seq_fmt_name(struct mmc_seq_file *s, const char *p)
{
	return (mmc_seq_printf(s, "%02x%02x%02x%02x%02x", p[0], p[1], p[2], p[3], p[4]));
}

static inline int
seq_fmt_rev(struct mmc_seq_file *s, uint8_t prv)
{
	return (mmc_seq_printf(s, "%02x", prv));
}

static inline int
seq_fmt_psn(struct mmc_seq_file *s, uint32_t psn)
{
	return (mmc_seq_printf(s, "%08x", psn));
}

static inline int
seq_fmt_year(struct mmc_seq_file *s, uint32_t y, uint32_t m)
{
	y -= 2000;
	return (mmc_seq_printf(s, "%03x%01x", y, m));
}

static inline int
seq_fmt_crc(struct mmc_seq_file *s, uint8_t crc)
{
	return (mmc_seq_printf(s, "%02x", crc));
}

static int
seq_add_cid(struct mmc_seq_file *s, device_t card)
{
	uint32_t status;
	struct mmc_cid *cid = mmc_get_cid(card);

	status = (uint32_t)seq_add_card_name(s, cid->pnm);
	status |= (uint32_t)seq_add_mid(s, cid->mid);
	status |= (uint32_t)seq_add_oid(s, cid->oid);
	status |= (uint32_t)mmc_seq_printf(s, "\tCard cid: ");
	status |= (uint32_t)seq_fmt_mid(s, cid->mid);
	status |= (uint32_t)seq_fmt_oid(s, cid->oid);
	status |= (uint32_t)seq_fmt_name(s, cid->pnm);
	status |= (uint32_t)seq_fmt_rev(s, cid->prv);
	status |= (uint32_t)seq_fmt_psn(s, cid->psn);
	status |= (uint32_t)seq_fmt_year(s, cid->mdt_year, cid->mdt_month);
	status |= (uint32_t)seq_fmt_crc(s, cid->crc);
	status |= (uint32_t)mmc_seq_printf(s, "\n");
	if (status != 0) {
		return (-ENOMEM);
	} else {
		return (0);
	}
}

static int
proc_show(struct mmc_seq_file *s, void *v)
{
	(void)v;
	uint32_t status;
	int card_no;
	uint32_t slot;
	device_t dev = NULL;
	device_t bus_dev = NULL;
	device_t card = NULL;
	mmc_dev *devs = &mmc_devs;
	struct mmc_softc *sc = NULL;

	mmc_resource_mutex_lock();
	for (slot = 0; slot < devs->slots; slot++) {
		dev = devs->dev[slot];
		status = (uint32_t)seq_add_head(s, slot, (uint32_t)(dev == NULL));
		if (status != 0) {
			mmc_resource_mutex_unlock();
			return (-ENOMEM);
		}
		if (dev == NULL) {
			continue;
		}

		bus_dev = mmcdev_get_child(dev);
		status = (uint32_t)seq_add_plug_status(s, bus_dev, dev);
		if ((bus_dev == NULL) || status) {
			continue;
		}
		sc = device_get_softc(bus_dev);

		for (card_no = 0; card_no < sc->child_count; card_no++) {
			card = sc->child_list[card_no];
			if (card == NULL) {
				mmc_resource_mutex_unlock();
				return (-ENOMEM);
			}
			status |= (uint32_t)seq_add_card_type(s, (uint32_t)mmc_get_card_type(card));
			status |= (uint32_t)seq_add_caps(s, bus_dev, card);
			status |= (uint32_t)seq_add_timing(s, bus_dev);
			status |= (uint32_t)seq_add_speed(s, card);
			status |= (uint32_t)seq_add_clk(s, bus_dev);
			status |= (uint32_t)seq_add_error_cnt(s, bus_dev);
			status |= (uint32_t)seq_add_cid(s, card);
			if (status != 0) {
				mmc_resource_mutex_unlock();
				return (-ENOMEM);
			}
		}
	}
	mmc_resource_mutex_unlock();

	return (0);
}

int
set_mmc_proc(device_t dev, uint32_t index)
{
	/*
	 * try to keep the dev in the private data
	 * Note: if mmc_devs is not initialized,
	 * it's must be that the proc is not enabled in menu-configuration.
	 * we just return OK in this case.
	 */
	if (mmc_devs.dev != NULL) {
		if (index >= mmc_devs.slots ||
		    mmc_devs.dev[index] != NULL) {
			return (-EINVAL);
		}

		mmc_devs.dev[index] = dev;
	}
	return (0);
}

void
mmc_proc_exit(void)
{
	if (mmc_devs.dev != NULL) {
		mmc_remove_proc_entry(MCI_PROC_ROOT, NULL);
		mmc_remove_proc_entry(MCI_PROC_NAME, mmc_devs.root);
		free(mmc_devs.dev);
		mmc_devs.dev = NULL;
	}
}

static int
proc_open(struct inode *node, struct proc_file *pf)
{
	struct mmc_proc_dir_entry *entry = (struct mmc_proc_dir_entry *)node;

	return (single_open(pf, proc_show, entry->data));
}

static const struct proc_file_operations g_mciProcFops = {
	.open = proc_open,
	.release = single_release,
	.llseek = seq_lseek,
	.read = seq_read,
};

int
mmc_proc_init(uint32_t count)
{
	device_t *dev;
	struct mmc_proc_dir_entry *root = NULL;
	struct mmc_proc_dir_entry *entry = NULL;
	unsigned long flags;
	int ret = 0;

	spin_lock_irqsave(&mmc_proc_init_lock, flags);
	if (mmc_devs.dev != NULL) {
		/*
		 * Because these function could be called for multiple times.
		 * So we will not return failure if already inited.
		 */
		goto unlocked;
	}

	if (mmc_devs.init != MMC_PROC_DEFAULT)  {
		ret = -EAGAIN;
		goto unlocked;
	}
	mmc_devs.init = MMC_PROC_INITING;
	spin_unlock_irqrestore(&mmc_proc_init_lock, flags);

	root = mmc_proc_mkdir(MCI_PROC_ROOT, NULL);
	if (root == NULL) {
		dprintf("can not create root proc entry!\n");
		ret = -EEXIST;
		goto reset_init_flag;
	}

	entry = mmc_create_proc_entry(MCI_PROC_NAME, 0, root);
	if (entry == NULL) {
		mmc_remove_proc_entry(MCI_PROC_ROOT, NULL);
		dprintf("can not create proc entry!\n");
		ret = -EINVAL;
		goto reset_init_flag;
	}

	dev = zalloc(sizeof(device_t) * count);
	if (dev == NULL) {
		mmc_remove_proc_entry(MCI_PROC_ROOT, NULL);
		mmc_remove_proc_entry(MCI_PROC_NAME, root);
		ret = -ENOMEM;
		goto reset_init_flag;
	}

	mmc_devs.dev = dev;
	mmc_devs.slots = count;
	mmc_devs.root = root;
	SET_PROC_ENTRY_OPS(entry, g_mciProcFops);

reset_init_flag:
	spin_lock_irqsave(&mmc_proc_init_lock, flags);
	mmc_devs.init = MMC_PROC_DEFAULT;
unlocked:
	spin_unlock_irqrestore(&mmc_proc_init_lock, flags);

	return (ret);
}
