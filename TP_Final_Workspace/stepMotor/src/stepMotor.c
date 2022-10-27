/*
===============================================================================
 Name        : motor_tp.c
 Description : codigo del motor
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
void motorOpen(void);
void motorClose(void);
void limpiarPines(void);

uint8_t control = 0;
uint8_t flag_timer = 0;

int main(void) {

	limpiarPines();
	confPin();
	confTimer();
	confInt();

    while(1) {}

    return 0 ;
}

/*
 * 		LOS CAMBIOS A HACER SON QUE SE DEBERIA ACTIVAR EL TIMER 1 Y 0 CUANDO UN
 * 		EMPLEADO INGRESA SU CODIGO CORRECTAMENTE (FUNCION enterEmployee) Y CERRARSE
 * 		CUANDO INTERRUMPA EL SENSOR.
 *
 */

void confPin(void) {
	// Pines P0.0 a P0.3 como salida
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
	TIM_MATCHCFG_Type match_config;

	/*
	 * 	Timer 0: Control de bobinas del motor
	 */

	// Configuramos timer
	timer_config.PrescaleOption = TIM_PRESCALE_USVAL;
	timer_config.PrescaleValue = 100;

	TIM_Init(LPC_TIM0,TIM_TIMER_MODE,&timer_config);

	// Configuramos match 0
	match_config.MatchChannel = 0;
	match_config.IntOnMatch = ENABLE;
	match_config.ResetOnMatch = ENABLE;
	match_config.StopOnMatch = DISABLE;
	match_config.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
	match_config.MatchValue = 5000000; // 5ms

	TIM_ConfigMatch(LPC_TIM0,&match_config);

	/*
	 * 	Timer 1: Control de apertura de la barrera
	 */

	// Configuramos timer
	timer_config.PrescaleOption = TIM_PRESCALE_USVAL;
	timer_config.PrescaleValue = 100000;

	TIM_Init(LPC_TIM1,TIM_TIMER_MODE,&timer_config);

	// Configuramos match 0
	match_config.MatchChannel = 0;
	match_config.StopOnMatch = ENABLE;
	match_config.MatchValue = 30;	// 3 segundos

	TIM_ConfigMatch(LPC_TIM0,&match_config);
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
	NVIC_SetPriority(TIMER1_IRQn,4);

	// Habilitamos interrupciones
	NVIC_EnableIRQ(EINT3_IRQn);
	NVIC_EnableIRQ(TIMER0_IRQn);
	NVIC_EnableIRQ(TIMER1_IRQn);
}

/*
 * 		EN EL PROYECTO PRINCIPAL, ESTA FUNCION DEBERIA EJECUTARSE CUANDO UN
 * 		EMPLEADO ENTRA CORRECTAMENTE SU CODIGO -> PONEMOS LA VARIABLE CONTROL EN 1.
 * 		LUEGO, CUANDO INTERRUMPE EL SENSOR POR DETECTAR QUE EL VEHICULO SE FUE,
 * 		VOLVEMOS A EJECUTAR ESTA FUNCION Y PONEMOS LA VARIABLE CONTROL EN 0.
 * 		TAMBIEN HAY QUE CAMBIAR EL NOMBRE DE LA FUNCION.
 * 		*DEBERIAMOS PASARLE A LA FUNCION UN PARAMETRO DE REFERENCIA, CUANDO LA USAMOS EN
 * 		enterEmployee HACE QUE CONTROL SEA = 1, Y CUANDO LA USAMOS EN LA INTERRUPCION DEL
 * 		SENSOR, HACE QUE CONTROL SEA = 0.
 */
void EINT3_IRQHanlder(void) {
	// Activamos Timer 0 para movilizar el motor
	TIM_Cmd(LPC_TIM0,ENABLE);
	// Activamos Timer 1 para temporizar el motor
	TIM_Cmd(LPC_TIM1,ENABLE);

	// Cambiamos el modo *
	if(!control){
		control = 1;	// motorOpen
	} else {
		control = 0;	// motorClose
	}

	// Desactivamos EINT3 hasta que termine el motor
	NVIC_DisableIRQ(EINT3_IRQn);
	// Limpiamos banderas
	EXTI_ClearEXTIFlag(EXTI_EINT3);

	return;
}

void limpiarPines(void){
	GPIO_ClearValue(PORT_ZERO,PIN_0);
	GPIO_ClearValue(PORT_ZERO,PIN_1);
	GPIO_ClearValue(PORT_ZERO,PIN_2);
	GPIO_ClearValue(PORT_ZERO,PIN_3);
}

void motorOpen(void) {

	// Interrupcion por match 0
	if(TIM_GetIntStatus(LPC_TIM0,TIM_MR0_INT)) {
		switch(flag_timer){
			case 0:
				limpiarPines();
				GPIO_SetValue(PORT_ZERO,PIN_0);
				flag_timer++;
				break;

			case 1:
				limpiarPines();
				GPIO_SetValue(PORT_ZERO,PIN_1);
				flag_timer++;
				break;

			case 2:
				limpiarPines();
				GPIO_SetValue(PORT_ZERO,PIN_2);
				flag_timer++;
				break;

			case 3:
				limpiarPines();
				GPIO_SetValue(PORT_ZERO,PIN_3);
				flag_timer = 0;
				break;
		}
	}
	return;

}

void motorClose(void) {

	// Interrupcion por match 0
	if(TIM_GetIntStatus(LPC_TIM0,TIM_MR0_INT)) {
		switch(flag_timer){
			case 0:
				limpiarPines();
				GPIO_SetValue(PORT_ZERO,PIN_3);
				flag_timer++;
				break;

			case 1:
				limpiarPines();
				GPIO_SetValue(PORT_ZERO,PIN_2);
				flag_timer++;
				break;

			case 2:
				limpiarPines();
				GPIO_SetValue(PORT_ZERO,PIN_1);
				flag_timer++;
				break;

			case 3:
				limpiarPines();
				GPIO_SetValue(PORT_ZERO,PIN_0);
				flag_timer = 0;
				break;
		}
	}
	return;

}


void TIMER0_IRQHandler(void) {
	if(control) {
		motorOpen();
	} else {
		motorClose();
	}

	// Limpiamos bandera
	TIM_ClearIntPending(LPC_TIM0,TIM_MR0_INT);
}

void TIMER1_IRQHandler(void) {
	// Desactivamos Timer 0 para parar el motor
	TIM_Cmd(LPC_TIM0,DISABLE);

	// Reiniciamos y desactivamos Timer 1
	TIM_Cmd(LPC_TIM1,DISABLE);
	TIM_ResetCounter(LPC_TIM1);

	// Limpiamos bandera
	TIM_ClearIntPending(LPC_TIM1,TIM_MR0_INT);

	// Volvemos a activar interrupciones EINT3
	NVIC_EnableIRQ(EINT3_IRQn);
}

