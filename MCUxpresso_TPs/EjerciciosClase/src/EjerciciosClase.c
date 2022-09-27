/*
===============================================================================
 Name        : InterruptExt_ejemplo.c
 Description : main definition
===============================================================================
*/

#include "LPC17xx.h"

void confGPIO(void); // Prototipo de la funcion de conf. de puertos
void confIntExt(void); //Prototipo de la funci�n de conf. de interrupciones externas
void EINT0_IRQHandler(void); //Funci�n de interrupci�n EINT0
void retardo(uint32_t tiempo);

uint32_t tiempo;

int main(void) {

	confGPIO();
	confIntExt();

	while(1){
		LPC_GPIO0->FIOSET = (1<<22);
		retardo(tiempo);
		LPC_GPIO0->FIOCLR = (1<<22);
		retardo(tiempo);
	}
	return 0;
}

void retardo (uint32_t tiempo){
	for(uint32_t conta = 0; conta<tiempo;conta++){}
	return;
}

void confGPIO(void){
	LPC_GPIO0->FIODIR |= (1<<22); // Pin P0.22 como salida
	return;
}
void confIntExt(void){
	LPC_PINCON->PINSEL4 |= (1<<20); //Selecciona funcionamiento del pin P2.10 como INT0
	LPC_SC->EXTMODE     |= 1; //Selecciona interrupcion por flanco
	LPC_SC->EXTPOLAR    |= 1; //Interrumpe cuando el flanco es de subida
	LPC_SC->EXTINT      |= 1;   //Limpia bandera de interrupci�n
	NVIC_EnableIRQ(EINT0_IRQn);    // Habilita de interrupciones externas.
	return;
}
void EINT0_IRQHandler(void)
{
	static uint8_t inte = 0;
	// En este caso no es necesario identificar quien interrumpe
	inte++;
	if(inte%2){
		tiempo = 1000000;
	}
	else {
		tiempo = 4000000;
	}
	LPC_SC->EXTINT |= 1;   //Limpia bandera de interrupci�n
	return;
}
