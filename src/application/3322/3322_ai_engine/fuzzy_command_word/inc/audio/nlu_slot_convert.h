/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: NLU_SLOT_CONVERT_H
 * Author: zcl
 * Create: 2025-10-15
 */

#ifndef NLU_SLOT_CONVERT_H
#define NLU_SLOT_CONVERT_H

#include <stdint.h>
#include "nlu_postproc_tab.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AUDIO_SLOT_SUCCESS 0
#define AUDIO_SLOT_FAILURE (-1)
#define AUDIO_SLOT_TYPE_FAILURE (-2)
#define AUDIO_MEM_APPLY_FAIL   (0x50000001)
#define AUDIO_IN_BUFFER_NULL   (0x50000002)
#define AUDIO_HANDLE_PTR_NULL  (0x50000003)


#define AUDIO_NLU_MAX_OUT_SIZE 100
#define AUDIO_SLU_PINYIN_MAX_LEN 25 // 20
#define AUDIO_SLU_INTENT_NUM 104
#define AUDIO_SLU_SLOT_NUM 16
#define AUDIO_SLU_SLOT_MAX_LABEL_LEN 30
#define AUDIO_SLU_SLOT_MAX_FILLER_SIZE 50
#define AUDIO_SLOT_MAX_FIL_SIZE 50
#define AUDIO_MAX_STRING 400
#define AUDIO_SLOT_VALUE_MAX_SIZE 200

#define NO_MATCH 255
#define INTENT_NUM_THRESHOLD_1 81
#define INTENT_NUM_THRESHOLD_2 82
#define INTENT_NUM_THRESHOLD_3 83
#define INTENT_NUM_THRESHOLD_4 84
#define VAR_NUM_0 0
#define VAR_NUM_1 1
#define VAR_NUM_2 2
#define VAR_NUM_3 3
#define VAR_NUM_4 4
#define VAR_NUM_5 5
#define VAR_NUM_6 6
#define VAR_NUM_7 7
#define VAR_NUM_8 8
#define VAR_NUM_9 9
#define VAR_NUM_10 10
#define SLOT_TAG_COUNT 16
#define RETURN_NOT_FOUND 1000
#define TIME_QUARTER 15
#define TIME_HALF 30
#define TIME_THREE_QUARTER 45
#define TIME_HOUR 60


typedef struct {
    uint8_t date;
    char* date_filler;
    uint8_t am_or_pm;  /* 1:am, 2:pm */
    char* am_or_pm_filler;
    uint8_t alarm_hour;
    uint8_t alarm_minute;
} SlotDateTime;

typedef struct {
    uint32_t hours; /* 0~99999,支持x.5说法 */
    uint32_t minutes;
    uint32_t seconds;
}SlotTimeDuration;

typedef struct {
    float amount;
    uint8_t energy_unit; /* 固定为kcal */
    char* energy_unit_filler;
}SlotEnergy;


typedef struct {
    uint32_t telnum;
}SlotTelNumber;

typedef struct {
    float amount;
    uint8_t dis_unit;  /* 1:km, 2:step, 3:lap, 4:m */
    char* dis_unit_filler;
}SlotDistance;


/* 0:null, 1:datetime, 2:time_duration, 3:energy, 4:distance, 5:telnum */
typedef struct {
    SlotDateTime datetime_stru;
    SlotTimeDuration timedur_stru;
    SlotEnergy energy_stru;
    SlotDistance distance_stru;
    SlotTelNumber telnum_stru;
    float amount;
}NluSlot;

typedef struct {
    char **asr_text;
    char **bio_label;
    char  *intent_pred;
    char  *wav_id;
    uint8_t  intent_num;
    uint8_t  intent_class;
    uint8_t  slot_type; /* 0:null, 1:datetime, 2:time_duration, 3:energy, 4:distance, 5:telnum */
    NluSlot  *nlu_slot;
    uint8_t  sentence_len;
    char *nlu_scratch;
} nlu_stru;


uint8_t map_intent_to_num(char* intent_en);
uint8_t classify_intent(uint8_t intent_num);

int split(char* string, char* pattern, char* substr1, char* substr2);
int slu_slot_convert_init(void **handle, int has_wav_id);
int slu_read_nlu_output(void *handle, char *in, int has_wav_id);
int slu_slot_convert_process(void *handle);
int slu_slot_convert_deinit(void *handle, int has_wav_id);


#ifdef __cplusplus
}
#endif

#endif