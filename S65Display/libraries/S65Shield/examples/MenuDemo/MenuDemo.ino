//S65-Shield: Menu-Demo (Display + Rotary Encoder + SD-Card)

#include <S65Display.h>
#include <RotaryEncoder.h>
#include <SDcard.h>

#define BGCOLOR      RGB(  0,  0,  0) //background
#define FGCOLOR      RGB(255,255,  0) //foreground
#define SELCOLOR     RGB(255,255,255) //selection
#define EDGECOLOR    RGB(120,120,100) //edges
#define SBCOLOR      RGB(255,255,255) //standby

#define LINES        (10) //lines
#define LINEHEIGHT   (13) //pixel
#define ITEM_LEFTTOP (2)

S65Display lcd;
RotaryEncoder encoder;
SDcard sd;

#if _USE_LFN //use LFN (long file names)
char LFName[_DF1S ? ((_MAX_LFN*2)+1) : (_MAX_LFN+1)]; //LFN buffer
#endif

uint8_t menu_items=0, menu_first=0, menu_last=0, menu_sel=0, menu_lastsel=0;
char card_path[64] = {0,};


ISR(TIMER2_OVF_vect)
{
  static uint8_t count=1;

  TCNT2 -= 250; //1000 Hz
  
  encoder.service();

  if(--count == 0) //100 Hz
  {
    count = 10;
    sd.service();
  }
}


uint8_t isItemDir(char *path, uint8_t item)
{
  FILINFO finfo;
  DIR dir;
  uint8_t i=1;

#if _USE_LFN
  finfo.lfname = LFName;
  finfo.lfsize = sizeof(LFName);
#endif

  if(item)
  {
    if(f_opendir(&dir, path) == FR_OK)
    {
      while((f_readdir(&dir, &finfo) == FR_OK) && finfo.fname[0])
      {
        if(item == i)
        {
          if(finfo.fattrib & AM_DIR)
          {
            return 1;
          }
          break;
        }
        i++;
      }
    }
  }

  return 0;
}


void getItem(char *path, uint8_t item, char *name)
{
  FILINFO finfo;
  DIR dir;
  uint8_t i=1;
  char *fname;

#if _USE_LFN
  finfo.lfname = LFName;
  finfo.lfsize = sizeof(LFName);
#endif

  if(item == 0)
  {
    strcpy(name, "<< back");
  }
  else
  {
    *name = 0;
    if(f_opendir(&dir, path) == FR_OK)
    {
      while((f_readdir(&dir, &finfo) == FR_OK) && finfo.fname[0])
      {
        if(item == i)
        {
#if _USE_LFN
          fname = (*finfo.lfname) ? finfo.lfname : finfo.fname;
#else
          fname = finfo.fname;
#endif
          if(finfo.fattrib & AM_DIR)
          {
            *name++ = '/';
            strcpy(name, fname);
          }
          else
          {
            strcpy(name, fname);
          }
          break;
        }
        i++;
      }
    }
  }

  return;
}


uint8_t getItemNum(char *path)
{
  FILINFO finfo;
  DIR dir;
  uint8_t i=1;

#if _USE_LFN
  finfo.lfname = LFName;
  finfo.lfsize = sizeof(LFName);
#endif

  if(f_opendir(&dir, path) == FR_OK)
  {
    while((f_readdir(&dir, &finfo) == FR_OK) && finfo.fname[0])
    {
      i++;
    }
  }

  return i;
}


void drawMenu(uint8_t redraw)
{
  uint8_t i, x;
  static uint8_t last_first=0;
  char tmp[16];

  if(redraw == 0)
  {
    if(menu_sel == menu_lastsel)
    {
      return;
    }
  }

  //clear last selection
  i = (menu_lastsel-last_first)*LINEHEIGHT;
  lcd.drawRect(0, i, S65_WIDTH-5, i+LINEHEIGHT, BGCOLOR);

  //draw items
  if((menu_first != last_first) || redraw)
  {
    last_first = menu_first;
    for(i=0; (i < LINES) && ((menu_first+i) < menu_items); i++)
    {
      getItem(card_path, menu_first+i, tmp);
      x = lcd.drawText(ITEM_LEFTTOP, ITEM_LEFTTOP+(i*LINEHEIGHT), tmp, 1, FGCOLOR, BGCOLOR);
      if(x < (S65_WIDTH-5))
      {
        lcd.fillRect(x, i*LINEHEIGHT+1, S65_WIDTH-5-1, i*LINEHEIGHT+LINEHEIGHT-1, BGCOLOR);
      }
    }
  }

  //selection
  i = (menu_sel-menu_first)*LINEHEIGHT;
  lcd.drawRect(0, i, S65_WIDTH-5, i+LINEHEIGHT, SELCOLOR);

  //scrollbar
  i = (menu_sel*(S65_HEIGHT-1-8))/(menu_items-1);
  lcd.fillRect(S65_WIDTH-4, 0, S65_WIDTH-1, S65_HEIGHT-1, EDGECOLOR);
  lcd.fillRect(S65_WIDTH-4, i, S65_WIDTH-1, i+8, FGCOLOR);

  menu_lastsel = menu_sel;

  return;
}


void setup()
{
  //init LCD
  lcd.init(2); //spi-clk = Fcpu/2

  //init Rotary Encoder
  encoder.init();

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
  sd.init(2); //spi-clk = Fcpu/2

  //init Timer2
  TCCR2B  = (1<<CS22); //clk=F_CPU/64
  TCNT2   = 0x00;
  TIMSK2 |= (1<<TOIE2); //enable overflow interupt

  //interrupts on
  sei();

  //mount Card
  lcd.clear(BGCOLOR);
  lcd.drawTextPGM(5, 5, PSTR("Mount card..."), 1, RGB(255,255,255), RGB(255,0,0));
  do
  {
    delay(500);
  }while(sd.mount() != 0);

  //init Menu
  card_path[0] = 0;
  menu_items   = getItemNum(card_path);
  menu_first   = 0;
  menu_last    = (menu_items >= LINES)?(LINES-1):(menu_items-1);
  menu_sel     = 0;
  menu_lastsel = 0;
  lcd.clear(BGCOLOR);
  drawMenu(1);
}


void loop()
{
  uint8_t i, redraw=0;
  int8_t move;

  if(encoder.sw())
  {
    if(menu_sel == 0) //back
    {
      if(card_path[0] != 0) //root dir -> shrink path
      {
        for(i=strlen(card_path); i!=0; i--)
        {
          if(card_path[i] == '/')
          {
            break;
          }
        }
        card_path[i] = 0;
      }
    }
    else if(isItemDir(card_path, menu_sel)) //open dir
    {
      char tmp[16];
      getItem(card_path, menu_sel, tmp);
      strcat(card_path, tmp);
    }
    else //open item
    {
      lcd.drawTextPGM(70, 50, PSTR(">OPEN<"), 1, RGB(255,255,255), RGB(255,0,0));
      while(!encoder.sw()); //wait for press
    }
    menu_items   = getItemNum(card_path);
    menu_first   = 0;
    menu_last    = (menu_items >= LINES)?(LINES-1):(menu_items-1);
    menu_sel     = 0;
    redraw       = 1;
  }

  move = encoder.step();
  if(move > 0)
  {
    do
    {
      if(menu_sel < menu_last)
      {
        menu_sel++;
      }
      else if(menu_sel < (menu_items-1))
      {
        menu_sel++;
        menu_first++;
        menu_last++;
      }
    }while(--move);
  }
  else if(move < 0)
  {
    do
    {
      if(menu_sel > menu_first)
      {
        menu_sel--;
      }
      else if(menu_sel > 0)
      {
        menu_sel--;
        menu_first--;
        menu_last--;
      }
    }while(++move);
  }

  if(redraw)
  {
    lcd.clear(BGCOLOR);
  }
  drawMenu(redraw);
}
