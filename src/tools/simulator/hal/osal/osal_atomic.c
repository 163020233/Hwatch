/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: OS Abstract Layer.
 */

#include "osal_atomic.h"

int osal_atomic_read(osal_atomic *atomic)
{
    return InterlockedExchangeAdd(&atomic->value, 0);
}

void osal_atomic_set(osal_atomic *atomic, int value)
{
    InterlockedExchange(&atomic->value, value);
}

int osal_atomic_inc_return(osal_atomic *atomic)
{
    return InterlockedIncrement(&atomic->value);
}

int osal_atomic_add_return(osal_atomic *atomic, int count)
{
    return InterlockedAdd(&atomic->value, count);
}

int osal_atomic_dec_return(osal_atomic *atomic)
{
    return InterlockedDecrement(&atomic->value);
}

void osal_atomic_sub(osal_atomic *atomic, unsigned int count)
{
    InterlockedExchangeAdd(&atomic->value, -count);
}

void osal_atomic_add(osal_atomic *atomic, int count)
{
    InterlockedAdd(&atomic->value, count);
}

int osal_atomic_dec_and_test(osal_atomic *atomic)
{
    return InterlockedDecrement(&atomic->value) == 0;
}

int osal_atomic_inc_and_test(osal_atomic *atomic)
{
    return InterlockedIncrement(&atomic->value) == 0;
}

int osal_atomic_inc_not_zero(osal_atomic *atomic)
{
    LONG result = InterlockedIncrement(&atomic->value);
    if (result != 0) {
        return 1;
    } else {
        InterlockedDecrement(&atomic->value);
        return 0;
    }
}

void osal_atomic_inc(osal_atomic *atomic)
{
    InterlockedIncrement(&atomic->value);
}

void osal_atomic_dec(osal_atomic *atomic)
{
    InterlockedDecrement(&atomic->value);
}