/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: lowpower header file for SELiteOS.
 */

#ifndef LOWPOWER_H
#define LOWPOWER_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void lowpower_resume(void);
void lowpower_suspend(void);

void tee_lowpower_resume(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LOWPOWER_H */
