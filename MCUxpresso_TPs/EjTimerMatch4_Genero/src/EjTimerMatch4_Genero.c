/*
===============================================================================
 Name        : EjTimerMatch4_Genero.c
 Author      : Bruno A. Genero
 Description : Escribir un programa para que por cada presión de un
 pulsador, la frecuencia de parpadeo disminuya a la mitad debido a la
 modificación del registro del Match 0. El pulsador debe producir una
 interrupción por EINT2 con flanco descendente. Adjuntar el código en C.
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

    while(1) {
    }
    return 0 ;
}

void confGPIO(void) {
	// Configuramos el pin P0.22 como salida
	LPC_GPIO0->FIODIR |= (1<<22);
	return;
}

void confTimer(void) {
	// Alimentamos el Timer0
	LPC_SC->PCONP |= (1<<1);
	// Clock del Timer = cclk
	LPC_SC->PCLKSEL0 |= (1<<2);
	// Pin 1.28 para funcion MAT0
	LPC_PINCON->PINSEL3 |= (3<<24);
	// Funcion de toggle para el pin MAT0
	LPC_TIM0->EMR |= (3<<4);
	// Cargamos Match0 para 1 segundo (inicialmente)
	LPC_TIM0->MR0 = 0x5F5E0FF;
	// Hacemos que el TC se resetee cada match
	LPC_TIM0->MCR |= (1<<1);
	// Preparamos el counter
	LPC_TIM0->TCR = 3;
	// Y lo activamos
	LPC_TIM0->TCR &= ~(1<<1);
	return;
}

void confInt(void) {
	// Seleccionamos pin P2.12 para EINT2
	LPC_PINCON->PINSEL4 |= (1<<24);
	// Configuramos EINT2 para int por flanco descendente
	LPC_SC->EXTMODE |= (1<<2);
	LPC_SC->EXTPOLAR &= ~(1<<2);
	LPC_SC->EXTINT |= (1<<2);
	// Habilitamos la interrupcion
	NVIC_EnableIRQ(EINT2_IRQn);
	return;
}

void EINT2_IRQHandler(void) {
	// Duplicamos el valor cargado en MR0
	LPC_TIM0->MR0 *= 2;

	// Chequeamos que el valor no sea muy grande
	if((LPC_TIM0->MR0) > (0xFFFFFFFF)) {
		// Reiniciamos el MR0 a su valor inicial
		LPC_TIM0->MR0 = 0x5F5E0FF;
	}

	// Limpiamos la bandera
	LPC_SC->EXTINT |= (1<<2);
	return;
}



