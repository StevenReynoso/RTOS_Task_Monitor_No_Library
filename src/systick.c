#include "systick.h"
#include "rtos.h"
#include "system.h"
#include <stdint.h>

volatile uint32_t current_time = 0;

void delay_us(uint32_t us){
    uint32_t ticks = us * (SystemCoreClock / 1000000);
    uint32_t start = SYST_CVR;

    while ((start - SYST_CVR) < ticks) {
        if (SYST_CVR > start) break;  // handles wrap-around
    }
}

void delay_ms(uint32_t ms) {
    if (ms == 0) return;
    uint32_t start = current_time;
    while ((current_time - start) < ms);
}

__attribute__((naked)) void SysTick_Handler(void){
    current_time++;
    SCB_ICSR |= SCB_ICSR_PENDSVSET;
}

uint32_t millis(void) {
    return current_time;
}

int SysTick_init(uint32_t ticks){
    if(ticks > 0xFFFFFF){
        return 1;
    }

    SYST_RVR = ticks - 1;
    SYST_CVR = 0;
    SYST_CSR = SYST_CTRL_ENABLE | SYST_CTRL_TICKINT | SYST_CTRL_CLKSOURCE;

    return 0;

}