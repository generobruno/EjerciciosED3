/*
===============================================================================
 Name        : SysTick_Ejemplo.c
 Author      : Bruno A. Genero
 Description : main definition
===============================================================================
*/

#include "LPC17xx.h"

void configGPIO(void);
uint8_t inte = 0;

int main(void) {

	configGPIO();
	// Config del intervalo de tiempo entre interrups del SysTick
	if(SysTick_Config(SystemCoreClock/10)) {
		while(1); // AVISO DE ERROR
		// El programa llega hasta aqui si la funcion devuelve un 1
	}
    while(1) {
    	if(inte%2) {
    		LPC_GPIO0->FIOSET = (1<<22);
    	} else {
    		LPC_GPIO0->FIOCLR = (1<<22);
    	}
    }

    return 0;
}

void configGPIO(void) {
	LPC_GPIO0->FIODIR |= (1<<22);
	return;
}

void SysTick_Handler(void) {
	inte++;
	SysTick->CTRL &= SysTick->CTRL;
	return;
}

