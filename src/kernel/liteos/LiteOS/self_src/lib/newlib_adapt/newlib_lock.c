/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: 适配newlib锁接口。
 * Author: LibCK项目组
 * Create: 2025-04-15
 */
#include "newlib_external.h"
#include "sre_attr_external.h"
#include "sre_log_external.h"

/*
 * 描述: 初始化自旋锁
 */
OS_SEC_L4_TEXT void spin_lock_init(spin_lock_t *sl)
{
    if (UNLIKELY(sl == NULL)) {
        OS_LOG_E("spin lock init failed, ptr is null");
        return;
    }

    sl->locked = 0;
}

/*
 * 描述: 请求自旋锁
 */
OS_SEC_TEXT void spin_lock_acquire(spin_lock_t *sl)
{
    OsSplLock((volatile uintptr_t *)(uintptr_t)&sl->locked);
}

/*
 * 描述: 释放自旋锁
 */
OS_SEC_TEXT void spin_lock_release(spin_lock_t *sl)
{
    OsSplUnlock((volatile uintptr_t *)(uintptr_t)&sl->locked);
}

/*
 * 描述: 关闭自旋锁
 */
OS_SEC_L4_TEXT void spin_lock_close(spin_lock_t *sl)
{
    (void)sl;
}

/*
 * 描述: 初始化可重入自旋锁
 */
OS_SEC_L4_TEXT void spin_lock_init_recursive(spin_lock_recursive_t *slr)
{
    if (UNLIKELY(slr == NULL)) {
        OS_LOG_E("recursive spin lock init failed, ptr is null");
        return;
    }

    spin_lock_init(&slr->sl);
    slr->user = -1;
    slr->count = 0;
}

/*
 * 描述: 请求可重入自旋锁
 */
OS_SEC_TEXT void spin_lock_acquire_recursive(spin_lock_recursive_t *slr)
{
    int id;

    id = (int)OsGetCoreId();
    if (slr->user != id) {
        spin_lock_acquire(&slr->sl);
        slr->user = id;
    }

    if (UNLIKELY(slr->count >= SPIN_LOCK_RECURSIVE_COUNT_UPPER_LIMIT)) {
        OS_LOG_E("recursive spin lock acquire failed, count reached upper limit, user=0x%x", slr->user);
        return;
    }

    slr->count++;
}

/*
 * 描述: 释放可重入自旋锁
 */
OS_SEC_TEXT void spin_lock_release_recursive(spin_lock_recursive_t *slr)
{
    int id;

    id = (int)OsGetCoreId();
    if (UNLIKELY(slr->user != id)) {
        OS_LOG_E("recursive spin lock release failed, user not match, user=0x%x, id=0x%x", slr->user, id);
        return;
    }

    if (--(slr->count) == 0) {
        slr->user = -1;
        spin_lock_release(&slr->sl);
    }
}

/*
 * 描述: 关闭可重入自旋锁
 */
OS_SEC_L4_TEXT void spin_lock_close_recursive(spin_lock_recursive_t *slr)
{
    (void)slr;
}
