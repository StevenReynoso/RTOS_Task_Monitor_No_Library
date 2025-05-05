
MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 512K    
    SRAM  (rwx) : ORIGIN = 0x20000000, LENGTH = 128K   
}

/* Initial main stack pointer (MSP) for startup */
_estack = ORIGIN(SRAM) + LENGTH(SRAM);

ENTRY(Reset_Handler)

SECTIONS
{
    .isr_vector :
    {
        KEEP(*(.isr_vector))
    } > FLASH

    .text :
    {
        *(.text*)
        *(.rodata*)
        _etext = .;
    } > FLASH

    .data : AT(_etext)
    {
        _sdata = .;
        *(.data*)
        _edata = .;
    } > SRAM

    .bss :
    {
        _sbss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
    } > SRAM

    /* Optional: Provide space after BSS for custom heap/task stacks */
    . = ALIGN(8);
    _heap_start = .;

    /* Reserve ~4KB for heap or RTOS task stacks */
    . = . + 0x1000;
    _heap_end = .;

    . = ALIGN(8);
    _end = .;
}
