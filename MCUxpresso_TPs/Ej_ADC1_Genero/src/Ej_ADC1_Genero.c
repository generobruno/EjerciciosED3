/*
===============================================================================
 Name        : Ej_ADC1_Genero.c
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
	LPC_ADC->ADCR |= (3<<0);			// channels ADC.0 y ADC.1
	LPC_ADC->ADCR |= (1<<16); 			// burst
	LPC_PINCON->PINMODE1 |= (10<<14);	// neither pull-up nor pull-down
	LPC_PINCON->PINSEL1 |= (5<<14);
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
	//LPC_ADC->ADSTAT |= (1<<16);

	return;
}

/**
 * RESPUESTAS:
 *
 * El ADC se encuentra trabajando en modo burst (LPC_ADC->ADCR |= (1<<16)).
 * Si suponemos el cclk de 100 MHz, en este caso el adc esta trabajando a una
 * frecuencia (f_T) de 12.5 MHz (LPC_SC->PCLKSEL0 |= (3<<24) => pclk = cclk/8 = 12.5 MHz).
 * En modo burst, el ADC necesita 64 ciclos de reloj para poder realizar una conversion,
 * por lo que su frecuencia de muestreo (f_m) se calcula de la siguiente manera:
 *
 * 			f_T / f_m = 64 => f_m = f_T / 64 = 195 [KHz]
 *
 * Si ahora modificamos el codigo para que se utilizren 2 canales de conversion
 * (LPC_ADC->ADCR |= (3<<0) => ADC.0 (P0.23) y ADC.1 (P0.24)), el calculo es el siguiente:
 *
 * 			f_T / f_m = 128 => f_m = f_T / 128 = 95.3 [KHz]
 *
 */



