#!/usr/bin/env python3
# coding: utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
# Description: Generate boxes' configuring files automatically.
# Author: Huawei LiteOS Team

'''
Usage:
    python3 boxes_auto_cfg.py [box_config_file]

Detailed Description:
    The template of box_config_file content should like as following:

    box_dir = ./targets/hi3892_se/box/usr_demo/tboxA
    box_name = boxA
    heap_size = 0x1C00
    permission = 0x2
    stack_size = 0x1000

    box_dir = ./targets/hi3892_se/box/usr_demo/tboxB
    box_name = boxB
    heap_size = 0x1C00
    permission = 0x2
    stack_size = 0x1000

Attention:
    For above configurable items, only [heap_size] and [permission] fields are
    able to be reconfigured by modified box_config_file and re-launching the script.
'''

import sys
import os
import stat
from shutil import copy

box_list = []
current_dir = ""
auto_gen_dir = ""
auto_config_dir = "./tools/scripts/boxes_auto_config"
box_cfg_tag = [\
    "box_dir", "box_name", "heap_size", "permission", "stack_size", "shell_perm", "lms_enable", "task_num", \
    "event_num", "mutex_num", "queue_num", "sem_num", "shmem_num", "swtmr_num", "sid_num"
]
platform = ""
risv_plat = "hi3892_se"

def parese_cfg_file(file_path):
    global platform

    with open(file_path, "r") as f:
        lines_buf = f.readlines();
    tag_idx = 0
    tmp_dict = {}
    for i in range(len(lines_buf)):
        if lines_buf[i] == '\n' and (i + 1) % (len(box_cfg_tag) + 1) != 0 and i != (len(lines_buf) - 1):
            exit("The input box config file is incomplete!")

        if lines_buf[i].strip().startswith(box_cfg_tag[tag_idx]):
            tmp_dict[box_cfg_tag[tag_idx]] = lines_buf[i].split("=")[1].strip().rstrip("/")
            tag_idx = (tag_idx + 1) % len(box_cfg_tag);

            if tag_idx == 1:
                if len(platform) > 0 and platform != tmp_dict["box_dir"].split("/")[1]:
                    exit("The platform name ({}) is different!".format(tmp_dict["box_dir"].split("/")[1]))
                platform = tmp_dict["box_dir"].split("/")[1]
            elif tag_idx == 8:
                if int(tmp_dict["task_num"]) == 0:
                    exit("The task number of box name({}) is 0".format(tmp_dict["box_name"]))
            elif tag_idx == 0:
                sid_sum = int(tmp_dict["event_num"]) + int(tmp_dict["mutex_num"]) + int(tmp_dict["queue_num"]) + \
                          int(tmp_dict["sem_num"]) + int(tmp_dict["shmem_num"])
                if int(tmp_dict["sid_num"]) < sid_sum:
                    exit("The sid number({}) < Total IPC number({})".format(tmp_dict["sid_num"], sid_sum))

            if tag_idx == 0:
                box_list.append(tmp_dict)
                tmp_dict = {}

    global auto_gen_dir
    if len(platform) > 0 and platform != risv_plat:
        auto_gen_dir = "./targets/{}/safety/box/boxes_cfg".format(platform)
    else:
        auto_gen_dir = "./targets/{}/box/boxes_cfg".format(platform)

    if tag_idx != 0:
        exit("The input box config file is incomplete!")

    for d in box_list:
        for t in box_cfg_tag:
            print("{}:{}".format(t, d[t]))
        print("")

def gen_boxes_contents():
    main_header = [\
        "/* ----------------------------------------------------------------------------\n",\
        " * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.\n",\
        " * Description : LiteOS box isolation demo implementation\n",\
        " * Author: Huawei LiteOS Team\n",\
        " * Create : 2022-08-10\n",\
        " * Redistribution and use in source and binary forms, with or without modification,\n",\
        " * are permitted provided that the following conditions are met:\n",\
        " * 1. Redistributions of source code must retain the above copyright notice, this list of\n",\
        " * conditions and the following disclaimer.\n",\
        " * 2. Redistributions in binary form must reproduce the above copyright notice, this list\n",\
        " * of conditions and the following disclaimer in the documentation and/or other materials\n",\
        " * provided with the distribution.\n",\
        " * 3. Neither the name of the copyright holder nor the names of its contributors may be used\n",\
        " * to endorse or promote products derived from this software without specific prior written\n",\
        " * permission.\n",\
        " * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS\n",\
        " * \"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,\n",\
        " * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR\n",\
        " * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR\n",\
        " * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,\n",\
        " * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,\n",\
        " * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;\n",\
        " * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,\n",\
        " * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR\n",\
        " * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF\n",\
        " * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n",\
        " * --------------------------------------------------------------------------- */\n\n",\
        "#include \"stdio.h\"\n",\
        "#include \"boxes_conf.h\"\n",\
        "#include \"boxes_res_conf.h\"\n",\
        "#include \"los_memory.h\"\n",\
        "#ifdef LOSCFG_SHELL\n",\
        "#include \"los_shell.h\"\n",\
        "#endif\n",\
    ]

    main_file = [\
        "\nLOS_TASK_STACK_ALIGN UINT8 g_{}Stack[LOS_{}_STACK_SIZE];\n\n",\
        "LITE_OS_SEC_BOX_RESSPACE LosTaskCB     g_{}TaskCB[LOSCFG_{}_TASK_LIMIT];\n",\
        "LITE_OS_SEC_BOX_RESSPACE LosTaskStack  g_{}KerStack[LOSCFG_{}_TASK_LIMIT];\n",\
        "LITE_OS_SEC_BOX_RESSPACE LosEventCB    g_{}EventCB[LOSCFG_{}_EVENT_LIMIT];\n",\
        "LITE_OS_SEC_BOX_RESSPACE EntityObj     g_{}EventEntity[LOSCFG_{}_EVENT_LIMIT];\n",\
        "LITE_OS_SEC_BOX_RESSPACE LosMuxCB      g_{}MuxCB[LOSCFG_{}_MUX_LIMIT];\n",\
        "LITE_OS_SEC_BOX_RESSPACE EntityObj     g_{}MuxEntity[LOSCFG_{}_MUX_LIMIT];\n",\
        "LITE_OS_SEC_BOX_RESSPACE LosQueueCB    g_{}QueCB[LOSCFG_{}_QUEUE_LIMIT];\n",\
        "LITE_OS_SEC_BOX_RESSPACE EntityObj     g_{}QueEntity[LOSCFG_{}_QUEUE_LIMIT];\n",\
        "LITE_OS_SEC_BOX_RESSPACE LosSemCB      g_{}SemCB[LOSCFG_{}_SEM_LIMIT];\n",\
        "LITE_OS_SEC_BOX_RESSPACE EntityObj     g_{}SemEntity[LOSCFG_{}_SEM_LIMIT];\n",\
        "LITE_OS_SEC_BOX_RESSPACE LosShMemCB    g_{}ShmCB[LOSCFG_{}_SHM_LIMIT];\n",\
        "LITE_OS_SEC_BOX_RESSPACE EntityObj     g_{}ShmEntity[LOSCFG_{}_SHM_LIMIT];\n",\
        "LITE_OS_SEC_BOX_RESSPACE LosSwtmrCB    g_{}SwtCB[LOSCFG_{}_SWT_LIMIT];\n",\
        "LITE_OS_SEC_BOX_RESSPACE SIdObj        g_{}SIdPool[LOSCFG_{}_SID_LIMIT];\n\n",\
        "#ifdef LOSCFG_SHELL\n",\
        "STATIC UINT32 OsShellCmdTest(UINT32 argc, const CHAR **argv)\n",\
        "{\n",\
        "    PRINTK(\"OsShellCmdTest: {}\\n\");\n",\
        "    return 0;\n",\
        "}\n",\
        "#endif\n\n",\
        "VOID {}Entry(VOID)\n",\
        "{\n",\
        "    UINT32 ret;\n\n",\
        "    ret = LOS_MemInit((VOID *)OS_{}_POOL_ADDR, (UINT32){}_HEAP_SIZE);\n",\
        "    if (ret != LOS_OK) {\n",\
        "        printf(\"LOS_MemInit init Failed!\\n\");\n",\
        "        return;\n",\
        "    }\n\n",\
        "#ifdef LOSCFG_SHELL\n",\
        "    ret = LOS_ShellCreate();\n",\
        "    if (ret != LOS_OK) {\n",\
        "        printf(\"LOS_ShellCreate Failed(%X)!\\n\", ret);\n",\
        "        return;\n",\
        "    }\n\n",\
        "    ret = LOS_ShellCmdReg(CMD_TYPE_EX, \"test\", 0, OsShellCmdTest);\n",\
        "    if (ret != LOS_OK) {\n",\
        "        printf(\"LOS_ShellCmdReg Failed(%X)!\\n\", ret);\n",\
        "        return;\n",\
        "    }\n",\
        "#endif\n\n",\
        "    printf(\"Hello {}!\\n\");\n",\
        "}\n"\
    ]

    for box in box_list:
    # Create a the directory if it is not existed
        box_path = ""
        if box["box_dir"].startswith("/"):
            box_path = box["box_dir"]
        else:
            box_path = "{}/{}".format(current_dir, box["box_dir"])
        if not os.path.exists(box_path):
            os.makedirs(box_path)
    # Create the Makefile if it is not existed
        if not os.path.exists("{}/{}".format(box_path, "Makefile")):
            copy("{}/template_files/Makefile".format(auto_config_dir), box_path)
    # Create the CMakeLists.txt if it is not existed
        if not os.path.exists("{}/{}".format(box_path, "CMakeLists.txt")):
            copy("{}/template_files/CMakeLists.txt".format(auto_config_dir), box_path)
    # Create main.c file
        main_file_path = "{}/{}_main.c".format(box_path, box["box_name"])
        if not os.path.exists(main_file_path):
            flags = os.O_WRONLY | os.O_CREAT
            modes = stat.S_IWUSR | stat.S_IRUSR
            with os.fdopen(os.open(main_file_path, flags, modes), 'w') as fout:
                if int(box["shell_perm"]) == 0:
                    for i in range(3):
                        main_header[31 + i] = ""
                fout.writelines(main_header)

                idx = 0
                tmp_file = main_file[0:]
                tmp_file[idx] = tmp_file[idx].format(box["box_name"].lower(), box["box_name"].upper())
                tmp_file[idx + 1] = tmp_file[idx + 1].format(box["box_name"].lower(), box["box_name"].upper())
                tmp_file[idx + 2] = tmp_file[idx + 2].format(box["box_name"].lower(), box["box_name"].upper())

                if int(box["event_num"]) != 0:
                    tmp_file[idx + 3] = tmp_file[idx + 3].format(box["box_name"].lower(), box["box_name"].upper())
                    tmp_file[idx + 4] = tmp_file[idx + 4].format(box["box_name"].lower(), box["box_name"].upper())
                else:
                    tmp_file[idx + 3] = ""
                    tmp_file[idx + 4] = ""

                if int(box["mutex_num"]) != 0:
                    tmp_file[idx + 5] = tmp_file[idx + 5].format(box["box_name"].lower(), box["box_name"].upper())
                    tmp_file[idx + 6] = tmp_file[idx + 6].format(box["box_name"].lower(), box["box_name"].upper())
                else:
                    tmp_file[idx + 5] = ""
                    tmp_file[idx + 6] = ""

                if int(box["queue_num"]) != 0:
                    tmp_file[idx + 7] = tmp_file[idx + 7].format(box["box_name"].lower(), box["box_name"].upper())
                    tmp_file[idx + 8] = tmp_file[idx + 8].format(box["box_name"].lower(), box["box_name"].upper())
                else:
                    tmp_file[idx + 7] = ""
                    tmp_file[idx + 8] = ""

                if int(box["sem_num"]) != 0:
                    tmp_file[idx + 9]  = tmp_file[idx + 9].format(box["box_name"].lower(), box["box_name"].upper())
                    tmp_file[idx + 10] = tmp_file[idx + 10].format(box["box_name"].lower(), box["box_name"].upper())
                else:
                    tmp_file[idx + 9]  = ""
                    tmp_file[idx + 10] = ""

                if int(box["shmem_num"]) != 0:
                    tmp_file[idx + 11] = tmp_file[idx + 11].format(box["box_name"].lower(), box["box_name"].upper())
                    tmp_file[idx + 12] = tmp_file[idx + 12].format(box["box_name"].lower(), box["box_name"].upper())
                else:
                    tmp_file[idx + 11] = ""
                    tmp_file[idx + 12] = ""

                if int(box["swtmr_num"]) != 0:
                    tmp_file[idx + 13] = tmp_file[idx + 13].format(box["box_name"].lower(), box["box_name"].upper())
                else:
                    tmp_file[idx + 13] = ""

                if int(box["sid_num"]) != 0:
                    tmp_file[idx + 14] = tmp_file[idx + 14].format(box["box_name"].lower(), box["box_name"].upper())
                else:
                    tmp_file[idx + 14] = ""

                # LOSCFG_SHELL
                idx = idx + 15
                if int(box["shell_perm"]) != 0:
                    tmp_file[idx + 3] = tmp_file[idx + 3].format(box["box_name"])
                else:
                    for i in range(7):
                        tmp_file[idx + i] = ""

                # Entry
                idx = idx + 7
                tmp_file[idx] = tmp_file[idx].format(box["box_name"])
                tmp_file[idx + 3] = tmp_file[idx + 3].format(box["box_name"].upper(), box["box_name"].upper())

                # LOSCFG_SHELL
                idx = idx + 8
                if int(box["shell_perm"]) == 0:
                    for i in range(12):
                        tmp_file[idx + i] = ""

                # printf
                idx = idx + 12
                tmp_file[idx] = tmp_file[idx].format(box["box_name"])
                fout.writelines(tmp_file)

def gen_boxes_ld():
    ld_file_template = [\
        "",\
        ".{}.text : ALIGN(0x2000)",\
        "{",\
        "    __{}_text_start = .;",\
        "    lib{}.O(.text* .text.*)",\
        "    __{}_rodata_start = .;",\
        "    lib{}.O(.rodata* .rodata.*)",\
        "    . = ALIGN(0x2000);",\
        "    __{}_rodata_end = .;",\
        "    __{}_text_end = .;",\
        "} > FLASH",\
        "",\
        ".{}.data : ALIGN(0x8000)",\
        "{",\
        "    __{}_data_start = .;",\
        "    lib{}.O(.data* .data.*)",\
        "    . = ALIGN(0x20);",\
        "}} > {} AT > FLASH",\
        "__{}_data_load_start = LOADADDR(.{}.data);",\
        "__{}_data_load_size = SIZEOF(.{}.data);",\
        "__{}_data_load_end = __{}_data_load_start + __{}_data_load_size;",\
        "",\
        ".{}.bss .(NOLOAD) : ALIGN(0x4)",\
        "{",\
        "    __{}_bss_start = .;",\
        "    lib{}.O(.bss* .bss.*)",\
        "    . = ALIGN(0x20);",\
        "    __{}_bss_end = .;",\
        "}} > {}",\
        "__{}_bss_size = __{}_bss_end - __{}_bss_start;",\
        "",\
        ".{}.heap (NOLOAD) : ALIGN(0x4)",\
        "{",\
        "    __{}_heap_start = .;",\
        "    . += {};",\
        "    . = ALIGN(0x8000);",\
        "    __{}_heap_end = .;",\
        "    __{}_data_end = .;",\
        "}} > {}",\
        "__{}_heap_size = __{}_heap_end - __{}_heap_start;",\
        "__{}_data_sec_size = __{}_data_end - __{}_data_start;",\
        "",\
        "ASSERT((__{}_data_sec_size <= 0x8000),\"size overflow, please modify the range of __{}_data_start and __{}_data_end.\")",\
    ]
    ld_file_template_risv = [\
        "",\
        ".{}.text : ALIGN(0x20)",\
        "{",\
        "    __{}_text_start = .;",\
        "    lib{}.O(.text* .text.*)",\
        "    __{}_rodata_start = .;",\
        "    lib{}.O(.rodata* .rodata.*)",\
        "    . = ALIGN(0x20);",\
        "    __{}_rodata_end = .;",\
        "    __{}_text_end = .;",\
        "} > FLASH",\
        "",\
        ".{}.data : ALIGN(0x20)",\
        "{",\
        "    __{}_data_start = .;",\
        "    lib{}.O(.data* .data.* .sdata*)",\
        "    . = ALIGN(0x20);",\
        "}} > {} AT > FLASH",\
        "__{}_data_load_start = LOADADDR(.{}.data);",\
        "__{}_data_load_size = SIZEOF(.{}.data);",\
        "__{}_data_load_end = __{}_data_load_start + __{}_data_load_size;",\
        "",\
        ".{}.bss .(NOLOAD) : ALIGN(0x4)",\
        "{",\
        "    __{}_bss_start = .;",\
        "    lib{}.O(.bss* .bss.* .sbss* .sbss.*)",\
        "    . = ALIGN(0x20);",\
        "    __{}_bss_end = .;",\
        "}} > {}",\
        "__{}_bss_size = __{}_bss_end - __{}_bss_start;",\
        "",\
        ".{}.heap (NOLOAD) : ALIGN(0x20)",\
        "{",\
        "    __{}_heap_start = .;",\
        "    . += {};",\
        "    . = ALIGN(0x40);",\
        "    __{}_heap_end = .;",\
        "    __{}_data_end = .;",\
        "}} > {}",\
        "__{}_heap_size = __{}_heap_end - __{}_heap_start;",\
    ]

    platform_str = ""
    ld_file = []
    if len(platform) > 0 and platform != risv_plat:
        platform_str = "RAM"
        ld_file = ld_file_template
    else:
        platform_str = "ram"
        ld_file = ld_file_template_risv

    ld_file_output = []
    for box in box_list:
        box_ld_file = ld_file[0:]

        box_ld_file[17] = box_ld_file[17].format(platform_str)
        box_ld_file[28] = box_ld_file[28].format(platform_str)
        box_ld_file[38] = box_ld_file[38].format(platform_str)
        box_ld_file[1] = box_ld_file[1].format(box["box_name"])
        box_ld_file[3] = box_ld_file[3].format(box["box_name"])
        box_ld_file[5] = box_ld_file[5].format(box["box_name"])
        box_ld_file[8] = box_ld_file[8].format(box["box_name"])
        box_ld_file[9] = box_ld_file[9].format(box["box_name"])
        box_ld_file[12] = box_ld_file[12].format(box["box_name"])
        box_ld_file[14] = box_ld_file[14].format(box["box_name"])
        box_ld_file[18] = box_ld_file[18].format(box["box_name"], box["box_name"])
        box_ld_file[19] = box_ld_file[19].format(box["box_name"], box["box_name"])
        box_ld_file[20] = box_ld_file[20].format(box["box_name"], box["box_name"], box["box_name"])
        box_ld_file[22] = box_ld_file[22].format(box["box_name"])
        box_ld_file[24] = box_ld_file[24].format(box["box_name"])
        box_ld_file[27] = box_ld_file[27].format(box["box_name"])
        box_ld_file[29] = box_ld_file[29].format(box["box_name"], box["box_name"], box["box_name"])
        box_ld_file[31] = box_ld_file[31].format(box["box_name"])
        box_ld_file[33] = box_ld_file[33].format(box["box_name"])
        box_ld_file[36] = box_ld_file[36].format(box["box_name"])
        box_ld_file[37] = box_ld_file[37].format(box["box_name"])
        box_ld_file[39] = box_ld_file[39].format(box["box_name"], box["box_name"], box["box_name"])

        if len(platform) > 0 and platform != risv_plat:
            box_ld_file[40] = box_ld_file[40].format(box["box_name"], box["box_name"], box["box_name"])
            box_ld_file[42] = box_ld_file[42].format(box["box_name"], box["box_name"], box["box_name"])

        obj_file_name = box["box_dir"].strip().split("/")[-1].strip()
        box_ld_file[4] = box_ld_file[4].format(obj_file_name)
        box_ld_file[6] = box_ld_file[6].format(obj_file_name)
        box_ld_file[15] = box_ld_file[15].format(obj_file_name)
        box_ld_file[25] = box_ld_file[25].format(obj_file_name)
        box_ld_file[34] = box_ld_file[34].format(box["heap_size"])
        ld_file_output += box_ld_file

    for i in range(len(ld_file_output)):
        ld_file_output[i] = "{}\n".format(ld_file_output[i])
    ld_file_path = "{}/boxes.ld".format(auto_gen_dir)
    with open(ld_file_path, "w") as f:
        f.writelines(ld_file_output)

def gen_boxes_mk():
    boxes_mk_template = "LIB_SUBDIRS += {}\n"
    boxes_mk_output = []
    mk_file_path = "{}/boxes.mk".format(auto_gen_dir)
    for box in box_list:
        box_path = ""
        if box["box_dir"].startswith("/"):
            box_path = box["box_dir"]
        else:
            box_path = "$(LITEOSTOPDIR)/{}".format(box["box_dir"])
        boxes_mk_output.append(boxes_mk_template.format(box_path))

    with open(mk_file_path, "w") as f:
        f.writelines(boxes_mk_output)

def gen_boxes_cmake():
    boxes_cmake_template = "add_subdirectory({})\n"
    boxes_cmake_output = []
    cmake_file_path = "{}/boxes.cmake".format(auto_gen_dir)
    for box in box_list:
        box_path = ""
        if box["box_dir"].startswith("/"):
            box_path = box["box_dir"]
        else:
            box_path = "{}/{}".format("${LITEOSTOPDIR}", box["box_dir"])
        boxes_cmake_output.append(boxes_cmake_template.format(box_path))

    with open(cmake_file_path, "w") as f:
        f.writelines(boxes_cmake_output)

def gen_boxes_header():
    header_template = [\
        "#define LOS_{}_STACK_SIZE     {}",\
        "extern UINT8 g_{}Stack[LOS_{}_STACK_SIZE];",\
        "extern void {}Entry(void);",\
        "extern char __{}_text_start;",\
        "extern char __{}_text_end;",\
        "extern char __{}_rodata_start;",\
        "extern char __{}_rodata_end;",\
        "extern char __{}_data_start;",\
        "extern char __{}_data_load_start;",\
        "extern char __{}_data_load_end;",\
        "extern char __{}_bss_start;",\
        "extern char __{}_bss_end;",\
        "extern char __{}_data_end;",\
        "extern char __{}_heap_start;",\
        "extern char __{}_heap_end;",\
        "#define {}_TEXT_START      ((UINTPTR)(&__{}_text_start))",\
        "#define {}_TEXT_END        ((UINTPTR)(&__{}_text_end))",\
        "#define {}_RODATA_START    ((UINTPTR)(&__{}_rodata_start))",\
        "#define {}_RODATA_END      ((UINTPTR)(&__{}_rodata_end))",\
        "#define {}_DATA_START      ((UINTPTR)(&__{}_data_start))",\
        "#define {}_DATA_END        ((UINTPTR)(&__{}_data_end))",\
        "#define {}_DATA_LOAD_START ((UINTPTR)(&__{}_data_load_start))",\
        "#define {}_DATA_LOAD_END   ((UINTPTR)(&__{}_data_load_end))",\
        "#define {}_DATA_LOAD_SIZE  ({}_DATA_LOAD_END - {}_DATA_LOAD_START)",\
        "#define {}_BSS_START       ((UINTPTR)(&__{}_bss_start))",\
        "#define {}_BSS_END         ((UINTPTR)(&__{}_bss_end))",\
        "#define {}_BSS_SIZE        ({}_BSS_END - {}_BSS_START)",\
        "#define OS_{}_POOL_ADDR    ((UINTPTR)(&__{}_heap_start))",\
        "#define OS_{}_POOL_END_ADDR ((UINTPTR)(&__{}_heap_end))",\
        "#define {}_HEAP_SIZE       (OS_{}_POOL_END_ADDR - OS_{}_POOL_ADDR)"\
    ]

    data_drvbox_extern = "extern void drvboxEntry(void);"

    box_id = 0
    header_output = []
    header_dynbox_macro = "#define LOS_{}_ID             ({}U + (UINT16)LOS_BOX_BASE_ID)"

    for box in box_list:
        header = []
        header = header_template[0:]
        header[0]  = header[0].format(box["box_name"].upper(),box["stack_size"])
        header[1]  = header[1].format(box["box_name"].lower(),box["box_name"].upper())
        header[2]  = header[2].format(box["box_name"])
        header[3]  = header[3].format(box["box_name"])
        header[4]  = header[4].format(box["box_name"])
        header[5]  = header[5].format(box["box_name"])
        header[6]  = header[6].format(box["box_name"])
        header[7]  = header[7].format(box["box_name"])
        header[8]  = header[8].format(box["box_name"])
        header[9]  = header[9].format(box["box_name"])
        header[10] = header[10].format(box["box_name"])
        header[11] = header[11].format(box["box_name"])
        header[12] = header[12].format(box["box_name"])
        header[13] = header[13].format(box["box_name"])
        header[14] = header[14].format(box["box_name"])
        header[15] = header[15].format(box["box_name"].upper(), box["box_name"])
        header[16] = header[16].format(box["box_name"].upper(), box["box_name"])
        header[17] = header[17].format(box["box_name"].upper(), box["box_name"])
        header[18] = header[18].format(box["box_name"].upper(), box["box_name"])
        header[19] = header[19].format(box["box_name"].upper(), box["box_name"])
        header[20] = header[20].format(box["box_name"].upper(), box["box_name"])
        header[21] = header[21].format(box["box_name"].upper(), box["box_name"])
        header[22] = header[22].format(box["box_name"].upper(), box["box_name"])
        header[23] = header[23].format(box["box_name"].upper(), box["box_name"].upper(), box["box_name"].upper())
        header[24] = header[24].format(box["box_name"].upper(), box["box_name"])
        header[25] = header[25].format(box["box_name"].upper(), box["box_name"])
        header[26] = header[26].format(box["box_name"].upper(), box["box_name"].upper(), box["box_name"].upper())
        header[27] = header[27].format(box["box_name"].upper(), box["box_name"])
        header[28] = header[28].format(box["box_name"].upper(), box["box_name"])
        header[29] = header[29].format(box["box_name"].upper(), box["box_name"].upper(), box["box_name"].upper())
        header_output.append(header_dynbox_macro.format(box["box_name"].upper(), box_id))
        box_id += 1
        header_output += header
    header_output.append(data_drvbox_extern)
    for i in range(len(header_output)):
        header_output[i] = "{}\n".format(header_output[i])

    template_file_path = "{}/template_files/boxes_conf.h".format(auto_config_dir)
    template_file_buf = []
    with open(template_file_path, "r") as f:
        template_file_buf = f.readlines()

    insert_idx = 0
    for i in range(len(template_file_buf)):
        if template_file_buf[i].strip().startswith("<TEMPLATE>"):
            insert_idx = i

    header_output = template_file_buf[:insert_idx] + header_output + template_file_buf[insert_idx + 1:]

    header_file_path = "{}/boxes_conf.h".format(auto_gen_dir)
    with open(header_file_path, "w") as f:
        f.writelines(header_output)

def gen_boxes_cfg():
    cfg_file_template = [\
        "{",\
        "    .boxId = LOS_{}_ID,",\
        "    {",\
        "        {",\
        "            .boxTextStartAddr = (UINTPTR){}_TEXT_START,",\
        "            .boxTextEndAddr = (UINTPTR){}_TEXT_END,",\
        "            .boxRodataStartAddr = (UINTPTR){}_RODATA_START,",\
        "            .boxRodataEndAddr = (UINTPTR){}_RODATA_END,",\
        "            .boxDataStartAddr = (UINTPTR){}_DATA_START,",\
        "            .boxDataEndAddr = (UINTPTR){}_DATA_END,",\
        "            .boxMemPoolAddr = (VOID*)OS_{}_POOL_ADDR,",\
        "            .boxMemPoolEndAddr = (VOID*)OS_{}_POOL_END_ADDR,",\
        "            .boxDataLoadStartAddr = (UINTPTR){}_DATA_LOAD_START,",\
        "            .boxDataLoadEndAddr = (UINTPTR){}_DATA_LOAD_END,",\
        "            .boxBssStartAddr = (UINTPTR){}_BSS_START,",\
        "            .boxBssEndAddr = (UINTPTR){}_BSS_END",\
        "        }",\
        "    },",\
        "    .taskCBList.pstPrev = (LOS_DL_LIST *)0,",\
        "    .taskCBList.pstNext = (LOS_DL_LIST *)0,",\
        "    .boxEntryTask = (TSK_ENTRY_FUNC){}Entry,",\
        "    .boxStatus = OS_BOX_UNINIT,",\
        "    .swtmrTaskId = 0xFFFFFFFF,",\
        "    .boxPerm = {},",\
        "    .boxSIdPool = g_{}SIdPool,",\
        "    .boxSIdPoolSize = LOSCFG_{}_SID_LIMIT,",\
        "    .entryTaskStackSize = 0,",\
        "    .stack = NULL,",\
        '    .entryTaskName = "{}EntryTask",',\
        "    .entryTaskPrio = 0,",\
        "    .dynboxWord = 0,",\
        "    .shellPerm = {},",\
        "    .enableLms = {},",\
        "    .boxResStartAddr = BOX_RES_START,",\
        "    .boxResEndAddr = BOX_RES_END,",\
        "    .boxResTaskList = g_{}TaskCB,",\
        "    .boxResStackList = g_{}KerStack,",\
        "    .boxResTaskNum = LOSCFG_{}_TASK_LIMIT,",\
        "    .boxResEvtList = g_{}EventCB,",\
        "    .boxResEvtEntityList = g_{}EventEntity,",\
        "    .boxResEvtNum = LOSCFG_{}_EVENT_LIMIT,",\
        "    .boxResMuxList = g_{}MuxCB,",\
        "    .boxResMuxEntityList = g_{}MuxEntity,",\
        "    .boxResMuxNum = LOSCFG_{}_MUX_LIMIT,",\
        "    .boxResQueList = g_{}QueCB,",\
        "    .boxResQueEntityList = g_{}QueEntity,",\
        "    .boxResQueNum = LOSCFG_{}_QUEUE_LIMIT,",\
        "    .boxResSemList = g_{}SemCB,",\
        "    .boxResSemEntityList = g_{}SemEntity,",\
        "    .boxResSemNum = LOSCFG_{}_SEM_LIMIT,",\
        "    .boxResShmList = g_{}ShmCB,",\
        "    .boxResShmEntityList = g_{}ShmEntity,",\
        "    .boxResShmNum = LOSCFG_{}_SHM_LIMIT,",\
        "    .boxResSwtList = g_{}SwtCb,",\
        "    .boxResSwtNum = LOSCFG_{}_SWT_LIMIT,",\
        "},\n",
    ]

    cfg_output = []

    for box in box_list:
        cfg_file = []
        cfg_file = cfg_file_template[0:]

        # .boxId
        idx = 1
        cfg_file[idx]     = cfg_file[idx].format(box["box_name"].upper())
        idx = idx + 3
        cfg_file[idx] = cfg_file[idx].format(box["box_name"].upper())
        cfg_file[idx + 1] = cfg_file[idx + 1].format(box["box_name"].upper())
        cfg_file[idx + 2] = cfg_file[idx + 2].format(box["box_name"].upper())
        cfg_file[idx + 3] = cfg_file[idx + 3].format(box["box_name"].upper())
        cfg_file[idx + 4] = cfg_file[idx + 4].format(box["box_name"].upper())
        cfg_file[idx + 5] = cfg_file[idx + 5].format(box["box_name"].upper())
        cfg_file[idx + 6] = cfg_file[idx + 6].format(box["box_name"].upper())
        cfg_file[idx + 7] = cfg_file[idx + 7].format(box["box_name"].upper())
        cfg_file[idx + 8] = cfg_file[idx + 8].format(box["box_name"].upper())
        cfg_file[idx + 9] = cfg_file[idx + 9].format(box["box_name"].upper())
        cfg_file[idx + 10] = cfg_file[idx + 10].format(box["box_name"].upper())
        cfg_file[idx + 11] = cfg_file[idx + 11].format(box["box_name"].upper())

        # .boxEntryTask
        idx = idx + 16
        cfg_file[idx]     = cfg_file[idx].format(box["box_name"])

        # .boxPerm
        idx = idx + 3
        cfg_file[idx]     = cfg_file[idx].format(box["permission"])
        cfg_file[idx + 1] = cfg_file[idx + 1].format(box["box_name"].lower())
        cfg_file[idx + 2] = cfg_file[idx + 2].format(box["box_name"].upper())

        # .entryTaskName
        idx = idx + 5
        cfg_file[idx]     = cfg_file[idx].format(box["box_name"])

        # .shellPerm
        idx = idx + 3
        if int(box["shell_perm"]) != 0:
            cfg_file[idx] = cfg_file[idx].format("OS_BOX_SHPERM_ALLOW")
        else:
            cfg_file[idx] = cfg_file[idx].format("OS_BOX_SHPERM_DENY")
        cfg_file[idx + 1] = cfg_file[idx + 1].format(box["lms_enable"])

        # .boxResTaskList
        idx = idx + 4
        cfg_file[idx]     = cfg_file[idx].format(box["box_name"].lower())
        cfg_file[idx + 1] = cfg_file[idx + 1].format(box["box_name"].lower())
        cfg_file[idx + 2] = cfg_file[idx + 2].format(box["box_name"].upper())

        if int(box["event_num"]) != 0:
            cfg_file[idx + 3] = cfg_file[idx + 3].format(box["box_name"].lower())
            cfg_file[idx + 4] = cfg_file[idx + 4].format(box["box_name"].lower())
            cfg_file[idx + 5] = cfg_file[idx + 5].format(box["box_name"].upper())
        else:
            cfg_file[idx + 3] = ""
            cfg_file[idx + 4] = ""
            cfg_file[idx + 5] = ""

        if int(box["mutex_num"]) != 0:
            cfg_file[idx + 6] = cfg_file[idx + 6].format(box["box_name"].lower())
            cfg_file[idx + 7] = cfg_file[idx + 7].format(box["box_name"].lower())
            cfg_file[idx + 8] = cfg_file[idx + 8].format(box["box_name"].upper())
        else:
            cfg_file[idx + 6] = ""
            cfg_file[idx + 7] = ""
            cfg_file[idx + 8] = ""

        if int(box["queue_num"]) != 0:
            cfg_file[idx + 9]  = cfg_file[idx + 9].format(box["box_name"].lower())
            cfg_file[idx + 10] = cfg_file[idx + 10].format(box["box_name"].lower())
            cfg_file[idx + 11] = cfg_file[idx + 11].format(box["box_name"].upper())
        else:
            cfg_file[idx + 9]  = ""
            cfg_file[idx + 10] = ""
            cfg_file[idx + 11] = ""

        if int(box["sem_num"]) != 0:
            cfg_file[idx + 12] = cfg_file[idx + 12].format(box["box_name"].lower())
            cfg_file[idx + 13] = cfg_file[idx + 13].format(box["box_name"].lower())
            cfg_file[idx + 14] = cfg_file[idx + 14].format(box["box_name"].upper())
        else:
            cfg_file[idx + 12] = ""
            cfg_file[idx + 13] = ""
            cfg_file[idx + 14] = ""

        if int(box["shmem_num"]) != 0:
            cfg_file[idx + 15] = cfg_file[idx + 15].format(box["box_name"].lower())
            cfg_file[idx + 16] = cfg_file[idx + 16].format(box["box_name"].lower())
            cfg_file[idx + 17] = cfg_file[idx + 17].format(box["box_name"].upper())
        else:
            cfg_file[idx + 15] = ""
            cfg_file[idx + 16] = ""
            cfg_file[idx + 17] = ""

        if int(box["swtmr_num"]) != 0:
            cfg_file[idx + 18] = cfg_file[idx + 18].format(box["box_name"].lower())
            cfg_file[idx + 19] = cfg_file[idx + 19].format(box["box_name"].upper())
        else:
            cfg_file[idx + 18] = ""
            cfg_file[idx + 19] = ""

        cfg_output += cfg_file
    for i in range(len(cfg_output)):
        if len(cfg_output[i]) == 0:
            continue
        cfg_output[i] = "{}\n".format(cfg_output[i])

    cfg_file_path = "{}/boxes_auto.cfg".format(auto_gen_dir)
    with open(cfg_file_path, "w") as f:
        f.writelines(cfg_output)

def gen_boxes_res_cfg():
    cfg_macro_template = [\
        "#define LOSCFG_{}_TASK_LIMIT       {}",\
        "#define LOSCFG_{}_EVENT_LIMIT      {}",\
        "#define LOSCFG_{}_MUX_LIMIT        {}",\
        "#define LOSCFG_{}_QUEUE_LIMIT      {}",\
        "#define LOSCFG_{}_SEM_LIMIT        {}",\
        "#define LOSCFG_{}_SHM_LIMIT        {}",\
        "#define LOSCFG_{}_SWT_LIMIT        {}",\
        "#define LOSCFG_{}_SID_LIMIT        {}\n",\
        "#if LOSCFG_{}_SID_LIMIT < (LOSCFG_{}_EVENT_LIMIT + LOSCFG_{}_MUX_LIMIT + LOSCFG_{}_QUEUE_LIMIT + LOSCFG_{}_SEM_LIMIT + LOSCFG_{}_SHM_LIMIT)",\
        "#error \"{} SID number should be larger than or equal to total IPC number.\"",\
        "#endif\n",\
    ]
    cfg_macro_dynbox_template = [\
        "#ifdef LOS_DYN_SYSCALL_LIB",\
        "#define LOSCFG_DYNBOX_TASK_LIMIT     3",\
        "#define LOSCFG_DYNBOX_EVENT_LIMIT    1",\
        "#define LOSCFG_DYNBOX_MUX_LIMIT      1",\
        "#define LOSCFG_DYNBOX_QUEUE_LIMIT    1",\
        "#define LOSCFG_DYNBOX_SEM_LIMIT      1",\
        "#define LOSCFG_DYNBOX_SHM_LIMIT      1",\
        "#define LOSCFG_DYNBOX_SWT_LIMIT      1",\
        "#define LOSCFG_DYNBOX_SID_LIMIT      5\n",\
        "#if LOSCFG_DYNBOX_SID_LIMIT < (LOSCFG_DYNBOX_EVENT_LIMIT + LOSCFG_DYNBOX_MUX_LIMIT + LOSCFG_DYNBOX_QUEUE_LIMIT + LOSCFG_DYNBOX_SEM_LIMIT + LOSCFG_DYNBOX_SHM_LIMIT)",\
        "#error \"DynBox SID number should be larger than or equal to total IPC number.\"\n#endif",\
        "#endif /* LOS_DYN_SYSCALL_LIB */\n",\
    ]
    cfg_declare_template = [\
        "extern LosTaskCB     g_{}TaskCB[LOSCFG_{}_TASK_LIMIT];",\
        "extern LosTaskStack  g_{}KerStack[LOSCFG_{}_TASK_LIMIT];",\
        "extern LosEventCB    g_{}EventCB[LOSCFG_{}_EVENT_LIMIT];",\
        "extern EntityObj     g_{}EventEntity[LOSCFG_{}_EVENT_LIMIT];",\
        "extern LosMuxCB      g_{}MuxCB[LOSCFG_{}_MUX_LIMIT];",\
        "extern EntityObj     g_{}MuxEntity[LOSCFG_{}_MUX_LIMIT];",\
        "extern LosQueueCB    g_{}QueCB[LOSCFG_{}_QUEUE_LIMIT];",\
        "extern EntityObj     g_{}QueEntity[LOSCFG_{}_QUEUE_LIMIT];",\
        "extern LosSemCB      g_{}SemCB[LOSCFG_{}_SEM_LIMIT;",\
        "extern EntityObj     g_{}SemEntity[LOSCFG_{}_SEM_LIMIT];",\
        "extern LosShMemCB    g_{}ShmCB[LOSCFG_{}_SHM_LIMIT];",\
        "extern EntityObj     g_{}ShmEntity[LOSCFG_{}_SHM_LIMIT];",\
        "extern LosSwtmrCB    g_{}SwtCB[LOSCFG_{}_SWT_LIMIT];",\
        "extern SIdObj        g_{}SIdPool[LOSCFG_{}_SID_LIMIT];",\
    ]
    cfg_dynbox_template = [\
        "#ifndef LOS_DYN_SYSCALL_LIB",\
        "#endif",\
    ]

    cfg_output = []
    for box in box_list:
        cfg_file = []
        cfg_file = cfg_macro_template[0:]
        cfg_file[0] = cfg_file[0].format(box["box_name"].upper(), box["task_num"])
        cfg_file[1] = cfg_file[1].format(box["box_name"].upper(), box["event_num"])
        cfg_file[2] = cfg_file[2].format(box["box_name"].upper(), box["mutex_num"])
        cfg_file[3] = cfg_file[3].format(box["box_name"].upper(), box["queue_num"])
        cfg_file[4] = cfg_file[4].format(box["box_name"].upper(), box["sem_num"])
        cfg_file[5] = cfg_file[5].format(box["box_name"].upper(), box["shmem_num"])
        cfg_file[6] = cfg_file[6].format(box["box_name"].upper(), box["swtmr_num"])
        cfg_file[7] = cfg_file[7].format(box["box_name"].upper(), box["sid_num"])
        cfg_file[8] = cfg_file[8].format(box["box_name"].upper(), box["box_name"].upper(), box["box_name"].upper(), \
                      box["box_name"].upper(), box["box_name"].upper(), box["box_name"].upper())
        cfg_file[9] = cfg_file[9].format(box["box_name"].upper())
        cfg_output += cfg_file

    if platform != risv_plat:
        cfg_output += cfg_macro_dynbox_template[0:]
        cfg_output += cfg_dynbox_template[:1]

    for box in box_list:
        cfg_file = []
        cfg_file = cfg_declare_template[0:]
        cfg_file[0] = cfg_file[0].format(box["box_name"].lower(), box["box_name"].upper())
        cfg_file[1] = cfg_file[1].format(box["box_name"].lower(), box["box_name"].upper())

        if int(box["event_num"]) != 0:
            cfg_file[2] = cfg_file[2].format(box["box_name"].lower(), box["box_name"].upper())
            cfg_file[3] = cfg_file[3].format(box["box_name"].lower(), box["box_name"].upper())
        else:
            cfg_file[2] = ""
            cfg_file[3] = ""

        if int(box["mutex_num"]) != 0:
            cfg_file[4] = cfg_file[4].format(box["box_name"].lower(), box["box_name"].upper())
            cfg_file[5] = cfg_file[5].format(box["box_name"].lower(), box["box_name"].upper())
        else:
            cfg_file[4] = ""
            cfg_file[5] = ""

        if int(box["queue_num"]) != 0:
            cfg_file[6] = cfg_file[6].format(box["box_name"].lower(), box["box_name"].upper())
            cfg_file[7] = cfg_file[7].format(box["box_name"].lower(), box["box_name"].upper())
        else:
            cfg_file[6] = ""
            cfg_file[7] = ""

        if int(box["sem_num"]) != 0:
            cfg_file[8] = cfg_file[8].format(box["box_name"].lower(), box["box_name"].upper())
            cfg_file[9] = cfg_file[9].format(box["box_name"].lower(), box["box_name"].upper())
        else:
            cfg_file[8] = ""
            cfg_file[9] = ""

        if int(box["shmem_num"]) != 0:
            cfg_file[10] = cfg_file[10].format(box["box_name"].lower(), box["box_name"].upper())
            cfg_file[11] = cfg_file[11].format(box["box_name"].lower(), box["box_name"].upper())
        else:
            cfg_file[10] = ""
            cfg_file[11] = ""

        if int(box["swtmr_num"]) != 0:
            cfg_file[12] = cfg_file[12].format(box["box_name"].lower(), box["box_name"].upper())
        else:
            cfg_file[12] = ""

        if int(box["sid_num"]) != 0:
            cfg_file[13] = cfg_file[13].format(box["box_name"].lower(), box["box_name"].upper())
        else:
            cfg_file[13] = ""
        cfg_output += cfg_file

    if platform != risv_plat:
        cfg_output += cfg_dynbox_template[1:2]

    for i in range(len(cfg_output)):
        if len(cfg_output[i]) == 0:
            continue
        cfg_output[i] = "{}\n".format(cfg_output[i])

    cfg_file_path = "{}/boxes_res_auto.cfg".format(auto_gen_dir)
    with open(cfg_file_path, "w") as f:
        f.writelines(cfg_output)

usage_err_msg = "Usage Error!\n\t Missing the box config file as an input argument.\n"
path_err_msg = "Please launch the scripts right under the self_src directory.\n"

if __name__ == '__main__':
    if len(sys.argv) != 2:
        exit(usage_err_msg)

    # Get current working directory
    current_dir = os.getcwd()
    # Check valid root path
    if current_dir.strip().split("/")[-1].strip() != "self_src":
        exit(path_err_msg)
    # Parsing usr input configuration file
    parese_cfg_file(sys.argv[1])

    # Generate directory and basic contents: main.c, Makefile and CMakeLists.txt files.
    gen_boxes_contents()

    # Generate ld file
    gen_boxes_ld()

    # Generate mk file
    gen_boxes_mk()

    # Generate cmake file
    gen_boxes_cmake()

    # Generate header file
    gen_boxes_header()

    # Generate cfg file
    gen_boxes_cfg()

    # Generate box resource cfg file
    gen_boxes_res_cfg()
