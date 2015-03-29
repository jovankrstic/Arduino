//S65-Shield: OpenFileDemo (Display + SD-Card)

#include <S65Display.h>
#include <SDcard.h>

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


void OpenFile(char *file)
{
  FIL fsrc;     //file objects
  char buf[32]; //file buffer
  FRESULT res;  //result code
  UINT rd;      //file R/W count
  
  lcd.clear(RGB(255,255,255));

  res = f_open(&fsrc, file, FA_OPEN_EXISTING | FA_READ);
  if(res == FR_OK)
  {
    res = f_read(&fsrc, buf, sizeof(buf), &rd);
    if(res == FR_OK)
    {
      if(rd >=sizeof(buf))
      {
        rd = sizeof(buf)-1;
      }
      buf[rd] = 0;
      lcd.drawText(5, 5, buf, 1, RGB(0,0,0), RGB(255,255,255));
      return;
    }
    f_close(&fsrc);
  }
  
  lcd.drawTextPGM(5, 5, PSTR("File not found!"), 1, RGB(0,0,0), RGB(255,255,255));
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
    lcd.drawTextPGM(5, 5, PSTR("Read file..."), 1, RGB(255,255,255), RGB(0,255,0));
    OpenFile("test.txt");
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
