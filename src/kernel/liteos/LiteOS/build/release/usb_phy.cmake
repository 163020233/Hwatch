set(COMPONENT_LIST "usb_phy")

string(REGEX MATCH "([^/]+)$" SUB_DIR "${CMAKE_BINARY_DIR}")
set(LIBRARY_OUTPUT_PATH ${LITEOS_TOP_DIR}/build/lib_a/${SUB_DIR})

foreach(COMPONENT ${COMPONENT_LIST})
    set(COMPONENT_NAME ${COMPONENT})
    find_library(LIBS${COMPONENT_NAME} "lib${COMPONENT_NAME}.a" ${LIBRARY_OUTPUT_PATH})
    target_link_libraries(${TARGET_NAME} PRIVATE -Wl,--whole-archive  ${LIBS${COMPONENT_NAME}} -Wl,--no-whole-archive )
    build_component()
endforeach()
