/*
 * Copyright (c) 2019-2020 Huawei Technologies Co., Ltd.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "lwip_public.h"
#include "lwip_private.h"
#include "lwip/tcpip.h"
#include "lwip/netifapi.h"

#if LWIP_LOWPOWER
#include "lwip/lowpower.h"

static int g_low_power_vote[LOW_MAX_MODULE] = {0};

void lwip_lowpower_inc(lowpower_module mod)
{
#if LWIP_TCPIP_CORE_LOCKING
    if (mod >= LOW_MAX_MODULE || mod < 0) {
        return;
    }

    LOCK_TCPIP_CORE();
    g_low_power_vote[mod]++;
    for (uint8_t index = 0; index < LOW_MAX_MODULE; index++) {
        if (g_low_power_vote[index] < 0) { /* some module prevent entering low power mode */
            UNLOCK_TCPIP_CORE();
            return;
        }
    }
    set_lowpower_mod(LOW_TMR_LOWPOWER_MOD);
    UNLOCK_TCPIP_CORE();
#else
    netifapi_enable_lowpower();
#endif
}

void lwip_lowpower_dec(lowpower_module mod)
{
#if LWIP_TCPIP_CORE_LOCKING
    if (mod >= LOW_MAX_MODULE || mod < 0) {
        return;
    }

    LOCK_TCPIP_CORE();
    if (g_low_power_vote[mod] > 0) {
        g_low_power_vote[mod]--;
    }

    if (g_low_power_vote[mod] > 0) {
        UNLOCK_TCPIP_CORE();
        return;
    }
    set_lowpower_mod(LOW_TMR_NORMAL_MOD);
    UNLOCK_TCPIP_CORE();
#else
    netifapi_disable_lowpower();
#endif
}

#endif