/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Libc Locale
 * Author: RTOS_Lite项目组
 * Create: 2025-03-22
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <locale.h>
#include <string.h>
#include <stdlib.h>
#include "locale_impl.h"
#include "libc.h"
#include "lock.h"

#ifdef __LITEOS__
volatile pthread_mutex_t __locale_lock[1];
#else
volatile int __locale_lock[1];
#endif

LITE_OS_SEC_TEXT const struct __locale_map *__get_locale(int cat, const char *val)
{
    static void *volatile loc_head = 0;
    const struct __locale_map *p;
    struct __locale_map *new = 0;
    size_t n;

    if (val == NULL) {
        return NULL;
    }

    if (!(*val)) {
        val = "C.UTF-8";
    }

    for (n = 0; n < LOCALE_NAME_MAX && val[n] && val[n] != '/'; n++) {}

    if (val[0] == '.' || val[n]) {
        val = "C.UTF-8";
    }

    if ((val[0] == 'C' && !val[1]) || !strcmp(val, "C.UTF-8") || !strcmp(val, "POSIX")) {
        if (cat == LC_CTYPE && val[1] == '.') {
            return (void *)&__c_dot_utf8;
        }
        return 0;
    }

    for (p = loc_head; p; p = p->next) {
        if (!strcmp(val, p->name)) {
            return p;
        }
    }

    if (new = malloc(sizeof(*new))) {
        new->map = __c_dot_utf8.map;
        new->map_size = __c_dot_utf8.map_size;
        (void)memcpy_s(new->name, sizeof(new->name), val, n);
        new->name[n] = 0;
        new->next = loc_head;
        loc_head = new;
    }

    if (!new && cat == LC_CTYPE) {
        new = (void *)&__c_dot_utf8;
    }

    return new;
}
