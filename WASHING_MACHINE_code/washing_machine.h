/*
 * washing_machine.h
 *
 * Created: 2025-03-15 오후 5:48:34
 *  Author: microsoft
 */ 


#ifndef WASHING_MACHINE_H_
#define WASHING_MACHINE_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#define  F_CPU 16000000UL  // 16MHZ

#include "led.h"
#include "button.h"

#define MAX_WAHSING_TIME	40
#define MIN_WAHSING_TIME	10
#define MAX_RINSE_COUNT		5
#define MIN_RINSE_COUNT		1
#define MAX_DRY_TIME		50
#define MIN_DRY_TIME		10

#define POWER0_PWM	80
#define POWER1_PWM	100
#define POWER2_PWM	120
#define DRY_PWM		40

#define RINSE_TIME 15

#endif /* WASHING_MACHINE_H_ */