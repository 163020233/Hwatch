/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides ipc port \n
 *
 * History: \n
 * 2022-11-26， Create file. \n
 */

#include "ipc_porting.h"
#include "ipc_new_shared.h"
#include "cmsis_os2.h"
#include "debug_print.h"
#include "osal_msgqueue.h"
#include "ipc_new_shared.h"
#include "tcxo.h"

#ifdef AT_IPC_V1_TEST

#define IPC_RTC_ONE_SECOND                          1000000
#define BITS_WIDTH                      8
/* bits/us convert to kbits/s => 1 bits/us = (1 * 1000000)/1024 = 1 * 976 kbits/s */
#define CONVERT_PARA         976
#define IPC_AT_A2X_NO_WAIT 0
#define IPC_TASK_PRIORITY 20
#define IPC_TASK_STACK_SIZE 0x0C00
#define IPC_ASYNC_TEST_STACK_SIZE 0x0C00
#define IPC_WAIT_FOREVER 0xFFFFFFFF

typedef struct {
    uint16_t send_index;
    uint16_t recv_index;
    uint16_t err_index;
} ipc_basic_comm_trace;

volatile ipc_basic_comm_trace g_basic_comm_test = {0};
static uint32_t g_ipc_test_count = 0;
ipc_test_struct g_ipc_test = {0};

static void ipc_v1_host_test_callback(uint8_t *payload_addr, uint32_t payload_len)
{
    UNUSED(payload_len);
    ipc_test_struct *ipc_data = (ipc_test_struct *)payload_addr;
    if (ipc_data->header.len > IPC_TEST_DATA_LEN) {
        PRINT("receive len is too large, is %d.\r\n", ipc_data->header.len);
        return;
    }
    if (ipc_data->header.cmd == IPC_TEST_TYPE_BASIC_COM_RES) {
        uint8_t data = g_basic_comm_test.send_index;
        data = ~data;
        for (int i = 0; i < ipc_data->header.len; i++) {
            if (ipc_data->data[i] != data) {
                PRINT("rec data[%d] error, expect[0x%x], actual[0x%x] \r\n", i, data, ipc_data->data[i]);
                g_basic_comm_test.err_index++;
            }
        }
        if (g_basic_comm_test.err_index == 0) {
            g_basic_comm_test.recv_index++;
        }
    }
}

static void ipc_v1_slave_test_callback(uint8_t *payload_addr, uint32_t payload_len)
{
    UNUSED(payload_len);
    ipc_test_struct *ipc_data = (ipc_test_struct *)payload_addr;
    if (ipc_data->header.len > IPC_TEST_DATA_LEN) {
        PRINT("receive len is too large, is %d.\r\n", ipc_data->header.len);
        return;
    }
    if (ipc_data->header.cmd == IPC_TEST_TYPE_BASIC_COM_CMD) {
        uint8_t src_core = ipc_data->header.core;
        for (int i = 0; i < ipc_data->header.len; i++) {
            uint8_t data = ipc_data->data[i];
            data = ~data;
            ipc_data->data[i] = data;
        }
        ipc_data->header.cmd = IPC_TEST_TYPE_BASIC_COM_RES;
        ipc_data->header.core = CORE;
        if (ipc_data->header.core == 0x7) { // ipc内部把Sensor当做4处理
            ipc_data->header.core = 0x4;
        }
        ipc_msg_info_t msg_info = {0};
        msg_info.dst_core = src_core;
        msg_info.msg_id = IPC_MSG_TEST_HOST;
        msg_info.channel = ipc_data->header.channel;
        msg_info.priority = IPC_PRIORITY_LOWEST;
        msg_info.buf_addr = payload_addr;
        msg_info.buf_len = sizeof(ipc_test_header) + ipc_data->header.len;
        uapi_ipc_send_msg_async(&msg_info);
    } else if (ipc_data->header.cmd == IPC_TEST_TYPE_PFMS_DATA_SEND) {
        g_ipc_test_count++;
        int sec_ret = memcpy_s((uint8_t *)&g_ipc_test.data, IPC_TEST_DATA_LEN,
                               (void *)ipc_data->data, ipc_data->header.len);
        if (sec_ret != 0) {
            PRINT("copy err ret = %d, copy_len = %d\r\n", sec_ret, ipc_data->header.len);
            return;
        }
        if (g_ipc_test_count % ipc_data->header.times == 0) {
            PRINT("receive %d times\r\n", ipc_data->header.times);
            g_ipc_test_count = 0;
        }
    }
}

void test_ipc_basic_send(ipc_test_params *argc)
{
    ipc_test_struct* src_buf = malloc(sizeof(ipc_test_struct));
    if (src_buf == NULL) {
        PRINT("malloc failed\n");
        return;
    }
    memset_s(src_buf, sizeof(ipc_test_struct), 0, sizeof(ipc_test_struct));
    src_buf->header.cmd = IPC_TEST_TYPE_BASIC_COM_CMD;
    src_buf->header.times = argc->times;
    src_buf->header.len = argc->size;
    src_buf->header.core = CORE;
    if (src_buf->header.core == 0x7) { // ipc内部把Sensor当做4处理
        src_buf->header.core = 0x4;
    }
    src_buf->header.channel = argc->channel;
    g_basic_comm_test.send_index = 0;
    g_basic_comm_test.recv_index = 0;
    g_basic_comm_test.err_index = 0;
    uint64_t start_rtc_time = uapi_tcxo_get_us();
    uint64_t rtc_abort_time = uapi_tcxo_get_us() + (30 * IPC_RTC_ONE_SECOND);
    do {
        g_basic_comm_test.send_index++;
        memset_s(src_buf->data, IPC_TEST_DATA_LEN, (uint8_t)g_basic_comm_test.send_index, argc->size);
        ipc_msg_info_t msg_info = {0};
        msg_info.dst_core = argc->dst_core;
        msg_info.msg_id = IPC_MSG_TEST_SLAVE;
        msg_info.channel = argc->channel;
        msg_info.priority = IPC_PRIORITY_LOWEST;
        msg_info.buf_addr = (uint8_t *)src_buf;
        msg_info.buf_len = sizeof(ipc_test_header) + argc->size;
        uapi_ipc_send_msg_async(&msg_info);
        while (g_basic_comm_test.send_index != g_basic_comm_test.recv_index) {
            if (uapi_tcxo_get_us() >= rtc_abort_time) {
                free(src_buf);
                return;
            }
        }
    } while (g_basic_comm_test.send_index < argc->times);
    if (g_basic_comm_test.err_index) {
        free(src_buf);
        return;
    }
    uint64_t end_rtc_time = uapi_tcxo_get_us();
    PRINT("send %ld byte cause %lld us rtc time, rate = %lld kbits/s\r\n",
        argc->times * argc->size, end_rtc_time - start_rtc_time,
        (uint64_t)((uint64_t)(argc->times * argc->size) * BITS_WIDTH) * CONVERT_PARA / (end_rtc_time - start_rtc_time));
    PRINT("test success, send_tims = %d, recv times = %d, test data size = %d, err_times = %d.\r\n",
        g_basic_comm_test.send_index, g_basic_comm_test.recv_index, argc->size, g_basic_comm_test.err_index);
    free(src_buf);
}

static osThreadId_t g_ipc_task_handler = NULL;
static unsigned long g_ipc_queue_id = 0;

void ipc_test_task_main(void *params)
{
#define IPC_QUEUE_LEN 0x4
    UNUSED(params);
    ipc_test_params msg;
    uint32_t buffer_size = sizeof(ipc_test_params);
    osal_msg_queue_create(NULL, IPC_QUEUE_LEN, &g_ipc_queue_id, 0, (uint16_t)buffer_size);
    while (1) {
        if (osal_msg_queue_read_copy(g_ipc_queue_id, &msg, &buffer_size, IPC_WAIT_FOREVER) == 0) {
            test_ipc_basic_send(&msg);
        }
    }
}

static void ipc_v1_at_a2x_callback(uint8_t *payload_addr, uint32_t payload_len)
{
    UNUSED(payload_len);
    ipc_test_params *ipc_params = (ipc_test_params *)payload_addr;
    if (g_ipc_task_handler == NULL) {
        PRINT("ipc_test_task create failed\r\n");
        return;
    }
    if (osal_msg_queue_write_copy(g_ipc_queue_id, ipc_params, sizeof(ipc_test_params), IPC_AT_A2X_NO_WAIT) != 0) {
        PRINT("osal_msg_queue_write_copy failed\r\n");
    }
}

void test_ipc_init(void)
{
    ipc_rx_handler_info_t handler_info = {0};
    handler_info.msg_id = IPC_MSG_TEST_HOST;
    handler_info.cb = ipc_v1_host_test_callback;
    uapi_ipc_register_rx_handler(&handler_info);
    handler_info.msg_id = IPC_MSG_TEST_SLAVE;
    handler_info.cb = ipc_v1_slave_test_callback;
    uapi_ipc_register_rx_handler(&handler_info);

    handler_info.msg_id = IPC_MSG_AT_A2X_TEST;
    handler_info.cb = ipc_v1_at_a2x_callback;
    uapi_ipc_register_rx_handler(&handler_info);
    osThreadAttr_t attr = { "ipc_test_task", 0, NULL, 0, NULL, IPC_TASK_STACK_SIZE, IPC_TASK_PRIORITY, 0, 0 };
    g_ipc_task_handler = osThreadNew(ipc_test_task_main, NULL, &attr);
    if (g_ipc_task_handler == NULL) {
        PRINT("ipc_test_task create failed\r\n");
        return;
    }
    PRINT("ipc_test_task create success \r\n");
}

// ipc async test
volatile int g_recv_counts[2] = {0, 0};
#define REPORT_LEN 300
static osThreadId_t g_ipc_async_task_handler = NULL;
static unsigned long g_ipc_async_queue_id = 0;
volatile uint64_t g_start_rtc_time = 0;

void report_result(ipc_test_params *param)
{
    uint64_t cost_us = uapi_tcxo_get_us() - g_start_rtc_time;
    uint64_t rate = 0;
    char report[REPORT_LEN] = {0};

    if (param->cmd_type == IPC_TEST_ASYNC_MULTI) {
        rate = (uint64_t)(g_recv_counts[0] + g_recv_counts[1] - 1) * param->size * BITS_WIDTH * CONVERT_PARA / cost_us;
        snprintf_s(report, REPORT_LEN, REPORT_LEN - 1,
                   "expect msg count=%d, channel:0 recv msg count=%d, channel:1 recv msg count=%d, expect recv %d byte "
                   "cost %u us rtc time, maybe rate = %u kbits/s report result.\n",
                   param->times, g_recv_counts[0], g_recv_counts[1], param->times * param->size * 2,  // 2: 两个通道
                   (uint32_t)cost_us, (uint32_t)rate);
    } else {
        rate = (uint64_t)(g_recv_counts[param->channel] - 1) * param->size * BITS_WIDTH * CONVERT_PARA / cost_us;
        snprintf_s(report, REPORT_LEN, REPORT_LEN - 1,
                   "expect msg count=%d, recv msg count=%d, expect recv %d byte "
                   "cost %u us rtc time, maybe rate = %u kbits/s report result.\n",
                   param->times, g_recv_counts[param->channel], param->times * param->size, (uint32_t)cost_us,
                   (uint32_t)rate);
    }

    if (CORE == APPS) {
        PRINT("%s\r\n", report);
    } else {
        ipc_msg_info_t msg_info = {0};
        msg_info.dst_core = APPS;
        msg_info.msg_id = IPC_ASYNC_TEST_REPORT;
        msg_info.channel = 0;
        msg_info.priority = IPC_PRIORITY_LOWEST;
        msg_info.buf_addr = &report;
        msg_info.buf_len = strlen(report) + 1;
        uapi_ipc_send_msg_async(&msg_info);
    }

    memset_s(&g_recv_counts, sizeof(g_recv_counts), 0, sizeof(g_recv_counts));
}

static void ipc_v1_async_test_msg_callback(uint8_t *payload_addr, uint32_t payload_len)
{
    // 收到消息，校验，更新计数器
    UNUSED(payload_len);
    ipc_test_struct *ipc_data = (ipc_test_struct *)payload_addr;
    if (ipc_data->header.len == 0) {
        return;
    }

    if (ipc_data->header.cmd == IPC_TEST_ASYNC_MULTI) {
        if (g_recv_counts[0] == 0 && g_recv_counts[1] == 0) {
            g_start_rtc_time = uapi_tcxo_get_us();  // 记录开始时间
        }
    } else {
        if (g_recv_counts[ipc_data->header.channel] == 0) {
            g_start_rtc_time = uapi_tcxo_get_us();  // 记录开始时间
        }
    }

    for (int i = 0; i < ipc_data->header.len; ++i) {
        if (ipc_data->data[i] != (g_recv_counts[ipc_data->header.channel] & 0xFF)) {
            PRINT("recv data[%d] error, expect[0x%x], actual[0x%x] \r\n", i, g_recv_counts[ipc_data->header.channel], ipc_data->data[i]);
            return;
        }
    }
    ++g_recv_counts[ipc_data->header.channel];
}

static void ipc_v1_async_test_notify_callback(uint8_t *payload_addr, uint32_t payload_len)
{
    UNUSED(payload_len);
    ipc_test_params *ipc_params = (ipc_test_params *)payload_addr;
    osal_msg_queue_write_copy(g_ipc_async_queue_id, ipc_params, sizeof(ipc_test_params), IPC_AT_A2X_NO_WAIT);
}

// A核接收测试报告
static void ipc_v1_async_test_report_callback(uint8_t *payload_addr, uint32_t payload_len)
{
    PRINT("payload_len=%d\r\n", payload_len);
    PRINT("%s\r\n", payload_addr);
}

static void test_send_async(ipc_msg_info_t* msg_info, ipc_test_params *param, ipc_test_struct* src_buf)
{
    volatile int g_send_error_count = 0;
    for (int i = 0; i < param->times;) {
        // data中每个字节值都改为i
        memset_s(src_buf->data, IPC_TEST_DATA_LEN, (uint8_t)i, param->size);
        // 发送数据
        errcode_t ret = uapi_ipc_send_msg_async(msg_info);
        if (ret != ERRCODE_SUCC) {
            ++g_send_error_count;
        } else {
            ++i;
        }
    }
    PRINT("g_send_error_count=%d\r\n", g_send_error_count);
}

static void test_send_async_multi(ipc_msg_info_t *msg_info, ipc_test_params *param, ipc_test_struct *src_buf)
{
    volatile int chan0_send_error_count = 0;
    volatile int chan1_send_error_count = 0;
    for (int i = 0; i < param->times; ++i) {
        // data中每个字节值都改为i
        memset_s(src_buf->data, IPC_TEST_DATA_LEN, (uint8_t)i, param->size);

        // 发送数据
        src_buf->header.channel = msg_info->channel = 0;
        while (1) {
            if (uapi_ipc_send_msg_async(msg_info) == ERRCODE_SUCC) {
                break;
            }
            ++chan0_send_error_count;
        }

        src_buf->header.channel = msg_info->channel = 1;
        while (1) {
            if (uapi_ipc_send_msg_async(msg_info) == ERRCODE_SUCC) {
                break;
            }
            ++chan1_send_error_count;
        }
    }
    PRINT("chan0 error=%d, chan1 error=%d\n", chan0_send_error_count, chan1_send_error_count);
}

void test_ipc_async_send(ipc_test_params *param)
{
    ipc_test_struct* src_buf = malloc(sizeof(ipc_test_struct));
    if (src_buf == NULL) {
        PRINT("malloc failed\n");
        return;
    }
    memset_s(src_buf, sizeof(ipc_test_struct), 0, sizeof(ipc_test_struct));
    src_buf->header.times = param->times;
    src_buf->header.len = param->size;
    src_buf->header.core = CORE;
    src_buf->header.channel = param->channel;
    src_buf->header.cmd = param->cmd_type;

    ipc_msg_info_t msg_info = {0};
    msg_info.dst_core = param->dst_core;
    msg_info.msg_id = IPC_ASYNC_TEST_MSG;
    msg_info.channel = param->channel;
    msg_info.priority = IPC_PRIORITY_LOWEST;
    msg_info.buf_addr = src_buf;
    msg_info.buf_len = sizeof(ipc_test_header) + param->size;
    if (param->cmd_type == IPC_TEST_ASYNC_MULTI) {
        test_send_async_multi(&msg_info, param, src_buf);
    } else {
        test_send_async(&msg_info, param, src_buf);
    }

    free(src_buf);
}

void test_ipc_async_recv(ipc_test_params *param)
{
    // 开始等待
    uint64_t abort_rtc_time = uapi_tcxo_get_us() + (30 * IPC_RTC_ONE_SECOND);
    while (uapi_tcxo_get_us() < abort_rtc_time) {
        if (param->cmd_type == IPC_TEST_ASYNC_MULTI) {
            if (param->times == g_recv_counts[0] && param->times == g_recv_counts[1]) {
                break;
            }
        } else {
            if (param->times == g_recv_counts[param->channel]) {
                break;
            }
        }
    }

    report_result(param);
}

void ipc_aysnc_test_task_main(void *params)
{
#define IPC_QUEUE_LEN 0x4
    UNUSED(params);
    ipc_test_params msg;
    uint32_t buffer_size = sizeof(ipc_test_params);
    osal_msg_queue_create(NULL, IPC_QUEUE_LEN, &g_ipc_async_queue_id, 0, (uint16_t)buffer_size);
    while (1) {
        if (osal_msg_queue_read_copy(g_ipc_async_queue_id, &msg, &buffer_size, IPC_WAIT_FOREVER) != 0) {
            continue;
        }

        // 收到通知，根据src和dst判断自己是生产还是消费者
        if (msg.src_core == CORE) {
            // 生产者，开始发送消息
            test_ipc_async_send(&msg);
        } else if (msg.dst_core == CORE) {
            // 消费者，接收消息
            test_ipc_async_recv(&msg);
        }
    }
}

void test_ipc_async(ipc_test_params *param)
{
    ipc_msg_info_t msg_info = {0};
    msg_info.msg_id = IPC_ASYNC_TEST_NOTIFY;
    msg_info.channel = param->channel;
    msg_info.priority = IPC_PRIORITY_LOWEST;
    msg_info.buf_addr = param;
    msg_info.buf_len = sizeof(ipc_test_params);

    if (param->src_core == CORE) {
        // MCU是生产者
        msg_info.dst_core = param->dst_core;
        uapi_ipc_send_msg_async(&msg_info);
        ipc_v1_async_test_notify_callback(param, sizeof(ipc_test_params));
    } else if (param->dst_core == CORE) {
        // MCU是消费者
        msg_info.dst_core = param->src_core;
        uapi_ipc_send_msg_async(&msg_info);
        ipc_v1_async_test_notify_callback(param, sizeof(ipc_test_params));
    } else {
        // MCU只接收报告
        msg_info.dst_core = param->dst_core;
        uapi_ipc_send_msg_async(&msg_info);
        msg_info.dst_core = param->src_core;
        uapi_ipc_send_msg_async(&msg_info);
    }
}

void test_ipc_async_init(void)
{
    ipc_rx_handler_info_t handler_info = {0};
    handler_info.msg_id = IPC_ASYNC_TEST_NOTIFY;  // 发送测试消息前，告知生产、消费双方的测试信息
    handler_info.cb = ipc_v1_async_test_notify_callback;
    uapi_ipc_register_rx_handler(&handler_info);

    handler_info.msg_id = IPC_ASYNC_TEST_MSG;  // ipc测试消息
    handler_info.cb = ipc_v1_async_test_msg_callback;
    uapi_ipc_register_rx_handler(&handler_info);

#if CORE == APPS
    handler_info.msg_id = IPC_ASYNC_TEST_REPORT;
    handler_info.cb = ipc_v1_async_test_report_callback;  // 测试完成后，消费方向A核报告测试结果
    uapi_ipc_register_rx_handler(&handler_info);
#endif

    osThreadAttr_t attr = {"ipc_async_test_task", 0, NULL, 0, NULL, IPC_ASYNC_TEST_STACK_SIZE, IPC_TASK_PRIORITY, 0, 0};
    g_ipc_async_task_handler = osThreadNew(ipc_aysnc_test_task_main, NULL, &attr);
    if (g_ipc_async_task_handler == NULL) {
        PRINT("ipc_async_test_task create failed\r\n");
        return;
    }
    PRINT("ipc_async_test_task create success \r\n");
}

#endif