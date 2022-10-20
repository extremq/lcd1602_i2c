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
 * D7 D6 D5 D4 Bck E RW RS
 *
 * Then, send the low part of the 8 data bits
 * D3 D2 D1 D0 Bck E RW RS
 *
 * The only time we don't respect this pair is when
 * the LCD is initialized. Only 4 data bits are needed
 * for that.
 *
 * Explanations for RW, RS, E are on pages 24-25
 * of the HD44780U datasheet pdf.
 *
 * Bck comes from backlight and it sets the backlight.
 */
#ifndef __LCD1602_I2C_H
#define __LCD1602_I2C_H

#include "hardware/i2c.h"
#include "pico/stdlib.h"

/* Uses I2C 100khz mode */
#define STANDARD_RATE 100000

/*
 * All these define statemens are based
 * on pages 24-25 of the HD44780U datasheet pdf.
 */

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

/* 1602 config */
#define MAX_LINES 2
#define MAX_CHARS 16

/* Delay values in microseconds, check page 24 */
#define FAST_DELAY 80
#define SLOW_DELAY 2000 // Used for Returning home and Clearing the display

/*
 * One byte struct describing an LCD status.
 */
typedef struct _LCD_STATUS {
    /*
     * Cursor_direction increment/decrement - I_D
     * Cursor on/off - C
     * Display shift - S
     * Display on/off - D
     * Blinking on/off - B
     * Backlight on/off - Bck
     * Shift display/cursor - S/C
     * Shift to the right/left - R/L
     */

    uint8_t cursor_dir : 1;
    uint8_t cursor_on : 1;
    uint8_t display_shift : 1;
    uint8_t display_on : 1;
    uint8_t blinking_on : 1;
    uint8_t backlight_on : 1;
    uint8_t shift_display : 1;
    uint8_t shift_right : 1;
} LCD_STATUS;

typedef struct _LCD {
    /*
     * Bus address used by the I2C chip.
     * May be 0x3F or 0x27, depending on manufacturer
     */
    uint8_t addr;

    /*
     * Pins used for this lcd.
     */
    uint8_t pin_sda, pin_scl;

    /*
     * Prefer to use uint8_t instead of i2c_inst_t.
     * My method uses 1 byte, storing the former uses 8 bytes
     * and will most likely extend my struct to 16 bytes.
     */
    uint8_t i2c_type;

    LCD_STATUS status;
} LCD;

/* Helper function for interfacing with pico-sdk */
void i2c_write_byte (struct _LCD* lcd_inst, uint8_t val);

/*
 * The actual sending of data by enabling and disabling the E pin
 * Has a delay_us parameter since some commands differ in time needed
 */
void lcd_toggle_enable (LCD* lcd_inst, uint8_t val, uint64_t delay_us);

/*
 * Sends two separate nibbles (4-bits), explained at the beginning
 * of this header file.
 */
void send_byte (LCD* lcd_inst, uint8_t val, uint8_t mode, uint64_t delay_us);

/*
 *  The initialization protocol is from page 46
 *  of the HD44780U datasheet pdf.
 *
 *  Set address to -1 to use default (0x27).
 *
 *  Check the pinouts to know which gpio pins (counted from gp0 to gp28)
 *  have the I2C SDA and I2C SCL feature.
 *  i2c_inst needs either i2c0 or i2c1 as a parameter.
 *
 *  Example:
 *  GP4, GP5 as SDA, SCL need i2c0.
 *  GP6, GP7 as SDA, SCL need i2c1.
 */
void lcd_init (LCD* lcd_inst, int address, uint8_t pin_sda, uint8_t pin_scl, i2c_inst_t* i2c_inst);

/*
 * Clears the display
 */
void lcd_clear(LCD* lcd_inst);

/*
 * Puts a character on the screen
 */
void lcd_char(LCD* lcd_inst, char val);


/*
 * Puts a string on the screen
 * Remember, the lcd has 80 bytes for display memory,
 * that means 80 characters.
 * You can fill 2 and a half screens with this much
 */
void lcd_string(LCD* lcd_inst, const char *s);

/*
 * Moves the cursor somewhere inside 0, 0 to 1, 15
 */
void lcd_set_cursor_pos(LCD* lcd_inst, uint8_t line, uint8_t position);

/*
 * Turn display on or off (hides characters)
 */
void lcd_set_display(LCD* lcd_inst, uint8_t set_display);

/*
 * Turn backlight on or off (turns off the screen)
 */
void lcd_set_backlight(LCD* lcd_inst, uint8_t set_backlight);

/*
 * Show or hide the cursor under characters
 */
void lcd_show_cursor(LCD* lcd_inst, uint8_t show_cursor);

/*
 * Show or hide blinking of characters
 */
void lcd_set_blinking(LCD* lcd_inst, uint8_t set_blinking);

/*
 * Shifts the display right or left by amount
 * You can shift it outside the screen, doesn't matter
 */
void lcd_shift_display(LCD* lcd_inst, uint8_t move_right, uint8_t amount);

/*
 * Shifts the cursor right or left by amount
 * You can shift it outside, doesn't matter
 * Remember to bring cursor back
 */
void lcd_shift_cursor(LCD* lcd_inst, uint8_t move_right, uint8_t amount);
#endif //__LCD1602_I2C_H
