add_subdirectory_if_exist(sle_service)
add_subdirectory_if_exist(product)
add_subdirectory_if_exist(sdm)

if (${CHIP} MATCHES "3322|socmn2")
    include(bt_service.cmake)
endif()
