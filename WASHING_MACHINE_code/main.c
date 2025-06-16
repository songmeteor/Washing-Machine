/*
 * main.c
 *
 * Created: 2025-03-14 오후 12:25:34
 * Author : microsoft
 */ 

#include "washing_machine.h"

extern void init_timer3(void);
extern void init_button(void);
extern void init_fnd(void);
extern void init_L98N(void);
extern void init_buzzer(void);
extern void init_led(void);

extern int get_button(int button_num, int button_pin);
extern void fnd_display_set_mode(uint8_t mode_set_num, int total_left_time);
extern void fnd_display_left_time(int circle_mode, int left_curr_mode_time, int left_total_time, uint8_t is_done);
extern void button_click_sound(void);
extern void power_on_sound(void);
extern void power_off_sound(void);
extern void wash_complete_sound(uint8_t* is_done);
extern void led_blink(void);

void init_main(void);
void init_parameters(uint8_t* is_on, uint8_t* curr_washing_mode, uint8_t* wash_power, uint8_t* total_rinse_count, uint8_t* total_washing_time, uint8_t* total_rinse_time, uint8_t* total_dry_time, uint8_t* circle_mode, int* total_left_time);
void init_timer0(void);

void on_button_check(uint8_t* is_on);
void stop_check(void);

void set_mode(uint8_t* curr_washing_mode, uint8_t** params, uint8_t pwm);
void set_wash_power(uint8_t* wash_power);
void set_total_washing_time(uint8_t* total_washing_time);
void set_total_rinse_count(uint8_t* total_rinse_count);
void set_total_dry_time(uint8_t* total_dry_time);

void operate(uint8_t* curr_washing_mode, uint8_t** left_mode_time, int* left_total_sec_count, uint8_t* circle_mode, uint8_t wash_power);
void update_sec_circle(uint8_t* circle_mode, uint8_t wash_power);

uint8_t operating_mode = 0;
uint8_t is_stop = 0;

uint8_t fnd_clk = 0;
uint8_t clk_1hz = 0;
volatile int msec_count = 0;	
volatile int circle_count = 0;

uint8_t is_buzzer_playing = 0;
int buzzer_msec = 0;
int buzzer_delay = 0;

//250개의 pluse를 count(1ms)하면 자동적으로 ISR 실행, ISR은 가능한 짧게 작성
ISR(TIMER0_OVF_vect)		// 매개변수는 P278 표 12-3 참고
{
	TCNT0 = 6;
	fnd_clk = 1;
	if (is_buzzer_playing)
	{
		buzzer_msec++;
		if (buzzer_msec == buzzer_delay)
		{
			is_buzzer_playing = 0;
			buzzer_msec = 0;
			buzzer_delay = 0;
			OCR1A = 0;
		}
	}
	if (~is_stop)
	{
		msec_count++;
		circle_count++;	
	}
}	// 이것도 조금 길다.. 더 짧게 해야함

int main(void)
{
	uint8_t is_on = 0;
	
	uint8_t curr_washing_mode = 0;
	uint8_t wash_power = 1;
	uint8_t total_rinse_count = 2;
	uint8_t total_washing_time = 15;
	uint8_t total_rinse_time = RINSE_TIME * total_rinse_count;
	uint8_t total_dry_time = 30;
	
	uint8_t circle_mode = 0; 
	
	uint8_t* left_mode_time[] = {&total_washing_time, &total_rinse_time, &total_dry_time};
	int total_left_time = total_washing_time + total_rinse_time + total_dry_time;
	
	uint8_t pwms[] = {POWER0_PWM, POWER1_PWM, POWER2_PWM};
	uint8_t* params[] = {&wash_power, &total_washing_time, &total_rinse_count, &total_dry_time};
	
	uint8_t done_total_time = 0;
	
	while(1)
	{	
		on_button_check(&is_on);
		if (!is_on)
		{
			init_main();
			init_parameters(&is_on, &curr_washing_mode, &wash_power, &total_rinse_count, &total_washing_time, &total_rinse_time, &total_dry_time, &circle_mode, &total_left_time);
			continue;
		}
		
		if (!operating_mode)
		{
			set_mode(&curr_washing_mode, params, pwms[wash_power]);
			total_rinse_time = RINSE_TIME * total_rinse_count;
			total_left_time = total_washing_time + total_rinse_time + total_dry_time;
			done_total_time = total_left_time;
			if(fnd_clk)
			{
				fnd_clk = 0;
				fnd_display_set_mode(*params[curr_washing_mode], total_left_time);
			}
		}
		else if (operating_mode == 1)
		{
			stop_check();
			if (!is_stop)
				operate(&curr_washing_mode, left_mode_time, &total_left_time, &circle_mode, wash_power);
			if(fnd_clk)
			{
				fnd_clk = 0;
				fnd_display_left_time(circle_mode, *left_mode_time[curr_washing_mode-1], total_left_time, 0);
			}
		}
		else if (operating_mode == 2)
		{	
			static uint8_t is_done = 0;
			wash_complete_sound(&is_done);
			led_blink();
			if(fnd_clk)
			{
				fnd_clk = 0;
				fnd_display_left_time(circle_mode, 0, done_total_time, 1);
			}
			if (is_done)
			{
				is_done = 0;
				init_main();
				init_parameters(&is_on, &curr_washing_mode, &wash_power, &total_rinse_count, &total_washing_time, &total_rinse_time, &total_dry_time, &circle_mode, &total_left_time);
			}
		}
		
	}
}

void init_main(void)
{
	init_timer0();
	init_timer3();
	init_fnd();
	init_L98N();
	init_button();
	init_buzzer();
	init_led();
	sei();
}

void init_parameters(uint8_t* is_on, uint8_t* curr_washing_mode, uint8_t* wash_power, uint8_t* total_rinse_count, uint8_t* total_washing_time, uint8_t* total_rinse_time, uint8_t* total_dry_time, uint8_t* circle_mode, int* total_left_time)
{
    *is_on = 0;
    *curr_washing_mode = 0;
    *wash_power = 1;          
    *total_rinse_count = 2;   
    *total_washing_time = 15;
    *total_rinse_time = RINSE_TIME * (*total_rinse_count);
    *total_dry_time = 30;
    *circle_mode = 0;
    *total_left_time = *total_washing_time + *total_rinse_time + *total_dry_time;  
    operating_mode = 0;
	is_stop = 0;
    msec_count = 0;
    circle_count = 0;
    fnd_clk = 0;
    clk_1hz = 0;
}

// timer0를 초기화, AVR에서 8bit timer 0/2번중 0번을 초기화
void init_timer0(void)
{
	// 16MHz /64 분주(down) 분주: divider/prescale
	// -----분주비 계산-----
	// (1) 16,000,000Hz/64 ==> 250,000Hz
	// (2) T(주기) 1clock의 소요시간 : 1/f = 1/250,000 ==> 0.000004sec(4us, 0.004ms)
	// (3) 8bite timer OV(OverFlow) : 0.004ms * 256 = 0.001024sec --> 1.024ms
	// 1ms마다 정확하게 INT를 띄우고 싶으면 0.004ms * 250개를 count --> 0.001sec --> 1ms
	TCNT0=6;	// TCNT: 0~256 1ms마다 TIMER0_OVF_vector로 진입
	// TCNT0 = 6으로 설정한 이유 : 6-->256 : 50개의 펄스를 count하기 때문에 1ms가 됨
	// (4) 분주비 설정 (250,000Hz --> 250KHz) P296 표 13-1
	TCCR0 |= 1 << CS02 | 0 << CS01 | 0 << CS00;		// TCCR0 |= 0xf4
	// (5) Timer0 overflow INT를 허용(enable), P297 그림 13-5
	TIMSK |= 1 << TOIE0;
}

void on_button_check(uint8_t* is_on)
{
	if (get_button(BUTTON0, BUTTON0PIN))
	{
		*is_on = !*is_on;
		if (*is_on)
		{
			power_on_sound();
			LED_PORT = 0x01;
		}
		else
		{
			init_main();
			power_off_sound();
		}
	}
}

void stop_check(void)
{
	static uint8_t prev_pwm;
	if (get_button(BUTTON3, BUTTON3PIN))
	{
		button_click_sound();
		is_stop = !is_stop;
		if (is_stop) 
		{
			prev_pwm = OCR3C;
			OCR3C = 0;
		}
		else OCR3C = prev_pwm;
	}
}

void set_mode(uint8_t* curr_washing_mode, uint8_t** params, uint8_t pwm)
{
	void (*fp[])(uint8_t*) = {set_wash_power, set_total_washing_time, set_total_rinse_count, set_total_dry_time};
	if(get_button(BUTTON1, BUTTON1PIN))	// 세탁 설정
	{
		button_click_sound();
		fp[*curr_washing_mode](params[*curr_washing_mode]);
	}
	else if(get_button(BUTTON2, BUTTON2PIN))	// 다음 설정 모드
	{
		(*curr_washing_mode)++;
		if (*curr_washing_mode == 4)
		{
			operating_mode = 1;
			*curr_washing_mode = 1;
			msec_count = 0;
			OCR3C = pwm;
		}
		LED_PORT = 1 << *curr_washing_mode;
		button_click_sound();
	}
	else if(get_button(BUTTON3, BUTTON3PIN))	// 이전 설정 모드
	{
		*curr_washing_mode = (*curr_washing_mode) ? *curr_washing_mode - 1 : 0;
		LED_PORT = 1 << *curr_washing_mode;
		button_click_sound();
	}
}

void set_wash_power(uint8_t* wash_power)
{
	*wash_power = (*wash_power + 1) % 3;
}

void set_total_washing_time(uint8_t* total_washing_time)
{
	(*total_washing_time)++;
	if (*total_washing_time > MAX_WAHSING_TIME) *total_washing_time = MIN_WAHSING_TIME;
}

void set_total_rinse_count(uint8_t* total_rinse_count)
{
	(*total_rinse_count)++;
	if (*total_rinse_count > MAX_RINSE_COUNT) *total_rinse_count = MIN_RINSE_COUNT;
}

void set_total_dry_time(uint8_t* total_dry_time)
{
	(*total_dry_time)++;
	if (*total_dry_time > MAX_DRY_TIME) *total_dry_time = MIN_DRY_TIME;
}

void operate(uint8_t* curr_washing_mode, uint8_t** left_mode_time, int* left_total_sec_count, uint8_t* circle_mode, uint8_t wash_power)
{
	static uint8_t prev_pwm = 0;
	update_sec_circle(circle_mode, wash_power);
	if (clk_1hz)
	{
		if (*curr_washing_mode != 3)
			PORTF ^= 0xc0; 
		clk_1hz = 0;
		
		if (*curr_washing_mode == 2 && *left_mode_time[1] % RINSE_TIME == 0)
		{
			OCR3C = prev_pwm;
		}
		else if (*curr_washing_mode == 2 && *left_mode_time[1] % RINSE_TIME == 2)
		{
			OCR3C = 0;
		}
		
		(*left_mode_time[(*curr_washing_mode)-1])--;
		(*left_total_sec_count)--;
		
		if (*left_mode_time[*curr_washing_mode - 1] == 2)
		{
			prev_pwm = OCR3C;
			OCR3C = 0;
		}
		
		if (*left_mode_time[(*curr_washing_mode)-1] == 0)
		{
			(*curr_washing_mode)++;
			LED_PORT = 1 << *curr_washing_mode;
			if (*curr_washing_mode == 3) OCR3C = prev_pwm + DRY_PWM;
			else if (*curr_washing_mode == 4)
			{
				OCR3C = 0;
				LED_PORT = 0x00;
				operating_mode = 2;
			}
		}
	}
}

void update_sec_circle(uint8_t* circle_mode, uint8_t wash_power)
{
	uint8_t circle_cycle[] = {150, 100, 50}; 
	if(msec_count >= 1000)
	{
		msec_count = 0;
		clk_1hz = 1;
	}
	if(circle_count >= circle_cycle[wash_power])
	{
		circle_count = 0;
		*circle_mode = (*circle_mode + 1) % 6;
	}
}