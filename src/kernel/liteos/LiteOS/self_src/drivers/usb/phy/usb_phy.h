/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Usb Physic Abstract Layer Header file
 * Create: 2021-04-29
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
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
 * --------------------------------------------------------------------------- */
#ifndef _USB_PHY_H
#define _USB_PHY_H

#include "stdbool.h"
#include "kal_base.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
    void (*usbStartHcd)(void);
    void (*usbStopHcd)(void);
    void (*usbHost2Device)(void);
    void (*usbDevice2Host)(void);
    void (*usbSuspend)(void);
    void (*usbResume)(void);
    void (*usbReleaseCtrl)(void);
} UsbPhyOps;

extern void UsbPhyInit(void);
extern void UsbPhyReg(const UsbPhyOps *usbPhyOps);
extern void UsbPhyStartHcd(void);
extern void UsbPhyStopHcd(void);
extern void UsbPhyHost2Device(void);
extern void UsbPhyDevice2Host(void);
extern void UsbPhySuspend(void);
extern void UsbPhyResume(void);
extern void UsbPhySetDeviceState(void);
extern void UsbPhyClearDeviceState(void);
extern bool UsbPhyIsDeviceMode(void);
extern void UsbPhyReleaseCtrl(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _USB_INIT_H */
