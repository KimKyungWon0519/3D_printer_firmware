#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

volatile unsigned long sys_count = 0;
volatile unsigned int TimingDelay = 0;

void init();
void systick_init();
void rcc_init();
void x_motor_init();
void Delay(unsigned int nTime);
void TimingDelay_Decrement(void);

int main(void)
{	
	int i = 0;
	
	init();
	
	while(1) {
		GPIOE->ODR |= (0x01 << 9);
		Delay(20);
		GPIOE->ODR &= ~(0x01 << 9);
		Delay(20);
	}
}

void init() {
	systick_init();
	rcc_init();
	x_motor_init();
}

void systick_init() {
	/* 0.1MHz */
	SysTick->CTRL = 0x07;
	SysTick->LOAD = 1680;
}

void rcc_init() {
	RCC->AHB1ENR |= (0x01 << 4);	// GPIOE
	RCC->AHB1ENR |= (0x01 << 5);	// GPIOF
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