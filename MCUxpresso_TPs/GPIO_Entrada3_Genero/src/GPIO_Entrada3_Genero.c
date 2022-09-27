/*
===============================================================================
 Name        : GPIO_Entrada3_Genero.c
 Author      : Bruno A. Genero
 Description : Escribir un programa en C que permita realizar un promedio
 movil con los últimos 8 datos ingresados por el puerto 1. Considerar que
 cada dato es un entero signado y está formado por los 16 bits menos
 significativos de dicho puerto. El resultado, también de 16 bits, debe
 ser sacado por los pines P0.0 al P0.11 y P0.15 al P0.18.
 Recordar que en un promedio movil primero se descarta el dato mas viejo
 de los 8 datos guardados, se ingresa un nuevo dato proveniente del
 puerto y se realiza la nueva operación de promedio con esos 8 datos
 disponibles, así sucesivamente. Considerar el uso de un retardo antes
 de tomar una nueva muestra por el puerto.
===============================================================================
*/

#include "LPC17xx.h"

void configGPIO(void);
void retardo(void);

int main(void) {

	int8_t datos[8];	// Array con los datos para el promedio
	int8_t suma = 0;	// Suma de los datos
	int8_t prom = 0;	// Promedio de los datos

	configGPIO();

	// Cargamos los primero datos
	for(uint8_t i = 0; i < 8; i++) {
		datos[i] = LPC_GPIO1->FIOPINL;
		suma += datos[i];
		retardo();
	}

    while(1) {
    	// Calculo del promedio
    	for(uint8_t i = 0; i < 8; i++) {
    		prom = (suma/8);

    		// Enmascaramos pines
    		LPC_GPIO0->FIOMASK = ~(0x00078FFF);
    		// Mostramos el resultado
    		LPC_GPIO0->FIOPIN = ((prom) & (0x00078FFF));

    		retardo();

    		// Se descarta el dato mas viejo
    		suma -= datos[i];
    		// Se carga un dato nuevo
    		datos[i] = LPC_GPIO1->FIOPINL;
    		// Se suma este dato
    		suma += datos[i];
    	}

    }
    return 0 ;
}

void configGPIO(void) {
	// Pines P1.0 a P1.15 como entrada
	LPC_GPIO1->FIODIRL = 0x00;
	// Pines P0.0 a P0.11 y P0.15 a P0.18 como salida
	LPC_GPIO0->FIODIR = 0x00078FFF;
}

void retardo(void) {
	for(uint32_t i = 0; i < 6000000; i++){}
}



