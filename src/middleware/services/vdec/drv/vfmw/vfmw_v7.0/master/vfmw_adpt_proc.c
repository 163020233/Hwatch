/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "vfmw_adpt_proc.h"
#include "vfmw_adpt.h"
#include "vfmw_osal.h"
#include "vfmw_proc.h"
#include "local_proc.h"
#include "liteos_proc.h"
#include "liteos_mem.h"
#include "vfmw_pdt.h"
#include "dbg.h"
#include "pts_ext.h"
#ifdef VFMW_STREAM_SUPPORT
#include "stream_log.h"
#endif
#ifdef VFMW_VPP_SUPPORT
#include "vpp_log.h"
#endif
#ifdef VFMW_USER_SUPPORT
#include "userdec.h"
#endif

#define PROC_VFMW_STM "vfmw_stm"
#define PROC_VFMW_SYN "vfmw_syn"
#define PROC_VFMW_DEC "vfmw_dec"
#define PROC_VFMW_COM "vfmw_com"
#define PROC_VFMW_FSP "vfmw_fsp"
#define PROC_VFMW_KPI "vfmw_kpi"
#define PROC_VFMW_PTS "vfmw_pts"
#define PROC_VFMW_HDR "vfmw_hdr"
#define PROC_VFMW_USR "vfmw_usr"
#define PROC_VFMW_MEM "vfmw_mem"

typedef struct {
    td_s32 curr_chan;
    os_sema proc_sema;
} vfmw_proc_entry;

static vfmw_proc_entry g_vfmw_proc_entry = {
    .curr_chan = 0,
    .proc_sema = -1,
};

static vfmw_proc_entry *vfmw_proc_get_entry(td_void)
{
    return &g_vfmw_proc_entry;
}

STATIC td_void vfmw_proc_sema_down(td_void)
{
    vfmw_proc_entry *entry = vfmw_proc_get_entry();

    os_sema_down(entry->proc_sema);
}

STATIC td_void vfmw_proc_sema_up(td_void)
{
    vfmw_proc_entry *entry = vfmw_proc_get_entry();

    os_sema_up(entry->proc_sema);
}

STATIC td_u32 vfmw_proc_argv_to_cmd_str(const td_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], td_s32 argc,
    cmd_str_ptr cmd_str)
{
    td_u32 index;
    td_s32 ret;
    td_s32 argc_tmp = argc;

    if (argc_tmp > PROC_CMD_SINGEL_LENGTH_MAX) {
        return 0;
    }

    if (argc_tmp > CMD_PARAM_MAX_COUNT) {
        os_print("argc:%d > vfmw_max vfmw cnt %d\n", argc_tmp, CMD_PARAM_MAX_COUNT);
        argc_tmp = CMD_PARAM_MAX_COUNT;
    }

    for (index = 0; index < (td_u32)argc_tmp; index++) {
        ret = snprintf_s((*cmd_str)[index], CMD_PARAM_MAX_LEN, CMD_PARAM_MAX_LEN - 1, *(argv + index));
        if (ret < 0) {
            os_print("snprintf_s failed ret = %d\n", ret);
            return index;
        }
    }

    return index;
}

#define HELP_H "echo   %-17s %-7s %-7s %-15s %s\r\n"
#define HELP_L "echo   %-17s %-7s %-7s %-15s %-45s %s\r\n"

#define help_print(Fmt, Arg...) os_print(Fmt, ##Arg)

STATIC td_void vfmw_proc_help_default(td_void)
{
    help_print(HELP_L, LOG_CMD_PRINT,         "0", " ", PROC_VFMW_COM, "set print control value", "p1(0~MAX_INT)");
    help_print(HELP_L, LOG_CMD_CUR_CHAN,      "0", " ", PROC_VFMW_COM, "set current chan id", "p1(chn_id)");
    help_print(HELP_L, LOG_CMD_ERR_HOLD,      "0", " ", PROC_VFMW_COM, "set error threshold", "p1(0~100)");
    help_print(HELP_L, LOG_CMD_OUT_ORDER,     "0", " ", PROC_VFMW_COM, "set output order", "p1(0:disp/1:dec ...)");
    help_print(HELP_L, LOG_CMD_DEC_MODE,      "0", " ", PROC_VFMW_COM, "set decode mode", "p1(0:IPB/1:IP/2:I ...)");
    help_print(HELP_L, LOG_CMD_DPS_SER,       "0", " ", PROC_VFMW_COM, "turn on/off dps serial mode", "p1(1/0)");
    help_print(HELP_L, LOG_CMD_DPS_NUM,       "0", " ", PROC_VFMW_COM, "set dps num", "p1(0~MAX_INT)");
    help_print(HELP_L, LOG_CMD_VPP_BYPASS,    "0", " ", PROC_VFMW_COM, "set vpp bypass", "p1(chn_id)");
    help_print(HELP_L, LOG_CMD_STM_SLEEP,     "0", " ", PROC_VFMW_STM, "set stm thread sleep time", "p1(0~MAX_INT)");
    help_print(HELP_L, LOG_CMD_STM_WAIT,      "0", " ", PROC_VFMW_STM, "set stm thread wait  time", "p1(0~MAX_INT)");
    help_print(HELP_L, LOG_CMD_STM_BYPASS,    "0", " ", PROC_VFMW_STM, "set stm bypass", "p1(chn_id)");
    help_print(HELP_L, LOG_CMD_SYN_SLEEP,     "0", " ", PROC_VFMW_SYN, "set syn thread sleep time", "p1(0~MAX_INT)");
    help_print(HELP_L, LOG_CMD_SYN_WAIT,      "0", " ", PROC_VFMW_SYN, "set syn thread wait  time", "p1(0~MAX_INT)");
    help_print(HELP_L, LOG_CMD_SYN_BYPASS,    "0", " ", PROC_VFMW_SYN, "set syn bypass", "p1(chn_id)");
    help_print(HELP_L, LOG_CMD_DEC_SLEEP,     "0", " ", PROC_VFMW_DEC, "set dec thread sleep time", "p1(0~MAX_INT)");
    help_print(HELP_L, LOG_CMD_DEC_WAIT,      "0", " ", PROC_VFMW_DEC, "set dec thread wait  time", "p1(0~MAX_INT)");
    help_print(HELP_L, LOG_CMD_DEC_BYPASS,    "0", " ", PROC_VFMW_DEC, "set dec bypass", "p1(chn_id)");
    help_print(HELP_L, LOG_CMD_SAMPLE_NUM,    "0", " ", PROC_VFMW_PTS, "set pts frd sample num", "p1(0~MAX_INT)");
    help_print(HELP_L, LOG_CMD_USR_DEC_NUM,   "0", " ", PROC_VFMW_PTS, "set user dec frame num", "p1(-1:no limit/...)");
#ifdef VFMW_HDR_SUPPORT
    help_print(HELP_L, LOG_CMD_READ_HDR,      "0", " ", PROC_VFMW_HDR, "turn on/off hdr proc info", "p1(1/0)");
#endif
}

STATIC td_void vfmw_proc_help_debug(td_void)
{
#if defined(VDEC_DEBUG_SUPPORT)
    help_print(HELP_L, LOG_CMD_SET_DBG_PATH,  "p", " ", PROC_VFMW_COM, "set debug path", "p1(path)");
    help_print(HELP_L, LOG_CMD_CMP_MODE,      "0", " ", PROC_VFMW_COM, "set cmp mode", "p1(0:adpt/1:on/2:off)");
    help_print(HELP_L, LOG_CMD_FRM_LAYOUT,    "0", " ", PROC_VFMW_COM, "set frm layout", "p1(0:adpt/1:tile/2:linear)");
    help_print(HELP_L, LOG_CMD_MOD_LOW_DLY,   "0", " ", PROC_VFMW_COM, "turn on/off module low delay", "p1(1/0)");
    help_print(HELP_L, LOG_CMD_TUNL_LINE,     "0", " ", PROC_VFMW_COM, "set tunnel line num", "p1(0~MAX_INT)");
    help_print(HELP_L, LOG_CMD_SET_CLK,       "0", " ", PROC_VFMW_COM, "set vdh clock configuration", "p1(clk_val)");
    help_print(HELP_L, LOG_CMD_LINEAR_NO_GAP, "0", " ", PROC_VFMW_COM, "set linear type", "p1(1/0)");
    help_print(HELP_L, LOG_CMD_SHOW_DFLT,     " ", " ", PROC_VFMW_COM, "show vctrl default configuration", "NA");
    help_print(HELP_L, LOG_CMD_VPP_PR_FRM,    "0", " ", PROC_VFMW_COM, "print video frame info", "p1(1/0)");
    help_print(HELP_L, LOG_CMD_VPP_PR_BUF,    "0", " ", PROC_VFMW_COM, "print video buf info", "p1(1/0)");
    help_print(HELP_L, LOG_CMD_VPP_NO_RECV,   "0", " ", PROC_VFMW_COM, "force vpp recv fail", "p1(1/0)");
    help_print(HELP_L, LOG_CMD_DPS_NO_RECV,   "0", " ", PROC_VFMW_COM, "force dps recv fail", "p1(1/0)");
    help_print(HELP_L, LOG_CMD_PRT_STM_BLK,   "0", " ", PROC_VFMW_COM, "turn on/off stream block proc", "p1(1/0)");
    help_print(HELP_L, LOG_CMD_SAVE_VDH_REG,  " ", " ", PROC_VFMW_COM, "save reg of vdh", "NA");
    help_print(HELP_L, LOG_CMD_SAVE_BEGIN,    "0", " ", PROC_VFMW_COM, "set save frame begin index", "p1(0~MAX_INT)");
    help_print(HELP_L, LOG_CMD_SAVE_END,      "0", " ", PROC_VFMW_COM, "set save frame end index", "p1(0~MAX_INT)");
    help_print(HELP_L, LOG_CMD_SAVE_RAW,      "0", " ", PROC_VFMW_COM, "save raw", "p1(chn_id)");
    help_print(HELP_L, LOG_CMD_SAVE_YUV,      "0", " ", PROC_VFMW_COM, "save yuv", "p1(chn_id)");
    help_print(HELP_L, LOG_CMD_SAVE_MTDT,     "0", " ", PROC_VFMW_COM, "save metadata", "p1(chn_id)");
    help_print(HELP_L, LOG_CMD_SAVE_SEG,      "0", " ", PROC_VFMW_STM, "save seg", "p1(chn_id)");
    help_print(HELP_L, LOG_CMD_SAVE_SEEK_RAW, "0", " ", PROC_VFMW_STM, "turn on/off save stream after seek", "p1(1/0)");
    help_print(HELP_L, LOG_CMD_STM_NO_RAW,    "0", " ", PROC_VFMW_STM, "force stm no raw",   "p1(1/0)");
    help_print(HELP_L, LOG_CMD_STM_NO_SEG,    "0", " ", PROC_VFMW_STM, "force stm no seg",   "p1(1/0)");
    help_print(HELP_L, LOG_CMD_STM_SEG_FULL,  "0", " ", PROC_VFMW_STM, "force stm seg full", "p1(1/0)");
    help_print(HELP_L, LOG_CMD_NO_RECV_SEG,   "0", " ", PROC_VFMW_SYN, "force syn recv seg fail", "p1(1/0)");
    help_print(HELP_L, LOG_CMD_DEC_CHECK,     "0", " ", PROC_VFMW_DEC, "set dec thread check mem",  "p1(0~MAX_INT)");
    help_print(HELP_L, LOG_CMD_FSP_NO_DEC,    "0", " ", PROC_VFMW_FSP, "force fsp no dec",    "p1(1/0)");
    help_print(HELP_L, LOG_CMD_FSP_NO_DSP,    "0", " ", PROC_VFMW_FSP, "force fsp no dsp",    "p1(1/0)");
    help_print(HELP_L, LOG_CMD_FSP_NO_PMV,    "0", " ", PROC_VFMW_FSP, "force fsp no pmv",    "p1(1/0)");
    help_print(HELP_L, LOG_CMD_FSP_NO_ATT,    "0", " ", PROC_VFMW_FSP, "force fsp no attach", "p1(1/0)");
#endif
}

STATIC td_void vfmw_proc_help(cmd_str_ptr cmd_str, td_u32 count)
{
    vfmw_unused(cmd_str);
    vfmw_unused(count);

    help_print(LOG_VFMW_HELP_HEAD);
    help_print(HELP_H, "command", "p1", "p2", "target", "comment");

    vfmw_proc_help_default();
    vfmw_proc_help_debug();

    help_print(LOG_VFMW_HELP_TAIL);
}

STATIC td_void vfmw_proc_set_print(cmd_str_ptr cmd_str, td_u32 count)
{
    td_u32 data = 0;

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((*cmd_str)[1], &data);

    (td_void)dbg_set_print_word(data);
    os_print("set print level 0x%x\n", data);
}

STATIC td_void vfmw_proc_set_cur_chan(cmd_str_ptr cmd_str, td_u32 count)
{
    td_u32 chan_id = 0;
    vfmw_proc_entry *entry = vfmw_proc_get_entry();

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((*cmd_str)[1], &chan_id);

    vfmw_check_print(chan_id < VFMW_CHAN_NUM, "chan_id %u invalid.\n", chan_id);

    entry->curr_chan = (td_s32)chan_id;
    os_print("Pay attention to chan %d\n", chan_id);
}

STATIC td_void vfmw_proc_save_reg(cmd_str_ptr cmd_str, td_u32 count)
{
    vfmw_unused(cmd_str);
    vfmw_unused(count);
    dbg_read_reg();
}

STATIC td_void vfmw_proc_set_dbg_path(cmd_str_ptr cmd_str, td_u32 cmd_cnt)
{
    td_s32 ret;
    td_char *path = TD_NULL;

    vfmw_check_print(cmd_cnt >= 0x2, "cmd param cmd_cnt %d invalid.\n", cmd_cnt);

    path = (*cmd_str)[1];
    ret = dbg_set_dbg_path(path, strlen(path));
    vfmw_check(ret == EXT_SUCCESS);

    os_print("set dbg path %s ok\n", path);
}

STATIC td_void vfmw_proc_save_yuv(cmd_str_ptr cmd_str, td_u32 count)
{
    td_u32 chan_id = 0;
    td_char file_name[FILE_NAME_MAX_LEN];
    vfmw_chan *chan = TD_NULL;
    vfmw_entry *entry = TD_NULL;
    td_char *dbg_path = dbg_get_dbg_path();

    entry = vadpt_entry();

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((*cmd_str)[1], &chan_id);
    vfmw_check_print(chan_id < VFMW_CHAN_NUM, "chan_id %d out of range[0,%d).\n", chan_id, VFMW_CHAN_NUM);

    chan = vadpt_chan((td_s32)chan_id);
    vfmw_check_print(chan != TD_NULL, "chan %d invalid.\n", chan_id);

    os_sema_down(entry->dbg_sema);

    if (chan->yuv == TD_NULL) {
        if (snprintf_s(file_name, sizeof(file_name), sizeof(file_name) - 1,
                "%s/vfmw_chan_%d.yuv", dbg_path, (td_s32)chan_id) < 0) {
            os_print("%s %d sec func err\n", __func__, __LINE__);
            goto out;
        }
        if (os_fopen == TD_NULL) {
            goto out;
        }
        chan->yuv = os_fopen(file_name, OS_RDWR | OS_CREATE | OS_TRUNC, OS_OPEN_MODE);
        if (chan->yuv == TD_NULL) {
            os_print("Open %s failed.\n", file_name);
        } else {
            os_print("start save chan %d yuv to %s.\n", chan_id, file_name);
        }
    } else {
        os_fclose(chan->yuv);
        chan->yuv = TD_NULL;
        os_print("end save chan %d yuv.\n", chan_id);
    }
out:
    os_sema_up(entry->dbg_sema);
}

STATIC td_void vfmw_proc_save_mtdt(cmd_str_ptr cmd_str, td_u32 count)
{
    td_u32 chan_id = 0;
    td_char file_name[FILE_NAME_MAX_LEN];
    vfmw_chan *chan = TD_NULL;
    td_char *dbg_path = dbg_get_dbg_path();

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((*cmd_str)[1], &chan_id);
    vfmw_check_print(chan_id < VFMW_CHAN_NUM, "chan_id %d out of range[0,%d).\n", chan_id, VFMW_CHAN_NUM);

    chan = vadpt_chan((td_s32)chan_id);
    vfmw_check_print(chan != TD_NULL, "chan %d invalid.\n", chan_id);

    os_sema_down(chan->sema);

    if (chan->mtdt == TD_NULL) {
        if (snprintf_s(file_name, sizeof(file_name), sizeof(file_name) - 1,
                "%s/vfmw_chan_%d.yuv", dbg_path, (td_s32)chan_id) < 0) {
            os_print("%s %d sec func err\n", __func__, __LINE__);
            goto out;
        }

        if (os_fopen == TD_NULL) {
            goto out;
        }

        chan->mtdt = os_fopen(file_name, OS_RDWR | OS_CREATE | OS_TRUNC, OS_OPEN_MODE);
        if (chan->mtdt == TD_NULL) {
            os_print("Open %s failed.\n", file_name);
        } else {
            os_print("start save chan %d metadata to %s.\n", chan_id, file_name);
        }
    } else {
        os_fclose(chan->mtdt);
        chan->mtdt = TD_NULL;
        os_print("end save chan %d metadata.\n", chan_id);
    }
out:
    os_sema_up(chan->sema);
}

STATIC td_void vfmw_proc_set_save_begin(cmd_str_ptr cmd_str, td_u32 count)
{
    td_u32 data = 0;
    vfmw_dflt *dflt = vadpt_get_dflt();

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((*cmd_str)[1], &data);

    dflt->save_begin = data;
    os_print("set save_begin = %d.\n", data);
}

STATIC td_void vfmw_proc_set_save_end(cmd_str_ptr cmd_str, td_u32 count)
{
    td_u32 data = 0;
    vfmw_dflt *dflt = vadpt_get_dflt();

    vfmw_check_print(count >= 0x2, "cmd param count %d invalid.\n", count);

    str_to_val((*cmd_str)[1], &data);

    dflt->save_end = data;
    os_print("set save_end = %d.\n", data);
}

STATIC td_void vfmw_proc_read_adpt_modules(td_void *buf, td_s32 len, td_s32 *used)
{
#ifdef VFMW_STREAM_SUPPORT
    stream_log_read(buf, len, used);
#endif

#ifdef VFMW_VPP_SUPPORT
    vpp_log_read(buf, len, used);
#endif

    return;
}

STATIC td_s32 vfmw_proc_write_local_modules(cmd_str_ptr buffer, td_u32 count)
{
    td_s32 ret;

    ret = EXT_FAILURE;
#ifdef VFMW_STREAM_SUPPORT
    ret = stream_log_write(buffer, count);
    if (ret == EXT_SUCCESS) {
        return EXT_SUCCESS;
    }
#endif

#ifdef VFMW_VPP_SUPPORT
    ret = vpp_log_write(buffer, count);
    if (ret == EXT_SUCCESS) {
        return EXT_SUCCESS;
    }
#endif

    return ret;
}

STATIC td_void vfmw_proc_set_vdh_clk(cmd_str_ptr buffer, td_u32 count)
{
    td_s32 ret;
    td_u32 data = 0;

    vfmw_check_print(count >= 2, "Cmd param count %d invalid.\n", count); /* 2 :a number */

    str_to_val((td_char *)((*buffer)[1]), &data); /* 1 means 2nd param */

    ret = pdt_set_attr(ATTR_SET_CLK, &data);
    if (ret == EXT_SUCCESS) {
        os_print("set vdh clk %d succ\n", data);
    } else {
        os_print("set vdh clk %d fail\n", data);
    }
}

STATIC td_s32 vfmw_proc_local_write(td_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], td_u32 argc)
{
    td_s32 ret = EXT_FAILURE;
    cmd_handler handler = TD_NULL;
    td_char *cmd_id = TD_NULL;
    td_u32 i, cmd_cnt;
    td_u32 count;
    td_char cmd_str[CMD_PARAM_MAX_COUNT][CMD_PARAM_MAX_LEN];

    STATIC const log_cmd s_adpt_cmd[] = {
        { LOG_CMD_HELP,           vfmw_proc_help },
        { LOG_CMD_PRINT,          vfmw_proc_set_print },
        { LOG_CMD_CUR_CHAN,       vfmw_proc_set_cur_chan },
        { LOG_CMD_SAVE_YUV,       vfmw_proc_save_yuv },
        { LOG_CMD_SAVE_MTDT,      vfmw_proc_save_mtdt },
        { LOG_CMD_SAVE_BEGIN,     vfmw_proc_set_save_begin },
        { LOG_CMD_SAVE_END,       vfmw_proc_set_save_end },
        { LOG_CMD_SET_CLK,        vfmw_proc_set_vdh_clk },
        { LOG_CMD_SAVE_VDH_REG,   vfmw_proc_save_reg },
        { LOG_CMD_SET_DBG_PATH,   vfmw_proc_set_dbg_path },
    };

    cmd_cnt = vfmw_proc_argv_to_cmd_str(argv, (td_s32)argc, &cmd_str);
    vfmw_check_ret(cmd_cnt > 0, EXT_FAILURE);
    vfmw_proc_sema_down();

    cmd_id = cmd_str[0];
    count = (td_u32)sizeof(s_adpt_cmd) / (td_u32)sizeof(log_cmd);
    vfmw_check_goto(os_strncmp != TD_NULL, out);
    vfmw_check_goto(os_strlen != TD_NULL, out);

    for (i = 0; i < count; i++) {
        if (os_strncmp(cmd_id, s_adpt_cmd[i].cmd_name, os_strlen(cmd_id)) == 0) {
            handler = s_adpt_cmd[i].handler;
            break;
        }
    }

    if (handler == TD_NULL) {
        ret = vfmw_proc_write_local_modules(&cmd_str, count);
    } else {
        handler(&cmd_str, cmd_cnt);
        ret = EXT_SUCCESS;
    }

out:
    vfmw_proc_sema_up();

    return ret;
}

STATIC td_s32 vfmw_proc_read(log_module module, td_void *buf)
{
    td_s32 ret;
    td_s32 chan_id;
    vfmw_chan *chan = TD_NULL;

    chan_id = vfmw_proc_get_entry()->curr_chan;
    chan = vadpt_chan(chan_id);
    vfmw_check_ret(chan != TD_NULL, EXT_FAILURE);

    vfmw_proc_sema_down();

#ifdef VFMW_LOCAL_SUPPORT
    ret = local_proc_read(module, buf, chan_id);
#endif

    vfmw_proc_sema_up();

    return ret;
}

STATIC td_s32 vfmw_proc_write(log_module module, td_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], td_u32 argc)
{
    td_u32 cmd_cnt;
    td_char cmd_str[CMD_PARAM_MAX_COUNT][CMD_PARAM_MAX_LEN];

    cmd_cnt = vfmw_proc_argv_to_cmd_str(argv, (td_s32)argc, &cmd_str);
    vfmw_check_ret(cmd_cnt > 0, EXT_FAILURE);

    vfmw_proc_sema_down();

#ifdef VFMW_LOCAL_SUPPORT
    (td_void)local_proc_write(module, &cmd_str, (td_s32)cmd_cnt);
#endif

    vfmw_proc_sema_up();

    return EXT_SUCCESS;
}

STATIC td_void vfmw_proc_read_chan_state(td_void *buf, td_s32 len, td_s32 *used)
{
    td_s32 chan_id;
    vfmw_type type;
    vfmw_chan_state chan_state;
    vfmw_chan *chan = TD_NULL;
    vfmw_entry *entry = vadpt_entry();
    td_char *type_str[VFMW_TYPE_BUFF + 1] = {"Local", "Tee", "Mdc", "User", "NA"};
    td_char *chan_state_str[VFMW_CHAN_STATE_BUTT + 1] = {"Null", "Wait", "Create", "NA"};

    for (chan_id = 0; chan_id < VFMW_CHAN_NUM; chan_id++) {
        os_sema_down(entry->chan[chan_id].sema);

        chan = vadpt_chan(chan_id);
        if (chan == TD_NULL || chan->state == VFMW_CHAN_STATE_NULL) {
            os_sema_up(entry->chan[chan_id].sema);
            continue;
        }

        type = (chan->type < VFMW_TYPE_BUFF) ? chan->type : VFMW_TYPE_BUFF;
        chan_state = (chan->state < VFMW_CHAN_STATE_BUTT) ? chan->state : VFMW_CHAN_STATE_BUTT;
        os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "chan_id", chan_id, "is_sec", chan->is_sec);
        os_dump_proc(buf, len, used, 0, LOG_FMT_S_S, "chan_state", chan_state_str[chan_state], "fmw_type",
                     type_str[type]);
        os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "start_cnt", chan->start_cnt, "stop_cnt", chan->stop_cnt);
        os_dump_proc(buf, len, used, 0, LOG_FMT_D_D, "flush_cnt", chan->flush_cnt, "reset_cnt", chan->reset_cnt);

        os_sema_up(entry->chan[chan_id].sema);
    }
}

STATIC td_s32 vfmw_proc_read_adpt(td_void *buf)
{
    vfmw_entry *vadpt_entry(td_void);
    vfmw_chan *vadpt_chan(td_s32 chan_id);
    vfmw_entry *entry = TD_NULL;
    const td_s32 len = 0;
    td_s32 used = 0;
    vfmw_state state;
    td_char *state_str[VFMW_STATE_BUTT + 1] = {"Null", "Open", "NA"};
    td_char *board_str[VFMW_BOARD_MAX + 1] = {"NA", "ES", "CS", "EC", "NA"};
    const vfmw_product_info *pdt_info = pdt_get_product_info();

    vfmw_check_ret(os_get_vir_mem_num != TD_NULL, EXT_FAILURE);
    vfmw_proc_sema_down();

    entry = vadpt_entry();

    os_dump_proc(buf, len, &used, 0, LOG_VFMW_COM_FMT_HEAD);
    os_dump_proc(buf, len, &used, 0, LOG_FMT_D_N, "vfmw_version", VFMW_VERSION, " ");

    state = (entry->state < VFMW_STATE_BUTT) ? entry->state : VFMW_STATE_BUTT;
    os_dump_proc(buf, len, &used, 0, LOG_FMT_X_S, "check1", entry->reserve1, "state", state_str[state]);
    os_dump_proc(buf, len, &used, 0, LOG_FMT_X_X, "check2", entry->reserve2, "print",
                 dbg_get_print_word());
    os_dump_proc(buf, len, &used, 0, LOG_FMT_D_D, "max_chan_num", VFMW_CHAN_NUM, "vir_num", os_get_vir_mem_num());
    os_dump_proc(buf, len, &used, 0, LOG_FMT_D_D, "vdh_clk", pdt_info->vdh_clk_max, "mdc_clk", pdt_info->mdc_clk_max);
    os_dump_proc(buf, len, &used, 0, LOG_FMT_S_D, "board", board_str[pdt_info->pdt_board],
        "vdh_lpow", pdt_info->vdh_lowpower);
    os_dump_proc(buf, len, &used, 0, LOG_FMT_D_D, "suspend_cnt", entry->suspend_cnt, "resume_cnt", entry->resume_cnt);
    os_dump_proc(buf, len, &used, 0, LOG_FMT_S_N, "dbg_path", dbg_get_dbg_path(), " ");

    vfmw_proc_read_chan_state(buf, len, &used);

    vfmw_proc_read_adpt_modules(buf, len, &used);

    os_read_osal(buf, len, &used);

    vfmw_proc_sema_up();

    return EXT_SUCCESS;
}

STATIC td_s32 vfmw_proc_read_adpt_kpi(td_void *buf)
{
    const td_s32 len = 0;
    td_s32 used = 0;
    td_s32 chan_id;
    vfmw_entry *glb = TD_NULL;
    vfmw_proc_entry *entry = TD_NULL;
    vfmw_chan *chan = TD_NULL;
    vfmw_kpi *kpi = TD_NULL;
#define KPI_FMT_H "%-12s %-12s %-12s %-33s| \r\n"
#define KPI_FMT_L "%-12s %-12u %-12u %-33u| \r\n"

    vfmw_proc_sema_down();

    glb = vadpt_entry();
    entry = vfmw_proc_get_entry();
    chan_id = entry->curr_chan;

    chan = vadpt_chan(entry->curr_chan);
    if (chan == TD_NULL) {
        vfmw_proc_sema_up();
        return EXT_FAILURE;
    }
    kpi = &chan->kpi;

    os_dump_proc(buf, len, &used, 0, "=================================vfmw_kpi=================================\r\n");
    os_dump_proc(buf, len, &used, 0, "---------------------------------chan_%02d----------------------------------\r\n",
        chan_id);
    os_dump_proc(buf, len, &used, 0, KPI_FMT_H, "state", "cost(us)", "begin(us)", "end(us)");
    os_dump_proc(buf, len, &used, 0, KPI_FMT_L, "init", glb->init_kpi.cost, glb->init_kpi.begin,
                 glb->init_kpi.end);
    os_dump_proc(buf, len, &used, 0, KPI_FMT_L, "create", kpi->create.cost, kpi->create.begin, kpi->create.end);
    os_dump_proc(buf, len, &used, 0, KPI_FMT_L, "config", kpi->config.cost, kpi->config.begin, kpi->config.end);
    os_dump_proc(buf, len, &used, 0, KPI_FMT_L, "start", kpi->start.cost, kpi->start.begin, kpi->start.end);
    os_dump_proc(buf, len, &used, 0, KPI_FMT_L, "questm", kpi->questm.cost, kpi->questm.begin, kpi->questm.end);
    os_dump_proc(buf, len, &used, 0, KPI_FMT_L, "getfrm", kpi->getfrm.cost, kpi->getfrm.begin, kpi->getfrm.end);
    os_dump_proc(buf, len, &used, 0, KPI_FMT_L, "stop", kpi->stop.cost, kpi->stop.begin, kpi->stop.end);
    os_dump_proc(buf, len, &used, 0, KPI_FMT_L, "reset", kpi->reset.cost, kpi->reset.begin, kpi->reset.end);
    os_dump_proc(buf, len, &used, 0, KPI_FMT_L, "destroy", kpi->destroy.cost, kpi->destroy.begin,
                 kpi->destroy.end);
    os_dump_proc(buf, len, &used, 0, KPI_FMT_L, "exit", glb->exit_kpi.cost, glb->exit_kpi.begin,
                 glb->exit_kpi.end);
    os_dump_proc(buf, len, &used, 0, KPI_FMT_L, "suspend", glb->susp_kpi.cost, glb->susp_kpi.begin,
                 glb->susp_kpi.end);
    os_dump_proc(buf, len, &used, 0, KPI_FMT_L, "resume", glb->resu_kpi.cost, glb->resu_kpi.begin,
                 glb->resu_kpi.end);
    dbg_show_time(buf);
    os_dump_proc(buf, len, &used, 0, "=========================================================================\r\n");

    vfmw_proc_sema_up();

    return EXT_SUCCESS;
}

STATIC td_s32 vfmw_proc_read_stm(struct osal_proc_dir_entry *entry)
{
    return vfmw_proc_read(LOG_MODULE_STM, entry->seqfile);
}

STATIC td_s32 vfmw_proc_write_stm(td_u32 argc, td_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], td_void *private)
{
    vfmw_unused(private);
    return vfmw_proc_write(LOG_MODULE_STM, argv, argc);
}

STATIC td_s32 vfmw_proc_read_syn(struct osal_proc_dir_entry *entry)
{
    return vfmw_proc_read(LOG_MODULE_SYN, entry->seqfile);
}

STATIC td_s32 vfmw_proc_write_syn(td_u32 argc, td_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], td_void *private)
{
    vfmw_unused(private);
    return vfmw_proc_write(LOG_MODULE_SYN, argv, argc);
}

STATIC td_s32 vfmw_proc_read_dec(struct osal_proc_dir_entry *entry)
{
    return vfmw_proc_read(LOG_MODULE_DEC, entry->seqfile);
}

STATIC td_s32 vfmw_proc_write_dec(td_u32 argc, td_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], td_void *private)
{
    vfmw_unused(private);
    return vfmw_proc_write(LOG_MODULE_DEC, argv, argc);
}

STATIC td_s32 vfmw_proc_read_com(struct osal_proc_dir_entry *entry)
{
    if (vfmw_proc_read_adpt(entry->seqfile) == (td_s32)EXT_FAILURE) {
        os_print("%s read proc adpt failure!\n", __func__);
    }

    return vfmw_proc_read(LOG_MODULE_COM, entry->seqfile);
}

STATIC td_s32 vfmw_proc_write_com(td_u32 argc, td_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], td_void *private)
{
    vfmw_unused(private);
    if (vfmw_proc_local_write(argv, argc) == (td_s32)EXT_SUCCESS) {
        return EXT_SUCCESS;
    }

    return vfmw_proc_write(LOG_MODULE_COM, argv, argc);
}

STATIC td_s32 vfmw_proc_read_fsp(struct osal_proc_dir_entry *entry)
{
    return vfmw_proc_read(LOG_MODULE_FSP, entry->seqfile);
}

#ifdef VDEC_DEBUG_SUPPORT
STATIC td_s32 vfmw_proc_write_fsp(td_u32 argc, td_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], td_void *private)
{
    vfmw_unused(private);
    return vfmw_proc_write(LOG_MODULE_FSP, argv, argc);
}
#endif

STATIC td_s32 vfmw_proc_read_kpi(struct osal_proc_dir_entry *entry)
{
    return vfmw_proc_read_adpt_kpi(entry->seqfile);
}

STATIC td_s32 vfmw_proc_read_pts(struct osal_proc_dir_entry *entry)
{
    td_s32 ret;
    td_s32 chan_id;

    vfmw_proc_sema_down();

    chan_id = vfmw_proc_get_entry()->curr_chan;

    ret = pts_info_read(entry->seqfile, chan_id);

    vfmw_proc_sema_up();

    return ret;
}

STATIC td_s32 vfmw_proc_read_psram(struct osal_proc_dir_entry *entry)
{
    td_s32 ret;
    vfmw_unused(entry);

    ret = liteos_mem_proc(entry->seqfile);

    return ret;
}

STATIC td_s32 vfmw_proc_read_usr(struct osal_proc_dir_entry *entry)
{
    td_s32 ret;
    vfmw_unused(entry);

    ret = user_info_read(entry->seqfile);

    return ret;
}

STATIC td_s32 vfmw_proc_write_pts(td_u32 argc, td_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], td_void *private)
{
    td_u32 cmd_cnt;
    td_char cmd_str[CMD_PARAM_MAX_COUNT][CMD_PARAM_MAX_LEN];

    vfmw_unused(private);

    cmd_cnt = vfmw_proc_argv_to_cmd_str(argv, (td_s32)argc, &cmd_str);
    vfmw_check_ret(cmd_cnt > 0, EXT_FAILURE);

    vfmw_proc_sema_down();

    pts_info_write(&cmd_str, cmd_cnt);

    vfmw_proc_sema_up();

    return EXT_SUCCESS;
}

STATIC td_s32 vfmw_proc_write_usr(td_u32 argc, td_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], td_void *private)
{
    td_u32 cmd_cnt;
    td_char cmd_str[CMD_PARAM_MAX_COUNT][CMD_PARAM_MAX_LEN];

    vfmw_unused(private);

    cmd_cnt = vfmw_proc_argv_to_cmd_str(argv, (td_s32)argc, &cmd_str);
    vfmw_check_ret(cmd_cnt > 0, EXT_FAILURE);

    vfmw_proc_sema_down();

    user_info_write(&cmd_str, cmd_cnt);

    vfmw_proc_sema_up();

    return EXT_SUCCESS;
}

#ifdef VFMW_HDR_SUPPORT
STATIC td_s32 vfmw_proc_read_hdr(PROC_FILE *buf, td_void *private)
{
    td_s32 ret;
    td_s32 chan_id;

    vfmw_unused(private);

    vfmw_proc_sema_down();

    chan_id = vfmw_proc_get_entry()->curr_chan;
    ret = hdr_info_read(buf, chan_id);

    vfmw_proc_sema_up();

    return ret;
}

STATIC td_s32 vfmw_proc_write_hdr(td_u32 argc, td_char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], td_void *private)
{
    td_u32 cmd_cnt;
    td_s8 cmd_str[CMD_PARAM_MAX_COUNT][CMD_PARAM_MAX_LEN];

    vfmw_unused(private);

    cmd_cnt = vfmw_proc_argv_to_cmd_str(argv, argc, &cmd_str);
    vfmw_check_ret(cmd_cnt > 0, EXT_FAILURE);

    vfmw_proc_sema_down();

    hdr_info_write(&cmd_str, cmd_cnt);

    vfmw_proc_sema_up();

    return EXT_SUCCESS;
}
#endif

STATIC OS_PROC_CMD g_vfmw_com_list[] = {
    {LOG_CMD_HELP, vfmw_proc_write_com},
    {LOG_CMD_PRINT, vfmw_proc_write_com},
    {LOG_CMD_CUR_CHAN, vfmw_proc_write_com},
    {LOG_CMD_DPS_SER, vfmw_proc_write_com},
    {LOG_CMD_DPS_NUM, vfmw_proc_write_com},
    {LOG_CMD_ERR_HOLD, vfmw_proc_write_com},
    {LOG_CMD_OUT_ORDER, vfmw_proc_write_com},
    {LOG_CMD_DEC_MODE, vfmw_proc_write_com},
    {LOG_CMD_VPP_BYPASS, vfmw_proc_write_com},
#if defined(VDEC_DEBUG_SUPPORT)
    {LOG_CMD_SAVE_YUV, vfmw_proc_write_com},
    {LOG_CMD_SAVE_MTDT, vfmw_proc_write_com},
    {LOG_CMD_SAVE_RAW, vfmw_proc_write_com},
    {LOG_CMD_CMP_MODE, vfmw_proc_write_com},
    {LOG_CMD_TUNL_LINE, vfmw_proc_write_com},
    {LOG_CMD_MOD_LOW_DLY, vfmw_proc_write_com},
    {LOG_CMD_SAVE_BEGIN, vfmw_proc_write_com},
    {LOG_CMD_SAVE_END, vfmw_proc_write_com},
    {LOG_CMD_SET_CLK, vfmw_proc_write_com},
    {LOG_CMD_LINEAR_NO_GAP, vfmw_proc_write_com},
    {LOG_CMD_SHOW_DFLT, vfmw_proc_write_com},
    {LOG_CMD_PRT_STM_BLK, vfmw_proc_write_com},
    {LOG_CMD_VPP_PR_FRM, vfmw_proc_write_com},
    {LOG_CMD_VPP_PR_BUF, vfmw_proc_write_com},
    {LOG_CMD_VPP_NO_RECV, vfmw_proc_write_com},
    {LOG_CMD_DPS_NO_RECV, vfmw_proc_write_com},
    {LOG_CMD_FRM_LAYOUT, vfmw_proc_write_com},
    {LOG_CMD_SAVE_VDH_REG, vfmw_proc_write_com},
    {LOG_CMD_SET_DBG_PATH, vfmw_proc_write_com},
#endif
};

STATIC OS_PROC_CMD g_vfmw_stm_list[] = {
    {LOG_CMD_STM_SLEEP, vfmw_proc_write_stm},
    {LOG_CMD_STM_WAIT, vfmw_proc_write_stm},
    {LOG_CMD_STM_BYPASS, vfmw_proc_write_stm},
#if defined(VDEC_DEBUG_SUPPORT)
    {LOG_CMD_SAVE_SEEK_RAW, vfmw_proc_write_stm},
    {LOG_CMD_SAVE_SEG, vfmw_proc_write_stm},
    {LOG_CMD_STM_NO_RAW, vfmw_proc_write_stm},
    {LOG_CMD_STM_NO_SEG, vfmw_proc_write_stm},
    {LOG_CMD_STM_SEG_FULL, vfmw_proc_write_stm},
#endif
};

STATIC OS_PROC_CMD g_vfmw_syn_list[] = {
    {LOG_CMD_SYN_SLEEP, vfmw_proc_write_syn},
    {LOG_CMD_SYN_WAIT, vfmw_proc_write_syn},
    {LOG_CMD_SYN_BYPASS, vfmw_proc_write_syn},
#if defined(VDEC_DEBUG_SUPPORT)
    {LOG_CMD_NO_RECV_SEG, vfmw_proc_write_syn},
#endif
};

STATIC OS_PROC_CMD g_vfmw_dec_list[] = {
    {LOG_CMD_DEC_SLEEP, vfmw_proc_write_dec},
    {LOG_CMD_DEC_WAIT, vfmw_proc_write_dec},
    {LOG_CMD_DEC_BYPASS, vfmw_proc_write_dec},
#if defined(VDEC_DEBUG_SUPPORT)
    {LOG_CMD_DEC_CHECK, vfmw_proc_write_dec},
#endif
};

STATIC OS_PROC_CMD g_vfmw_fsp_list[] = {
#if defined(VDEC_DEBUG_SUPPORT)
    {LOG_CMD_FSP_NO_DEC, vfmw_proc_write_fsp},
    {LOG_CMD_FSP_NO_DSP, vfmw_proc_write_fsp},
    {LOG_CMD_FSP_NO_PMV, vfmw_proc_write_fsp},
    {LOG_CMD_FSP_NO_ATT, vfmw_proc_write_fsp},
#endif
};

STATIC OS_PROC_CMD g_vfmw_pts_list[] = {
    {LOG_CMD_SAMPLE_NUM, vfmw_proc_write_pts},
};

STATIC OS_PROC_CMD g_vfmw_usr_list[] = {
    {LOG_CMD_USR_DEC_NUM, vfmw_proc_write_usr},
};

#ifdef VFMW_HDR_SUPPORT
STATIC OS_PROC_CMD g_vfmw_hdr_list[] = {
    {LOG_CMD_READ_HDR, vfmw_proc_write_hdr},
};
#endif

td_s32 vfmw_adpt_proc_create(td_void)
{
    td_s32 ret;
    td_u32 cmd_cnt;
    vfmw_proc_entry *entry = vfmw_proc_get_entry();

    vfmw_check_ret(os_proc_create != TD_NULL, EXT_FAILURE);

    cmd_cnt = (td_u32)sizeof(g_vfmw_com_list) / (td_u32)sizeof(OS_PROC_CMD);
    ret = os_proc_create(PROC_VFMW_COM, vfmw_proc_read_com, g_vfmw_com_list, cmd_cnt);
    vfmw_check_goto_print(ret == 0, err_exit, "create proc %s failed!\n", PROC_VFMW_COM);

    cmd_cnt = (td_u32)sizeof(g_vfmw_stm_list) / (td_u32)sizeof(OS_PROC_CMD);
    ret = os_proc_create(PROC_VFMW_STM, vfmw_proc_read_stm, g_vfmw_stm_list, cmd_cnt);
    vfmw_check_goto_print(ret == 0, err_exit, "create proc %s failed!\n", PROC_VFMW_STM);

    cmd_cnt = (td_u32)sizeof(g_vfmw_syn_list) / (td_u32)sizeof(OS_PROC_CMD);
    ret = os_proc_create(PROC_VFMW_SYN, vfmw_proc_read_syn, g_vfmw_syn_list, cmd_cnt);
    vfmw_check_goto_print(ret == 0, err_exit, "create proc %s failed!\n", PROC_VFMW_SYN);

    cmd_cnt = (td_u32)sizeof(g_vfmw_dec_list) / (td_u32)sizeof(OS_PROC_CMD);
    ret = os_proc_create(PROC_VFMW_DEC, vfmw_proc_read_dec, g_vfmw_dec_list, cmd_cnt);
    vfmw_check_goto_print(ret == 0, err_exit, "create proc %s failed!\n", PROC_VFMW_DEC);

    cmd_cnt = (td_u32)sizeof(g_vfmw_fsp_list) / (td_u32)sizeof(OS_PROC_CMD);
    ret = os_proc_create(PROC_VFMW_FSP, vfmw_proc_read_fsp, g_vfmw_fsp_list, cmd_cnt);
    vfmw_check_goto_print(ret == 0, err_exit, "create proc %s failed!\n", PROC_VFMW_FSP);

    cmd_cnt = (td_u32)sizeof(g_vfmw_pts_list) / (td_u32)sizeof(OS_PROC_CMD);
    ret = os_proc_create(PROC_VFMW_PTS, vfmw_proc_read_pts, g_vfmw_pts_list, cmd_cnt);
    vfmw_check_goto_print(ret == 0, err_exit, "create proc %s failed!\n", PROC_VFMW_PTS);

#ifdef VFMW_USER_SUPPORT
    cmd_cnt = (td_u32)sizeof(g_vfmw_usr_list) / (td_u32)sizeof(OS_PROC_CMD);
    ret = os_proc_create(PROC_VFMW_USR, vfmw_proc_read_usr, g_vfmw_usr_list, cmd_cnt);
    vfmw_check_goto_print(ret == 0, err_exit, "create proc %s failed!\n", PROC_VFMW_USR);
#endif

#ifdef VFMW_HDR_SUPPORT
    cmd_cnt = (td_u32)sizeof(g_vfmw_hdr_list) / (td_u32)sizeof(OS_PROC_CMD);
    ret = os_proc_create(PROC_VFMW_HDR, vfmw_proc_read_hdr, g_vfmw_hdr_list, cmd_cnt);
    vfmw_check_goto_print(ret == 0, err_exit, "create proc %s failed!\n", PROC_VFMW_HDR);
#endif

    ret = os_proc_create(PROC_VFMW_KPI, vfmw_proc_read_kpi, TD_NULL, 0);
    vfmw_check_goto_print(ret == 0, err_exit, "create proc %s failed!\n", PROC_VFMW_KPI);

    ret = os_proc_create(PROC_VFMW_MEM, vfmw_proc_read_psram, TD_NULL, 0);
    vfmw_check_goto_print(ret == 0, err_exit, "create proc %s failed!\n", PROC_VFMW_MEM);

    os_sema_init(&entry->proc_sema);

    return EXT_SUCCESS;

err_exit:
    vfmw_adpt_proc_destroy();

    return EXT_FAILURE;
}

td_void vfmw_adpt_proc_destroy(td_void)
{
    vfmw_proc_entry *entry = vfmw_proc_get_entry();

    os_proc_destroy(PROC_VFMW_COM);
    os_proc_destroy(PROC_VFMW_STM);
    os_proc_destroy(PROC_VFMW_SYN);
    os_proc_destroy(PROC_VFMW_DEC);
    os_proc_destroy(PROC_VFMW_FSP);
    os_proc_destroy(PROC_VFMW_KPI);
    os_proc_destroy(PROC_VFMW_PTS);
    os_proc_destroy(PROC_VFMW_MEM);
#ifdef VFMW_USER_SUPPORT
    os_proc_destroy(PROC_VFMW_USR);
#endif
#ifdef VFMW_HDR_SUPPORT
    os_proc_destroy(PROC_VFMW_HDR);
#endif

    os_sema_exit(entry->proc_sema);
}
