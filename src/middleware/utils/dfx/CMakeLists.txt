#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
#===============================================================================

add_subdirectory_if_exist(dfx_reboot/cpu_utils)
add_subdirectory_if_exist(dfx_reboot/reboot)
add_subdirectory_if_exist(diag_dfx_cmd)
add_subdirectory_if_exist(file_operation)
add_subdirectory_if_exist(log_limit)
add_subdirectory_if_exist(dfx_mem)
add_subdirectory_if_exist(dfx_os)
add_subdirectory_if_exist(soc_log)
add_subdirectory_if_exist(log_file)
add_subdirectory_if_exist(boot_msg)
add_subdirectory_if_exist(panic)
add_subdirectory_if_exist(preserve)
add_subdirectory_if_exist(log_reader)
add_subdirectory_if_exist(dfx_print)
add_subdirectory_if_exist(massdata)

if("SUPPORT_DIAG_V2_PROTOCOL" IN_LIST DEFINES)

    set(COMPONENT_NAME "dfx_diag")

    set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/romable/diag_oam_log.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/romable/diag_rom_api.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_debug.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_dfx.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_pkt.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_mem.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_msg.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_tx.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_rx_api.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_channel.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_pkt_router.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_stat.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_filter.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_service.c

        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_cmd_router/diag_cmd_dispatch.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_cmd_router/diag_cmd_dst.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_cmd_router/diag_ind_src.c

        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_system_cmd/diag_cmd_beat_heart.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_system_cmd/diag_cmd_connect.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_system_cmd/diag_cmd_dfx_case.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_system_cmd/diag_cmd_filter.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_system_cmd/diag_cmd_get_mem_info.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_system_cmd/diag_cmd_get_task_info.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_system_cmd/diag_cmd_get_res_info.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_system_cmd/diag_cmd_get_init_info.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_system_cmd/diag_cmd_get_cpup.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_system_cmd/diag_cmd_log_level.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_system_cmd/diag_cmd_mem_read_write.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_system_cmd/diag_cmd_offline_log.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_system_cmd/diag_cmd_password.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_system_cmd/diag_cmd_trace_info.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_system_cmd/diag_cmd_stat.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_system_cmd/diag_mocked_shell.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_system_cmd/last_dump.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_system_cmd/diag_bt_sample_data.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_system_cmd/diag_offline_cmd.c

        ${CMAKE_CURRENT_SOURCE_DIR}/diag_transmit/transmit_cmd_ls.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag_transmit/transmit_cmd_delete_file.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag_transmit/transmit.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag_transmit/transmit_send_recv_pkt.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag_transmit/transmit_item.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag_transmit/transmit_resume.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag_transmit/transmit_dst.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag_transmit/transmit_src.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag_transmit/transmit_write_read.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag_transmit/transmit_host.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag_transmit/transmit_msg.c
        ${CMAKE_CURRENT_SOURCE_DIR}/diag_transmit/transmit_debug.c
    )
if("CONFIG_DIAG_WITH_SECURE" IN_LIST DEFINES)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_secure.c)
endif()

    set(PUBLIC_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/diag
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/include
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/dfx_include
        ${CMAKE_CURRENT_SOURCE_DIR}/soc_log/include
        ${CMAKE_CURRENT_SOURCE_DIR}/log_file/include
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_cmd_router/include
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_system_cmd/dfx_include
        ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_system_cmd/include
        ${CMAKE_CURRENT_SOURCE_DIR}/diag_transmit/include
        ${CMAKE_CURRENT_SOURCE_DIR}/diag_transmit/dfx_include
        ${CMAKE_CURRENT_SOURCE_DIR}/dfx_os/dfx_include
        ${CMAKE_CURRENT_SOURCE_DIR}/dfx_os/include
    )

    set(PRIVATE_HEADER
        ${CMAKE_CURRENT_SOURCE_DIR}/diag_transmit/
    )

    set(PRIVATE_DEFINES
    )

    set(PUBLIC_DEFINES
    )

    # use this when you want to add ccflags like -include xxx
    set(COMPONENT_PUBLIC_CCFLAGS
    )

    set(COMPONENT_CCFLAGS
    )

    set(WHOLE_LINK
        true
    )

    set(MAIN_COMPONENT
        false
    )

    install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/diag/" "*.h")
    install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/diag/include/" "*.h")
    install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/diag/dfx_include/" "*.h")
    install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_cmd_router/include/" "*.h")
    install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_system_cmd/include/" "*.h")
    install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_system_cmd/dfx_include/" "*.h")
    install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/diag_transmit/dfx_include/" "transmit_st.h")
    install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/dfx_os/dfx_include/" "*.h")
    install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/dfx_os/include/" "*.h")

    build_component()

endif()

set(COMPONENT_NAME "dfx_log")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/log/log.c
    ${CMAKE_CURRENT_SOURCE_DIR}/log/log_oam_logger.c
    ${CMAKE_CURRENT_SOURCE_DIR}/log/log_oam_mem_query.c
    ${CMAKE_CURRENT_SOURCE_DIR}/log/log_oam_ota.c
    ${CMAKE_CURRENT_SOURCE_DIR}/log/log_oam_pcm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/log/log_oam_reg_query.c
    ${CMAKE_CURRENT_SOURCE_DIR}/log/log_oam_status.c
    ${CMAKE_CURRENT_SOURCE_DIR}/log/log_oam_dscr_cb.c
    ${CMAKE_CURRENT_SOURCE_DIR}/log/log_oml_exception.c
    ${CMAKE_CURRENT_SOURCE_DIR}/log/log_oml_ota_rom.c
    ${CMAKE_CURRENT_SOURCE_DIR}/log/log_printf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/log/log_printf_rom.c
    ${CMAKE_CURRENT_SOURCE_DIR}/log/log_reg_dump.c
    ${CMAKE_CURRENT_SOURCE_DIR}/log/log_trigger.c
    ${CMAKE_CURRENT_SOURCE_DIR}/log/log_oam_msg.c

    ${CMAKE_CURRENT_SOURCE_DIR}/log_buffer/log_buffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/log_buffer/log_buffer_reader.c
    ${CMAKE_CURRENT_SOURCE_DIR}/log_buffer/log_memory_region.c
    ${CMAKE_CURRENT_SOURCE_DIR}/log_buffer/log_memory_section.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/log/include
    ${CMAKE_CURRENT_SOURCE_DIR}/log_buffer
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
    SUPPORT_DFX_LOG
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/log/include/" "*.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/log_buffer/" "*.h")

build_component()

set(COMPONENT_NAME "dfx_report_data")
 
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_data_store/diag_data_store.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_data_transport/diag_data_transport.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_data_transport/diag_data_transport_bt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_data_transport/diag_data_transport_ble.c
    ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_data_transport/diag_batch.c
)
 
set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_data_store/include
    ${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_data_transport/include
)
 
set(PRIVATE_HEADER
)
 
set(PRIVATE_DEFINES
)
 
set(PUBLIC_DEFINES
)
 
# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)
 
set(COMPONENT_CCFLAGS
)
 
set(WHOLE_LINK
    true
)
 
set(MAIN_COMPONENT
    false
)
 
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_data_store/include/" "*.h")
install_sdk_by_sh("${CMAKE_CURRENT_SOURCE_DIR}/diag/diag_data_transport/include/" "*.h")
 
build_component()