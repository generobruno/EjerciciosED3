/*
===============================================================================
 Name        : EjemploADC_Driver.c
 Author      : Bruno A. Genero
===============================================================================
*/

#include "lpc17xx_adc.h"
#include "lpc17xx_pinsel.h"

#define _ADC_INT		ADC_ADINTEN2
#define _ADC_CHANNEL	ADC_CHANNEL_2

__IO uint32_t adc_value;

void ADC_IRQHandler(void);
void configPin(void);
void configADC(void);


int main(void) {
	uint32_t tmp;

	configPin();
	configADC();

	while(1) {
		// Start conversion
		ADC_StartCmd(LPC_ADC, ADC_START_NOW);
		// Enable ADC in NVIC
		NVIC_EnableIRQ(ADC_IRQn);
		// Retardo
		for(tmp = 0; tmp < 1000000; tmp++);
	}

	return 0;
}

void configPin(void) {
	PINSEL_CFG_Type PinCfg;

	PinCfg.Funcnum 		=		PINSEL_FUNC_1;
	PinCfg.OpenDrain 	=		PINSEL_PINMODE_NORMAL;
	PinCfg.Pinmode 		=		PINSEL_PINMODE_TRISTATE;
	PinCfg.Pinnum 		=		25;
	PinCfg.Portnum 		=		0;

	PINSEL_ConfigPin(&PinCfg);

	return;
}

void configADC(void) {
	ADC_Init(LPC_ADC,200000);						// ADC con f_muestreo = 200 KHz
	ADC_IntConfig(LPC_ADC,_ADC_INT,ENABLE);			// Habilitamos interrupciones
	ADC_ChannelCmd(LPC_ADC,_ADC_CHANNEL,ENABLE);	// Elegimos el canal 2 de ADC

	NVIC_SetPriority(ADC_IRQn, (9));

	return;
}

void ADC_IRQHanlder(void) {
	adc_value = 0;

	if(ADC_ChannelGetStatus(LPC_ADC,_ADC_CHANNEL,ADC_DATA_DONE)) {
		adc_value = ADC_ChannelGetData(LPC_ADC,_ADC_CHANNEL);
		NVIC_DisableIRQ(ADC_IRQn);	// Se deshabilita y se vuelve a habilitar despues del retardo
	}

}



