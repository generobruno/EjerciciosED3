/*
===============================================================================
 Name        : EjTimerMatch3_Genero.c
 Author      : Bruno A. Genero
 Description : Escribir un programa para que por cada presión de un
 pulsador, la frecuencia de parpadeo de un led disminuya a la mitad
 debido a la modificación del pre-escaler del Timer 2. El pulsador debe
 producir una interrupción por EINT1 con flanco descendente. Adjuntar el
 código en C.
===============================================================================
*/

#include "LPC17xx.h"

void confTimer(void);
void confInt(void);

int main(void) {

	confTimer();
	confInt();

    while(1) {
    }
    return 0 ;
}

void confTimer(void) {
	// Alimentamos el Timer2
	LPC_SC->PCONP |= (1<<22);
	// Clock periferico = cclk
	LPC_SC->PCLKSEL1 |= (1<<12);
	// Pin P0.6 como MAT2
	LPC_PINCON->PINSEL0 |= (3<<12);
	// Funcion de toggle para el pin MAT2
	LPC_TIM2->EMR |= (3<<8);
	// Cargamos MR0 para 1 segundo (inicialmente)
	LPC_TIM2->MR0 = 0x5F5E0FF;
	// Hacemos que TC se resetee con el match
	LPC_TIM2->MCR |= (1<<1);
	// Configuramos TIM2 para no interrumpir
	LPC_TIM2->MCR &= ~(1<<0);
	// Preparamos el Counter
	LPC_TIM2->TCR = 3;
	// Y lo activamos
	LPC_TIM2->TCR &= ~(1<<1);
	return;
}

void confInt(void) {
	// Pin 2.11 como EINT1
	LPC_PINCON->PINSEL4 |= (1<<22);
	// Configuramos EINT1 por flanco descendente
	LPC_SC->EXTMODE |= (1<<1);
	LPC_SC->EXTPOLAR &= ~(1<<1);
	LPC_SC->EXTINT |= (1<<1);
	// Habilitamos las interrupciones
	NVIC_EnableIRQ(EINT1_IRQn);
	return;
}

void EINT1_IRQHandler(void) {
	// Variable de control
	static uint8_t control = 0;
	control++;

	// Aumentamos el prescaler
	LPC_TIM2->PR *= 2;

	// Chequeamos que la frecuencia no sea muy lenta
	if(control > 120) {
		// Reiniciamos el prescaler
		LPC_TIM2->PR = 0;
		// Reiniciamos el contador
		control = 0;
	}

	// Limpiamos la bandera
	LPC_SC->EXTINT |= (1<<1);
	return;
}
