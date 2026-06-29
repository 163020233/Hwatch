    /* boxA text&rodata section */
    __box_text_rodata_start = BOXA_TEXT_RODATA_START;
    __boxA_text_rodata_start = BOXA_TEXT_RODATA_START;
    .boxA.text.rodata : {
        LIB_BOXA_FUNC(SORT_TEXT)
        LIB_BOXA_FUNC(SORT_RODATA)
        __boxA_text_rodata_end = .;
    } > FLASH_BOXA_CODE
    __boxA_text_rodata_size = __boxA_text_rodata_end - __boxA_text_rodata_start;
    ASSERT(__boxA_text_rodata_size <= BOXA_TEXT_RODATA_SIZE,
        "__boxA_text_rodata_size is larger than BOXA_TEXT_RODATA_SIZE")

    /* boxB text&rodata section */
    __boxB_text_rodata_start = BOXB_TEXT_RODATA_START;
    .boxB.text.rodata : {
        LIB_BOXB_FUNC(SORT_TEXT)
        LIB_BOXB_FUNC(SORT_RODATA)
        __boxB_text_rodata_end = .;
    } > FLASH_BOXB_CODE
    __boxB_text_rodata_size = __boxB_text_rodata_end - __boxB_text_rodata_start;
    __box_text_rodata_size = __boxB_text_rodata_end - __boxA_text_rodata_start;
    ASSERT(__boxB_text_rodata_size <= BOXB_TEXT_RODATA_SIZE,
        "__boxB_text_rodata_size is larger than BOXB_TEXT_RODATA_SIZE")

    /* boxA data section */
    __boxA_all_data_start = BOXA_DATA_START;
    .boxA.heap (NOLOAD): {
        __boxA_heap_start = .;
        . += BOXA_HEAP_SIZE;
        __boxA_heap_end = .;
    } > DTCM_BOXA_DATA
    __boxA_heap_size = __boxA_heap_end - __boxA_heap_start;

    __boxA_stack_start = BOXA_STACK_START;
    .boxA.stack (NOLOAD): {
        . += BOXA_STACK_SIZE;
        __boxA_stack_top = .;
    } > DTCM_BOXA_DATA
    __boxA_stack_size = __boxA_stack_top - __boxA_stack_start;

    __boxA_data_start = .;
    .boxA.data : ALIGN(0x20) {
        LIB_BOXA_FUNC(SORT_DATA)
        . = ALIGN(0x20);
    } > DTCM_BOXA_DATA AT > FLASH_EXT
    __boxA_data_load_start = LOADADDR(.boxA.data);
    __boxA_data_load_size = SIZEOF(.boxA.data);

    /* boxA bss section */
    .boxA.bss (NOLOAD) : ALIGN(0x20) {
        __boxA_bss_start = .;
        LIB_BOXA_FUNC(SORT_BSS)
        __boxA_bss_end = .;
        __boxA_all_data_end = .;
    } > DTCM_BOXA_DATA
    __boxA_bss_size = __boxA_bss_end - __boxA_bss_start;
    __boxA_data_size = __boxA_all_data_end - __boxA_all_data_start;

    /* boxB data section */
    __boxB_all_data_start = BOXB_DATA_START;
    .boxB.heap (NOLOAD) : ALIGN(0x20)
    {
        __boxB_heap_start = .;
        . += BOXB_HEAP_SIZE;
        __boxB_heap_end = .;
    } > DTCM_BOXB_DATA
    __boxB_heap_size = __boxB_heap_end - __boxB_heap_start;

    __boxB_stack_start = BOXB_STACK_START;
    .boxB.stack (NOLOAD): {
        . += BOXB_STACK_SIZE;
        __boxB_stack_top = .;
    } > DTCM_BOXB_DATA
    __boxB_stack_size = __boxB_stack_top - __boxB_stack_start;

    __boxB_data_start = .;
    .boxB.data : ALIGN(0x20) {
        LIB_BOXB_FUNC(SORT_DATA)
        . = ALIGN(0x20);
    } > DTCM_BOXB_DATA AT > FLASH_EXT
    __boxB_data_load_start = LOADADDR(.boxB.data);
    __boxB_data_load_size = SIZEOF(.boxB.data);

    /* boxB bss section */
    .boxB.bss (NOLOAD) : ALIGN(0x20) {
        __boxB_bss_start = .;
        LIB_BOXB_FUNC(SORT_BSS)
        __boxB_bss_end = .;
        __boxB_all_data_end = .;
    } > DTCM_BOXB_DATA
    __boxB_bss_size = __boxB_bss_end - __boxB_bss_start;
    __boxB_data_size = __boxB_all_data_end - __boxB_all_data_start;
