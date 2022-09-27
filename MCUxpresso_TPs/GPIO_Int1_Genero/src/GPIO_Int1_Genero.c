/*
===============================================================================
 Name        : GPIO_Int1_Genero.c
 Author      : Bruno A. Genero
 Description : Realizar un programa que configure el puerto P2.0 y P2.1
 para que provoquen una interrupción por flanco de subida para el primer
 pin y por flanco de bajada para el segundo. Cuando la interrupción sea
 por P2.0 se enviará por el pin P0.0 la secuencia de bits 010011010. Si
 la interrupción es por P2.1 se enviará por el pin P0.1 la secuencia
 011100110. Las secuencias se envían únicamente cuando se produce una
 interrupción, en caso contrario la salida de los pines tienen valores 1
 lógicos. ¿que prioridad tienen configuradas por defecto estas
 interrupciones?
===============================================================================
*/

#include "LPC17xx.h"

void configGPIO(void);
void configIntGPIO(void);
void retardo(void);

int main(void) {

	configGPIO();
	configIntGPIO();

	while(1) {
		// P0.0 y P0.1 = 1
		LPC_GPIO0->FIOSET0 |= (3<<0);
	}

    return 0 ;
}

void configGPIO(void) {
	// Configuramos Pines P2.0 y P2.1 como entrada
	LPC_GPIO2->FIODIR0 &= ~(3<<0);
	// Configuramos Pines P0.0 y P0.1 como salida
	LPC_GPIO0->FIODIR0 |= (3<<0);
	return;
}

void configIntGPIO(void) {
	// Configuramos P2.0 para int por flanco de subida
	LPC_GPIOINT->IO2IntEnR |= (1<<0);
	// Configuramos P2.1 para int por flanco de bajada
	LPC_GPIOINT->IO2IntEnF |= (1<<1);
	// Limpiamos las banderas
	LPC_GPIOINT->IO2IntClr |= (3<<0);
	// Habilitamos las interrupciones
	NVIC_EnableIRQ(EINT3_IRQn);
	return;
}

void EINT3_IRQHandler(void) {
	// Arrays con secuencia de salida
	uint8_t secuencia0[9] = {0,1,0,0,1,1,0,1,0};
	uint8_t secuencia1[9] = {0,1,1,1,0,0,1,1,0};

	// Analizamos quien interrumpe
	if((LPC_GPIOINT->IO2IntStatR) & (1<<0)){
		// Int es por flanco de subida
		for(int i = 0; i < 9; i++) {
			LPC_GPIO0->FIOSET0 |= (secuencia0[i]<<0);
			retardo();
		}
	} else {
		// Int es por flanco de bajada
		for(int i = 0; i < 9; i++) {
			LPC_GPIO0->FIOSET0 = (secuencia1[i]<<1);
			retardo();
		}
	}

	return;
}

void retardo(void) {
	uint32_t cont;
	for(cont = 0; cont < 6000000; cont ++){}
	return;
}

