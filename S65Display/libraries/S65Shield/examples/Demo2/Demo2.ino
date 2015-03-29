//S65-Shield: Demo 2 (Display + SD-Card)

#include <S65Display.h>
#include <SDcard.h>

S65Display lcd;
SDcard sd;

#if _USE_LFN //use LFN (long file names)
char LFName[_DF1S ? ((_MAX_LFN*2)+1) : (_MAX_LFN+1)]; //LFN buffer
#endif


ISR(TIMER2_OVF_vect) //Timer 2 Interrupt, called every 1ms
{
  static uint8_t count=1;

  TCNT2 -= 250; //1000 Hz

  if(--count == 0) //100 Hz
  {
    count = 10;
    sd.service();
  }
}


void showDir(char *path)
{
  FILINFO finfo;
  DIR dir;
  int8_t i=15;
  char *fname;

  Serial.println(path);

  lcd.clear(RGB(0,255,0));
  lcd.drawText(5, 5, path, 1, RGB(255,255,255), RGB(0,255,0));

#if _USE_LFN
  finfo.lfname = LFName;
  finfo.lfsize = sizeof(LFName);
#endif

  if(f_opendir(&dir, path) == FR_OK)
  {
    while((f_readdir(&dir, &finfo) == FR_OK) && finfo.fname[0])
    {
#if _USE_LFN
      fname = (*finfo.lfname) ? finfo.lfname : finfo.fname;
#else
      fname = finfo.fname;
#endif
      Serial.println(fname);
      lcd.drawText(5, i, fname, 1, RGB(255,255,255), RGB(0,255,0));
      i += 10;
      if(i >= (S65_HEIGHT-10))
      {
        break;
      }
    }
  }
}


void setup()
{
  //init Serial port
  Serial.begin(38400); 

  //init LCD
  Serial.println("Init Display...");
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
  Serial.println("Init SD-Card...");
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
    Serial.println("Files on the card:");
    showDir("/");
    sd.unmount();
  }
  else
  {
    Serial.println("No card found!");
    lcd.clear(RGB(255,0,0));
    lcd.drawTextPGM(5, 5, PSTR("No card found!"), 1, RGB(255,255,255), RGB(255,0,0));
  }
}


void loop()
{

}
