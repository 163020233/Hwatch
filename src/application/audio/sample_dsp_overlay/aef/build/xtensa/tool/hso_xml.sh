#===============================================================================
# @brief   Shell script
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================
#!/bin/sh

if [ $# -lt 5 ]; then
    exit -1
fi

DIR=`dirname $0`
PROJECT_TOP_DIR=`realpath $DIR/../../../../../../..`
TXT_FILE=${1}
TXT_DIR=`dirname ${1}`
CHIP_TYPE=${2}
SOLUTION_TYPE=$CHIP_TYPE
FIRMWARE_DIR=${3}
ACTION_TYPE=${4} # clean prepare process
BOARD_TYPE=${5}
OBJ_TYPE=${6:-main} # main aef sea
TXT_FILE_MERGED=${TXT_DIR}/log_file_dsp.txt
TXT_DEF_MERGED=${TXT_DIR}/log_def_dsp.txt
HEADER_FILE=${TXT_DEF_MERGED%%.*}.h

if [ $CHIP_TYPE == "socmn1" ]; then
    SOLUTION_TYPE="melody"
fi

out_to_xml()
{
    local py_tool=$PROJECT_TOP_DIR/build/script/hdbxml/mk_hso_prim_xml_dsp.py
    if [ ! -e $py_tool ]; then
        return -1
    fi

    python3 $py_tool \
    mkdir \
    $PROJECT_TOP_DIR \
    $SOLUTION_TYPE \
    "dsp_core"

    python3 $py_tool \
    $PROJECT_TOP_DIR \
    $SOLUTION_TYPE \
    "dsp_core" \
    "TRUE" \
    "dsp" \
    ${1}
}

txt_dir_clean()
{
    local txt_dir=$1

    rm -rf ${txt_dir}/*.txt
    rm -rf ${txt_dir}/*.out
    rm -rf ${txt_dir}/*.h
}

txt_to_out()
{
    local num=0
    local txt_file=$1
    local header_file=$2
    local out_file=${txt_file%%.*}.out

    rm -rf ${out_file}

    echo -n ",${txt_file%/*}/log_def_dsp.h" >> ${out_file}

    while read line
    do
        let num++

        if [ $num -eq 1 ]; then
            echo -n " $line" >> ${out_file}
        else
            echo -n ",$line" >> ${out_file}
        fi
    done < ${txt_file}

    out_to_xml ${out_file}
}

def_check()
{
    local def_name=$1
    local txt_def_merged=$2

    if [ ! -e ${txt_def_merged} ]; then
        # file not exists
        return 1
    fi

    while read line
    do
        if [ $def_name == $line ]; then
            # item already exists
            return 0
        fi
    done < ${txt_def_merged}
    # item not exists
    return 1
}

txts_merge()
{
    local num=0
    local res="0"
    local txt_file=$1
    local txt_def_merged=$2
    local txt_file_merged=$3

    while read line
    do
        local c_file_path=${line}
        local c_file_name=${line##*/}
        local upper_name=${c_file_name^^}
        local file_def_name="${upper_name%%.*}_C,"

        # Check whether file already exist
        def_check ${file_def_name} ${txt_def_merged}
        res=`echo $?`
        if [ $res = "1" ]; then
            # item not exists
            # Save FILE_ID definition to one file
            echo ${file_def_name} >> ${txt_def_merged}
            # Save all file path to one file for log scan
            echo ${c_file_path} >> ${txt_file_merged}
        fi
    done < ${txt_file}
}

txt_to_head_file()
{
    local txt_file=$1
    local head_file=${txt_file%%.*}.h

    local module=MAIN
    local start_value=0

    if [ ${OBJ_TYPE} == "aef" ]; then
        local module=AEF
        local start_value=256
    fi

    if [ ${OBJ_TYPE} == "sea" ]; then
        local module=SEA
        local start_value=320
    fi

    if [ ${OBJ_TYPE} == "aef" ]; then
        local module=AEF
        local start_value=320
    fi

    # Sorting the FILE_ID DEFINES txt file before generate the FILE_ID header file to make FILE_ID definition unique.
    sort -b ${txt_file} -o ${txt_file}

    rm -rf ${head_file}

    echo "/*" >> ${head_file}
    echo " * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved." >> ${head_file}
    echo " * Description:" >> ${head_file}
    echo " * Author: @CompanyNameTag" >> ${head_file}
    echo " * Create:" >> ${head_file}
    echo " */" >> ${head_file}
    echo "" >> ${head_file}
    echo "#ifndef _LOG_DEF_DSP_H_" >> ${head_file}
    echo "#define _LOG_DEF_DSP_H_" >> ${head_file}
    echo "" >> ${head_file}
    echo "typedef enum {" >> ${head_file}
    echo "    FILE_ID_${module}_START = ${start_value}," >> ${head_file}

    while read line
    do
        echo "    ${line}" >> ${head_file}
    done < ${txt_file}

    echo "" >> ${head_file}
    echo "    FILE_ID_MAX = 1024," >> ${head_file}
    echo "} log_file_list_enum_dsp_t;" >> ${head_file}
    echo -n "#endif" >> ${head_file}
    echo "" >> ${head_file}
}

xml_clean_hso_database()
{
    local dsp_core_src=$PROJECT_TOP_DIR/output/$CHIP_TYPE/build_hso_cfg/hso_database/xml/base/dsp_core
    if [ -d $dsp_core_src ]; then
        rm -rf $dsp_core_src
    fi
}

database_copy_for_socmn2()
{
    local dsp_core_src=$PROJECT_TOP_DIR/output/$CHIP_TYPE/build_hso_cfg/hso_database/xml/base/dsp_core
    local path_head=$PROJECT_TOP_DIR/interim_binary/$SOLUTION_TYPE/audio
    local path_tail=build_hso_cfg/hso_database/xml/base/dsp_core
    local bmic_rx=$path_head/demo_bmic_rx/$path_tail
    local bmic_tx=$path_head/demo_bmic_tx/$path_tail
    local earphone=$path_head/demo_earphone/$path_tail
    local soundbox=$path_head/demo_sound_box/$path_tail

    declare -A board_target=(   \
        ["bmic_rx"]="$bmic_rx" \
        ["bmic_tx"]="$bmic_tx" \
        ["earphone"]="$earphone" \
        ["sound_box"]="$soundbox" \
    )

    local target_dir="${board_target[$BOARD_TYPE]}"

    if [ ! -d "$dsp_core_src" ]; then
        echo "Error: Source directory $dsp_core_src does not exist."
        return
    fi

    mkdir -p "$target_dir"
    # rm -rf "$target_dir"
    cp -r ${dsp_core_src}/* ${target_dir}/
}

xml_copy_to_hso_database()
{
    local dsp_core_src=$PROJECT_TOP_DIR/output/$CHIP_TYPE/build_hso_cfg/hso_database/xml/base/dsp_core
    local dsp_core_to=$PROJECT_TOP_DIR/interim_binary/$SOLUTION_TYPE/audio/build_hso_cfg/hso_database/xml/base/dsp_core

    if [ $CHIP_TYPE == "sw21" ]; then
        dsp_core_to=$PROJECT_TOP_DIR/interim_binary/$SOLUTION_TYPE/bin/audio/build_hso_cfg/hso_database/xml/base/dsp_core
    fi

    if [ $CHIP_TYPE == "socmn2" ]; then
        database_copy_for_socmn2
        return
    fi

    if [ ! -d $dsp_core_to ]; then
        mkdir -p $dsp_core_to
    fi

    if [ -d $dsp_core_src ]; then
        rm -rf $dsp_core_to
        cp -r $dsp_core_src $dsp_core_to
    fi
}

if [ ${ACTION_TYPE} == "clean" ]; then
    # Clean hso log txt/header file directory
    txt_dir_clean ${TXT_DIR}
else
    if [ ${ACTION_TYPE} == "prepare" ]; then
        file_list=`ls ${TXT_DIR}`

        for file in $file_list
        do
            # Merge each target file list to two file(list file && FILE_ID file)
            txts_merge ${TXT_DIR}/${file} ${TXT_DEF_MERGED} ${TXT_FILE_MERGED}
        done

        # Generate FILE_ID definition header file from FILE_ID file
        txt_to_head_file ${TXT_DEF_MERGED}
    else
        if [ ${ACTION_TYPE} == "process" ]; then
            xml_clean_hso_database
            # Generate log xml for hso database
            txt_to_out ${TXT_FILE_MERGED} ${HEADER_FILE}
            # Copy log xml to solution hso database
            xml_copy_to_hso_database
        fi
    fi
fi
