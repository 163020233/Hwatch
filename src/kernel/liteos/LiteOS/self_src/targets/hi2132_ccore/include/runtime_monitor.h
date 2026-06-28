/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: main process
 * Author: Huawei LiteOS Team
 * Create: 2020-08-10
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

#ifndef __RUNTIME_MONITOR_H
#define __RUNTIME_MONITOR_H

#include "los_typedef.h"

/* define the system status check interval  */
#define STATUS_CHECK_INTERVAL 5000    /* 5s */
#define STATUS_CHECK_MAX_FAIL_COUNT 6 /* 30s */

#define SET_SYS_READY 1
#define SET_SYS_SLEEP 0

typedef enum tagSysStatus {
    SYS_START_ERR = -1,
    SYS_OS_READY = 0,
    SYS_INIT = 1,
    SYS_FPGA_DOWNLOAD,
    SYS_READY,
    SYS_STANDBY,
    SYS_SLEEP,
    SYS_POWERDOWN,
} SYS_STATUS;

typedef enum tagMonitorTask {
#if (CORE == APPS)
    MON_TASK_BEGIN = 0,

    /* APP TASKS BEGIN */
    MON_TASK_RPC = MON_TASK_BEGIN, /* rpc_main */
    MON_TASK_AGING,                /* aging_test_task */
    MON_TASK_APP,                  /* app_task */
    MON_TASK_BASIC_SENSOR,         /* basic_sensor_task */
    MON_TASK_COMMU,                /* commu_task */
    MON_TASK_DEVICE_MISC,          /* device_misc_task */
    MON_TASK_FORWARD,              /* forward_task */
    MON_TASK_FILE_RTAN,            /* file_tran_task */
    MON_TASK_OTA,                  /* protocol_task */
    MON_TASK_GESTURE,              /* gesture_task */
    MON_TASK_MAINTENANCE,          /* maintenance_task */
    MON_TASK_MANUFACTURE,          /* manufacture_task */
    MON_TASK_POWER,                /* power_task */
    MON_TASK_SYS,                  /* sys_task */
    MON_TASK_UX,                   /* ux_task */
    MON_TASK_WEAR,                 /* wear_task */
    MON_TASK_BT_DEVICE,            /* bt_task */
    MON_TASK_AUDIO,                /* audio_thread_task */
    MON_TASK_SENSOR_DATA,          /* sensors_data_task */

    MON_TASK_BTC,
    MON_TASK_BTH_INIT,
    MON_TASK_BTH_RPC,
    MON_TASK_BTH_GDK,
    MON_TASK_BTH_SDK,
    MON_TASK_BTH_MSG,

#if (APP_BTN_TASK_MONITOR_ENABLE == YES)
    MON_TASK_BUTTON, /* button_task */
#endif
    MON_TASK_LOG,
    /* APP TASKS END */
    MON_TASK_END,
#else
    MON_TASK_BEGIN = 0,
    MON_TASK_BT_DEMO = MON_TASK_BEGIN,
    MON_TASK_RPC,
    MON_TASK_BTC,
    MON_TASK_BTH_INIT,
    MON_TASK_BTH_RPC,
    MON_TASK_BTH_GDK,
    MON_TASK_BTH_SDK,
    MON_TASK_BTH_MSG,
#if (BTH_WITH_SMART_WEAR == YES)
    MON_TASK_BTV_BT_TRAN,
    MON_TASK_BTV_A2DP_SERVICE,
#endif
    MON_TASK_END,
#endif
} MONITOR_TASK_ID_E;

VOID runtime_monitor_init(VOID);
VOID osTskMonBegin(UINT32 uwMailID);
VOID osTskMonEnd(UINT32 uwMailID);
void statuscheck_timer_callback(void);
void RuntimeMonitorTimerStop(void);
void RuntimeMonitorTimerStart(void);

#endif
