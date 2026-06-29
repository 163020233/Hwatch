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

#include "locale_impl.h"
#include "pprivate.h"
#include "libc_pri.h"

LITE_OS_SEC_TEXT static locale_t __uselocale(locale_t new)
{
#if defined(LOSCFG_COMPAT_POSIX) && defined(LC_GLOBAL_LOCALE) && defined(LOSCFG_COMPAT_POSIX_THREAD_LOCALE)
    _pthread_data *self = __pthread_self();
    locale_t old = self->locale;

    if (new) {
        self->locale = new == LC_GLOBAL_LOCALE ? g_locale : new;
    }

    return old == g_locale ? LC_GLOBAL_LOCALE : old;
#elif defined(LOSCFG_COMPAT_POSIX) && !defined(LOSCFG_COMPAT_POSIX_THREAD_LOCALE)
    locale_t old = g_locale;
    if (new) {
        g_locale = new;
    }
    return old;
#else
    return NULL;
#endif
}

weak_alias(__uselocale, uselocale);
