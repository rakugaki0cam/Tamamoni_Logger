//
//  touch_2046.h
//
//  2019.09.15
//

#ifndef TOUCH_2046_H
#define TOUCH_2046_H


void touch_init(void);
bool touch_xyz(uint16_t *);
void touch_adc(uint16_t *);


//button
void    button_init(const uint16_t [][4], uint8_t);
uint8_t button_detect(uint16_t, uint16_t, const uint16_t [][4], uint8_t);
uint8_t button_number(const uint16_t [][4], uint8_t);
void    button_select_disp(uint8_t, bool, const uint16_t [][4], uint8_t);

#endif //TOUCH_2046_H