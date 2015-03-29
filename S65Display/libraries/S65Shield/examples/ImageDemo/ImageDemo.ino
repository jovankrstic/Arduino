//S65-Shield: ImageDemo (Display + SD-Card)
//Thanks to Mirco Schmedicke

#include <S65Display.h>
#include <SDcard.h>
#include <BMPheader.h>

S65Display lcd;
SDcard sd;


ISR(TIMER2_OVF_vect)
{
  static uint8_t count=1;

  TCNT2 -= 250; //1000 Hz

  if(--count == 0) //100 Hz
  {
    count = 10;
    sd.service();
  }
}


#define CHANNELS 3
void OpenImageFile(char *file, uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
  FIL fsrc;     //file objects
  FRESULT res;  //result code
  UINT rd;      //file R/W count
  uint8_t nWidth, nHeight;
  uint8_t data[CHANNELS];

  res = f_open(&fsrc, file, FA_OPEN_EXISTING | FA_READ);
  if(res != FR_OK)
  {
    lcd.drawTextPGM(x, y, PSTR("File not found!"), 1, RGB(0,0,0), RGB(255,255,255));
    return;
  }

  lcd.setArea(0, 0, (S65_WIDTH-1), (S65_HEIGHT-1));
  for(nHeight=0; nHeight < height; nHeight++) //for every line
  {
    for(nWidth=0; nWidth < width; nWidth++) //for every pixel in line
    {
      res = f_read(&fsrc, &data, CHANNELS, &rd);
      if(res != FR_OK)
      {
        nHeight = height;
        break;
      }
      lcd.drawPixel(x+nWidth, y+nHeight, RGB(data[0],data[1],data[2]));
    }
  }

  f_close(&fsrc);
}


void OpenBMPFile(char *file, uint8_t x, uint8_t y)
{
  FIL fsrc;        //file objects
  FRESULT res;     //result code
  UINT rd;         //file R/W count
  uint8_t buf[40]; //read buf (min. size = sizeof(BMP_DIPHeader))
  BMP_Header *bmp_hd;
  BMP_DIPHeader *bmp_dip;
  int width, height, pad, w, h;

  res = f_open(&fsrc, file, FA_OPEN_EXISTING | FA_READ);
  if(res != FR_OK)
  {
    lcd.drawTextPGM(x, y, PSTR("File not found!"), 1, RGB(0,0,0), RGB(255,255,255));
    return;
  }

  //BMP Header
  bmp_hd = (BMP_Header*)&buf[0];
  res = f_read(&fsrc, &buf, sizeof(BMP_Header), &rd);
  if((res == FR_OK) &&
     (bmp_hd->magic[0] == 'B') && (bmp_hd->magic[1] == 'M') && (bmp_hd->offset == 54))
  {
    //BMP DIP-Header
    bmp_dip = (BMP_DIPHeader*)&buf[0];
    res = f_read(&fsrc, &buf, sizeof(BMP_DIPHeader), &rd);
    if((res == FR_OK) && 
       (bmp_dip->size == sizeof(BMP_DIPHeader)) && (bmp_dip->bitspp == 24) && (bmp_dip->compress == 0))
    {
      //BMP data (1. pixel = bottom left)
      width  = bmp_dip->width;
      height = bmp_dip->height;
      pad    = width % 4; //padding (line is multiply of 4)
      lcd.setArea(x, y, x+width, y+height);
      for(h=(y+height-1); h >= y; h--) //for every line
      {
        for(w=x; w < (x+width); w++) //for every pixel in line
        {
          f_read(&fsrc, &buf, 3, &rd);
          lcd.drawPixel(w, h, RGB(buf[2],buf[1],buf[0]));
        }
        if(pad)
        {
          f_read(&fsrc, &buf, pad, &rd);
        }
      }
    }
  }

  f_close(&fsrc);
}


void setup()
{
  //init LCD
  lcd.init(4); //spi-clk = Fcpu/4

  //if using Ethernet-Shield this must be init before the SD-Card
  //(not needed for the Arduino Mega when using Software-SPI)
  //here is a quick and dirty init:
/*
  DDRB  |=  (1<<2); //SS -> Output
  PORTB &= ~(1<<2); //SS -> Low -> Select Ethernet-Shield
  SPDR   =  (0xF0);                   loop_until_bit_is_set(SPSR, SPIF);
  SPDR   =  ((0x8000 & 0xFF00) >> 8); loop_until_bit_is_set(SPSR, SPIF);
  SPDR   =  ( 0x8000 & 0x00FF);       loop_until_bit_is_set(SPSR, SPIF);
  SPDR   =  (0x80);                   loop_until_bit_is_set(SPSR, SPIF);
  PORTB |=  (1<<2); //SS -> High -> Deselect Ethernet-Shield
*/

  //init SD-Card
  sd.init(4); //spi-clk = Fcpu/4

  //init Timer2
  TCCR2B  = (1<<CS22); //clk=F_CPU/64
  TCNT2   = 0x00;
  TIMSK2 |= (1<<TOIE2); //enable overflow interupt
  
  //interrupts on
  sei();

  //clear screen
  lcd.clear(RGB(255,255,255));
  lcd.drawTextPGM(5, 5, PSTR("Mount card..."), 1, RGB(0,0,0), RGB(255,255,255));

  if(sd.mount() == 0)
  {
    lcd.clear(RGB(0,255,0));
    lcd.drawTextPGM(5, 5, PSTR("Load file..."), 1, RGB(255,255,255), RGB(0,255,0));

    //image file: libraries/S65Shield/ImageDemo/image.raw
    OpenImageFile("image.raw", 0, 0, 176, 132);

    //windows bmp file (24bit RGB):  libraries/S65Shield/BMPDemo/image.bmp
    OpenBMPFile("image.bmp", 30, 20);
    
    sd.unmount();
  }
  else
  {
    lcd.clear(RGB(255,0,0));
    lcd.drawTextPGM(5, 5, PSTR("No card!"), 1, RGB(255,255,255), RGB(255,0,0));
  }
}


void loop()
{

}
