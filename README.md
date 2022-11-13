# lcd1602_i2c
A simple, lightning-fast and scalable I2C-based implementation of the HD44780U 1602 LCD with I2C interface in C using the Raspberry Pi Pico SDK.

![](https://i.imgur.com/s03eaDj.jpeg)

# Features
- Very efficient
  - Used the datasheet to provide minimal sleep times. On average, the display needs to sleep 160 microseconds for each command
  - Sends only the minimum amount of bytes necessary.
- Can be used to control multiple HD44780U 1602 LCDs.
  - Just create an LCD object and use its address for each call.
  - For example `lcd_char(&lcd_1, 'a'); lcd_char(&lcd_2, 'b');`.
- Heavily documented code.
  - Made sure to reference pages in the original datasheet.
  - Explained functions, variables and protocol. (Had to reverse engineer popular libraries, literally no documentation is provided for the I2C interface)


# How to install the module (for beginners)
The minimal way of adding this module to your CMake project is by downloading the `lcd1602_i2c.c` and `lcd1602_i2c.h` files
and placing them in the same folder as your main source.

From there, you have to add the files inside `add_executable`. For example:
```cmake
add_executable(my_project
        main.c
        lcd1602_i2c.c
        lcd1602_i2c.h
        )
```

DO NOT FORGET to include `hardware_i2c` in your `target_link_libraries`.
```cmake
target_link_libraries(my_project hardware_i2c pico_stdlib)
```

# Usage
Check example.c for a complete showcase. Minimal example here:
```c
#include "lcd1602_i2c.h"

int main() {
    /* Init my modules */
    LCD my_lcd;
    lcd_init(&my_lcd, 0x27, 4, 5, i2c0); // Check the pins on your Pico
    
    char message[] = "Thanks extremq";
    lcd_string(&my_lcd, message); 
}
```
