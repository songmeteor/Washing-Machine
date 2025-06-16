/*
 * led.c
 *
 * Created: 2025-03-14 오후 12:51:56
 *  Author: microsoft
 */ 

#include "led.h"

void init_led(void);
void led_blink(void);

extern volatile int msec_count;

void init_led(void)
{
	LED_DDR = 0xff;
	LED_PORT = 0x00;
}

void led_blink(void)
{
	if (msec_count == 100)
	{
		LED_PORT = ~LED_PORT;
		msec_count = 0;
	}
}