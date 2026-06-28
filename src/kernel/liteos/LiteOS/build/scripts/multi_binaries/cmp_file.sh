#!/bin/bash
//===============================================================================
// @brief    source file
// Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2025. All rights reserved.
//===============================================================================

# @shell multi_binaries.sh
# copyright(c), 2020 - 2020, huawei tech. co., ltd. ALL RIGHTS RESERVED.
# @brief handle LiteOS head file(.h)
# @version 1.0
set -e

if cmp -s $1 $2;
then echo "#########ROM BIN IS SAME WITH FIRST COMPILE!#########";
else echo "#########ROM BIN IS DIFFERENT WITH FIRST COMPILE!#########";
exit 1;
fi