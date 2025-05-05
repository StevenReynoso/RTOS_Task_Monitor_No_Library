#include "gpio.h"
#include "rtos.h"
#include "lcd.h"
#include "systick.h"
#include "system.h"

#define SCB_SHCSR (*(volatile uint32_t*)0xE000ED24)


void task1(void)
{
    lcd_set_cursor(0, 0);
    lcd_print("Task 1 Running ");
    while (1);
}

void task2(void)
{
    lcd_set_cursor(0, 1);
    lcd_print("Task 2 Running ");
    while (1);
}

int main(void)
{
    //                 D7 is also BF (Busy Flag)
    // bit manipulation sheet D7 | D6 | D5 | D4 | R/W | E | RS | , documentation says DB4 - DB7 on LCD is for 4 or 8 bit transfer, Not DB0-DB3
    //     All Port A -      8     7    6    5   10    1   0
    gpio_config_t lcd_cfg5 = {
        .pin = PIN('A', 5),
        .mode = GPIO_MODE_OUTPUT,
        .otype = GPIO_OTYPE_PUSHPULL,
        .speed = GPIO_SPEED_HIGH,
        .pull = GPIO_NO_PULL};

    gpio_config_t lcd_cfg6 = {
        .pin = PIN('A', 6),
        .mode = GPIO_MODE_OUTPUT,
        .otype = GPIO_OTYPE_PUSHPULL,
        .speed = GPIO_SPEED_HIGH,
        .pull = GPIO_NO_PULL};
    gpio_config_t lcd_cfg7 = {
        .pin = PIN('A', 7),
        .mode = GPIO_MODE_OUTPUT,
        .otype = GPIO_OTYPE_PUSHPULL,
        .speed = GPIO_SPEED_HIGH,
        .pull = GPIO_NO_PULL};
    gpio_config_t lcd_cfg8 = {
        .pin = PIN('A', 8),
        .mode = GPIO_MODE_OUTPUT,
        .otype = GPIO_OTYPE_PUSHPULL,
        .speed = GPIO_SPEED_HIGH,
        .pull = GPIO_NO_PULL};

    gpio_config_t RS_cfg = {
        .pin = PIN('A', 0),
        .mode = GPIO_MODE_OUTPUT,
        .otype = GPIO_OTYPE_PUSHPULL,
        .speed = GPIO_SPEED_HIGH,
        .pull = GPIO_NO_PULL};

    gpio_config_t En_cfg = {
        .pin = PIN('A', 1),
        .mode = GPIO_MODE_OUTPUT,
        .otype = GPIO_OTYPE_PUSHPULL,
        .speed = GPIO_SPEED_HIGH,
        .pull = GPIO_NO_PULL};

    gpio_config_t RW_cfg = {
        .pin = PIN('A', 10),
        .mode = GPIO_MODE_OUTPUT,
        .otype = GPIO_OTYPE_PUSHPULL,
        .speed = GPIO_SPEED_HIGH,
        .pull = GPIO_NO_PULL};

    gpio_config_t btn_cfg = {
        .pin = PIN('A', 9),
        .mode = GPIO_MODE_INPUT,
        .otype = GPIO_OTYPE_PUSHPULL,
        .speed = GPIO_SPEED_HIGH,
        .pull = GPIO_NO_PULL};


    SysTick_init(SystemCoreClock / 1000); // 1ms
    __asm volatile ("cpsie i");  // <--- ENABLE IRQs BEFORE USING delay_ms()

    SCB_SHCSR |= (1 << 11); 

    gpio_init_pin(lcd_cfg5);
    gpio_init_pin(lcd_cfg6);
    gpio_init_pin(lcd_cfg7);
    gpio_init_pin(lcd_cfg8);
    gpio_init_pin(RS_cfg);
    gpio_init_pin(En_cfg);
    gpio_init_pin(RW_cfg);
    gpio_init_pin(btn_cfg);

    // testing to see if i get stuck.
    volatile uint32_t temp_time = current_time;
    while (current_time == temp_time); // Wait for current_time to change


    lcd_init();
    lcd_clear();

    // Set up tasks
    task_stack_init(&tasks[0], task1);
    task_stack_init(&tasks[1], task2);

    start_scheduler(); // starts PendSV/context switcher

    while (1);
}