#include <inttypes.h>
#if defined(__AVR__)
# include <avr/io.h>
#endif
#if ARDUINO >= 100
# include "Arduino.h"
#else
# include "WProgram.h"
#endif
#include "../digitalWriteFast/digitalWriteFast.h"
#include "RotaryEncoder.h"


#define RE_SW_PIN            (5)
#define RE_PHA_PIN           (6)
#define RE_PHB_PIN           (7)

#define RE_SW_READ()         digitalReadFast(RE_SW_PIN)
#define RE_PHA_READ()        digitalReadFast(RE_PHA_PIN)
#define RE_PHB_READ()        digitalReadFast(RE_PHB_PIN)


//-------------------- Constructor --------------------


RotaryEncoder::RotaryEncoder(void)
{
  return;
}


//-------------------- Public --------------------


void RotaryEncoder::init(void)
{
  //init pins
  pinMode(RE_SW_PIN, INPUT);
  digitalWrite(RE_SW_PIN, HIGH); //pull-up
  pinMode(RE_PHA_PIN, INPUT);
  digitalWrite(RE_PHA_PIN, HIGH); //pull-up
  pinMode(RE_PHB_PIN, INPUT);
  digitalWrite(RE_PHB_PIN, HIGH); //pull-up

  //init global vars
  re_sw    = 0;
  re_delta = 0;
  re_last  = 0;
  if(RE_PHA_READ())
  {
    re_last = 3;
  }
  if(RE_PHB_READ())
  {
    re_last ^= 1;
  }

  return;
}


//Rotary Encoder routine by Peter Dannegger
//http://www.mikrocontroller.net/articles/Drehgeber
void RotaryEncoder::service(void) //called every 1ms
{
  int8_t re_new, re_diff;
  static int16_t re_pressed=0;

  re_new = 0;
  if(RE_PHA_READ())
  {
    re_new = 3;
  }
  if(RE_PHB_READ())
  {
    re_new ^= 1;                //convert gray to binary
  }
  re_diff = re_last-re_new;     //difference last - new
  if(re_diff & 1)               //bit 0 = value (1)
  {
    re_last   = re_new;         //store new as next last
    re_delta += (re_diff&2)-1;  //bit 1 = direction (+/-)
  }

  if(RE_SW_READ())
  {
    if(re_pressed > 800) //800 ms
    {
      if(re_sw == 0)
      {
        re_sw = SW_PRESSEDLONG;
      }
    }
    else if(re_pressed > 80) //80 ms
    {
      if(re_sw == 0)
      {
        re_sw = SW_PRESSED;
      }
    }
    re_pressed = 0;
  }
  else
  {
    re_pressed++;
  }

  return;
}


int8_t RotaryEncoder::step(void)
{
  int8_t val;

  cli();
  val      = re_delta;
  re_delta = val&3;     //1step:0 //2step:val&1 //4step:val&3;
  sei();
  
  val = val>>2;      //1step:val //2step:val>>1 //4step:val>>2

  if(val < 0)
  {
    return +1;
  }
  else if(val > 0)
  {
    return -1;
  }

  return 0;
}


int8_t RotaryEncoder::sw(void)
{
  int8_t val;

  val   = re_sw;
  re_sw = 0;

  return val;
}


//-------------------- Private --------------------

