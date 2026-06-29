/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: Boot string function implementation.
 *
 * Create: 2012-12-22
 */

#include <string.h>

int strcmp(const char *l, const char *r)
{
	for (; *l==*r && *l; l++, r++);
	return *(unsigned char *)l - *(unsigned char *)r;
}
