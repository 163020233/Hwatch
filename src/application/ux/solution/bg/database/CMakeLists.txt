#===============================================================================
# @brief    cmake make file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ux_bs_database")

if("MELODY_DEMO_DONGLE" IN_LIST DEFINES OR "MELODY_DEMO_SOUNDBAR" IN_LIST DEFINES)
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_bs_database_grant.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_bs_sle_db_grant.c
    )

    set(PUBLIC_HEADER
    )

    set(PRIVATE_HEADER
        ${ROOT_DIR}/application/ux/solution/bg/database
        ${UX_INTERNAL_HEADERS}
        ${ROOT_DIR}/protocol/bt/host/include/sle/sle_service
    )
endif()

if("MELODY_DEMO_EARPHONE" IN_LIST DEFINES OR "MELODY_DEMO_HA" IN_LIST DEFINES OR "MELODY_DEMO_SOUND_BOX" IN_LIST DEFINES)
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_bs_database.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_bs_sle_db.c
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_bs_bt_db.c
    )

    set(PUBLIC_HEADER
    )

    set(PRIVATE_HEADER
        ${ROOT_DIR}/application/ux/solution/bg/database
        ${UX_INTERNAL_HEADERS}
        ${ROOT_DIR}/protocol/bt/host/include/bs_service
        ${ROOT_DIR}/protocol/bt/host/include/sle/sdm
        ${ROOT_DIR}/protocol/bt/host/include/sle/sle_service
        ${ROOT_DIR}/protocol/bt/host/include/sle/sle_audio
    )
endif()

if("MELODY_DEMO_BMIC_RX" IN_LIST DEFINES OR "MELODY_DEMO_BMIC_TX" IN_LIST DEFINES)
    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ux_bs_sle_db_livemic.c
    )

    set(PUBLIC_HEADER
    )

    set(PRIVATE_HEADER
        ${ROOT_DIR}/application/ux/solution/bg/database
        ${UX_INTERNAL_HEADERS}
    )
endif()

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Werror=all
    -Wconversion
    -Werror=incompatible-pointer-types
    -Werror=unused-parameter
    -Werror=enum-conversion
    -Werror=sign-compare
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
