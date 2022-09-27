/*
===============================================================================
 Name        : TimerMatch_Ejemplo.c
 Author      : Bruno A. Genero
===============================================================================
*/

#include "LPC17xx.h"

//void confGPIO(void);
void confTimer(void);

int main(void) {

	//confGPIO();
	confTimer();

	while(1){}

    return 0 ;
}

/*
void confGPIO(void) {
	LPC_GPIO0->FIODIR |= (1<<22);
	return;
}
*/

void confTimer(void) {
	// Alimentamos el TIMER0
	LPC_SC->PCONP		|=	(1<<1);
	// Clock periferico = cclk
	LPC_SC->PCLKSEL0	|=	(1<<2);
	// Pin P1.28 para funcion MAT0 (NO ES GPIO)
	LPC_PINCON->PINSEL3	|=	(3<<24);
	// Funcion de toggle para el pin ext MAT0
	LPC_TIM0->EMR		|=	(3<<4);
	// Cargamos valor para hacer match cada 0,7 segs (prescaler = 0)
	LPC_TIM0->MR0		=	70000000;
	// Hacemos que TC resetee cuando hay match
	LPC_TIM0->MCR		|=	(1<<1);
	// Configuramos para que TIMER0 no interrumpa en match
	LPC_TIM0->MCR		&=	~(1<<0);
	//LPC_TIM0->MCR		|= (1<<0); //-> Esto es para habiltar la int
	// Primero deshabilitamos el Counter y lo Reseteamos ->
	// Es como dejarlo en stand-by
	LPC_TIM0->TCR		=	3;
	// Lo sacamos del reset y le habilitamos ->
	// Se hace esto para empezar a contar en este momento con la config de arriba
	LPC_TIM0->TCR		&= ~(1<<1);
	//
	//NVIC_EnableIRQ(TIMER0_IRQn);
	return;
}

/*
void TIMER0_IRQHandler(void) {
	static uint8_t i = 0;
	if(i == 0) {
		LPC_GPIO0->FIOSET = (1<<22);
		i = 1;
	} else {
		LPC_GPIO0->FIOCLR = (1<<22);
		i = 0;
	}
	LPC_TIM0->IR |= 1;	// Limpiamos bandera de int
	return;
}
*/



