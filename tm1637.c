#include "tm1637.h"

static const int8_t num_list[] = {
	0b00111111, // 0
	0b00000110, // 1
	0b01011011, // 2
	0b01001111, // 3
	0b01100110, // 4
	0b01101101, // 5
	0b01111101, // 6
	0b00000111, // 7
	0b01111111, // 8
	0b01101111, // 9
	0b01110111, // A
	0b01111100, // b
	0b00111001, // C
	0b01011110, // d
	0b01111001, // E
	0b01110001	// F
};

/************* Private functions *************/

void tm1637_data_start(tm1637__7seg_t *_7seg)
{
	gpio_set_level(_7seg->CLK_PIN, 1);
	gpio_set_level(_7seg->DIO_PIN, 1);
	gpio_set_level(_7seg->DIO_PIN, 0);
	gpio_set_level(_7seg->CLK_PIN, 0);
}

void tm1637_data_stop(tm1637__7seg_t *_7seg)
{
	gpio_set_level(_7seg->CLK_PIN, 0);
	gpio_set_level(_7seg->DIO_PIN, 0);
	gpio_set_level(_7seg->CLK_PIN, 1);
	gpio_set_level(_7seg->DIO_PIN, 1);
}

void tm1637_data_send_byte(tm1637__7seg_t *_7seg, uint8_t byte)
{
	for (uint8_t i = 0; i < 8; ++i)
	{
		gpio_set_level(_7seg->CLK_PIN, 0);
		gpio_set_level(_7seg->DIO_PIN, (byte & 0x01));
		gpio_set_level(_7seg->CLK_PIN, 1);
		byte >>= 1;
	}

	gpio_set_level(_7seg->CLK_PIN, 0);
	gpio_set_level(_7seg->DIO_PIN, 1);
	gpio_set_level(_7seg->CLK_PIN, 1);
	gpio_set_direction(_7seg->DIO_PIN, GPIO_MODE_INPUT);
	uint8_t ack = gpio_get_level(_7seg->DIO_PIN);
	if (ack == 0)
	{
		gpio_set_direction(_7seg->DIO_PIN, GPIO_MODE_OUTPUT);
		gpio_set_level(_7seg->DIO_PIN, 0);
	}
	gpio_set_direction(_7seg->DIO_PIN, GPIO_MODE_OUTPUT);
}

/************* Public functions *************/

tm1637__7seg_t *tm1637_init(uint8_t pin_clk, uint8_t pin_data)
{
	tm1637__7seg_t *_7seg = malloc(sizeof(tm1637__7seg_t));
	_7seg->CLK_PIN = pin_clk;
	_7seg->DIO_PIN = pin_data;
	_7seg->brightness = 0x07;
	gpio_set_direction(pin_clk, GPIO_MODE_OUTPUT);
	gpio_set_direction(pin_data, GPIO_MODE_OUTPUT);
	gpio_set_level(pin_clk, 0);
	gpio_set_level(pin_data, 0);
	return _7seg;
}

void tm1637_set_brightness(tm1637__7seg_t *_7seg, uint8_t level)
{
	_7seg->brightness = (level > 7 ? 7 : level);
}

void tm1637_turnoff(tm1637__7seg_t *_7seg){
	for(uint8_t i=0;i<4;i++){
		tm1637_set_seg_raw(_7seg,i,0x00);
	}
}

void tm1637_set_seg_value(tm1637__7seg_t *_7seg, const uint8_t seg_num, const uint8_t value, const bool dot)
{
	uint8_t data = 0x00;
	if (value < (sizeof(num_list) / sizeof(num_list[0])))
	{
		data = (dot ? (num_list[value] | 0x80) : num_list[value]);
	}
	tm1637_set_seg_raw(_7seg, seg_num, data);
}

void tm1637_set_seg_raw(tm1637__7seg_t *_7seg, const uint8_t seg_num, const uint8_t data)
{
	tm1637_data_start(_7seg);
	tm1637_data_send_byte(_7seg, 0x44);
	tm1637_data_stop(_7seg);
	tm1637_data_start(_7seg);
	tm1637_data_send_byte(_7seg, (seg_num | 0xc0));
	tm1637_data_send_byte(_7seg, data);
	tm1637_data_stop(_7seg);
	tm1637_data_start(_7seg);
	tm1637_data_send_byte(_7seg, (_7seg->brightness | 0x88));
	tm1637_data_stop(_7seg);
}

void tm1637_set_value(tm1637__7seg_t *_7seg, uint16_t value)
{
	tm1637_set_value_zero_dot(_7seg, value, false, 0x00);
}

void tm1637_set_value_zero_dot(tm1637__7seg_t *_7seg, uint16_t value, bool zero, const uint8_t dot_mask)
{
	uint8_t zero_seg = zero ? 0xFF : num_list[0];
	if (value < 10)
	{
		tm1637_set_seg_value(_7seg, 3, value, dot_mask & 0x01);
		tm1637_set_seg_value(_7seg, 2, zero_seg, dot_mask & 0x02);
		tm1637_set_seg_value(_7seg, 1, zero_seg, dot_mask & 0x04);
		tm1637_set_seg_value(_7seg, 0, zero_seg, dot_mask & 0x08);
	}
	else if (value < 100)
	{
		tm1637_set_seg_value(_7seg, 3, value % 10, dot_mask & 0x01);
		tm1637_set_seg_value(_7seg, 2, (value / 10) % 10, dot_mask & 0x02);
		tm1637_set_seg_value(_7seg, 1, zero_seg, dot_mask & 0x04);
		tm1637_set_seg_value(_7seg, 0, zero_seg, dot_mask & 0x08);
	}
	else if (value < 1000)
	{
		tm1637_set_seg_value(_7seg, 3, value % 10, dot_mask & 0x01);
		tm1637_set_seg_value(_7seg, 2, (value / 10) % 10, dot_mask & 0x02);
		tm1637_set_seg_value(_7seg, 1, (value / 100) % 10, dot_mask & 0x04);
		tm1637_set_seg_value(_7seg, 0, zero_seg, dot_mask & 0x08);
	}
	else
	{
		tm1637_set_seg_value(_7seg, 3, value % 10, dot_mask & 0x01);
		tm1637_set_seg_value(_7seg, 2, (value / 10) % 10, dot_mask & 0x02);
		tm1637_set_seg_value(_7seg, 1, (value / 100) % 10, dot_mask & 0x04);
		tm1637_set_seg_value(_7seg, 0, (value / 1000) % 10, dot_mask & 0x08);
	}
}
