/*
 *	Estudiante: Bruno A. Genero
 *	Carrera: Ing. en Comp.
 *	Com: 1
 */

#include "LPC17xx.h"

void retardo(uint32_t tiempo);
void confGPIO(void);
void confIntGPIO(void);
uint8_t inte;

int main(void) {
	//uint32_t clock = SystemCoreClock;
	uint32_t tiempo;

	confGPIO();
	confIntGPIO();

	while(1) {
		// Tenemos un tiempo de retardo dependiendo de si hubo
		// o no interrupcion por el puerto P0.15
		if(inte%2){
			tiempo = 4000000;
		} else {
			tiempo = 1000000;
		}

		LPC_GPIO0->FIOSET2 |=(1<<6);
		retardo(tiempo);
		LPC_GPIO0->FIOCLR2 |=(1<<6);
		retardo(tiempo);
	}

	return 0;
}

void EINT3_IRQHandler(void) {
	// Vemos si el pin P0.15 interrumpio por flanco de subida (Interrupt Status Rising)
	if(LPC_GPIOINT->IO0IntStatR & (1<<15)) {
		inte++;
	}
	LPC_GPIOINT->IO0IntClr |= (1<<15); // Limpiamos la bandera
}

void confGPIO(void) {
	//LPC_PINCON->PINSEL1 &=~(0b11<<12); // 00...010101 & 11...001111 = 00...(00)0101 -> Modifique lo que esta entre parentesis
	//LPC_PINCON->PINSEL4 &=~(3<<20);
	LPC_GPIO0->FIODIR2 |=(1<<6); // Pin P0.22 como salida
	LPC_GPIO0->FIODIR &=~(1<<15); // Pin P2.15 como entrada
	//LPC_PINCON->PINMODE4 |= (3<<20); // Resistencia de Pull-down para P2.10
	//LPC_GPIO0->FIOMASK2 &=~(1<<6);
}

/**
 * La interrupcion se genera por
 * el pin P0.15, flanco de subida
 */
void confIntGPIO(void) {
	// Se configuran las int -> se limian sus banderas -> se habilitan
	LPC_GPIOINT->IO0IntEnR |= (1<<15); // Selecciona int por flanco de subida
	LPC_GPIOINT->IO0IntClr |= (1<<15); // Limpiamos el flag
	NVIC_EnableIRQ(EINT3_IRQn); // Hab las int por P0 y P2, que van a el vector EINT3 (func de MCSYS)
	return;
}

void retardo(uint32_t tiempo) {
	uint32_t contador;

	for(contador = 0; contador < tiempo; contador++){};
}



