# S65-Shield

French description and additional functions:
http://www.mon-club-elec.fr/pmwiki_reference_arduino/pmwiki.php?n=Main.LibrairieS65Shield


## Installation

Download the files [here](https://github.com/watterott/S65-Shield/archive/master.zip).

Copy the content of */libraries/* to your Arduino lib folder */arduino/libraries/* or to your user documents folder */My Documents/Arduino/libraries/*
If there are existing folders from a previous installation, please delete them before copying.

Further infos: http://arduino.cc/en/Guide/Libraries and http://arduino.cc/en/Hacking/Libraries


## Known Issues

If using the Ethernet-Shield together with the S65-Shield, this must be initialized before using the SD-Card.

For Hardware-SPI support on Mega boards connect the S65-Shield as follows:

         Mega   S65-Shield
    SCK   52   ->   13
    MOSI  51   ->   11
    MISO  50   ->   12

and comment the following lines:

    libraries/S65Display/S65Display.h:23 (#define S65_SOFTWARE_SPI)
    libraries/SDcard/mmc.h.h:63 (#define SD_SOFTWARE_SPI)


## Examples

Examples can be found in the Arduino IDE under ```File -> Examples -> S65Shield```.


## Libraries


### S65Display

    void init(uint8_t clock_div)
* clock_div - SPI clock divider (2, 4, 8, 16, 32),  SPI clock = Main clock / clock_div

Initialize the display.

    void setCursor(uint8_t x, uint8_t y)
* x - X postion
* y - Y postion

Set cursor.

    void setArea(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
* x0 - X postion of 1st point (left top)
* y0 - Y postion of 1st point (left top)
* x1 - X postion of 2nd point (right bottom)
* y1 - Y postion of 2nd point (right bottom)

Set draw area.

    void drawStart(void)
Start drawing.

    void draw(uint16_t color)
Draw color.

    void drawStop(void)
Stop drawing.

    void clear(uint16_t color)
* color - screen color

Clear the complete screen. 

    void drawPixel(uint8_t x0, uint8_t y0, uint16_t color)
* x0    - X postion of the pixel
* y0    - Y postion of the pixel
* color - Color

Draw a pixel.

    void drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t color)
* x0    - X postion of 1st point
* y0    - Y postion of 1st point
* x1    - X postion of 2nd point
* y1    - Y postion of 2nd point
* color - Color

Draw a line.

    void drawRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t color)
* x0    - X postion of 1st point
* y0    - Y postion of 1st point
* x1    - X postion of 2nd point
* y1    - Y postion of 2nd point
* color - Color

Draw a rectangle edge.

    void fillRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t color)
* x0    - X postion of 1st point
* y0    - Y postion of 1st point
* x1    - X postion of 2nd point
* y1    - Y postion of 2nd point
* color - Color

Draw a filled rectangle.

    void drawCircle(uint8_t x0, uint8_t y0, uint8_t radius, uint16_t color)
* x0        - X postion of middle point
* y0        - Y postion of middle point
* radius    - Radius
* color     - Color

Draw a circle.

    void fillCircle(uint8_t x0, uint8_t y0, uint8_t radius, uint16_t color)
* x0        - X postion of middle point
* y0        - Y postion of middle point
* radius    - Radius
* color     - Color

Draw a filled circle.

    void drawChar(uint8_t x, uint8_t y, char c, uint8_t size, uint16_t color, uint16_t bg_color)
* x         - X postion (left top)
* y         - Y postion (left top)
* c         - Character (RAM)
* size      - Font size scale-up (1=normal size, 2=double size...)
* color     - Font color
* bg_color  - Background color

Draw a single character from RAM.

    uint8_t drawText(uint8_t x, uint8_t y, char *s, uint8_t size, uint16_t color, uint16_t bg_color)
* x         - X postion (left top)
* y         - Y postion (left top)
* s         - Pointer to string (RAM)
* size      - Font size scale-up (1=normal size, 2=double size...)
* color     - Font color
* bg_color  - Background color

Draw a string from RAM. The return value is the X end postion of the text.

    uint8_t drawTextPGM(uint8_t x, uint8_t y, PGM_P s, uint8_t size, uint16_t color, uint16_t bg_color)
* x         - X postion (left top)
* y         - Y postion (left top)
* s         - Pointer to string (Flash)
* size      - Font size scale-up (1=normal size, 2=double size...)
* color     - Font color
* bg_color  - Background color

Draw a string from Flash. The return value is the X end postion of the text.

    uint8_t drawMLText(uint8_t x, uint8_t y, char *s, uint8_t size, uint16_t color, uint16_t bg_color)
* x         - X postion (left top)
* y         - Y postion (left top)
* s         - Pointer to string (RAM)
* size      - Font size scale-up (1=normal size, 2=double size...)
* color     - Font color
* bg_color  - Background color

Draw a multi-line string (line end = "\n") from RAM. The return value is the X end postion of the text.

    uint8_t drawMLTextPGM(uint8_t x, uint8_t y, PGM_P s, uint8_t size, uint16_t color, uint16_t bg_color)
* x         - X postion (left top)
* y         - Y postion (left top)
* s         - Pointer to string (Flash)
* size      - Font size scale-up (1=normal size, 2=double size...)
* color     - Font color
* bg_color  - Background color

Draw a multi-line string (line end = "\n") from Flash. The return value is the X end postion of the text.

#### Note

For colors you can use the ```RGB(red,green,blue)``` macro.
  
To rotate or mirror the display uncomment the definitions ```S65_ROTATE``` or ```S65_MIRROR``` in */libraries/S65Display/S65Display.h*

The text functions can draw characters from 0x20 to 0x7F and the default font is 8x12.
To change the font to 8x8 uncomment the ```FONT_8X8``` definition and comment the ```FONT_8X12``` definition in */libraries/S65Display/fonts.h*
To support characters from 0x20 to 0xFF comment the ```FONT_END7F``` definition in */libraries/S65Display/fonts.h*.
Use Umlaut characters as hex values, for example "Test \x84 \x94 \x81 \x8E \x99 \x9A" to display "Test ä ö ü Ä Ö Ü"

    0x84 = ä, 0x8E = Ä, 0x94 = ö, 0x99 = Ö, 0x81 = ü, 0x9A = Ü*


### RotaryEncoder

    void init(void)
Initialize the microcontroller peripherals.

    void service(void)
This function has to be called every 1 millisecond (1000 Hz).

    int8_t step(void)
Get the step of the rotary encoder: -1, 0, +1

    int8_t sw(void)
Get the state of the switch: 0, SW_PRESSED, SW_PRESSEDLONG


### SDcard

    void init(uint8_t clock_div)
* clock_div - SPI clock divider (2, 4, 8, 16, 32),  SPI clock = Main clock / clock_div

Initialize the microcontroller peripherals.

    void service(void)
This function has to be called every 10 milliseconds (100 Hz).

    uint8_t mount(void)
Initialize the memory card and mount it.
The return value is 0 if the mounting failed and 1 if the card is mounted successfully.

    void unmount(void)
Unmount the memory card.

See also the FatFS docu for more information about the file system functions (*f_open()*...):
*/libraries/SDcard/doc/00index_e.html* or http://elm-chan.org/fsw/ff/00index_e.html
To enable LFN support (long file names) set ```_USE_LFN``` to **1** in */libraries/SDcard/ffconf.h*. The *Demo2* has also LFN support.


## Pin Map

    Display          A3 17 - Reset
                     A2 16 - CS
                         4 - RS
                   MOSI 11 - MOSI
                    SCK 13 - SCK

    Rotary Encoder       5 - Switch
                         6 - PhA
                         7 - PhB

    microSD              8 - Power (low -> power on)
                         9 - CS
                   MISO 12 - MISO
                   MOSI 11 - MOSI
                    SCK 13 - SCK
