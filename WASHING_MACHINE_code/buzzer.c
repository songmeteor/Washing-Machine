/*
 * buzzer.c
 *
 * Created: 2025-03-14 오후 12:41:21
 *  Author: microsoft
 */ 

#include "buzzer.h"

void init_buzzer(void);
void button_click_sound(void);
void power_on_sound(void);
void power_off_sound(void);
void wash_complete_sound(uint8_t* is_done);

extern uint8_t is_buzzer_playing;
extern int buzzer_delay;

void init_buzzer(void)
{
	BUZZER_DDR |= 1 << BUZZER_PORT_NUM;   // PWM CHANNEL  OC3A(PE3) 출력 모드로 설정 한다.
	TCCR1A = (1<<COM1A0); // COM3A0 : 비교일치시 PE3 출력 반전 (P328 표14-6 참고)
	TCCR1B = (1<<WGM12) | (1<<CS11);   // WGM32 : CTC 4(P327 표14-5) CS31: 8분주(P318)
	// CTC mode : 비교일치가 되면 카운터는 reset되면서 PWM 파형 출력 핀의 출력이 반전 됨.
	// 정상모드와 CTC모드의 차이점은 비교일치 발생시 TCNT1의 레지스터값을 0으로 설정 하는지 여부 이다.
	// 정상모드를 사용시 TCNT1이 자동으로 0으로 설정 되지 않아 인터럽트 루틴에서 TCNT1을 0으로 설정 해 주었다.
	// 위상교정 PWM mode4  CTC 분주비 8  16000000/8 ==> 2,000,000HZ(2000KHZ) :
	// up-dounting: 비교일치시 LOW, down-counting시 HIGH출력
	// 1/2000000 ==> 0.0000005sec (0.5us)
	// P599 TOP 값 계산 참고
	// PWM주파수 = OSC(16M) / ( 2(up.down)x N(분주율)x(1+TOP) )
	// TOP =  (fOSC(16M) /  2(up.down)x N(분주율)x 원하는주파수 )) -1
	//-----------------------------------------------------------
	// - BOTTOM :  카운터가 0x00/0x0000 일때를 가리킨다.
	// - MAX : 카운터가 0xFF/0xFFFF 일 때를 가리킨다.
	// - TOP?:  카운터가 가질 수 있는 최대값을 가리킨다. 오버플로우 인터럽트의 경우 TOP은 0xFF/0xFFFF
	//          이지만 비교일치 인터럽트의 경우 사용자가 설정한 값이 된다.
	
	TCCR1C = 0;  // P328 그림 14-11 참고
	OCR1A = 0;   // 비교 일치값을 OCR3A에 넣는다.
	
	return;
}

void button_click_sound(void)
{
	is_buzzer_playing = 1;
	buzzer_delay = 50;
	OCR1A = MI_02;
}

void power_on_sound(void)
{
	OCR1A = DO_02;  // 도
	_delay_ms(100);
	OCR1A = MI_02;  // 미
	_delay_ms(100);
	OCR1A = SO_02;  // 솔
	_delay_ms(150);
	OCR1A = 0;
}

void power_off_sound(void)
{
	// 도(높은)-라-파 부드러운 하강 시퀀스
	OCR1A = DO_03;  // 높은 도
	_delay_ms(120);
	OCR1A = LA_02;  // 라
	_delay_ms(120);
	OCR1A = FA_02;  // 파
	_delay_ms(180);
	OCR1A = 0;
}

void wash_complete_sound(uint8_t* is_done)
{
	const int Elise_Tune[] = {MI_02, RE_02_H, MI_02, RE_02_H, MI_02, TI_01, RE_02, DO_02, LA_01, 0,
		DO_01, MI_01, LA_01, TI_01, 0, MI_01, SO_01_H, TI_01, DO_02, 0,
		MI_01, MI_02, RE_02_H, MI_02, RE_02_H, MI_02, TI_01, RE_02, DO_02, LA_01, 0,
	DO_01, MI_01, LA_01, TI_01, 0, MI_01, DO_02, TI_01, LA_01, '/0'};

	const int Elise_Beats[] =
	{ BEAT_1_8, BEAT_1_8, BEAT_1_8, BEAT_1_8, BEAT_1_8, BEAT_1_8, BEAT_1_8, BEAT_1_8, BEAT_1_4, BEAT_1_4,
		BEAT_1_8, BEAT_1_8, BEAT_1_8, BEAT_1_4, BEAT_1_4, BEAT_1_8, BEAT_1_8, BEAT_1_8, BEAT_1_4, BEAT_1_4,
		BEAT_1_8, BEAT_1_8, BEAT_1_8, BEAT_1_8, BEAT_1_8, BEAT_1_8, BEAT_1_8, BEAT_1_8, BEAT_1_8, BEAT_1_4, BEAT_1_4,
	BEAT_1_8, BEAT_1_8, BEAT_1_8, BEAT_1_4, BEAT_1_4, BEAT_1_8, BEAT_1_8, BEAT_1_8, BEAT_1_2, '\0'};
	
	static uint8_t idx = 0;
	
	if (is_buzzer_playing == 0)
	{
		if (Elise_Beats[idx] == '\0')
		{
			idx = 0;
			*is_done = 1;
			return;
		}
		OCR1A = Elise_Tune[idx];
		is_buzzer_playing = 1;
		buzzer_delay = Elise_Beats[idx];
		idx++;
	}
	return;
}