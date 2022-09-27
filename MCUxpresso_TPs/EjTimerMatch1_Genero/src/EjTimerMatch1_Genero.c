/*
===============================================================================
 Name        : EjTimerMatch1_Genero.c
 Author      : Bruno A. Genero
 Description : Utilizando el Timer 1, escribir un código en C para que
 por cada presión de un pulsador, la frecuencia de parpadeo de un led
 disminuya a la mitad debido a la modificación del reloj que llega al
 periférico. El pulsador debe producir una interrupción por GPIO0 con
 flanco descendente. Adjuntar el código en C.
===============================================================================
*/

#include "LPC17xx.h"

void confGPIO(void);
void confTimer(void);
void confInt(void);

int main(void) {

	confGPIO();
	confTimer();
	confInt();

    while(1) {}

    return 0 ;
}

void confGPIO(void) {
	// Configuramos Pin P0.2 como entrada
	LPC_GPIO0->FIODIR0 &= ~(1<<2);
}

void confTimer(void) {
	// Alimentamos el Timer1
	LPC_SC->PCONP |= (1<<2);
	// Clock Periferico inicial = cclk
	LPC_SC->PCLKSEL0 |= (1<<4);
	// Pin P1.28 para funcion MAT1
	LPC_PINCON->PINSEL3 |= (3<<12);
	// Funcion toggle para el pin MAT1
	LPC_TIM1->EMR |= (3<<6);
	// Cargamos MR0 para 1 segundo (inicialmente)
	LPC_TIM1->MR0 = 0x5F5E0FF;
	// Hacemos que TC se resetee cada match
	LPC_TIM1->MCR |= (1<<1);
	// Configuramos TIM1 para no interrumpir
	LPC_TIM1->MCR &= ~(1<<0);
	// Preparamos el Counter
	LPC_TIM1->TCR = 3;
	// Y lo activamos
	LPC_TIM1->TCR &= ~(1<<1);
	return;
}

void confInt(void) {
	// Configuramos int Pin P0.2 por flanco descendente
	LPC_GPIOINT->IO0IntEnF |= (1<<2);
	// Limpiamos Banderas
	LPC_GPIOINT->IO0IntClr |= (1<<2);
	// Habilitamos la interrupcion
	NVIC_EnableIRQ(EINT3_IRQn);
	return;
}

void EINT3_IRQHandler(void) {
	static uint8_t control = 0;

	// Chequeamos la interrupcion
	if((LPC_GPIOINT->IO0IntStatF) & (1<<2)) {
		// Aumentamos el contador
		control++;

		// Chequeamos el contador
		if(control == 1){
			// pclk = cclk/2
			LPC_SC->PCLKSEL0 &= ~(1<<5);
			LPC_SC->PCLKSEL0 |= (2<<4);
		} else if(control == 2){
			// pclk = cclk/4
			LPC_SC->PCLKSEL0 &= ~(1<<4);
		} else if(control == 3){
			// pclk = cclk/8
			LPC_SC->PCLKSEL0 |= (3<<4);
		} else {
			// pckl = cclk (vuleve a empezar)
			LPC_SC->PCLKSEL0 &= ~(1<<4);
			// Reiniciamos el contador
			control = 0;
		}
	}

	// Limpiamos la bandera
	LPC_GPIOINT->IO0IntClr |= (1<<2);
	return;
} // Fin de la funcion
