/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: media hal common
 * Author: Media Software Group
 * Create: 2021-03-02
 */

#include "device_config.h"
#include "securec.h"
#include "audio_errors.h"
#include "audio_utils.h"

#define LOG_MODULE_NAME "AudioDeviceMgr"

#define PRIMARY_ADAPTER_NAME   "Primary"

/* primary adapter */
static AudioDeviceType g_primaryDevice[] = {
    OUT_BUILTIN_SPEAKER,
    IN_BUILTIN_MIC,
};

#ifdef SUPPORT_BT
#define BLUETOOTH_ADAPTER_NAME "Bluetooth"
/* bluetoot adapter */
static AudioDeviceType g_bluetoothDevice[] = {
    OUT_BLUETOOTH_SCO,
    OUT_BLUETOOTH_A2DP,
    IN_BLUETOOTH_SCO,
    IN_BLUETOOTH_A2DP,
    OUT_NEARLINK_ISO_CALL,
    OUT_NEARLINK_ISO_MUSIC,
    IN_NEARLINK_ISO_CALL,
    IN_NEARLINK_ISO_MUSIC,
    OUT_NEARLINK_DONGLE,
    IN_NEARLINK_DONGLE,
};
#endif

/* modem adapter */
#ifdef SUPPORT_MODEM
#define MODEM_ADAPTER_NAME     "Modem"
static AudioDeviceType g_modemDevice[] = {
    OUT_MODEM,
    OUT_MODEM_HEADSET,
    IN_MODEM,
    IN_MODEM_HEADSET,
};
#endif

#ifdef SUPPORT_USB
#define USB_ADAPTER_NAME     "Usb"
static AudioDeviceType g_usbDevice[] = {
    OUT_USB,
    OUT_USB_DONGLE,
    IN_USB,
    IN_USB_DONGLE,
};
#endif

int32_t GetAdapterListFromConfig(AdapterDeviceMap **adapterList, int32_t *adapterNum)
{
    if (adapterList == NULL || adapterNum == NULL) {
        return AUDIO_ERROR;
    }
    /* adapter list */
    static AdapterDeviceMap  s_adapterConfigList[] = {
#ifdef SUPPORT_USB
        {
            .adapterName = USB_ADAPTER_NAME,
            .supportDeviceCount = sizeof(g_usbDevice) / sizeof(AudioDeviceType),
            .supportDevice = g_usbDevice,
        },
#endif
#ifdef SUPPORT_BT
        {
            .adapterName = BLUETOOTH_ADAPTER_NAME,
            .supportDeviceCount = sizeof(g_bluetoothDevice) / sizeof(AudioDeviceType),
            .supportDevice = g_bluetoothDevice,
        },
#endif
#ifdef SUPPORT_MODEM
        {
            .adapterName = MODEM_ADAPTER_NAME,
            .supportDeviceCount = sizeof(g_modemDevice) / sizeof(AudioDeviceType),
            .supportDevice = g_modemDevice,
        },
#endif
        {
            .adapterName = PRIMARY_ADAPTER_NAME,
            .supportDeviceCount = sizeof(g_primaryDevice) / sizeof(AudioDeviceType),
            .supportDevice = g_primaryDevice,
        },
    };
    *adapterList = s_adapterConfigList;
    *adapterNum = (int32_t)(sizeof(s_adapterConfigList) / sizeof(AdapterDeviceMap));
    return AUDIO_SUCCESS;
}
