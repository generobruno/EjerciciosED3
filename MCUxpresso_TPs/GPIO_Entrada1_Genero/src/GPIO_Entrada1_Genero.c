/*
===============================================================================
 Name        : GPIO_Entrada1_Genero.c
 Author      : $(author)
 Description : Configurar el pin P0.4 como entrada digital con resistencia
 de pull down y utilizarlo para decidir si el valor representado por los
 pines P0.0 al P0.3 van a ser sumados o restados al valor guardado en la
 variable "acumulador".El valor inicial de "acumulador" es 0.
===============================================================================
*/

#include "LPC17xx.h"


int main(void) {
	uint8_t acumulador = 0;

	LPC_GPIO0->FIODIR0 &= ~(1<<4);	// Pin P0.4 como entrada digital
	LPC_GPIO0->FIODIR0 |= (15<<0);	// Pin P0.0 a P0.3 como salida digital
	LPC_PINCON->PINMODE0 |= (3<<8);	// Pin P0.4 con pull-down

	while(1) {
		if((LPC_GPIO0->FIOPIN0) & (1<<4)) {
			LPC_GPIO0->FIOMASK0 = 0xF0;
			acumulador += LPC_GPIO0->FIOPIN0;
		} else {
			LPC_GPIO0->FIOMASK0 = 0xF0;
			acumulador -= LPC_GPIO0->FIOPIN0;
		}
	}


    return 0 ;
}
