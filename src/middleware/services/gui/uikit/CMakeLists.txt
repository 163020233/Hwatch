# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
#===============================================================================

add_subdirectory_if_exist(proprietary/src/service)
add_subdirectory_if_exist(sample)
add_compile_options(-D_FORTIFY_SOURCE=2 -O2)
add_compile_options(-ftrapv)
set(UIKIT_ROOT_DIR ${PROJECT_ROOT_DIR}/uikit)

# set public headers

set(public_headers
    ${UIKIT_ROOT_DIR}/config/wearable
    ${UIKIT_ROOT_DIR}/utils/interfaces/kits
    ${UIKIT_ROOT_DIR}/utils/interfaces/innerkits
    ${UIKIT_ROOT_DIR}/ui/interfaces/kits
    ${UIKIT_ROOT_DIR}/ui/interfaces/innerkits
    ${UIKIT_ROOT_DIR}/ui/frameworks
    ${UIKIT_ROOT_DIR}/surface/interfaces/kits
    ${UIKIT_ROOT_DIR}/surface/interfaces/innerkits
    ${UIKIT_ROOT_DIR}/proprietary/src/utils/innerkits
    ${UIKIT_ROOT_DIR}/proprietary/src/ui/innerkits
    ${UIKIT_ROOT_DIR}/proprietary/src/ui/frameworks
    ${UIKIT_ROOT_DIR}/proprietary/src/utils/innerkits/hals
    ${UIKIT_ROOT_DIR}/proprietary/include
    ${OHOS_DIR}/drivers/peripheral/display/interfaces/include
    ${PROJECT_ROOT_DIR}/hal/input/include
    ${ROOT_DIR}/include/driver/security_unified/
)

# set public configs
set(public_configs
    -std=c++11
    -Wno-macro-redefined
    -Wno-unused-parameter
    -fno-exceptions
    -fno-unwind-tables
    -fno-asynchronous-unwind-tables
    -Wno-undef
    -Wno-bitwise-op-parentheses
    -Wno-shift-op-parentheses
    -nostdlibinc
    -Wno-missing-field-initializers
    -Wno-writable-strings
    -Wno-reorder-ctor
    -Wno-tautological-pointer-compare
    -Wno-unused-function
    -Werror=return-type
)

if("${CHIP}" STREQUAL "socmn2")
    list(APPEND public_configs -nostdlibinc)
endif()

# build uikit ext
set(COMPONENT_NAME "uikit_ext")
set(PROP_UI_SRC ${UIKIT_ROOT_DIR}/proprietary/src/ui/frameworks)
if("${CHIP}" STREQUAL "socmn2")
set(COMPONENT_SRC
    ${PROP_UI_SRC}/font/ui_shaping_cache.cpp
    ${PROP_UI_SRC}/font/ui_text_shaping_ext.cpp
    ${PROP_UI_SRC}/font/ui_font_bitmap_custom.cpp
    ${PROP_UI_SRC}/components_ext/ui_transform_list.cpp
    ${PROP_UI_SRC}/components_ext/ui_transform_group.cpp
)
else()
file(GLOB_RECURSE lottie ${PROP_UI_SRC}/lottie/*.cpp)
set(COMPONENT_SRC
    ${PROP_UI_SRC}/font/ui_shaping_cache.cpp
    ${PROP_UI_SRC}/font/ui_text_shaping_ext.cpp
    ${PROP_UI_SRC}/components_ext/ui_label_ext.cpp
    ${PROP_UI_SRC}/components_ext/text_ext.cpp
    ${PROP_UI_SRC}/components_ext/ui_label_button_ext.cpp
    ${PROP_UI_SRC}/components_ext/image_cache_manager.cpp
    ${PROP_UI_SRC}/components_ext/ui_hexagons_list.cpp
    ${PROP_UI_SRC}/components_ext/ui_waterfall_list.cpp
    ${PROP_UI_SRC}/components_ext/ui_planet_list.cpp
    ${PROP_UI_SRC}/components_ext/ui_canvas_ext.cpp
    ${PROP_UI_SRC}/components_ext/ui_card_container.cpp
    ${PROP_UI_SRC}/components_ext/ui_cross_view.cpp
    ${PROP_UI_SRC}/components_ext/kaleidoscope_utils.cpp
    ${PROP_UI_SRC}/components_ext/ui_kaleidoscope_view.cpp
    ${PROP_UI_SRC}/components_ext/ui_roller_view.cpp
    ${PROP_UI_SRC}/components_ext/ui_map_view.cpp
    ${PROP_UI_SRC}/components_ext/map_svg_parser.cpp
    ${PROP_UI_SRC}/components_ext/ui_amap_view.cpp
    ${PROP_UI_SRC}/components_ext/amap_svg_parser.cpp
    ${PROP_UI_SRC}/components_ext/ui_slipflow_view.cpp
    ${PROP_UI_SRC}/components_ext/ui_foldable_view.cpp
    ${PROP_UI_SRC}/components_ext/ui_coverflow_view.cpp
    ${PROP_UI_SRC}/components_ext/ui_coverflow_view2.cpp
    ${PROP_UI_SRC}/components_ext/ui_icosahedron_view.cpp
    ${PROP_UI_SRC}/components_ext/ui_transform_group.cpp
    ${PROP_UI_SRC}/components_ext/ui_transform_list.cpp
    ${PROP_UI_SRC}/components_ext/ui_sphere_view.cpp
    ${PROP_UI_SRC}/components_ext/particle_cell.cpp
    ${PROP_UI_SRC}/components_ext/ui_particle_view.cpp
    ${PROP_UI_SRC}/components_ext/ui_float_box_view.cpp
    ${PROP_UI_SRC}/components_ext/ui_rain_view.cpp
    ${PROP_UI_SRC}/components_ext/ui_3d_view.cpp
    ${PROP_UI_SRC}/components_ext/ui_3d_mesh.cpp
    ${PROP_UI_SRC}/components_ext/ui_3d_sphere.cpp
    ${PROP_UI_SRC}/components_ext/ui_3d_cylinder.cpp
    ${PROP_UI_SRC}/components_ext/ui_scatter_plot.cpp
    ${PROP_UI_SRC}/components_ext/ui_chart_pillar_ext.cpp
    ${lottie}
)
endif()

set(SOURCES
    ${COMPONENT_SRC}
)

set(PUBLIC_HEADER
    ${public_headers}
)

set(PRIVATE_HEADER
    ${public_headers}
    ${ROOT_DIR}/include/osal
    ${ROOT_DIR}/kernel/osal/adapt/include
    ${ROOT_DIR}/include/osal/lock
    ${OHOS_DIR}/third_party/icu/icu4c/source/common
    ${PROJECT_BINARY_DIR}/gen/third_party/freetype/freetype/include
    ${ROOT_DIR}/open_source/stb
    ${PROJECT_BINARY_DIR}/gen/third_party/libjpeg-turbo/libjpeg-turbo-2.1.1
    ${PROJECT_BINARY_DIR}/gen/third_party/libpng/libpng-1.6.38
    ${PROJECT_BINARY_DIR}/gen/third_party/harfbuzz/harfbuzz-2.8.2/src
    ${ROOT_DIR}/open_source/flatbuffers/include
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    ${public_configs}
    -Wno-c++11-narrowing
    -Wno-error=unused-function
)

if("${CHIP}" STREQUAL "socmn2")
    list(APPEND COMPONENT_CCFLAGS -nostdlibinc)
endif()

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()

# build traceui
set(COMPONENT_NAME "graphics_ui_traceui")
set(TRACEUI_SRC_DIR ${UIKIT_ROOT_DIR}/proprietary/src/ui/frameworks/dfx/traceui)

if (DEFINES MATCHES "__LITEOS__")
set(TRACEUI_SRC
    ${TRACEUI_SRC_DIR}/traceui_buf.c
    ${TRACEUI_SRC_DIR}/traceui_convert_perfetto_proto.c
    ${TRACEUI_SRC_DIR}/traceui_core.c
)
endif()

set(SOURCES
    ${TRACEUI_SRC}
)

set(PUBLIC_HEADER
    ${UIKIT_ROOT_DIR}/ui/interfaces/kits
)

set(PRIVATE_HEADER
    ${public_headers}
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
    -Wno-float-to-int-overflow
    -Wno-unused-parameter
    -Wno-unused-variable
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
