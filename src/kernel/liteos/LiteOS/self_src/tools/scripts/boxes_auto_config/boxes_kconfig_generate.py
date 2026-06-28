#!/usr/bin/env python3
# coding: utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
# Description: Generate boxes' kconfig files automatically.
# Author: Huawei LiteOS Team

import json
import sys

boxList = []
boxDefaultConfig = []

kconfig = ['menu "Box Config"\n']
menutemp = '\nmenu "{} Config"\n'
configtemp = '\nconfig LOSCFG_{}_{}_LIMIT\n    int "{} {} num"\n    default {}\n'

def PraseJson(file):
    with open(file, 'r') as f:
        data = json.load(f)
    configList = data['boxList']
    for item in configList:
        has_conditional = False if item['conditions'] == "" else True
        boxList.append(item['name'])
        boxName = item['name']
        boxdefaultConf = item['default']
        if has_conditional:
            kconfig.append('\nif {}\n'.format(item['conditions']))
        kconfig.append(menutemp.format(boxName))
        for key in boxdefaultConf:
            kconfig.append(configtemp.format(boxName.upper(), key.upper(), boxName, key, boxdefaultConf[key]))
        kconfig.append('\nendmenu\n')
        if has_conditional:
            kconfig.append('endif')
    kconfig.append('\nendmenu\n')

if __name__ == '__main__':
    if len(sys.argv) != 3:
        exit('{} argv error!'.format(sys.argv))
    PraseJson(sys.argv[1])
    cfg_file_path = sys.argv[2]
    with open(cfg_file_path, 'w') as f:
        f.writelines(kconfig)