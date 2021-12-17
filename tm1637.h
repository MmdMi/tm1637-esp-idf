#ifndef __TM1637_H__
#define __TM1637_H__

#include <stdint.h>
#include <driver/gpio.h>

typedef struct{
	uint8_t CLK_PIN;
	uint8_t DIO_PIN;
	uint8_t brightness;
}tm1637__7seg_t;


tm1637__7seg_t * tm1637_init(uint8_t pin_clk, uint8_t pin_data);

void tm1637_set_brightness(tm1637__7seg_t * _7seg, uint8_t level);

void tm1637_turnoff(tm1637__7seg_t *_7seg);

void tm1637_set_seg_value(tm1637__7seg_t *_7seg, const uint8_t seg_num, const uint8_t value, const bool dot);

void tm1637_set_seg_raw(tm1637__7seg_t * _7seg, const uint8_t segment_idx, const uint8_t data);

void tm1637_set_value(tm1637__7seg_t *_7seg, uint16_t value);

void tm1637_set_value_zero_dot(tm1637__7seg_t *_7seg, uint16_t value, bool zero, const uint8_t dot_mask);

#endif // __TM1637_H__
