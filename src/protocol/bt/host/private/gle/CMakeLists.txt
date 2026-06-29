#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
# 定义BTH_PROJECT是为了将各平台不同方式项目名称定义转化为统一的bth内部的定义, 防止平台修改定义方式后bth内部散弹式修改cmake文件
# xxxx为切换CodeHub源后的首个项目，平台采用全新的货架化架构，编译系统套件采用CFBB编译系统，与原gerrit上的编译套件不兼容

if(${CHIP} MATCHES "bisheng|mp17c|mp12")
    include(gle_offload.cmake)
else()
    if(${CHIP} MATCHES "sw39|sw21|3322|socmn2|sw22|1a71e")
        set(MODULE_NAME "bgh")
    else()
        set(MODULE_NAME "bth")
    endif()

    if((${CHIP} MATCHES "bs25") AND (${CORE} MATCHES "bt_core"))
    set(MODULE_NAME "bt")
    endif()

    if("gle_app" IN_LIST TARGET_COMPONENT)
        include(gle_app_private.cmake)
    endif()

    if("bth_gle" IN_LIST TARGET_COMPONENT)
        include(bth_gle_private.cmake)
    endif()
    if("gle_host" IN_LIST TARGET_COMPONENT)
        include(gle_host_private.cmake)
    endif()

    if(${CHIP} MATCHES "ws73_sle|ws75_sle|ws73_bsle|ws75_bsle")
        include(gle_linux.cmake)
    endif()
endif()