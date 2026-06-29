/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: sample audio file ops
 * Author: audio
 */

#ifndef __SAMPLE_AUDIO_FOPS_H__
#define __SAMPLE_AUDIO_FOPS_H__

#include <stdio.h>
#include "td_type.h"
#include "sample_audio_interface.h"

#ifdef SAP_FOPS_TYPE_DUMMY
FILE *sap_fopen(const char *name, const char *type);
int sap_fclose(FILE *f);
size_t sap_fread(void *v, size_t size, size_t n, FILE *f);
size_t sap_fwrite(const void *v, size_t size, size_t n, FILE *f);
td_s32 sap_fseek(FILE *f, td_slong offset, td_s32 fromwhere);
td_slong sap_ftell(FILE *f);
void sap_rewind(FILE *f);
#else
#define sap_fopen fopen
#define sap_fclose fclose
#define sap_fread fread
#define sap_fwrite fwrite
#define sap_fseek fseek
#define sap_ftell ftell
#define sap_rewind rewind
#endif

#endif /* __SAMPLE_AUDIO_FOPS_H__ */
