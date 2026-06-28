/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __VFMW_PROC__
#define __VFMW_PROC__

#include "vfmw.h"

#define FILE_NAME_MAX_LEN 100

#define CMD_PARAM_MAX_COUNT 5
#define CMD_PARAM_MAX_LEN   20

#define CMD_STR_MAX_LEN 128

#define LOG_MAX_WAIT_TIME  200  /*  ms  */
#define LOG_MAX_SLEEP_TIME 2000 /*  ms  */

#define LOG_FMT_HEAD               "-----------------------------%-3s-----------------------------\r\n"
#define LOG_VFMW_COM_FMT_HEAD      "=================================vfmw_com=====================================\r\n"
#define LOG_STREAM_FMT_HEAD        "----------------------------------stream--------------------------------------\r\n"
#define LOG_STREAM_BLOCK_FMT_HEAD  "-------------------------------stream_block-----------------------------------\r\n"
#define LOG_VPP_FMT_HEAD           "------------------------------------vpp---------------------------------------\r\n"
#define LOG_USDPOOL_FMT_HEAD       "-----------------------------------usdpool------------------------------------\r\n"
#define LOG_USDPOOL_BLK_FMT_HEAD   "--------------------------------usdpool_block---------------------------------\r\n"
#define LOG_FAULT_FMT_HEAD         "------------------------------------fault-------------------------------------\r\n"
#define LOG_FENCE_FMT_HEAD         "------------------------------------fence-------------------------------------\r\n"
#define LOG_USER_DEC_FMT_HEAD      "----------------------------------user_dec------------------------------------\r\n"
#define LOG_USER_DEC_FRM_HEAD      "----------------------------------user_frm------------------------------------\r\n"
#define LOG_COM_FMT_HEAD           "-------------------------------------com--------------------------------------\r\n"
#define LOG_DPS_FMT_HEAD           "-------------------------------------dps--------------------------------------\r\n"
#define LOG_DPS_LIST_FMT_HEAD      "----------------------------------dps_list------------------------------------\r\n"
#define LOG_VOQ_FMT_HEAD           "-------------------------------------voq--------------------------------------\r\n"
#define LOG_IPT_FMT_HEAD           "-------------------------------------ipt--------------------------------------\r\n"
#define LOG_FMT_TAIL               "==============================================================================\r\n"
#define LOG_FSP_FMT_HEAD           "==================================vfmw_fsp====================================\r\n"
#define LOG_FLN_FMT_HEAD           "--------------------------------fsp_logic_num---------------------------------\r\n"
#define LOG_FLUI_FMT_HEAD          "-------------------------------fsp_logic_used_info----------------------------\r\n"
#define LOG_FSP_DEC_NUM_FMT_HEAD   "---------------------------------fsp_dec_num----------------------------------\r\n"
#define LOG_FSP_DEC_INFO_FMT_HEAD  "---------------------------------fsp_dec_info---------------------------------\r\n"
#define LOG_FSP_DISP_NUM_FMT_HEAD  "---------------------------------fsp_disp_num---------------------------------\r\n"
#define LOG_FSP_DISP_INFO_FMT_HEAD "---------------------------------fsp_disp_info--------------------------------\r\n"
#define LOG_FSP_PMV_NUM_FMT_HEAD   "---------------------------------fsp_pmv_num----------------------------------\r\n"
#define LOG_FSP_PMV_INFO_FMT_HEAD  "---------------------------------fsp_pmv_info---------------------------------\r\n"
#define LOG_FSP_NOR_NUM_FMT_HEAD   "---------------------------------fsp_nor_num----------------------------------\r\n"
#define LOG_FSP_NOR_INFO_FMT_HEAD  "---------------------------------fsp_nor_info---------------------------------\r\n"
#define LOG_VFMW_STM_FMT_HEAD      "==================================vfmw_stm====================================\r\n"
#define LOG_DEV_FMT_HEAD           "-------------------------------------dev--------------------------------------\r\n"
#define LOG_CHN_FMT_HEAD           "-------------------------------------chn--------------------------------------\r\n"
#define LOG_RAW_FMT_HEAD           "-------------------------------------raw--------------------------------------\r\n"
#define LOG_SEG_FMT_HEAD           "-------------------------------------seg--------------------------------------\r\n"
#define LOG_VFMW_SYN_FMT_HEAD      "==================================vfmw_syn====================================\r\n"
#define LOG_PCTI_FMT_HEAD          "-----------------------------parse_cost_time_info-----------------------------\r\n"
#define LOG_PCMT_FMT_HEAD          "------------------------------parse_cost_maxtime------------------------------\r\n"
#define LOG_STD_FMT_HEAD           "-------------------------------------std--------------------------------------\r\n"
#define LOG_DEC_FMT_HEAD           "==================================vfmw_dec====================================\r\n"
#define LOG_CHN_POOL_FMT_HEAD      "----------------------------------chn_pool------------------------------------\r\n"
#define LOG_DEV_X_FMT_HEAD         "------------------------------------dev_%d------------------------------- ----\r\n"
#define LOG_DTL_FMT_HEAD           "--------------------------------dec_task_list---------------------------------\r\n"
#define LOG_CTL_FMT_HEAD           "--------------------------------cfg_task_list---------------------------------\r\n"
#define LOG_HDR_FMT_HEAD           "===================================vfmw_hdr===================================\r\n"
#define LOG_HDR_IP_INFO_FMT_HEAD   "--------------------------------hdr_input_info--------------------------------\r\n"
#define LOG_HDR_OP_INFO_FMT_HEAD   "--------------------------------hdr_output_info-------------------------------\r\n"
#define LOG_DYN_META_SRT_FMT_HEAD  "-----------------------------dynamic_metadata_start---------------------------\r\n"
#define LOG_VMM_MEMNODE_INFON_HEAD "-------------------------------------- memnode infon ---------------------------" \
                                   "------------- \r\n"
#define LOG_VMM_MEMNODE_INFO_HEAD  "-------------------------------------------------------------memnode_info-------" \
                                   "-------------------------------------------------------------\r\n"
#define LOG_VMM_MEM_STATUS_HEAD    "--------------------------------mem_list_status-------------------------------\r\n"
#define LOG_VMM_FMT_TAIL           "================================================================================" \
                                   "=======================================================\r\n"
#define LOG_CS_HEAD                "====================================vfmw_cs===================================\r\n"
#define LOG_CS_SHR_EVT_HEAD        "------------------------------------shr_evt-----------------------------------\r\n"
#define LOG_CS_SHR_IMG_HEAD        "------------------------------------shr_img-----------------------------------\r\n"
#define LOG_CS_VALID_IMAGE_HEAD    "----------------------------------valid_image---------------------------------\r\n"
#define LOG_CS_VALID_IMAGE2_HEAD   "----------------------------------valid_image2--------------------------------\r\n"
#define LOG_CS_SHR_STM_HEAD        "------------------------------------shr_stm-----------------------------------\r\n"
#define LOG_MDC_CLI_FMT_HEAD       "-----------------------------------mdc_client---------------------------------\r\n"
#define LOG_MDC_CLI_ADA_FMT_HEAD   "--------------------------------mdc_client_adapt------------------------------\r\n"
#define LOG_MDC_DEV_HEAD           "------------------------------------mdc_dev-----------------------------------\r\n"
#define LOG_MCC_SEND_MSG_FMT_HEAD  "------------------------------mdc_clt_com_send_msg----------------------------\r\n"
#define LOG_MCC_RECV_MSG_FMT_HEAD  "------------------------------mdc_clt_com_recv_msg----------------------------\r\n"
#define LOG_MDC_SERVER_FMT_HEAD    "-----------------------------------mdc_server---------------------------------\r\n"
#define LOG_IRQ_MSG_HEAD           "------------------------------------irq_msg-----------------------------------\r\n"
#define LOG_TASK_MSG_HEAD          "------------------------------------task_msg----------------------------------\r\n"
#define LOG_MDC_SVR_CHAN_HEAD      "----------------------------------mdc_svr_chan--------------------------------\r\n"
#define LOG_MSC_RECV_MSG_HEAD      "------------------------------mdc_svr_com_recv_msg----------------------------\r\n"
#define LOG_MSC_SEND_MSG_HEAD      "------------------------------mdc_svr_com_send_msg----------------------------\r\n"
#define LOG_PTS_FMT_HEAD           "=================vfmw_pts===============\r\n"
#define LOG_PTS_FMT_TAIL           "========================================\r\n"
#define LOG_MEM_FMT_HEAD           "=====================================vfmw_mem==================================\r\n"
#define LOG_MEM_FMT_TAIL           "===============================================================================\r\n"
#define LOG_VFMW_HELP_HEAD         "=========================================vfmw_help=============================\r\n"
#define LOG_VFMW_HELP_TAIL         "===============================================================================\r\n"

#define LOG_FMT_D     "%-19s:%-19d|\r\n"
#define LOG_FMT_X     "%-19s:0x%-17x|\r\n"
#define LOG_FMT_P     "%-19s:%-19p|\r\n"
#define LOG_FMT_S     "%-19s:%-19s|\r\n"
#define LOG_FMT_L     "%-19s:%-19lld|\r\n"
#define LOG_FMT_I_D   "%-19s:%03d.%-15d|\r\n"
#define LOG_FMT_I_D_D "%-15s:%d/%d|\r\n"
#define LOG_FMT_D_N   "%-19s:%-19d|%-39s|\r\n"
#define LOG_FMT_D_D   "%-19s:%-19d|%-19s:%-19d|\r\n"
#define LOG_FMT_X_X   "%-19s:0x%-17x|%-19s:0x%-17x|\r\n"
#define LOG_FMT_X_N   "%-19s:0x%-17x|%-39s|\r\n"
#define LOG_FMT_LX_N  "%-19s:0x%-17llx|%-39s|\r\n"
#define LOG_FMT_P_P   "%-19s:%-19p|%-19s:%-19p|\r\n"
#define LOG_FMT_S_S   "%-19s:%-19s|%-19s:%-19s|\r\n"
#define LOG_FMT_U_U   "%-19s:%-19u|%-19s:%-19u|\r\n"
#define LOG_FMT_D_X   "%-19s:%-19d|%-19s:0x%-17x|\r\n"
#define LOG_FMT_D_LX  "%-19s:%-19d|%-19s:0x%-17llx|\r\n"
#define LOG_FMT_D_P   "%-19s:%-19d|%-19s:%-19p|\r\n"
#define LOG_FMT_D_S   "%-19s:%-19d|%-19s:%-19s|\r\n"
#define LOG_FMT_D_U   "%-19s:%-19d|%-19s:%-19u|\r\n"
#define LOG_FMT_X_D   "%-19s:0x%-17x|%-19s:%-19d|\r\n"
#define LOG_FMT_LX_D  "%-19s:0x%-17llx|%-19s:%-19d|\r\n"
#define LOG_FMT_X_P   "%-19s:0x%-17x|%-19s:%-19p|\r\n"
#define LOG_FMT_X_S   "%-19s:0x%-17x|%-19s:%-19s|\r\n"
#define LOG_FMT_LX_S  "%-19s:0x%-17llx|%-19s:%-19s|\r\n"
#define LOG_FMT_X_U   "%-19s:0x%-17x|%-19s:%-19u|\r\n"
#define LOG_FMT_P_D   "%-19s:%-19p|%-19s:%-19d|\r\n"
#define LOG_FMT_P_X   "%-19s:%-19p|%-19s:0x%-17x|\r\n"
#define LOG_FMT_P_S   "%-19s:%-19p|%-19s:%-19s|\r\n"
#define LOG_FMT_P_U   "%-19s:%-19p|%-19s:%-19u|\r\n"
#define LOG_FMT_S_N   "%-19s:%-19s|%-39s|\r\n"
#define LOG_FMT_S_D   "%-19s:%-19s|%-19s:%-19d|\r\n"
#define LOG_FMT_S_X   "%-19s:%-19s|%-19s:0x%-17x|\r\n"
#define LOG_FMT_S_P   "%-19s:%-19s|%-19s:%-19p|\r\n"
#define LOG_FMT_S_U   "%-19s:%-19s|%-19s:%-19u|\r\n"
#define LOG_FMT_U_D   "%-19s:%-19u|%-19s:%-19d|\r\n"
#define LOG_FMT_U_X   "%-19s:%-19u|%-19s:0x%-17x|\r\n"
#define LOG_FMT_U_P   "%-19s:%-19u|%-19s:%-19p|\r\n"
#define LOG_FMT_U_S   "%-19s:%-19u|%-19s:%-19s|\r\n"
#define LOG_FMT_D_F   "%-19s:%-19d|%-19s:%08d.%-10d|\r\n"
#define LOG_FMT_X_D_D "%-19s:0x%-17x|%-19s:%d/%d|\r\n"
#define LOG_FMT_LX_LX "%-19s:0x%-17llx|%-19s:0x%-17llx|\r\n"
#define LOG_FMT_LX_X  "%-19s:0x%-17llx|%-19s:0x%-17x|\r\n"


/*   com log command  */
#define LOG_CMD_HELP           "help"
#define LOG_CMD_CUR_CHAN       "chan"
#define LOG_CMD_SAVE_YUV       "saveyuv"
#define LOG_CMD_SAVE_MTDT      "savemtdt"
#define LOG_CMD_PRINT          "print"
#define LOG_CMD_DPS_SER        "dpsser"
#define LOG_CMD_DPS_NUM        "dpsnum"
#define LOG_CMD_ERR_HOLD       "errhold"
#define LOG_CMD_CMP_MODE       "cmpmode"
#define LOG_CMD_FRM_LAYOUT     "frmlayout"
#define LOG_CMD_OUT_ORDER      "outorder"
#define LOG_CMD_DEC_MODE       "decmode"
#define LOG_CMD_TUNL_LINE      "tunlline"
#define LOG_CMD_MOD_LOW_DLY    "modlowdly"
#define LOG_CMD_SAVE_BEGIN     "savebegin"
#define LOG_CMD_SAVE_END       "saveend"
#define LOG_CMD_SET_CLK        "setclk"
#define LOG_CMD_SHOW_DFLT      "showdflt"
#define LOG_CMD_PRT_STM_BLK    "stm_pr_block"
#define LOG_CMD_VPP_BYPASS     "vpp_bypass"
#define LOG_CMD_VPP_PR_FRM     "vpp_pr_frm"
#define LOG_CMD_VPP_PR_BUF     "vpp_pr_buf"
#define LOG_CMD_VPP_NO_RECV    "vpp_no_recv"
#define LOG_CMD_DPS_NO_RECV    "set_no_dps"
#define LOG_CMD_SET_DBG_PATH   "setpath"
#define LOG_CMD_SAVE_VDH_REG   "savereg"
#define LOG_CMD_SAVE_RAW       "saveraw"
#define LOG_CMD_LINEAR_NO_GAP  "linear_no_gap"

/*   stm log command  */
#define LOG_CMD_SAVE_SEG       "saveseg"
#define LOG_CMD_SAVE_SEEK_RAW  "saveseekraw"
#define LOG_CMD_STM_SLEEP      "stmsleep"
#define LOG_CMD_STM_WAIT       "stmwait"
#define LOG_CMD_STM_NO_RAW     "stm_no_raw"
#define LOG_CMD_STM_NO_SEG     "stm_no_seg"
#define LOG_CMD_STM_SEG_FULL   "stm_seg_full"
#define LOG_CMD_STM_BYPASS     "stmbypass"

/*   syn log command  */
#define LOG_CMD_SYN_SLEEP      "synsleep"
#define LOG_CMD_SYN_WAIT       "synwait"
#define LOG_CMD_NO_RECV_SEG    "syn_no_recv_seg"
#define LOG_CMD_SYN_BYPASS     "synbypass"

/*   dec log command  */
#define LOG_CMD_DEC_SLEEP      "decsleep"
#define LOG_CMD_DEC_WAIT       "decwait"
#define LOG_CMD_DEC_CHECK      "deccheck"
#define LOG_CMD_DEC_BYPASS     "decbypass"

/*   fsp log command  */
#define LOG_CMD_FSP_NO_DEC     "fsp_no_dec"
#define LOG_CMD_FSP_NO_DSP     "fsp_no_dsp"
#define LOG_CMD_FSP_NO_PMV     "fsp_no_pmv"
#define LOG_CMD_FSP_NO_ATT     "fsp_no_att"

/*   pts log command  */
#define LOG_CMD_SAMPLE_NUM     "samplenum"

/*   usr log command  */
#define LOG_CMD_USR_DEC_NUM    "decnum"

/*   hdr log command  */
#define LOG_CMD_READ_HDR       "readhdr"

#define CMD_STR_LEN (CMD_PARAM_MAX_COUNT * CMD_PARAM_MAX_LEN * sizeof(td_s8))

#define VFMW_LOG_ADDR_MASK

#define NONE_ZERO_ADDR_MASK 0xfffffffff
#ifndef VFMW_LOG_ADDR_MASK
#define log_get_mask_vir_addr(x) ((UADDR)((UADDR)(x) & NONE_ZERO_ADDR_MASK))
#define log_get_mask_phy_addr(x) ((UADDR)((UADDR)(x) & NONE_ZERO_ADDR_MASK))
#else
#define PHY_ADDR_MASK       0xf00000fff
#define log_get_mask_vir_addr(x) ((UADDR)((x) == 0 ? 0 : NONE_ZERO_ADDR_MASK))
#define log_get_mask_phy_addr(x) ((UADDR)((x) == 0 ? 0 : ((UADDR)(x) | PHY_ADDR_MASK)))
#endif

typedef td_char (*cmd_str_ptr)[CMD_PARAM_MAX_COUNT][CMD_PARAM_MAX_LEN];
typedef td_void (*cmd_handler)(cmd_str_ptr, td_u32);

typedef enum {
    LOG_MODULE_STM = 0,
    LOG_MODULE_SYN,
    LOG_MODULE_DEC,
    LOG_MODULE_COM,
    LOG_MODULE_FSP,
    LOG_MODULE_SHR,
    LOG_MODULE_MDC,
    LOG_MODULE_MAX,
} log_module;

typedef struct {
    td_char cmd_name[CMD_PARAM_MAX_LEN];
    cmd_handler handler;
} log_cmd;

#endif
