/*
===============================================================================
 Name        : EjTimerMatch2_Genero.c
 Author      : Bruno A. Genero
 Description : Escribir el código que configure el timer0 para cumplir
 con las especificaciones dadas en la figura adjunta. (Pag 510 Figura 115
 del manual de usuario del LPC 1769). Considerar una frecuencia de cclk
 de 100 Mhz y una división de reloj de periférico de 2.
===============================================================================
*/

#include "LPC17xx.h"

/*
 * Headers files. Include Library
 */
#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"

/*
 * Function Definitions
 */
void confGPIO(void);
void confTimer(void);

/*
 * Definitions and declarations
 */
#define PORT_ZERO 		(uint8_t) 0
#define PIN_22			((uint32_t)(1<<22))
#define PIN_OUT			(uint8_t) 1

int main(void) {

	confGPIO();
	confTimer();

    while(1) {}

    return 0 ;
}

void confGPIO(void) {
	GPIO_SetDir(PORT_ZERO,PIN_22,PIN_OUT);
	return;
}

void confTimer(void) {
	TIM_TIMERCFG_Type config;
	TIM_MATCHCFG_Type match;

	config.PrescaleOption = TIM_PRESCALE_USVAL;
	config.PrescaleValue = (6/100);

	match.MatchChannel = 0;
	match.MatchValue = 6;
	match.ResetOnMatch = ENABLE;
	match.IntOnMatch = ENABLE;

	TIM_Init(LPC_TIM0,TIM_TIMER_MODE,&config);
	TIM_ConfigMatch(LPC_TIM0,&match);

	TIM_Cmd(LPC_TIM0,ENABLE);
	NVIC_EnableIRQ(TIMER0_IRQn);
	return;
}

void TIMER0_IRQHandler(void) {
	static uint8_t i = 0;

	if(i == 0) {
		GPIO_SetValue(PORT_ZERO,PIN_22);
		i = 1;
	} else {
		GPIO_ClearValue(PORT_ZERO,PIN_22);
		i = 0;
	}
	// Limpiamos banderas
	TIM_ClearIntPending(LPC_TIM0,TIM_MR0_INT);
}






