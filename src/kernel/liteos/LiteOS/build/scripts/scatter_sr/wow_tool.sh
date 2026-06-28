#!/bin/bash
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2019. All rights reserved.
# Description: scatter tool

set -e

cross_compile="$1"
tmp_dir="$2"
scripts_dir="$3"
objs_dir="$4"

ld="${cross_compile}"ld

wow_objs_dir="${tmp_dir}"/wow_objs

[ -n "${wow_objs_dir}" ] &&  rm -rf "${wow_objs_dir}" && mkdir "${wow_objs_dir}"

python3 "${scripts_dir}"/make_bigo.py "${scripts_dir}"/symbol_list.wow "${tmp_dir}"/libs_symbols.list >> "${tmp_dir}"/wow_objs.list
while read LINE; do
    lib="$(echo $LINE | awk -F " " '{print $1}')"
    obj_file="$(echo $LINE | awk -F " " '{print $2}')"
    dir=${lib%\.a}
    if [ -f "${tmp_dir}"/"${dir}"/"${obj_file}" ]; then
        mv "${tmp_dir}"/"${dir}"/"${obj_file}" "${wow_objs_dir}"/"${dir}"_"${obj_file}"
    fi
done < "${tmp_dir}"/wow_objs.list

ls "${wow_objs_dir}"/ | sort -t '.' -k1,1 > "${tmp_dir}"/wow_objs.txt
file="$(cat "${tmp_dir}"/wow_objs.txt | xargs)"

cd "${wow_objs_dir}"/
$ld -z muldefs -r -o "${tmp_dir}"/wow.O $file
mv "${tmp_dir}"/wow.O "${objs_dir}"
cd ../

cd ..
[ -n "${wow_objs_dir}" ] && rm -rf "${wow_objs_dir}"
