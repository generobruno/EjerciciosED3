/*
===============================================================================
 Name        : InterruptGPIO_ejemplo.c
 Description : main definition
===============================================================================
*/

#include "LPC17xx.h"

void confGPIO(void); // Prototipo de la funci�n de conf. de puertos
void confIntGPIO(void); //Prototipo de la funci�n de conf. de interrupciones por GPIO
void retardo(uint32_t tiempo);

uint8_t inte = 0;

int main(void) {
	uint32_t tiempo;

	confGPIO();
	confIntGPIO();

	while(1){
		if(inte%2){
			tiempo = 1000000;
		}
		else {
			tiempo = 4000000;
		}

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
	LPC_GPIO0->FIODIR |= (1<<22);
	LPC_GPIO0->FIODIR &=~ (1<<15);
	return;
}
void confIntGPIO(void){
	LPC_GPIOINT->IO0IntEnR |= (1<<15); //Selecciona Interrupci�n por flanco de subida.
	LPC_GPIOINT->IO0IntClr |= (1<<15); //Limpia la bandera de interrupci�n
	NVIC_EnableIRQ(EINT3_IRQn);        // Habilita de interrupciones externas.
	return;
}
void EINT3_IRQHandler(void)
{
	if(LPC_GPIOINT->IO0IntStatR & (1<<15)){ //Pregunta si el pin P0.15 produjo la interrupci�n
		inte++;
		LPC_GPIOINT->IO0IntClr |= (1<<15); //limpia la bandera de interrupci�n
	}
	return;
}
