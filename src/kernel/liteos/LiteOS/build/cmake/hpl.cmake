if(LOSCFG_DEBUG_HPL_EMULATION)
    set(HPL_METADATA_FILE ${OUT}/hpl/preprocess/Metadata.c)
    set(HPL_METADATA_OBJECT ${OUT}/hpl/preprocess/Metadata.o)

    if (EXISTS ${HPL_METADATA_FILE})
        list(APPEND LITEOS_LD_COMMAND ${HPL_METADATA_OBJECT})
    endif()
endif()