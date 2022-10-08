/*
===============================================================================
 Name        : EjTimerMatch3_Genero.c
 Author      : Bruno A. Genero
 Description : Escribir un programa para que por cada presión de un
 pulsador, la frecuencia de parpadeo de un led disminuya a la mitad
 debido a la modificación del pre-escaler del Timer 2. El pulsador debe
 producir una interrupción por EINT1 con flanco descendente. Adjuntar el
 código en C.
===============================================================================
*/

#include "LPC17xx.h"

/*
 * Header Files. Include Library
 */
#include "lpc17xx_pinsel.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_timer.h"

/*
 * Function Definitions
 */
void confPin(void);
void confTimer(void);
void confInt(void);

/*
 * Declarations and definitions
 */


int main(void) {

	confPin();
	confTimer();
	confInt();

    while(1) {}

    return 0 ;
}

void confPin(void) {
	PINSEL_CFG_Type pin_config;

	// Pin P0.6 como MAT2
	pin_config.Portnum = PINSEL_PORT_0;
	pin_config.Pinnum = PINSEL_PIN_6;
	pin_config.Funcnum = PINSEL_FUNC_3;

	PINSEL_ConfigPin(&pin_config);

	// Pin 2.11 como EINT1
	pin_config.Portnum = PINSEL_PORT_2;
	pin_config.Pinnum = PINSEL_PIN_11;
	pin_config.Funcnum = PINSEL_FUNC_1;

	PINSEL_ConfigPin(&pin_config);

	return;
}

void confTimer(void) {
	TIM_TIMERCFG_Type config;
	TIM_MATCHCFG_Type match;

	config.PrescaleOption = TIM_PRESCALE_USVAL;
	config.PrescaleValue = (1/100);

	match.MatchChannel = 0;
	match.MatchValue = 0x5F5E0FF;		// 1 segundo
	match.ResetOnMatch = ENABLE;
	match.ExtMatchOutputType = TIM_EXTMATCH_TOGGLE;

	TIM_Init(LPC_TIM2,TIM_TIMER_MODE,&config);
	TIM_ConfigMatch(LPC_TIM2,&match);

	TIM_Cmd(LPC_TIM2,ENABLE);
	return;
}

void confInt(void) {
	EXTI_InitTypeDef exti_config;

	exti_config.EXTI_Line = EXTI_EINT1;
	exti_config.EXTI_Mode = EXTI_MODE_EDGE_SENSITIVE;
	exti_config.EXTI_polarity = EXTI_POLARITY_LOW_ACTIVE_OR_FALLING_EDGE;

	EXTI_Config(&exti_config);
	EXTI_Init();
	EXTI_ClearEXTIFlag(EXTI_EINT1);

	NVIC_EnableIRQ(EINT1_IRQn);
	return;
}

void EINT1_IRQHandler(void) {
	// Detenemos el Timer
	TIM_Cmd(LPC_TIM2,DISABLE);
	//TIM_DeInit(LPC_TIM2);

	TIM_TIMERCFG_Type reconfig;
	reconfig.PrescaleOption = TIM_PRESCALE_USVAL;

	// Variable de control
	static uint8_t control = 0;
	control++;

	// Aumentamos el prescaler
	reconfig.PrescaleValue *= 2;

	// Chequeamos que la frec no sea muy lenta
	if(control > 120) {
		// Reiniciamos el prescaler y la variable
		reconfig.PrescaleValue = (1/100);
	}

	// Limpiamos la bander
	EXTI_ClearEXTIFlag(EXTI_EINT1);

	// Reiniciamos el timer
	TIM_Init(LPC_TIM2,TIM_TIMER_MODE,&reconfig);
	TIM_Cmd(LPC_TIM2,ENABLE);
	return;
}






