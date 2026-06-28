# C sources

set(LOCAL_SRCS_y 
${CMAKE_CURRENT_SOURCE_DIR}/hi2132_ccore/board.c
${CMAKE_CURRENT_SOURCE_DIR}/hi2132_ccore/tick_timer.c
${CMAKE_CURRENT_SOURCE_DIR}/hi2132_ccore/os_adapt.c
)
list(APPEND MODULE_INCLUDE_PUB ${CMAKE_CURRENT_SOURCE_DIR}/hi2132_ccore/include)
