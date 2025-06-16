/*
 * fnd.c
 *
 * Created: 2025-03-06 오후 12:23:41
 *  Author: microsoft
 */ 
#include "fnd.h"
void init_fnd(void);
void fnd_display_left_time(uint8_t circle_mode, uint8_t left_curr_mode_time, int left_total_time, uint8_t is_done);
void fnd_display_set_mode(uint8_t mode_set_num ,int total_sec_count);

extern uint8_t is_stop;

void fnd_display_left_time(uint8_t circle_mode, uint8_t left_curr_mode_time, int left_total_time, uint8_t is_done)
{
	uint8_t fnd_font[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x27, 0x7f, 0x67, 0x80};
	uint8_t fnd_circle[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20};
	uint8_t fnd_stop[] = {0x73, 0x3f, 0x78, 0x6d};
	uint8_t fnd_done[] = {0x7b, 0x54, 0x5c, 0x5e};
	
	FND_DIGIT_PORT |= (1 << FND_DIGIT_D1 | 1 << FND_DIGIT_D2 | 1 << FND_DIGIT_D3 | 1 << FND_DIGIT_D4 | 1 << FND_DIGIT_D5 | 1 << FND_DIGIT_D7 | 1 << FND_DIGIT_D8);
	FND_DIGIT_D6_PORT |= 1 << FND_DIGIT_D6;
	
	static int digit_select=0;  // static를 쓰면 전역 변수 처럼 함수가 빠져 나갔다가 다시 들어 오더라도 값을 유지
	
	switch(digit_select)
	{
		case 0:
			FND_DIGIT_PORT &= ~(1 << FND_DIGIT_D8);   // 011111111 FND_DIGIT_PORT = ~0x80
			FND_DATA_PORT = fnd_font[(left_total_time % 10)];   // 0~9초
			break;
		case 1:
			FND_DIGIT_PORT &= ~(1 << FND_DIGIT_D7);
			FND_DATA_PORT = fnd_font[(left_total_time / 10) % 6];   // 10단위초
			break;
		case 2:
			FND_DIGIT_D6_PORT &= ~(1 << FND_DIGIT_D6);
			FND_DATA_PORT = fnd_font[left_total_time / 60];   // 1단위 분
			break;
		case 3:
			FND_DIGIT_PORT &= ~(1 << FND_DIGIT_D5);
			FND_DATA_PORT = is_done ? fnd_font[0] : fnd_circle[circle_mode];
			break;
		case 4:
			FND_DIGIT_PORT &= ~(1 << FND_DIGIT_D4);
			FND_DATA_PORT =  is_stop ? fnd_stop[digit_select-4] : (is_done ? fnd_done[digit_select-4] : fnd_font[left_curr_mode_time % 10]);
			break;
		case 5:
			FND_DIGIT_PORT &= ~(1 << FND_DIGIT_D3);
			FND_DATA_PORT = is_stop ? fnd_stop[digit_select-4] : (is_done ? fnd_done[digit_select-4] : fnd_font[(left_curr_mode_time / 10) % 6]);
			break;
		case 6:
			FND_DIGIT_PORT &= ~(1 << FND_DIGIT_D2);
			FND_DATA_PORT = is_stop ? fnd_stop[digit_select-4] : (is_done ? fnd_done[digit_select-4] : fnd_font[left_curr_mode_time / 60]);
			break;
		case 7:
			FND_DIGIT_PORT &= ~(1 << FND_DIGIT_D1);
			FND_DATA_PORT = is_stop ? fnd_stop[digit_select-4] : (is_done ? fnd_done[digit_select-4] : fnd_circle[circle_mode]);
			break;
	}
	digit_select++;
	digit_select %= 8;   // 다음 표시할 자리수 선택
}

void fnd_display_set_mode(uint8_t mode_set_num ,int total_sec_count)
{
	uint8_t fnd_font[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x27, 0x7f, 0x67, 0x80};
	
	FND_DIGIT_PORT |= (1 << FND_DIGIT_D1 | 1 << FND_DIGIT_D2 | 1 << FND_DIGIT_D3 | 1 << FND_DIGIT_D4 | 1 << FND_DIGIT_D5 | 1 << FND_DIGIT_D7 | 1 << FND_DIGIT_D8);
	FND_DIGIT_D6_PORT |= 1 << FND_DIGIT_D6;
	
	static int digit_select=0;  // static를 쓰면 전역 변수 처럼 함수가 빠져 나갔다가 다시 들어 오더라도 값을 유지
	
	switch(digit_select)
	{
		case 0:
		FND_DIGIT_PORT &= ~(1 << FND_DIGIT_D8);   // 011111111 FND_DIGIT_PORT = ~0x80
		FND_DATA_PORT = fnd_font[total_sec_count % 10];   // 0~9초
		break;
		case 1:
		FND_DIGIT_PORT &= ~(1 << FND_DIGIT_D7);
		FND_DATA_PORT = fnd_font[total_sec_count / 10 % 6];   // 10단위초
		break;
		case 2:
		FND_DIGIT_D6_PORT &= ~(1 << FND_DIGIT_D6);
		FND_DATA_PORT = fnd_font[total_sec_count / 60 % 10];   // 1단위 분
		break;
		case 3:
		FND_DIGIT_PORT &= ~(1 << FND_DIGIT_D5);
		FND_DATA_PORT = fnd_font[total_sec_count / 600 % 6];   // 10단위 분
		break;
		case 4:
		FND_DIGIT_PORT &= ~(1 << FND_DIGIT_D4);
		FND_DATA_PORT = fnd_font[mode_set_num % 10];
		break;
		case 5:
		FND_DIGIT_PORT &= ~(1 << FND_DIGIT_D3);
		FND_DATA_PORT = fnd_font[(mode_set_num / 10) % 10];
		break;
		case 6:
		FND_DIGIT_PORT &= ~(1 << FND_DIGIT_D2);
		FND_DATA_PORT = fnd_font[(mode_set_num / 100) % 10];
		break;
		case 7:
		FND_DIGIT_PORT &= ~(1 << FND_DIGIT_D1);
		FND_DATA_PORT = fnd_font[(mode_set_num / 1000) % 10];
		break;
	}
	digit_select++;
	digit_select %= 8;   // 다음 표시할 자리수 선택
}

void init_fnd(void)
{
	FND_DATA_DDR = 0xff;  // 출력 모드로 설정
	
	// FND_DIGIT_DDR |= 0xf0;   // 자릿수 선택 7654 
	FND_DIGIT_DDR |= (1 << FND_DIGIT_D1 | 1 << FND_DIGIT_D2 | 1 << FND_DIGIT_D3 | 1 << FND_DIGIT_D4 | 1 << FND_DIGIT_D5 | 1 << FND_DIGIT_D7 | 1 << FND_DIGIT_D8);
	FND_DIGIT_D6_DDR |= 1 << FND_DIGIT_D6;
	
	FND_DATA_PORT = 0x00;   // fnd를 all off   
	
	FND_DIGIT_PORT |= (1 << FND_DIGIT_D1 | 1 << FND_DIGIT_D2 | 1 << FND_DIGIT_D3 | 1 << FND_DIGIT_D4 | 1 << FND_DIGIT_D5 | 1 << FND_DIGIT_D7 | 1 << FND_DIGIT_D8);
	FND_DIGIT_D6_PORT |= 1 << FND_DIGIT_D6;
}