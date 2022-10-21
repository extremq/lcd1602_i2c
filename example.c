#include "lcd1602_i2c.h"

int main() {
    /* init */
    LCD my_lcd;
    lcd_init(&my_lcd, 0x27, 4, 5, i2c0);

    /* Test that uses every function */
    char* message[] = {"Hello world!!!!", "humans", "LOL!"};
    sleep_ms(1000);
    lcd_string(&my_lcd, message[0]); // Send a string to the screen
    sleep_ms(1000);
    lcd_set_cursor_pos(&my_lcd, 0, 6); // Move the cursor
    sleep_ms(1000);
    lcd_shift_display(&my_lcd, true, 3); // Shift the display
    sleep_ms(1000);
    lcd_shift_display(&my_lcd, false, 3);
    sleep_ms(1000);
    lcd_string(&my_lcd, message[1]);
    sleep_ms(1000);
    lcd_shift_cursor(&my_lcd, false, 3); // Shift the cursor
    sleep_ms(1000);
    lcd_shift_cursor(&my_lcd, true, 3);
    sleep_ms(1000);
    lcd_set_cursor_pos(&my_lcd, 1, 6);
    sleep_ms(1000);
    lcd_show_cursor(&my_lcd, true); // Show the cursor
    sleep_ms(1000);
    lcd_set_blinking(&my_lcd, true); // Blink the character
    sleep_ms(1000);
    lcd_set_blinking(&my_lcd, false);
    sleep_ms(1000);
    lcd_show_cursor(&my_lcd, false);
    sleep_ms(1000);
    lcd_string(&my_lcd, message[2]);
    sleep_ms(1000);
    lcd_set_display(&my_lcd, 0); // Turn off the display
    sleep_ms(1000);
    lcd_set_display(&my_lcd, 1);
    sleep_ms(1000);
    lcd_set_backlight(&my_lcd, 0); // Turn off the backlight
    sleep_ms(1000);
    lcd_set_backlight(&my_lcd, 1);
    while (1) {
    }
}
