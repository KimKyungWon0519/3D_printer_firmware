#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define X_STEP (0x01 << 9)

volatile unsigned long sys_count = 0;
volatile unsigned int TimingDelay = 0;

void init();
void systick_init();
void rcc_init();
void x_motor_init();
void timer2_init();
void nvic_init();
void Delay(unsigned int nTime);
void TimingDelay_Decrement(void);
void TIM2_IRQHandler (void);

char toggle_x = 0;
unsigned int count_x = 0;
	
int main(void)
{	
	int i = 0;
	
	init();
	
	while(1);
}

void init() {
	systick_init();
	rcc_init();
	x_motor_init();
	timer2_init();
	nvic_init();
}

void systick_init() {
	/* 0.1MHz */
	SysTick->CTRL = 0x07;
	SysTick->LOAD = 1680;
}

void rcc_init() {
	RCC->AHB1ENR |= (0x01 << 4);	// GPIOE
	RCC->AHB1ENR |= (0x01 << 5);	// GPIOF
	RCC->APB1ENR |= (0x01 << 0);	// TIM2
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

void timer2_init() {
	TIM2->CR1 |= (0x01 << 0);	// CEN (Counter Enable)
	TIM2->DIER |= (0x01 << 0);	// UIE ( Update Interrupt Enable )
	TIM2->CNT = 0;									// Timer Count
	TIM2->PSC = 84 - 1;						// 1us
	TIM2->ARR = 200 - 1;					// 200 count
}

void nvic_init() {
	NVIC->ISER[0] |= (0x01 << 28);	// TIM2 Enable
}

void TIM2_IRQHandler (void) {
	 if ((TIM2->SR & 0x0001) != 0) { 
		if(toggle_x == 0) {
			GPIOE->ODR |= (0x01 << 9);
			toggle_x = 1;
		} else {
			GPIOE->ODR &= ~(0x01 << 9);
			toggle_x = 0;
		}
		 
		TIM2->SR &= ~(1<<0); // clear UIF flag
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