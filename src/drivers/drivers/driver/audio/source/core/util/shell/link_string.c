/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: link string
 * Author: audio
 */

#include <string.h>
#include "audio_type.h"

/* this function is used to format the td_char string to the argc mode */
/* this function will changed the original string, used it carefully */
/* return how many arguments has been */
td_s32 string_to_arg(td_s32 *argc, const td_char *argv[], td_char *string)
{
    td_s32 argvlen = 0;
    td_s32 paramnum = 0;
    td_char *tmp = TD_NULL;
    td_char bak;
    td_s32 len;

    argvlen = *argc;
    *argc = paramnum;
    if (TD_NULL == string) {
        return paramnum;
    }

    /* use the '\0' to replace the ' ' */
    len = strlen(string);
    tmp = string;
    while (tmp < (string + len)) {
        if (*tmp == ' ') {
            *tmp = '\0';
        }
        tmp++;
    }

    bak = '\0';
    tmp = string;
    while (tmp < (string + len)) {
        if ((*tmp != '\0') && (bak == '\0')) {
            if (paramnum < argvlen) {
                argv[paramnum] = tmp;
                paramnum++;
            }
        }
        bak = *tmp;
        tmp++;
    }

    *argc = paramnum;

    return paramnum;
}

/*********************************************************************
* Convert an unsigned hexadecimal or decimal string into an integer number
*/
td_s32 string2int(const td_char* str, td_u32* val)
{
    td_s32 hex;
    td_s32 len;
    td_s32 value = 0;
    const td_char* tmp = str;

    len = strlen(str);
    /* 10:no more than 32bit */
    if ((len > 10) || (len == 0)) {
        return -1;
    }

    if (len > 2 && ('0' == tmp[0]) && (('x' == tmp[1]) || ('X' == tmp[1]))) { /* 2:two characters "0x" */
        /* hex convert */
        tmp += 2; /* 2:Addr offset by two characters */
        while ('\0' != *tmp) {
            value = value << 4; /* 4 digits per digit */
            if (*tmp >= '0' && *tmp <= '9') {
                hex = *tmp - '0';
            } else if (*tmp >= 'a' && *tmp <= 'f') {
                hex = *tmp - 'a' + 10;  /* 10:Numerical value */
            } else if (*tmp >= 'A' && *tmp <= 'F') {
                hex = *tmp - 'A' + 10; /* 10:Numerical value */
            } else {
                return -1;
            }
            value |= hex;
            tmp++;
        }
    } else {
        /* oct convert */
        while ('\0' != *tmp) {
            value *= 10; /* 10:arithmetic carry */
            if (*tmp >= '0' && *tmp <= '9') {
                value += *(tmp++) - '0';
            } else {
                return -1;
            }
        }
    }

    *val = value;
    return 0;
}
