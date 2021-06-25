#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define ONE_CM 800

#define X_STEP (0x01 << 9)
#define Y_STEP (0x01 << 11)

volatile unsigned long sys_count = 0;
volatile unsigned int TimingDelay = 0;

void init();
void systick_init();
void rcc_init();
void x_motor_init();
void y_motor_init();
void timer2_init();
void timer3_init();
void nvic_init();
void move_x(double distance);
void move_y(double distance);
void gotoXY(double x, double y);
void Delay(unsigned int nTime);
void TimingDelay_Decrement(void);
void TIM2_IRQHandler (void);

char toggle_x = 0;
char move_x_enable = 0;
unsigned int step_count_x = 0;
unsigned int max_count_x = 0;
char toggle_y = 0;
char move_y_enable = 0;
unsigned int step_count_y = 0;
unsigned int max_count_y = 0;

int main(void)
{	
	int i = 0;

	init();
	
	gotoXY(5, 0);
	
	while(1);
}

void gotoXY(double x, double y) {
	move_x(x);
	move_y(y);
}

void move_x(double distance) {
	max_count_x = distance * ONE_CM;
	step_count_x = 0;
	move_x_enable = 1;
}

void move_y(double distance) {
	max_count_y = distance * ONE_CM;
	step_count_y = 0;
	move_y_enable = 1;
}

void init() {
	systick_init();
	rcc_init();
	x_motor_init();
	y_motor_init();
	timer2_init();
	timer3_init();
	nvic_init();
}

void systick_init() {
	/* 0.1MHz */
	SysTick->CTRL = 0x07;
	SysTick->LOAD = 1680;
}

void rcc_init() {
	RCC->AHB1ENR |= (0x01 << 3);	// GPIOD
	RCC->AHB1ENR |= (0x01 << 4);	// GPIOE
	RCC->AHB1ENR |= (0x01 << 5);	// GPIOF
	
	RCC->APB1ENR |= (0x01 << 0);	// TIM2
	RCC->APB1ENR |= (0x01 << 1);	// TIM3
}

void x_motor_init() {
	/* x enable ( PF2 ) */
	GPIOF->MODER |= (0x01 << 2 * 2);			// ouput mode
	GPIOF->OTYPER |= (0x00 << 2);					// push pull
	GPIOF->OSPEEDR |= (0x03 << 2 * 2);	// Very high speed 

	/* x step ( PE9 ) */
	GPIOE->MODER |= (0x01 << ( 9 * 2 ));		// ouput mode
	GPIOE->OTYPER |= (0x00 << 9);						// push pull
	GPIOE->OSPEEDR |= (0x03 << (9 * 2));	// Very high speed 
}

void y_motor_init() {	
	/* y enable ( PD7 ) */
	GPIOD->MODER |= (0x01 << 7 * 2);			// ouput mode
	GPIOD->OTYPER |= (0x00 << 7);					// push pull
	GPIOD->OSPEEDR |= (0x03 << 7 * 2);	// Very high speed 

	/* y step ( PE11 ) */
	GPIOE->MODER |= (0x01 << ( 11 * 2 ));		// ouput mode
	GPIOE->OTYPER |= (0x00 << 11);						// push pull
	GPIOE->OSPEEDR |= (0x03 << (11 * 2));	// Very high speed 
}

void timer2_init() {
	TIM2->CR1 |= (0x01 << 0);	// CEN (Counter Enable)
	TIM2->DIER |= (0x01 << 0);	// UIE ( Update Interrupt Enable )
	TIM2->CNT = 0;									// Timer Count
	TIM2->PSC = 84 - 1;						// 1us
	TIM2->ARR = 200 - 1;					// 200 count
}

void timer3_init() {
	TIM3->CR1 |= (0x01 << 0);	// CEN (Counter Enable)
	TIM3->DIER |= (0x01 << 0);	// UIE ( Update Interrupt Enable )
	TIM3->CNT = 0;									// Timer Count
	TIM3->PSC = 84 - 1;						// 1us
	TIM3->ARR = 200 - 1;					// 200 count
}

void nvic_init() {
	NVIC->ISER[0] |= (0x01 << 28);	// TIM2 Enable
	NVIC->ISER[0] |= (0x01 << 29);	// TIM3 Enable
}

void TIM2_IRQHandler (void) {
	if ((TIM2->SR & 0x0001) != 0) { 
		if(move_x_enable) {
			if(toggle_x == 0) {
				GPIOE->ODR |= X_STEP;
				toggle_x = 1;
			} else {
				GPIOE->ODR &= ~X_STEP;
				toggle_x = 0;
				
				if(step_count_x >= max_count_x) {
					move_x_enable = 0;
				}
				
				step_count_x++;
			}
		}

	TIM2->SR &= ~(1<<0); // clear UIF flag
	}		
}

void TIM3_IRQHandler (void) {
	if ((TIM3->SR & 0x0001) != 0) { 
		if(move_y_enable) {
			if(toggle_y == 0) {
				GPIOE->ODR |= Y_STEP;
				toggle_y = 1;
			} else {
				GPIOE->ODR &= ~Y_STEP;
				toggle_y = 0;
				
				if(step_count_y >= max_count_y) {
					move_y_enable = 0;
				}
				
				step_count_y++;
			}
		}

	TIM3->SR &= ~(1<<0); // clear UIF flag
	}		
}

void Delay(unsigned int nTime)
{ 
	TimingDelay = nTime;

	while(TimingDelay > 0);
}

void TimingDelay_Decrement(void)
{
	if (TimingDelay != 0x00)
	{ 
		TimingDelay--;
	}
}