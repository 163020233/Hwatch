/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Create Date : 2025-10
 */

#ifndef UTF8_TO_GBK
#define UTF8_TO_GBK

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * UTF-8 → GBK 转换（内部申请内存）
 * @param utf8: 输入UTF-8字符串
 * @param outGbk: 输出GBK字符串指针（需要free）
 * @return 成功返回true，失败返回false
 */

bool ConvertUtf8ToGbk(const char* utf8, char** outGbk);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif