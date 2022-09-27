/*
===============================================================================
 Name        : TimerCapture_Ejemplo.c
 Author      : Bruno A. Genero
===============================================================================
*/

#include "LPC17xx.h"

void confGPIO(void);
void confTimer(void);
uint32_t aux = 0;

int main(void) {

	confGPIO();
	confTimer();

    while(1){}

    return 0 ;
}

void confGPIO(void) {
	LPC_GPIO0->FIODIR |= (1<<22);
	return;
}

void confTimer(void) {
	LPC_SC->PCONP		|=	(1<<1);
	LPC_SC->PCLKSEL0	|=	(1<<2);
	LPC_PINCON->PINSEL3	|=	(3<<20);
	// Carga el contenido de TC en el reg de Cap0 cuando hay flanco de bajada en el pin
	// CAT0.0 y habilita la interrupcion por este pin
	LPC_TIM0->CCR		|= 	(1<<1) | (1<<2);
	LPC_TIM0->TCR		=	3;
	LPC_TIM0->TCR		&= ~(1<<1);
	NVIC_EnableIRQ(TIMER0_IRQn);
	return;
}

void TIMER0_IRQHanlder(void) {
	static uint8_t i = 0;
	// Var aux para observar el valor del reg de captura
	aux = LPC_TIM0->CR0;
	if(i == 0) {
		LPC_GPIO0->FIOSET |= (1<<22);
		i = 1;
	} else if (i == 1) {
		LPC_GPIO0->FIOCLR |= (1<<22);
		i = 0;
	}
	// Limpiamos bandera
	LPC_TIM0->IR |= 1;
	return;
}



