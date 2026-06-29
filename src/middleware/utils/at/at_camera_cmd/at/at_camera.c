/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: At camera function
 *
 */

#include <string.h>

#include "at_cmd.h"
#include "at_camera_cmd_register.h"
#include "at_camera_cmd_table.h"
#include "debug_print.h"
#include "td_base.h"
#include "camera_sample.h"
#include "at_camera.h"

#define AT_CAMERA_FUNC_NUM (sizeof(at_camera_cmd_parse_table) / sizeof(at_camera_cmd_parse_table[0]))
void los_at_camera_cmd_register(void)
{
    print_str("los_at_camera_cmd_register EXCUTE\r\n");
    uapi_at_camera_register_cmd(at_camera_cmd_parse_table, AT_CAMERA_FUNC_NUM);
}

at_ret_t camera_process(const camera_para_t *arg)
{
#ifdef SUPPORT_AT_CAMERA
    PRINT("camera_process enter \r\n");
    char *argv[] = {arg->para1, arg->para2, arg->para3, arg->para4, arg->para5, arg->para6};
    CameraSample(sizeof(argv) / sizeof(char *), argv);
    return AT_RET_OK;
#else
    uapi_unused(arg);
    PRINT("SUPPORT_AT_CAMERA NOT DEFINE! \r\n");
    return AT_RET_PROC_CMD_FUNC_MISSING;
#endif
}
