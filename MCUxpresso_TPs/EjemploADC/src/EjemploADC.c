/*
===============================================================================
 Name        : EjemploADC.c
 Author      : Bruno A. Genero
===============================================================================
*/

#include "LPC17xx.h"

void confGPIO(void);
void confADC(void);

uint16_t ADC0Value = 0;

int main(void) {

	confGPIO();
	confADC();

    while(1) {}

    return 0 ;
}

void confGPIO(void) {
	LPC_GPIO0->FIODIR |= (1<<22);
	return;
}

void confADC(void) {
	LPC_SC->PCONP |= (1<<12);
	LPC_ADC->ADCR |= (1<<21);			// Habilita el ADC
	LPC_SC->PCLKSEL0 |= (3<<24);		// cclk/8
	LPC_ADC->ADCR &= ~(255<<8);			// clkdiv (0+1)=1
	//LPC_ADC->ADCR |= (1<<0);			// channel
	LPC_ADC->ADCR |= (1<<16); 			// burst
	LPC_PINCON->PINMODE1 |= (1<<15);	// neither pull-up nor pull-down
	LPC_PINCON->PINSEL1 |= (1<<14);
	LPC_ADC->ADINTEN |= (1<<0);
	LPC_ADC->ADINTEN &= ~(1<<8);
	NVIC_EnableIRQ(ADC_IRQn);

	return;
}

void ADC_IRQHandler(void) {
	// Var aux para observar el valor del registro de la captura
	ADC0Value = ((LPC_ADC->ADDR0)>>4) & 0xFFF;

	if(ADC0Value < 2054) {
		LPC_GPIO0->FIOSET |= (1<<22);
	} else {
		LPC_GPIO0->FIOCLR |= (1<<22);
	}

	// LIMPIAR BANDERAS??

	return;
}


