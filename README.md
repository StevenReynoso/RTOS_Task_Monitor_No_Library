# STM32 Bare-Metal RTOS (Cooperative Task Scheduler)

This project implements a minimal, no-libraries cooperative RTOS from scratch on the STM32F446RE microcontroller. It manages multiple tasks using a simple round-robin scheduler and displays system output on a 16x2 LCD. All hardware interactions are done through direct register access.

## Features
- Written entirely in C with **no HAL or CMSIS libraries**
- Basic **cooperative multitasking** (3 tasks: LCD print, heartbeat, button press)
- Uses **SysTick** for millisecond timing and task delay control
- LCD1602 display integration via 4-bit GPIO interface
- Button handling on **PA9** with software debounce
- Task scheduling and internal state debugged using **GDB**

## Tasks Implemented
| Task Name       | Behavior                        | Interval |
|----------------|----------------------------------|----------|
| task_blink      | Displays "Blinking..."          | 1000 ms  |
| task_heartbeat  | Displays "Task 2 is Alive"      | 2000 ms  |
| task_button_check | Detects button press on PA9 and prints "Button Pressed!" | 50 ms debounce check |

## Tools
- STM32F446RE Nucleo Board
- ARM-GCC toolchain
- GDB for live debugging
- Makefile build system (no IDE bloat)

## How It Works
- Each task is a function with an associated delay and ready flag.
- `SysTick_Handler` increments a global time counter every millisecond.
- `update_task_delays()` is called each loop to check if tasks should be reactivated.
- Tasks self-schedule their next delay inside their own logic.
- `run_scheduler()` loops through task slots and runs ready tasks.

## Getting Started
1. Clone repo
2. Flash to STM32F4 with your preferred tool (ST-Link, OpenOCD, etc.)
3. Connect LCD (PA5–PA8 for data, PA0, PA1, PA10 for control)
4. Connect pushbutton to PA9 and GND
5. Power on and watch tasks rotate live on LCD

## Roadmap
- [ ] Add task state monitor to LCD
- [ ] Implement task counters
- [ ] Add UART logging
- [ ] Support dynamic task creation
- [ ] Preemptive scheduling via timer ISR

## Author
Steven Reynoso  
M.S. Computer Engineering, University of Michigan - Dearborn  
https://github.com/StevenReynoso
