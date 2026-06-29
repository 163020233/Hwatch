#!/usr/bin/env python3
# coding: utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.

__all__ = ['SensorhubNsv', 'AudiodspLL', 'DefaultTarget', 'DefaultRiscvTarget']

import importlib.util as imputil

from targets.default import DefaultTarget

if imputil.find_spec('targets.default_riscv'):
    from targets.default_riscv import DefaultRiscvTarget

if imputil.find_spec('targets.sensorhub_nsv'):
    from targets.sensorhub_nsv import SensorhubNsv

if imputil.find_spec('targets.audiodsp_linglong'):
    from targets.audiodsp_linglong import AudiodspLL
