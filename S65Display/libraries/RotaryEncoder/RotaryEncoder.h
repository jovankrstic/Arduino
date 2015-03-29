#ifndef RotaryEncoder_h
#define RotaryEncoder_h


#include <inttypes.h>


#define SW_PRESSED           (1)
#define SW_PRESSEDLONG       (2)


class RotaryEncoder
{
  public:
    RotaryEncoder();
    void init(void);
    void service(void);
    int8_t step(void);
    int8_t sw(void);
  private:
    volatile int8_t re_sw;
    volatile int8_t re_delta;
    int8_t re_last;
};


#endif //RotaryEncoder_h
