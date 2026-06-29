/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2021. All rights reserved.
 * Description: Function of pwm.h
 * Author: AuthorNameMagicTag
 * Create: 2012-06-28
 */

#ifndef __DRV_IOCTL_PWM_H__
#define __DRV_IOCTL_PWM_H__

#include "drv_mcp_dependency.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct {
    unsigned char pwm_num;  /* 0:PWM0,1:PWM1,2:PWM2,3:PWMII0,4:PWMII1,5:PWMII2 */
    unsigned int  duty;
    unsigned int  period;
    unsigned char enable;
} pwm_data;

#define PWM_IOC_MAGIC    'w'

#define PWM_CMD_READ     _IOR(PWM_IOC_MAGIC,  1, pwm_data)
#define PWM_CMD_WRITE    _IOW(PWM_IOC_MAGIC,  2, pwm_data)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  /* __DRV_IOCTL_PWM_H__ */
