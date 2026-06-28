/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description:  PMU alarm Source
 */

#include "pmu_alarm.h"
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include "securec.h"
#include "pmu_ulp.h"
#include "systick_porting.h"
#include "osal_task.h"
#include "soc_osal.h"
#include "cmsis_os2.h"
#include "debug_print.h"
#include "osal_msgqueue.h"
#include "chip_io.h"
#include "sys/time.h"
#include "clock_calibration.h"

#if !defined(SW_UART_DEBUG) && !defined(SW_RTT_DEBUG)
#define PMU_ALARM_PRINT_ERR(fmt, arg...) uapi_diag_error_log(0, fmt, ##arg)
#define PMU_ALARM_PRINT_WARN(fmt, arg...) uapi_diag_warning_log(0, fmt, ##arg)
#define PMU_ALARM_PRINT_INFO(fmt, arg...) uapi_diag_info_log(0, fmt, ##arg)
#ifdef WEARABLE_LOG_DEBUG
#define PMU_ALARM_PRINT_DEBUG(fmt, arg...) uapi_diag_debug_log(0, fmt, ##arg)
#else
#define PMU_ALARM_PRINT_DEBUG(fmt, arg...)
#endif
#else
#define PMU_ALARM_PRINT_ERR(fmt, arg...) PRINT("[PMU_ALARM E/] %s: " fmt "\r\n", __FUNCTION__, ##arg)
#define PMU_ALARM_PRINT_WARN(fmt, arg...) PRINT("[PMU_ALARM W/] %s: " fmt "\r\n", __FUNCTION__, ##arg)
#define PMU_ALARM_PRINT_INFO(fmt, arg...) PRINT("[PMU_ALARM I/] %s: " fmt "\r\n", __FUNCTION__, ##arg)
#ifdef WEARABLE_LOG_DEBUG
#define PMU_ALARM_PRINT_DEBUG(fmt, arg...) PRINT("[PMU_ALARM D/] %s: " fmt "\r\n", __FUNCTION__, ##arg)
#else
#define PMU_ALARM_PRINT_DEBUG(fmt, arg...)
#endif
#endif

#define CONFIG_PATH "/system/"
#define CONFIG_FILE "alarm.cfg"
#define CONFIG_FILE_PATH CONFIG_PATH CONFIG_FILE    // 闹钟配置文件

#define ALARM_TASK_SIZE 0x1000
#define ALARM_MSG_COUNT 4
#define ALARM_TASK_PRIO 5

#define DAYS_PER_WEEK 7

#define FILE_HEADER_VERSION_10 (0x10)
#define FILE_HEADER_VERSION_11 (0x11)
#define FILE_HEADER_VERSION FILE_HEADER_VERSION_11  // 当前配置文件版本，有文件改动时递增

enum {
    SUNDAY = 0,
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY,
};

typedef struct {
    uint8_t file_version;
    uint8_t reserved[3];
} file_header_t;

static file_header_t g_file_header = {
    .file_version = FILE_HEADER_VERSION,
};

static alarm_info_t g_period_alarm_infos[MAX_ALARM_NUM] = {0};       // 全局按周配置闹钟信息数组
static alarm_info_t g_single_alarm_infos[MAX_ALARM_NUM] = {0};       // 全局单次闹钟信息数组
static alarm_callback_t g_alarm_callbacks[ALARM_APP_MAX_NUM] = {0};  // 全局回调函数指针
static unsigned long g_alarm_queue = 0xFFFFFFFF;
static bool g_alarm_inited = false;
static bool g_alarm_trigger = false;

static uint64_t get_timestamp(void)
{
    struct timeval tv = {0};
    gettimeofday(&tv, NULL);
    return tv.tv_sec;
}

static void clear_alarm_infos(void)
{
    uint32_t irq_sts = osal_irq_lock();
    memset_s(g_period_alarm_infos, sizeof(g_period_alarm_infos), 0, sizeof(g_period_alarm_infos));
    memset_s(g_single_alarm_infos, sizeof(g_single_alarm_infos), 0, sizeof(g_single_alarm_infos));
    osal_irq_restore(irq_sts);
}

static void flush_alarm_info(void)
{
    uint32_t irq_sts = osal_irq_lock();
    alarm_info_t tmp_alarm_infos[MAX_ALARM_NUM];
    memcpy_s(tmp_alarm_infos, sizeof(tmp_alarm_infos), g_period_alarm_infos, sizeof(tmp_alarm_infos));
    osal_irq_restore(irq_sts);
    FILE *fp = fopen(CONFIG_FILE_PATH, "wb+");
    if (!fp) {
        PMU_ALARM_PRINT_ERR("failed to open the alarm file in write mode.\r\n");
        return;
    }

    do {
        if (fwrite(&g_file_header, sizeof(file_header_t), 1, fp) == 0) {
            PMU_ALARM_PRINT_ERR("write file header failed\r\n");
            break;
        }
        bool err_sign = false;
        for (int i = 0; i < MAX_ALARM_NUM; ++i) {
            if (fwrite(&tmp_alarm_infos[i], sizeof(alarm_info_t), 1, fp) == 0) {
                PMU_ALARM_PRINT_ERR("write alarm file failed\r\n");
                err_sign = true;
                break;
            }
        }
        if (err_sign) {
            break;
        }
        if (fflush(fp) != 0) {
            PMU_ALARM_PRINT_ERR("flush alarm file failed\r\n");
        }
    } while (0);

    if (fclose(fp) != 0) {
        PMU_ALARM_PRINT_ERR("failed to close the alarm file in write mode.\r\n");
    }
}

uint32_t get_file_version(file_header_t file_header)
{
    for (uint32_t i = 0; i < sizeof(file_header.reserved); ++i) {
        // reserved应该均为0
        if (file_header.reserved[i] != 0) {
            return 0;
        }
    }
    return file_header.file_version;
}

static void read_alarm_info_version_10(FILE *fp)
{
    alarm_info_t tmp_alarm_infos[MAX_ALARM_NUM] = {0};
    for (int i = 0; i < MAX_ALARM_NUM; ++i) {
        if (fread(&tmp_alarm_infos[i], sizeof(alarm_info_t), 1, fp) == 0) {
            PMU_ALARM_PRINT_ERR("read alarm file failed\r\n");
            return;
        }
    }
    uint32_t irq_sts = osal_irq_lock();
    for (int i = 0; i < MAX_ALARM_NUM; ++i) {
        if (tmp_alarm_infos[i].type == ALARM_WEEK) {  // 只转移周期闹钟
            g_period_alarm_infos[i] = tmp_alarm_infos[i];
        }
    }
    osal_irq_restore(irq_sts);
}

static void read_alarm_info_version_11(FILE *fp)
{
    alarm_info_t tmp_alarm_infos[MAX_ALARM_NUM] = {0};
    for (int i = 0; i < MAX_ALARM_NUM; ++i) {
        if (fread(&tmp_alarm_infos[i], sizeof(alarm_info_t), 1, fp) == 0) {
            PMU_ALARM_PRINT_ERR("read alarm file failed\r\n");
            break;
        }
    }
    uint32_t irq_sts = osal_irq_lock();
    memcpy_s(g_period_alarm_infos, sizeof(g_period_alarm_infos), tmp_alarm_infos, sizeof(tmp_alarm_infos));
    osal_irq_restore(irq_sts);
}

static void read_alarm_info(void)
{
    clear_alarm_infos();

    FILE *fp = fopen(CONFIG_FILE_PATH, "rb+");
    if (!fp) {
        PMU_ALARM_PRINT_ERR("failed to open the alarm file in read mode.\r\n");
        return;
    }

    bool flush = false;
    do {
        file_header_t file_header = {0};
        if (fread(&file_header, sizeof(file_header_t), 1, fp) == 0) {
            PMU_ALARM_PRINT_ERR("read file_version failed\r\n");
            break;
        }

        uint32_t file_version = get_file_version(file_header);
        PMU_ALARM_PRINT_INFO("alarm file version=%x, current file version=%x\r\n", file_version, FILE_HEADER_VERSION);

        // 进行兼容性处理
        if (file_version == FILE_HEADER_VERSION) {
            read_alarm_info_version_11(fp);
        } else if (file_version == FILE_HEADER_VERSION_10) {
            // 读取version10配置
            read_alarm_info_version_10(fp);
            flush = true;
        } else {
            flush = true;  // 其他版本直接覆盖
        }
    } while (0);

    if (fclose(fp) != 0) {
        PMU_ALARM_PRINT_ERR("failed to close the alarm file in read mode.\r\n");
    }

    if (flush) {
        flush_alarm_info();
    }
}

static bool check_cfg_path_exist(void)
{
    struct stat st;
    if (stat(CONFIG_PATH, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            return true;
        }
    }
    PMU_ALARM_PRINT_WARN("CONFIG_PATH:%s not exist.\r\n", CONFIG_PATH);
    return false;
}

// 从week_days_t获取那些天被设置，放到target_week_days数组中，并返回总天数
static int get_alarm_week_days(week_days_t week_days, int *target_week_days, uint8_t length)
{
    unused(length);
    int week_index = 0;
    // 解析 week_days 字段
    if (week_days.days.sun) {
        target_week_days[week_index++] = SUNDAY;
    }
    if (week_days.days.mon) {
        target_week_days[week_index++] = MONDAY;
    }
    if (week_days.days.tue) {
        target_week_days[week_index++] = TUESDAY;
    }
    if (week_days.days.wed) {
        target_week_days[week_index++] = WEDNESDAY;
    }
    if (week_days.days.thu) {
        target_week_days[week_index++] = THURSDAY;
    }
    if (week_days.days.fri) {
        target_week_days[week_index++] = FRIDAY;
    }
    if (week_days.days.sat) {
        target_week_days[week_index++] = SATURDAY;
    }
    return week_index;
}

// 获取闹钟的到期时间
static uint64_t get_last_time(alarm_info_t *alarm_info)
{
    if (!alarm_info || alarm_info->status != ALARM_ENABLE) {
        return 0;
    }
    return alarm_info->alarm_timestamp;
}

// 获取周期闹钟下一次调度时间
static uint64_t get_period_next_time(alarm_info_t *alarm_info)
{
    if (!alarm_info || alarm_info->status != ALARM_ENABLE) {
        return 0;
    }

    time_t now = (time_t)get_timestamp();
    struct tm now_tm = {0};
    localtime_r(&now, &now_tm);
    int current_weekday = now_tm.tm_wday;

    alarm_week_time_t *alarm_week_time = &alarm_info->alarm_week_time;
    int alarm_week_days[DAYS_PER_WEEK] = {0};
    int week_num = get_alarm_week_days(alarm_week_time->week_days, alarm_week_days, DAYS_PER_WEEK);
    if (week_num == 0) {    // 表示一个不重复的闹钟
        struct tm target_tm = now_tm;
        target_tm.tm_hour = alarm_week_time->hour;
        target_tm.tm_min = alarm_week_time->min;
        target_tm.tm_sec = 0;
        if (mktime(&target_tm) <= now) {    // 如果小于当前，则说明是明天的闹钟
            target_tm.tm_mday += 1;
        }
        return mktime(&target_tm);
    }

    uint64_t min_seconds = UINT64_MAX;
    for (int i = 0; i < week_num; ++i) {
        int alarm_weekday = alarm_week_days[i];

        // 构造目标时间的 tm 结构
        struct tm target_tm = now_tm;
        target_tm.tm_hour = alarm_week_time->hour;
        target_tm.tm_min = alarm_week_time->min;
        target_tm.tm_sec = 0;
        target_tm.tm_mday += ((alarm_weekday - current_weekday + DAYS_PER_WEEK) % DAYS_PER_WEEK);

        // 如果目标时间小于当前时间，说明是下周
        if (mktime(&target_tm) <= now) {
            target_tm.tm_mday += DAYS_PER_WEEK;
        }

        time_t target_time = mktime(&target_tm);
        uint64_t seconds = (uint64_t)(target_time - now);
        if (seconds < min_seconds) {
            min_seconds = seconds;
        }
    }

    // 如果没有设置任何星期，返回最大值
    if (min_seconds == UINT64_MAX) {
        return 0;
    }

    return (uint64_t)now + min_seconds;
}

static alarm_handle_t find_idle_handle(alarm_info_t *alarm_infos)
{
    uint32_t irq_sts = osal_irq_lock();
    for (int i = 0; i < MAX_ALARM_NUM; ++i) {
        if (alarm_infos[i].status == ALARM_IDLE) {
            osal_irq_restore(irq_sts);
            return i;
        }
    }
    osal_irq_restore(irq_sts);
    return -1;
}

static void update_period_alarm_info(void)
{
    uint32_t irq_sts = osal_irq_lock();
    for (int i = 0; i < MAX_ALARM_NUM; ++i) {
        uint64_t time = get_period_next_time(&g_period_alarm_infos[i]);
        if (time != 0) {  // 更新到alarm_timestamp
            g_period_alarm_infos[i].alarm_timestamp = time;
        }
    }
    osal_irq_restore(irq_sts);
}

static alarm_handle_t find_min_handle(uint64_t *last_time)
{
    uint32_t irq_sts = osal_irq_lock();
    *last_time = UINT64_MAX;
    alarm_handle_t handle = -1;
    for (int i = 0; i < MAX_ALARM_NUM; ++i) {
        if (g_period_alarm_infos[i].status != ALARM_ENABLE) {
            continue;
        }
        uint64_t time = get_last_time(&g_period_alarm_infos[i]);  // 获取最小的触发时间
        if (time != 0 && time < *last_time) {
            handle = i;
            *last_time = time;
        }
    }
    for (int i = 0; i < MAX_ALARM_NUM; ++i) {
        if (g_single_alarm_infos[i].status != ALARM_ENABLE) {
            continue;
        }
        uint64_t time = get_last_time(&g_single_alarm_infos[i]);  // 获取最小的触发时间
        if (time != 0 && time < *last_time) {
            handle = i + MAX_ALARM_NUM;
            *last_time = time;
        }
    }
    osal_irq_restore(irq_sts);
    return handle;
}

static errcode_t exec_alarm_schedule(bool flush)
{
    update_period_alarm_info();  // 先更新所有alarm info
    if (flush) {
        flush_alarm_info();
    }

    uint64_t last_time;
    alarm_handle_t min_handle = find_min_handle(&last_time);  // 找到最小时间的handle
    if (min_handle < 0) {
        PMU_ALARM_PRINT_INFO("no handle, alarm disable\r\n");
        return uapi_alarm_disable();  // 无闹钟关闭
    }

    PMU_ALARM_PRINT_INFO("min_handle=%d, last_time=%llu\r\n", (int)min_handle, last_time);
    int32_t seconds = (int32_t)(last_time - get_timestamp());
    PMU_ALARM_PRINT_INFO("next alarm seconds=%d\r\n", seconds);
    if (seconds <= 0) {
        seconds = 1;  // 1: 最小1s，如果意外的导致一些alarm未触发使得seconds<0，或者seconds为0，则给与一个最小等待时间，重新处理
    }
    return uapi_alarm_enable(seconds);
}

// 处理多业务回调
static void alarm_callback_process(alarm_handle_t handle)
{
    // 回调上层业务
    PMU_ALARM_PRINT_INFO("alarm handle=%d\r\n", handle);
    for (int i = 0; i < ALARM_APP_MAX_NUM; ++i) {
        if (g_alarm_callbacks[i]) {
            g_alarm_callbacks[i](handle);
        }
    }
}

static bool check_null_weekdays(week_days_t week_days)
{
    return (week_days.raw & 0x7F) == 0;
}

static void alarm_process(void)
{
    uint32_t irq_sts = osal_irq_lock();
    // 查看那些alarm被触发
    bool handles[MAX_ALARM_HANDLE] = {false};
    for (int i = 0; i < MAX_ALARM_NUM; ++i) {
        if (g_period_alarm_infos[i].status != ALARM_ENABLE) {
            continue;
        }

        PMU_ALARM_PRINT_INFO("period handle=%d, alarm_timestamp=%llu, week_days=%d, hour=%d, min=%d\r\n", i,
                             g_period_alarm_infos[i].alarm_timestamp, g_period_alarm_infos[i].alarm_week_time.week_days.raw,
                             g_period_alarm_infos[i].alarm_week_time.hour, g_period_alarm_infos[i].alarm_week_time.min);

        uint64_t last_time = get_last_time(&g_period_alarm_infos[i]);
        uint64_t cur_time = get_timestamp();
        PMU_ALARM_PRINT_INFO("last_time=%llu, cur_time=%llu\r\n", last_time, cur_time);
        if (last_time > 0 && last_time <= cur_time) {
            handles[i] = true;
            if (check_null_weekdays(g_period_alarm_infos[i].alarm_week_time.week_days)) {
                // 关闭不重复闹钟
                g_period_alarm_infos[i].status = ALARM_CLOSE;
            }
        }
    }

    for (int i = 0; i < MAX_ALARM_NUM; ++i) {
        if (g_single_alarm_infos[i].status != ALARM_ENABLE) {
            continue;
        }
        PMU_ALARM_PRINT_INFO("single handle=%d, alarm_timestamp=%llu\r\n", MAX_ALARM_NUM + i,
                             g_single_alarm_infos[i].alarm_timestamp);

        uint64_t last_time = get_last_time(&g_single_alarm_infos[i]);
        uint64_t cur_time = get_timestamp();
        PMU_ALARM_PRINT_INFO("last_time=%llu, cur_time=%llu\r\n", last_time, cur_time);
        if (last_time > 0 && last_time <= cur_time) {
            handles[MAX_ALARM_NUM + i] = true;
            g_single_alarm_infos[i].status = ALARM_IDLE;  // 单次闹钟触发后删除
        }
    }
    osal_irq_restore(irq_sts);

    // 继续调度
    exec_alarm_schedule(true);

    for (int i = 0; i < MAX_ALARM_HANDLE; ++i) {
        if (handles[i]) {
            alarm_callback_process(i);
        }
    }
}

static void alarm_task(void const *arg)
{
    unused(arg);
    int msg_type = 0;
    uint32_t msgsize = (uint32_t)sizeof(int);
    while (1) {
        int ret = osal_msg_queue_read_copy(g_alarm_queue, (void *)&msg_type, &msgsize, OSAL_MSGQ_WAIT_FOREVER);
        if (ret != OSAL_SUCCESS) {
            continue;
        }
        if (msg_type == -1) {
            PMU_ALARM_PRINT_INFO("exit alarm thread\r\n");
            osal_msg_queue_delete(g_alarm_queue);
            break;
        } else if (msg_type == CLOCK_CALIBRATION) { // 校准任务
#ifdef CONFIG_ENABLE_RC_CALIBRATION
            rc32k_cali_process();
#endif
            continue;
        }
        
        alarm_process();
    }
}

errcode_t notify_alarm(int msg_type)
{
    int ret = osal_msg_queue_write_copy(g_alarm_queue, (void *)&msg_type, sizeof(int), 0);
    if (ret != OSAL_SUCCESS) {
        PMU_ALARM_PRINT_ERR("write msg to queue failed!\r\n");
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

static errcode_t alarm_queue_init(void)
{
    if (osal_msg_queue_create("alarm_queue", ALARM_MSG_COUNT, &g_alarm_queue, 0, sizeof(int)) != ERRCODE_SUCC) {
        PMU_ALARM_PRINT_ERR("create alarm queue fail\r\n");
        return ERRCODE_FAIL;
    }

    osal_task *task_handle = osal_kthread_create((osal_kthread_handler)alarm_task, NULL, "alarm_task", ALARM_TASK_SIZE);
    if (task_handle == NULL) {
        osal_msg_queue_is_full(g_alarm_queue);
        PMU_ALARM_PRINT_ERR("create alarm thread fail!\r\n");
        return ERRCODE_FAIL;
    }

    osal_kthread_set_priority(task_handle, ALARM_TASK_PRIO);

    return ERRCODE_SUCC;
}

static errcode_t alarm_queue_deinit(void)
{
    notify_alarm(-1);
    return ERRCODE_SUCC;
}

errcode_t uapi_alarm_register_callback(alarm_app_t app_type, alarm_callback_t callback)
{
    if (callback == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    if (app_type >= ALARM_APP_MAX_NUM) {
        return ERRCODE_INVALID_PARAM;
    }
    g_alarm_callbacks[app_type] = callback;
    return ERRCODE_SUCC;
}

errcode_t uapi_alarm_init(void)
{
    alarm_queue_init();

    if (!check_cfg_path_exist()) {
        return ERRCODE_ALARM_CFG_PATH_NOT_EXIST;
    }

    read_alarm_info();  // 从文件读取闹钟信息

    // 如果是因为alarm而唤醒，需要通知上层
    if (g_alarm_trigger) {
        g_alarm_trigger = false;
        PMU_ALARM_PRINT_INFO("trigger alarm!\r\n");
        notify_alarm(0);
    } else {
        exec_alarm_schedule(true);
    }
    g_alarm_inited = true;
    return ERRCODE_SUCC;
}

static void clear_alarm_callbacks(void)
{
    memset_s(g_alarm_callbacks, sizeof(g_alarm_callbacks), 0, sizeof(g_alarm_callbacks));
}

errcode_t uapi_alarm_deinit(void)
{
    uapi_alarm_disable();
    alarm_queue_deinit();
    clear_alarm_callbacks();
    g_alarm_inited = false;
    return ERRCODE_SUCC;
}

errcode_t uapi_alarm_create(alarm_handle_t *handle, uint32_t seconds)
{
    if (!g_alarm_inited) {
        return ERRCODE_ALARM_NOT_INIT;
    }
    if (seconds == 0 || !handle) {
        return ERRCODE_INVALID_PARAM;
    }

    alarm_handle_t new_handle = find_idle_handle(g_single_alarm_infos);
    if (new_handle < 0) {
        return ERRCODE_ALARM_NOT_ENOUGH;
    }

    uint32_t irq_sts = osal_irq_lock();
    g_single_alarm_infos[new_handle].alarm_timestamp = get_timestamp() + seconds;
    g_single_alarm_infos[new_handle].type = ALARM_SINGLE;
    g_single_alarm_infos[new_handle].status = ALARM_ENABLE;
    osal_irq_restore(irq_sts);

    *handle = new_handle + MAX_ALARM_NUM;  // 单次闹钟handle从MAX_ALARM_NUM开始
    return exec_alarm_schedule(false);
}

static bool check_alarm_time_valid(alarm_week_time_t time)
{
    if (time.hour >= 24 ||  // hour must < 24
        time.min >= 60) {   // min must < 60
        return false;
    }
    return true;
}

errcode_t uapi_alarm_period_create(alarm_handle_t *handle, alarm_week_time_t alarm_week_time)
{
    if (!g_alarm_inited) {
        return ERRCODE_ALARM_NOT_INIT;
    }
    if (!check_alarm_time_valid(alarm_week_time) || !handle) {
        return ERRCODE_INVALID_PARAM;
    }

    alarm_handle_t new_handle = find_idle_handle(g_period_alarm_infos);
    if (new_handle < 0) {
        return ERRCODE_ALARM_NOT_ENOUGH;
    }

    uint32_t irq_sts = osal_irq_lock();
    g_period_alarm_infos[new_handle].alarm_timestamp = 0;
    g_period_alarm_infos[new_handle].alarm_week_time = alarm_week_time;
    g_period_alarm_infos[new_handle].type = ALARM_WEEK;
    g_period_alarm_infos[new_handle].status = ALARM_ENABLE;
    osal_irq_restore(irq_sts);

    *handle = new_handle;
    return exec_alarm_schedule(true);
}

errcode_t uapi_alarm_close(alarm_handle_t handle)
{
    if (!g_alarm_inited) {
        return ERRCODE_ALARM_NOT_INIT;
    }
    if (handle < 0 || handle >= MAX_ALARM_HANDLE) {
        return ERRCODE_INVALID_PARAM;
    }
    uint32_t irq_sts = osal_irq_lock();
    if (handle < MAX_ALARM_NUM) {
        if (g_period_alarm_infos[handle].status != ALARM_ENABLE) {
            osal_irq_restore(irq_sts);
            return ERRCODE_ALARM_NOT_CLOSE;
        }
        g_period_alarm_infos[handle].status = ALARM_CLOSE;
    } else {
        if (g_single_alarm_infos[handle - MAX_ALARM_NUM].status != ALARM_ENABLE) {
            osal_irq_restore(irq_sts);
            return ERRCODE_ALARM_NOT_CLOSE;
        }
        g_single_alarm_infos[handle - MAX_ALARM_NUM].status = ALARM_IDLE;
    }
    osal_irq_restore(irq_sts);

    return exec_alarm_schedule(true);
}

errcode_t uapi_alarm_restart(alarm_handle_t handle)
{
    if (!g_alarm_inited) {
        return ERRCODE_ALARM_NOT_INIT;
    }
    if (handle < 0 || handle >= MAX_ALARM_NUM) {
        return ERRCODE_INVALID_PARAM;
    }

    uint32_t irq_sts = osal_irq_lock();
    if (g_period_alarm_infos[handle].status != ALARM_CLOSE) {
        osal_irq_restore(irq_sts);
        return ERRCODE_ALARM_NOT_RESTART;
    }
    g_period_alarm_infos[handle].status = ALARM_ENABLE;
    osal_irq_restore(irq_sts);

    return exec_alarm_schedule(true);
}

errcode_t uapi_alarm_delete(alarm_handle_t handle)
{
    if (!g_alarm_inited) {
        return ERRCODE_ALARM_NOT_INIT;
    }

    if (handle < 0 || handle >= MAX_ALARM_HANDLE) {
        return ERRCODE_INVALID_PARAM;
    }

    uint32_t irq_sts = osal_irq_lock();
    if (handle < MAX_ALARM_NUM) {
        if (g_period_alarm_infos[handle].status == ALARM_IDLE) {
            osal_irq_restore(irq_sts);
            return ERRCODE_ALARM_NOT_DEL;
        }
        g_period_alarm_infos[handle].status = ALARM_IDLE;
    } else {
        if (g_single_alarm_infos[handle - MAX_ALARM_NUM].status == ALARM_IDLE) {
            osal_irq_restore(irq_sts);
            return ERRCODE_ALARM_NOT_DEL;
        }
        g_single_alarm_infos[handle - MAX_ALARM_NUM].status = ALARM_IDLE;
    }
    osal_irq_restore(irq_sts);

    return exec_alarm_schedule(true);
}

errcode_t uapi_alarm_query(alarm_info_t *alarm_infos, uint32_t alarm_size)
{
    if (alarm_infos == NULL || alarm_size < MAX_ALARM_NUM) {
        return ERRCODE_INVALID_PARAM;
    }
    uint32_t irq_sts = osal_irq_lock();
    if (memcpy_s(alarm_infos, sizeof(alarm_info_t) * alarm_size, g_period_alarm_infos, sizeof(g_period_alarm_infos)) !=
        EOK) {
        PMU_ALARM_PRINT_INFO("memcpy_s failed. alarm_size=%lu\r\n", alarm_size);
    }
    if (alarm_size >= MAX_ALARM_HANDLE) {
        memcpy_s(alarm_infos + MAX_ALARM_NUM, sizeof(alarm_info_t) * MAX_ALARM_HANDLE, g_single_alarm_infos,
                 sizeof(g_single_alarm_infos));
    }
    osal_irq_restore(irq_sts);
    return ERRCODE_SUCC;
}

int alarm_irq_handler(int irq_num, const void *tmp)
{
    unused(tmp);
    PMU_ALARM_PRINT_INFO("alarm irq handler\r\n");

    // 通知队列
    notify_alarm(irq_num);
    return 0;
}

void alarm_sync_timestamp(uint64_t timestamp)
{
    if (!g_alarm_inited) {
        return;
    }
    PMU_ALARM_PRINT_INFO("cur time=%llu\r\n", timestamp);
    // 先关闭闹钟
    uapi_alarm_disable();

    // 单次闹钟更新
    uint32_t irq_sts = osal_irq_lock();
    for (int i = 0; i < MAX_ALARM_NUM; ++i) {
        if (g_single_alarm_infos[i].status != ALARM_ENABLE) {
            continue;
        }

        if (g_single_alarm_infos[i].alarm_timestamp <= timestamp) {
            g_single_alarm_infos[i].status = ALARM_IDLE;  // 更改时间后，如果单次闹钟已超时，则关闭不触发
            PMU_ALARM_PRINT_INFO("handle=%d was deleted due to expiration.\r\n", i);
        }
    }
    osal_irq_restore(irq_sts);

    // 周期闹钟更新
    exec_alarm_schedule(true);
}

void trigger_alarm(void)
{
    g_alarm_trigger = true;
}

void alarm_before_shipmode(void)
{
    if (!g_alarm_inited) {
        return;
    }
    // 清理单次闹钟
    PMU_ALARM_PRINT_INFO("clear single alarm and reschedule\r\n");
    uint32_t irq_sts = osal_irq_lock();
    memset_s(g_single_alarm_infos, sizeof(g_single_alarm_infos), 0, sizeof(g_single_alarm_infos));
    osal_irq_restore(irq_sts);
    exec_alarm_schedule(false);  // 不更新文件，只重新调度
}
