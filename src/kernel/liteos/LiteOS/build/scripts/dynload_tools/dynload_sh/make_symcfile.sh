#!/bin/bash
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2019. All rights reserved.
# Description: make symcfile

set -e
if [ -z "$2" ] ; then
	echo "Output file not specified!" 1>&2
	exit 1
fi

if [ ! -f "$1" ] ; then
	echo "Input file not found." 1>&2
	exit 1
fi

cat > "$2" <<EOFH
#include "los_elf_symbol_pri.h"


#define SYMBOL_ENTRY(_symbol)     \\
{                                 \\
    #_symbol,                     \\
    (const INT8 *)_symbol         \\
}

EOFH

sed -r 's#^(.+)$#extern void \1\(void\);#' < "$1" >> "$2"

if [ "$3" = "true" ]; then
    cat >> "$2" <<EOFH

__attribute__((aligned(sizeof(UINTPTR))))
LOS_HAL_TABLE_ENTRY_USR(dynload_gsymbol) LDSymbol g_liteGlobalSymTab[] = {
EOFH
else
    cat >> "$2" <<EOFH

__attribute__((aligned(sizeof(UINTPTR))))
LOS_HAL_TABLE_ENTRY(dynload_gsymbol) LDSymbol g_liteGlobalSymTab[] = {
EOFH
fi

sed -r 's#^(.+)$#    SYMBOL_ENTRY\(\1\),#' < "$1" >> "$2"

cat >> "$2" <<EOFH
};
EOFH
