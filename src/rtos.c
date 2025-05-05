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
    // Make sure stack is initialized to a known value (optional)
    for (int i = 0; i < STACK_SIZE; i++) {
        task->stack[i] = 0xDEADBEEF;
    }
    
    // Ensure 8-byte alignment for stack
    uint32_t *stack = (uint32_t*)((((uint32_t)(task->stack + STACK_SIZE) - 16*4) & ~0x7));
    
    // Initialize stack frame - order matters here!
    // This is the frame that will be restored on first context switch
    stack[0] = 0x01000000;      // xPSR (Thumb bit set)
    stack[1] = (uint32_t)task_func; // PC (entry point)
    stack[2] = 0xFFFFFFFF;      // LR (unused)
    stack[3] = 0;               // R12
    stack[4] = 0;               // R3
    stack[5] = 0;               // R2
    stack[6] = 0;               // R1
    stack[7] = 0;               // R0
    stack[8] = 0;               // R11
    stack[9] = 0;               // R10
    stack[10] = 0;              // R9
    stack[11] = 0;              // R8
    stack[12] = 0;              // R7
    stack[13] = 0;              // R6
    stack[14] = 0;              // R5
    stack[15] = 0;              // R4
    
    // Store stack pointer in TCB
    task->stack_pointer = stack;
}


/* void __attribute__((naked)) PendSV_Handler(void) {
    __asm volatile (
        // Save context of current task
        "mrs r0, psp              \n" // Get current process stack pointer
        "stmdb r0!, {r4-r11}      \n" // Save core registers
        
        // Store current PSP in TCB
        "ldr r1, =current_task    \n"
        "ldr r1, [r1]             \n"
        "str r0, [r1]             \n" // Save stack pointer to current task's TCB
        
        // Call scheduler with proper C function call standard
        "push {r0, lr}            \n" // Save registers that might be modified
        "bl schedule              \n" // Call scheduler
        "pop {r0, lr}             \n" // Restore registers
        
        // Load new task's context
        "ldr r0, =current_task    \n"
        "ldr r0, [r0]             \n"
        "ldr r0, [r0]             \n" // Get new task's stack pointer
        
        // Restore context
        "ldmia r0!, {r4-r11}      \n" // Restore core registers
        
        "msr psp, r0              \n" // Update PSP with new stack pointer
        "bx lr                    \n" // Return to thread mode
    );
}
*/

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

