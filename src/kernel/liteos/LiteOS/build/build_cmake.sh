#!/bin/bash
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
# Description: build bash
set -e

operate=''
outdir=''
top_outdir=buildcmakeout
top_installdir=output
curdir=$(pwd)

# check kconfiglib is install or not by import.
kconfig_is_install="$(python3 build/scripts/check_kconfiglib.py)"

help_info()
{
    echo "--- base cmds --"
    echo "eg: ./build/build_cmake.sh menuconfig"
    echo "eg: ./build/build_cmake.sh savemenuconfig"
    echo "eg: ./build/build_cmake.sh clean"
    echo "eg: ./build/build_cmake.sh cleanall"
    echo "eg: ./build/build_cmake.sh"
    echo "eg: VERBOSE=1 LOSCFG_MULTI_BINARIES=y ./build/build_cmake.sh"
    echo "--- extended cmds --"
    echo "eg: ./build/build_cmake.sh clean outdir"
    echo "eg: ./build/build_cmake.sh cleanall outdir"
    echo "eg: ./build/build_cmake.sh build target outdir"
    echo "eg: ./build/build_cmake.sh install target outdir "
    echo "eg: ./build/build_cmake.sh uni_install [PLAT_TYPE] [CPU_TYPE]"
    echo "eg: VERBOSE=1 LOSCFG_MULTI_BINARIES=y ./build/build_cmake.sh build outdir"
}

if [ -z "$1" ]; then
    operate="build"  #default
else
    operate=$1
fi

if [ "$operate" != "install" ]; then
    box=$(grep 'LOSCFG_BOX=' .config | cut -d '=' -f2)
    echo "box configuration from .config: $box"

    if ! grep -q INTERNAL_OS_UNIFIED_COMPILE .config; then
        platform=$(grep 'LOSCFG_PLATFORM=\"' .config | cut -d '"' -f2)
        echo "targets name extracted from .config: $platform"
    elif grep -q INTERNAL_OS_UNIFIED_COMPILE=y .config; then
        platform=$(grep 'CONFIG_OS_CPU_TYPE=\"' .config | cut -d '"' -f2 | tr '[:upper:]' '[:lower:]' )
        export INTERNAL_OS_UNIFIED_COMPILE=y
        echo "targets name extracted from .config: $platform"
    else
        echo "The current board does not support unified comiple framework."
        exit 1
    fi

    if [ -z "$platform" ]; then
        echo "no targets name extracted"
        exit 1
    fi

    runstop=$(grep 'LOSCFG_KERNEL_RUNSTOP=' .config | cut -d '=' -f2)
    echo "runstop configuration from .config: $runstop"

    scatter=$(grep 'LOSCFG_KERNEL_SCATTER=' .config | cut -d '=' -f2)
    echo "scatter configuration from .config: $scatter"

    trustzone=$(grep 'LOSCFG_TRUSTZONE=' .config | cut -d '=' -f2)
    echo "trustzone configuration from .config: $trustzone"

    aslr_gspr=$(grep 'LOSCFG_LASLR_GSPR=' .config | cut -d '=' -f2)
    echo "aslr_gspr configuration from .config: $aslr_gspr"

    hpl_emulation=$(grep 'LOSCFG_DEBUG_HPL_EMULATION=' .config | cut -d '=' -f2)
    echo "hpl_emulation configuration from .config: $hpl_emulation"

    hpl_preprocessor=$(grep 'LOSCFG_KERNEL_HPL_SCRIPT_PREPROCESSOR=' .config | cut -d '=' -f2)
    echo "hpl_preprocessor configuration from .config: $hpl_preprocessor"

    hpl_enable=$(grep 'LOSCFG_KERNEL_HPL=' .config | cut -d '=' -f2)
    echo "hpl_enable configuration from .config: $hpl_enable"

    box_kconfig=''
    boxcfgfile=''
    if [ "$box" = "y" ]; then
        box_kconfig="$curdir/self_src/targets/$platform/Kconfig"
        if [ -f "$box_kconfig" ]; then
            rm -f $box_kconfig
        fi
        testflag=$(grep 'LOSCFG_TEST=' .config | cut -d '=' -f2)
        if [ "$testflag" = "y" ]; then
            boxcfgfile=$curdir/self_src/targets/$platform/box/boxconfigtest.json
        else
            boxcfgfile=$curdir/self_src/targets/$platform/box/boxconfig.json
        fi
        if [ -f "$boxcfgfile" ]; then
            python3 self_src/tools/scripts/boxes_auto_config/boxes_kconfig_generate.py $boxcfgfile $box_kconfig
        fi
    fi

    if [ "$operate" != "build" ]; then
        if [ "$2" ]; then
            top_outdir=$2
        fi
    else
        if [ "$3" ]; then
            top_outdir=$3
        fi
    fi

    outdir_tmp=''
    if [ "$trustzone" ]; then
        secure_bin=$(grep 'LOSCFG_SECURE_REGION=' .config | cut -d '=' -f2)
        if [ "$secure_bin" ]; then
            outdir_tmp="/secure"
        else
            outdir_tmp="/nonSecure"
        fi
    fi

    outdir="$top_outdir/$platform$outdir_tmp"
    installdir="$top_installdir/$platform$outdir_tmp"
fi

case "${operate}" in
    "build")
        if [ ! -d "$outdir" ]; then
            mkdir -p "$outdir"
        fi
        if [ ! -d "$installdir" ]; then
            mkdir -p "$installdir"
        fi

        target=vs_server
        if [ "$2" ]; then
            target=$2
        fi

        # Prevent users from forgetting to run savemenuconfig after modifying Kconfig.
        cmake . -B ${outdir} -DCMAKE_TOOLCHAIN_FILE="" -DCMAKE_INSTALL_PREFIX="$installdir" -DTARGET="$target" # -G "Ninja" is ok

        # perform savemenuconfig only when kconfig is install
        if [ "$kconfig_is_install" = "True" ]; then
            cmake --build ${outdir} -j --target savemenuconfig
        fi

        if [ "$runstop" -a "$runstop" = y ]; then
            cmake . -B ${outdir} -DMAKE_WOW_IMAGE_VAR=ON -DMAKE_SCATTER_IMAGE_VAR=OFF -DCMAKE_TOOLCHAIN_FILE="" -DCMAKE_INSTALL_PREFIX="$installdir" # -G "Ninja" is ok
            cmake --build ${outdir} -j --target vs_wow_server
            cmake . -B ${outdir} -UMAKE_WOW_IMAGE_VAR -UMAKE_SCATTER_IMAGE_VAR
        fi
        if [ "$scatter" -a "$scatter" = y ]; then
            cmake . -B ${outdir} -DMAKE_WOW_IMAGE_VAR=OFF -DMAKE_SCATTER_IMAGE_VAR=ON -DCMAKE_TOOLCHAIN_FILE="" -DCMAKE_INSTALL_PREFIX="$installdir" # -G "Ninja" is ok
            cmake --build ${outdir} -j  --target vs_scatter_server
            cmake . -B ${outdir} -UMAKE_WOW_IMAGE_VAR -UMAKE_SCATTER_IMAGE_VAR
        fi
        if [ "$aslr_gspr" -a "$aslr_gspr" = y ]; then # link twice for rel array
            cmake . -B ${outdir} -DCMAKE_TOOLCHAIN_FILE="" -DCMAKE_INSTALL_PREFIX="$installdir" # -G "Ninja" is ok
            cmake --build ${outdir} -j --target vs_server
        fi
        # link twice for metadata file.
        if [ "$hpl_emulation" -a "$hpl_emulation" = y -a "$hpl_preprocessor" -a "$hpl_preprocessor" = y ]; then
            cmake . -B ${outdir} -DCMAKE_TOOLCHAIN_FILE="" -DCMAKE_INSTALL_PREFIX="$installdir" # -G "Ninja" is ok
            cmake --build ${outdir} -j --target vs_server
        fi
        # generate hpl decompressor.
        if [ "$hpl_enable" -a "$hpl_enable" = y -a ! "$hpl_emulation" ]; then
            cmake --build ${outdir} -j --target hpl_decompressor
        fi
        cmake . -B ${outdir} -DCMAKE_TOOLCHAIN_FILE="" -DCMAKE_INSTALL_PREFIX="$installdir" -DTARGET="$target" # -G "Ninja" is ok

        # perform savemenuconfig only when kconfig is install
        if [ "$kconfig_is_install" = "True" ]; then
            cmake --build ${outdir} -j --target savemenuconfig
        fi

        target=vs_server
        if [ "$2" ]; then
            target=$2
        fi

        cmake --build ${outdir} -j --target $target
        ;;
    "install")
        target=all
        if [ "$2" ]; then
            target=$2
        fi

        if [ "$3" ]; then
            outdir="$3/buildcmakeout"
            installdir="$3/output"
        else
            outdir=buildcmakeout
            installdir=output
        fi

        if [ ! -d "$outdir" ]; then
            mkdir -p "$outdir"
        fi
        if [ ! -d "$installdir" ]; then
            mkdir -p "$installdir"
        fi

        cmake build/cmake/superbuild -B ${outdir} -DCMAKE_INSTALL_PREFIX="$installdir"
        cmake --build ${outdir} --target $target -j
        ;;
    "uni_install")
        if [ $# -lt 3 ]; then
            echo "wrong input, ex. uni_install [PLAT_TYPE] [CPU_TYPE]"
            exit 1
        fi

        target=os_$2_$3

        if [ "$4" ]; then
            outdir="$4/buildcmakeout"
            installdir="$4/output"
        else
            outdir=buildcmakeout
            installdir=output
        fi

        if [ ! -d "$outdir" ]; then
            mkdir -p "$outdir"
        fi
        if [ ! -d "$installdir" ]; then
            mkdir -p "$installdir"
        fi

        cmake . -B ${outdir} -DCMAKE_INSTALL_PREFIX="$installdir" -DTARGET="$target" -DUNI_INSTALL=y -DPLAT_TYPE="$2" -DCPU_TYPE="$3"
        cmake --build ${outdir} -j --target $target
        cmake --install ${outdir}
    ;;
    "menuconfig")
        if [ ! -d "$outdir" ]; then
            mkdir -p "$outdir"
        fi
        if [ ! -d "$installdir" ]; then
            mkdir -p "$installdir"
        fi
        cmake . -B ${outdir} -DCMAKE_TOOLCHAIN_FILE="" -DCMAKE_INSTALL_PREFIX="$installdir" # -G "Ninja" is ok
        cmake --build ${outdir} -j --target menuconfig
        ;;
    "savemenuconfig")
        if [ ! -d "$outdir" ]; then
            mkdir -p "$outdir"
        fi
        if [ ! -d "$installdir" ]; then
            mkdir -p "$installdir"
        fi
        cmake . -B ${outdir} -DCMAKE_TOOLCHAIN_FILE="" -DCMAKE_INSTALL_PREFIX="$installdir" # -G "Ninja" is ok
        cmake --build ${outdir} -j --target savemenuconfig
        if [ -f "./${outdir}/menuconfig/menuconfig.h" ]; then
            cp ./${outdir}/menuconfig/menuconfig.h ${outdir}/../../self_src/targets/menuconfig.h
        else
            echo "can not find ${outdir}/hiwing/menuconfig/menuconfig.h !!!"
        fi
        ;;
    "clean")
        if [ -d "$outdir" ]; then
            rm -r "$outdir"
        fi
        if [ -d "$installdir" ]; then
            rm -r "$installdir"
        fi
        if [ -f "$box_kconfig" ]; then
            rm -f $box_kconfig
        fi
        ;;
    "cleanall")
        if [ -d "$top_outdir" ]; then
            rm -r "$top_outdir"
        fi
        if [ -d "$top_installdir" ]; then
            rm -r "$top_installdir"
        fi
        if [ -f "$box_kconfig" ]; then
            rm -f $box_kconfig
        fi
        ;;
    "help")
        help_info
        ;;
    *)
        echo "invalid input"
        help_info
        ;;
esac
