######### if LOCAL_SRCS is NULL, return right now #########
if(NOT LOCAL_SRCS)
    return()
endif()

######### record libs' path and name to LITEOS_MODULE_LIBS_PATH_PROP #########
if(CMAKE_ARCHIVE_OUTPUT_DIRECTORY) # priority option
    set_property(GLOBAL APPEND PROPERTY LITEOS_MODULE_LIBS_PATH_PROP ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/lib${MODULE_NAME}.a)
    set_property(GLOBAL APPEND PROPERTY LITEOS_MODULE_LIBS_PATH_PROP_INTF ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/lib${MODULE_NAME}.a)
else()
    # if no defined LIB_PATH, use default lib_path
    set_property(GLOBAL APPEND PROPERTY LITEOS_MODULE_LIBS_PATH_PROP ${CMAKE_CURRENT_BINARY_DIR}/lib${MODULE_NAME}.a)
    set_property(GLOBAL APPEND PROPERTY LITEOS_MODULE_LIBS_PATH_PROP_INTF ${CMAKE_CURRENT_BINARY_DIR}/lib${MODULE_NAME}.a)
endif()


####################### support only c asm and cpp files #######################
# add global include, compile macro and options to ${MODULE_NAME}${LOS_PUB_INTF_SUFFIX} #
set(MODULE_CONFIG_PUBLIC "${MODULE_NAME}${LOS_PUB_INTF_SUFFIX}")
add_library(${MODULE_CONFIG_PUBLIC} INTERFACE)

# set syscalls.h include path for user space
list(APPEND MODULE_INCLUDE_PRI ${LITEOS_U_INCLUDES})
list(APPEND MODULE_CXXINCLUDE_PRI ${LITEOS_U_INCLUDES})

# *.S files are treated as c-files, while *.s files are treated as asm-files, referring to module.mk
target_include_directories(${MODULE_CONFIG_PUBLIC} INTERFACE
    $<$<COMPILE_LANGUAGE:C>:${MODULE_INCLUDE_PUB}>
    $<$<COMPILE_LANGUAGE:ASM>:${MODULE_INCLUDE_PUB}>
    $<$<COMPILE_LANGUAGE:CXX>:${MODULE_CXXINCLUDE_PUB}>
)

target_compile_options(${MODULE_CONFIG_PUBLIC} INTERFACE
    $<$<COMPILE_LANGUAGE:C>:${MODULE_COPTS_PUB}>
    $<$<COMPILE_LANGUAGE:ASM>:${MODULE_ASOPTS_PUB}>
    $<$<COMPILE_LANGUAGE:CXX>:${MODULE_CXXOPTS_PUB}>
)

target_compile_definitions(${MODULE_CONFIG_PUBLIC} INTERFACE
    $<$<COMPILE_LANGUAGE:C>:${MODULE_CMACRO_PUB}>
    $<$<COMPILE_LANGUAGE:ASM>:${MODULE_ASMACRO_PUB}>
    $<$<COMPILE_LANGUAGE:CXX>:${MODULE_CMACRO_PUB}>
    $<$<COMPILE_LANGUAGE:CXX>:${MODULE_CXXMACRO_PUB}>
)

target_link_libraries(${MODULE_CONFIG_PUBLIC} INTERFACE
)


# add local include, compile macro and options to ${MODULE_NAME}${LOS_PRI_INTF_SUFFIX} #
set(MODULE_CONFIG_PRIVATE "${MODULE_NAME}${LOS_PRI_INTF_SUFFIX}")
add_library(${MODULE_CONFIG_PRIVATE} INTERFACE)

target_include_directories(${MODULE_CONFIG_PRIVATE} INTERFACE
    $<$<COMPILE_LANGUAGE:C>:${MODULE_INCLUDE_PRI}>
    $<$<COMPILE_LANGUAGE:ASM>:${MODULE_INCLUDE_PRI}>
    $<$<COMPILE_LANGUAGE:CXX>:${MODULE_CXXINCLUDE_PRI}>
)

###################### support high performance log preprocessor ######################
# high performance log interface is supported to be invoked by c language only.
if(LOSCFG_KERNEL_HPL_SCRIPT_PREPROCESSOR)
    # C compiler preprocessor delete annotation code which may case implicit fall-through error.
    # clang not support '-Wimplicit-fallthrough=0'
    if(NOT LOSCFG_COMPILER_CLANG)
        target_compile_options(${MODULE_CONFIG_PRIVATE} INTERFACE
            $<$<COMPILE_LANGUAGE:C>: -Wimplicit-fallthrough=0>
        )
    endif()
    set(MODULE_CONFIG_HPL_PRE "${MODULE_NAME}${LOS_HPL_PRE_INTF_SUFFIX}")
    add_library(${MODULE_CONFIG_HPL_PRE} INTERFACE)
    target_include_directories(${MODULE_CONFIG_HPL_PRE} INTERFACE
        ${MODULE_INCLUDE_PUB}
    )

        target_compile_options(${MODULE_CONFIG_HPL_PRE} INTERFACE
        ${MODULE_COPTS_PUB}
    )

    target_compile_definitions(${MODULE_CONFIG_HPL_PRE} INTERFACE
        ${MODULE_CMACRO_PUB}
    )

    set(HPL_INCLUDE_PRI)
    FOREACH(TEMP_MODULE_INCLUDE_PRI ${MODULE_INCLUDE_PRI})
        get_filename_component(INCLUDE_ABSOLUTE_PATH ${TEMP_MODULE_INCLUDE_PRI} ABSOLUTE)
        list(APPEND HPL_INCLUDE_PRI ${INCLUDE_ABSOLUTE_PATH})
    ENDFOREACH(TEMP_MODULE_INCLUDE_PRI)

    list(TRANSFORM HPL_INCLUDE_PRI PREPEND -I)
    set(HPL_COPTS_PRI ${MODULE_COPTS_PRI} -Wno-date-time)
    set(HPL_CMACRO_PRI ${MODULE_CMACRO_PRI})
    list(TRANSFORM HPL_CMACRO_PRI PREPEND -D)
    file(GENERATE OUTPUT ${LOS_HPL_PREPROCESS_OUT_PATH}/compile_options/includes_${MODULE_NAME}.txt
    CONTENT "-I$<JOIN:$<TARGET_PROPERTY:${LOS_HPL_PRE_INTF},INTERFACE_INCLUDE_DIRECTORIES>,;-I>")

    if(LOSCFG_DEBUG_HPL_EMULATION)
        set(HPL_EMULATION TRUE)
    else()
        set(HPL_EMULATION FALSE)
    endif()

    if(LOSCFG_KERNEL_HPL_STATIC_INFO)
        set(FILE_STATIC_INFO TRUE)
    else()
        set(FILE_STATIC_INFO FALSE)
    endif()
endif()

set(MODULE_SRCS)
list(SORT LOCAL_SRCS)
FOREACH(TEMP_SRCS_FILES ${LOCAL_SRCS})
    get_filename_component(FILE_TYPE ${TEMP_SRCS_FILES} LAST_EXT)
    if(FILE_TYPE MATCHES "^\.S$")
        # *.S files are treated as c-files, while *.s files are treated as asm-files, referring to module.mk
        set_source_files_properties(${TEMP_SRCS_FILES} PROPERTIES LANGUAGE C)
        list(APPEND MODULE_SRCS ${TEMP_SRCS_FILES})
    endif()
    if(FILE_TYPE MATCHES "^\.S$" OR FILE_TYPE MATCHES "^\.s$")
        # add private compile definitions and options only for *.S and *.s files
        if(MODULE_ASOPTS_PRI)
            set_source_files_properties(${TEMP_SRCS_FILES} PROPERTIES
                COMPILE_OPTIONS "${MODULE_ASOPTS_PRI}")
        endif()
        if(MODULE_ASMACRO_PRI)
            set_source_files_properties(${TEMP_SRCS_FILES} PROPERTIES
                COMPILE_DEFINITIONS "${MODULE_ASMACRO_PRI}")
        endif()
        list(APPEND MODULE_SRCS ${TEMP_SRCS_FILES})
    elseif(FILE_TYPE MATCHES "^\.c$")
        if(LOSCFG_KERNEL_HPL_SCRIPT_PREPROCESSOR)
            # use ".c" as compiler preprocess out file name suffix instead of ".i",
            # other wise cmake cannot determine link language.
            get_filename_component(SRCS_FILE_ABSOLUTE_PATH ${TEMP_SRCS_FILES} ABSOLUTE)
            string(TOLOWER ${SRCS_FILE_ABSOLUTE_PATH} SRCS_FILE_TOLOWER_PATH)
            string(REGEX MATCH "lite.*$" SRCS_FILE_RELATIVE_PATH ${SRCS_FILE_TOLOWER_PATH})
            set(PREPROCESS_OUT_FILE ${LOS_HPL_PREPROCESS_OUT_PATH}/${SRCS_FILE_RELATIVE_PATH})
            get_filename_component(PREPROCESS_OUT_DIR ${PREPROCESS_OUT_FILE} DIRECTORY)
            file(MAKE_DIRECTORY ${PREPROCESS_OUT_DIR})
            add_custom_command(
                DEPENDS ${SRCS_FILE_ABSOLUTE_PATH}
                BYPRODUCTS ${PREPROCESS_OUT_FILE}
                OUTPUT ${PREPROCESS_OUT_FILE}
                COMMAND ${CMAKE_C_COMPILER} ${HPL_INCLUDE_PRI} ${HPL_CMACRO_PRI}
                "-I$<JOIN:$<TARGET_PROPERTY:${LOS_HPL_PRE_INTF},INTERFACE_INCLUDE_DIRECTORIES>,;-I>"
                "-D$<JOIN:$<TARGET_PROPERTY:${LOS_HPL_PRE_INTF},INTERFACE_COMPILE_DEFINITIONS>,;-D>"
                "$<TARGET_PROPERTY:${LOS_HPL_PRE_INTF},INTERFACE_COMPILE_OPTIONS>"
                ${HPL_COPTS_PRI}
                "-I$<JOIN:$<TARGET_PROPERTY:${VENDOR_HPL_PRE_INTF},INTERFACE_INCLUDE_DIRECTORIES>,;-I>"
                "-D$<JOIN:$<TARGET_PROPERTY:${VENDOR_HPL_PRE_INTF},INTERFACE_COMPILE_DEFINITIONS>,;-D>"
                "$<TARGET_PROPERTY:${VENDOR_HPL_PRE_INTF},INTERFACE_COMPILE_OPTIONS>"
                -E ${SRCS_FILE_ABSOLUTE_PATH}
                -o ${PREPROCESS_OUT_FILE}
                COMMAND ${PYTHON3} ${LITEOS_SCRIPTPATH}/hpl/log_hpl_preprocessor.py
                ${OS_HPL_LOG_MODULE_ID}
                ${PREPROCESS_OUT_FILE}
                ${LOSCFG_KERNEL_HPL_CORE_BIT}
                ${LOS_HPL_PREPROCESS_OUT_PATH}
                ${HPL_EMULATION}
                ${FILE_STATIC_INFO}
                COMMAND_EXPAND_LISTS
                VERBATIM
            )
            list(APPEND MODULE_SRCS ${PREPROCESS_OUT_FILE})
            set(SOURCE_FILE ${PREPROCESS_OUT_FILE})
        else()
            list(APPEND MODULE_SRCS ${TEMP_SRCS_FILES})
            set(SOURCE_FILE ${TEMP_SRCS_FILES})
        endif()

        # add private compile definitions and options only for *.c files
        if(MODULE_COPTS_PRI)
            set_source_files_properties(${SOURCE_FILE} PROPERTIES
                COMPILE_OPTIONS "${MODULE_COPTS_PRI}")
        endif()
        if(MODULE_CMACRO_PRI)
            set_source_files_properties(${SOURCE_FILE} PROPERTIES
                COMPILE_DEFINITIONS "${MODULE_CMACRO_PRI}")
        endif()
    endif()
ENDFOREACH(TEMP_SRCS_FILES)

# refer to cmake target_compile_features or generator expression for more details
target_compile_options(${MODULE_CONFIG_PRIVATE} INTERFACE
    $<$<COMPILE_LANGUAGE:CXX>:${MODULE_CXXOPTS_PRI}>
)

target_compile_definitions(${MODULE_CONFIG_PRIVATE} INTERFACE
    $<$<COMPILE_LANGUAGE:CXX>:${MODULE_CMACRO_PRI}>
    $<$<COMPILE_LANGUAGE:CXX>:${MODULE_CXXMACRO_PRI}>
)

target_link_libraries(${MODULE_CONFIG_PRIVATE} INTERFACE
)


# add srcs in the module to ${MODULE_NAME}, generating lib${MODULE_NAME}.a #
add_library(${MODULE_NAME} STATIC ${MODULE_SRCS})

target_link_libraries(${MODULE_NAME} PRIVATE
    $<BUILD_INTERFACE:${MODULE_CONFIG_PRIVATE}>
    $<BUILD_INTERFACE:${LOS_CC_PROP_INTF_PUB}>
    $<BUILD_INTERFACE:${VENDOR_CC_PROP_INTF_PUB}>
)

set_target_properties(${MODULE_NAME}
    PROPERTIES
    OUTPUT_NAME $<IF:$<STREQUAL:${OS_TYPE},windows>,lib${MODULE_NAME},${MODULE_NAME}>
)

if(LOSCFG_SAFETY_KERNEL AND (NOT LOSCFG_COMPILE_LIB_WITHOUT_OBJ))
set(MODULE_BIGO_NAME lib${MODULE_NAME}.O)
add_executable(${MODULE_BIGO_NAME} ${LOCAL_SRCS})
target_link_options(${MODULE_BIGO_NAME} PRIVATE -r -nostdlib)

target_link_libraries(${MODULE_BIGO_NAME} PRIVATE
    $<BUILD_INTERFACE:${MODULE_CONFIG_PRIVATE}>
    $<BUILD_INTERFACE:${LOS_CC_PROP_INTF_PUB}>
    $<BUILD_INTERFACE:${VENDOR_CC_PROP_INTF_PUB}>
)

set_target_properties(${MODULE_BIGO_NAME}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)
endif()
