/*
 * led.h
 *
 * Created: 2025-03-14 오후 12:47:37
 *  Author: microsoft
 */ 


#ifndef LED_H_
#define LED_H_
#define  F_CPU 16000000UL  // 16MHZ
#include <avr/io.h>
#include <util/delay.h>  // _delay_ms _delay_us

#define LED_DDR		DDRA
#define LED_PORT	PORTA

#endif /* LED_H_ */