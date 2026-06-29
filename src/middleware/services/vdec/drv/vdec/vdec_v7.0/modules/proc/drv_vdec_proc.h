/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2019-04-22
 */

#ifndef __DRV_VDEC_PROC_H__
#define __DRV_VDEC_PROC_H__

#include "drv_ioctl_vdec.h"
#include "soc_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define VP_RET_UNSUPPORT (-2)

#define VP_HEAD  "--------------------------------------- %-3s --------------------------------------\r\n"
#define VP_VDEC_HEAD       "=====================================vdec=======================================\r\n"
#define VP_DRV_HEAD        "-------------------------------------drv----------------------------------------\r\n"
#define VP_VDEC_INFO_HEAD  "----------------------------------vdec_info-------------------------------------\r\n"
#define VP_HEIF_HEAD       "-------------------------------------heif---------------------------------------\r\n"
#define VP_VFMW_HEAD       "-------------------------------------vfmw---------------------------------------\r\n"
#define VP_KPI_HEAD        "-------------------------------------kpi----------------------------------------\r\n"
#define VP_VDEC_TAIL       "================================================================================\r\n"
#define VP_D     "  %-15s: %-20d\r\n"
#define VP_X     "  %-15s: 0x%-19x\r\n"
#define VP_P     "  %-15s: %-20p\r\n"
#define VP_S     "  %-15s: %-20s\r\n"
#define VP_L     "  %-15s: %-20lld\r\n"
#define VP_I_D   "  %-15s: %d.%d\r\n"
#define VP_I_D_D "  %-15s: %d/%d\r\n"
#define VP_D_N   "%-19s:%-19d|%-39s|\r\n"
#define VP_D_D   "%-19s:%-19d|%-19s:%-19d|\r\n"
#define VP_X_X   "%-19s:0x%-17x|%-19s:0x%-17x|\r\n"
#define VP_P_P   "%-19s:%-19p|%-19s:%-19p|\r\n"
#define VP_S_S   "%-19s:%-19s|%-19s:%-19s|\r\n"
#define VP_U_U   "%-19s:%-19u|%-19s:%-19u|\r\n"
#define VP_D_X   "%-19s:%-19d|%-19s:%-19x|\r\n"
#define VP_D_P   "%-19s:%-19d|%-19s:%-19p|\r\n"
#define VP_D_S   "%-19s:%-19d|%-19s:%-19s|\r\n"
#define VP_D_U   "%-19s:%-19d|%-19s:%-19u|\r\n"
#define VP_X_D   "%-19s:0x%-17x|%-19s:%-19d|\r\n"
#define VP_X_P   "%-19s:0x%-17x|%-19s:%-19p|\r\n"
#define VP_X_S   "%-19s:0x%-17x|%-19s:%-19s|\r\n"
#define VP_X_U   "%-19s:0x%-17x|%-19s:%-19u|\r\n"
#define VP_P_D   "%-19s:%-19p|%-19s:%-19d|\r\n"
#define VP_P_X   "%-19s:%-19p|%-19s:0x%-17x|\r\n"
#define VP_P_S   "%-19s:%-19p|%-19s:%-19s|\r\n"
#define VP_P_U   "%-19s:%-19p|%-19s:%-19u|\r\n"
#define VP_S_D   "%-19s:%-19s|%-19s:%-19d|\r\n"
#define VP_S_X   "%-19s:%-19s|%-19s:0x%-17x|\r\n"
#define VP_S_P   "%-19s:%-19s|%-19s:%-19p|\r\n"
#define VP_S_U   "%-19s:%-19s|%-19s:%-19u|\r\n"
#define VP_S_N   "%-19s:%-19s|%-39s|\r\n"
#define VP_U_D   "%-19s:%-19u|%-19s:%-19d|\r\n"
#define VP_U_X   "%-19s:%-19u|%-19s:0x%-17x|\r\n"
#define VP_U_P   "%-19s:%-19u|%-19s:%-19p|\r\n"
#define VP_U_S   "%-19s:%-19u|%-19s:%-19s|\r\n"
#define VP_D_F   "%-19s:%-19d|%-19s:%-10d.%-8d%|\r\n"

#define VDEC_PROC_NAME "vdec"
#define VDEC_CMD_NUM   2
#define VDEC_CMD_LEN   128
#define VDEC_HELP_FMT_H  "echo %-17s %-7s %-7s %s\r\n"
#define VDEC_HELP_FMT_L  "echo %-17s %-7s %-7s %-40s %s\r\n"

#define VDEC_PROC_CMD_SET_CHAN        "set_chan"
#define VDEC_PROC_CMD_SET_BYPASS      "bypass"
#define VDEC_PROC_CMD_SET_CYCLE       "cycle"
#define VDEC_PROC_CMD_SET_MOCK        "mock"
#define VDEC_PROC_CMD_HELP            "help"

typedef td_char (*cmd_str)[VDEC_CMD_NUM][VDEC_CMD_LEN];

#define VDEC_PROC_READ_BUF_SIZE (10 * 1024)

typedef td_s32 (*fn_vdec_proc_read)(struct osal_proc_dir_entry *entry);

td_s32 vdec_proc_read(td_void *buf, td_u32 len, td_u32 *used);
td_s32 vdec_proc_get_cmd(td_void **cmd_list, td_u32 *cmd_cnt);
td_void vdec_drv_proc_dump(td_void *page, td_u32 page_count, td_u32 *used_bytes, const td_char *format, ...);

td_s32 vdec_drv_create_proc(td_char *name, fn_vdec_proc_read read_fn, osal_proc_cmd *cmd_list, td_u32 cmd_cnt);
td_void vdec_drv_destroy_proc(td_char *name);

td_s32 vdec_drv_proc_read(td_s32 argc, const td_char *argv[]);
td_s32 vdec_drv_proc_write(td_s32 argc, const td_char *argv[]);

td_u32 vdec_drv_proc_get_bypass(td_s32 chan_id);
td_u32 vdec_drv_proc_get_cycle_mode(td_s32 chan_id);
td_u32 vdec_drv_proc_get_low_power_test_mode(td_s32 chan_id);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
