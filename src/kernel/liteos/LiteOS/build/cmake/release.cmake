# 定义固定目录名
set(FIXED_DIR "lib_a")

# 使用 string 命令提取最后一个路径部分
string(REGEX MATCH "([^/]+)$" SUB_DIR "${CMAKE_BINARY_DIR}")

# 检查提取结果
if(NOT SUB_DIR)
    message(FATAL_ERROR "Failed to extract the last path component from CMAKE_BINARY_DIR")
endif()

if (NOT DEFINED SDK_OUTPUT_PATH)
    return()
else()
    # 创建固定目录
    file(MAKE_DIRECTORY ${LITEOS_TOP_DIR}/build/${FIXED_DIR})
    # 创建子目录
    file(MAKE_DIRECTORY ${LITEOS_TOP_DIR}/build/${FIXED_DIR}/${SUB_DIR})
endif()

# 定义源文件路径
set(SOURCE_LIB_PATH_KEYSRC "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/")
# 定义目标文件路径
set(TARGET_LIB_PATH_KEYSRC ${LITEOS_TOP_DIR}/build/${FIXED_DIR}/${SUB_DIR})

# 创建一个自定义目标来移动文件
add_custom_target(move_libraries ALL)
add_custom_target(delete_keysrc_file ALL)

# 定义一个函数来添加移动文件的命令
function(add_move_library_target SOURCE_PATH TARGET_PATH TARGET_NAME)
    if (NOT DEFINED SDK_OUTPUT_PATH)
        return()
    endif()
    add_custom_command(
        TARGET move_libraries POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SOURCE_PATH} ${TARGET_PATH}
        COMMENT "Moving ${SOURCE_PATH} to ${TARGET_PATH}"
    )
endfunction()

function(delete_keysrc_target TARGET_NAME)
    if (NOT DEFINED SDK_OUTPUT_PATH)
        return()
    endif()
    add_custom_command(
        TARGET delete_keysrc_file POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove ${LITEOS_TOP_DIR}/self_src/fs/vfs/bcache/src/bcache.c
        COMMENT "delete keysrc files"
    )
endfunction()

# 调用函数
set(KEYSRC_LIBS
    ${SOURCE_LIB_PATH_KEYSRC}/liblitekey.a  
)

if(LOSCFG_DRIVERS_USB)
    list(APPEND KEYSRC_LIBS ${SOURCE_LIB_PATH_KEYSRC}/libusb_phy.a)
endif()

if(LOSCFG_FS_YAFFS)
    list(APPEND KEYSRC_LIBS ${SOURCE_LIB_PATH_KEYSRC}/libyaffs2.a)
endif()

if(LOSCFG_FS_FAT_FTL)
    list(APPEND KEYSRC_LIBS ${SOURCE_LIB_PATH_KEYSRC}/libftl.a)
endif()

if(LOSCFG_FS_FAT_CACHE)
    list(APPEND KEYSRC_LIBS ${SOURCE_LIB_PATH_KEYSRC}/libbcache.a)
endif()

foreach(lib_a ${KEYSRC_LIBS})
    add_move_library_target(${lib_a} ${TARGET_LIB_PATH_KEYSRC} move_keysrc)
endforeach()