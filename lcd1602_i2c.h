/*
 * Reference:
 * https://www.sparkfun.com/datasheets/LCD/HD44780.pdf
 *
 * I had to reverse engineer raspberry pi's
 * implementation since it was using weird and
 * contradictory ways of sending bytes using the
 * I2C protocol, even wasting time doing so.
 *
 * Inspired from
 * https://github.com/raspberrypi/pico-examples/
 *
 * What you need to know is that the LCD supports
 * two modes of data addressing: 8-bit and 4-bit.
 *
 * Since I2C can only send one byte of data at a
 * time, you'd think we could use the 8-bit mode
 * but no, we have 4 more pins we need to control.
 *
 * So, we use the 4-bit mode. The usual I2C module
 * used for extending the 1602 LCD will be configured
 * this way:
 *
 * First send the high part of the 8 data bits
 * D7 D6 D5 D4 B E RW RS
 *
 * Then, send the low part of the 8 data bits
 * D3 D2 D1 D0 B E RW RS
 *
 * The only time we don't respect this pair is when
 * the LCD is initialized. Only 4 data bits are needed
 * for that.
 *
 * Explanations for RW, RS, E are on pages 24-25
 * of the HD44780U datasheet pdf.
 *
 * B comes from backlight and it sets the backlight.
 */
#ifndef __LCD1602_I2C_H
#define __LCD1602_I2C_H

#include <stdio.h>
#include "hardware/i2c.h"
#include "pico/stdlib.h"

/* Uses I2C 100khz mode */
#define STANDARD_RATE 100000

/*
 * All these define statemens are based
 * on pages 24-25 of the HD44780U datasheet pdf.
 * /

/* Commands. */
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

/* Flags for Entry mode. */
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYLEFT 0x02

/* Flags for display and cursor control. */
#define LCD_BLINKON 0x01
#define LCD_CURSORON 0x02
#define LCD_DISPLAYON 0x04

/* Flags for display and cursor shift. */
#define LCD_MOVERIGHT 0x04
#define LCD_DISPLAYMOVE 0x08

/* Flags for function set. */
#define LCD_5x10DOTS 0x04
#define LCD_2LINE 0x08
#define LCD_8BITMODE 0x10

/* Flag for backlight control. */
#define LCD_BACKLIGHT 0x08

/*
 * E (Enable) pin is used to enable the display.
 * When this pin is set to LOW, the LCD does not
 * care what is happening on the R/W, RS, and data bus lines.
 * When this pin is set to HIGH, the LCD processes the incoming data.
 *
 * Explanation taken from
 * https://lastminuteengineers.com/arduino-1602-character-lcd-tutorial/
 */

#define LCD_ENABLE_PIN 0x04

/*
 * RS (Register Select) pin is set to LOW when sending commands
 * to the LCD (such as setting the cursor to a specific location, clearing the display, etc.)
 * and HIGH when sending data to the LCD. Basically this pin
 * is used to separate the command from the data.
 *
 * Explanation taken from
 * https://lastminuteengineers.com/arduino-1602-character-lcd-tutorial/
 */
#define LCD_COMMAND_MODE 0x00
#define LCD_CHAR_MODE 0x01

/*
 * Bus address used by the I2C chip.
 * May be 0x3F or 0x27, depending on manufacturer
 */
extern uint8_t addr;

/* 1602 config */
#define MAX_LINES 2
#define MAX_CHARS 16

/* Delay values in microseconds, check page 24 */
#define FAST_DELAY 80
#define SLOW_DELAY 2000 // Used for Returning home and Clearing the display

/* Helper function for interfacing with pico-sdk */
void i2c_write_byte(uint8_t val);

/*
 * The actual sending of data by enabling and disabling the E pin
 * Has a delay_us parameter since some commands differ in time needed
 */
void lcd_toggle_enable(uint8_t val, uint64_t delay_us);

/*
 * Sends two separate nibbles (4-bits), explained at the beginning
 * of this header file.
 */
void lcd_send_byte(uint8_t val, uint8_t mode, uint64_t delay_us);

/*
 * Just use the default i2c pins on Pico.
 * GPIO 4 (pin 6)-> SDA on LCD bridge board
 * GPIO 5 (pin 7)-> SCL on LCD bridge board
 */
void lcd_use_default_i2c_pins(void);

/*
 *  The initialization protocol is from page 46
 *  of the HD44780U datasheet pdf.
 *  Set address to -1 to use default (0x27).
 */
void lcd_init(int address);

/*
 * Clears the display
 */
void lcd_clear(void);

/*
 * Puts a character on the screen
 */
void lcd_char(char val);


/*
 * Puts a string on the screen
 */
void lcd_string(const char *s);

/*
 * Moves the cursor somewhere inside 0, 0 to 1, 15
 */
void lcd_set_cursor(uint8_t line, uint8_t position);

/*
 * Turn display on or off (hides characters)
 */
void lcd_set_display(uint8_t set_display);

/*
 * Turn backlight on or off (turns off the screen)
 */
void lcd_set_backlight(uint8_t set_backlight);

#endif //__LCD1602_I2C_H
