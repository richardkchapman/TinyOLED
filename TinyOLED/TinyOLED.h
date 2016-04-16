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

#ifndef TINYOLED_H
#define TINYOLED_H

// ----------------------------------------------------------------------------

// Port definitions - I have set up to use XTAL pins but any on portB can be used.
// To move off portB you will need to edit the cpp file too...

#if defined (__AVR_ATtiny85__) || defined(__AVR_ATtiny45__)
 #define SSD1306_SCL		PB3	    // SCL,	Pin 3 on SSD1306 Board, pin 2 on ATTTINY85
 #define SSD1306_SDA		PB4	    // SDA,	Pin 4 on SSD1306 Board, pin 3 on ATTTINY85
#elif defined(__AVR_ATtiny84__) || defined(__AVR_ATtiny44__)
 #define SSD1306_SCL        PB0     // SCL, Pin 3 on SSD1306 Board, pin 2 on ATTTINY84
 #define SSD1306_SDA        PB1     // SDA, Pin 4 on SSD1306 Board, pin 3 on ATTTINY84
#else
 #error "Please edit to indicate the PORTB pins to use"
#endif

#define SSD1306_SA		0x78	// Slave address

// ----------------------------------------------------------------------------

// Font classes provide bitmap fonts at various sizes.
// The base class provites the interface

class bitmap_font
{
protected:
    uint8_t w;   // in bits
    uint8_t h;   // in bytes
public:
    inline constexpr bitmap_font(uint8_t _w, uint8_t _h) : w(_w), h(_h) {};
    // Return the bitmap for column c, row r of this character.
    virtual uint8_t getBits(char ch, uint8_t c, uint8_t r) const = 0;
    inline uint8_t charwidth() const { return  w; }
    inline uint8_t charheight() const { return h; }
};

// bitmap_font_simple covers normal font mappings

class bitmap_font_simple : public bitmap_font
{
protected:
    const uint8_t **bits;  // [0] is chars 32-63, [1] is chars 64-95, [2] is chars 96-127
public:
    inline constexpr bitmap_font_simple(uint8_t _w, uint8_t _h,const uint8_t **_bits) : bitmap_font(_w, _h), bits(_bits) {};
    uint8_t getBits(char ch, uint8_t col, uint8_t row) const;
};

// bitmap_font_digits can be used to save program space when only digits are being output

class bitmap_font_digitsonly : public bitmap_font_simple
{
public:
    inline constexpr bitmap_font_digitsonly(uint8_t _w, uint8_t _h,const uint8_t **_bits) : bitmap_font_simple(_w, _h, _bits) {};
    virtual uint8_t getBits(char ch, uint8_t col, uint8_t row) const;
};

// Pick a font... these ones are 16x8

extern bitmap_font_simple bitmap_font_full;   // All chars 32-126 mapped
extern bitmap_font_simple bitmap_font_uc;     // Only upper case chars
extern bitmap_font_simple bitmap_font_lc;     // Only lower case chars
extern bitmap_font_simple bitmap_font_6bit;   // Only chars 32-95
extern bitmap_font_digitsonly bitmap_font_digits; // Only digits

// These ones are 8x6

extern bitmap_font_simple bitmap_font_small_full;
extern bitmap_font_simple bitmap_font_small_uc;
extern bitmap_font_simple bitmap_font_small_lc;
extern bitmap_font_simple bitmap_font_small_6bit;
extern bitmap_font_digitsonly bitmap_font_small_digits;

class tinyOLED
{
private:
    void xfer_start(void);
    void xfer_stop(void);
    void send_byte(uint8_t byte);
    void send_command_start(void);
    void send_command(uint8_t command);
    void send_command_stop(void);
    void send_data_start(void);
    void send_data_stop(void);
    static const uint8_t init_sequence [];
    uint8_t x = 0, y = 0;
    bitmap_font *font = nullptr;

public:
    constexpr tinyOLED() = default;

    // init() - call before any other functions. Allow a delay after power on before calling
    void init();

    // setpos(x,y) - x is in pixels, y is in "cells" where a cell is 8 bytes high
    void setpos(uint8_t x, uint8_t y);

    // fillp(p1, p2) - fill screen with a pattern of alternating p1,p2
    void fillp(uint8_t p1, uint8_t p2);

    // fill(p) - fill screen with all bytes set to p
    void fill(uint8_t p);

    // setfont(font) - must be called before any of the text output functions below
    // Can be called multiple times to switch font sizes
    void setfont(bitmap_font &font);

    // Make sure you call setfont before using any of these!

    void output_char(char ch);
    void output_string(const char *s);
    void output_num(uint16_t num, int8_t pad = 0);
    void draw_bmp(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t bitmap[]);
};

extern tinyOLED TinyOLED;
#endif
