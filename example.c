#include "lcd1602_i2c.h"

int main() {
    lcd_use_default_i2c_pins();
    lcd_init(0x27);

    char* message[] = {"Hello world", "humans", "LOL!"};
    sleep_ms(2000);
    lcd_string(message[0]);
    sleep_ms(2000);
    lcd_set_cursor(0, 6);
    sleep_ms(2000);
    lcd_string(message[1]);
    sleep_ms(2000);
    lcd_set_cursor(1, 0);
    sleep_ms(2000);
    lcd_string(message[2]);
    sleep_ms(2000);
    lcd_set_display(0);
    sleep_ms(1000);
    lcd_set_display(1);
    sleep_ms(1000);
    lcd_set_backlight(0);
    sleep_ms(1000);
    lcd_set_backlight(1);
    while (1) {
    }
}
