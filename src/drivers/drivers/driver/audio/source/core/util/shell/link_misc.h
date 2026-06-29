/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: link misc
 * Author: audio
 */

#ifndef LINK_MISC_H_
#define LINK_MISC_H_
#include "audio_type.h"

/**
 * @brief: use this function to translate the string to argv mode
 *
 * @param[in]:string , the string to translate
 * @param[inout]:argc, the sizeof argv and how many prarameters has been splited
 * @param[inout]:argv, used to storage the parameters splited
 *
 * @retrun:how many parameters geted
 *
 * */
td_s32 string_to_arg(td_s32 *argc, const td_char *argv[], td_char *string);

#endif /* LINK_MISC_H_ */
