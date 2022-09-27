/*
===============================================================================
 Name        : eje2Genero.c
 Author      : Bruno A. Genero
 Description : En los pines P2.0 a P2.7 se encuentra conectado un display
 de 7 segmentos. Utilizando la variable numDisplay
 [10] ={0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67}
 que codifica los números del 0 a 9 para ser mostrados en el display,
 realizar un programa que muestre indefinidamente la cuenta de 0 a 9
 en dicho display.
===============================================================================
*/

#include "LPC17xx.h"

void retardo(void);

int main(void){
	uint32_t numDisplay[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67};

	// Configuramos los pines P2.0 a P2.7 como salida
	LPC_GPIO2->FIODIR0 = 0xFF;

	// Muestro los digitos
	for(int i=0; i<sizeof(numDisplay); i++) {
		LPC_GPIO2->FIOSET0 = numDisplay[i];
		retardo();
		LPC_GPIO2->FIOCLR0 = numDisplay[i];
		retardo();
	}

	return 0;
}

/*
 * Funcion retardo
 * Genera un retardo
 */
void retardo(void) {
	uint32_t cont;
	for(cont = 0; cont < 6000000; cont++){};
}
