/*
===============================================================================
 Name        : GPIO_Entrada2_Genero.c
 Author      : Bruno A. Genero
 Description : Considerando pulsadores normalmente abiertos conectados
 en un extremo a masa y en el otro directamente a las entradas P0.0 y
 p0.1. Realizar un programa que identifique en una variable cual o cuales
 pulsadores han sido presionados. Las identificaciones posibles a
 implementar en esta variable van a ser "ninguno", "pulsador 1",
 "pulsador 2", "pulsador 1 y 2".
===============================================================================
*/

#include "LPC17xx.h"
#include <string.h>


int main(void) {
	char *pulsados = NULL;

	LPC_GPIO0->FIODIR0 &= ~(3<<0);	// Pines P0.0 y P0.1 como entrada digital
	// Los pines P0.0 y P0.1 tienen pull-ups habilitadas en reset

	// Enmascaro el resto de los pines
	LPC_GPIO0->FIOMASK0 = 0xFC;

    while(1) {
    	switch(LPC_GPIO0->FIOPIN0) {
    	case 0:
    		pulsados = "Ninguno";
    		break;
    	case 1:
    		pulsados = "Pulsador 1";
    		break;
    	case 2:
    		pulsados = "Pulsador 2";
    		break;
    	case 3:
    		pulsados = "Ambos";
    		break;
    	}

    }
    return 0 ;
}
