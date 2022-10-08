/*
===============================================================================
 Name        : EjTimerMatch1_Genero.c
 Author      : Bruno A. Genero
 Description : Utilizando el Timer 1, escribir un código en C para que
 por cada presión de un pulsador, la frecuencia de parpadeo de un led
 disminuya a la mitad debido a la modificación del reloj que llega al
 periférico. El pulsador debe producir una interrupción por GPIO0 con
 flanco descendente. Adjuntar el código en C.
===============================================================================
*/

#include "LPC17xx.h"

/*
 * Header Files. Include Library
 */
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_timer.h"

/*
 * Funcion Definitions
 */
void confPin(void);
void confTimer(void);

/*
 * Definitions and declarations
 */
#define PORT_ZERO 	(uint8_t) 0
#define PORT_ONE	(uint8_t) 1
#define PIN_2		((uint32_t)(1<<2))
#define PIN_28		((uint32_t)(1<<28))
#define PIN_IN		(uint8_t) 0
#define F_EDGE		(uint8_t) 1

int main(void) {

	confPin();
	confTimer();

    while(1) {}

    return 0 ;
}

void confPin(void) {
	PINSEL_CFG_Type pin_config;

	pin_config.Portnum = PINSEL_PORT_1;
	pin_config.Pinnum = PINSEL_PIN_28;
	pin_config.Funcnum = PINSEL_FUNC_3;

	// Pin P1.28 con funcion MAT1
	PINSEL_ConfigPin(&pin_config);

	// Pin P0.2 como salida
	GPIO_SetDir(PORT_ZERO,PIN_2,PIN_IN);

	// Interrupción por Pin P0.2
	GPIO_IntCmd(PORT_ZERO,PIN_2,F_EDGE);
	GPIO_ClearInt(PORT_ZERO,PIN_2);
	NVIC_EnableIRQ(EINT3_IRQn);

	return;
}

void confTimer(void) {
	TIM_TIMERCFG_Type config;
	TIM_MATCHCFG_Type match;

	config.PrescaleOption = TIM_PRESCALE_USVAL;
	config.PrescaleValue = (1/100);

	match.MatchChannel = 0;
	match.ExtMatchOutputType = TIM_EXTMATCH_TOGGLE;
	match.MatchValue = 0x5F5E0FF;	// 1 segundo
	match.ResetOnMatch = ENABLE;
	match.IntOnMatch = DISABLE;

	TIM_Init(LPC_TIM1,TIM_TIMER_MODE,&config);
	TIM_ConfigMatch(LPC_TIM1,&match);

	TIM_Cmd(LPC_TIM1,ENABLE);

	return;
}

void EINT3_IRQHandler(void) {
	TIM_TIMERCFG_Type reconfig;
	static uint8_t control = 0;

	//Chequeamos la interrupcion
	if(GPIO_GetIntStatus(PORT_ZERO,PIN_2,F_EDGE)) {
		// Aumentamos el contador
		control++;
		// Detenemos el timer
		TIM_Cmd(LPC_TIM1,DISABLE);

		// Chequeamos el contador
		if(control == 1) {
			// pclk = cclk/2
			reconfig.PrescaleValue = (2/100);
		} else if(control == 2) {
			// pckl = cclk/4
			reconfig.PrescaleValue = (4/100);
		} else if(control == 3) {
			// pclk = cclk/8
			reconfig.PrescaleValue = (8/100);
		} else {
			// pclk = cclk (vuelve a empezar)
			reconfig.PrescaleValue = (1/100);
			control = 0;
		}
	}

	// Limpiamos banderas
	GPIO_ClearInt(PORT_ZERO,PIN_2);
	// Reconfiguramos el timer
	TIM_Init(LPC_TIM1,TIM_TIMER_MODE,&reconfig);
	// Volvemos a activar el timer
	TIM_Cmd(LPC_TIM1,ENABLE);
	return;
}






