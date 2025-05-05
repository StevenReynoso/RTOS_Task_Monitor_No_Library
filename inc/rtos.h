#ifndef RTOS_H
#define RTOS_H

#include <stdint.h>

#define MAX_TASKS  3
#define STACK_SIZE 128

#define SCB_SHCRS  (*(volatile uint32_t*)0xE000ED24)
#define SCB_ICSR   (*(volatile uint32_t*)0xE000ED04)

#define SCB_ICSR_PENDSVSET   (1 << 28)     // pendsvset RW Write 0 = no effect, 1 changes pendsv to read, read : 0 exception not pending, 1: is pending


typedef enum {
    TASK_READY   = 0,
    TASK_RUNNING = 1,
    TASK_BLOCKED = 2
} task_state_t;

typedef struct {
    uint32_t *stack_pointer;
    uint32_t stack[STACK_SIZE];
    uint8_t id;
    const char *name;
    task_state_t state;
} TCB_t;

extern TCB_t tasks[MAX_TASKS];
extern TCB_t *current_task;
extern int current_task_index;

void task_stack_init(TCB_t *task, void (*task_func)(void));
void schedule(void);
void start_scheduler(void);

#endif // RTOS_H
