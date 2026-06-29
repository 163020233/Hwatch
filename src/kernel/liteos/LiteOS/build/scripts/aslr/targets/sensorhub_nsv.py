#!/usr/bin/env python3
# coding: utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
'''
Sensorhub nsv target
'''

from typing import Optional
from targets.base import RiscvTargets


class SensorhubNsv(RiscvTargets, targets_name='sensorhub_se_lx'):
    '''
    Class for Sensorhub nsv target
    '''
    def __init__(self, parser, arch, safety):
        super().__init__(parser, arch, safety)

        self.max_random_value = 0x1000

        self.rel_hi20_offset = ''

        self.load_img_start = parser.get_one_symtab_value('__load_img_start')
        self.load_img_end = parser.get_one_symtab_value('__load_img_end')

        self.laslr_text_start = parser.get_one_symtab_value('__laslr_start')
        self.laslr_text_end = parser.get_one_symtab_value('__laslr_end')
        self.laslr_data_start = parser.get_one_symtab_value('__laslr_data_start')
        self.laslr_data_end = parser.get_one_symtab_value('__laslr_data_end')

        self.kernel_drvbox_tcm_text_start = parser.get_one_symtab_value('__aslr_kernel_drvbox_tcm_text_start')
        self.kernel_drvbox_tcm_text_end = parser.get_one_symtab_value('__aslr_kernel_drvbox_tcm_text_end')
        self.kernel_drvbox_tcm_data_start = parser.get_one_symtab_value('__aslr_kernel_drvbox_tcm_data_start')
        self.kernel_drvbox_tcm_data_end = parser.get_one_symtab_value('__aslr_kernel_drvbox_tcm_data_end')

        self.kernel_drvbox_ddr_text_start = parser.get_one_symtab_value('__aslr_kernel_drvbox_ddr_text_start')
        self.kernel_drvbox_ddr_text_end = parser.get_one_symtab_value('__aslr_kernel_drvbox_ddr_text_end')
        self.kernel_drvbox_ddr_data_start = parser.get_one_symtab_value('__aslr_kernel_drvbox_ddr_data_start')
        self.kernel_drvbox_ddr_data_end = parser.get_one_symtab_value('__aslr_kernel_drvbox_ddr_data_end')

        self.usr_tcm_text_start = parser.get_one_symtab_value('__aslr_usr_tcm_text_start')
        self.usr_tcm_text_end = parser.get_one_symtab_value('__aslr_usr_tcm_text_end')
        self.usr_tcm_data_start = parser.get_one_symtab_value('__aslr_usr_tcm_data_start')
        self.usr_tcm_data_end = parser.get_one_symtab_value('__aslr_usr_tcm_data_end')

        self.usr_ddr_text_start = parser.get_one_symtab_value('__aslr_usr_ddr_text_start')
        self.usr_ddr_text_end = parser.get_one_symtab_value('__aslr_usr_ddr_text_end')
        self.usr_ddr_data_start = parser.get_one_symtab_value('__aslr_usr_ddr_data_start')
        self.usr_ddr_data_end = parser.get_one_symtab_value('__aslr_usr_ddr_data_end')

        self.box_tcm_text_start = parser.get_one_symtab_value('__aslr_box_tcm_text_start')
        self.box_tcm_text_end = parser.get_one_symtab_value('__aslr_box_tcm_text_end')
        self.box_tcm_data_start = parser.get_one_symtab_value('__aslr_box_tcm_data_start')
        self.box_tcm_data_end = parser.get_one_symtab_value('__aslr_box_tcm_data_end')

        self.box_ddr_text_start = parser.get_one_symtab_value('__aslr_box_ddr_text_start')
        self.box_ddr_text_end = parser.get_one_symtab_value('__aslr_box_ddr_text_end')
        self.box_ddr_data_start = parser.get_one_symtab_value('__aslr_box_ddr_data_start')
        self.box_ddr_data_end = parser.get_one_symtab_value('__aslr_box_ddr_data_end')

        self.aslr_img_start = parser.get_one_symtab_value('__laslr_start')
        self.aslr_img_end = parser.get_one_symtab_value('__laslr_end')

    @property
    def check_ldsize_symbol(self):
        return True

    @staticmethod
    def _mask_data(data, mask):
        '''
        get mask data
        '''
        return format(int(data, 16) & mask, 'x')

    def get_offset_value_type(self, addr):
        '''
        get addr random type
        '''
        min_code_size = 2
        aslr_type = 0
        while 1:
            if self.laslr_text_end > addr >= self.laslr_text_start or \
                self.load_img_end > addr >= self.load_img_start:
                aslr_type = 0
                break
            # kernel tcm
            if self.laslr_data_end > addr >= self.laslr_data_start or \
                self.kernel_drvbox_tcm_text_end > addr >= self.kernel_drvbox_tcm_text_start or \
                self.kernel_drvbox_tcm_data_end > addr >= self.kernel_drvbox_tcm_data_start:
                aslr_type = 1
                break
            # kernel ddr
            if self.kernel_drvbox_ddr_text_end > addr >= self.kernel_drvbox_ddr_text_start or \
                self.kernel_drvbox_ddr_data_end > addr >= self.kernel_drvbox_ddr_data_start:
                aslr_type = 1
                break
            # usr tcm
            if self.usr_tcm_text_end > addr >= self.usr_tcm_text_start or \
                self.usr_tcm_data_end > addr >= self.usr_tcm_data_start:
                aslr_type = 2
                break
            # usr ddr
            if self.usr_ddr_text_end > addr >= self.usr_ddr_text_start or \
                self.usr_ddr_data_end > addr >= self.usr_ddr_data_start:
                aslr_type = 2
                break
            # box tcm
            if self.box_tcm_text_end > addr >= self.box_tcm_text_start or \
                self.box_tcm_data_end > addr >= self.box_tcm_data_start:
                aslr_type = 2
                break
            # box ddr
            if self.box_ddr_text_end > addr >= self.box_ddr_text_start or \
                self.box_ddr_data_end > addr >= self.box_ddr_data_start:
                aslr_type = 2
                break
            addr -= min_code_size
        return aslr_type

    def get_info(self, relinfo: dict) -> int:
        offset_type = self.get_offset_value_type(int(relinfo['offset'], 16))
        val_type = self.get_offset_value_type(int(relinfo['value'], 16))
        info = int(relinfo['info'], 16) & self._arch.relocinfo_mask(self._parser.elfclass)
        return offset_type << 24 | val_type << 16 | info

    def _check_kernel_relocation(self, item):
        if ((self.kernel_drvbox_tcm_text_end > int(item['offset'], 16) >= self.kernel_drvbox_tcm_text_start) or \
            (self.kernel_drvbox_ddr_text_end > int(item['offset'], 16) >= self.kernel_drvbox_ddr_text_start) or \
            (self.kernel_drvbox_tcm_data_end > int(item['offset'], 16) >= self.kernel_drvbox_tcm_data_start) or \
            (self.kernel_drvbox_ddr_data_end > int(item['offset'], 16) >= self.kernel_drvbox_ddr_data_start)) and \
            ((self.kernel_drvbox_tcm_data_start > int(item['value'], 16) >= self.box_tcm_text_start) or \
            (self.usr_tcm_data_end > int(item['value'], 16) >= self.box_tcm_data_start) or \
            (self.usr_ddr_data_end > int(item['value'], 16) >= self.box_ddr_data_start) or \
            (self.usr_ddr_text_end > int(item['value'], 16) >= self.box_ddr_text_start) or \
            (self.aslr_img_end > int(item['value'], 16) >= self.aslr_img_start)):
            return True
        return False

    def _check_usr_relocation(self, item):
        if ((self.kernel_drvbox_tcm_text_end > int(item['value'], 16) >= self.kernel_drvbox_tcm_text_start) or \
            (self.kernel_drvbox_ddr_text_end > int(item['value'], 16) >= self.kernel_drvbox_ddr_text_start) or \
            (self.kernel_drvbox_tcm_data_end > int(item['value'], 16) >= self.kernel_drvbox_tcm_data_start) or \
            (self.kernel_drvbox_ddr_data_end > int(item['value'], 16) >= self.kernel_drvbox_ddr_data_start)) and \
            ((self.kernel_drvbox_tcm_data_start > int(item['offset'], 16) >= self.box_tcm_text_start) or \
            (self.usr_tcm_data_end > int(item['offset'], 16) >= self.box_tcm_data_start) or \
            (self.usr_ddr_data_end > int(item['offset'], 16) >= self.box_ddr_data_start) or \
            (self.usr_ddr_text_end > int(item['offset'], 16) >= self.box_ddr_text_start)):
            return True
        return False

    def _check_pcrel_relocation(self, item, mask):
        if item['value'] == self.rel_hi20_offset and (SensorhubNsv._mask_data(item['info'], mask) == '18' or \
            SensorhubNsv._mask_data(item['info'], mask) == '19'):
            return True
        return False

    def _filter_relocation_address(self, relinfo: dict) -> Optional[dict]:
        '''
        filter some relocation
        '''
        mask = self._arch.relocinfo_mask(self._parser.elfclass)
        filter_status = None
        # 0000c150  003f1e14 R_RISCV_GOT_HI20  200e6860   __stack_chk_guard + 0
        # R_RISCV_GOT_HI20 relocation value(200e6860) is the final address, instead of got position.
        # The machine code need reloate to access got table, so need update value first.
        if SensorhubNsv._mask_data(relinfo['info'], mask) == '14' and \
           int(relinfo['offset'], 16) in self.arch.reloc_dict:
            relinfo = dict(relinfo)
            relinfo.update({'value' : format(self.arch.reloc_dict[int(relinfo['offset'], 16)], 'x')})

        # gp is relocated with box random, so access kernel\drvbox data with gp need to be reloated. The access of box
        # whit gp do not need relocated because of the fixed distance of data and gp.
        if (relinfo['type'] == 'R_RISCV_GPREL_I' or relinfo['type'] == 'R_RISCV_GPREL_S' or
            relinfo['type'] == 'R_RISCV_GPREL17_I' or relinfo['type'] == 'R_RISCV_GPREL17_S'):
            if self.kernel_drvbox_tcm_data_end > int(relinfo['value'], 16) >= self.kernel_drvbox_tcm_data_start:
                return relinfo
            return None

        if self._check_kernel_relocation(relinfo):
            filter_status = relinfo

        if self._check_usr_relocation(relinfo):
            filter_status = relinfo

        if self._check_pcrel_relocation(relinfo, mask):
            filter_status = relinfo

        # PC_REL_HI20 or GOT_HI20
        if filter_status and (SensorhubNsv._mask_data(relinfo['info'], mask) == '17' or
            SensorhubNsv._mask_data(relinfo['info'], mask) == '14'):
            self.rel_hi20_offset = relinfo['offset']

        return filter_status

    def _reloc_addr(self, raw):
        if raw['reltype'] == 'rel':
            raw = self._filter_relocation_address(raw)
            if not raw:
                return raw
            self._arch.check_random_overflow(raw, self.max_random_value)
            if not self._safety:
                return raw
        return super()._reloc_addr(raw)
