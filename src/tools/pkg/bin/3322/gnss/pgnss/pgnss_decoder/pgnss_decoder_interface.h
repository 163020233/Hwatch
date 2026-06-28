/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: pgnss include with pgnss data decode module
 */

#ifndef PGNSS_DECODER_INTERFACE_H
#define PGNSS_DECODER_INTERFACE_H

#include <stdint.h>

#ifndef WIN32
#define EXPORT_API __attribute__((visibility("default")))
#else
#define EXPORT_API __declspec(dllexport)
#endif

/*
 * 函数功能：获取种子文件可预测的起止时间
 * 入参：filePath：种子文件的完整路径，包括路径和文件名
 * 出参：startTimestamp：种子文件可预测的开始UTC时间对应的unix时间戳
 *      endTimestamp：种子文件可预测的结束UTC时间对应的unix时间戳
 */
EXPORT_API int32_t GetPgnssDataValidTime(const char *filePath, uint32_t *startTimestamp, uint32_t *endTimestamp);

/*
 * 函数功能：获取跳秒时间
 * 入参：filePath：种子文件的完整路径，包括路径和文件名
 * 返回值：LeapSecond跳秒
 */
EXPORT_API int32_t GetLeapSecond(const char *filePath);

/*
 * 函数功能：读取PGNSS种子文件，解析PGNSS辅助数据，扩展星历，保存星历文件；
 *         每一份星历文件可使用前后两小时，两份星历文件间隔两小时，支持一次扩展多份星历文件
 * 入参：filePath：种子文件的完整路径，包括路径和文件名
 *      outputDir：扩展的辅助数据存放路径
 *      timestamp：开始预测时间对应的unix时间戳，可取用当前时间，也可自行设置时间，单位：秒
 *      hours：解析X小时星历，调用者需保证种子文件中从timestamp开始有X小时的有效星历
 */
EXPORT_API int32_t ParsePgnssData(const char *filePath, const char *outputDir, uint32_t timestamp, uint32_t hours);

/*
 * 函数功能：读取PGNSS种子文件，解析PGNSS辅助数据，扩展星历，保存星历文件；
 *          与ParsePgnssData不同，每次解析生成三份文件，分别为GLONASS星历、除GLONASS外其余星历以及其余辅助信息
 *          每一份星历文件可使用前后两小时，两份星历文件间隔两小时，支持一次扩展多份星历文件
 * 入参：与ParsePgnssData一致
 *      filePath：种子文件的完整路径，包括路径和文件名
 *      outputDir：扩展的辅助数据存放路径
 *      timestamp：开始预测时间对应的unix时间戳，可取用当前时间，也可自行设置时间，单位：秒
 *      hours：解析X小时星历，调用者需保证种子文件中从timestamp开始有X小时的有效星历
 */
EXPORT_API int32_t ParsePgnssDataByLessFiles(const char *filePath, const char *outputDir, uint32_t timestamp, uint32_t hours);

/*
 * 函数功能：返回pgnss解码库的版本号
 * 入参：无
 * 返回值：版本号对应字符串
 */
EXPORT_API const char *GetPgnssVersion(void);

#endif
