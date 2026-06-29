/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2025-2025. All rights reserved.
* Description: USB port header
* Author: Media Software Group
* Create: 2025-03-04
*/

#ifndef USB_PORT_H
#define USB_PORT_H

#include "audio_port_plugin.h"

typedef const AudioPortPlugin *(*GetUsbPortPlugin)(AudioPluginType pluginType);

const AudioPortPlugin *GetUsbPortPluginFuncs(AudioPluginType pluginType);

#endif
