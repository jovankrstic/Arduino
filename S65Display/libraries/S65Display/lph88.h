#ifndef lph88_h
#define lph88_h


#ifdef S65_LPH88

void s65_drawStart(void);
void s65_draw(uint16_t color);
void s65_drawStop(void);
void s65_setArea(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void s65_setCursor(uint8_t x, uint8_t y);
void s65_init(void);
void s65_writeReg(uint8_t reg);
void s65_writeData(uint16_t data);
void s65_writeCmd(uint8_t reg, uint16_t param);
void s65_writeSPI(uint8_t data);

#endif


#endif //lph88_h
