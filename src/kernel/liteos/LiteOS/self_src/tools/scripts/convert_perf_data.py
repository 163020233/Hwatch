#!/usr/bin/env python3
# -*- coding: UTF-8 -*-

# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
# Author: Huawei LiteOS Team
# Create: Mon July 24 10:25:38 2023
# Description: sample.data convert linux perf.data

"""
Description:
1. copy kev ring buffer output from console and save as "sample.data"
2. copy relevat elf files, directory structure is as follows:
├── elf
│   └── tsch_fw.elf
├── convert_perf_data.py
└── sample.data
3. run this scripts, it will generate "perf.data"
4. run perf report to display hotspot functions
"""
import functools
import argparse
import enum
import logging
import os
import struct
import sys

def writeb(array, f):
    """
    Write a unsigned char array to file
    """
    for x in array:
        a = struct.pack('B', x)
        f.write(a)


def writeh(x, f):
    """
    Write a unsigned int variable to file in big-endian
    """
    a = struct.pack('>H', x)
    f.write(a)


def writeq(x, f):
    """
    Write a unsigned long long variable to file in big-endian
    """
    a = struct.pack('>Q', x)
    f.write(a)


def writel(x, f):
    """
    Write a unsigned long variable to file in big-endian
    """
    a = struct.pack('>L', x)
    f.write(a)


class StructField:
    '''
    Descriptor representing a simple structure field
    '''
    def __init__(self, format, offset):
        self.format = format
        self.offset = offset

    def __get__(self, instance, cls):
        if instance is None:
            return self
        else:
            r = struct.unpack_from(instance._endian % self.format, instance._buffer, self.offset)
            return r[0] if len(r) == 1 else r

class StructureMeta(type):
    '''
    Metaclass that automatically creates StructField descriptors
    '''
    def __init__(self, clsname, bases, clsdict):
        fields = getattr(self, '_fields_', [])
        offset = 0
        for format, fieldname in fields:
            setattr(self, fieldname, StructField(format, offset))
            offset += struct.calcsize(format)
        setattr(self, 'struct_size', offset)

class Structure(metaclass=StructureMeta):
    def __init__(self, bytedata, endian):
        self._buffer = bytedata
        self._endian = endian

    @classmethod
    def from_file(cls, f, is_little, is_text):
        if is_text == True:
            hexade_per_char = 2
            sample_data = f.read(cls.struct_size * hexade_per_char)
            data = bytes.fromhex(sample_data.decode('ascii'))
        else :
            data = f.read(cls.struct_size)

        if is_little == True:
            return cls(data, "<%s")
        return cls(data, ">%s")

class SamplePerfHeader(Structure):
    _fields_ = [
        ('I', 'magic'),
        ('I', 'event_type'),
        ('I', 'len'),
        ('I', 'sample_type'),
        ('I', 'section_id')
    ]

class ElfIdent(Structure):
    _fields_ = [
        ('I', 'ei_magic'),
        ('B', 'ei_class'),
        ('B', 'ei_data'),
        ('B', 'ei_version'),
        ('9x', 'ei_pad')
    ]

class Elf32Header(Structure):
    _fields_ = [
        ('H', 'e_type'),
        ('H', 'e_machine'),
        ('I', 'e_version'),
        ('I', 'e_entry'),
        ('I', 'e_phoff'),
        ('I', 'e_shoff'),
        ('I', 'e_flags'),
        ('H', 'e_ehsize'),
        ('H', 'e_phentsize'),
        ('H', 'e_phnum'),
        ('H', 'e_shentsize'),
        ('H', 'e_shnum'),
        ('H', 'e_shstrndx')
    ]

class Elf64Header(Structure):
    _fields_ = [
        ('H', 'e_type'),
        ('H', 'e_machine'),
        ('I', 'e_version'),
        ('Q', 'e_entry'),
        ('Q', 'e_phoff'),
        ('Q', 'e_shoff'),
        ('I', 'e_flags'),
        ('H', 'e_ehsize'),
        ('H', 'e_phentsize'),
        ('H', 'e_phnum'),
        ('H', 'e_shentsize'),
        ('H', 'e_shnum'),
        ('H', 'e_shstrndx')
    ]

class Elf32Shdr(Structure):
    _fields_ = [
        ('I', 'sh_name'),
        ('I', 'sh_type'),
        ('I', 'sh_flags'),
        ('I', 'sh_addr'),
        ('I', 'sh_offset'),
        ('I', 'sh_size'),
        ('I', 'sh_link'),
        ('I', 'sh_info'),
        ('I', 'sh_addralign'),
        ('I', 'sh_entsize')
    ]

class Elf64Shdr(Structure):
    _fields_ = [
        ('I', 'sh_name'),
        ('I', 'sh_type'),
        ('Q', 'sh_flags'),
        ('Q', 'sh_addr'),
        ('Q', 'sh_offset'),
        ('Q', 'sh_size'),
        ('I', 'sh_link'),
        ('I', 'sh_info'),
        ('Q', 'sh_addralign'),
        ('Q', 'sh_entsize')
    ]

class PerfHeaderSection:
    """
    file header structure, include:
        magic                : 8Byte
        size of header       : 8Byte
        size of attr         : 8Byte
        attrs section        : 16Byte
        data_offset section  : 8Byte
        event_type section   : 16Byte
        adds features section: 16Byte

    file section, include:
        offset  : 8Byte
        size    : 8Byte
    """

    def __init__(self, fd, data_size):
        self.fd = fd
        self.data_size = data_size

    def _generate_magic(self):
        array = [0x32, 0x45, 0x4c, 0x49, 0x46, 0x52, 0x45, 0x50,  # magic "PERFILE2"
                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68,  # sizeof header
                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80]  # sizeof one attr section
        writeb(array, self.fd)

    def _generate_attrs_section(self):
        array = [0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68,  # attr.offset
                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80]  # attr_size
        writeb(array, self.fd)

    def _generate_data_section(self):
        array = [0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE8]  # data_offset
        writeb(array, self.fd)
        writeq(self.data_size, self.fd)

    def _generate_event_type_section(self):
        array = [0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  # event_type_offset
                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]  # event_type_size
        writeb(array, self.fd)

    def _generate_adds_features_section(self):
        array = [0x00, 0x01, 0x3D, 0xFC, 0x00, 0x00, 0x00, 0x00,  # add_features 
                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  # add_features
                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  # add_features
                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]  # add_features
        writeb(array, self.fd)

    def _generate_file_attr_section(self):
        array = [0x00, 0x00, 0x00, 0x00,                          # type hardware
                 0x00, 0x00, 0x00, 0x70,                          # size
                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  # config
                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xA0,  # sample_period/sample_freq
                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xa7,  # sample_type ip、tid、time、callchain、cpu、period
                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  # read_format
                 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x1b, 0x03,  # disable.....inhert....
                 0x00, 0x00, 0x00, 0x00,                          # wakeup_events/wakeup_watermark
                 0x00, 0x00, 0x00, 0x00,                          # bp_type
                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  # bp_addr
                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  # bp_len
                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  # branch_sample_type
                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  # sample_regs_user
                 0x00, 0x00, 0x00, 0x00,                          # sample_stack_user
                 0x00, 0x00, 0x00, 0x00,                          # clockid
                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  # sample_regs_intr
                 0x00, 0x00, 0x00, 0x00,                          # aux_watermark
                 0x00, 0x00, 0x00, 0x00]                          # __reserved_2
        writeb(array, self.fd)

    def _generate_perf_file_section(self):
        array = [0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x68,  # ids offset
                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]  # ids size
        writeb(array, self.fd)

    def generate_section(self):
        """
        Generate according to the template and write the file head of perf.data to `perf.data`
        """
        self._generate_magic()
        self._generate_attrs_section()
        self._generate_data_section()
        self._generate_event_type_section()
        self._generate_adds_features_section()
        self._generate_file_attr_section()
        self._generate_perf_file_section()


class PerfSampleEvent:
    """
    event header, include:
        type  : 4Byte
        misc  : 2Byte
        size  : 2Byte

    sample event structure, include:
        event header : 8Byte
        ip           : 8Byte
        pid, tid     : 4Byte
        time         : 8Byte
        cpu, res     : 4Byte
        period       : 8Byte
        nr           : 8Byte
        ips[nr]      : nr * 8Byte
    """

    def __init__(self, fd, event_data):
        self.fd = fd
        self.ip = event_data['ip']
        self.stack = event_data['stack']
        self.pid = event_data['pid']
        self.tid = event_data['tid']
        self.time = event_data['time']
        self.period = event_data['period']
        self.cpu = event_data['cpu']
        self.stack_nr = len(self.stack)

    def _generate_user_header(self):
        # 9: SAMPLE 2: USER
        array = [0, 0, 0, 9, 0, 2]
        writeb(array, self.fd)
        # SIZE, 7:sample event number 8:8Byte
        writeh((7 + self.stack_nr) * 8, self.fd)

    def _generate_kernel_header(self):
        # 9: SAMPLE 1: KERNEL
        array = [0, 0, 0, 9, 0, 1, 0, ((7 + self.stack_nr) * 8)]
        writeb(array, self.fd)

    def _generate_ip(self):
        writeq(self.ip, self.fd)

    def _generate_cpu(self):
        writel(self.cpu, self.fd)
        writel(0x0, self.fd) #res

    def _generate_pid_tid(self):
        writel(self.pid, self.fd)
        writel(self.tid, self.fd)

    def _generate_time(self):
        writeq(self.time, self.fd)

    def _generate_period(self):
        writeq(self.period, self.fd)

    def _generate_stack(self):
        writeq(self.stack_nr, self.fd)
        for i in range(self.stack_nr):
            writeq(self.stack[i], self.fd)

    def generate_event(self):
        """
        Generate a perf sample event and write to the specified file
        """
        self._generate_user_header()
        self._generate_ip()
        self._generate_pid_tid()
        self._generate_time()
        self._generate_cpu()
        self._generate_period()
        self._generate_stack()


class PerfMmapEvent:
    """
    sample event structure, include:
       event header      : 8Byte
       pid, tid          : 4Byte
       addr              : 8Byte
       len               : 8Byte
       pgoff             : 8Byte
       major, min        : 4Byte
       ino               : 8Byte
       ino_generation    : 8Byte
       prot, flags       : 4Byte
       filename          :
       sample id: pid_tid: 4Byte
       sample_id: time   : 8Byte
       sample_id:cpu, res: 4Byte
    """

    def __init__(self, fd, event_data):
        self.name = event_data['name']
        self.pid = event_data['pid']
        self.tid = event_data['tid']
        self.addr = event_data['addr']
        self.mmap2_len = event_data['mmap2_len']
        self.pgoff = event_data['pgoff']
        self.time = event_data['time']
        self.cpu = event_data['cpu']
        self.fd = fd

    def _generate_header(self):
        name_len = len(self.name)
        name_space = int(name_len / 8 + 1) * 8
        # 96: struct excluding the size of filename.
        size = 96 + name_space
        array = [0, 0, 0, 0xA, 0, 2]                              # A: MMAP 2: USER
        writeb(array, self.fd)
        writeh(size, self.fd)

    def _generate_pid_tid(self):
        writel(self.pid, self.fd)
        writel(self.tid, self.fd)

    def _generate_mmap_info(self):
        writeq(self.addr, self.fd)
        writeq(self.mmap2_len, self.fd)
        writeq(self.pgoff, self.fd)

    def _generate_node_info(self):
        # major, minor
        writel(0x0, self.fd)
        writel(0x0, self.fd)
        # ino
        writeq(0x0, self.fd)
        # ino_generation
        writeq(0x0, self.fd)
        # prot flags
        writeq(0x00000000, self.fd)

    def _generate_path(self):
        self.fd.write(self.name.encode())

        name_len = len(self.name)
        name_space = int(name_len / 8 + 1) * 8
        left = name_space - name_len
        for _ in range(left):
            array = [0]
            writeb(array, self.fd)

    def _generate_id(self):
        writel(self.pid, self.fd)
        writel(self.tid, self.fd)
        writeq(self.time, self.fd)
        writel(self.cpu, self.fd)
        writel(0x0, self.fd) #res

    def generate_event(self):
        """
        Generate a perf mmap event and write to the specified file
        """
        self._generate_header()
        self._generate_pid_tid()
        self._generate_mmap_info()
        self._generate_node_info()
        self._generate_path()
        self._generate_id()


class PerfCommEvent:
    """
    command event structure, include:
       event header      : 8Byte
       pid, tid          : 4Byte
       comm[]            : 16Byte
       sample id: pid_tid: 4Byte
       sample_id: time   : 8Byte
       sample_id:cpu, res: 4Byte
    """
    def __init__(self, fd, event_data):
        self.fd = fd
        self.comm = event_data['comm']
        self.pid = event_data['pid']
        self.tid = event_data['tid']
        self.time = event_data['time']
        self.execf = event_data['execf']
        self.cpu = event_data['cpu']

    def _generate_header(self):
        comm_len = len(self.comm)
        comm_space = int(comm_len / 8 + 1) * 8
        size = 40 + comm_space
        array = [0, 0, 0, 0x3, 0x20 if self.execf else 0x0, 0]
        writeb(array, self.fd)
        writeh(size, self.fd)

    def _generate_pid_tid(self):
        writel(self.pid, self.fd)
        writel(self.tid, self.fd)

    def _generate_comm(self):
        self.fd.write(self.comm.encode())

        comm_len = len(self.comm)
        comm_space = int(comm_len / 8 + 1) * 8
        left = comm_space - comm_len
        for _ in range(left):
            array = [0]
            writeb(array, self.fd)

    def _generate_id(self):
        writel(self.pid, self.fd)
        writel(self.tid, self.fd)
        writeq(self.time, self.fd)
        writel(self.cpu, self.fd)
        writel(0x0, self.fd) #res

    def generate_event(self):
        """
        Generate a perf common event and write to the specified file
        """
        self._generate_header()
        self._generate_pid_tid()
        self._generate_comm()
        self._generate_id()

class PerfData:
    """
    The main process that convert perf data form hongmeng format to linux format
    """
    SAMPLE_DATA_MAGIC = 0xEFEFEF00
    ELF_DATA_MAGIC = 0x7F454C46  # ".ELF"
    HEXADECIMAL_PER_CHAR = 2

    DEFAULT_PID = 0x1
    DEFAULT_TID = 0x1
    DEFAULT_TIME = 0x0
    DEFAULT_CPUID = 0x0
    DEFAULT_PERIOD = 0x1

    def __init__(self, elf_path, sample_data_file, section_id):
        self.elf_dir = elf_path
        self.section_id = section_id
        self.sample_file = sample_data_file
        self.is_little_endian = True
        self.is_class32_file = True
        self.sample_data_fd = None
        self.files_mmap_dict = {}
        self.sample_size = 0
        self.elf_file_name = ''
        self.sample_num = 0
        self.multiple = 1
        self.pc_format = '<I'
        self.is_save = True
        self.pc_size = struct.calcsize('I')
        self.sample_struct = [
            ('I', 'cpuid'),
            ('I', 'taskId'),
            ('I', 'eventId'),
            ('I', 'period'),
            ('Q', 'time'),
            ('I', 'pc'),
            ('I', 'ipNr')
        ]

        if 0 == os.path.getsize(sample_data_file):
            logging.error("'%s' total size = %d", sample_data_file,
                          os.path.getsize(sample_data_file))
            sys.exit(1)
        self.sample_size = os.stat(sample_data_file).st_size
        with open(sample_data_file, 'rb') as self.sample_data_fd:
            sample_data = self.sample_data_fd.read(8)
            self.sample_data_fd.seek(0)
        # whether a binary data file
        try:
            # not a binary data file
            bytes.fromhex(sample_data.decode('ascii'))
            self.is_text_file = True
            self.multiple = self.HEXADECIMAL_PER_CHAR
        except UnicodeDecodeError:
            # is a binary data file
            self.is_text_file = False

    def __generate_comm_events(self, fd, comm, event_data):
        data = {'comm': comm,
                'execf': False,
                'pid': self.DEFAULT_PID,
                'tid': event_data['tid'],
                'time': self.DEFAULT_TIME,
                'cpu': event_data['cpu']}
        comm_event = PerfCommEvent(fd, data)
        comm_event.generate_event()

    def __generate_mmap_events(self, fd, comm, event_data):
        for mmaps in self.files_mmap_dict[comm]['mmap']:
            data = {'pid': self.DEFAULT_PID,
                    'tid': event_data['tid'],
                    'addr': mmaps['addr'],
                    'mmap2_len': mmaps["size"],
                    'pgoff': mmaps["offset"],
                    'cpu': event_data['cpu'],
                    'time': self.DEFAULT_TIME,
                    'name': self.files_mmap_dict[comm]['filepath']}
            mmap_event = PerfMmapEvent(fd, data)
            mmap_event.generate_event()

    def __generate_sample_header(self):
        sample_data_object = None
        if ((SamplePerfHeader.struct_size * self.multiple) <= (self.sample_size - self.sample_data_fd.tell())):
            header = SamplePerfHeader.from_file(self.sample_data_fd, self.is_little_endian, self.is_text_file)
            if header.magic != self.SAMPLE_DATA_MAGIC:
                logging.error("This is not a valid sample data type file")
                sys.exit(1)

            if self.section_id != -1 and (self.section_id != header.section_id):
                self.is_save = False

            fields = []
            is_backtrace = False
            for index, (format, name) in enumerate(self.sample_struct):
                if ((header.sample_type >> index) & 1):
                    if name == "ipNr":
                        is_backtrace = True
                    if (name == "pc" and self.is_class32_file != True):
                        format = 'Q'
                    fields.append((format, name))

            if is_backtrace == False:
                logging.error("LOSCFG_BACKTRACE or PERF_RECORD_CALLCHAIN is not set")
                sys.exit(1)

            sample_data_object = type('SamplePerfData', (Structure,), {
                '_fields_': fields
            })

        return sample_data_object

    def __generate_sample_data(self, sample_data):
        event_data = {}
        if ((sample_data.struct_size * self.multiple) <= (self.sample_size - self.sample_data_fd.tell())):
            data = sample_data.from_file(self.sample_data_fd, self.is_little_endian, self.is_text_file)
            if struct.unpack_from(data._endian % 'I', data._buffer, 0)[0] == self.SAMPLE_DATA_MAGIC:
                self.sample_data_fd.seek(-(sample_data.struct_size * self.multiple), 1)
                return event_data

            ip_size = data.ipNr * self.pc_size * self.multiple
            if ip_size <= (self.sample_size - self.sample_data_fd.tell()):
                event_data = {
                    'ip': data.pc,
                    'stack': [data.pc],
                    'pid': self.DEFAULT_PID,
                    'time': self.DEFAULT_TIME,
                    'cpu': data.cpuid if hasattr(data, 'cpuid') else self.DEFAULT_CPUID,
                    'tid': data.taskId if hasattr(data, 'taskId') else self.DEFAULT_TID,
                    'period': data.period if hasattr(data, 'period') else self.DEFAULT_PERIOD}
                buf = self.sample_data_fd.read(ip_size)
                if self.is_text_file == True:
                    buf = bytes.fromhex(buf.decode('ascii'))
                for off in range(0, len(buf), self.pc_size):
                    ip = struct.unpack_from(self.pc_format, buf, off)[0]
                    event_data['stack'].append(ip)
        return event_data

    def __generate_sample_events(self, fd):
        cpu_tid_list = []
        for data_object in iter(self.__generate_sample_header, None):
            for event_data in iter(functools.partial(self.__generate_sample_data, data_object), {}):
                if self.is_save == True and event_data['ip'] != 0:
                    if (event_data['cpu'], event_data['tid']) not in cpu_tid_list:
                        self.__generate_comm_events(fd, self.elf_file_name, event_data)
                        self.__generate_mmap_events(fd, self.elf_file_name, event_data)
                        cpu_tid_list.append((event_data['cpu'], event_data['tid']))
                    sample_event = PerfSampleEvent(fd, event_data)
                    sample_event.generate_event()
                    self.sample_num += 1
            self.is_save = True

    def __generate_event_section(self, fd):
        with open(self.sample_file, 'rb') as self.sample_data_fd:
            self.__generate_sample_events(self.perf_data_fd)

    def __generate_header_section(self, fd):
        head = PerfHeaderSection(fd, 0x12345678)
        head.generate_section()

    def __elf_parser(self, fd):
        ELFCLASS32 = 1
        ELFCLASS64 = 2
        ELFDATA2LSB = 1
        ELFDATA2MSB = 2
        SHT_PROGBITS = 1
        SHF_WRITE = (1 << 0)
        SHF_EXECINSTR = (1 << 2)

        elf_i = ElfIdent.from_file(fd, False, False)
        if elf_i.ei_magic != self.ELF_DATA_MAGIC:
            logging.error("Not elf file")
            return

        if elf_i.ei_data == ELFDATA2LSB:
            self.is_little_endian = True
            self.pc_format = '<'
        elif elf_i.ei_data == ELFDATA2MSB:
            self.is_little_endian = False
            self.pc_format = '>'
        else:
            logging.error('Illegal type.')

        mmap_data = []
        if elf_i.ei_class == ELFCLASS32:
            self.is_class32_file = True
            self.pc_format += 'I'
            self.pc_size = struct.calcsize('I')
            elf_h = Elf32Header.from_file(fd, True, False)
            fd.seek(elf_h.e_shoff)
            for x in range(elf_h.e_shnum):
                elf_s = Elf32Shdr.from_file(fd, True, False)
                if elf_s.sh_type == SHT_PROGBITS and elf_s.sh_size != 0 and \
                   ((elf_s.sh_flags & SHF_EXECINSTR) != 0 or (elf_s.sh_flags & SHF_WRITE) != 0):
                    mmap_data.append({"addr": elf_s.sh_addr, "offset": elf_s.sh_offset, "size": elf_s.sh_size})
        elif elf_i.ei_class == ELFCLASS64:
            self.is_class32_file = False
            self.pc_format += 'Q'
            self.pc_size = struct.calcsize('Q')
            elf_h = Elf64Header.from_file(fd, True, False)
            fd.seek(elf_h.e_shoff)
            for x in range(elf_h.e_shnum):
                elf_s = Elf64Shdr.from_file(fd, True, False)
                if elf_s.sh_type == SHT_PROGBITS and elf_s.sh_size != 0 and \
                   ((elf_s.sh_flags & SHF_EXECINSTR) != 0 or (elf_s.sh_flags & SHF_WRITE) != 0):
                    mmap_data.append({"addr": elf_s.sh_addr, "offset": elf_s.sh_offset, "size": elf_s.sh_size})
        else:
            logging.error('Illegal data encoding.')

        return mmap_data

    def __load_file_mmap_info(self, elf_path):
        for entry in os.listdir(elf_path):
            self.elf_file_name = entry
            file_path = os.path.join(elf_path, entry)
            with open(file_path, 'rb') as fd:
                mmap_data = self.__elf_parser(fd)
                self.files_mmap_dict[entry] = {
                            "filepath" : file_path,
                            "mmap" : mmap_data,
                            "type" : 'ET_EXEC'}

    def generate(self, perf_data_file):
        """
        Convert from a liteos sample.data to a linux perf.data
        """
        self.__load_file_mmap_info(self.elf_dir)
        with os.fdopen(os.open(perf_data_file, os.O_CREAT | os.O_WRONLY, 0o600),
                       'wb') as self.perf_data_fd:
            self.__generate_header_section(self.perf_data_fd)
            self.__generate_event_section(self.perf_data_fd)

def main():
    """
    The entry of conversion
    """
    logging.basicConfig(
            level=logging.INFO,
            format='[%(levelname)s] %(message)s')

    elf_dir = "elf"
    perf_data_file = "perf.data"

    parser = argparse.ArgumentParser(formatter_class=argparse.RawTextHelpFormatter,
                                     description="Convert sample.data(LiteOS) to perf.data(linux).")
    # By default, convert according to contents of file,
    # we provide a parameter to support remove stacks
    parser.add_argument("-s", "--sectionId, tpye=int", default=-1, 
                        dest="section_id", help="section id")
    parser.add_argument("-f", "--filename, tpye=str", default="sample.data",
                        dest="sample_data_file", help="sample data filename")

    args = parser.parse_args()

    if int(args.section_id) < int(-1):
        logging.error('sectionId: %s is an invalid positive int value', args.section_id)
        sys.exit(1)

    elf_path = os.path.abspath(elf_dir)
    perf_data = PerfData(elf_path, args.sample_data_file, int(args.section_id))
    perf_data.generate(perf_data_file)
    logging.info("Generate perf.data:")
    logging.info("ELF files dir   : %s", elf_path)
    logging.info("Sample file     : %s, num = %d", args.sample_data_file, perf_data.sample_num)
    logging.info("Output perf data: %s\n", perf_data_file)

if __name__ == '__main__':
    main()


