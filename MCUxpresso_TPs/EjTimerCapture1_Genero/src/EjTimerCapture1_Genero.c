/*
===============================================================================
 Name        : EjTimerCapture1_Genero.c
 Author      : Bruno A. Genero
 Description : Utilizando el modo Capture, escribir un código en C  para que guarde en una
 variable ,llamada "shooter", el tiempo (en milisegundos) que le lleva a una persona presionar
 dos pulsadores con un único dedo.
===============================================================================
*/

#include "LPC17xx.h"

void confTimer(void);

uint32_t shooter;

int main(void) {

	confTimer();

    while(1) {
    }
    return 0 ;
}

void confTimer(void) {
	// Timer 0 ya esta siendo alimentado
	// Seleccionamos pclk = cclk
	LPC_SC->PCLKSEL0 |= (1<<2);
	// Seleccionamos funciones CAP0.0 y CAP0.1 para pines P1.26 y P1.27
	LPC_PINCON->PINSEL3 |= (15<<20);
	// Guardamos los valores de TC en CAP0.0 y CAP0.1 cuando hay flanco de bajada
	LPC_TIM0->CCR |= (1<<1) | (1<<4);
	// Interrumpimos cuando se presiona el pin P1.26 o P1.27
	LPC_TIM0->CCR |= (1<<2) | (1<<5);
	// Habilitamos la interrupcion
	NVIC_EnableIRQ(TIMER0_IRQn);
	// Habilitamos el timer
	LPC_TIM0->TCR = 3;
	LPC_TIM0->TCR &= ~(1<<1);
	return;
}

/*
 * Siempre se presiona P1.26 antes que P1.27
 */
void TIMER0_IRQHandler(void) {
	// Variable auxiliar
	static uint8_t aux = 0;
	static uint8_t ready = 0;

	// Se presiona primero P1.26 y luego P1.27
	if(aux != 2){
		if( ((LPC_TIM0->IR) & (1<<4)) && ((LPC_TIM0->IR) & ~(1<<5)) ) {
			aux = 1;
		}
	}

	// Se presiono P1.26 y ahora se presiona P1.27
	if((aux == 1) && ((LPC_TIM0->IR) & (1<<5))){
		ready = 1;
	}

	// Se presiona primer P1.27 y luego P1.26
	if(aux != 1){
		if( ((LPC_TIM0->IR) & (1<<5)) && ((LPC_TIM0->IR) & ~(1<<4)) ) {
			aux = 2;
		}
	}

	// Se presiono P1.27 y ahora se presiona P1.26
	if((aux == 2) && ((LPC_TIM0->IR) & (1<<4))){
			ready = 1;
		}

	// Si se presionaron ambos pines se calcula el tiempo
	if(ready) {
		// Calculo del tiempo dependiendo que pin se presiono primero
		switch(aux) {
			case 1:
				shooter = ( ((LPC_TIM0->CR1) - (LPC_TIM0->CR0)) * SystemCoreClock );
				break;
			case 2:
				shooter = ( ((LPC_TIM0->CR0) - (LPC_TIM0->CR1)) * SystemCoreClock );
				break;
			default:
				break;
			}

		// Reseteamos auxiliares
		aux = 0;
		ready = 0;
	}

	// Limpiamos banderas
	LPC_TIM0->IR |= (1<<4) | (1<<5);

	return;

}
