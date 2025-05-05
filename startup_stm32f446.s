
.syntax unified                                
.cpu cortex-m4                                  
.thumb                                          

.section .isr_vector, "a", %progbits
.global _vector_table
.type _vector_table, %object
.size _vector_table, . - _vector_table

_vector_table:
    .word _estack
    .word Reset_Handler
    .word NMI
    .word Hard_Fault
    .word Mem_Manage
    .word Bus_Fault
    .word Usage_Fault
    .word 0
    .word 0
    .word 0
    .word 0
    .word SVCall
    .word Debug_Monitor
    .word 0
    .word PendSV_Handler
    .word SysTick_Handler



.section .text                                  
.global Reset_Handler
.type Reset_Handler, %function


Reset_Handler:
    ldr r0, =_sdata                             
    ldr r1, =_etext                             
    ldr r2, =_edata                             
                    
.data_copy:                 
    cmp r0, r2                                  
    ittt lt                 
    ldrlt r3, [r1], #4                  
    strlt r3, [r0], #4                  
    blt .data_copy                  
                    
    ldr r0, =_sbss                              
    ldr r1, =_ebss                              
    movs r2, #0                                 
                    
.bss_clear:                 
    cmp r0, r1                                 
    it lt                   
    strlt r2, [r0], #4                  
    blt .bss_clear                  
                    
    bl main                                    
                    
infinite_loop:                  
    b infinite_loop                             


.weak NMI, Hard_Fault, Mem_Manage, Bus_Fault, Usage_Fault, SVCall, Debug_Monitor,
.thumb_set NMI,                   infinite_loop
.thumb_set Hard_Fault,            infinite_loop
.thumb_set Mem_Manage,            infinite_loop
.thumb_set Bus_Fault,             infinite_loop
.thumb_set Usage_Fault,           infinite_loop
.thumb_set SVCall,                infinite_loop
.thumb_set Debug_Monitor,         infinite_loop