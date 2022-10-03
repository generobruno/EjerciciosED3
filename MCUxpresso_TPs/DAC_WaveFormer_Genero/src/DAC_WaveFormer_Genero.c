/*
===============================================================================
 Name        : DAC_WaveFormer_Genero.c
 Author      : Bruno A. Genero
 Main		 : Generamos una onda con el DAC que tenga una determinada amplitud
 y frecuencia.
===============================================================================
*/

#include "LPC17xx.h"

void configDAC(void);
void configTimer(void);

int main(void) {
	// Inicializamos variables
	uint8_t amplitudMax = 0x26C;	// Amplitud Max = 2 [V] -> VALUE = 620 = 0x26C
	uint8_t delta = 0x3E;			// Delta = 0.2 [V] -> VALUE = 62 = 0x3E
	uint8_t grad = 1;

	configDAC();
	configTimer();

	// Valor inicial de la onda
	((LPC_DAC->DACR<<6)&(0xFF)) = 1;

    while(1) {
    	// Vemos si el timer interrumpio
    	if((LPC_TIM0->IR) & (1<<0)) {
    		// Si el Valor del DAC es mayor a 2, decrementamos
    		if((LPC_DAC->DACR) >= (amplitudMax<<6)) {
    			grad = 0;
    		} else if((LPC_DAC->DACR) == 0){ // Si llegamos al minimo, incrementamos
    			grad = 1;
    		}

    		// Chequeamos grad para ver que hacer
    		if(grad){
    			((LPC_DAC->DACR<<6)&(0xFF)) += delta;
    		}else {
    			((LPC_DAC->DACR<<6))&(0xFF) -= delta;
    		}
    	}
    	// No es necesario un retardo en esta frecuencia.
    } // FIN DEL WHILE()

    return 0;
}

void configDAC(void) {
	// Configuramos el pin P0.26 para AOUT
	LPC_PINCON->PINSEL1 |= (2<<20);
	// Configuramos el pckl = cclk
	LPC_SC->PCLKSEL0 |= (1<<22);
}

void configTimer(void) {
	// Tim0 encendido por default, seleccionamos pclk = cclk
	LPC_SC->PCLKSEL0 |= (1<<2);
	// Seleccionamos funcion de MAT0.0 (toggle)
	LPC_TIM0->EMR |= (3<<6);
	// Cargamos el valor de match = 1ms (pclk = 100MHz)
	LPC_TIM0->MR0 = 0xF423F;
	// Match reset en 0
	LPC_TIM0->MCR |= (1<<1);
	// Habilito y reseteo el timer
	LPC_TIM0->TCR = 3;
	LPC_TIM0->TCR &= ~(1<<1);
}
