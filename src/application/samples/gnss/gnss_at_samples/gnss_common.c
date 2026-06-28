/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: gnss common
 */

#include "gnss_common.h"
#include <stdbool.h>
#include <dirent.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/stat.h>
#include "securec.h"
#include "gnss_log.h"
#include "tcxo.h"

#define ONE_CENTURY_YEARS           100
#define FOUR_CENTURY_YEARS          400
#define LEAP_YEAR_LENGTH            4
#define UNIX_TIME_START_YEAR        1970    // start year of unix time
#define TIME_UNIT                   1000
#define THE_SECOND_MONTH            2
#define SECONDS_PER_MIN             (60)
#define SECONDS_PER_HOUR            (60 * SECONDS_PER_MIN)
#define SECONDS_PER_DAY             (24 * SECONDS_PER_HOUR)
#define SECONDS_PER_NORMAL_YEAR     (365 * SECONDS_PER_DAY)
#define MONTH_NUM_FOR_DAYS          13
#define WEEK_SECONDS_NUMBER         604800
#define DAY_SECONDS_NUMBER          86400
#define HOUR_SECONDS_NUMBER         3600
#define WEEK_NUMBER_IN_ROUND        1024
#define HOUR_MINUTES_NUMBER         60
#define WEEK_DAYW_NUMBER            7
#define NORMAL_YEAR_DAY_NUMBER      365
#define START_YEAR_OF_GPST          1980      // GPS时间起算的原点定义在1980年1月6日世界协调时
#define START_DAY_OF_GPST           6
#define LEAP_YEAR_DAY_NUMBER        366
#define GPST_START_YEAR_RES_DAYS    (LEAP_YEAR_DAY_NUMBER - START_DAY_OF_GPST)
#define SECOND_STR_LEN 16
#define DAY_STR_LEN 16
#define FIX_STR_LEN 5
#define RMC_TALKER_LEN 3
#define NMEA_HEAD_LEN 3

/** days per month on normal and leap year */
int32_t g_normMonthDays[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
int32_t g_leapMonthDays[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

/* 计算命令内容校准和 */
uint16_t get_check_sum(uint8_t *buff, uint16_t len)
{
    uint16_t checksum = 0;
    uint16_t i = 0;
    while (i++ < len) {
        checksum += *buff;
        buff++;
    }
    return checksum;
}

void get_sys_utc_time(GnssUtcTime* utcTime)
{
    // 获取当前时间
    time_t now = time(NULL);
    struct tm* utc = gmtime(&now);  // 转换为 UTC 时间
    if (utc == NULL) {
        GNSS_LOG_ERROR("[GNSS ERROR] get time error");
    }

    // 获取毫秒部分
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint16_t ms = (uint16_t)(tv.tv_usec / 1000);  // 将微秒转换为毫秒

    // 填充结构体
    utcTime->year = 1900 + utc->tm_year;  // tm_year 是从 1900 年开始的年数
    utcTime->month = 1 + utc->tm_mon;     // tm_mon 是从 0 开始的月份
    utcTime->day = utc->tm_mday;          // tm_mday 是月份中的日期
    utcTime->hour = utc->tm_hour;         // tm_hour 是小时
    utcTime->minute = utc->tm_min;        // tm_min 是分钟
    utcTime->second = utc->tm_sec;        // tm_sec 是秒
    utcTime->ms = ms;                     // 毫秒部分
    utcTime->valid = 1;                   // 设置为有效
}

/* 获取begin和end年份之间的闰年数 */
static int32_t get_leap_year_num(uint16_t begin, uint16_t end)
{
    if (end <= begin || end < 1) {
        return 0;
    }
    int32_t numPer400Year = (end - 1) / FOUR_CENTURY_YEARS - begin / FOUR_CENTURY_YEARS;
    int32_t numPer100Year = (end - 1) / ONE_CENTURY_YEARS - begin / ONE_CENTURY_YEARS;
    int32_t numPer4Year = (end - 1) / LEAP_YEAR_LENGTH - begin / LEAP_YEAR_LENGTH;
    return (numPer400Year + numPer4Year - numPer100Year);
}

/* 闰年判断 */
static int32_t check_if_leap_year(uint16_t year)
{
    if ((year % ONE_CENTURY_YEARS) == 0) {
        if ((year % FOUR_CENTURY_YEARS) == 0) {
            return 1;
        }
    } else {
        if ((year % LEAP_YEAR_LENGTH) == 0) {
            return 1;
        }
    }
    return 0;
}

/* 将日历时(年月日时分秒) 转换为 unix时间(1970年1月1日起的时间秒数) */
int32_t utc_to_unixtime(GnssUtcTime utcTime, uint32_t *unixTime)
{
    if (unixTime == NULL) {
        GNSS_LOG_ERROR("[GNSS ERROR] [utc_to_unixtime] null pointer!\n");
        return 0;
    }

    // 判断时间有效性（一年最多12个月）
    if (utcTime.month > 12 || utcTime.valid == 0) {
        GNSS_LOG_ERROR("[GNSS ERROR] [utc_to_unixtime] utcTime invalid!\n");
        return 0;
    }

    // 判断是否为闰年
    int32_t *monthDays = (check_if_leap_year(utcTime.year) == 1) ? g_leapMonthDays : g_normMonthDays;

    // 计算从1970.01.01开始的unix 秒数
    uint32_t timestamp = 0;
    timestamp += (uint32_t)(utcTime.year - UNIX_TIME_START_YEAR) * SECONDS_PER_NORMAL_YEAR;
    timestamp += (uint32_t)get_leap_year_num(UNIX_TIME_START_YEAR, utcTime.year) * SECONDS_PER_DAY;

    for (int32_t i = 1; i < utcTime.month; i++) {
        timestamp += (uint32_t)monthDays[i - 1] * SECONDS_PER_DAY;
    }

    timestamp += (uint32_t)(utcTime.day - 1) * SECONDS_PER_DAY;
    timestamp += (uint32_t)utcTime.hour * SECONDS_PER_HOUR;
    timestamp += (uint32_t)utcTime.minute * SECONDS_PER_MIN;
    uint32_t timestampSec = (uint32_t)(utcTime.second + utcTime.ms / TIME_UNIT);

    *unixTime = timestamp + timestampSec;
    return 1;
}

static void adjust_first_day_on_leap_year(int32_t *firstDayOfMonth, const int32_t length, const int32_t year)
{
    int32_t i;
    if (check_if_leap_year(year) == 1) {
        for (i = THE_SECOND_MONTH + 1; i < length; i++) {
            firstDayOfMonth[i]++;
        }
    }
}

static int32_t get_days_from_1980(const GnssUtcTime *utc, int32_t year, int32_t yearDays)
{
    int32_t days = GPST_START_YEAR_RES_DAYS + 1;  // 361 days from Jan 6 to Dec 31, 1980
    int32_t i;
    // Add on days for whole year from 1981 to year-1
    for (i = START_YEAR_OF_GPST + 1; i < year; i++) {
        days += NORMAL_YEAR_DAY_NUMBER;
        if (check_if_leap_year(i)) {
            days++;
        }
    }
    // Plus days in the current year
    days += ((int32_t)utc->day + yearDays) - 2;

    return days;
}

static void utc_to_gps_time(GnssUtcTime *utc, GpsTime *gps)
{
    int32_t year = (int32_t)utc->year;
    int32_t month = (int32_t)utc->month;
    int32_t firstDayOfMonth[MONTH_NUM_FOR_DAYS] = { 0, 1, 32, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 };
    adjust_first_day_on_leap_year(firstDayOfMonth, MONTH_NUM_FOR_DAYS, year);
    if (year < 1980 + 1 || year > 2100 || month >= MONTH_NUM_FOR_DAYS) {
        gps->valid = 0;
        return;
    }

    int32_t days = get_days_from_1980(utc, year, firstDayOfMonth[month]);
    double seconds = utc->second + (double)utc->minute * HOUR_MINUTES_NUMBER +
                     (double)utc->hour * HOUR_SECONDS_NUMBER + (double)(days % WEEK_DAYW_NUMBER) * DAY_SECONDS_NUMBER;
    // adjust gps leap seconds
    seconds += 18;  // 跳秒按照默认18s
    if (seconds >= WEEK_SECONDS_NUMBER) {
        seconds -= WEEK_SECONDS_NUMBER;
        days++;
    }

    // GPS Week number starting from 0 and Moudulo 1024
    int32_t weeks = days / WEEK_DAYW_NUMBER;
    gps->weekRound = (uint32_t)(weeks / WEEK_NUMBER_IN_ROUND);
    gps->weekNumber = weeks % WEEK_NUMBER_IN_ROUND;
    gps->valid = 1;
    gps->weekSeconds = seconds;
}

static void extract_utc_time(const char* nmea, GnssUtcTime *utc_time)
{
    if ((nmea == NULL) || (strlen(nmea) <= NMEA_HEAD_LEN + RMC_TALKER_LEN) ||
        (strncmp(nmea + NMEA_HEAD_LEN, "RMC", RMC_TALKER_LEN) != 0)) {
        return;
    }

    char* data_copy = (char *)malloc(strlen(nmea) + 1);
    if (data_copy == NULL) {
        return;
    }
    errno_t ret = strcpy_s(data_copy, strlen(nmea) + 1, nmea);
    if (ret != EOK) {
        GNSS_LOG_ERROR("copy nmea fail");
        free(data_copy);
        return;
    }

    int count = 0;
    char sec_str[SECOND_STR_LEN] = { 0 };
    char day_str[DAY_STR_LEN] = { 0 };
    char fix_str[FIX_STR_LEN] = { 0 };

    char* token = strtok(data_copy, ",");
    while (token != NULL) {
        // RMC语句中时间在第1个字段
        if (count == 1) {
            // 时间格式：hhmmss.sss
            ret = strcpy_s(sec_str, SECOND_STR_LEN, token);
            if (ret != EOK) {
                GNSS_LOG_ERROR("copy nmea fail");
                free(data_copy);
                return;
            }
        } else if (count == 2) {  // 第2个字段表示是否定位
            ret = strcpy_s(fix_str, FIX_STR_LEN, token);
            if (ret != EOK) {
                GNSS_LOG_ERROR("copy nmea fail");
                free(data_copy);
                return;
            }
            if (fix_str[0] != 'A') {  // 没有定位时间无意义，直接返回
                utc_time->valid = 0;
                free(data_copy);
                return;
            }
        } else if (count == 9) { // 第9个字段表示日期
            // 日期格式：hhmmss
            ret = strcpy_s(day_str, DAY_STR_LEN, token);
            if (ret != EOK) {
                GNSS_LOG_ERROR("copy nmea fail");
                free(data_copy);
                return;
            }
        }
        count++;
        token = strtok(NULL, ",");
    }
    utc_time->hour = (sec_str[0] - '0') * 10 + sec_str[1] - '0';
    utc_time->minute = (sec_str[2] - '0') * 10 + sec_str[3] - '0';
    utc_time->second = (sec_str[4] - '0') * 10 + sec_str[5] - '0';
    utc_time->ms = (sec_str[7] - '0') * 100 + (sec_str[8] - '0') * 10;

    utc_time->day = (day_str[0] - '0') * 10 + day_str[1] - '0';
    utc_time->month = (day_str[2] - '0') * 10 + day_str[3] - '0';
    utc_time->year = (day_str[4] - '0') * 10 + day_str[5] - '0' + 2000;
    utc_time->valid = 1;
    free(data_copy);
}

void extra_time_from_nmea(const char* nmea, GpsTime *gps_time)
{
    GnssUtcTime utc_time = { 0 };
    extract_utc_time(nmea, &utc_time);
    if (utc_time.valid == 1) {
        utc_to_gps_time(&utc_time, gps_time);
    }
}
