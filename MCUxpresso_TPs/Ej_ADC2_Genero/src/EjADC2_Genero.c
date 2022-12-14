/*
===============================================================================
 Name        : EjADC2_Genero.c
 Author      : Bruno A. Genero
 Description : Configurar 4 canales del ADC para que funcionando en modo burst
 se obtenga una frecuencia de muestreo en cada uno de 50Kmuestras/seg. Suponer un
 cclk = 100 Mhz.
===============================================================================
*/

#include "LPC17xx.h"

void confADC(void);

int main(void) {

	confADC();

    while(1) {
    }
    return 0 ;
}

void confADC(void) {
	// Alimentamos el ADC
	LPC_SC->PCONP |= (1<<12);
	// Habilitamos el ADC
	LPC_ADC->ADCR |= (1<<21);
	// Seleccionamos frec de trabajo (pclk) = cclk/4
	LPC_SC->PCLKSEL0 &= ~(3<<24);
	// Seleccionamos divisor del reloj ADC = f_T = pckl/2 -> CLKDIV = 1
	LPC_ADC->ADCR |= (1<<8);		// f_T = 12.5 MHz (cclk = 100MHz)
	// Seleccionamos los canales de conversion (ADC.0 a ADC.3)
	LPC_ADC->ADCR |= (15<<0);
	// Seleccionamos el modo burst
	LPC_ADC->ADCR |= (1<<16);
	// PinMode para los canales
	LPC_PINCON->PINMODE1 |= (0xAA<<14);
	// PinSel para los pines de los canales
	LPC_PINCON->PINSEL1 |= (0x55<<14);
	// Interrupciones para todos los pines
	//LPC_ADC->ADINTEN |= (15<<0);
	// ADGINTEN para el modo burst
	LPC_ADC->ADINTEN &= ~(1<<8);
	// Habilitamos interrupciones
	//NVIC_EnableIRQ(ADC_IRQn);
	return;
}
