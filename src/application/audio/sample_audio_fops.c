/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: sample audio file ops
 * Author: audio
 */

#include "sample_audio_fops.h"

#ifdef SAP_FOPS_TYPE_DUMMY
FILE *sap_fopen(const char *name, const char *type)
{
    (void)(name);
    (void)(type);

    return NULL;
}

int sap_fclose(FILE *f)
{
    (void)(f);

    return 0;
}

size_t sap_fread(void *v, size_t size, size_t n, FILE *f)
{
    (void)(v);
    (void)(size);
    (void)(n);
    (void)(f);

    return 0;
}

size_t sap_fwrite(const void *v, size_t size, size_t n, FILE *f)
{
    (void)(v);
    (void)(size);
    (void)(n);
    (void)(f);

    return 0;
}

td_s32 sap_fseek(FILE *f, td_slong offset, td_s32 fromwhere)
{
    (void)(f);
    (void)(offset);
    (void)(fromwhere);
    return 0;
}

td_slong sap_ftell(FILE *f)
{
    (void)(f);
    return 0;
}

void sap_rewind(FILE *f)
{
    (void)(f);
}

#ifdef SINGLE_CORE_DEPLOY
FILE *fopen(const char *filename, const char *mode)
{
    (void)(filename);
    (void)(mode);
    return (FILE *)0;
}

int fclose(FILE *stream)
{
    (void)(stream);
    return 0;
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    (void)(ptr);
    (void)(size);
    (void)(nmemb);
    (void)(stream);
    return 0;
}
#endif
#endif
