/*
===============================================================================
 Name        : EjTimerMatch5_D_Genero.c
 Author      : Bruno A. Genero
 Description : main definition
===============================================================================
*/

#include "lpc17xx_timer.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_exti.h"

#define PORT_ZERO 0
#define PIN_0 (uint32_t)(1<<0)
#define PIN_1 (uint32_t)(1<<1)
#define PIN_2 (uint32_t)(1<<2)
#define PIN_3 (uint32_t)(1<<3)
#define OUTPUT 1

void confTimer(void);
void confPin(void);
void confInt(void);
void parMenor(void);
void parMayor(void);

uint8_t control = 0;

int main(void) {

	confPin();
	confTimer();
	confInt();

    while(1) {}

    return 0 ;
}

void confPin(void) {
	// Pine P0.0 a P0.3 como salida
	GPIO_SetDir(PORT_ZERO,PIN_0,OUTPUT);
	GPIO_SetDir(PORT_ZERO,PIN_1,OUTPUT);
	GPIO_SetDir(PORT_ZERO,PIN_2,OUTPUT);
	GPIO_SetDir(PORT_ZERO,PIN_3,OUTPUT);

	// Pin P2.13 como EINT3
	PINSEL_CFG_Type pin_config;

	pin_config.Portnum = PINSEL_PORT_2;
	pin_config.Pinnum = PINSEL_PIN_13;
	pin_config.Funcnum = PINSEL_FUNC_1;
	pin_config.Pinmode = PINSEL_PINMODE_PULLUP;
	pin_config.OpenDrain = PINSEL_PINMODE_NORMAL;

	PINSEL_ConfigPin(&pin_config);
}

void confTimer(void) {
	TIM_TIMERCFG_Type timer_config;

	// Configuramos timer
	timer_config.PrescaleOption = TIM_PRESCALE_TICKVAL;
	timer_config.PrescaleValue = 0;

	// Configuramos los match
	TIM_MATCHCFG_Type match_config;

	// Configuramos match 0
	match_config.MatchChannel = 0;
	match_config.IntOnMatch = ENABLE;
	match_config.ResetOnMatch = ENABLE;
	match_config.StopOnMatch = ENABLE;
	match_config.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
	match_config.MatchValue = 0x7A11F; // 5ms

	TIM_ConfigMatch(LPC_TIM0,&match_config);

	// Configuramos match 1
	match_config.MatchChannel = 1;
	match_config.MatchValue = 0xF423F;

	TIM_ConfigMatch(LPC_TIM0,&match_config);

	// Configuramos match 2
	match_config.MatchChannel = 2;
	match_config.MatchValue = 0x16E35F;

	TIM_ConfigMatch(LPC_TIM0,&match_config);

	// Configuramos match 3
	match_config.MatchChannel = 1;
	match_config.MatchValue = 0x1E847F;

	TIM_ConfigMatch(LPC_TIM0,&match_config);

	TIM_Init(LPC_TIM0,TIM_TIMER_MODE,&timer_config);
	TIM_Cmd(LPC_TIM0,ENABLE);
}

void confInt(void) {
	// EINT 3 por flanco de bajada
	EXTI_InitTypeDef exti_config;

	exti_config.EXTI_Line = EXTI_EINT3;
	exti_config.EXTI_Mode = EXTI_MODE_EDGE_SENSITIVE;
	exti_config.EXTI_polarity = EXTI_POLARITY_LOW_ACTIVE_OR_FALLING_EDGE;

	EXTI_Config(&exti_config);
	EXTI_Init();

	// Seteamos prioridades
	NVIC_SetPriority(EINT3_IRQn,6);
	NVIC_SetPriority(TIMER0_IRQn,5);

	// Habilitamos interrupciones
	NVIC_EnableIRQ(EINT3_IRQn);
	NVIC_EnableIRQ(TIMER0_IRQn);
}

void EINT3_IRQHanlder(void) {
	// Desactivamos el timer para reconfigurar
	TIM_Cmd(LPC_TIM0,DISABLE);

	// Cambiamos el modo
	if(!control){
		// ParMayor
		control = 1;
	} else {
		// ParMenor
		control = 0;
	}

	// Limpiamos banderas
	EXTI_ClearEXTIFlag(EXTI_EINT3);
	// Volvemos a habilitar el timer
	TIM_Cmd(LPC_TIM0,ENABLE);

	return;
}

void parMenor(void) {
	// Interrupcion por match 0
	if(TIM_GetIntStatus(LPC_TIM0,TIM_MR0_INT)) {
		GPIO_SetValue(PORT_ZERO,PIN_0);
		// Limpiamos bandera
		TIM_ClearIntPending(LPC_TIM0,TIM_MR0_INT);
	} else {
		GPIO_ClearValue(PORT_ZERO,PIN_0);
	}

	// Interrupcion por match 1
	if(TIM_GetIntStatus(LPC_TIM0,TIM_MR1_INT)) {
		GPIO_SetValue(PORT_ZERO,PIN_1);
		// Limpiamos bandera
		TIM_ClearIntPending(LPC_TIM0,TIM_MR1_INT);
	} else {
		GPIO_ClearValue(PORT_ZERO,PIN_1);
	}

	// Interrupcion por match 2
	if(TIM_GetIntStatus(LPC_TIM0,TIM_MR2_INT)) {
		GPIO_SetValue(PORT_ZERO,PIN_2);
		// Limpiamos bandera
		TIM_ClearIntPending(LPC_TIM0,TIM_MR2_INT);
	} else {
		GPIO_ClearValue(PORT_ZERO,PIN_2);
	}

	// Interrupcion por match 3
	if(TIM_GetIntStatus(LPC_TIM0,TIM_MR3_INT)) {
		GPIO_SetValue(PORT_ZERO,PIN_3);

		// Reiniciamos el contador
		TIM_ResetCounter(LPC_TIM0);

		// Limpiamos bandera
		TIM_ClearIntPending(LPC_TIM0,TIM_MR3_INT);
	} else {
		GPIO_ClearValue(PORT_ZERO,PIN_3);
	}

	return;

}

void parMayor(void) {
	// Interrupcion por match 0
	if(TIM_GetIntStatus(LPC_TIM0,TIM_MR0_INT)) {
		GPIO_SetValue(PORT_ZERO,PIN_0);

		GPIO_ClearValue(PORT_ZERO,PIN_2);

		// Limpiamos bandera
		TIM_ClearIntPending(LPC_TIM0,TIM_MR0_INT);
	}

	// Interrupcion por match 1
	if(TIM_GetIntStatus(LPC_TIM0,TIM_MR1_INT)) {
		GPIO_SetValue(PORT_ZERO,PIN_1);

		GPIO_ClearValue(PORT_ZERO,PIN_3);

		// Limpiamos bandera
		TIM_ClearIntPending(LPC_TIM0,TIM_MR1_INT);
	}

	// Interrupcion por match 2
	if(TIM_GetIntStatus(LPC_TIM0,TIM_MR2_INT)) {
		GPIO_SetValue(PORT_ZERO,PIN_2);

		GPIO_ClearValue(PORT_ZERO,PIN_0);

		// Limpiamos bandera
		TIM_ClearIntPending(LPC_TIM0,TIM_MR2_INT);
	}

	// Interrupcion por match 3
	if(TIM_GetIntStatus(LPC_TIM0,TIM_MR3_INT)) {
		GPIO_SetValue(PORT_ZERO,PIN_3);

		GPIO_ClearValue(PORT_ZERO,PIN_1);

		// Reiniciamos el contador
		TIM_ResetCounter(LPC_TIM0);

		// Limpiamos bandera
		TIM_ClearIntPending(LPC_TIM0,TIM_MR3_INT);
	}

	return;
}

void TIMER0_IRQHandler(void) {
	if(!control){
		parMenor();
	} else {
		parMayor();
	}
}










