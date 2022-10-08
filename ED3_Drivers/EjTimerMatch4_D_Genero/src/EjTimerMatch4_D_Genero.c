/*
===============================================================================
 Name        : EjTimerMatch4_Genero.c
 Author      : Bruno A. Genero
 Description : Escribir un programa para que por cada presión de un
 pulsador, la frecuencia de parpadeo disminuya a la mitad debido a la
 modificación del registro del Match 0. El pulsador debe producir una
 interrupción por EINT2 con flanco descendente. Adjuntar el código en C.
===============================================================================
*/

#include "LPC17xx.h"

/*
 * Header Files. Include Library
 */
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_exti.h"

/*
 * Function Definitions
 */
void confPin(void);
void confTimer(void);
void confInt(void);

/*
 * Declarations and definitions
 */
#define PORT_ZERO		(uint8_t) 0
#define PIN_22			((uint32_t)(1<<22))
#define PIN_OUT			(uint8_t) 1
#define MATCH_0			(uint8_t) 0

int main(void) {

	confPin();
	confTimer();
	confInt();

    while(1) {}

    return 0 ;
}

void confPin(void) {
	PINSEL_CFG_Type pin_config;

	// Pin P1.28 como MAT0
	pin_config.Portnum = PINSEL_PORT_0;
	pin_config.Pinnum = PINSEL_PIN_22;
	pin_config.Funcnum = PINSEL_FUNC_3;

	PINSEL_ConfigPin(&pin_config);

	// Pin P2.12 como EINT2
	pin_config.Portnum = PINSEL_PORT_2;
	pin_config.Pinnum = PINSEL_PIN_12;
	pin_config.Funcnum = PINSEL_FUNC_1;

	PINSEL_ConfigPin(&pin_config);

	// Pin P0.22 como salida
	GPIO_SetDir(PORT_ZERO,PIN_22,PIN_OUT);
	return;
}

void confTimer(void) {
	TIM_TIMERCFG_Type config;
	TIM_MATCHCFG_Type match;

	config.PrescaleOption = TIM_PRESCALE_USVAL;
	config.PrescaleValue = (1/100);

	match.MatchChannel = 0;
	match.MatchValue = 0x5F5E0FF;
	match.ExtMatchOutputType = TIM_EXTMATCH_TOGGLE;
	match.ResetOnMatch = ENABLE;

	TIM_Init(LPC_TIM0,TIM_TIMER_MODE,&config);
	TIM_ConfigMatch(LPC_TIM0,&match);

	TIM_Cmd(LPC_TIM0,ENABLE);
	return;
}

void confInt(void) {
	EXTI_InitTypeDef exti_config;

	exti_config.EXTI_Line = EXTI_EINT2;
	exti_config.EXTI_Mode = EXTI_MODE_EDGE_SENSITIVE;
	exti_config.EXTI_polarity = EXTI_POLARITY_LOW_ACTIVE_OR_FALLING_EDGE;

	EXTI_Config(&exti_config);
	EXTI_ClearEXTIFlag(EXTI_EINT2);
	EXTI_Init();

	NVIC_EnableIRQ(EINT2_IRQn);
	return;
}

void EINT2_IRQHandler(void) {
	// Desactivamos el Timer para reconfigurarlo
	TIM_Cmd(LPC_TIM2,DISABLE);

	// Duplicamos el valor de MR0
	uint32_t newMatch = (LPC_TIM0->MR0)*2;
	TIM_UpdateMatchValue(LPC_TIM2,MATCH_0,newMatch);

	// Chequeamos que el valor no sea muy grande
	if((LPC_TIM0->MR0) > 0xFFFFFFFF) {
		// Reiniciamos el val inicial de MR0
		TIM_UpdateMatchValue(LPC_TIM2,MATCH_0,0x05F5E0FF);
	}

	// Limpiamos bandera de int
	EXTI_ClearEXTIFlag(EXTI_EINT2);
	// Volvemos a habilitar el
	TIM_Cmd(LPC_TIM2,ENABLE);
	return;
}




