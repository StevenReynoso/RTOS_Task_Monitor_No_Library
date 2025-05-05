#include <stdint.h>
#include <stddef.h>
#include "rtos.h"
#include "system.h"

TCB_t tasks[MAX_TASKS];
TCB_t *current_task = NULL;
int current_task_index = 0;

static inline void __set_PSP(uint32_t topOfProcStack) {
    __asm volatile ("msr psp, %0" : : "r" (topOfProcStack) : );
}

void task_stack_init(TCB_t *task, void (*task_func)(void)) {
    uint32_t *stack = task->stack + STACK_SIZE - 16;

    stack[8]  = 0x01000000;         // xPSR
    stack[9]  = (uint32_t)task_func;// PC
    stack[14] = 0xFFFFFFFD;         // LR

    task->stack_pointer = stack;
}

void PendSV_Handler(void) {
    __asm volatile (
        "mrs r0, psp\n"            // Load current PSP
        "stmdb r0!, {r4-r11}\n"    // Save R4-R11 to current task stack
        "ldr r1, =current_task\n"
        "ldr r2, [r1]\n"
        "str r0, [r2]\n"           // Save updated stack pointer

        "bl schedule\n"            // Pick next task

        "ldr r1, =current_task\n"
        "ldr r2, [r1]\n"
        "ldr r0, [r2]\n"           // Load new stack pointer
        "ldmia r0!, {r4-r11}\n"    // Restore R4-R11
        "msr psp, r0\n"            // Set new PSP
        "bx lr\n"
    );
}

void schedule(void) {
    current_task_index = (current_task_index + 1) % MAX_TASKS;
    current_task = &tasks[current_task_index];
}

void start_scheduler(void) {
    current_task_index = 0;
    current_task = &tasks[0];

    __set_PSP((uint32_t)current_task->stack_pointer);
    
    __asm volatile (
        "mov r0, #0x03\n"
        "msr CONTROL, r0\n"
        "isb\n"
    );

    // trigger first context switch
    SCB_ICSR |= SCB_ICSR_PENDSVSET;

    while (1); // safety loop
}

