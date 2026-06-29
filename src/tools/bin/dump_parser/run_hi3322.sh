# !/bin/bash
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 
if [[ -z $1 ]]; then
    echo "Usage: ./run.sh <last word bin> [lst file]"
    exit
fi

echo "Available last word types:"
echo "1. hi3322-a-liteos"
echo "2. hi3322-bt"

read -p "Input number for last word or q for quit (num/q): " num
case $num in
1)
    header="hi3322_a_liteos.h.pre"
    struct="crash_data_save_t"
    ;;
2)
    header="hi3322_bt.h.pre"
    struct="crash_data_save_t"
    ;;
q)
    exit
    ;;
*)
    echo "The number is invalid!"
    exit
    ;;
esac

if [[ -n $2 ]]; then
    extra_opt="-l$2"
    python3 dump_parser.py -d headers/preprocess/$header -s $struct -i "$1" "$extra_opt"
else
    python3 dump_parser.py -d headers/preprocess/$header -s $struct -i "$1"
fi

