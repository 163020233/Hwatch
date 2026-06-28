/* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: the header of task control block in seliteos
 */

#ifndef ARCH_TASK_CB_H
#define ARCH_TASK_CB_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* task TCB offset, LosTaskCB structure */
#define TASK_CB_KERNEL_SP         0x0
#define TASK_CB_STATUS            (TASK_CB_KERNEL_SP + 0x4)
#define TASK_CB_STACK_SIZE        (TASK_CB_STATUS + 0x4)
#define TASK_CB_KERNEL_TOP_SP     (TASK_CB_STACK_SIZE + 0x4)
#define TASK_CB_TASK_NAME         (TASK_CB_KERNEL_TOP_SP + 0x4)

#ifdef LOSCFG_BOX
#define TASK_CB_USER_SP           (TASK_CB_TASK_NAME + 0x4)
#define TASK_CB_USER_SP_SIZE      (TASK_CB_USER_SP + 0x4)
#define TASK_CB_USER_TOP_SP       (TASK_CB_USER_SP_SIZE + 0x4)
#define TASK_CB_DRVBX_STATUS      (TASK_CB_USER_TOP_SP + 0x4)
#define TA_HEADER_INFO            (TASK_CB_DRVBX_STATUS + 0x4)

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ARCH_TASK_CB_H */
