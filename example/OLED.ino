/*
 * SSD1306xLED - Drivers for SSD1306 controlled dot matrix OLED/PLED 128x64 displays
 *
 * @created: 2014-08-08
 * @author: Neven Boyanov
 *
 * Copyright (c) 2015 Neven Boyanov, Tinusaur Team. All Rights Reserved.
 * Distributed as open source software under MIT License, see LICENSE.txt file.
 * Please, as a favour, retain the link http://tinusaur.org to The Tinusaur Project.
 *
 * Source code available at: https://bitbucket.org/tinusaur/ssd1306xled
 *
 */

// ----------------------------------------------------------------------------

#include "TinyOLED.h"

// ============================================================================

#include "img0_128x64c1.h"
#include "img1_128x64c1.h"

// ============================================================================

//ssd1306_font_6x8 f6;

int main(void)
{
	// ---- Initialization ----
	
	// Small delay is necessary if TinyOLED.initis the first operation in the application.
	_delay_ms(1000);
  TinyOLED.init();
  TinyOLED.setfont(bitmap_font_full);

	// ---- Main Loop ----

	while (1) {

  // ---- Fill out screen with patterns ----
		uint8_t p = 0xff;
		for (uint8_t i = 0; i < 5; i++)
		{
			p = (p >> i);
			TinyOLED.fill(~p);
			_delay_ms(100);
		}
		_delay_ms(1000);

		TinyOLED.fillp(0xAA, 0x55);
		
    // ---- Print numbers on the screen ----
    TinyOLED.setfont(bitmap_font_small_digits);
		TinyOLED.fillp(0xAA, 0x55);	// Clear screen
		uint16_t n1 = 0;
		for (uint8_t j = 0; j < 8; j++) {
			TinyOLED.setpos(0, j);
			for (uint8_t i = 0; i < 7; i++) {
				TinyOLED.output_num(n1++, 3);
			}
		}
		_delay_ms(4000);
    
		TinyOLED.fillp(0xAA, 0x55);	// Clear screen
		uint16_t n2 = 199;
		for (uint8_t j = 1; j < 7; j++) {
			TinyOLED.setpos(10, j);
			for (uint8_t i = 0; i < 3; i++) {
				TinyOLED.output_num(n2, 5);
        TinyOLED.output_char(' ');
				n2 += 567;
			}
		}
		_delay_ms(4000);
 
    TinyOLED.setfont(bitmap_font_full);
		TinyOLED.fillp(0x0, 0x0);	// Clear screen
		uint16_t n3 = 0;
		for (uint8_t i = 0; i < 163; i++) {
			TinyOLED.setpos(44, 1);
			TinyOLED.output_string("a=");
			TinyOLED.output_num(n3,5);
			TinyOLED.setpos(44, 3);
			TinyOLED.output_string("b=");
			TinyOLED.output_num(0xffff - n3, 5);
			n3 += (n3 * 3) / 33 + 1;
		}
		_delay_ms(2000);

		// ---- Print text on the screen ----
		TinyOLED.fill(0x00);	// Clear screen
    TinyOLED.setfont(bitmap_font_small_full);
		TinyOLED.output_string("That's ");
    TinyOLED.setfont(bitmap_font_full);
		TinyOLED.output_string("Tinusaur\n");
    TinyOLED.setfont(bitmap_font_small_full);
		TinyOLED.output_string("The platform that gives you all you need for your first microcontroller project\n\n");
		TinyOLED.output_string("http://tinusaur.org");

    _delay_ms(4000);
		// ---- Draw bitmap on the screen ----
		TinyOLED.draw_bmp(0,0,128,8, img1_128x64c1);
		_delay_ms(4000);

		TinyOLED.draw_bmp(0,0,128,8, img0_128x64c1);
		_delay_ms(6000);
    
	}
	
	return (0);
}
