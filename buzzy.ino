/**
   The MIT License (MIT)

   Copyright (c) 2018 by ThingPulse, Daniel Eichhorn
   Copyright (c) 2018 by Fabrice Weinberg

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.

   ThingPulse invests considerable time and money to develop these open source libraries.
   Please support us by buying our products (and not the clones) from
   https://thingpulse.com

*/

// Include the correct display library
// For a connection via I2C using Wire include
#include <Wire.h>        // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`
// or #include "SH1106Wire.h", legacy include: `#include "SH1106.h"`
// For a connection via I2C using brzo_i2c (must be installed) include
// #include <brzo_i2c.h> // Only needed for Arduino 1.6.5 and earlier
// #include "SSD1306Brzo.h"
// #include "SH1106Brzo.h"
// For a connection via SPI include
// #include <SPI.h> // Only needed for Arduino 1.6.5 and earlier
// #include "SSD1306Spi.h"
// #include "SH1106SPi.h"

// Use the corresponding display class:

// Initialize the OLED display using SPI
// D5 -> CLK
// D7 -> MOSI (DOUT)
// D0 -> RES
// D2 -> DC
// D8 -> CS
// SSD1306Spi        display(D0, D2, D8);
// or
// SH1106Spi         display(D0, D2);

// Initialize the OLED display using brzo_i2c
// D3 -> SDA
// D5 -> SCL
// SSD1306Brzo display(0x3c, D3, D5);
// or
// SH1106Brzo  display(0x3c, D3, D5);

#define BUZZER D3

#define SELECT D4

#define BOTAO_VERDE D5
#define BOTAO_VERMELHO D6
#define BOTAO_DIREITA D7
#define BOTAO_ESQUERDA D8

#define byte uint8_t

#include "font.h"
#include "notes.h"

// Initialize the OLED display using Wire library
SSD1306Wire display(0x3c, D1, D2); // ADDRESS, SDA, SCL  -  SDA and SCL usually populate automatically based on your board's pins_arduino.h e.g. https://github.com/esp8266/Arduino/blob/master/variants/nodemcu/pins_arduino.h
// SH1106Wire display(0x3c, SDA, SCL);

bool redraw = true;
byte cursor_x = 0;
byte cursor_y = 0;
Note tracklist[96];
byte loopsize = 16;
short defaultdelay = 100;

void playNote(Note note)
{
    if (note.note == 0)
    {
        delay(note.duration);
    }
    else
    {
        tone(BUZZER, nfreq[note.note]*note.octave, note.duration);
        delay(note.duration);
    }
}

void playTrack()
{
    for (byte i = 0; i < loopsize; i++)
    {
        playNote(tracklist[i]);
    }
}

void setup()
{
    display.init();

    // display.flipScreenVertically();

    display.setContrast(255);

    display.flipScreenVertically();

    pinMode(BOTAO_VERDE, INPUT);
    pinMode(BOTAO_VERMELHO, INPUT);
    pinMode(BOTAO_DIREITA, INPUT);
    pinMode(BOTAO_ESQUERDA, INPUT);
    pinMode(SELECT, INPUT);
    pinMode(BUZZER, OUTPUT);

    display.setFont(Font6x8);
    for (byte i = 0; i < 96; i++)
    {
        tracklist[i].note = 0;
        tracklist[i].octave = 6;
        tracklist[i].duration = 100;
    }
}

void loop()
{

    if (redraw)
    {
        display.clear();
        char buffer[25];
        for (byte y = 0; y < 6; y++)
        {
            for (byte x = 0; x < 16; x++)
            {
                display.drawStringf((x * 8)+1, y * 8, buffer, "%c", notes[tracklist[y * 16 + x].note]);
            }
        }
        display.setColor(WHITE); // alternate colors
        display.fillRect(cursor_x-1, cursor_y+1, 8, 8);
        display.setColor(BLACK); // alternate colors
        display.drawStringf(cursor_x, cursor_y, buffer, "%c", notes[tracklist[cursor_y / 8 * 16 + cursor_x / 8].note]);
        display.setColor(WHITE); // alternate colors
        //display.drawString(cursor_x, cursor_y, "8");
        display.drawStringf(0, 53, buffer, "%c%i %ims %inotes %ims", notes[tracklist[cursor_y / 8 * 16 + cursor_x / 8].note], tracklist[cursor_y / 8 * 16 + cursor_x / 8].octave, tracklist[cursor_y / 8 * 16 + cursor_x / 8].duration, loopsize, defaultdelay);
        display.drawLine(0, 51, display.getWidth() - 1, 51);
        display.display();
        redraw = false;
    }
    
    if (digitalRead(SELECT) == HIGH)
    {
        if (digitalRead(BOTAO_ESQUERDA) == HIGH)
        {
            if (digitalRead(BOTAO_DIREITA) == HIGH)
            {
                if (digitalRead(BOTAO_VERDE) == HIGH)
                {
                    defaultdelay += 10;
                    redraw = true;
                }
                else if (digitalRead(BOTAO_VERMELHO) == HIGH)
                {
                    if (defaultdelay >= 10)
                    {
                        defaultdelay -= 10;
                        redraw = true;
                    }
                }
            }
        }
        else if (digitalRead(BOTAO_DIREITA) == HIGH)
        {
            if (digitalRead(BOTAO_VERDE) == HIGH)
            {
                loopsize ++;
                redraw = true;
            }
            else if (digitalRead(BOTAO_VERMELHO) == HIGH)
            {
                if (loopsize > 1)
                {
                    loopsize--;
                    redraw = true;
                }
            }
        }
        else if (digitalRead(BOTAO_VERMELHO) == HIGH)
        {
            cursor_x = 0;
            cursor_y = 0;
            redraw = true;
        }
        else if (digitalRead(BOTAO_VERDE) == HIGH)
        {
            playTrack();
        }
    }
    else if (digitalRead(BOTAO_DIREITA) == HIGH)
    {
        if (digitalRead(BOTAO_VERDE) == HIGH)
        {
            //increase duration
            tracklist[cursor_y / 8 * 16 + cursor_x / 8].duration += 10;
            redraw = true;
        }
        else if (digitalRead(BOTAO_VERMELHO) == HIGH)
        {
            //decrease duration
            if (tracklist[cursor_y / 8 * 16 + cursor_x / 8].duration >= 10)
                tracklist[cursor_y / 8 * 16 + cursor_x / 8].duration -= 10;
            redraw = true;
        }
    }
    else if (digitalRead(BOTAO_ESQUERDA) == HIGH)
    {
        if (digitalRead(BOTAO_VERDE) == HIGH)
        {
            if (cursor_x < 120)
            {
                cursor_x += 8;
            }
            else
            {
                if (cursor_y < 40)
                {
                    cursor_y += 8;
                }
                else if (cursor_y == 40)
                {
                    cursor_y = 0;
                }
                cursor_x = 0;
            }
            redraw = true;
        }
        else if (digitalRead(BOTAO_VERMELHO) == HIGH)
        {
            if (cursor_x > 0)
            {
                cursor_x -= 8;
            }
            else
            {
                if (cursor_y > 0)
                {
                    cursor_y -= 8;
                }
                else if (cursor_y == 0)
                {
                    cursor_y = 40;
                }
                cursor_x = 120;
            }
            redraw = true;
        }
    }
    else if (digitalRead(BOTAO_VERDE) == HIGH)
    {
        if (tracklist[cursor_y / 8 * 16 + cursor_x / 8].note < 12)
            tracklist[cursor_y / 8 * 16 + cursor_x / 8].note++;
        else
        {
            tracklist[cursor_y / 8 * 16 + cursor_x / 8].note = 0;
            tracklist[cursor_y / 8 * 16 + cursor_x / 8].octave++;
        }
        redraw = true;
    }
    else if (digitalRead(BOTAO_VERMELHO) == HIGH)
    {
        if (tracklist[cursor_y / 8 * 16 + cursor_x / 8].note > 0)
            tracklist[cursor_y / 8 * 16 + cursor_x / 8].note--;
        else
        {
            tracklist[cursor_y / 8 * 16 + cursor_x / 8].note = 12;
            if (tracklist[cursor_y / 8 * 16 + cursor_x / 8].octave > 1)
                tracklist[cursor_y / 8 * 16 + cursor_x / 8].octave--;
        }
        redraw = true;
    }
    
    if(defaultdelay > 0) 
        delay(defaultdelay);
    display.clear();
}
