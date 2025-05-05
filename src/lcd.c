/*
    Writing mode Timing Characteristics.
    Enable Cycle Time       | Pin E             | 1200 ns
    Enable Pulse Width      | Pin E             | 140 ns
    Enable Rise / Fall Time | Pin E             | 25ns
    Address Setup Time      | Pins: RS, RW, E   | 0 ns
    Address Hold up         | Pins: RS, RW, E   | 10ns
    Data Setup Time         | Pins: DB0-DB7     | 40ns
    Data Hold Time          | Pins: DB0-DB7     | 10 ns

    Clear Display           | 1.52 ms
    Return Home             | 1.52 ms
    Entry Mode Set          | 37us
    Display On/Off          | 37us
    Cursor Display shift    | 37us
    function set            | 37us
    set CGRAM               | 37us
    set DDRAM               | 37us
    Read busy flag          | 0us
    write data to ram       | 37us
    read data from ram      | 37 us
*/


#include <stdint.h>
#include "gpio.h"
#include "rtos.h"
#include "systick.h"
#include "system.h"


void lcd_read_busy(void) {
    // Set D7 (PA8) as input
    gpio_set_mode(PIN('A', 8), GPIO_MODE_INPUT);
    
    GPIO('A')->ODR |= (1 << 10);  // RW = 1 (read mode)     // test light will turn on,
    GPIO('A')->ODR &= ~(1 << 0); // RS = 0 (instruction register)
    
    while (1) {
        // First nibble read (Busy flag on D7)
        GPIO('A')->ODR |= (1 << 1);  // E = 1
        delay_us(10);  // Let LCD output data
        
        // Read busy flag (D7) - busy when bit is 1
        uint8_t busy = (GPIO('A')->IDR & (1 << 8)) != 0;
        
        GPIO('A')->ODR &= ~(1 << 1);  // E = 0
        delay_us(10);  // Meet hold time
        
        // Second nibble read (ignored for busy check)
        GPIO('A')->ODR |= (1 << 1);  // E = 1
        delay_us(10);
        GPIO('A')->ODR &= ~(1 << 1);  // E = 0
        delay_us(10);
        
        if (!busy) break;  // Exit when busy flag is clear (0)
    }
    
    gpio_set_mode(PIN('A', 8), GPIO_MODE_OUTPUT);  // Restore D7 as output
    GPIO('A')->ODR &= ~(1 << 10);  // RW = 0 (write mode)       // test light off, 
}

static void lcd_send_nibble(uint8_t nibble){
    // turn off our pins, before activating with new nibble
    GPIO('A')->ODR &= ~((1 << 5) | (1 << 6) | (1 << 7) | (1 << 8));

    if (nibble & 0x01) GPIO('A')->ODR |= (1 << 5);
    if (nibble & 0x02) GPIO('A')->ODR |= (1 << 6);
    if (nibble & 0x04) GPIO('A')->ODR |= (1 << 7);
    if (nibble & 0x08) GPIO('A')->ODR |= (1 << 8);

}

static void lcd_pulse_enable(void) {  // without pulse LCD will ignore our data lines.
    GPIO('A')->ODR |= (1 << 1);
    delay_us(1);
    GPIO('A')->ODR &= ~(1 << 1);
    delay_us(100);                   // too low of delay will jam up data lines ?, gives bad characters
}

void lcd_send_cmd(uint8_t cmd) {
    GPIO('A')->ODR &= ~(1 << 0);  // RS = 0 for command     // puts our lcd in command mode from register select and does same as send data

    lcd_send_nibble(cmd >> 4);
    lcd_pulse_enable();
    lcd_send_nibble(cmd & 0x0F);
    lcd_pulse_enable();
    lcd_read_busy();
}

void lcd_send_data(uint8_t data) {      // first nibble MSB, then second nibble is LSB, to form the full 8 bit character instruction., pulses to wait for LCD to latch.
    GPIO('A')->ODR |= (1 << 0);  // RS = 1 for data

    lcd_send_nibble(data >> 4);  
    lcd_pulse_enable();
    lcd_send_nibble(data & 0x0F);
    lcd_pulse_enable();
    lcd_read_busy();

}

void lcd_print(const char *str) {   // using the pointer for our string to load in one char at a time to send data that will be split for our 4 bit data mode.

    while (*str) {
        lcd_send_data(*str++);
    }
}

void lcd_clear(void) {
    lcd_send_cmd(0x01);
    lcd_read_busy();
}

void lcd_set_cursor(uint8_t col, uint8_t row) {                 // bottom row starts at 0x40 
    uint8_t address = (row == 0) ? col : (0x40 + col);
    lcd_send_cmd(0x80 | address);
}

void lcd_init(void) {
    delay_ms(50);                                         // wait 15ms or more after vdd reaches 4.5v, wait 4.1 ms or more, wait 100 micro seconds, end of initialization

    lcd_send_nibble(0x03); // 8bit sync 1 // send 3 to db4 and db5
    lcd_pulse_enable();
    delay_ms(10);                                         // busy cant be read until after initialization complete

    lcd_send_nibble(0x03);  // 8bit sync 2  // send 3 to db4 and db5
    lcd_pulse_enable();
    delay_ms(10);

    lcd_send_nibble(0x03);  // 8 bit sync 3 // send 3 to db4 and db5
    lcd_pulse_enable();
    delay_ms(10);

    lcd_send_nibble(0x02);  // enter 4-bit mode
    lcd_pulse_enable();
    delay_ms(10);

    // Now you're in 4-bit mode — proceed normally
    lcd_send_cmd(0x28);  // Function set: 4-bit, 2 line, 5x8 font | 101000 in bin
    lcd_send_cmd(0x0C);  // Display ON, cursor OFF                |   1100 in bin
    lcd_send_cmd(0x01);  // Clear display
    lcd_send_cmd(0x06);  // Entry mode: increment                 |   0110  | can shift display with 0x07 and get a scrolling effect.
    delay_ms(2);
}

void lcd_task(void){
    while (1) {
        lcd_set_cursor(0, 1);
        lcd_print("Task:           ");
        lcd_set_cursor(6, 1);
        lcd_print(tasks[current_task_index].name);
        delay_ms(1000);
    }
}

void lcd_task_monitor(void){
    lcd_set_cursor(0, 1);  // Write on second line only
    lcd_print("Task:           ");  // Padding to erase leftovers

    while (1) {
        lcd_set_cursor(6, 1);  // Position after "Task: "
        lcd_print(tasks[current_task_index].name);
        delay_ms(1000);
    }
}