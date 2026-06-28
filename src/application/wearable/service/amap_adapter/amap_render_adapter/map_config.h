/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: map config
 * Created: 2026-01-14
 */

#ifndef MAP_CONFIG_H
#define MAP_CONFIG_H

#define MAP_BASE_DIR "/user/data/map"

#ifdef AMAP_SVG_SUPPORT
#define MAP_ROOT_DIR MAP_BASE_DIR "/svg_root"
#define OFFLINE_MAP_DIR MAP_BASE_DIR "/svg_offline"
#else
#define MAP_ROOT_DIR MAP_BASE_DIR "/jpeg_root"
#define OFFLINE_MAP_DIR MAP_BASE_DIR "/jpeg_offline"
#endif

#endif