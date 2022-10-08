/*
===============================================================================
 Name        : EjTimerCapture1_Genero.c
 Author      : Bruno A. Genero
 Description : Utilizando el modo Capture, escribir un código en C  para que guarde en una
 variable ,llamada "shooter", el tiempo (en milisegundos) que le lleva a una persona presionar
 dos pulsadores con un único dedo.
===============================================================================
*/

#include "LPC17xx.h"

/*
 * Header files. Include Library
 */
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_timer.h"

/*
 * Function definitions
 */
void confPin(void);
void confTimer(void);

/*
 * Definitions and declarations
 */
#define PORT_ONE 		(uint8_t) 1
#define PIN_26 			((uint32_t)(1<<26))
#define PIN_27			((uint32_t)(1<<27))
#define PIN_OUT			(uint8_t) 1

uint32_t 				shooter;

int main(void) {

	confPin();
	confTimer();

    while(1) {}

    return 0 ;
}

void confPin(void) {
	PINSEL_CFG_Type pin_config;

	// Funcion CAP0.0 para el pin P1.26
	pin_config.Portnum = PINSEL_PORT_1;
	pin_config.Pinnum = PINSEL_PIN_26;
	pin_config.Funcnum = PINSEL_FUNC_3;		// CAP0.0
	pin_config.OpenDrain = PINSEL_PINMODE_NORMAL;

	PINSEL_ConfigPin(&pin_config);

	// Funcion CAP0.1 para el pin P1.27
	pin_config.Pinnum = PINSEL_PIN_27;

	PINSEL_ConfigPin(&pin_config);

	GPIO_SetDir(PORT_ONE,PIN_26,PIN_OUT);
	GPIO_SetDir(PORT_ONE,PIN_27,PIN_OUT);

	return;
}

void confTimer(void) {
	TIM_TIMERCFG_Type config;
	TIM_CAPTURECFG_Type capture;

	config.PrescaleOption = TIM_PRESCALE_USVAL;
	config.PrescaleValue = 1000;

	capture.CaptureChannel = 0;
	capture.FallingEdge = ENABLE;
	capture.IntOnCaption = ENABLE;

	TIM_Init(LPC_TIM0,TIM_TIMER_MODE,&config);
	// CAP0.0
	TIM_ConfigCapture(LPC_TIM0,&capture);

	// CAP0.1
	capture.CaptureChannel = 1;
	TIM_ConfigCapture(LPC_TIM0,&capture);

	NVIC_EnableIRQ(TIMER0_IRQn);

	TIM_Cmd(LPC_TIM0,ENABLE);
}

void TIMER0_IRQHandler(void) {
	// Variable auxiliares
	static uint8_t aux = 0;
	static uint8_t ready = 0;

	// Se presiona primero P1.26 y luego P1.27
	if(aux != 2) {
		if((TIM_GetIntCaptureStatus(LPC_TIM0,TIM_CR0_INT)) && !(TIM_GetIntCaptureStatus(LPC_TIM0,TIM_CR1_INT))){
			aux = 1;
		}
	}

	// Se presiono P1.26 y ahora se presiona P1.27
	if((aux == 1) && (TIM_GetIntCaptureStatus(LPC_TIM0,TIM_CR1_INT))) {
		ready = 1;
	}

	// Se presiona primero P1.27 y luego P1.26
	if(aux != 1) {
		if((TIM_GetIntCaptureStatus(LPC_TIM0,TIM_CR1_INT)) && !(TIM_GetIntCaptureStatus(LPC_TIM0,TIM_CR0_INT))){
			aux = 2;
		}
	}

	// Se presiono P1.27 y ahora se presiona P1.26
	if((aux == 2) && (TIM_GetIntCaptureStatus(LPC_TIM0,TIM_CR0_INT))) {
		ready = 1;
	}

	// Si se presionaron ambos pines se calcula el tiempo
	if(ready) {
		// Calculo el tiempo dependiendo del pin que se haya presionado primero
		switch(aux) {
		case 1:
			shooter = (TIM_GetCaptureValue(LPC_TIM0,TIM_COUNTER_INCAP1) - TIM_GetCaptureValue(LPC_TIM0,TIM_COUNTER_INCAP0)) * SystemCoreClock;
			break;
		case 2:
			shooter = (TIM_GetCaptureValue(LPC_TIM0,TIM_COUNTER_INCAP0) - TIM_GetCaptureValue(LPC_TIM0,TIM_COUNTER_INCAP1)) * SystemCoreClock;
			break;
		default:
			break;
		}
		// Reseteamos los auxiliares
		aux = 0;
		ready = 0;
	}

	// Limpiamos banderas
	TIM_ClearIntCapturePending(LPC_TIM0,TIM_CR0_INT);
	TIM_ClearIntCapturePending(LPC_TIM0,TIM_CR1_INT);

	return;
}




