/*
===============================================================================
 Name        : EjTimerMatch2_Genero.c
 Author      : Bruno A. Genero
 Description : Escribir el código que configure el timer0 para cumplir
 con las especificaciones dadas en la figura adjunta. (Pag 510 Figura 115
 del manual de usuario del LPC 1769). Considerar una frecuencia de cclk
 de 100 Mhz y una división de reloj de periférico de 2.
===============================================================================
*/

#include "LPC17xx.h"

void confGPIO(void);
void confTimer(void);

int main(void) {

	confGPIO();
	confTimer();

    while(1) {}
    return 0 ;
}

void confGPIO(void) {
	// Pin P0.22 como salida
	LPC_GPIO0->FIODIR |= (1<<22);
	return;
}

void confTimer(void) {
	// Alimentamos el Timer0
	LPC_SC->PCONP |= (1<<1);
	// Clock del TIM0 = cclk/2
	LPC_SC->PCLKSEL0 |= (2<<2);
	// Cargamos el Prescaler
	LPC_TIM0->PR = 2;
	// Cargamos el Match0
	LPC_TIM0->MR0 = 6;
	// TC reinicia con el match e interrumpe
	LPC_TIM0->MCR |= (3<<0);
	// Preparamos el Timer
	LPC_TIM0->TCR = 3;
	// Y lo habilitamos
	LPC_TIM0->TCR &= ~(1<<1);
	// Habilitamos la interrupcion
	NVIC_EnableIRQ(TIMER0_IRQn);
	return;
}

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
