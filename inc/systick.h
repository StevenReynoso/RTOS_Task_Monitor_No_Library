#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdint.h>

extern volatile uint32_t current_time;

#define SYST_CSR (*(volatile uint32_t *) (0xE000E010))
#define SYST_RVR (*(volatile uint32_t *) (0xE000E014))
#define SYST_CVR (*(volatile uint32_t *) (0xE000E018))

#define SYST_CTRL_ENABLE     (1 << 0)
#define SYST_CTRL_TICKINT    (1 << 1)
#define SYST_CTRL_CLKSOURCE  (1 << 2)

int SysTick_init(uint32_t ticks);
void SysTick_Handler(void);
void delay_ms(uint32_t ms);
void delay_us(uint32_t us);
uint32_t millis(void);

#endif //SYSTICK_H