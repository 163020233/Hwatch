/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: voice assistant service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-08， Create file. \n
 */
#ifndef VOICE_ASSISTANT_MANAGER_H
#define VOICE_ASSISTANT_MANAGER_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


typedef enum {
    VOICE_ASSISTANT_DISABLE = 0,
    VOICE_ASSISTANT_ENABLE,
    VOICE_ASSISTANT_BUTT
} voice_assistant_switch_t;

void set_voice_assistant_switch_ext(uint8_t mode);
uint8_t get_voice_assistant_switch_ext(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif