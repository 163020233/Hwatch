/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "fsp.h"
#include "dbg.h"
#include "vfmw_define.h"
#include "usrdata.h"
#include "dps.h"
#include "event.h"
#include "vdec_chr.h"

STATIC td_void fsp_require_fs(const fsp_chan *chan, fsp_mem_type type, td_u32 size);
STATIC td_void fsp_release_fs(const fsp_chan *chan, fsp_mem_type type, const td_void *args);
STATIC td_s32 fsp_add_pmv_fs(fsp_chan *chan, const vfmw_mem_report *mem);
STATIC td_s32 fsp_add_dec_fs(fsp_chan *chan, const vfmw_mem_report *dec_mem, const vfmw_mem_report *info_mem,
    const vfmw_mem_report *hfbc_mem);

static fsp_entry g_fsp_entry = {
    .reserve1 = 0x1234abcd,
    .ref_cnt = 0,
    .state = FSP_STATE_NULL,
    .ctx_size = sizeof(fsp_entry),
    .reserve2 = 0x1234abcd,
};

#define DEC_BUF_NAME  "VFMW_DecBuf"
#define DISP_BUF_NAME "VFMW_DispBuf"
#define PMV_BUF_NAME  "VFMW_PmvBuf"
#define NOR_BUF_NAME  "VFMW_NorInfo"
#define SEC_BUF_NAME  "VFMW_SecInfo"
#define FSP_NAME_LEN  50

#define fsp_assert_mod_goto(to) \
    if (g_fsp_entry.state != FSP_STATE_INIT) { \
        dprint(PRN_ERROR, "%s %d Fsp entry state %d invalid!\n", __func__, __LINE__, g_fsp_entry.state); \
        goto to; \
    }

#define fsp_assert_logic_goto(logic, to) \
    do { \
        if ((logic) == TD_NULL) { \
            dprint(PRN_WARN, "%s %d pLogic = TD_NULL!\n", __func__, __LINE__); \
            goto to; \
        } \
        if ((logic)->is_use == 0) { \
            goto to; \
        } \
    } while (0)

/* inner func */
fsp_entry *fsp_get_entry(td_void)
{
    return &g_fsp_entry;
}

td_s32 fsp_get_mem_need_size(vfmw_svr_mem_need *mem)
{
    vfmw_check_ret(mem != TD_NULL, FSP_ERR);

    mem->chan_cache_size += vfmw_align_up((td_u32)sizeof(fsp_chan), VFMW_CTX_ALIGN);

    return FSP_OK;
}

fsp_chan *fsp_get_chan(td_s32 chan_id)
{
    if (chan_id < 0 || chan_id >= FSP_CHAN_NUM) {
        return TD_NULL;
    }

    return fsp_get_entry()->chan[chan_id];
}

td_void fsp_sema_down(td_void)
{
    os_sema_down(fsp_get_entry()->sema);
}

td_void fsp_sema_up(td_void)
{
    os_sema_up(fsp_get_entry()->sema);
}

td_void fsp_lock(td_s32 chan_id, td_ulong *lock_flag)
{
    os_spin_lock(fsp_get_entry()->lock[chan_id], lock_flag);
}

td_void fsp_unlock(td_s32 chan_id, td_ulong *lock_flag)
{
    os_spin_unlock(fsp_get_entry()->lock[chan_id], lock_flag);
}

td_void fsp_img_format_lock(td_ulong *lock_flag)
{
    os_spin_lock(fsp_get_entry()->img_format_lock, lock_flag);
}

td_void fsp_img_format_unlock(td_ulong *lock_flag)
{
    os_spin_unlock(fsp_get_entry()->img_format_lock, lock_flag);
}

td_bool fsp_is_dec_over(const fsp_logic *logic_fs)
{
    td_u32 dec_over = 0;
    td_ulong lock_flag;

    dec_over |= (td_u32)(logic_fs->dec_state.data == FSP_DEC_STATE_IDLE);
    dec_over |= (td_u32)(logic_fs->dec_state.bits.top_state == FSP_DEC_STATE_OVER &&
                         logic_fs->dec_state.bits.bot_state == FSP_DEC_STATE_OVER);
    dec_over |= (td_u32)(logic_fs->dec_state.bits.top_state == FSP_DEC_STATE_ERR &&
                         logic_fs->dec_state.bits.bot_state == FSP_DEC_STATE_ERR);
    if (dec_over != 0) {
        return TD_TRUE;
    }

    fsp_img_format_lock(&lock_flag);
    dec_over |= (td_u32)(logic_fs->image.format.field_valid_flag == 1 &&
                         (logic_fs->dec_state.bits.top_state == FSP_DEC_STATE_OVER ||
                          logic_fs->dec_state.bits.top_state == FSP_DEC_STATE_ERR));
    dec_over |= (td_u32)(logic_fs->image.format.field_valid_flag == 2 && /* 2 means bottom field */
                         (logic_fs->dec_state.bits.bot_state == FSP_DEC_STATE_OVER ||
                          logic_fs->dec_state.bits.bot_state == FSP_DEC_STATE_ERR));
    fsp_img_format_unlock(&lock_flag);

    return (dec_over == 0) ? TD_FALSE : TD_TRUE;
}

STATIC td_void fsp_free_pmv_fs(fsp_logic *logic_fs, td_bool dec_over)
{
    if (logic_fs->is_ref == 0 &&
        logic_fs->is_dec_ref == 0 &&
        logic_fs->dec_fs != TD_NULL &&
        logic_fs->dec_fs->ref_cnt == 0) {
        if (dec_over == TD_TRUE && logic_fs->pmv_fs != TD_NULL) {
            logic_fs->pmv_fs->is_use = 0;
            logic_fs->pmv_fs = TD_NULL;
        }
    }
}

STATIC td_void fsp_free_attach_fs(fsp_logic *logic_fs)
{
    if (logic_fs->disp_fs == TD_NULL) {
        if (logic_fs->nor_fs != TD_NULL) {
            logic_fs->nor_fs->is_use = 0;
            logic_fs->nor_fs = TD_NULL;
        }

        if (logic_fs->sec_fs != TD_NULL) {
            logic_fs->sec_fs->is_use = 0;
            logic_fs->sec_fs = TD_NULL;
        }
    }
}

STATIC td_void fsp_free_phy_fs_type_share(fsp_logic *logic_fs, td_bool dec_over)
{
    if (logic_fs->is_ref == 0 && logic_fs->is_dec_ref == 0 &&
        ((logic_fs->dec_fs != TD_NULL && logic_fs->dec_fs->ref_cnt == 0) ||
        logic_fs->dec_fs == TD_NULL) && logic_fs->disp_state == FSP_DISP_STATE_IDLE &&
        logic_fs->disp_fs != TD_NULL && dec_over == TD_TRUE) {
        logic_fs->disp_fs->is_use = 0;
        logic_fs->disp_fs = TD_NULL;
    }
}

STATIC td_void fsp_free_phy_fs_type_other(fsp_logic *logic_fs, td_bool dec_over)
{
    /* resolve the issue of the display frame being released prematurely on support-mdma protocol */
    if (logic_fs->disp_state == FSP_DISP_STATE_IDLE && logic_fs->disp_fs != TD_NULL &&
        dec_over == TD_TRUE && logic_fs->disp_fs->fs_air_state == FS_AIR_STATE_IDLE &&
        ((logic_fs->logic_type == FSP_TYPE_LINK && logic_fs->is_ref == 0 && logic_fs->is_dec_ref == 0) ||
        logic_fs->logic_type != FSP_TYPE_LINK)) {
        logic_fs->disp_fs->is_use = 0;
        logic_fs->disp_fs = TD_NULL;
    }
}

STATIC td_void fsp_free_phy_fs_not_ref(fsp_logic *logic_fs, td_bool dec_over)
{
    if (logic_fs->is_ref == 0 &&
        logic_fs->is_dec_ref == 0 &&
        logic_fs->dec_fs != TD_NULL &&
        logic_fs->dec_fs->ref_cnt == 0 &&
        dec_over == TD_TRUE) {
        logic_fs->dec_fs->is_use = 0;
        logic_fs->dec_fs = TD_NULL;
        if (logic_fs->pmv_fs != TD_NULL) {
            logic_fs->pmv_fs->is_use = 0;
            logic_fs->pmv_fs = TD_NULL;
        }
        if (logic_fs->disp_fs != TD_NULL &&
            logic_fs->fs_air_state == FS_AIR_STATE_USE) {
            logic_fs->is_use = 0;
            logic_fs->disp_fs = TD_NULL;
        }
    }
}

STATIC td_void fsp_free_phy_fs(fsp_logic *logic_fs)
{
    td_bool dec_over;

    dec_over = fsp_is_dec_over(logic_fs);

    fsp_free_pmv_fs(logic_fs, dec_over);

    if (logic_fs->dec_fs != logic_fs->disp_fs) {
        if (logic_fs->logic_type == FSP_TYPE_SHARE) {
            fsp_free_phy_fs_type_share(logic_fs, dec_over);
        } else {
            fsp_free_phy_fs_type_other(logic_fs, dec_over);
        }
        fsp_free_phy_fs_not_ref(logic_fs, dec_over);
    } else {
        if (logic_fs->is_ref == 0 &&
            logic_fs->is_dec_ref == 0 &&
            logic_fs->dec_fs != TD_NULL &&
            logic_fs->dec_fs->ref_cnt == 0 &&
            dec_over == TD_TRUE && logic_fs->disp_state == FSP_DISP_STATE_IDLE) {
            logic_fs->dec_fs->is_use = 0;
            logic_fs->dec_fs = TD_NULL;
            logic_fs->disp_fs = TD_NULL;
        }
    }

    fsp_free_attach_fs(logic_fs);
}

td_bool fsp_is_logic_fs_free(const fsp_logic *logic_fs, td_bool is_disp_need)
{
    td_bool flag;

    vfmw_check_ret(logic_fs != TD_NULL, TD_FALSE);

    flag = (is_disp_need == TD_TRUE && logic_fs->logic_type == FSP_TYPE_LINK) ||
            (logic_fs->dec_fs == TD_NULL && logic_fs->pmv_fs == TD_NULL);

    if (logic_fs->is_ref == 0 &&
        logic_fs->is_in_voq == 0 &&
        logic_fs->disp_state == 0 &&
        logic_fs->is_dec_ref == 0 &&
        flag &&
        logic_fs->disp_fs == TD_NULL &&
        logic_fs->nor_fs == TD_NULL &&
        logic_fs->dec_state.data == 0) {
        return TD_TRUE;
    } else {
        return TD_FALSE;
    }
}

td_bool fsp_is_dec_fs_free(const fsp_chan *chan, const fsp_phy *dec_fs)
{
    td_bool buf_size_ok;
    fsp_entry *entry = fsp_get_entry();

    vfmw_check_ret(chan != TD_NULL, TD_FALSE);
    vfmw_check_ret(dec_fs != TD_NULL, TD_FALSE);
    vfmw_check_ret_no_print(entry->no_dec_fs == 0, TD_FALSE);

    if (chan->fs_mem.need.disp_info.is_disp_need == 0 && chan->attr.alloc_type == VFMW_FRM_ALLOC_OUTER) {
        buf_size_ok = ((td_s32)dec_fs->buf_size >= chan->fs_mem.wait_dec_size) ? TD_TRUE : TD_FALSE;
    } else {
        buf_size_ok = ((td_s32)dec_fs->buf_size == chan->fs_mem.wait_dec_size) ? TD_TRUE : TD_FALSE;
    }

    if ((dec_fs->is_valid == 1) &&
        (dec_fs->is_use == 0) &&
        (chan->fs_mem.wait_dec_size == 0 || buf_size_ok)) {
        return TD_TRUE;
    }

    return TD_FALSE;
}

td_bool fsp_is_disp_fs_free(const fsp_chan *chan, const fsp_phy *disp_fs)
{
    td_bool buf_size_ok;
    fsp_entry *entry = fsp_get_entry();

    vfmw_check_ret(chan != TD_NULL, TD_FALSE);
    vfmw_check_ret(disp_fs != TD_NULL, TD_FALSE);
    vfmw_check_ret_no_print(entry->no_dsp_fs == 0, TD_FALSE);

    if (chan->fs_mem.need.disp_info.is_disp_need != 0 && chan->attr.alloc_type == VFMW_FRM_ALLOC_OUTER) {
        buf_size_ok = ((td_s32)disp_fs->buf_size >= chan->fs_mem.wait_disp_size) ? TD_TRUE : TD_FALSE;
    } else {
        buf_size_ok = ((td_s32)disp_fs->buf_size == chan->fs_mem.wait_disp_size) ? TD_TRUE : TD_FALSE;
    }

    if ((disp_fs->is_valid == 1) &&
        (disp_fs->is_use == 0) &&
        (chan->fs_mem.wait_disp_size == 0 || buf_size_ok)) {
        return TD_TRUE;
    }

    return TD_FALSE;
}

td_bool fsp_is_pmv_free(const fsp_pmv *pmv_fs, td_u32 pmv_size)
{
    fsp_entry *entry = fsp_get_entry();

    vfmw_check_ret(pmv_fs != TD_NULL, TD_FALSE);
    vfmw_check_ret_no_print(entry->no_pmv_fs == 0, TD_FALSE);

    if ((pmv_fs->is_valid == 1) &&
        (pmv_fs->is_use == 0) &&
        (pmv_size == 0 || pmv_fs->buf_size == pmv_size)) {
        return TD_TRUE;
    } else {
        return TD_FALSE;
    }
}

td_bool fsp_is_attach_free(const fsp_attach *attach_fs)
{
    fsp_entry *entry = fsp_get_entry();

    vfmw_check_ret(attach_fs != TD_NULL, TD_FALSE);
    vfmw_check_ret_no_print(entry->no_att_fs == 0, TD_FALSE);

    if (attach_fs->is_valid == 1 &&
        attach_fs->is_use == 0 &&
        attach_fs->phy_addr != 0 &&
        attach_fs->vir_addr != 0 &&
        attach_fs->buf_size != 0) {
        return TD_TRUE;
    } else {
        return TD_FALSE;
    }
}

td_bool fsp_has_free_phy(td_s32 chan_id)
{
    td_s32 idx;
    fsp_phy *dec_fs = TD_NULL;
    fsp_pmv *pmv_fs = TD_NULL;
    fsp_chan *chan = TD_NULL;
    td_bool has_dec = TD_FALSE;
    td_bool has_pmv = TD_FALSE;
    td_ulong lock_flag;

    fsp_lock(chan_id, &lock_flag);

    chan = fsp_get_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
        fsp_unlock(chan_id, &lock_flag);
        return TD_FALSE;
    }

    for (idx = 0; idx < FSP_FRAME_NUM; idx++) {
        dec_fs = &chan->dec_fs[idx];
        if (dec_fs->is_valid == 0) {
            continue;
        }
        if (fsp_is_dec_fs_free(chan, dec_fs) == TD_TRUE) {
            has_dec = TD_TRUE;
            break;
        }
    }

    for (idx = 0; idx < FSP_PMV_NUM; idx++) {
        pmv_fs = &chan->pmv_fs[idx];
        if (pmv_fs->is_valid == 0) {
            continue;
        }
        if (fsp_is_pmv_free(pmv_fs, (td_u32)chan->fs_mem.wait_pmv_size) == TD_TRUE) {
            has_pmv = TD_TRUE;
            break;
        }
    }

    if (has_pmv == TD_TRUE && has_dec == TD_TRUE) {
        fsp_unlock(chan_id, &lock_flag);
        return TD_TRUE;
    } else {
        fsp_unlock(chan_id, &lock_flag);
        return TD_FALSE;
    }
}

STATIC td_bool fsp_has_free_dec(td_s32 chan_id)
{
    td_s32 idx;
    td_bool has_dec = TD_FALSE;
    fsp_chan *chan = TD_NULL;

    chan = fsp_get_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
        return TD_FALSE;
    }

    for (idx = 0; idx < FSP_FRAME_NUM; idx++) {
        fsp_phy *dec_fs = &chan->dec_fs[idx];
        if (dec_fs->is_valid == 0) {
            continue;
        }
        if (fsp_is_dec_fs_free(chan, dec_fs) == TD_TRUE) {
            has_dec = TD_TRUE;
            break;
        }
    }

    if (has_dec == TD_TRUE) {
        return TD_TRUE;
    } else {
        return TD_FALSE;
    }
}

STATIC td_bool fsp_has_free_pmv(td_s32 chan_id)
{
    td_s32 idx;
    td_bool has_pmv = TD_FALSE;
    fsp_chan *chan = TD_NULL;

    chan = fsp_get_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
        return TD_FALSE;
    }

    for (idx = 0; idx < FSP_PMV_NUM; idx++) {
        fsp_pmv *pmv_fs = &chan->pmv_fs[idx];
        if (pmv_fs->is_valid == 0) {
            continue;
        }
        if (fsp_is_pmv_free(pmv_fs, (td_u32)chan->fs_mem.wait_pmv_size) == TD_TRUE) {
            has_pmv = TD_TRUE;
            break;
        }
    }

    if (has_pmv == TD_TRUE) {
        return TD_TRUE;
    }

    return TD_FALSE;
}

STATIC td_bool fsp_has_free_attach(td_s32 chan_id)
{
    td_s32 idx;
    fsp_chan *chan = TD_NULL;
    td_bool is_free;

    chan = fsp_get_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
        return TD_FALSE;
    }

    for (idx = 0; idx < FSP_FRAME_NUM; idx++) {
        fsp_attach *attach = &chan->nor_fs[idx];
        if (attach->is_valid == 0) {
            continue;
        }
        is_free = fsp_is_attach_free(attach);
        if (is_free == TD_TRUE) {
            return TD_TRUE;
        }
    }

    return TD_FALSE;
}

#define MAX_WAIT_FRM 4
#define FSP_DPS_NUM  MAX_DPS_NUM

typedef struct {
    td_u32 dec_num[FSP_DPS_NUM];
    td_u32 pmv_num[FSP_DPS_NUM];
    td_u32 disp_num[FSP_DPS_NUM];
    td_u32 dec_size[FSP_DPS_NUM];
    td_u32 pmv_size[FSP_DPS_NUM];
    td_u32 disp_size[FSP_DPS_NUM];
} fsp_rels_part_use_info;

STATIC td_void fsp_req_attach(const fsp_chan *chan)
{
    vfmw_check(chan != TD_NULL);

    if (chan->fs_mem.need.sec_size > 0) {
        fsp_require_fs(chan, FSP_MEM_TYPE_SEC, chan->fs_mem.need.sec_size);
    }
}

STATIC td_void fsp_set_fs_name(vfmw_mem_report *mem_rep, const char *name)
{
    vfmw_check(mem_rep != TD_NULL);

    if (snprintf_s(mem_rep->buf_name, VFMW_REPORT_MAX_NAME_LEN,
                   VFMW_REPORT_MAX_NAME_LEN - 1, "%s", name) < 0) {
        dprint(PRN_ERROR, "%s %d sec func err\n", __func__, __LINE__);
    }
    mem_rep->buf_name[sizeof(mem_rep->buf_name) - 1] = '\0';
}

td_void fsp_require_fs(const fsp_chan *chan, fsp_mem_type type, td_u32 size)
{
    td_bool is_map = TD_TRUE;
    td_bool is_need_attach = TD_FALSE;
    td_u64 unid;
    vfmw_mem_report mem_rep;

    vfmw_check(chan != TD_NULL);
    vfmw_check_sec_func(memset_s(&mem_rep, sizeof(vfmw_mem_report), 0, sizeof(vfmw_mem_report)));

    mem_rep.sec_flag = chan->attr.sec_flag;

    switch (type) {
        case FSP_MEM_TYPE_DEC:
            fsp_set_fs_name(&mem_rep, DEC_BUF_NAME);
            is_need_attach = (chan->fs_mem.need.attach == ATTACH_DEC) ? TD_TRUE : TD_FALSE;
            break;

        case FSP_MEM_TYPE_DISP:
            fsp_set_fs_name(&mem_rep, DISP_BUF_NAME);
            is_need_attach = (chan->fs_mem.need.attach == ATTACH_DISP) ? TD_TRUE : TD_FALSE;
            break;

        case FSP_MEM_TYPE_PMV:
            is_map = TD_FALSE;
            fsp_set_fs_name(&mem_rep, PMV_BUF_NAME);
            break;

        case FSP_MEM_TYPE_SEC:
            is_map = TD_TRUE; /* if store will conflict with FSP_MEM_TYPE_NOR */
            fsp_set_fs_name(&mem_rep, SEC_BUF_NAME);
            break;

        default:
            dprint(PRN_ERROR, "%s unsupport type %d\n", __func__, type);
    }

    unid = set_chan_uid(chan->unid, (td_u32)chan->id);

    mem_rep.is_map = is_map;
    mem_rep.size = size;
    mem_rep.unique_id = (td_s64)set_mem_uid(unid, VFMW_MEM_GROUP_FSP, type);

    report_require_alloc_mem(chan->id, &mem_rep);

    if (is_need_attach == TD_TRUE) {
        fsp_req_attach(chan);
    }
}

STATIC td_s32 fsp_report_disp_para(const fsp_chan *chan, td_u32 *para, td_u32 size)
{
    td_u32 idx;

    vfmw_check_ret(chan != TD_NULL, FSP_ERR);
    vfmw_check_ret(para != TD_NULL, FSP_ERR);

    if (size < FSP_OUTER_PARA_SIZE) {
        dprint(PRN_ERROR, "%s report disp para err\n", __func__);
        return FSP_ERR;
    }

    idx = 0;
    para[idx++] = chan->fs_mem.need.disp_info.disp_size;
    para[idx++] = chan->fs_mem.need.disp_info.disp_num;
    para[idx++] = chan->fs_mem.need.disp_info.disp_width;
    para[idx++] = chan->fs_mem.need.disp_info.disp_height;
    para[idx++] = chan->fs_mem.need.disp_info.disp_stride;
    para[idx++] = chan->fs_mem.need.disp_info.bit_depth;
    para[idx++] = chan->cfg.disp_width;
    para[idx++] = chan->cfg.disp_height;
    para[idx++] = (td_u32)chan->fs_mem.need.disp_info.compress_en;
    para[idx++] = chan->cfg.splice_info.ori_width;
    para[idx++] = chan->cfg.splice_info.ori_height;
    para[idx++] = (td_u32)chan->cfg.splice_info.type;

    return FSP_OK;
}

STATIC td_s32 fsp_report_dec_para(const fsp_chan *chan, td_u32 *para, td_u32 size)
{
    td_u32 idx;

    if (size < FSP_OUTER_PARA_SIZE) {
        dprint(PRN_ERROR, "%s report disp para err\n", __func__);
        return FSP_ERR;
    }

    idx = 0;
    para[idx++] = chan->fs_mem.need.dec_info.dec_size;
    para[idx++] = chan->fs_mem.need.dec_info.dec_num;
    para[idx++] = chan->fs_mem.need.dec_info.dec_width;
    para[idx++] = chan->fs_mem.need.dec_info.dec_height;
    para[idx++] = chan->fs_mem.need.dec_info.dec_stride;
    para[idx++] = chan->fs_mem.need.dec_info.bit_depth;
    para[idx++] = chan->cfg.disp_width;
    para[idx++] = chan->cfg.disp_height;
    para[idx++] = (td_u32)chan->fs_mem.need.dec_info.compress_en;
    para[idx++] = chan->cfg.splice_info.ori_width;
    para[idx++] = chan->cfg.splice_info.ori_height;
    para[idx++] = (td_u32)chan->cfg.splice_info.type;

    return FSP_OK;
}

STATIC td_s32 fsp_report_req_outer(fsp_chan *chan, const fsp_report *report)
{
    td_u32 para[FSP_OUTER_PARA_SIZE] = {0};
    td_bool change_flag;
    mem_need_info *need = &chan->fs_mem.need;
    mem_need_info *rep = &chan->fs_mem.report;

    vfmw_unused(report);
    vfmw_check_ret_no_print(chan->attr.alloc_type == VFMW_FRM_ALLOC_OUTER, FSP_OK);

    change_flag = (need->dec_info.dec_size != rep->dec_info.dec_size)       ||
                  (need->dec_info.dec_width != rep->dec_info.dec_width)     ||
                  (need->dec_info.dec_num != rep->dec_info.dec_num)         ||
                  (need->disp_info.disp_size != rep->disp_info.disp_size)   ||
                  (need->disp_info.disp_width != rep->disp_info.disp_width) ||
                  (need->disp_info.disp_num != rep->disp_info.disp_num);
    if (change_flag) {
        if (need->disp_info.is_disp_need == TD_TRUE) {
            vfmw_check_ret(fsp_report_disp_para(chan, para, FSP_OUTER_PARA_SIZE) != FSP_ERR, FSP_ERR);
            chan->fs_mem.apply_disp_num += chan->fs_mem.need.disp_info.disp_num;
            chan->fs_mem.need.nor_num = (chan->fs_mem.need.nor_size != 0) ?
                chan->fs_mem.need.disp_info.disp_num : 0;
            fsp_rels_all_dec(chan);
        } else {
            vfmw_check_ret(fsp_report_dec_para(chan, para, FSP_OUTER_PARA_SIZE) != FSP_ERR, FSP_ERR);
            chan->fs_mem.apply_dec_num += chan->fs_mem.need.dec_info.dec_num;
            chan->fs_mem.need.nor_num = (chan->fs_mem.need.nor_size != 0) ?
                chan->fs_mem.need.dec_info.dec_num : 0;
        }

        fsp_clear_unused_pmv(chan);
        vfmw_check_ret(para[0] != 0, FSP_ERR);

        if (chan->attr.alloc_type == VFMW_FRM_ALLOC_OUTER && chan->attr.outer_frm_mode == VFMW_OUTER_FRM_WAIT) {
            chan->wait_unbind = 1;
        }

        report_resolution_change(chan->id, para);
        vfmw_check_sec_func(memcpy_s(&chan->fs_mem.report, sizeof(chan->fs_mem.report),
            &chan->fs_mem.need, sizeof(mem_need_info)));
    }

    return FSP_OK;
}

STATIC td_s32 fsp_alloc_mem(td_s32 chan_id, fsp_mem_type type, td_u32 dec_size, td_u32 idx, vfmw_mem_report *mem)
{
    mem_record mem_tmp = { 0 };
#ifdef VFMW_CHECK_MEM_SUPPORT
    td_s32 ret;
    td_char name[FSP_NAME_LEN];
#else
    td_char *name = TD_NULL;
#endif

    if (type == FSP_MEM_TYPE_DEC) {
#ifdef VFMW_CHECK_MEM_SUPPORT
        ret = snprintf_s(name, FSP_NAME_LEN, FSP_NAME_LEN - 1, "vfmw_dec_mem_%u", idx);
        vfmw_check_ret(ret >= 0, FSP_ERR);
#else
        vfmw_unused(idx);
        name = "vfmw_dec_mem";
#endif
    } else if (type == FSP_MEM_TYPE_PMV) {
#ifdef VFMW_CHECK_MEM_SUPPORT
        ret = snprintf_s(name, FSP_NAME_LEN, FSP_NAME_LEN - 1, "vfmw_pmv_mem_%u", idx);
        vfmw_check_ret(ret >= 0, FSP_ERR);
#else
        vfmw_unused(idx);
        name = "vfmw_pmv_mem";
#endif
    } else {
        dprint(PRN_ERROR, "%s, %d, unsupport type %d err\n", __func__, __LINE__, type);
        return FSP_ERR;
    }

    mem_tmp.length = dec_size;
    mem_tmp.is_cached = 0;
    mem_tmp.mem_name = name;
    mem_tmp.unid = (td_u32)chan_id;
    vfmw_check_ret(os_malloc != TD_NULL, FSP_ERR);
    if (os_malloc(&mem_tmp) != OSAL_OK) {
        dprint(PRN_ERROR, "%s, %d, os_malloc err\n", __func__, __LINE__);
        return FSP_ERR;
    }

    vfmw_check_sec_func(strcpy_s(mem->buf_name, VFMW_REPORT_MAX_NAME_LEN, mem_tmp.mem_name));
    mem->is_map = 1;
    mem->phy_addr = mem_tmp.phy_addr;
    mem->vir_addr = ptr_uint64(mem_tmp.vir_addr);
    mem->size = mem_tmp.length;

    return FSP_OK;
}

STATIC td_void fsp_free_mem(UADDR phy_addr)
{
    mem_record mem = { 0 };

    if (phy_addr != 0) {
        mem.phy_addr = phy_addr;
        mem.vir_addr = (td_u8 *)uint64_ptr(phy_addr);
        mem.is_cached = 0;
        os_free(&mem);
    }
}

STATIC td_void fsp_rls_dec_pmv_fs(fsp_chan *chan)
{
    int i;
    fsp_phy *phy_fs;
    fsp_pmv *pmv_fs;

    for (i = 0; i < FSP_FRAME_NUM; i++) {
        phy_fs = &chan->dec_fs[i];
        if (phy_fs->is_valid != 1) {
            continue;
        }
        fsp_free_mem(phy_fs->phy_addr);
        phy_fs->is_valid = 0;
    }
    chan->fs_mem.have_dec_num = 0;
    chan->fs_mem.total_dec_num = 0;

    for (i = 0; i < FSP_PMV_NUM; i++) {
        pmv_fs = &chan->pmv_fs[i];
        if (pmv_fs->is_valid != 1) {
            continue;
        }
        fsp_free_mem(pmv_fs->phy_addr);
        pmv_fs->is_valid = 0;
    }
    chan->fs_mem.have_pmv_num = 0;
    chan->fs_mem.total_pmv_num = 0;
}

STATIC td_s32 fsp_report_req(fsp_chan *chan, const fsp_report *report)
{
    td_u32 i;
    td_u32 max_num = 0;
    td_u32 req_dec_num = report->dec_num;
    td_u32 req_disp_num = report->disp_num;
    td_u32 req_pmv_num = report->pmv_num;
    mem_need_info *need = &chan->fs_mem.need;
    vfmw_mem_report mem = { 0 };
    td_s32 ret = FSP_ERR;

    if (fsp_report_req_outer(chan, report) == FSP_ERR) {
        dprint(PRN_ERROR, "%s report outer err\n", __func__);
        return FSP_ERR;
    }

    if (chan->attr.alloc_type == VFMW_FRM_ALLOC_OUTER) {
        if (need->disp_info.is_disp_need == TD_TRUE) {
            req_disp_num = 0;
        } else {
            req_dec_num = 0;
        }
    }

    max_num = vfmw_max(max_num, req_dec_num);
    max_num = vfmw_max(max_num, req_disp_num);
    max_num = vfmw_max(max_num, req_pmv_num);

    for (i = 0; i < max_num; i++) {
        if (req_dec_num > 0) {
            ret = fsp_alloc_mem(chan->id, FSP_MEM_TYPE_DEC, need->dec_info.dec_size, chan->fs_mem.apply_dec_num, &mem);
            vfmw_check_ret(ret == FSP_OK, FSP_ERR);
            ret = fsp_add_dec_fs(chan, &mem, TD_NULL, TD_NULL);
            vfmw_check_ret(ret == FSP_OK, FSP_ERR);
            req_dec_num--;
            chan->fs_mem.apply_dec_num++;
        }

        if (req_disp_num > 0) {
            /* unsupport yet */
            dprint(PRN_ERROR, "%s %d alloc disp err\n", __func__, __LINE__);
            return FSP_ERR;
        }

        if (req_pmv_num > 0) {
            ret = fsp_alloc_mem(chan->id, FSP_MEM_TYPE_PMV, need->pmv_size, chan->fs_mem.apply_pmv_num, &mem);
            vfmw_check_ret(ret == FSP_OK, FSP_ERR);

            ret = fsp_add_pmv_fs(chan, &mem);
            vfmw_check_ret(ret == FSP_OK, FSP_ERR);
            req_pmv_num--;
            chan->fs_mem.apply_pmv_num++;
        }
    }

    return FSP_OK;
}

STATIC td_s32 fsp_add_pmv_fs(fsp_chan *chan, const vfmw_mem_report *mem)
{
    td_s32 cnt;
    td_s32 add_pos = -1;
    fsp_pmv *pmv_fs = TD_NULL;

    for (cnt = 0; cnt < FSP_PMV_NUM; cnt++) {
        if (chan->pmv_fs[cnt].is_valid == 0) {
            add_pos = cnt;
            break;
        }
    }

    if (add_pos == -1) {
        return FSP_ERR;
    }

    pmv_fs = &(chan->pmv_fs[add_pos]);

    vfmw_check_sec_func(memset_s(pmv_fs, sizeof(fsp_pmv), 0, sizeof(fsp_pmv)));

    pmv_fs->is_valid = 1;
    pmv_fs->pmv_id = (td_u8)add_pos;
    pmv_fs->is_map = mem->is_map;
    pmv_fs->dma_buf = mem->dma_buf;
    pmv_fs->phy_addr = mem->phy_addr;
    pmv_fs->vir_addr = mem->vir_addr;
    pmv_fs->buf_size = mem->size;
    pmv_fs->sec_info = mem->sec_info;

    chan->fs_mem.total_pmv_num++;
    if (mem->size == chan->fs_mem.need.pmv_size) {
        chan->fs_mem.have_pmv_num++;
    }

    return FSP_OK;
}

STATIC td_s32 fsp_add_nor_fs(fsp_chan *chan, const vfmw_mem_attach *mem)
{
    td_s32 cnt;
    fsp_attach *attach_fs = TD_NULL;
    td_u32 nor_size = VFMW_META_USERDATA_SIZE;

    for (cnt = 0; cnt < FSP_FRAME_NUM; cnt++) {
        if (chan->nor_fs[cnt].is_valid == 0) {
            attach_fs = &(chan->nor_fs[cnt]);
            vfmw_check_sec_func(memset_s(attach_fs, sizeof(fsp_attach), 0, sizeof(fsp_attach)));
            attach_fs->is_valid = 1;
            attach_fs->fs_id = cnt;
            attach_fs->mem_fd = mem->fd;
            attach_fs->dma_buf = mem->dma_buf;
            attach_fs->phy_addr = mem->phy_addr + nor_size * (td_u32)cnt;
            attach_fs->vir_addr = mem->vir_addr + nor_size * (td_u32)cnt;
            attach_fs->buf_size = nor_size;
            attach_fs->fd_offset = nor_size * (td_u32)cnt;
        } else {
            return FSP_ERR;
        }
    }

    return FSP_OK;
}

STATIC td_s32 fsp_add_sec_fs(fsp_chan *chan, const vfmw_mem_report *mem)
{
    td_s32 cnt;
    td_s32 add_pos = -1;
    fsp_attach *attach_fs = TD_NULL;

    for (cnt = 0; cnt < FSP_FRAME_NUM; cnt++) {
        if (chan->sec_fs[cnt].is_valid == 0) {
            add_pos = cnt;
            break;
        }
    }

    if (add_pos == -1) {
        return FSP_ERR;
    }

    attach_fs = &(chan->sec_fs[add_pos]);
    if (attach_fs->is_valid == 1) {
        dprint(PRN_ERROR, "%s WARNNING: sec_fs[%d](0x%x) not free yet!\n", __func__, add_pos, attach_fs->phy_addr);
    }

    vfmw_check_sec_func(memset_s(attach_fs, sizeof(fsp_attach), 0, sizeof(fsp_attach)));

    attach_fs->is_valid = 1;
    attach_fs->fs_id = add_pos;
    attach_fs->mem_fd = mem->mem_fd;
    attach_fs->dma_buf = mem->dma_buf;
    attach_fs->phy_addr = mem->phy_addr;
    attach_fs->vir_addr = mem->vir_addr;
    attach_fs->buf_size = mem->size;
    attach_fs->sec_info = mem->sec_info;

    return FSP_OK;
}

td_s32 fsp_set_frame_buf_mode(td_s32 chan_id, vfmw_frm_alloc_type type)
{
    td_s32 ret;
    td_ulong lock_flag;
    fsp_chan *chan = TD_NULL;

    ret = FSP_ERR;

    fsp_lock(chan_id, &lock_flag);
    fsp_assert_mod_goto(out);
    chan = fsp_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    chan->attr.alloc_type = type;
    ret = FSP_OK;
out:
    fsp_unlock(chan_id, &lock_flag);

    return ret;
}

STATIC td_s32 fsp_add_disp_fs(fsp_chan *chan, const vfmw_mem_report *disp_mem, const vfmw_mem_report *info_mem,
                              const vfmw_mem_report *hfbc_mem)
{
    td_s32 cnt;
    td_s32 add_pos = -1;
    fsp_phy *phy_fs = TD_NULL;

    vfmw_unused(hfbc_mem);

    for (cnt = 0; cnt < FSP_FRAME_NUM; cnt++) {
        if (chan->disp_fs[cnt].is_valid == 0) {
            add_pos = cnt;
            break;
        }
    }

    if (add_pos == -1) {
        return FSP_ERR;
    }

    phy_fs = &(chan->disp_fs[add_pos]);

    vfmw_check_sec_func(memset_s(phy_fs, sizeof(fsp_phy), 0, sizeof(fsp_phy)));

    phy_fs->is_valid = 1;
    phy_fs->fs_id = add_pos;
    phy_fs->is_map = disp_mem->is_map;
    phy_fs->phy_addr = disp_mem->phy_addr;
    phy_fs->vir_addr = disp_mem->vir_addr;
    phy_fs->buf_size = disp_mem->size;
    phy_fs->need_attach = (chan->fs_mem.need.attach == ATTACH_DISP) ? TD_TRUE : TD_FALSE;
    phy_fs->sec_info = disp_mem->sec_info;

    chan->fs_mem.total_disp_num++;
    if (disp_mem->size == chan->fs_mem.need.disp_info.disp_size) {
        chan->fs_mem.have_disp_num++;
    }

    if (info_mem != TD_NULL) {
        phy_fs->info_phy_addr = info_mem->phy_addr;
        phy_fs->info_vir_addr = info_mem->vir_addr;
        phy_fs->info_buf_size = info_mem->size;
        phy_fs->info_offset = info_mem->offset;
    }

    return FSP_OK;
}

td_s32 fsp_bind_req_fs(td_s32 chan_id, const vfmw_mem_report *mem)
{
    td_s32 ret = FSP_ERR;
    td_ulong lock_flag;
    td_u64 unique_id_tmp;
    td_u64 unid;
    fsp_chan *chan = TD_NULL;
    fsp_mem_type mem_type;

    vfmw_check_ret(mem != TD_NULL, FSP_ERR);

    fsp_lock(chan_id, &lock_flag);

    fsp_assert_mod_goto(out);

    chan = fsp_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    unique_id_tmp = (td_u64)(mem->unique_id);

    unid = set_chan_uid(chan->unid, (td_u32)chan->id);
    if (unid != get_chan_uid(unique_id_tmp)) {
        dprint(PRN_ERROR, "%s Fs Chan ID %llx != Chan ID %llx\n", __func__, get_chan_uid(unique_id_tmp), unid);
        goto out;
    }

    mem_type = get_mem_id_by_uid(unique_id_tmp);

    if (mem->is_map && mem->vir_addr == 0) {
        dprint(PRN_ERROR, "%s ERROR: Buf %s vir_addr=0\n", __func__, mem->buf_name);
    }

    switch (mem_type) {
        case FSP_MEM_TYPE_DEC:
            ret = fsp_add_dec_fs(chan, mem, TD_NULL, TD_NULL);
            break;

        case FSP_MEM_TYPE_DISP:
            ret = fsp_add_disp_fs(chan, mem, TD_NULL, TD_NULL);
            break;

        case FSP_MEM_TYPE_PMV:
            ret = fsp_add_pmv_fs(chan, mem);
            break;

        case FSP_MEM_TYPE_SEC:
            ret = fsp_add_sec_fs(chan, mem);
            break;

        default:
            dprint(PRN_ERROR, "%s unkown buffer %s\n", __func__, mem->buf_name);
            break;
    }

out:

    fsp_unlock(chan_id, &lock_flag);

    return ret;
}

td_s32 fsp_bind_attach_fs(td_s32 chan_id, const vfmw_mem_attach *mem)
{
    td_s32 ret;
    td_ulong lock_flag;
    fsp_chan *chan = TD_NULL;

    vfmw_check_ret(mem != TD_NULL, FSP_ERR);

    ret = FSP_ERR;
    fsp_lock(chan_id, &lock_flag);

    fsp_assert_mod_goto(out);

    chan = fsp_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    switch (mem->type) {
        case NOR_MEM_TYPE:
            ret = fsp_add_nor_fs(chan, mem);
            break;

        default:
            dprint(PRN_ERROR, "%s unkown buffer %d\n", __func__, mem->type);
            break;
    }

out:

    fsp_unlock(chan_id, &lock_flag);

    return ret;
}

STATIC td_void fsp_rels_info_dec(const fsp_phy *phy_fs, vfmw_mem_report *mem_rep)
{
    vfmw_check(phy_fs != TD_NULL);
    vfmw_check(mem_rep != TD_NULL);

    mem_rep->is_cache = 0;
    mem_rep->is_map = phy_fs->is_map;
    mem_rep->size = phy_fs->buf_size;
    mem_rep->dma_buf = phy_fs->dma_buf;
    mem_rep->phy_addr = phy_fs->phy_addr;
    mem_rep->vir_addr = (td_ulong)(phy_fs->vir_addr);
    fsp_set_fs_name(mem_rep, DEC_BUF_NAME);
}

STATIC  td_void fsp_rels_info_disp(const fsp_phy *phy_fs, vfmw_mem_report *mem_rep)
{
    if (phy_fs == TD_NULL) {
        dprint(PRN_ERROR, "phy_fs is null.\n");
        return;
    }

    mem_rep->is_cache = 0;
    mem_rep->is_map = phy_fs->is_map;
    mem_rep->size = phy_fs->buf_size;
    mem_rep->dma_buf = phy_fs->dma_buf;
    mem_rep->phy_addr = phy_fs->phy_addr;
    mem_rep->vir_addr = (td_ulong)(phy_fs->vir_addr);
    fsp_set_fs_name(mem_rep, DISP_BUF_NAME);
}

STATIC td_void fsp_rels_info_pmv(const fsp_pmv *pmv_fs, vfmw_mem_report *mem_rep)
{
    mem_rep->is_cache = 0;
    mem_rep->is_map = pmv_fs->is_map;
    mem_rep->size = pmv_fs->buf_size;
    mem_rep->dma_buf = pmv_fs->dma_buf;
    mem_rep->phy_addr = pmv_fs->phy_addr;
    mem_rep->vir_addr = (td_ulong)(pmv_fs->vir_addr);
    fsp_set_fs_name(mem_rep, PMV_BUF_NAME);
}

STATIC td_void fsp_rels_info_nor(const fsp_attach *attach_fs, vfmw_mem_report *mem_rep)
{
    mem_rep->is_cache = 0;
    mem_rep->sec_flag = 0;
    mem_rep->is_map = TD_TRUE;
    mem_rep->size = attach_fs->buf_size;
    mem_rep->dma_buf = attach_fs->dma_buf;
    mem_rep->phy_addr = attach_fs->phy_addr;
    mem_rep->vir_addr = (td_ulong)(attach_fs->vir_addr);
    fsp_set_fs_name(mem_rep, NOR_BUF_NAME);
}

STATIC td_void fsp_rels_info_sec(const fsp_attach *attach_fs, vfmw_mem_report *mem_rep)
{
    mem_rep->is_cache = 0;
    mem_rep->is_map = TD_TRUE;
    mem_rep->size = attach_fs->buf_size;
    mem_rep->dma_buf = attach_fs->dma_buf;
    mem_rep->phy_addr = attach_fs->phy_addr;
    mem_rep->vir_addr = (td_ulong)(attach_fs->vir_addr);
    fsp_set_fs_name(mem_rep, SEC_BUF_NAME);
}

STATIC td_void fsp_release_fs_mem(const fsp_chan *chan, const vfmw_mem_report *mem_rep, fsp_mem_type type)
{
    if (chan->attr.alloc_type != VFMW_FRM_ALLOC_OUTER ||
        (chan->fs_mem.need.disp_info.is_disp_need == TD_TRUE && type != FSP_MEM_TYPE_DISP) ||
        (chan->fs_mem.need.disp_info.is_disp_need == TD_FALSE && type != FSP_MEM_TYPE_DEC)) {
#ifdef VFMW_VMM_SUPPORT
        report_require_release_mem(chan->id, mem_rep);
#else
        fsp_free_mem(mem_rep->phy_addr);
#endif
    }
}

td_void fsp_release_fs(const fsp_chan *chan, fsp_mem_type type, const td_void *args)
{
    td_u64 unid;
    vfmw_mem_report mem_rep;
    fsp_phy *phy_fs = TD_NULL;
    fsp_pmv *pmv_fs = TD_NULL;
    fsp_attach *attach_fs = TD_NULL;

    vfmw_check(chan != TD_NULL);
    vfmw_check(args != TD_NULL);

    phy_fs = (fsp_phy *)args;
    pmv_fs = (fsp_pmv *)args;
    attach_fs = (fsp_attach *)args;

    vfmw_check_sec_func(memset_s(&mem_rep, sizeof(mem_rep), 0, sizeof(mem_rep)));
    mem_rep.sec_flag = chan->attr.sec_flag;

    switch (type) {
        case FSP_MEM_TYPE_DEC: {
            fsp_rels_info_dec(phy_fs, &mem_rep);
            phy_fs->is_valid = 0;
            break;
        }

        case FSP_MEM_TYPE_DISP: {
            fsp_rels_info_disp(phy_fs, &mem_rep);
            phy_fs->is_valid = 0;
            break;
        }

        case FSP_MEM_TYPE_PMV: {
            fsp_rels_info_pmv(pmv_fs, &mem_rep);
            pmv_fs->is_valid = 0;
            break;
        }

        case FSP_MEM_TYPE_NOR: {
            fsp_rels_info_nor(attach_fs, &mem_rep);
            attach_fs->is_valid = 0;
            break;
        }

        case FSP_MEM_TYPE_SEC: {
            fsp_rels_info_sec(attach_fs, &mem_rep);
            attach_fs->is_valid = 0;
            break;
        }

        default:
            dprint(PRN_ERROR, "%s unsupport type %d\n", __func__, type);
    }

    unid = set_chan_uid(chan->unid, (td_u32)chan->id);
    mem_rep.unique_id = (td_s64)set_mem_uid(unid, VFMW_MEM_GROUP_FSP, type);

    fsp_release_fs_mem(chan, &mem_rep, type);
}

STATIC td_u32 fsp_get_dec_num(td_s32 chan_id, td_u32 dec_size)
{
    td_u32 num = 0;
    td_s32 cnt;
    fsp_chan *chan = TD_NULL;
    fsp_phy *dec_fs = TD_NULL;

    chan = fsp_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    for (cnt = 0; cnt < FSP_FRAME_NUM; cnt++) {
        dec_fs = chan->dec_fs + cnt;
        if (dec_fs->is_valid == 1) {
            if ((dec_size == 0) || (dec_fs->buf_size == dec_size)) {
                num++;
            }
        }
    }

out:

    return num;
}

STATIC td_u32 fsp_get_pmv_num(td_s32 chan_id, td_u32 pmv_size)
{
    td_u32 num = 0;
    td_s32 cnt;
    fsp_chan *chan = TD_NULL;
    fsp_pmv *pmv_fs = TD_NULL;

    chan = fsp_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    for (cnt = 0; cnt < FSP_PMV_NUM; cnt++) {
        pmv_fs = chan->pmv_fs + cnt;
        if (pmv_fs->is_valid == 1) {
            if (pmv_size == 0 || pmv_fs->buf_size == pmv_size) {
                num++;
            }
        }
    }

out:

    return num;
}

STATIC td_u32 fsp_get_disp_num(td_s32 chan_id, td_s32 disp_size)
{
    td_u32 num = 0;
    td_s32 cnt;
    fsp_chan *chan = TD_NULL;
    fsp_phy *disp_fs = TD_NULL;

    chan = fsp_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    for (cnt = 0; cnt < FSP_FRAME_NUM; cnt++) {
        disp_fs = chan->disp_fs + cnt;
        if (disp_fs->is_valid == 1) {
            if (disp_size == 0 || (td_s32)disp_fs->buf_size == disp_size) {
                num++;
            }
        }
    }

out:

    return num;
}

STATIC td_u32 fsp_get_nor_num(td_s32 chan_id, td_u32 nor_size)
{
    td_u32 num = 0;
    td_s32 cnt;
    fsp_chan *chan = TD_NULL;
    fsp_attach *nor_fs = TD_NULL;

    chan = fsp_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    for (cnt = 0; cnt < FSP_FRAME_NUM; cnt++) {
        nor_fs = chan->nor_fs + cnt;
        if (nor_fs->is_valid == 1) {
            if (nor_size == 0 || nor_fs->buf_size == nor_size) {
                num++;
            }
        }
    }

out:

    return num;
}

STATIC td_s32 fsp_if_need_report_dec_num(const mem_need_info *need, fsp_chan *chan, fsp_report *report)
{
    td_s32 if_report = 0;
    td_u32 fs_num;

    if (need->dec_info.dec_size != chan->fs_mem.need.dec_info.dec_size ||
        need->dec_info.dec_width != chan->fs_mem.need.dec_info.dec_width ||
        need->dec_info.dec_num > chan->fs_mem.need.dec_info.dec_num) {
        fs_num = fsp_get_dec_num(chan->id, need->dec_info.dec_size);
        chan->fs_mem.have_dec_num = fs_num;

        if (need->dec_info.dec_size != chan->fs_mem.need.dec_info.dec_size) {
            chan->fs_mem.apply_dec_num = fs_num;
        }

        if (need->dec_info.dec_width != chan->fs_mem.need.dec_info.dec_width) {
            chan->fs_mem.apply_dec_num = 0;
            chan->fs_mem.have_dec_num = 0;
        }

        if (chan->fs_mem.apply_dec_num < need->dec_info.dec_num) {
            if_report = 1;
            report->dec_num = chan->attr.alloc_type == VFMW_FRM_ALLOC_OUTER ? need->dec_info.dec_num : 1;
        }
    }

    return if_report;
}

STATIC td_void fsp_get_mem_req(const fsp_chan *chan, const fsp_logic *logic, mem_need_info *mem_need)
{
    vfmw_vid_std vid_std;
    td_u32 dec_width, dec_height, disp_width, disp_height, bit_depth, need_mtdt;
    td_u32 need_dec_num, need_pmv_num, need_disp_num;
    mem_req_info mem_req = { 0 };

    vid_std = (logic == TD_NULL) ? chan->cfg.vid_std : logic->image.vid_std;
    dec_width = (logic == TD_NULL) ? chan->cfg.dec_width : logic->image.image_width;
    dec_height = (logic == TD_NULL) ? chan->cfg.dec_height : logic->image.image_height;
    disp_height = (logic == TD_NULL) ? chan->cfg.disp_height : logic->image.disp_info.disp_height;
    disp_width = (logic == TD_NULL) ? chan->cfg.disp_width : logic->image.disp_info.disp_width;
    bit_depth = (logic == TD_NULL) ? chan->cfg.bit_depth : logic->image.bit_depth;
    need_dec_num = (logic == TD_NULL) ? chan->cfg.need_dec_num : 0;
    need_pmv_num = (logic == TD_NULL) ? chan->cfg.need_pmv_num : 0;
    need_disp_num = (logic == TD_NULL) ? chan->cfg.need_disp_num : 0;
    need_mtdt = (logic == TD_NULL) ? chan->cfg.need_mtdt : 0;

    mem_req.chan_id = chan->id;
    mem_req.extra_fs_num = chan->attr.extra_fs_num;
    mem_req.add_extra = (chan->attr.perf == FSP_HIGH_PERF) ? TD_TRUE : TD_FALSE;
    mem_req.cmp_mode = chan->attr.vcmp_mode;
    mem_req.vid_std = vid_std;
    mem_req.dec_width = dec_width;
    mem_req.dec_height = dec_height;
    mem_req.disp_height = disp_height;
    mem_req.disp_width = disp_width;
    mem_req.bit_depth = bit_depth;
    mem_req.need_dec_num = need_dec_num;
    mem_req.need_pmv_num = need_pmv_num;
    mem_req.need_disp_num = need_disp_num;
    mem_req.need_mtdt = need_mtdt;
    mem_req.disp_bit_depth_mode = chan->attr.disp_bit_depth_mode;
    mem_req.frm_layout = chan->attr.frm_layout;
    mem_req.need_film_grain = chan->attr.need_film_grain;
    mem_req.alloc_type = chan->attr.alloc_type;
    if (chan->attr.air_version == AIR_DEC_FRAME) {
        mem_req.cmp_mode = VFMW_CMP_OFF;
        mem_req.frm_layout = VFMW_FRM_LAYOUT_LINEAR;
    }
    mem_req.is_max_buf_size_en = chan->attr.is_max_buf_size_en;
    mem_req.max_dec_width = chan->attr.max_dec_width;
    mem_req.max_dec_height = chan->attr.max_dec_height;
    pdt_get_mem_req(&mem_req, mem_need);
    mem_need->extra_fs_num = mem_req.extra_fs_num;
    mem_need->dec_info.dec_num = (mem_need->dec_info.dec_num > FSP_FRAME_NUM) ?
                                FSP_FRAME_NUM : mem_need->dec_info.dec_num;
}

STATIC td_s32 fsp_if_need_report(fsp_chan *chan, fsp_report *report)
{
    td_u32 fs_num;
    td_s32 if_report;
    mem_need_info need = { 0 };

    vfmw_check_ret(chan != TD_NULL, FSP_ERR);

    fsp_get_mem_req(chan, TD_NULL, &need);

    if_report = fsp_if_need_report_dec_num(&need, chan, report);

    if (need.pmv_size != chan->fs_mem.need.pmv_size || need.pmv_num > chan->fs_mem.need.pmv_num) {
        fs_num = fsp_get_pmv_num(chan->id, need.pmv_size);
        chan->fs_mem.have_pmv_num = fs_num;
        if (need.pmv_size != chan->fs_mem.need.pmv_size) {
            chan->fs_mem.apply_pmv_num = fs_num;
        }
        if (chan->fs_mem.apply_pmv_num < need.pmv_num) {
            if_report = 1;
            report->pmv_num = chan->attr.alloc_type == VFMW_FRM_ALLOC_OUTER ? need.pmv_num : 1;
        }
    }

    if (need.disp_info.disp_size != chan->fs_mem.need.disp_info.disp_size ||
        need.disp_info.disp_width != chan->fs_mem.need.disp_info.disp_width ||
        need.disp_info.disp_num > chan->fs_mem.need.disp_info.disp_num) {
        fs_num = fsp_get_disp_num(chan->id, (td_s32)need.disp_info.disp_size);
        chan->fs_mem.have_disp_num = fs_num;
        if (need.disp_info.disp_size != chan->fs_mem.need.disp_info.disp_size) {
            chan->fs_mem.apply_disp_num = fs_num;
        }

        if (need.disp_info.disp_width != chan->fs_mem.need.disp_info.disp_width) {
            chan->fs_mem.apply_disp_num = 0;
            chan->fs_mem.have_disp_num = 0;
        }
        if (chan->fs_mem.apply_disp_num < need.disp_info.disp_num) {
            if_report = 1;
            report->disp_num = chan->attr.alloc_type == VFMW_FRM_ALLOC_OUTER ? need.disp_info.disp_num : 1;
        }
    }

    chan->fs_mem.have_nor_num = fsp_get_nor_num(chan->id, need.nor_size);
    if ((chan->fs_mem.need.nor_size != 0 && chan->fs_mem.need.nor_size != need.nor_size) ||
        (chan->fs_mem.need.sec_size != 0 && chan->fs_mem.need.sec_size != need.sec_size)) {
        dprint(PRN_ERROR, "%s attach info size not support dynamic change.\n", __func__);
    }

    vfmw_check_sec_func(memcpy_s(&chan->fs_mem.need, sizeof(chan->fs_mem.need), &need, sizeof(mem_need_info)));

    return if_report;
}

STATIC td_s32 fsp_req_new_fs(fsp_chan *chan)
{
    td_u32 flag;
    td_s32 if_report;
    fsp_report report = { 0 };

    vfmw_check_ret(chan != TD_NULL, FSP_ERR);
    if_report = fsp_if_need_report(chan, &report);

    chan->fs_mem.resize_succ = 1;
    if ((chan->attr.alloc_type != VFMW_FRM_ALLOC_OUTER) && if_report == 1 &&
        chan->fs_mem.have_dec_num < MAX_WAIT_FRM &&
        chan->fs_mem.total_dec_num >
        chan->fs_mem.need.dec_info.dec_num + MAX_WAIT_FRM) {
        chan->fs_mem.resize_succ = 0;
        dprint(PRN_ERROR, "%s have enough dec fs, no need requre new fs!\n", __func__);
        return FSP_ERR;
    }

    flag = ((chan->attr.alloc_type != VFMW_FRM_ALLOC_OUTER) &&
            (chan->fs_mem.total_dec_num + report.dec_num > FSP_FRAME_NUM ||
             chan->fs_mem.total_disp_num + report.disp_num > FSP_FRAME_NUM ||
             chan->fs_mem.total_pmv_num + report.pmv_num > FSP_PMV_NUM)) ||
           ((chan->attr.alloc_type == VFMW_FRM_ALLOC_OUTER) &&
            (report.dec_num > FSP_FRAME_NUM ||
             report.disp_num > FSP_FRAME_NUM ||
             report.pmv_num > FSP_PMV_NUM));
    if (flag != 0) {
        chan->fs_mem.resize_succ = 0;
        dprint(PRN_ERROR, "%s report too much fs, no need requre new fs!\n", __func__);
        return FSP_ERR;
    }

    if (if_report == 1) {
        if (fsp_report_req(chan, &report) != FSP_OK) {
            return FSP_ERR;
        }
    }

    return FSP_OK;
}

STATIC td_void fsp_req_part_fs_num(fsp_chan *chan, td_u32 *ref, td_u32 *disp, td_u32 *tmp)
{
    td_s32 cnt;
    td_u32 ref_num = 0;
    td_u32 disp_num = 0;
    td_u32 disp_tmp = 0;
    td_bool is_share = TD_FALSE;
    fsp_logic *logic_fs = TD_NULL;

    vfmw_check(chan != TD_NULL);
    vfmw_check(ref != TD_NULL);
    vfmw_check(disp != TD_NULL);
    vfmw_check(tmp != TD_NULL);

    for (cnt = 0; cnt < FSP_FRAME_NUM; cnt++) {
        logic_fs = chan->logic_fs + cnt;

        /* ref_num */
        if (logic_fs->is_use == 1 && (logic_fs->is_ref == 1 || logic_fs->is_dec_ref == 1)) {
            ref_num++;
        }

        /* not in voq */
        if (logic_fs->is_use == 1 && logic_fs->disp_state == FSP_DISP_STATE_ALLOC) {
            disp_num++;
        }

        /* not in voq and not ref */
        if (logic_fs->is_use == 1 && logic_fs->is_ref == 0 &&
            logic_fs->disp_state == FSP_DISP_STATE_ALLOC) {
            disp_tmp++;
        }

        if (logic_fs->is_use == 1 &&
            (logic_fs->logic_type == FSP_TYPE_SHARE || logic_fs->logic_type == FSP_TYPE_LINK)) {
            is_share = TD_TRUE;
        }
    }

    /* for share fs, disp_fs will not be released until the dec_fs is not referenced */
    if (is_share == TD_TRUE) {
        disp_num = vfmw_max(disp_num, (ref_num + disp_tmp));
    }

    *ref = ref_num;
    *disp = disp_num;
    *tmp = disp_tmp;
}

STATIC td_void fsp_req_part_fs_report_req(fsp_chan *chan, td_s32 if_report, const fsp_report *report)
{
    if (if_report == 1) {
        if (fsp_report_req(chan, report) != FSP_OK) {
            dprint(PRN_ERROR, "%s report req err\n", __func__);
        }
    }
}

STATIC td_void fsp_req_part_fs(fsp_chan *chan)
{
    td_u32 ref_num = 0;
    td_u32 disp_num = 0;
    td_u32 disp_tmp = 0;
    td_s32 if_report = 0;
    fsp_report report = { 0 };

    vfmw_check_no_print(chan != TD_NULL);
    vfmw_check_no_print(chan->cfg.dec_width > 0);
    vfmw_check_no_print(chan->cfg.dec_height > 0);
    vfmw_check_no_print(chan->cfg.bit_depth > 0);

    fsp_req_part_fs_num(chan, &ref_num, &disp_num, &disp_tmp);

    if (chan->fs_mem.apply_dec_num < chan->fs_mem.need.dec_info.dec_num &&
        chan->fs_mem.have_dec_num < chan->fs_mem.need.dec_info.dec_num) {
        if ((chan->fs_mem.need.disp_info.is_disp_need == 0 &&
             chan->fs_mem.apply_dec_num < ref_num + disp_tmp + chan->fs_mem.need.extra_fs_num + 1) ||
            (chan->fs_mem.need.disp_info.is_disp_need == 1 &&
             chan->fs_mem.apply_dec_num < ref_num + 1)) {
            report.dec_num = 1;
            if_report = 1;
        }
    }

    if (chan->fs_mem.apply_pmv_num < ref_num + chan->fs_mem.need.extra_pmv_num + 1 &&
        chan->fs_mem.apply_pmv_num < chan->fs_mem.need.pmv_num &&
        chan->fs_mem.have_pmv_num < chan->fs_mem.need.pmv_num) {
        report.pmv_num = 1;
        if_report = 1;
    }

    if (chan->fs_mem.apply_disp_num < disp_num + chan->fs_mem.need.extra_fs_num + 1 &&
        chan->fs_mem.apply_disp_num < chan->fs_mem.need.disp_info.disp_num &&
        chan->fs_mem.have_disp_num < chan->fs_mem.need.disp_info.disp_num) {
        report.disp_num = 1;
        if_report = 1;
    }

    fsp_req_part_fs_report_req(chan, if_report, &report);
}

STATIC td_void fsp_get_inuse_detail(fsp_rels_part_use_info *use_info, const mem_need_info *mem_need)
{
    td_s32 n;

    vfmw_check(use_info != TD_NULL);

    if (mem_need == TD_NULL) {
        dprint(PRN_ERROR, "mem_need is null.\n");
        return;
    }

    for (n = 0; n < FSP_DPS_NUM; n++) {
        if (use_info->dec_size[n] == 0 ||
            use_info->dec_size[n] == mem_need->dec_info.dec_size) {
            use_info->dec_num[n]++;
            use_info->dec_size[n] = mem_need->dec_info.dec_size;
            break;
        }
    }

    for (n = 0; n < FSP_DPS_NUM; n++) {
        if (use_info->pmv_size[n] == 0 ||
            use_info->pmv_size[n] == mem_need->pmv_size) {
            use_info->pmv_num[n]++;
            use_info->pmv_size[n] = mem_need->pmv_size;
            break;
        }
    }

    for (n = 0; n < FSP_DPS_NUM; n++) {
        if (use_info->disp_size[n] == 0 ||
            use_info->disp_size[n] == mem_need->disp_info.disp_size) {
            use_info->disp_num[n]++;
            use_info->disp_size[n] = mem_need->disp_info.disp_size;
            break;
        }
    }
}

STATIC td_void fsp_get_inuse_info(fsp_chan *chan, fsp_rels_part_use_info *use_info)
{
    td_s32 m;
    mem_need_info mem_need = { 0 };
    fsp_logic *logic_fs = TD_NULL;

    if (chan == TD_NULL) {
        dprint(PRN_ERROR, "chan is null.\n");
        return;
    }

    for (m = 0; m < FSP_FRAME_NUM; m++) {
        logic_fs = chan->logic_fs + m;
        if (logic_fs->is_use == 1 &&
            logic_fs->dec_fs == TD_NULL &&
            (logic_fs->dec_state.bits.top_state == FSP_DEC_STATE_WAIT ||
             logic_fs->dec_state.bits.bot_state == FSP_DEC_STATE_WAIT)) {
            fsp_get_mem_req(chan, logic_fs, &mem_need);
            fsp_get_inuse_detail(use_info, &mem_need);
        }
    }
}

STATIC td_void fsp_rel_unused_dec(fsp_chan *chan, const td_u32 *dec_size, td_u32 dec_size_array_size,
    const td_u32 *dec_num, td_u32 dec_num_array_size)
{
    td_u32 m, n;
    const fsp_phy *dec_fs = TD_NULL;
    td_u32 tmp_num[FSP_DPS_NUM] = { 0 };

    vfmw_check(chan != TD_NULL);
    vfmw_check(dec_num != TD_NULL);
    vfmw_check_print(dec_size != TD_NULL, "dec_size is null.\n");

    vfmw_check_no_print(dec_num[0] != 0);

    for (m = 0; m < FSP_FRAME_NUM; m++) {
        dec_fs = chan->dec_fs + m;
        if (dec_fs->is_valid == 0 && chan->attr.alloc_type == VFMW_FRM_ALLOC_INNER &&
            chan->fs_mem.need.disp_info.is_disp_need == TD_FALSE) {
            continue;
        }

        if (dec_fs->is_valid == 0 || dec_fs->is_use == 1) {
            continue;
        }

        for (n = 0; n < dec_size_array_size; n++) {
            if (dec_fs->buf_size == dec_size[n]) {
                tmp_num[n]++;
                break;
            }
        }

        if ((n >= dec_num_array_size || tmp_num[n] > dec_num[n]) &&
            (chan->attr.alloc_type == VFMW_FRM_ALLOC_INNER || chan->fs_mem.need.disp_info.is_disp_need == TD_TRUE)) {
            fsp_release_fs(chan, FSP_MEM_TYPE_DEC, (td_void *)dec_fs);
            chan->fs_mem.total_dec_num--;
            if (n == 0 && chan->fs_mem.apply_dec_num > 0) {
                chan->fs_mem.apply_dec_num--;
            }
        }
    }
}

STATIC td_void fsp_rel_unused_disp(fsp_chan *chan, const td_u32 *disp_size, td_u32 disp_size_arr_size,
    const td_u32 *disp_num, td_u32 disp_num_arr_size)
{
    td_u32 m, n;
    fsp_phy *disp_fs = TD_NULL;
    td_u32 tmp_num[FSP_DPS_NUM] = { 0 };

    vfmw_check(chan != TD_NULL);
    vfmw_check(disp_size != TD_NULL);
    vfmw_check(disp_num != TD_NULL);
    vfmw_check_no_print(disp_num[0] != 0);

    for (m = 0; m < FSP_FRAME_NUM; m++) {
        disp_fs = chan->disp_fs + m;
        if (disp_fs->is_valid == 0 && chan->attr.alloc_type == VFMW_FRM_ALLOC_INNER) {
            continue;
        }

        if (disp_fs->is_valid == 0 || disp_fs->is_use == 1) {
            continue;
        }

        for (n = 0; n < disp_size_arr_size; n++) {
            if (disp_fs->buf_size == disp_size[n]) {
                tmp_num[n]++;
                break;
            }
        }

        if ((n >= disp_num_arr_size || tmp_num[n] > disp_num[n]) &&
            chan->attr.alloc_type == VFMW_FRM_ALLOC_INNER) {
            fsp_release_fs(chan, FSP_MEM_TYPE_DISP, (td_void *)disp_fs);
            chan->fs_mem.total_disp_num--;
            if (n == 0 && chan->fs_mem.apply_disp_num > 0) {
                chan->fs_mem.apply_disp_num--;
            }
        }
    }
}

STATIC td_void fsp_rel_unused_pmv(fsp_chan *chan, const td_u32 *pmv_size, td_u32 pmv_size_arr_size,
    const td_u32 *pmv_num, td_u32 pmv_num_arr_size)
{
    td_u32 m, n;
    fsp_pmv *pmv_fs = TD_NULL;
    td_u32 tmp_num[FSP_DPS_NUM] = { 0 };

    vfmw_check(chan != TD_NULL);
    vfmw_check(pmv_num != TD_NULL);

    if (pmv_size == TD_NULL) {
        dprint(PRN_ERROR, "pmv_size is null.\n");
        return;
    }

    if (pmv_num[0] == 0) {
        return;
    }

    for (m = 0; m < FSP_PMV_NUM; m++) {
        pmv_fs = chan->pmv_fs + m;
        if (pmv_fs->is_valid == 0) {
            continue;
        }

        if (pmv_fs->is_use == 1) {
            continue;
        }

        for (n = 0; n < pmv_size_arr_size; n++) {
            if (pmv_fs->buf_size == pmv_size[n]) {
                tmp_num[n]++;
                break;
            }
        }

        if (n >= pmv_num_arr_size || tmp_num[n] > pmv_num[n]) {
            fsp_release_fs(chan, FSP_MEM_TYPE_PMV, (td_void *)pmv_fs);
            chan->fs_mem.total_pmv_num--;
            if (n == 0 && chan->fs_mem.apply_pmv_num > 0) {
                chan->fs_mem.apply_pmv_num--;
            }
        }
    }
}

STATIC td_void fsp_rel_part_fs(fsp_chan *chan)
{
    fsp_rels_part_use_info rls_use_info = {{0}, {0}, {0}, {0}, {0}, {0}};

    vfmw_check(chan != TD_NULL);

    /* record current need size */
    if (chan->fs_mem.need.dec_info.dec_size != 0) {
        rls_use_info.dec_size[0] = chan->fs_mem.need.dec_info.dec_size;
        rls_use_info.dec_num[0] = chan->fs_mem.need.dec_info.dec_num;
    }

    /* record current need pmv */
    if (chan->fs_mem.need.pmv_size != 0) {
        rls_use_info.pmv_size[0] = chan->fs_mem.need.pmv_size;
        rls_use_info.pmv_num[0] = chan->fs_mem.need.pmv_num;
    }

    if (chan->fs_mem.need.disp_info.disp_size != 0) {
        rls_use_info.disp_size[0] = chan->fs_mem.need.disp_info.disp_size;
        rls_use_info.disp_num[0] = chan->fs_mem.need.disp_info.disp_num;
    }

    /* search other size & num */
    fsp_get_inuse_info(chan, &rls_use_info);

    fsp_rel_unused_dec(chan, rls_use_info.dec_size, FSP_DPS_NUM, rls_use_info.dec_num, FSP_DPS_NUM);

    fsp_rel_unused_pmv(chan, rls_use_info.pmv_size, FSP_DPS_NUM, rls_use_info.pmv_num, FSP_DPS_NUM);

    fsp_rel_unused_disp(chan, rls_use_info.disp_size, FSP_DPS_NUM, rls_use_info.disp_num, FSP_DPS_NUM);

    return;
}

td_s32 fsp_refresh(td_s32 chan_id)
{
    td_s32 ret = FSP_ERR;
    td_ulong lock_flag;
    fsp_chan *chan = TD_NULL;

    fsp_lock(chan_id, &lock_flag);

    fsp_assert_mod_goto(out);
    chan = fsp_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    if (chan->attr.alloc_type == VFMW_FRM_ALLOC_OUTER) {
        ret = FSP_OK;
        goto out;
    }

    fsp_req_part_fs(chan);

    fsp_rel_part_fs(chan);

    ret = FSP_OK;

out:

    fsp_unlock(chan_id, &lock_flag);

    return ret;
}

td_void fsp_rels_all_dec(fsp_chan *chan)
{
    td_s32 i;
    fsp_phy *phy_fs = TD_NULL;

    vfmw_check(chan != TD_NULL);

    if (chan->fs_mem.total_dec_num == 0) {
        return;
    }

    for (i = 0; i < FSP_FRAME_NUM; i++) {
        phy_fs = &chan->dec_fs[i];
        if (phy_fs->is_valid != 1) {
            continue;
        }
        fsp_release_fs(chan, FSP_MEM_TYPE_DEC, (td_void *)phy_fs);
    }

    chan->fs_mem.have_dec_num = 0;
    chan->fs_mem.total_dec_num = 0;
}

/* find inused old resulotion pmv and set no_using */
td_void fsp_clear_unused_pmv(fsp_chan *chan)
{
    td_s32 m;
    fsp_pmv *pmv_fs = TD_NULL;

    if (chan == TD_NULL) {
        dprint(PRN_ERROR, "chan is null.\n");
        return;
    }

    for (m = 0; m < FSP_PMV_NUM; m++) {
        pmv_fs = chan->pmv_fs + m;
        if (pmv_fs->is_valid == 0) {
            continue;
        }

        if (pmv_fs->buf_size != 0 &&
            chan->fs_mem.need.pmv_size != 0 &&
            pmv_fs->buf_size != chan->fs_mem.need.pmv_size) {
            fsp_release_fs(chan, FSP_MEM_TYPE_PMV, (td_void *)pmv_fs);
            chan->fs_mem.total_pmv_num--;
        }
    }
}

td_void fsp_clear_all_attach(fsp_chan *chan)
{
    td_s32 m;
    fsp_attach *attach_fs = TD_NULL;

    vfmw_check(chan != TD_NULL);

    for (m = 0; m < FSP_FRAME_NUM; m++) {
        attach_fs = chan->nor_fs + m;
        if (attach_fs->is_valid == 0) {
            continue;
        }

        attach_fs->is_use = 0;
    }

    for (m = 0; m < FSP_FRAME_NUM; m++) {
        attach_fs = chan->sec_fs + m;
        if (attach_fs->is_valid == 0) {
            continue;
        }

        attach_fs->is_use = 0;
    }
}

td_s32 fsp_clear_unused_logic(td_s32 chan_id)
{
    td_u32 i;
    td_s32 ret;
    fsp_chan *chan = TD_NULL;

    ret = FSP_ERR;

    chan = fsp_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    for (i = 0; i < FSP_FRAME_NUM; i++) {
        /* if is_ref==true means this logic fs cannot be clean,is_dec_ref is not strict refence relation for logic_fs */
        if ((chan->logic_fs[i].is_in_voq == 1 &&
             chan->logic_fs[i].disp_state == FSP_DISP_STATE_WAIT &&
             chan->logic_fs[i].dec_state.bits.top_state == FSP_DEC_STATE_OVER &&
             chan->logic_fs[i].dec_state.bits.bot_state == FSP_DEC_STATE_OVER) ||
            (chan->logic_fs[i].is_in_voq == 0 &&
             chan->logic_fs[i].disp_state == FSP_DISP_STATE_IDLE &&
             chan->logic_fs[i].dec_state.bits.top_state == FSP_DEC_STATE_IDLE &&
             chan->logic_fs[i].dec_state.bits.bot_state == FSP_DEC_STATE_IDLE)) {
            chan->logic_fs[i].is_in_voq = 0;
            chan->logic_fs[i].disp_state = 0;
            chan->logic_fs[i].dec_state.data = 0;
            chan->logic_fs[i].is_dec_ref = 0;
            if (chan->logic_fs[i].dec_fs != TD_NULL) {
                chan->logic_fs[i].dec_fs->is_use = 0;
            }
            if (chan->logic_fs[i].disp_fs != TD_NULL) {
                chan->logic_fs[i].disp_fs->is_use = 0;
            }
            if (chan->logic_fs[i].pmv_fs != TD_NULL) {
                chan->logic_fs[i].pmv_fs->is_use = 0;
            }
            chan->logic_fs[i].dec_fs = TD_NULL;
            chan->logic_fs[i].disp_fs = TD_NULL;
            chan->logic_fs[i].pmv_fs = TD_NULL;
        }
    }
    ret = FSP_OK;
out:

    return ret;
}

td_s32 fsp_set_wait_flush(td_s32 chan_id, td_u32 wait_flush_flag)
{
    td_s32 ret;
    fsp_chan *chan = TD_NULL;

    ret = FSP_ERR;

    vfmw_check_ret(chan_id >= 0 && chan_id < FSP_CHAN_NUM, FSP_ERR);

    fsp_assert_mod_goto(out);
    chan = fsp_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    chan->wait_flush = wait_flush_flag;

    ret = FSP_OK;

out:
    return ret;
}

STATIC td_void fsp_rels_all_disp(fsp_chan *chan)
{
    td_s32 i;
    fsp_phy *phy_fs = TD_NULL;

    if (chan == TD_NULL) {
        return;
    }

    if (chan->fs_mem.total_disp_num == 0) {
        return;
    }

    for (i = 0; i < FSP_FRAME_NUM; i++) {
        phy_fs = &chan->disp_fs[i];
        if (phy_fs->is_valid != 1) {
            continue;
        }

        fsp_release_fs(chan, FSP_MEM_TYPE_DISP, (td_void *)phy_fs);
    }

    chan->fs_mem.have_disp_num = 0;
    chan->fs_mem.total_disp_num = 0;
}

STATIC td_void fsp_rels_all_pmv(fsp_chan *chan)
{
    td_s32 i;
    fsp_pmv *pmv_fs = TD_NULL;

    if (chan == TD_NULL) {
        dprint(PRN_ERROR, "chan is null.\n");
        return;
    }

    if (chan->fs_mem.total_pmv_num == 0) {
        return;
    }

    for (i = 0; i < FSP_PMV_NUM; i++) {
        pmv_fs = &chan->pmv_fs[i];
        if (pmv_fs->is_valid != 1) {
            continue;
        }
        fsp_free_mem(pmv_fs->phy_addr);
    }

    chan->fs_mem.have_pmv_num = 0;
    chan->fs_mem.total_pmv_num = 0;
}

#ifdef VFMW_VMM_SUPPORT
STATIC td_void fsp_rels_all_sec(fsp_chan *chan)
{
    td_s32 i;
    fsp_attach *attach_fs = TD_NULL;

    if (chan == TD_NULL) {
        dprint(PRN_ERROR, "chan is null.\n");
        return;
    }

    for (i = 0; i < FSP_FRAME_NUM; i++) {
        attach_fs = &chan->sec_fs[i];
        if (attach_fs->is_valid != 1) {
            continue;
        }

        fsp_release_fs(chan, FSP_MEM_TYPE_SEC, (td_void *)attach_fs);
    }
}

STATIC td_void fsp_rels_all_fs(fsp_chan *chan)
{
    fsp_rels_all_dec(chan);
    fsp_rels_all_disp(chan);
    fsp_rels_all_pmv(chan);
    fsp_rels_all_sec(chan);
}
#endif

vfmw_frm_alloc_type fsp_get_frame_buf_mode(td_s32 chan_id)
{
    vfmw_frm_alloc_type ret;
    td_ulong lock_flag;
    fsp_chan *chan = TD_NULL;

    fsp_lock(chan_id, &lock_flag);

    ret = VFMW_FRM_ALLOC_MAX;
    fsp_assert_mod_goto(out);
    chan = fsp_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    ret = chan->attr.alloc_type;

out:
    fsp_unlock(chan_id, &lock_flag);

    return ret;
}

td_s32 fsp_add_dec_fs(fsp_chan *chan, const vfmw_mem_report *dec_mem, const vfmw_mem_report *info_mem,
                      const vfmw_mem_report *hfbc_mem)
{
    td_s32 cnt;
    td_s32 add_pos = -1;
    fsp_phy *phy_fs = TD_NULL;

    vfmw_unused(hfbc_mem);
    vfmw_check_ret(chan != TD_NULL, FSP_ERR);
    vfmw_check_ret(dec_mem != TD_NULL, FSP_ERR);

    for (cnt = 0; cnt < FSP_FRAME_NUM; cnt++) {
        if (chan->dec_fs[cnt].is_valid == 0) {
            add_pos = cnt;
            break;
        }
    }

    if (add_pos == -1) {
        dprint(PRN_ERROR, "chan %d no position to store dec fs\n", chan->id);
        return FSP_ERR;
    }

    phy_fs = &(chan->dec_fs[add_pos]);

    vfmw_check_sec_func(memset_s(phy_fs, sizeof(fsp_phy), 0, sizeof(fsp_phy)));

    phy_fs->is_valid = 1;
    phy_fs->fs_id = add_pos;
    phy_fs->is_map = dec_mem->is_map;
    phy_fs->phy_addr = dec_mem->phy_addr;
    phy_fs->vir_addr = dec_mem->vir_addr;
    phy_fs->buf_size = dec_mem->size;
    phy_fs->need_attach = (chan->fs_mem.need.attach == ATTACH_DEC) ? TD_TRUE : TD_FALSE;
    phy_fs->sec_info = dec_mem->sec_info;

    chan->fs_mem.total_dec_num++;
    if (dec_mem->size == chan->fs_mem.need.dec_info.dec_size) {
        chan->fs_mem.have_dec_num++;
    }

    if (info_mem != TD_NULL) {
        phy_fs->info_phy_addr = info_mem->phy_addr;
        phy_fs->info_vir_addr = info_mem->vir_addr;
        phy_fs->info_buf_size = info_mem->size;
        phy_fs->info_offset = info_mem->offset;
    }

    return FSP_OK;
}

STATIC td_void fsp_unbind_dec_fs(fsp_chan *chan, const vfmw_ext_buf *ext_buf)
{
    td_u32 index;

    for (index = 0; index < FSP_FRAME_NUM; index++) {
        if (chan->dec_fs[index].phy_addr == ext_buf->ext_frm_buf.phy_addr) {
            chan->dec_fs[index].is_valid = 0;
            chan->dec_fs[index].is_use = 0;
            chan->fs_mem.apply_dec_num = (chan->fs_mem.apply_dec_num > 0) ? chan->fs_mem.apply_dec_num - 1 : 0;
            chan->fs_mem.have_dec_num = (chan->fs_mem.have_dec_num > 0) ? chan->fs_mem.have_dec_num - 1 : 0;
            chan->fs_mem.total_dec_num = (chan->fs_mem.total_dec_num > 0) ? chan->fs_mem.total_dec_num - 1 : 0;
            break;
        }
    }
}

STATIC td_void fsp_unbind_disp_fs(fsp_chan *chan, const vfmw_ext_buf *ext_buf)
{
    td_u32 index;

    for (index = 0; index < FSP_FRAME_NUM; index++) {
        if (chan->disp_fs[index].phy_addr == ext_buf->ext_frm_buf.phy_addr) {
            chan->disp_fs[index].is_valid = 0;
            chan->disp_fs[index].is_use = 0;
            chan->fs_mem.apply_disp_num = (chan->fs_mem.apply_disp_num > 0) ? chan->fs_mem.apply_disp_num - 1 : 0;
            chan->fs_mem.have_disp_num = (chan->fs_mem.have_disp_num > 0) ? chan->fs_mem.have_disp_num - 1 : 0;
            chan->fs_mem.total_disp_num = (chan->fs_mem.total_disp_num > 0) ? chan->fs_mem.total_disp_num - 1 : 0;
            break;
        }
    }
}

td_s32 fsp_unbind_extern_fs(td_s32 chan_id, const vfmw_ext_buf *ext_buf)
{
    td_s32 ret = FSP_ERR;
    td_ulong lock_flag;
    fsp_chan *chan = TD_NULL;

    vfmw_check_ret(ext_buf != TD_NULL, FSP_ERR);

    fsp_lock(chan_id, &lock_flag);
    chan = fsp_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    if (chan->attr.alloc_type != VFMW_FRM_ALLOC_OUTER) {
        goto out;
    }

    if (chan->fs_mem.need.disp_info.is_disp_need == TD_TRUE) {
        fsp_unbind_disp_fs(chan, ext_buf);
    } else {
        fsp_unbind_dec_fs(chan, ext_buf);
    }

    ret = FSP_OK;

out:
    fsp_unlock(chan_id, &lock_flag);

    return ret;
}

td_s32 fsp_bind_extern_fs(td_s32 chan_id, const vfmw_ext_buf *ext_buf)
{
    td_s32 ret = FSP_ERR;
    td_ulong lock_flag;
    fsp_chan *chan = TD_NULL;
    const vfmw_mem_report *tmp_frm_buf = TD_NULL;
    const vfmw_mem_report *tmp_info_buf = TD_NULL;
    const vfmw_mem_report *tmp_hfbc_buf = TD_NULL;
    td_u32 dest_mem_size, have_mem_num, need_mem_num;

    vfmw_check_ret(ext_buf != TD_NULL, FSP_ERR);

    tmp_frm_buf = &(ext_buf->ext_frm_buf);
    tmp_info_buf = &(ext_buf->ext_info_buf);
    tmp_hfbc_buf = &(ext_buf->ext_hfbc_buf);

    vfmw_check_ret(((tmp_frm_buf->phy_addr & 0xf) == 0), FSP_ERR);

    fsp_lock(chan_id, &lock_flag);

    fsp_assert_mod_goto(EXITE_ERR);

    chan = fsp_get_chan(chan_id);

    vfmw_check_goto(chan != TD_NULL, EXITE_ERR);

    dest_mem_size = chan->fs_mem.need.dec_info.dec_size;
    have_mem_num = chan->fs_mem.have_dec_num;
    need_mem_num = chan->fs_mem.need.dec_info.dec_num;

    if (chan->fs_mem.need.disp_info.is_disp_need == TD_TRUE) {
        dest_mem_size = chan->fs_mem.need.disp_info.disp_size;
        have_mem_num = chan->fs_mem.have_disp_num;
        need_mem_num = chan->fs_mem.need.disp_info.disp_num;
    }

    if (tmp_frm_buf->size < dest_mem_size) {
        dprint(PRN_ERROR, "%s mem %s size %d < need %d, ignor it.\n", __func__,
               tmp_frm_buf->buf_name, tmp_frm_buf->size, dest_mem_size);
        goto EXITE_ERR;
    }

    if (have_mem_num >= need_mem_num || need_mem_num >= FSP_FRAME_NUM) {
        dprint(PRN_ERROR, "%s cfg_num %d >= require_num %d/MAX_FRAME_NUM %d, ignor it.\n", __func__,
               have_mem_num, need_mem_num, FSP_FRAME_NUM);
        goto EXITE_ERR;
    }

    if (chan->fs_mem.need.disp_info.is_disp_need == TD_TRUE) {
        ret = fsp_add_disp_fs(chan, tmp_frm_buf, tmp_info_buf, tmp_hfbc_buf);
    } else {
        ret = fsp_add_dec_fs(chan, tmp_frm_buf, tmp_info_buf, tmp_hfbc_buf);
    }

EXITE_ERR:
    fsp_unlock(chan_id, &lock_flag);

    return ret;
}

td_s32 fsp_set_out_frm_state(td_s32 chan_id, const fsp_out_frm_state state)
{
    td_s32 ret = FSP_ERR;
    td_ulong lock_flag;
    fsp_chan *chan = TD_NULL;

    vfmw_check_ret(state < FS_OUT_FRM_MAX, FSP_ERR);

    fsp_lock(chan_id, &lock_flag);

    fsp_assert_mod_goto(EXITE_ERR);

    chan = fsp_get_chan(chan_id);

    vfmw_check_goto(chan != TD_NULL, EXITE_ERR);

    vfmw_check_goto(chan->attr.outer_frm_mode == VFMW_OUTER_FRM_WAIT, EXITE_ERR);

    chan->outer_frm_status = state;
    if (state == FS_OUT_FRM_UNBIND_READY) {
        chan->wait_unbind = 0;
    }
    ret = FSP_OK;

EXITE_ERR:
    fsp_unlock(chan_id, &lock_flag);

    return ret;
}

/* extern func */
td_s32 fsp_open(td_void)
{
    td_s32 id;
    fsp_entry *entry = TD_NULL;

    entry = fsp_get_entry();
    if (entry->state != FSP_STATE_NULL) {
        dprint(PRN_FATAL, "%s state Error \n", __func__);
        return FSP_ERR;
    }

    os_sema_init(&entry->sema);
    for (id = 0; id < FSP_CHAN_NUM; id++) {
        os_spin_lock_init(&entry->lock[id]);
    }
    os_spin_lock_init(&entry->img_format_lock);

    entry->state = FSP_STATE_OPEN;
    vfmw_check_sec_func(memset_s(entry->chan, sizeof(entry->chan), 0, sizeof(entry->chan)));

    return FSP_OK;
}

td_s32 fsp_close(td_void)
{
    td_s32 id;
    fsp_entry *entry = TD_NULL;

    entry = fsp_get_entry();
    if (entry->state == FSP_STATE_NULL ||
        entry->state == FSP_STATE_INIT) {
        dprint(PRN_FATAL, "%s state Error \n", __func__);
        return FSP_ERR;
    }

    entry->state = FSP_STATE_NULL;
    vfmw_check_sec_func(memset_s(entry->chan, sizeof(entry->chan), 0, sizeof(entry->chan)));

    os_sema_exit(entry->sema);

    for (id = 0; id < FSP_CHAN_NUM; id++) {
        os_spin_lock_exit(entry->lock[id]);
    }

    os_spin_lock_exit(entry->img_format_lock);

    return FSP_OK;
}

td_s32 fsp_initial(td_void)
{
    td_s32 ret = FSP_ERR;
    fsp_entry *entry = TD_NULL;

    fsp_sema_down();

    entry = fsp_get_entry();
    if (entry->state == FSP_STATE_NULL ||
        entry->state == FSP_STATE_INIT) {
        dprint(PRN_FATAL, "%s state Error \n", __func__);
        goto out;
    }

    entry->ref_cnt++;
    entry->state = FSP_STATE_INIT;
    vfmw_check_sec_func(memset_s(entry->chan, sizeof(entry->chan), 0, sizeof(entry->chan)));
    ret = FSP_OK;

out:

    fsp_sema_up();

    return ret;
}

td_s32 fsp_exit(td_void)
{
    td_s32 ret = FSP_ERR;
    fsp_entry *entry = TD_NULL;

    fsp_sema_down();

    entry = fsp_get_entry();
    if (entry->state != FSP_STATE_INIT) {
        dprint(PRN_FATAL, "%s state Error \n", __func__);
        goto out;
    }

    entry->ref_cnt--;
    entry->state = FSP_STATE_EXIT;
    vfmw_check_sec_func(memset_s(entry->chan, sizeof(entry->chan), 0, sizeof(entry->chan)));
    ret = FSP_OK;

out:

    fsp_sema_up();

    return ret;
}

td_s32 fsp_create(td_s32 chan_id)
{
    td_s32 ret = FSP_ERR;
    td_u32 size;
    td_u8 *vir = TD_NULL;
    fsp_chan *chan = TD_NULL;
    td_ulong lock_flag;
    mem_record mem = { 0 };

    size = (td_u32)sizeof(fsp_chan);
    mem.mem_name = "fsp_chan";
    mem.length = vfmw_align_up(size, VFMW_CTX_ALIGN);
    mem.is_cached = 0;
    mem.type = CACHE_MEM_TYPE;
    mem.unid = (td_u32)chan_id;
    vfmw_check_ret(os_malloc != TD_NULL, FSP_ERR);

    os_malloc(&mem); /* "FSP_Inst" */
    vir = mem.vir_addr;
    if (vir == TD_NULL) {
        dprint(PRN_FATAL, "FSP AllocMem(%d) Error\n", size);
        return FSP_ERR;
    }

    vfmw_check_sec_func(memset_s(vir, size, 0, size));
    vfmw_check_ret(os_get_unid != TD_NULL, FSP_ERR);

    fsp_sema_down();
    fsp_lock(chan_id, &lock_flag);

    fsp_assert_mod_goto(out);

    chan = fsp_get_chan(chan_id);
    if (chan == TD_NULL) {
        chan = (fsp_chan *)vir;
        chan->handle = chan;
        chan->id = chan_id;
        chan->unid = (td_u32)os_get_unid();
        chan->state = 1;
        chan->wait_unbind = 0;
        chan->outer_frm_status = FS_OUT_FRM_UNBIND_READY;

        fsp_get_entry()->chan[chan_id] = chan;
        fsp_get_entry()->ctx_size += size;
        ret = FSP_OK;
    }

out:

    fsp_unlock(chan_id, &lock_flag);
    fsp_sema_up();

    if (ret != FSP_OK) {
        mem.vir_addr = vir;
        mem.is_cached = 0;
        os_free(&mem);
    }
    vdec_record_chr_event(chan_id, MT_VDEC_SET_ATTR, MT_VDEC_SET_HW_ATTR, MT_VFMW_CREATE_FSP_OK);

    return ret;
}

td_s32 fsp_get_attr(td_s32 chan_id, fsp_attr *attr)
{
    td_s32 ret;
    fsp_chan *chan = TD_NULL;
    td_ulong lock_flag;

    ret = FSP_ERR;
    vfmw_check_ret(attr != TD_NULL, FSP_ERR);

    fsp_lock(chan_id, &lock_flag);

    fsp_assert_mod_goto(out);

    chan = fsp_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    vfmw_check_goto(chan->handle != TD_NULL, out);

    vfmw_check_sec_func(memcpy_s(attr, sizeof(fsp_attr), &chan->attr, sizeof(chan->attr)));

    ret = FSP_OK;

out:
    fsp_unlock(chan_id, &lock_flag);

    return ret;
}

td_s32 fsp_set_attr(td_s32 chan_id, const fsp_attr *attr)
{
    td_s32 ret;
    fsp_chan *chan = TD_NULL;
    td_ulong lock_flag;
    mem_need_info need = {0};

    ret = FSP_ERR;
    vfmw_check_ret(attr != TD_NULL, FSP_ERR);

    fsp_lock(chan_id, &lock_flag);

    fsp_assert_mod_goto(out);

    chan = fsp_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    vfmw_check_goto(chan->handle != TD_NULL, out);

    vfmw_check_sec_func(memcpy_s(&chan->attr, sizeof(chan->attr), attr, sizeof(fsp_attr)));

    fsp_get_mem_req(chan, TD_NULL, &need);
    vfmw_check_sec_func(memcpy_s(&chan->fs_mem.need, sizeof(chan->fs_mem.need), &need, sizeof(mem_need_info)));

    ret = FSP_OK;

out:
    fsp_unlock(chan_id, &lock_flag);

    return ret;
}

STATIC td_void fsp_reset_logic_fs(fsp_chan *chan)
{
    td_s8 cnt;
    fsp_logic *logic_fs = TD_NULL;

    for (cnt = 0; cnt < FSP_FRAME_NUM; cnt++) {
        logic_fs = &chan->logic_fs[cnt];
        if (logic_fs->disp_state != FSP_DISP_STATE_WAIT ||
            ((logic_fs->dec_fs == TD_NULL) && (logic_fs->disp_fs == TD_NULL))) {
            if (logic_fs->dec_fs != TD_NULL) {
                logic_fs->dec_fs->is_use = 0;
            }
            if (logic_fs->disp_fs != TD_NULL) {
                logic_fs->disp_fs->is_use = 0;
            }
            if (logic_fs->nor_fs != TD_NULL) {
                logic_fs->nor_fs->is_use = 0;
            }
            if (logic_fs->sec_fs != TD_NULL) {
                logic_fs->sec_fs->is_use = 0;
            }

            vfmw_check_sec_func(memset_s(logic_fs, sizeof(fsp_logic), 0, sizeof(fsp_logic)));
            logic_fs->logic_id = cnt;
        } else {
            logic_fs->is_ref = 0;
            logic_fs->is_dec_ref = 0;

            if (logic_fs->dec_fs != TD_NULL) {
                /* no post if dec proc task but not start */
                logic_fs->dec_fs->ref_cnt = 0;
            }

            if (logic_fs->dec_fs == logic_fs->disp_fs ||
                fsp_is_dec_over(logic_fs) != TD_TRUE) {
                continue;
            }

            if (logic_fs->dec_fs != TD_NULL) {
                logic_fs->dec_fs->is_use = 0;
                logic_fs->dec_fs = TD_NULL;
            }

            if (logic_fs->pmv_fs != TD_NULL) {
                logic_fs->pmv_fs->is_use = 0;
                logic_fs->pmv_fs = TD_NULL;
            }
        }
    }
}

td_s32 fsp_reset(td_s32 chan_id)
{
    td_s32 ret, cnt;
    fsp_chan *chan = TD_NULL;
    td_ulong lock_flag;

    ret = FSP_ERR;
    fsp_lock(chan_id, &lock_flag);

    fsp_assert_mod_goto(out);

    chan = fsp_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);
    vfmw_check_goto(chan->handle != TD_NULL, out);

    fsp_reset_logic_fs(chan);

    for (cnt = 0; cnt < FSP_PMV_NUM; cnt++) {
        chan->pmv_fs[cnt].is_use = 0;
    }

    ret = FSP_OK;

out:

    fsp_unlock(chan_id, &lock_flag);
    vdec_record_chr_event(chan_id, MT_VDEC_RESET, MT_VDEC_VFMW_RESET, MT_VFMW_FSP_RESET_OK);

    return ret;
}

STATIC td_void fsp_adjust_disp_wh(fsp_chan *chan)
{
    mem_need_info need = { 0 };

    fsp_get_mem_req(chan, TD_NULL, &need);
    if (chan->attr.air_version == AIR_DEC_UNSUPPORT) {
        chan->cfg.disp_width -= chan->cfg.disp_wofs;
        chan->cfg.disp_height -= chan->cfg.disp_hofs;
    }
}

td_s32 fsp_config(td_s32 chan_id, const td_void *attr)
{
    td_s32 ret;
    fsp_chan *chan = TD_NULL;
    td_ulong lock_flag;
    td_u16 dps_num;

    ret = FSP_ERR;
    fsp_lock(chan_id, &lock_flag);
    vfmw_check_goto(attr != TD_NULL, out);

    fsp_assert_mod_goto(out);

    chan = fsp_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    if (chan->attr.alloc_type == VFMW_FRM_ALLOC_OUTER) {
        dps_num = (td_u16)dps_get_unfinish_dps_num(chan_id);
        if (dps_num != 0) {
            goto out;
        }
    }

    if (chan->handle != TD_NULL) {
        vfmw_check_sec_func(memcpy_s(&chan->cfg, sizeof(chan->cfg), attr, sizeof(fsp_cfg)));
        fsp_adjust_disp_wh(chan);

        ret = fsp_req_new_fs(chan);
        if (ret != FSP_OK) {
            goto out;
        }

        ret = FSP_OK;
    }

out:

    fsp_unlock(chan_id, &lock_flag);

    return ret;
}

td_s32 fsp_get_pic_wh(td_s32 chan_id, td_s32 *height, td_s32 *width, td_u32 *tick)
{
    fsp_chan *chan = TD_NULL;

    if (height == TD_NULL || width == TD_NULL || tick == TD_NULL) {
        return FSP_ERR;
    }

    chan = fsp_get_chan(chan_id);
    if (chan == TD_NULL) {
        return FSP_ERR;
    }

    *height = (td_s32)chan->cfg.dec_height;
    *width = (td_s32)chan->cfg.dec_width;
    *tick = chan->tick;

    return FSP_OK;
}

td_s32 fsp_empty(td_s32 chan_id)
{
    vfmw_unused(chan_id);

    return FSP_OK;
}

td_s32 fsp_destroy(td_s32 chan_id)
{
    td_s32 ret;
    fsp_chan *chan = TD_NULL;
    td_ulong lock_flag = 0;
    mem_record mem = { 0 };

    ret = FSP_ERR;
    fsp_sema_down();
    fsp_lock(chan_id, &lock_flag);

    fsp_assert_mod_goto(out);

    chan = fsp_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

#ifdef VFMW_VMM_SUPPORT
    if (chan->attr.alloc_type != VFMW_FRM_ALLOC_INNER) {
        fsp_rels_all_fs(chan);
    }
#else
    if (chan->attr.alloc_type == VFMW_FRM_ALLOC_INNER) {
        fsp_rls_dec_pmv_fs(chan);
    } else {
        fsp_rels_all_pmv(chan);
    }
#endif

    fsp_get_entry()->chan[chan_id] = TD_NULL;
    fsp_get_entry()->ctx_size -= (td_u32)sizeof(fsp_chan);
    ret = FSP_OK;

out:

    fsp_unlock(chan_id, &lock_flag);
    fsp_sema_up();

    if (chan != TD_NULL) {
        mem.vir_addr = (td_u8 *)(chan);
        mem.is_cached = 0;
        os_free(&mem);
    }

    return ret;
}

td_s32 fsp_get_total_num(td_s32 chan_id)
{
    td_s32 frm_num = 0;
    fsp_chan *chan = TD_NULL;

    chan = fsp_get_chan(chan_id);
    if (chan != TD_NULL) {
        frm_num = (td_s32)chan->fs_mem.need.dec_info.dec_num;
    }

    return frm_num;
}

td_s32 fsp_check_fs_ok(td_s32 chan_id)
{
    td_s32 ret;
#ifdef VFMW_VMM_SUPPORT
    td_u32 total_disp_num;
#endif
    fsp_chan *chan = TD_NULL;
    td_ulong lock_flag;

    ret = FSP_OK;
    fsp_lock(chan_id, &lock_flag);

    fsp_assert_mod_goto(out);

    chan = fsp_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    if (chan->attr.alloc_type == VFMW_FRM_ALLOC_OUTER &&
        chan->attr.outer_frm_mode == VFMW_OUTER_FRM_WAIT && chan->wait_unbind == 1) {
        ret = FSP_ERR;
    }
#ifdef VFMW_VMM_SUPPORT
    total_disp_num = chan->fs_mem.total_disp_num;
    if (chan->cfg.resize_ref == TD_TRUE && chan->fs_mem.need.disp_info.disp_num != 0 &&
        total_disp_num >= chan->cfg.need_disp_num) {
        total_disp_num -= chan->cfg.need_disp_num;
    }
    if (chan->fs_mem.resize_succ == 0 &&
        ((chan->fs_mem.total_dec_num > chan->fs_mem.need.dec_info.dec_num + MAX_WAIT_FRM) ||
         (total_disp_num > chan->fs_mem.need.disp_info.disp_num + MAX_WAIT_FRM))) {
        fsp_rel_part_fs(chan);
        ret = FSP_ERR;
    }
#endif

out:

    fsp_unlock(chan_id, &lock_flag);

    return ret;
}

td_s32 fsp_get_post_num(td_s32 chan_id, td_s32 width, td_s32 height)
{
    td_s32 post_num = 0;
    fsp_chan *chan = TD_NULL;

    chan = fsp_get_chan(chan_id);
    if (chan == TD_NULL) {
        return post_num;
    }

    /* if vdh 2 path output, has no ocupied num for dec path */
    if (chan->fs_mem.need.disp_info.is_disp_need == 1) {
        return 0;
    }

    if (chan->attr.android_test == 0 &&
        width <= 1920 && height <= 1088) { /* 1920 :a number 1088 :a number */
        post_num = 4; /* 4 :a number */
    } else {
        post_num = 1;
    }

    return post_num;
}

td_s32 fsp_set_ext_intf(td_s32 chan_id, const td_void *args)
{
    td_s32 ret;
    fsp_chan *chan = TD_NULL;
    vfmw_fsp_frame_intf *ext_intf = TD_NULL;
    td_ulong lock_flag;

    if (args == TD_NULL) {
        return FSP_ERR;
    }

    ret = FSP_ERR;
    fsp_lock(chan_id, &lock_flag);

    fsp_assert_mod_goto(out);

    chan = fsp_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    ext_intf = (vfmw_fsp_frame_intf *)args;
    chan->ext_intf.mark_image_state = ext_intf->mark_image_state;
    chan->ext_intf.mark_field_flag = ext_intf->mark_field_flag;
    chan->ext_intf.update_image_info = ext_intf->update_image_info;
    chan->ext_intf.try_rels_image = ext_intf->try_rels_image;

    ret = FSP_OK;

out:

    fsp_unlock(chan_id, &lock_flag);

    return ret;
}

td_s32 fsp_new_pmv(td_s32 chan_id, td_s32 size)
{
    td_s32 phy_id = -1;
    td_s32 idx;
    fsp_chan *chan = TD_NULL;
    fsp_pmv *pmv_fs = TD_NULL;

    fsp_assert_mod_goto(out);

    chan = fsp_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    /* for vp9 only one pmv */
    if (chan->cfg.need_pmv_num == 1 &&
        chan->pmv_fs[0].is_valid == 1) {
        phy_id = 0;
        pmv_fs = &chan->pmv_fs[0];
        pmv_fs->is_use = 1;
    } else {
        for (idx = 0; idx < FSP_PMV_NUM; idx++) {
            pmv_fs = chan->pmv_fs + idx;
            if (fsp_is_pmv_free(pmv_fs, (td_u32)size) == TD_TRUE) {
                pmv_fs->is_use = 1;
                phy_id = idx;
                break;
            }
        }
    }

out:

    return phy_id;
}

UADDR fsp_get_pmv(td_s32 chan_id, td_s32 logic_id)
{
    UADDR pmv_addr = 0;
    fsp_logic *logic_fs = TD_NULL;
    fsp_pmv *pmv_fs = TD_NULL;

    logic_fs = fsp_get_logic(chan_id, logic_id);
    vfmw_check_goto(logic_fs != TD_NULL, out);

    if (logic_fs->is_use == 1) {
        pmv_fs = logic_fs->pmv_fs;
        if (pmv_fs != TD_NULL && pmv_fs->is_use == 1) {
            pmv_addr = pmv_fs->phy_addr;
        }
    }

out:

    return pmv_addr;
}

td_s32 fsp_free_pmv(td_s32 chan_id, td_s32 logic_id)
{
    td_s32 ret = FSP_ERR;
    fsp_logic *logic_fs = TD_NULL;
    fsp_pmv *pmv_fs = TD_NULL;

    logic_fs = fsp_get_logic(chan_id, logic_id);
    fsp_assert_logic_goto(logic_fs, out);

    pmv_fs = logic_fs->pmv_fs;
    if (pmv_fs == TD_NULL || pmv_fs->is_use == 0) {
        goto out;
    }

    pmv_fs->is_use = 0;
    ret = FSP_OK;

out:

    return ret;
}

STATIC td_s32 fsp_new_attach(fsp_chan *chan, fsp_logic *logic_fs)
{
    td_s32 idx;
    fsp_attach *tmp_nor_fs = TD_NULL;
    fsp_attach *tmp_sec_fs = TD_NULL;

    if (logic_fs->disp_fs->need_attach != TD_TRUE) {
        goto out;
    }

    idx = logic_fs->disp_fs->fs_id;

    tmp_nor_fs = chan->nor_fs + idx;
    if (fsp_is_attach_free(tmp_nor_fs) != TD_TRUE) {
        return FSP_ERR;
    }

    if (chan->fs_mem.need.sec_size > 0) {
        tmp_sec_fs = chan->sec_fs + idx;
        if (fsp_is_attach_free(tmp_sec_fs) != TD_TRUE) {
            return FSP_ERR;
        }
    }

    if (tmp_nor_fs != TD_NULL) {
        tmp_nor_fs->is_use = 1;
    }
    if (tmp_sec_fs != TD_NULL) {
        tmp_sec_fs->is_use = 1;
    }

out:
    logic_fs->nor_fs = tmp_nor_fs;
    logic_fs->sec_fs = tmp_sec_fs;

    return FSP_OK;
}

STATIC td_s32 fsp_new_pmv_fs(fsp_chan *chan, fsp_logic *logic_fs, const mem_need_info *mem_need)
{
    td_s32 pmv_id;
    fsp_logic *link_logic = TD_NULL;
    fsp_pmv *pmv_fs = TD_NULL;

    if (chan->cfg.need_pmv_num > 0) {
        if (mem_need->disp_info.is_disp_need != 0 &&
            logic_fs->logic_type == FSP_TYPE_LINK) {
            link_logic = fsp_get_logic(chan->id, logic_fs->link_id);
            if (link_logic == TD_NULL) {
                return FSP_ERR;
            }
            pmv_fs = link_logic->pmv_fs;
        } else {
            pmv_id = fsp_new_pmv(chan->id, chan->fs_mem.wait_pmv_size);
            if (pmv_id < 0) {
                return FSP_ERR;
            }

            pmv_fs = &chan->pmv_fs[pmv_id];
            pmv_fs->tick = logic_fs->tick;
        }
    }

    logic_fs->pmv_fs = pmv_fs;

    return FSP_OK;
}

STATIC td_s32 fsp_new_dec_fs(fsp_chan *chan, fsp_logic *logic_fs, const mem_need_info *mem_need)
{
    td_s32 idx;
    fsp_logic *link_logic = TD_NULL;
    fsp_phy *dec_fs = TD_NULL;

    if (mem_need->disp_info.is_disp_need != 0 &&
        logic_fs->logic_type == FSP_TYPE_LINK) {
        link_logic = fsp_get_logic(chan->id, logic_fs->link_id);
        if (link_logic == TD_NULL) {
            return FSP_ERR;
        }
        dec_fs = link_logic->dec_fs;
        goto out;
    }

    for (idx = 0; idx < FSP_FRAME_NUM; idx++) {
        dec_fs = chan->dec_fs + idx;
        if (fsp_is_dec_fs_free(chan, dec_fs) == TD_TRUE) {
            dec_fs = &chan->dec_fs[idx];
            dec_fs->fs_id = idx;
            dec_fs->ref_cnt = 0;
            dec_fs->is_use = 1;
            break;
        }
    }

    if (idx >= FSP_FRAME_NUM) {
        return FSP_ERR;
    }

    dec_fs->tick = logic_fs->tick;

out:
    logic_fs->dec_fs = dec_fs;

    return FSP_OK;
}

STATIC td_s32 fsp_new_disp_fs(fsp_chan *chan, fsp_logic *logic_fs, const mem_need_info *mem_need)
{
    td_s32 idx;
    fsp_phy *disp_fs = TD_NULL;

    if (mem_need->disp_info.is_disp_need != 0) {
        for (idx = 0; idx < FSP_FRAME_NUM; idx++) {
            disp_fs = chan->disp_fs + idx;

            if (fsp_is_disp_fs_free(chan, disp_fs) == TD_TRUE) {
                disp_fs = &chan->disp_fs[idx];
                disp_fs->fs_id = idx;
                disp_fs->ref_cnt = 0;
                disp_fs->is_use = 1;
                disp_fs->fs_air_state = logic_fs->fs_air_state;
                break;
            }
        }

        if (idx >= FSP_FRAME_NUM) {
            return FSP_ERR;
        }
    } else {
        disp_fs = logic_fs->dec_fs;
    }

    vfmw_check_ret(disp_fs != TD_NULL, FSP_ERR);
    disp_fs->tick = logic_fs->tick;
    logic_fs->disp_fs = disp_fs;

    return FSP_OK;
}

STATIC td_s32 fsp_new_phy_detail(fsp_chan *chan, fsp_logic *logic_fs, fsp_logic *share_disp_fs,
    const mem_need_info *mem_need)
{
    td_s32 ret;

    if (chan->wait_flush == 1 || chan->wait_unbind == 1) {
        return FSP_ERR;
    }

    ret = fsp_new_pmv_fs(chan, logic_fs, mem_need);
    vfmw_check_goto_no_print(ret == FSP_OK, out);

    ret = fsp_new_dec_fs(chan, logic_fs, mem_need);
    vfmw_check_goto_no_print(ret == FSP_OK, free_pmv);

    if (share_disp_fs == TD_NULL) {
        ret = fsp_new_disp_fs(chan, logic_fs, mem_need);
        vfmw_check_goto_no_print(ret == FSP_OK, free_dec);

        ret = fsp_new_attach(chan, logic_fs);
        vfmw_check_goto_no_print(ret == FSP_OK, free_disp);
    } else {
        logic_fs->disp_fs = share_disp_fs->disp_fs;
        vfmw_check_ret(logic_fs->disp_fs != TD_NULL, FSP_ERR);
        logic_fs->disp_fs->fs_air_state = logic_fs->fs_air_state;
    }

    logic_fs->is_dummy = 0;

    return FSP_OK;

free_disp:
    if (mem_need->disp_info.is_disp_need != 0) {
        logic_fs->disp_fs->is_use = 0;
    }
    logic_fs->disp_fs = TD_NULL;

free_dec:
    if (logic_fs->logic_type != FSP_TYPE_LINK ||
        mem_need->disp_info.is_disp_need == 0) {
        if (logic_fs->dec_fs != TD_NULL) {
            logic_fs->dec_fs->is_use = 0;
        }
    }
    logic_fs->dec_fs = TD_NULL;

free_pmv:
    if (logic_fs->logic_type != FSP_TYPE_LINK ||
        mem_need->disp_info.is_disp_need == 0) {
        if (logic_fs->pmv_fs != TD_NULL) {
            logic_fs->pmv_fs->is_use = 0;
        }
    }
    logic_fs->pmv_fs = TD_NULL;

out:
    return FSP_ERR;
}

td_s32 fsp_new_phy(td_s32 chan_id, td_s32 logic_id, td_s32 share_disp_id)
{
    td_s32 ret;
    fsp_chan *chan = TD_NULL;
    fsp_logic *logic_fs = TD_NULL;
    fsp_logic *share_disp_fs = TD_NULL;
    td_ulong lock_flag;
    mem_need_info mem_need = { 0 };
    td_void (*try_release)(td_s32 chan_id) = TD_NULL;

    ret = FSP_ERR;
    fsp_lock(chan_id, &lock_flag);
    fsp_assert_mod_goto(out);

    chan = fsp_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);
    try_release = chan->ext_intf.try_rels_image;
    fsp_unlock(chan_id, &lock_flag);

    if (try_release != TD_NULL) {
        try_release(chan_id);
    }

    fsp_lock(chan_id, &lock_flag);
    fsp_assert_mod_goto(out);

    chan = fsp_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    logic_fs = fsp_get_logic(chan_id, logic_id);
    vfmw_check_goto(logic_fs != TD_NULL, out);
    vfmw_check_goto(logic_fs->is_use != 0, out);
    if (share_disp_id != -1) {
        share_disp_fs = fsp_get_logic(chan_id, share_disp_id);
        vfmw_check_goto(share_disp_fs != TD_NULL, out);
        vfmw_check_goto(share_disp_fs->is_use != 0, out);
    }

    fsp_get_mem_req(chan, logic_fs, &mem_need);

    chan->fs_mem.wait_dec_size = (td_s32)mem_need.dec_info.dec_size;
    chan->fs_mem.wait_pmv_size = (td_s32)mem_need.pmv_size;
    chan->fs_mem.wait_disp_size = (td_s32)mem_need.disp_info.disp_size;

    ret = fsp_new_phy_detail(chan, logic_fs, share_disp_fs, &mem_need);

out:
    fsp_unlock(chan_id, &lock_flag);

    return ret;
}

UADDR fsp_get_phy(td_s32 chan_id, td_s32 logic_id)
{
    UADDR phy_addr = 0;
    fsp_logic *logic_fs = TD_NULL;
    fsp_phy *phy_fs = TD_NULL;
    fsp_chan *chan = TD_NULL;

    chan = fsp_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    logic_fs = fsp_get_logic(chan_id, logic_id);
    fsp_assert_logic_goto(logic_fs, out);

    phy_fs = logic_fs->dec_fs;
    if (phy_fs != TD_NULL && (phy_fs->is_use == 1)) {
        phy_addr = phy_fs->phy_addr;
    } else if (logic_fs->is_dummy == 1) {
        /* for h264 gap ref pic */
        phy_addr = chan->dec_fs[0].phy_addr;
    }

out:

    return phy_addr;
}

UADDR fsp_get_disp_addr(td_s32 chan_id, td_s32 logic_id)
{
    UADDR phy_addr = 0;
    fsp_logic *logic_fs = TD_NULL;
    fsp_phy *phy_fs = TD_NULL;
    fsp_chan *chan = TD_NULL;

    chan = fsp_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    logic_fs = fsp_get_logic(chan_id, logic_id);
    fsp_assert_logic_goto(logic_fs, out);

    phy_fs = logic_fs->disp_fs;
    if (phy_fs != TD_NULL && (phy_fs->is_use == 1)) {
        phy_addr = phy_fs->phy_addr;
    } else if (logic_fs->is_dummy == 1) {
        /* for h264 gap ref pic */
        phy_addr = chan->disp_fs[0].phy_addr;
    }

out:

    return phy_addr;
}

td_s32 fsp_new_logic(td_s32 chan_id, fsp_fs_type type)
{
    td_s8 logic_id = -1;
    td_s8 idx;
    fsp_chan *chan = TD_NULL;
    fsp_logic *logic_fs = TD_NULL;
    td_ulong lock_flag;

    fsp_lock(chan_id, &lock_flag);

    fsp_assert_mod_goto(out);
    chan = fsp_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    for (idx = 0; idx < FSP_FRAME_NUM; idx++) {
        if (fsp_is_logic_fs_free(&chan->logic_fs[idx], chan->fs_mem.need.disp_info.is_disp_need) == TD_TRUE) {
            chan->logic_fs[idx].is_use = 0;
            break;
        }
    }

    if (idx == FSP_FRAME_NUM) {
        goto out;
    }

    logic_fs = &chan->logic_fs[idx];
    vfmw_check_sec_func(memset_s(logic_fs, sizeof(fsp_logic), 0, sizeof(fsp_logic)));
    logic_fs->logic_id = logic_id = idx;
    logic_fs->disp_state = FSP_DISP_STATE_ALLOC;
    logic_fs->is_use = 1;
    logic_fs->is_dummy = 1;
    logic_fs->is_syn_drop = 0;
    logic_fs->tick = (chan->tick++);
    logic_fs->logic_type = type;

    logic_fs->image.vid_std = chan->cfg.vid_std;
    logic_fs->image.image_id = (td_u32)logic_fs->logic_id;
    logic_fs->image.image_id_1 = -1;
    logic_fs->image.shr_index = -1;
    logic_fs->image.bit_depth = chan->cfg.bit_depth;
    logic_fs->image.image_width = chan->cfg.dec_width;
    logic_fs->image.image_height = chan->cfg.dec_height;
    logic_fs->image.disp_info.disp_width = chan->cfg.disp_width;
    logic_fs->image.disp_info.disp_height = chan->cfg.disp_height;
    logic_fs->image.dec_info.dec_width = chan->fs_mem.need.dec_info.dec_width;
    logic_fs->image.dec_info.dec_height = chan->fs_mem.need.dec_info.dec_height;
    logic_fs->image.tick = logic_fs->tick;
    logic_fs->image.pts = VFMW_INVALID_PTS;
    logic_fs->fs_air_state = FS_AIR_STATE_IDLE;

out:
    fsp_unlock(chan_id, &lock_flag);

    return logic_id;
}

td_s32 fsp_link_logic(td_s32 chan_id, td_s32 link_id)
{
    td_s8 logic_id = -1;
    td_s32 idx;
    fsp_chan *chan = TD_NULL;
    fsp_logic *logic_fs = TD_NULL;
    fsp_logic *link_logic = TD_NULL;
    td_ulong lock_flag;

    fsp_lock(chan_id, &lock_flag);

    fsp_assert_mod_goto(out);
    chan = fsp_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    link_logic = fsp_get_logic(chan_id, link_id);
    if (link_logic == TD_NULL) {
        goto out;
    }

    for (idx = 0; idx < FSP_FRAME_NUM; idx++) {
        if (fsp_is_logic_fs_free(&chan->logic_fs[idx], chan->fs_mem.need.disp_info.is_disp_need) == TD_TRUE) {
            chan->logic_fs[idx].is_use = 0;
            break;
        }
    }

    if (idx == FSP_FRAME_NUM) {
        goto out;
    }

    logic_fs = &chan->logic_fs[idx];
    vfmw_check_sec_func(memset_s(logic_fs, sizeof(fsp_logic), 0, sizeof(fsp_logic)));

    logic_fs->logic_id = logic_id = (td_s8)idx;
    logic_fs->disp_state = FSP_DISP_STATE_ALLOC;
    logic_fs->is_use = 1;
    logic_fs->is_dummy = 1;
    logic_fs->tick = (chan->tick++);
    logic_fs->logic_type = FSP_TYPE_LINK;
    logic_fs->link_id = (td_s8)link_id;

    logic_fs->image.vid_std = chan->cfg.vid_std;
    logic_fs->image.image_id = (td_u32)logic_fs->logic_id;
    logic_fs->image.image_id_1 = -1;
    logic_fs->image.shr_index = -1;
    logic_fs->image.bit_depth = chan->cfg.bit_depth;
    logic_fs->image.image_width = chan->cfg.dec_width;
    logic_fs->image.image_height = chan->cfg.dec_height;
    logic_fs->image.disp_info.disp_width = chan->cfg.disp_width;
    logic_fs->image.disp_info.disp_height = chan->cfg.disp_height;
    logic_fs->image.pts = VFMW_INVALID_PTS;
    logic_fs->image.tick = logic_fs->tick;

out:
    fsp_unlock(chan_id, &lock_flag);

    return logic_id;
}

td_s32 fsp_null_logic(td_s32 chan_id)
{
    td_s32 logic_id;
    td_s32 idx;
    fsp_chan *chan = TD_NULL;
    fsp_logic *logic_fs = TD_NULL;
    td_ulong lock_flag;

    logic_id = -1; /* -1 unused id */

    fsp_lock(chan_id, &lock_flag);

    fsp_assert_mod_goto(out);
    chan = fsp_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    for (idx = 0; idx < FSP_FRAME_NUM; idx++) {
        if (fsp_is_logic_fs_free(&chan->logic_fs[idx], chan->fs_mem.need.disp_info.is_disp_need) == TD_TRUE) {
            chan->logic_fs[idx].is_use = 0;
            break;
        }
    }

    if (idx == FSP_FRAME_NUM) {
        goto out;
    }

    logic_fs = &chan->logic_fs[idx];
    logic_fs->logic_id = (td_s8)idx;
    logic_id = idx;

    logic_fs->is_ref = 0;
    logic_fs->is_dec_ref = 0;
    logic_fs->dec_fs = TD_NULL;
    logic_fs->disp_fs = TD_NULL;
    logic_fs->pmv_fs = TD_NULL;
    logic_fs->nor_fs = TD_NULL;
    logic_fs->sec_fs = TD_NULL;
    logic_fs->dec_state.bits.top_state = FSP_DEC_STATE_IDLE;
    logic_fs->dec_state.bits.bot_state = FSP_DEC_STATE_IDLE;
    logic_fs->disp_state = FSP_DISP_STATE_IDLE;
    logic_fs->tick = (chan->tick++);
    logic_fs->err_flag = 0;
    logic_fs->ref_err = 0;

    vfmw_check_sec_func(memset_s(&logic_fs->image, sizeof(logic_fs->image), 0, sizeof(vfmw_image)));

out:
    fsp_unlock(chan_id, &lock_flag);

    return logic_id;
}

fsp_logic *fsp_get_logic(td_s32 chan_id, td_s32 logic_id)
{
    fsp_logic *logic_fs = TD_NULL;
    fsp_chan *chan = TD_NULL;

    fsp_assert_mod_goto(out);

    if (logic_id >= 0 && logic_id < FSP_FRAME_NUM) {
        chan = fsp_get_chan(chan_id);
        if (chan != TD_NULL) {
            logic_fs = &chan->logic_fs[logic_id];
        }
    }

out:

    return logic_fs;
}

td_s32 fsp_clear_logic(td_s32 chan_id, td_s32 logic_id, td_s32 hold_disp)
{
    td_s32 ret;
    fsp_logic *logic_fs = TD_NULL;
    td_ulong lock_flag;

    vfmw_unused(hold_disp);

    ret = FSP_ERR;
    fsp_lock(chan_id, &lock_flag);

    logic_fs = fsp_get_logic(chan_id, logic_id);
    fsp_assert_logic_goto(logic_fs, out);

    logic_fs->is_ref = 0;
    if (logic_fs->disp_state != FSP_DISP_STATE_WAIT) {
        logic_fs->disp_state = FSP_DISP_STATE_IDLE;
    }

    if (logic_fs->disp_state == FSP_DISP_STATE_IDLE &&
        logic_fs->dec_state.bits.top_state == FSP_DEC_STATE_OVER &&
        logic_fs->dec_state.bits.bot_state == FSP_DEC_STATE_OVER) {
        logic_fs->dec_state.data = FSP_DEC_STATE_IDLE;

        fsp_free_phy_fs(logic_fs);
    }

    ret = FSP_OK;

out:

    fsp_unlock(chan_id, &lock_flag);

    return ret;
}

td_s32 fsp_clear_idle(td_s32 chan_id)
{
    vfmw_unused(chan_id);

    return FSP_OK;
}


td_s32 fsp_set_image(td_s32 chan_id, td_s32 logic_id, const image_buf *img_buf)
{
    td_ulong lock_flag;
    td_s32 ret = FSP_ERR;
    fsp_logic *logic_fs = fsp_get_logic(chan_id, logic_id);
    vfmw_image *img = TD_NULL;
    fsp_chan *chan = fsp_get_chan(chan_id);

    vfmw_check_ret(img_buf != TD_NULL, FSP_ERR);

    fsp_lock(chan_id, &lock_flag);

    fsp_assert_logic_goto(logic_fs, out);

    img = &logic_fs->image;

    img->disp_info.frm_mem_fd = img_buf->mem_fd;
    img->disp_info.frm_dma_buf = img_buf->dma_buf;
    img->line_num_phy_addr = img_buf->line_num_phy_addr;
    img->bit_depth = img_buf->bit_depth;

    img->disp_info.linear_en = (td_u8)img_buf->vdh_2d_en;
    img->disp_info.tile_format = (td_u8)img_buf->tile_format;
    img->disp_info.compress_en = (td_u8)img_buf->compress_en;
    img->disp_info.head_info_size = img_buf->head_info_size;
    img->disp_info.head_stride = img_buf->head_stride;
    img->disp_info.luma_phy_addr = img_buf->luma_phy_addr;
    img->disp_info.luma_vir_addr = img_buf->luma_vir_addr;
    img->disp_info.chrom_phy_addr = img_buf->chrom_phy_addr;
    img->disp_info.y_stride = img_buf->image_stride;
    img->disp_info.luma_phy_addr_2bit = img_buf->luma_2bit_phy_addr;
    img->disp_info.chrom_phy_addr_2bit = img_buf->chrom_2bit_phy_addr;
    img->disp_info.stride_2bit = img_buf->image_stride_2bit;
    img->disp_info.sec_info = img_buf->sec_info;
    img->disp_info.av1_film_tab_offset = img_buf->av1_film_tab_offset;

    img->attach_buf.attached_handle = img_buf->ext_att_handle;
    img->attach_buf.attached_phy_addr = img_buf->ext_att_phy_addr;
    img->attach_buf.attached_vir_addr = img_buf->ext_att_vir_addr;
    img->attach_buf.attached_size = (td_u32)img_buf->ext_att_buf_size;
    img->attach_buf.attached_offset = img_buf->ext_attr_offset;

    img->attach_buf.hfbc_handle = img_buf->ext_hfbc_handle;
    img->attach_buf.hfbc_vir_addr = img_buf->ext_hfbc_vir_addr;
    img->buf_size = img_buf->buf_size;

    img->disp_info.is_valid = 1; /* is_valid must be last for fence */

    if (chan != TD_NULL && chan->ext_intf.update_image_info != TD_NULL) {
        chan->ext_intf.update_image_info(chan_id, img);
    }

    ret = FSP_OK;

out:

    fsp_unlock(chan_id, &lock_flag);

    return ret;
}

vfmw_image *fsp_get_image(td_s32 chan_id, td_s32 logic_id)
{
    vfmw_image *img = TD_NULL;
    fsp_logic *logic_fs = TD_NULL;

    logic_fs = fsp_get_logic(chan_id, logic_id);
    fsp_assert_logic_goto(logic_fs, out);

    img = &logic_fs->image;

out:

    return img;
}

td_s32 fsp_set_ref(td_s32 chan_id, td_s32 logic_id, td_s32 is_ref)
{
    td_s32 ret;
    fsp_logic *logic_fs = TD_NULL;

    ret = FSP_ERR;
    logic_fs = fsp_get_logic(chan_id, logic_id);
    fsp_assert_logic_goto(logic_fs, out);

    logic_fs->is_ref = (td_s8)is_ref;
    ret = FSP_OK;

out:

    return ret;
}

td_s32 fsp_set_ref_cnt(td_s32 chan_id, td_s32 logic_id, td_s32 delta)
{
    td_s32 ret = FSP_ERR;
    fsp_logic *logic_fs = TD_NULL;
    td_ulong lock_flag;

    if (delta != 1 && delta != -1) {
        return FSP_ERR;
    }

    fsp_lock(chan_id, &lock_flag);

    logic_fs = fsp_get_logic(chan_id, logic_id);
    fsp_assert_logic_goto(logic_fs, out);

    if (logic_fs->is_dummy == 1) {
        goto out;
    }

    if (logic_fs->dec_fs != TD_NULL) {
        logic_fs->dec_fs->ref_cnt += delta;

        if (logic_fs->dec_fs->ref_cnt < 0) {
            logic_fs->dec_fs->ref_cnt = 0;
        }
    }

    fsp_free_phy_fs(logic_fs);

    ret = FSP_OK;

out:

    fsp_unlock(chan_id, &lock_flag);

    return ret;
}

td_s32 fsp_set_dec_ref(td_s32 chan_id, td_s32 logic_id)
{
    td_s32 ret = FSP_ERR;
    fsp_logic *logic_fs = TD_NULL;
    td_ulong lock_flag;

    fsp_lock(chan_id, &lock_flag);

    logic_fs = fsp_get_logic(chan_id, logic_id);
    fsp_assert_logic_goto(logic_fs, out);

    if (logic_fs->is_dummy == 1) {
        goto out;
    }

    logic_fs->is_dec_ref = 1;
    if (logic_fs->dec_fs != TD_NULL) {
        logic_fs->dec_fs->ref_cnt += FSP_REF_SKIP;
    }

    ret = FSP_OK;

out:

    fsp_unlock(chan_id, &lock_flag);

    return ret;
}

STATIC td_void fsp_clear_dec_ref_cnt(fsp_logic *logic_fs)
{
    logic_fs->is_dec_ref = 0;
    if (logic_fs->dec_fs != TD_NULL) {
        logic_fs->dec_fs->ref_cnt -= FSP_REF_SKIP;
        if (logic_fs->dec_fs->ref_cnt < 0) {
            logic_fs->dec_fs->ref_cnt = 0;
        }
    }
}

td_s32 fsp_clear_dec_ref(td_s32 chan_id, td_s32 logic_id)
{
    td_s32 ret = FSP_ERR;
    fsp_logic *logic_fs = TD_NULL;
    td_ulong lock_flag;

    fsp_lock(chan_id, &lock_flag);

    logic_fs = fsp_get_logic(chan_id, logic_id);
    fsp_assert_logic_goto(logic_fs, out);

    if (logic_fs->is_dummy == 1) {
        goto out;
    }

    logic_fs->is_dec_ref = 0;
    if (logic_fs->dec_fs != TD_NULL) {
        logic_fs->dec_fs->ref_cnt -= FSP_REF_SKIP;

        if (logic_fs->dec_fs->ref_cnt < 0) {
            logic_fs->dec_fs->ref_cnt = 0;
        }
    }

    if ((logic_fs->disp_state == FSP_DISP_STATE_IDLE) &&
        (logic_fs->dec_state.bits.top_state == FSP_DEC_STATE_OVER) &&
        (logic_fs->dec_state.bits.bot_state == FSP_DEC_STATE_OVER)) {
        logic_fs->dec_state.data = FSP_DEC_STATE_IDLE;
    }

    fsp_free_phy_fs(logic_fs);

    ret = FSP_OK;

out:

    fsp_unlock(chan_id, &lock_flag);

    return ret;
}

/* clear dec phy mem when it not be ref and also not dispaly */
td_s32 fsp_clear_useless_dec_fs(td_s32 chan_id)
{
    td_s32 ret;
    fsp_chan *chan = TD_NULL;
    fsp_logic *logic_fs = TD_NULL;
    td_ulong lock_flag;
    td_bool dec_over;
    td_s32 idx;

    ret = FSP_ERR;
    fsp_lock(chan_id, &lock_flag);

    chan = fsp_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    for (idx = 0; idx < FSP_FRAME_NUM; idx++) {
        logic_fs = &chan->logic_fs[idx];

        if (logic_fs->is_dummy == 1) {
            continue;
        }

        dec_over = fsp_is_dec_over(logic_fs);
        if (chan->tick - logic_fs->tick > MAX_DPS_NUM &&
            logic_fs->image.format.field_valid_flag == 0) {
            /* err stm single field unmark valid flag */
            dec_over = TD_TRUE;
        }

        if (dec_over == TD_TRUE && logic_fs->is_syn_drop == 1) {
            fsp_clear_dec_ref_cnt(logic_fs);
            logic_fs->is_syn_drop = 0;
        }

        if ((dec_over == TD_TRUE) &&
            (logic_fs->disp_state == FSP_DISP_STATE_IDLE) &&
            (logic_fs->is_dec_ref == 0) &&
            (logic_fs->is_ref == 0)) {
            logic_fs->dec_state.data = FSP_DEC_STATE_IDLE;
        }

        fsp_free_phy_fs(logic_fs);
    }

    ret = FSP_OK;

out:

    fsp_unlock(chan_id, &lock_flag);

    return ret;
}

td_s32 fsp_get_ref(td_s32 chan_id, td_s32 logic_id)
{
    td_s32 ref;
    fsp_logic *logic_fs = TD_NULL;

    ref = 0;
    logic_fs = fsp_get_logic(chan_id, logic_id);
    fsp_assert_logic_goto(logic_fs, out);

    ref = logic_fs->is_ref;

out:

    return ref;
}

td_s32 fsp_get_dec_ref(td_s32 chan_id, td_s32 logic_id)
{
    td_s32 ref;
    fsp_logic *logic_fs = TD_NULL;

    ref = 0;
    logic_fs = fsp_get_logic(chan_id, logic_id);
    fsp_assert_logic_goto(logic_fs, out);

    ref = logic_fs->is_dec_ref;

out:

    return ref;
}

td_s32 fsp_set_wait(td_s32 chan_id, td_s32 logic_id, td_s32 structure)
{
    td_s32 ret;
    td_u32 mask;
    fsp_logic *logic_fs = TD_NULL;
    td_ulong lock_flag;

    ret = FSP_ERR;
    fsp_lock(chan_id, &lock_flag);

    logic_fs = fsp_get_logic(chan_id, logic_id);
    fsp_assert_logic_goto(logic_fs, out);

    mask = (td_u32)(structure == 0 ? 3 : structure); /* 3 :a number */
    if ((mask & 1) != 0) { /* top field */
        logic_fs->dec_state.bits.top_state = FSP_DEC_STATE_WAIT;
    }
    if ((mask & 2) != 0) { /* bot field  2 :a number */
        logic_fs->dec_state.bits.bot_state = FSP_DEC_STATE_WAIT;
    }

    ret = FSP_OK;

out:

    fsp_unlock(chan_id, &lock_flag);

    return ret;
}

td_s32 fsp_clear_dec(td_s32 chan_id, td_s32 logic_id)
{
    td_s32 ret;
    td_u8 state;
    td_u32 mask;
    fsp_chan *chan = TD_NULL;
    fsp_logic *logic_fs = TD_NULL;
    td_ulong lock_flag;

    ret = FSP_ERR;
    fsp_lock(chan_id, &lock_flag);

    fsp_assert_mod_goto(out);
    chan = fsp_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    logic_fs = fsp_get_logic(chan_id, logic_id);
    fsp_assert_logic_goto(logic_fs, out);

    mask = 3; /* structure is 3 */
    state = FSP_DEC_STATE_IDLE;

    if ((mask & 1) != 0) { /* top field */
        logic_fs->dec_state.bits.top_state = state;
        logic_fs->image.dec_over.bits.top_over = state;
    }
    if ((mask & 2) != 0) { /* bot field  2 :a number */
        logic_fs->dec_state.bits.bot_state = state;
        logic_fs->image.dec_over.bits.bot_over = state;
    }

    if (logic_fs->dec_state.data == FSP_DEC_STATE_IDLE) {
        fsp_free_phy_fs(logic_fs);
    }

#ifdef VFMW_VMM_SUPPORT
    fsp_rel_part_fs(chan);
#endif

    ret = FSP_OK;

out:

    fsp_unlock(chan_id, &lock_flag);

    return ret;
}

td_s32 fsp_set_fld_flag(td_s32 chan_id, td_s32 logic_id, td_s32 fld_flag)
{
    td_s32 ret;
    fsp_logic *logic_fs = TD_NULL;
    fsp_chan *chan = TD_NULL;

    ret = FSP_ERR;
    chan = fsp_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    logic_fs = fsp_get_logic(chan_id, logic_id);
    fsp_assert_logic_goto(logic_fs, out);

    logic_fs->image.format.field_valid_flag = (td_u8)fld_flag;

    if (chan->ext_intf.mark_field_flag != TD_NULL) {
        chan->ext_intf.mark_field_flag(chan_id, &logic_fs->image);
    }

    ret = FSP_OK;

out:

    return ret;
}

td_u32 fsp_get_dec(td_s32 chan_id, td_s32 logic_id)
{
    td_u32 state;
    fsp_logic *logic_fs = TD_NULL;
    td_ulong lock_flag;

    state = 0;
    fsp_lock(chan_id, &lock_flag);

    logic_fs = fsp_get_logic(chan_id, logic_id);
    fsp_assert_logic_goto(logic_fs, out);

    state = (td_u32)logic_fs->dec_state.data;

out:

    fsp_unlock(chan_id, &lock_flag);

    return state;
}

td_s32 fsp_set_disp(td_s32 chan_id, td_s32 logic_id, td_s32 disp_state)
{
    td_s32 ret;
    fsp_logic *logic_fs = TD_NULL;
    td_ulong lock_flag;

    ret = FSP_ERR;
    fsp_lock(chan_id, &lock_flag);

    logic_fs = fsp_get_logic(chan_id, logic_id);
    fsp_assert_logic_goto(logic_fs, out);

    if (disp_state == 1) {
        logic_fs->is_in_voq = 1;
        logic_fs->disp_state = FSP_DISP_STATE_WAIT;
    } else {
        if (logic_fs->disp_fs != TD_NULL &&
            logic_fs->is_in_voq == 1 &&
            logic_fs->disp_fs->fs_air_state == FS_AIR_STATE_OVER) {
            logic_fs->disp_fs->fs_air_state = FS_AIR_STATE_IDLE;
        }
        logic_fs->is_in_voq = 0;
        logic_fs->disp_state = FSP_DISP_STATE_IDLE;

        fsp_free_phy_fs(logic_fs);
    }

    ret = FSP_OK;

out:

    fsp_unlock(chan_id, &lock_flag);

    return ret;
}

td_s32 fsp_get_disp(td_s32 chan_id, td_s32 logic_id)
{
    td_s32 disp;
    fsp_logic *logic_fs = TD_NULL;

    disp = 0;
    logic_fs = fsp_get_logic(chan_id, logic_id);
    fsp_assert_logic_goto(logic_fs, out);

    disp = logic_fs->disp_state;

out:

    return disp;
}

fsp_phy *fsp_get_dec_fs(td_s32 chan_id, td_s32 logic_id)
{
    fsp_phy *dec_fs = TD_NULL;
    fsp_logic *logic_fs = TD_NULL;

    logic_fs = fsp_get_logic(chan_id, logic_id);
    fsp_assert_logic_goto(logic_fs, out);

    dec_fs = logic_fs->dec_fs;

out:

    return dec_fs;
}

td_s32 fsp_set_store_type(td_s32 chan_id, td_s32 logic_id, td_s32 type)
{
    td_s32 ret;
    fsp_logic *logic_fs = TD_NULL;
    td_ulong lock_flag;

    ret = FSP_ERR;
    fsp_lock(chan_id, &lock_flag);

    logic_fs = fsp_get_logic(chan_id, logic_id);
    fsp_assert_logic_goto(logic_fs, out);

    logic_fs->store_type = (td_u8)type;

    ret = FSP_OK;

out:

    fsp_unlock(chan_id, &lock_flag);

    return ret;
}

td_s32 fsp_get_store_type(td_s32 chan_id, td_s32 logic_id)
{
    td_s32 type;
    fsp_logic *logic_fs = TD_NULL;
    td_ulong lock_flag;

    type = FSP_STORE_TYPE_BUTT;
    fsp_lock(chan_id, &lock_flag);

    logic_fs = fsp_get_logic(chan_id, logic_id);
    fsp_assert_logic_goto(logic_fs, out);

    type = (td_s32)logic_fs->store_type;

out:

    fsp_unlock(chan_id, &lock_flag);

    return type;
}

STATIC td_void fsp_update_image_foramt(td_s32 chan_id, vfmw_image *dst_img, const vfmw_image *src_img)
{
    td_ulong img_lock_flag;

    vfmw_unused(chan_id);
    vfmw_check(src_img != TD_NULL);
    vfmw_check(dst_img != TD_NULL);

    fsp_img_format_lock(&img_lock_flag);
    dst_img->format.video_format = src_img->format.video_format;
    dst_img->format.sample_type = src_img->format.sample_type;

    /* for H.264 initpic */
    dst_img->format.frame_type = src_img->format.frame_type;
    dst_img->format.source_format = src_img->format.source_format;
    dst_img->format.field_valid_flag = src_img->format.field_valid_flag;
    dst_img->format.top_field_first = src_img->format.top_field_first;
    dst_img->format.aspect_ratio = src_img->format.aspect_ratio;
    fsp_img_format_unlock(&img_lock_flag);
}

td_s32 fsp_update_image_param(td_s32 chan_id, td_s32 logic_id, const vfmw_image *src_img)
{
    td_s32 ret;
    vfmw_image *dst_img = TD_NULL;
    td_ulong lock_flag;

    ret = FSP_ERR;
    fsp_lock(chan_id, &lock_flag);
    vfmw_check_goto(src_img != TD_NULL, out);

    dst_img = fsp_get_image(chan_id, logic_id);
    vfmw_check_goto(dst_img != TD_NULL, out);

    dst_img->pts = src_img->pts;
    dst_img->user_tag = src_img->user_tag;
    dst_img->disp_time = src_img->disp_time;
    dst_img->disp_enable_flag = src_img->disp_enable_flag;
    dst_img->disp_frame_distance = src_img->disp_frame_distance;
    dst_img->distance_before_first_frame = src_img->distance_before_first_frame;
    dst_img->gop_num = src_img->gop_num;
    dst_img->dv_dual_layer = src_img->dv_dual_layer;

    dst_img->image_width = src_img->image_width;
    dst_img->image_height = src_img->image_height;
    dst_img->aspect_width = src_img->aspect_width;
    dst_img->aspect_height = src_img->aspect_height;

    dst_img->is_fld_flag = src_img->is_fld_flag;
    dst_img->left_offset = src_img->left_offset;
    dst_img->right_offset = src_img->right_offset;
    dst_img->top_offset = src_img->top_offset;
    dst_img->bottom_offset = src_img->bottom_offset;
    dst_img->disp_info.disp_width = (td_u32)src_img->disp_info.disp_width;
    dst_img->disp_info.disp_height = (td_u32)src_img->disp_info.disp_height;
    dst_img->bit_depth = src_img->bit_depth;
    dst_img->disp_info.disp_center_x = src_img->disp_info.disp_center_x;
    dst_img->disp_info.disp_center_y = src_img->disp_info.disp_center_y;
    dst_img->afd.active_format = src_img->afd.active_format;
    dst_img->afd.active_format_flag = src_img->afd.active_format_flag;
    dst_img->view_id = src_img->view_id;
    dst_img->splice_info.type = src_img->splice_info.type;
    dst_img->splice_info.ori_width = src_img->splice_info.ori_width;
    dst_img->splice_info.ori_height = src_img->splice_info.ori_height;

    fsp_update_image_foramt(chan_id, dst_img, src_img);

    ret = FSP_OK;

out:

    fsp_unlock(chan_id, &lock_flag);

    return ret;
}

td_s32 fsp_update_image_ext(td_s32 chan_id, td_s32 logic_id, const vfmw_image *src_img)
{
    td_s32 ret;
    vfmw_image *dst_img = TD_NULL;
    td_ulong lock_flag;
    td_ulong img_lock_flag;

    ret = FSP_ERR;
    fsp_lock(chan_id, &lock_flag);

    vfmw_check_goto(src_img != TD_NULL, out);

    dst_img = fsp_get_image(chan_id, logic_id);
    vfmw_check_goto(dst_img != TD_NULL, out);

    fsp_img_format_lock(&img_lock_flag);
    dst_img->frame_rate = src_img->frame_rate;
    dst_img->format.top_field_first = src_img->format.top_field_first;
    dst_img->format.field_valid_flag = src_img->format.field_valid_flag;
    dst_img->format.source_format = src_img->format.source_format;
    dst_img->format.frame_type = src_img->format.frame_type;
    dst_img->format.frame_rate_valid = src_img->format.frame_rate_valid;
    fsp_img_format_unlock(&img_lock_flag);

    ret = FSP_ERR;

out:

    fsp_unlock(chan_id, &lock_flag);

    return ret;
}

td_s32 fsp_check_img_dec_state(const vfmw_image *img, td_s32 state)
{
    td_u32 field_valid_flag;
    td_u32 check_rlt = 1;
    td_ulong img_lock_flag;

    if (img == TD_NULL) {
        return FSP_ERR;
    }

    fsp_img_format_lock(&img_lock_flag);
    field_valid_flag = img->format.field_valid_flag;
    fsp_img_format_unlock(&img_lock_flag);

    if (field_valid_flag == 0) {
        return FSP_ERR;
    }

    if (state == FSP_DEC_STATE_ERR) {
        check_rlt &=
           (td_u32)((field_valid_flag & 1) == 0 || (img->dec_over.bits.top_over == state));
        check_rlt &=
           (td_u32)((field_valid_flag & 2) == 0 || (img->dec_over.bits.bot_over == state)); /* 2 means bottom flag */
    } else {
        check_rlt &=
           (td_u32)((field_valid_flag & 1) == 0 || (img->dec_over.bits.top_over == state));
        check_rlt &=
           (td_u32)((field_valid_flag & 2) == 0 || (img->dec_over.bits.bot_over == state)); /* 2 means bottom flag */
    }

    return (check_rlt == 1 ? FSP_OK : FSP_ERR);
}

STATIC td_u32 fsp_get_field_over(const vfmw_image *img, td_s32 pic_struct, td_s32 err_ratio)
{
    td_u32 field_over;

    vfmw_unused(err_ratio);
    if (img == TD_NULL) {
        dprint(PRN_ERROR, "img is null.\n");
        return FSP_ERR;
    }

    field_over = (td_u32)(pic_struct == 0 ? 3 : pic_struct); /* 3 :a number */

    if (pic_struct == 1 &&
        (img->dec_over.bits.bot_over == FSP_DEC_STATE_OVER ||
         img->dec_over.bits.bot_over == FSP_DEC_STATE_ERR)) {
        field_over = 3; /* 3 :a number */
    } else if (pic_struct == 2 && /* 2 :a number */
               (img->dec_over.bits.top_over == FSP_DEC_STATE_OVER ||
                img->dec_over.bits.top_over == FSP_DEC_STATE_ERR)) {
        field_over = 3; /* 3 :a number */
    }

    return field_over;
}

STATIC td_u32 fsp_get_frame_err(const vfmw_image *img, td_s32 pic_struct, td_s32 err_ratio)
{
    td_u32 frame_err;

    vfmw_check_ret(img != TD_NULL, FSP_ERR);

    frame_err = (td_u32)(img->error_level + err_ratio);

    if (pic_struct == 1 &&
        (img->dec_over.bits.bot_over == FSP_DEC_STATE_OVER ||
         img->dec_over.bits.bot_over == FSP_DEC_STATE_ERR)) {
        frame_err = (frame_err + 1) / 2; /* 2 :a number */
    } else if (pic_struct == 2 && /* 2 :a number */
               (img->dec_over.bits.top_over == FSP_DEC_STATE_OVER ||
                img->dec_over.bits.top_over == FSP_DEC_STATE_ERR)) {
        frame_err = (frame_err + 1) / 2; /* 2 :a number */
    }

    frame_err = (frame_err > 100) ? 100 : frame_err; /* vfmw_max error ratio is 100 */

    return frame_err;
}


td_void fsp_set_air_state(td_s32 chan_id, td_s32 logic_id, fsp_air_state air_status)
{
    fsp_chan *chan = TD_NULL;
    fsp_logic *logic_fs = TD_NULL;
    td_ulong lock_flag;

    fsp_lock(chan_id, &lock_flag);

    chan = fsp_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    logic_fs = fsp_get_logic(chan_id, logic_id);
    vfmw_check_goto(logic_fs != TD_NULL, out);

    logic_fs->fs_air_state = air_status;
out:

    fsp_unlock(chan_id, &lock_flag);
}

td_s32 fsp_set_dec_state(td_s32 chan_id, td_s32 logic_id, td_s32 pic_struct, td_s32 err_ratio)
{
    td_s32 ret;
    td_u32 field_over;
    vfmw_image *img = TD_NULL;
    fsp_chan *chan = TD_NULL;
    fsp_logic *logic_fs = TD_NULL;
    td_ulong lock_flag;
    td_u32 frame_err;
    td_s32 dec_state;

    ret = FSP_ERR;
    fsp_lock(chan_id, &lock_flag);

    chan = fsp_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    logic_fs = fsp_get_logic(chan_id, logic_id);
    vfmw_check_goto(logic_fs != TD_NULL, out);
    img = &logic_fs->image;

    field_over = fsp_get_field_over(img, pic_struct, err_ratio);
    frame_err = fsp_get_frame_err(img, pic_struct, err_ratio);
    img->error_level = frame_err;

    dec_state = (chan->attr.uvmos_en == 1 || frame_err <= (td_u32)chan->attr.out_err_thr) ?
        FSP_DEC_STATE_OVER : FSP_DEC_STATE_ERR;

    if (frame_err > (td_u32)chan->attr.out_err_thr || logic_fs->ref_err == 1) {
        logic_fs->err_flag = 1;

        if (logic_fs->ref_err == 1) {
            dec_state = FSP_DEC_STATE_ERR;
        }
    }

    /* In field encoding stream, if the error rate exceeds the error threshold,
    output the top field or bottom field is an i frame marked as a reference error.
    This ensures the smoothness of the playback. */
    /* 3 is frame or top fld && bot fld */
    if ((field_over == 3) && (logic_fs->image.error_level < (td_u32)chan->attr.out_err_thr) &&
        (logic_fs->ref_err == 1) && (logic_fs->image.is_fld_flag == 1) &&
        (logic_fs->image.top_fld_type == 0 || logic_fs->image.bottom_fld_type == 0)) {
        dec_state = FSP_DEC_STATE_OVER;
    }

    if ((field_over & 1) != 0) {
        img->dec_over.bits.top_over = (td_u8)dec_state;
        logic_fs->dec_state.bits.top_state = (td_u8)dec_state;
    }

    if ((field_over & 2) != 0) { /* 2 :a number */
        img->dec_over.bits.bot_over = (td_u8)dec_state;
        logic_fs->dec_state.bits.bot_state = (td_u8)dec_state;
    }

    if ((chan->ext_intf.mark_image_state != TD_NULL) &&
        (chan->attr.air_version == AIR_DEC_UNSUPPORT ||
        (chan->attr.air_version == AIR_DEC_FRAME && logic_fs->fs_air_state == FS_AIR_STATE_OVER))) {
        chan->ext_intf.mark_image_state(chan_id, &logic_fs->image, pic_struct, (td_u32)dec_state);
    }

    ret = FSP_OK;

out:

    fsp_unlock(chan_id, &lock_flag);

    return ret;
}

STATIC td_u32 fsp_get_wait_disp_num(fsp_chan *chan)
{
    td_s32 idx;
    td_s32 dec_state;
    td_u32 wait_num = 0;
    fsp_logic *logic_fs = TD_NULL;

    for (idx = 0; idx < FSP_FRAME_NUM; idx++) {
        logic_fs = &chan->logic_fs[idx];
        dec_state = fsp_check_img_dec_state(&logic_fs->image, FSP_DEC_STATE_OVER);
        if (dec_state == FSP_OK &&
            logic_fs->disp_state == FSP_DISP_STATE_WAIT) {
            wait_num++;
        }
    }

    return wait_num;
}

td_s32 fsp_get_info(td_s32 chan_id, const td_void *args)
{
    td_s32 ret;
    td_ulong lock_flag;
    fsp_chan *chan = TD_NULL;
    fsp_info *info = (fsp_info *)args;

    ret = FSP_ERR;
    vfmw_check_ret(info != TD_NULL, FSP_ERR);
    vfmw_check_ret(chan_id >= 0 && chan_id < FSP_CHAN_NUM, FSP_ERR);

    fsp_lock(chan_id, &lock_flag);

    fsp_assert_mod_goto(out);
    chan = fsp_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    info->total_frm_num = chan->fs_mem.total_dec_num;
    info->wait_disp_frm_num = fsp_get_wait_disp_num(chan);
    info->has_dec_fs_free = fsp_has_free_dec(chan_id);
    info->has_pmv_fs_free = fsp_has_free_pmv(chan_id);
    info->has_attach_fs_free = fsp_has_free_attach(chan_id);

    ret = FSP_OK;

out:
    fsp_unlock(chan_id, &lock_flag);

    return ret;
}

/* used for OUTER alloc type */
td_void fsp_flush(td_s32 chan_id)
{
    fsp_chan *chan = TD_NULL;
    td_ulong lock_flag;

    fsp_lock(chan_id, &lock_flag);

    chan = fsp_get_chan(chan_id);
    vfmw_check_goto(chan != TD_NULL, out);

    if (chan->fs_mem.need.disp_info.is_disp_need == 0) {
        fsp_rels_all_dec(chan);
    }
    fsp_rels_all_disp(chan);
    fsp_clear_all_attach(chan);
    if (fsp_clear_unused_logic(chan_id) == FSP_ERR) {
        dprint(PRN_ERROR, "fsp_clear_unused_logic return err!\n");
    }

out:
    fsp_unlock(chan_id, &lock_flag);

    return;
}
