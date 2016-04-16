/*
 * TinyOLED - Driver for SSD1306 controlled dot matrix OLED/PLED 128x64 displays
 * from an attiny85 chip.
 *
 * Converted to C++ and Arduino compatible by richardkchapman@gmail.com
 *
 * Based in part on work by Neven Boyanov, Tinusaur Team
 * See http://tinusaur.org and https://bitbucket.org/tinusaur/ssd1306xled
 *
 * Some code based on "IIC_wtihout_ACK" by http://www.14blog.com/archives/1358 (no longer online)
 */

// ----------------------------------------------------------------------------

#include <stdlib.h>
#include <avr/io.h>

#include <avr/pgmspace.h>

#include "TinyOLED.h"
#include "font6x8.h"
#include "font8x16.h"

// ----------------------------------------------------------------------------

// Convenience definitions for PORTB
// Note that we go direct to port rather than using digitalWrite as it's much faster

#define DIGITAL_WRITE_HIGH(PORT) PORTB |= (1 << PORT)
#define DIGITAL_WRITE_LOW(PORT) PORTB &= ~(1 << PORT)
#define SET_OUTPUT(PORT) DDRB |= (1 << PORT);

// ----------------------------------------------------------------------------


const uint8_t tinyOLED::init_sequence [] PROGMEM = {	// Initialization Sequence
	0xAE,			// Display OFF (sleep mode)
	0x20, 0b00,		// Set Memory Addressing Mode
					// 00=Horizontal Addressing Mode; 01=Vertical Addressing Mode;
					// 10=Page Addressing Mode (RESET); 11=Invalid
	0xB0,			// Set Page Start Address for Page Addressing Mode, 0-7
	0xC8,			// Set COM Output Scan Direction
	0x00,			// ---set low column address
	0x10,			// ---set high column address
	0x40,			// --set start line address
	0x81, 0x3F,		// Set contrast control register
	0xA1,			// Set Segment Re-map. A0=address mapped; A1=address 127 mapped. 
	0xA6,			// Set display mode. A6=Normal; A7=Inverse
	0xA8, 0x3F,		// Set multiplex ratio(1 to 64)
	0xA4,			// Output RAM to Display
					// 0xA4=Output follows RAM content; 0xA5,Output ignores RAM content
	0xD3, 0x00,		// Set display offset. 00 = no offset
	0xD5,			// --set display clock divide ratio/oscillator frequency
	0xF0,			// --set divide ratio
	0xD9, 0x22,		// Set pre-charge period
	0xDA, 0x12,		// Set com pins hardware configuration		
	0xDB,			// --set vcomh
	0x20,			// 0x20,0.77xVcc
	0x8D, 0x14,		// Set DC-DC enable
	0xAF			// Display ON in normal mode
	
};

// ----------------------------------------------------------------------------

// These function should become separate library for handling I2C simplified output.

void tinyOLED::xfer_start(void)
{
	DIGITAL_WRITE_HIGH(SSD1306_SCL);	// Set to HIGH
	DIGITAL_WRITE_HIGH(SSD1306_SDA);	// Set to HIGH
	DIGITAL_WRITE_LOW(SSD1306_SDA);		// Set to LOW
	DIGITAL_WRITE_LOW(SSD1306_SCL);		// Set to LOW
}

void tinyOLED::xfer_stop(void)
{
	DIGITAL_WRITE_LOW(SSD1306_SCL);		// Set to LOW
	DIGITAL_WRITE_LOW(SSD1306_SDA);		// Set to LOW
	DIGITAL_WRITE_HIGH(SSD1306_SCL);	// Set to HIGH
	DIGITAL_WRITE_HIGH(SSD1306_SDA);	// Set to HIGH
}

void tinyOLED::send_byte(uint8_t byte)
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		if ((byte << i) & 0x80)
			DIGITAL_WRITE_HIGH(SSD1306_SDA);
		else
			DIGITAL_WRITE_LOW(SSD1306_SDA);
		
		DIGITAL_WRITE_HIGH(SSD1306_SCL);
		DIGITAL_WRITE_LOW(SSD1306_SCL);
	}
	DIGITAL_WRITE_HIGH(SSD1306_SDA);
	DIGITAL_WRITE_HIGH(SSD1306_SCL);
	DIGITAL_WRITE_LOW(SSD1306_SCL);
}

void tinyOLED::send_command_start(void) {
	xfer_start();
	send_byte(SSD1306_SA);  // Slave address, SA0=0
	send_byte(0x00);	// write command
}

void tinyOLED::send_command_stop(void) {
	xfer_stop();
}

void tinyOLED::send_command(uint8_t command)
{
	send_command_start();
	send_byte(command);
	send_command_stop();
}

void tinyOLED::send_data_start(void)
{
	xfer_start();
	send_byte(SSD1306_SA);
	send_byte(0x40);	//write data
}

void tinyOLED::send_data_stop(void)
{
	xfer_stop();
}

// ----------------------------------------------------------------------------

void tinyOLED::init()
{
	SET_OUTPUT(SSD1306_SDA);	// Set port as output
	SET_OUTPUT(SSD1306_SCL);	// Set port as output
	
	for (uint8_t i = 0; i < sizeof (init_sequence); i++) {
		send_command(pgm_read_byte(&init_sequence[i]));
	}
}

void tinyOLED::setfont(bitmap_font &_newfont)
{
    font = &_newfont;
}

void tinyOLED::setpos(uint8_t _x, uint8_t _y)
{
    if (x==_x && y==_y)
        return;
    x = _x;
    y = _y;
	send_command_start();
	send_byte(0xb0 + y);
	send_byte(((x & 0xf0) >> 4) | 0x10); // | 0x10
    send_byte((x & 0x0f)); // | 0x01
	send_command_stop();
}

void tinyOLED::fillp(uint8_t p1, uint8_t p2)
{
	setpos(0, 0);
	send_data_start();
	for (uint16_t i = 0; i < 128 * 8 / 2; i++)
	{
		send_byte(p1);
		send_byte(p2);
	}
	send_data_stop();
}

void tinyOLED::fill(uint8_t p)
{
	fillp(p, p);
}

// ----------------------------------------------------------------------------

void tinyOLED::output_char(char ch)
{
    uint8_t w = font->charwidth();
    uint8_t h = font->charheight();
    if (ch=='\n')
        setpos(0, y+h);
    else
    {
        uint8_t _x = x;
        uint8_t _y = y;
        uint8_t r = h;
        while (r--)
        {
            setpos(_x, _y+r);
            send_data_start();
            for (uint8_t i = 0; i < w; i++)
            {
               send_byte(font->getBits(ch, i, r));
            }
            send_data_stop();
        }
        x = _x+w;
        y = _y;
        if (x+w > 128)
            setpos(0, y+h);
    }
}

void tinyOLED::output_string(const char *s)
{
	while (*s)
       output_char(*s++);
}

void tinyOLED::output_num(uint16_t num, int8_t pad)
{
    if (num>9)
        output_num(num/10, pad-1);
    else while(--pad > 0)
        output_char(' ');
    output_char(num%10+'0');
}

// ----------------------------------------------------------------------------

void tinyOLED::draw_bmp(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t *bitmap)
{
	for (uint8_t y = y0; y < y1; y++)
	{
		setpos(x0,y);
		send_data_start();
		for (uint8_t x = x0; x < x1; x++)
		{
			send_byte(pgm_read_byte(bitmap++));
		}
		send_data_stop();
	}
}

// ----------------------------------------------------------------------------

uint8_t bitmap_font_simple::getBits(char ch, uint8_t i, uint8_t r) const
{
    uint8_t c = ch - 32;
    const uint8_t *bitmap = bits[c/32];
    if (!bitmap)
        return 0;
    c &= 31;
    return pgm_read_byte(&bitmap[c*w*h + i + r*w]);
}

uint8_t bitmap_font_digitsonly::getBits(char ch, uint8_t i, uint8_t r) const
{
    if (ch < '0' || ch > '9')
        return 0;
    return bitmap_font_simple::getBits(ch-'0'+' ', i, r);
}

tinyOLED TinyOLED;

bitmap_font_simple bitmap_font_full(8, 2, ssd1306xled_font8x16_FULL);
bitmap_font_simple bitmap_font_uc(8, 2, ssd1306xled_font8x16_UCONLY);
bitmap_font_simple bitmap_font_lc(8, 2, ssd1306xled_font8x16_LCONLY);
bitmap_font_simple bitmap_font_6bit(8, 2, ssd1306xled_font8x16_6BIT);
bitmap_font_digitsonly bitmap_font_digits(8, 2, ssd1306xled_font8x16_DIGITSONLY);

bitmap_font_simple bitmap_font_small_full(6, 1, ssd1306xled_font6x8_FULL);
bitmap_font_simple bitmap_font_small_uc(6, 1, ssd1306xled_font6x8_UCONLY);
bitmap_font_simple bitmap_font_small_lc(6, 1, ssd1306xled_font6x8_LCONLY);
bitmap_font_simple bitmap_font_small_6bit(6, 1, ssd1306xled_font6x8_6BIT);
bitmap_font_digitsonly bitmap_font_small_digits(6, 1, ssd1306xled_font6x8_DIGITSONLY);
