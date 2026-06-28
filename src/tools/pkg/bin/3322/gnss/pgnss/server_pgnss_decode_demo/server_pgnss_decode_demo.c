/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: pgnss decode demo on server
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <dlfcn.h>

#define XGNSS_LIB_PATH        "./libpgnss_decoder.so"    // 请自行配置解码库路径
#define XGNSS_SEED_FILE_PATH  "./demo_data/EE_7days.dat" // 请自行配置种子文件完整路径
#define XGNSS_OUTPUT_PATH_1   "./demo_data/output1/"     // 请自行配置解码后星历存放位置
#define XGNSS_OUTPUT_PATH_2   "./demo_data/output2/"     // 请自行配置解码后星历存放位置
#define HOUR_TO_SECONDS       3600
#define PARSE_HOURS           24

// 根据解码库接口文件，声明接口函数类型
typedef int32_t (*LibFuncGetValidTime)(const char *, uint32_t *, uint32_t *);
typedef int32_t (*LibFuncParseSeed)(const char *, const char *, uint32_t, uint32_t);

void PgnssGenerateDemo(bool mergeEphFiles)
{
    printf("\n[XGNSS] ==== parsing pgnss seed %s merge eph files. ====\n", mergeEphFiles ? "with" : "without");

    // 加载解码库
    void *handle = dlopen(XGNSS_LIB_PATH, RTLD_LAZY);
    char const *errStr = dlerror();
    if (handle == NULL) {
        printf("[XGNSS] error: module=%s\n%s\n", XGNSS_LIB_PATH, errStr != NULL ? errStr : "unknown");
        return;
    }
    printf("[XGNSS] load pgnss lib succeed.\n");

    // 获取接口函数
    LibFuncGetValidTime libGetValidTime = (LibFuncGetValidTime)dlsym(handle, "GetPgnssDataValidTime");
    LibFuncParseSeed libParseSeed;
    if (mergeEphFiles) {
        libParseSeed = (LibFuncParseSeed)dlsym(handle, "ParsePgnssDataByLessFiles");
    } else {
        libParseSeed = (LibFuncParseSeed)dlsym(handle, "ParsePgnssData");
    }
    printf("[XGNSS] load pgnss lib function succeed.\n");

    if (libGetValidTime == NULL || libParseSeed == NULL) {
        printf("[XGNSS] error: couldn't find symbol\n");
        dlclose(handle);
        return;
    }

    // 查询种子可支持预测的星历的起止unix时间
    uint32_t seedStartTime = 0;
    uint32_t seedEndTime = 0;
    int32_t ret = libGetValidTime(XGNSS_SEED_FILE_PATH, &seedStartTime, &seedEndTime);
    if (ret != 0) {
        printf("[XGNSS] error: get valid time error. ret=%d\n", ret);
        dlclose(handle);
        return;
    }
    printf("[XGNSS] get seeds valid time: start=%u, end=%u\n", seedStartTime, seedEndTime);

    // 获取当前UNIX时间（0时区）
#ifdef USE_REAL_TIME
    struct timeval currTime;
    gettimeofday(&currTime, NULL);
    uint32_t ts = currTime.tv_sec;
#else
    uint32_t ts = seedStartTime;
#endif

    // 设定时间，一次最多解码出24小时的星历，如需解码更多星历，请拆分多次调用解码函数
    int32_t hours = PARSE_HOURS;

    // 确保时间不超出种子可预测的有效时间，如果超出有效时间，可以考虑重新下载种子文件
    if (ts < seedStartTime || ts + hours * HOUR_TO_SECONDS > seedEndTime) {
        printf("[XGNSS] error: set time invalid. set start=%u, set end=%u\n", ts, ts + hours * HOUR_TO_SECONDS);
        dlclose(handle);
        return;
    }

    // 解码种子文件，得到解码后的星历
    printf("[XGNSS] start parsing pgnss seed ...\n");
    if (mergeEphFiles) {
        ret = libParseSeed(XGNSS_SEED_FILE_PATH, XGNSS_OUTPUT_PATH_1, ts, hours);
    } else {
        ret = libParseSeed(XGNSS_SEED_FILE_PATH, XGNSS_OUTPUT_PATH_2, ts, hours);
    }

    if (ret != 0) {
        printf("[XGNSS] Parse Pgnss Data failed, err=%d\n", ret);
    } else {
        printf("[XGNSS] Parse Pgnss Data succeed.\n");
    }
    dlclose(handle);
}

int main(int argc __attribute__((unused)), char *argv[] __attribute__((unused)))
{
    // PGNSS数据解析（合并文件输出方式，可减少生成的星历文件数量），详细说明见XGNSS开发指南
    PgnssGenerateDemo(true);

    // PGNSS数据解析（多文件输出方式），详细说明见XGNSS开发指南
    PgnssGenerateDemo(false);
}
