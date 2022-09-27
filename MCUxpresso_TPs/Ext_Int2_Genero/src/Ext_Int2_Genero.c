/*
===============================================================================
 Name        : Ext_Int2_Genero.c
 Author      : Bruno A. Genero
 Description : Escribir un programa en C que permita sacar por los pines
 P1.16 al P1.23 el equivalente en ascii de "a" si la interrupción se ha
 realizado por interrupción de EINT1 y no hay interrupción pendiente de
 EINT0. Para el caso en el cual se produzca una interrupción por EINT1 y
 exista una interrupción pendiente de EINT0 sacar por el  puerto el
 equivalente en ascii de "A". La interrupción de EINT1 es por el flanco
 de subida producido por un pulsador identificado como "tecla a/A",
 mientras que la interrupción EINT0 es por el nivel alto de un pulsador
 etiquetado como "Activación de Mayusculas".

Nota: Valerse de la configuración de los niveles de prioridad para que
la pulsación conjunta de "Activación de Mayúsculas" con "tecla a/A" de
como resultado a la salida el equivalente en ascii "A".
===============================================================================
*/

#include "LPC17xx.h"

void configGPIO(void);
void configIntExt(void);

// Variables Globales
uint8_t tecla = 0;

int main(void) {

	configGPIO();
	configIntExt();

	while(1) {
		// Sacamos el valor de tecla por los pines
		LPC_GPIO1->FIOPIN2 = tecla;
	}

    return 0 ;
}

void configGPIO() {
	// Pines P1.16 a P1.23 como salida
	LPC_GPIO1->FIODIR2 |= (0xFF<<0);
	return;
}

void configIntExt() {
	// Pines 2.10 y 2.11 como EINT0 y EINT1
	LPC_PINCON->PINSEL4 |= (5<<20);
	// Int EINT0 por nivel alto e Int EINT1 por flanco de subida
	LPC_SC->EXTMODE |= (1<<0);
	LPC_SC->EXTPOLAR |= (3<<0);
	LPC_SC->EXTINT |= (3<<0);
	// Habilitamos las interrupciones
	NVIC_EnableIRQ(EINT0_IRQn);
	NVIC_EnableIRQ(EINT1_IRQn);
	return;
}

// Pin 2.10 = Activacion de Mayusculas
void EINT0_IRQHanlder() {}

// Pin 2.11 = Tecla a/A
void EINT1_IRQHandler() {
	if(!(NVIC_GetPendingIRQ(EINT0_IRQn))) {
	// Interrumpimos por EINT1 y no hay Int pend de EINT0
		tecla = 0x61; // "a" = 97d = 61hex
	} else {
		tecla = 0x41; // "A" = 65d = 41hex
	}
}





