/*
 * pwm.c
 *
 * Created: 2025-03-13 오후 12:39:16
 *  Author: microsoft
 */ 

#include "pwm.h"

void init_timer3(void);
void init_L98N(void);

/*
	16bit timer 3번 활용
	PWM 출력신호
	============
	PE3 : OC3A
	PE4 : OC3B	/ INT4  -> 사용 중(초음파 echo펄스)
	PE5 : OC3C --->  모터 연결
	BTN0 : start / stop
	BTN1 : speed up	(OC3C : 20씩 증가 max 250)
	BTN2 : seed down (OC3C : 20씩 감소 min 60)
*/
void init_timer3(void)
{
	DDRE |= 1 << 3 | 1 << 5;	// PE4 사용중이기 때문에 PE3과 PE5만 	
	// 16bit timer 3번의 모드 5: 8비트 고속 PWM (P348 표 15-6)
	TCCR3A |= 1 << WGM30;
	TCCR3B |= 1 << WGM32;
	
	// 비반전 모드
	// 사용자가 설정한 PWM 값에 일치 시 OC3C의 출력핀이 LOW로 바뀌고 BOTTOM에 HIGH로 바뀐다.
	// P350 표 15-7
	TCCR3A |= 1 << COM3C1; 
	// 분주비 : 64 16,000,000(16MHz) / 64 --> 250,000Hz(250KHz)
	// T = 1/F = 1/250000Hz ==> 0.000004sec(4us)
	// 250,000Hz에서 256개의 펄스를 count하면 소요시간 1.024ms
	//				 127개
	TCCR3B |= 1 << CS31 | 1 << CS30;	//	64분주
	OCR3C = 0;		// OCR(Output Compar Register) PWM값
}

// PF6 : IN1 (L98N)
// PF7 : IN2 (L98N)
// PF6(IN1)	 PF7(IN2)
//	  0			1	: 역회전
//	  1			0	: 정회전
//	  1			1	: STOP
void init_L98N(void)
{
	DDRF |= 1 << 6 | 1 << 7;		// 출력모드
	PORTF &= ~(1 << 6 | 1 << 7);	
	PORTF |= 1 << 6;	// 정회전
}
