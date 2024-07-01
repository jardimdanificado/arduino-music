//
// Small Simple OLED library demo
//
// Demonstrates how to initialize and use a few functions of the ss_oled library
// If your MCU has enough RAM, enable the backbuffer to see a demonstration
// of the speed difference between drawing directly on the display versus
// deferred rendering, followed by a "dump" of the memory to the display
//
#include <ss_oled.h>

// if your system doesn't have enough RAM for a back buffer, comment out
// this line (e.g. ATtiny85)
#define USE_BACKBUFFER

#ifdef USE_BACKBUFFER
static uint8_t ucBackBuffer[1024];
#else
static uint8_t *ucBackBuffer = NULL;
#endif

// Use -1 for the Wire library default pins
// or specify the pin numbers to use with the Wire library or bit banging on any GPIO pins
// These are the pin numbers for the M5Stack Atom default I2C
#define SDA_PIN D1
#define SCL_PIN D2
// Set this to -1 to disable or the GPIO pin number connected to the reset
// line of your display if it requires an external reset
#define RESET_PIN -1
// let ss_oled figure out the display address
#define OLED_ADDR -1
// don't rotate the display
#define FLIP180 0
// don't invert the display
#define INVERT 0
// Bit-Bang the I2C bus
#define USE_HW_I2C 0

// Change these if you're using a different OLED display
#define MY_OLED OLED_128x64
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
// #define MY_OLED OLED_64x32
// #define OLED_WIDTH 64
// #define OLED_HEIGHT 32

#define BUZZER D3

#define SELECT D4

#define BOTAO_VERDE D5
#define BOTAO_VERMELHO D6
#define BOTAO_DIREITA D7
#define BOTAO_ESQUERDA D8

#define byte uint8_t

SSOLED ssoled;

void setup()
{
    int rc;
    // The I2C SDA/SCL pins set to -1 means to use the default Wire library
    // If pins were specified, they would be bit-banged in software
    // This isn't inferior to hw I2C and in fact allows you to go faster on certain CPUs
    // The reset pin is optional and I've only seen it needed on larger OLEDs (2.4")
    //    that can be configured as either SPI or I2C
    //
    // oledInit(SSOLED *, type, oled_addr, rotate180, invert, bWire, SDA_PIN, SCL_PIN, RESET_PIN, speed)

    rc = oledInit(&ssoled, MY_OLED, OLED_ADDR, FLIP180, INVERT, USE_HW_I2C, SDA_PIN, SCL_PIN, RESET_PIN, 400000L); // use standard I2C bus at 400Khz
    if (rc != OLED_NOT_FOUND)
    {
        char *msgs[] = {(char *)"SSD1306 @ 0x3C", (char *)"SSD1306 @ 0x3D", (char *)"SH1106 @ 0x3C", (char *)"SH1106 @ 0x3D"};
        oledFill(&ssoled, 0, 1);
        oledWriteString(&ssoled, 0, 0, 0, msgs[rc], FONT_NORMAL, 0, 1);
        oledSetBackBuffer(&ssoled, ucBackBuffer);
        delay(2000);
    }

    pinMode(BOTAO_VERDE, INPUT);
    pinMode(BOTAO_VERMELHO, INPUT);
    pinMode(BOTAO_DIREITA, INPUT);
    pinMode(BOTAO_ESQUERDA, INPUT);
    pinMode(SELECT, INPUT);
    pinMode(BUZZER, OUTPUT);
} /* setup() */
bool redraw = true;
byte *player = (byte *)"A";
byte cursor_x = 0;
byte cursor_y = 0;
int i = 0;
void loop()
{

    if (redraw)
    {
        /*oledFill(&ssoled, 0, 1);
        oledWriteString(&ssoled, 0, 0, 0, (char *)"Hello, World!", FONT_NORMAL, 0, 1);
        oledWriteString(&ssoled, 0, 0, 8, (char *)"Hello, World!", FONT_SMALL, 0, 1);
        oledWriteString(&ssoled, 0, 0, 16, (char *)"Hello, World!", FONT_LARGE, 0, 1);
        oledWriteString(&ssoled, 0, 0, 48, (char *)"Hello, World!", FONT_STRETCHED, 0, 1);
        oledDrawLine(&ssoled, 0, 0, OLED_WIDTH - 1, OLED_HEIGHT - 1, 1);
        oledDrawLine(&ssoled, 0, OLED_HEIGHT - 1, OLED_WIDTH - 1, 0, 1);*/
        oledFill(&ssoled, 0, 1);
        oledWriteString(&ssoled, 0, cursor_x, cursor_y, (char *)player, FONT_SMALL, 0, 1);
        //oledWriteString(&ssoled, 0, 0, 1, (char *)"96 lines", FONT_NORMAL, 0, 1);
        redraw = false;
    }

    if (digitalRead(BOTAO_DIREITA) == HIGH)
    {
        if (digitalRead(BOTAO_VERDE) == HIGH)
        {
            cursor_x+=8;
            redraw = true;
        }
        if (digitalRead(BOTAO_VERMELHO) == HIGH)
        {
            cursor_x-=8;
            redraw = true;
        }
    }
    else if (digitalRead(BOTAO_ESQUERDA) == HIGH)
    {
        if (digitalRead(BOTAO_VERDE) == HIGH)
        {
            cursor_y+=1;
            redraw = true;
        }
        if (digitalRead(BOTAO_VERMELHO) == HIGH)
        {
            cursor_y-=1;
            redraw = true;
        }
    }
    else if (digitalRead(BOTAO_VERDE) == HIGH)
    {
        player[0]++;
        redraw = true;
    }
    else if (digitalRead(BOTAO_VERMELHO) == HIGH)
    {
        player[0]--;
        redraw = true;
    };

    

    if (digitalRead(SELECT) == HIGH)
    {
        //seed
        randomSeed(i);
        int randomnumber = random(35, 85);
        tone(BUZZER, 294, randomnumber);
    }
    i++;
    delay(10);
} /* loop() */
