/*
===============================================================================
 Name        : GPIO_Int2_Genero.c
 Author      : Bruno A. Genero
 Description : Realizar un programa que configure el puerto P0.0 y P2.0
 para que provoquen una interrupción por flanco de subida. Si la
 interrupción es por P0.0 guardar el valor binario 100111 en la
 variable "auxiliar", si es por P2.0 guardar el valor binario
 111001011010110.
===============================================================================
*/

#include "LPC17xx.h"

void configGPIO(void);
void configIntGPIO(void);

uint32_t auxiliar = 0;

int main(void) {

	configGPIO();
	configIntGPIO();

	while(1){

	}

    return 0 ;
}

void configGPIO(void) {
	// Configuramos pin P0.0 como entrada
	LPC_GPIO0->FIODIR0 &= ~(1<<0);
	// Configuramos pin P2.0 como entrada
	LPC_GPIO2->FIODIR0 &= ~(1<<0);
	return;
}

void configIntGPIO(void) {
	// Configuramos int por flanco de subida para P0.0 y P2.0
	LPC_GPIOINT->IO0IntEnR |= (1<<0);
	LPC_GPIOINT->IO2IntEnR |= (1<<0);
	// Limpiamos banderas
	LPC_GPIOINT->IO0IntClr |= (1<<0);
	LPC_GPIOINT->IO2IntClr |= (1<<0);
	// Habilitamos interrupciones
	NVIC_EnableIRQ(EINT3_IRQn);
	return;
}

void EINT3_IRQHandler(void) {
	// Analizamos quien interrumpio
	if((LPC_GPIOINT->IO0IntStatF) & (1<<0)) {
		// La int fue por P0.0
		auxiliar = 100111;
	} else {
		// La int fue por P2.0
		auxiliar = 111001;
	}
	return;
}


