/*
 * Estudiante: Bruno A. Genero
 * Carrera: Ing en Comp.
 * Descripci�n: Este programa pende y apaga un led conectado al pin P0.22 de forma intermitente
	Ademas el tiempo de retardo es controlado por una se�al digital proveniente del
	pin P2.10 configurado como entrada.
*/

#include "LPC17xx.h"

void retardo(uint32_t tiempo);

int main(void){
	uint32_t tiempo;

	LPC_GPIO0->FIODIR    |= (1<<22);
	LPC_GPIO2->FIODIR    &= ~(1<<10); //Configura el pin P2.10 como entrada
	LPC_PINCON->PINMODE0 |= (3<<20);  //Asociar resistencia de Pull-Down al pin P2.10

	while(1){
		if ((LPC_GPIO2->FIOPIN)&(1<<10)){ //010101...1...0110 & 0000...1...00000 = 0000 ... 1..0000
			tiempo = 1000000;
		}
		else{
			tiempo = 4000000;
		}
		LPC_GPIO0->FIOCLR |= (1<<22); //Apaga el led
		retardo(tiempo);
		LPC_GPIO0->FIOSET |= (1<<22); //Prende el led
		retardo(tiempo);
	}
	return(0);
}

void retardo(uint32_t tiempo){
	uint32_t contador;
	for(contador=0; contador<tiempo; contador++){};
}
