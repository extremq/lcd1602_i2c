#include "lcd1602_i2c.h"

uint8_t addr = 0x27;

void i2c_write_byte(uint8_t val) {
    i2c_write_blocking(i2c_default, addr, &val, 1, false);
}

void lcd_toggle_enable(uint8_t val, const uint64_t delay_us) {
    sleep_us(delay_us);
    i2c_write_byte(val | LCD_ENABLE_PIN);
    sleep_us(delay_us);
    i2c_write_byte(val & ~LCD_ENABLE_PIN);
}

void lcd_send_byte(uint8_t val, uint8_t mode, uint64_t delay_us) {
    // Move the data bits in the appropriate place
    uint8_t high = mode | (val & 0xF0)  | LCD_BACKLIGHT;
    uint8_t low = mode | ((val << 4) & 0xF0) | LCD_BACKLIGHT;

    // Send two transfers
    lcd_toggle_enable(high, delay_us);
    lcd_toggle_enable(low, delay_us);
}

void lcd_send_byte_no_backlight(uint8_t val, uint8_t mode, uint64_t delay_us) {
    // Move the data bits in the appropriate place
    uint8_t high = mode | (val & 0xF0);
    uint8_t low = mode | ((val << 4) & 0xF0);

    // Send two transfers
    lcd_toggle_enable(high, delay_us);
    lcd_toggle_enable(low, delay_us);
}

void lcd_use_default_i2c_pins(void) {
    i2c_init(i2c_default, STANDARD_RATE);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
}

void lcd_init(int address) {
    // The initialization is from page 46 of the HD44780U datasheet.
    addr = (address == -1) ? 0x27: (uint8_t) address;

    // Sleeps are for the display, taken from the same page.
    sleep_us(40000);
    lcd_toggle_enable(LCD_FUNCTIONSET | LCD_8BITMODE, FAST_DELAY);
    sleep_us(4100);
    lcd_toggle_enable(LCD_FUNCTIONSET | LCD_8BITMODE, FAST_DELAY);
    sleep_us(100);
    lcd_toggle_enable(LCD_FUNCTIONSET | LCD_8BITMODE, FAST_DELAY);
    lcd_toggle_enable(LCD_FUNCTIONSET, FAST_DELAY);

    lcd_send_byte(LCD_ENTRYMODESET | LCD_ENTRYLEFT, LCD_COMMAND_MODE, FAST_DELAY);
    lcd_send_byte(LCD_FUNCTIONSET | LCD_2LINE, LCD_COMMAND_MODE, FAST_DELAY);
    lcd_send_byte(LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSORON | LCD_BLINKON,  LCD_COMMAND_MODE, FAST_DELAY);

    lcd_clear();
}

void lcd_clear(void) {
    // Watch out, it's slow (2ms)
    lcd_send_byte(LCD_CLEARDISPLAY, LCD_COMMAND_MODE, SLOW_DELAY);
}

void lcd_char(char val) {
    lcd_send_byte(val, LCD_CHAR_MODE, SLOW_DELAY);
}

void lcd_string(const char *s) {
    while (*s) {
        lcd_char(*s++);
    }
}

void lcd_set_cursor(uint8_t line, uint8_t position) {
    // Cannot move past these boundaries
    if (position >= MAX_CHARS || line >= MAX_LINES) {
        return;
    }
    uint8_t val = (line == 0) ? 0x80 + position : 0xC0 + position;
    lcd_send_byte(val, LCD_COMMAND_MODE, FAST_DELAY);
}

void lcd_set_display(uint8_t set_display) {
    if (set_display) {
        lcd_send_byte(LCD_DISPLAYCONTROL | LCD_DISPLAYON, LCD_COMMAND_MODE, FAST_DELAY);
    }
    else {
        lcd_send_byte(LCD_DISPLAYCONTROL, LCD_COMMAND_MODE, FAST_DELAY);
    }
}

void lcd_set_backlight(uint8_t set_backlight) {
    // Make use of display, so we preserve the state of the memory
    if (set_backlight) {
        lcd_send_byte(LCD_DISPLAYCONTROL | LCD_DISPLAYON, LCD_COMMAND_MODE, FAST_DELAY);
    }
    else {
        lcd_send_byte_no_backlight(LCD_DISPLAYCONTROL, LCD_COMMAND_MODE, FAST_DELAY);
    }
}
