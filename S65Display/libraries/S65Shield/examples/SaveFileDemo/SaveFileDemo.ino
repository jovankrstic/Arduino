//S65-Shield: SaveFileDemo (Display + SD-Card)

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


void write_something(void)
{
  FIL fdst;     //file objects
  FRESULT res;  //result code
  UINT wr;      //file R/W count
  char data[16];

  res = f_open(&fdst, "xxx.txt", FA_CREATE_ALWAYS | FA_WRITE);
  if(res == FR_OK)
  {
    strcpy(data, "test 123");
    res = f_write(&fdst, data, strlen(data), &wr);
    f_close(&fdst);
  }
}


void setup()
{
  //init LCD
  lcd.init(4); //spi-clk = Fcpu/4

  //init SD-Card
  sd.init(4); //spi-clk = Fcpu/4

  //init Timer2
  TCCR2B  = (1<<CS22); //clk=F_CPU/64
  TCNT2   = 0x00;
  TIMSK2 |= (1<<TOIE2); //enable overflow interupt
  
  //interrupts on
  sei();

  if(sd.mount() == 0)
  {
    write_something();
    sd.unmount();
  }
}


void loop()
{

}
