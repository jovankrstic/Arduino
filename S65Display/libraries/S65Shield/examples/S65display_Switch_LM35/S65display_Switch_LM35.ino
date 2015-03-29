//S65-Shield: Demo 1 (Display + Rotary Encoder)

#include <S65Display.h>
#include <RotaryEncoder.h>

S65Display lcd;
RotaryEncoder encoder;
const int analogInPin = A0;
const int tempPin = A1;
int sensorValue = 0;
int Temp = 0;
int time = 0;

ISR(TIMER2_OVF_vect)
{
  TCNT2 -= 250; //1000 Hz
  time++;
  if (time > 500)
  {
    time = 0;
    Temp = analogRead(tempPin);
    delay(1);
    Temp = analogRead(tempPin);
    Temp = Temp * 0.48828125;
  }
  encoder.service();
}
void Dispint(int ch,uint8_t x,uint8_t y,uint8_t s,uint16_t col,uint16_t bg_col)
{
   char c[2];
   c[1]=ch/10;
   c[2]=ch%10;
   lcd.drawChar(x, y,c[1]+48,  s, col,bg_col);
   if (s == 1)
   {
     lcd.drawChar(x+8, y,c[2]+48,  s, col,bg_col);
   }
   else if (s == 2)
   {
     lcd.drawChar(x+15, y,c[2]+48,  s, col,bg_col);
   }
}


void drawText(void)
{
  //lcd.drawLine(10, 10, 50, 40, RGB(  0,255,255));
  //lcd.drawLine(10, 40, 50, 10, RGB(  0,255,255));
  
  lcd.fillRect(58, 25, 120, 50, RGB(0,0,  255));
  Dispint(Temp,60, 30, 2, RGB(  0,255,  0), RGB(0,  0,  255));
  lcd.drawTextPGM(98, 30, PSTR("C"), 2, RGB(  0,255,  0), RGB(0,  0,  255));
  lcd.drawTextPGM(91, 25, PSTR("o"), 1, RGB(  0,255,  0), RGB(0,  0, 255));
  lcd.drawTextPGM(10, 50, PSTR("Smart Room"), 2, RGB(  0,255,  0), RGB(255,  0,  0));
  lcd.drawTextPGM(10, 80, PSTR("         by         "), 1, RGB(255,255,  0), RGB(255,  0,  0));
  lcd.drawTextPGM(10, 95, PSTR("  J.O.X electronic  "), 1, RGB(255,255,255), RGB(255,  0,  0));
  //lcd.drawMLText(10,110, PSTR(temps), 1, RGB(  0,  0,255), RGB(255,  0,  0));
  //Umlaut test text (remove FONT_END7F define)
  // lcd.drawTextPGM(10, 10, PSTR("Test \x84 \x94 \x81 \x8E \x99 \x9A"), 1, RGB(  0,255,  0), RGB(255,  0,  0));
  //lcd.drawText   (10, 25,      Tems,  1, RGB(  0,255,  0), RGB(255,  0,  0));
}


void setup()
{
  //init LCD
  lcd.init(4); //spi-clk = Fcpu/4
  
  //init Rotary Encoder
  encoder.init();

  //init Timer2
  TCCR2B  = (1<<CS22); //clk=F_CPU/64
  TCNT2   = 0x00;
  TIMSK2 |= (1<<TOIE2); //enable overflow interupt

  //interrupts on
  sei();

  //clear screen
  lcd.clear(RGB(255,0,0));
  
  drawText();
}


#define RECTSIZE 30
void loop()
{
  int8_t move, press;
  static int16_t mode=0, x=0, y=0, color=RGB(0,255,0);
  
  sensorValue = analogRead(analogInPin); 
  if (sensorValue > 140 & sensorValue > 150)
  {
    move = move + 1;
  }
  if (sensorValue > 320 & sensorValue > 340)
  {
    move = move - 1;
  }
  if (sensorValue > 730 & sensorValue > 750)
  {
    press = 1;
  }
  else 
  {
    press = 0;
  }
  //move  = encoder.step();
  //press = encoder.sw();
  
  if(move || press)
  {
    lcd.fillRect(x, y, x+RECTSIZE, y+RECTSIZE, RGB(255,0,0)); //clear last position
    drawText();

    if(press == SW_PRESSED)
    {
      color = ~color;
      mode  = 1-mode;
    }
    else if(press == SW_PRESSEDLONG)
    {
      x = 0;
      y = 0;
    }

    if(move)
    {
      if(mode == 0)
      {
        x += (move*4);
      }
      else
      {
        y += (move*4);
      }

      if(x < 0)
      {
        x = 0;
      }
      else if(x >= (S65_WIDTH-RECTSIZE))
      {
        x = (S65_WIDTH-RECTSIZE)-1;
      }
      
      if(y < 0)
      {
        y = 0;
      }
      else if(y >= (S65_HEIGHT-RECTSIZE))
      {
        y = (S65_HEIGHT-RECTSIZE)-1;
      }
    }

    lcd.fillRect(x, y, x+RECTSIZE, y+RECTSIZE, color);
  }
}
