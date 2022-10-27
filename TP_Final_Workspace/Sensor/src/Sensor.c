/*
===============================================================================
 Name        : HC-SR04 SENSOR
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================

===============================================================================
*/

#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_systick.h"


#define OUTPUT (uint8_t)1
#define INPUT (uint8_t)0

#define PORT_1 (uint8_t)1
#define PIN_28 (uint32_t)(1<<28)
#define PIN_18 (uint32_t)(1<<18)

#define MATCH_CHANNEL_0 (uint8_t)0
#define CAPTURE_CHANNEL_0 (uint8_t)0

uint8_t pin_flag = 0; // flag para saber si es flanco de subida o bajada en capture
uint8_t sensor_flag = 0; // 1 cuandose esta midiendo

uint8_t turnOnPin(uint8_t);
void config_pines();
void config_timer_SR04();
void signalChecker();
void TrigConfig();
void EchoConfig();
void delay(uint8_t);

int main(void) {
	config_pines();
	config_timer_SR04();

	while(1){

		/* enviamos un 1 durante 12us */
		GPIO_SetValue(PORT_1, PIN_28);
		delay(12); // delay de 12useg
		GPIO_ClearValue(PORT_1, PIN_28);
		signalChecker(); // a los ...

		EchoConfig();
		sensor_flag = 1;
		while(sensor_flag); // esperamos a que termine de sensar
	}
	return 0;
}

/* Metodo que genera un delay en us */
void delay(uint8_t time){

	uint32_t counter = (uint32_t)(time*100); // calcular cuentas para  el delay en us

	while(delayCounter < time) delayCounter++;

}

void config_pines(){

	/* Configuracion de los pines necesarios para el HC-SR04
	 *
	 * Trigger: P1.28
	 * Echo: P1.18 - CAP1.0
	 *
	 * */

	/* Trigger */
	GPIO_SetDir(PORT_1, PIN_28, OUTPUT, OUTPUT); // pin donde ira conectado trig

	/* Echo */
	PINSEL_CFG_Type *SR04_E;
	SR04_E->Portnum = PINSEL_PORT_1;
	SR04_E->Pinnum = PINSEL_PIN_18;
	SR04_E->Funcnum = PINSEL_FUNC_3; // funcion CAP1.0
	SR04_E->OpenDrain = PINSEL_PINSEL_PINMODE_NORMAL; // sin open drain
	PINSEL_ConfigPin(SR04_E);

	GPIO_SetDir(PORT_1, PIN_18, INPUT); //  no se si hace falta esto

	// TODO -> pin del led integrado que tiene la placa
	GPIO_SetDir(PORT_1, PIN_18, OUTPUT);

}

void config_timer_SR04(){

	/* Config Timer 1 que se usara para el HC-SR04 */
	EchoConfig();

}

/* Metodo que configura el timer 1 para que funcione como CAP1.0 */
void EchoConfig(){

	/* Capture en flanco de bajada y subida para capturar la respuesta del sensor */

	TIM_TIMERCFG_Type *timer_1;
	timer_0->PrescaleOption = TIM_PRESCALE_USVAL; // prescaler en useg
	timer_0->PrescaleValue = 12; // este valor puede ser 10 o cercano pero superior a 10
	TIM_Init(LPC_TIM1, TIM_TIMER_MODE,timer_1);

	TIM_CAPTURECFG_Type *capture_1;
	capture_1->CaptureChannel = CAPTURE_CHANNEL_0;
	capture_1->IntOnCaption = ENABLE;
	capture_1->FallingEdge = ENABLE;
	capture_1->RisingEdge = ENABLE;

	TIM_ConfigCapture(LPC_TIM1, capture_1);
	NVIC_EnableIRQ(TIMER1_IRQn);

}

void TIMER1_IRQHandler(){

	uint32_t captureValue = 0; // aca se guarda el valor de capture

	TIM_ClearIntPending(LPC_TIM1,TIM_CR0_INT); // limpiamos flag

	// si es cero entonces empezamos a contar
	if(!pin_flag){
		TIM_Cmd(LPC_TIM1,ENABLE);
		pin_flag = 1;
		captureValue = 0;
	}
	// si es uno debemos
	else if (pin_flag){
		captureValue = TIM_GetCaptureValue(LPC_TIM1,TIM_COUNTER_INCAP0);
		pin_flag = 0;
		turnOnPin(captureValue); // si senso algo prende un led
		captureValue = 0;
		// si capturo y no salto el SysTick es porque esta dentro de lo permitido
		TIM_Cmd(LPC_TIM1,DISABLE);
	}


}


/* Metodo que prende o apaga el led */
uint8_t turnOnPin(uint8_t captureValue){
	if(captureValue)
		GPIO_SetValue(portNum, bitValue);
	else
		GPIO_ClearValue(portNum, bitValue);
}

/* Metodo que inicializa el SysTick para contar useg
 *
 * Porque si pasa mas de ... se debe ignorar la medida ya que
 * significa que se supero la distancia maxima.
 *
 *  */
void signalChecker(){

	SysTick_Config(); // TODO

}

/* Metodo donde se configura el SysTick
 * @param time tiempo en ms
 *
 *
 * */
void SysTick_Config(uint32_t time){

	/* configuramos el SysTick */
	SYSTICK_InternalInit(time);
	SYSTICK_IntCmd(ENABLE);
	SYSTICK_Cmd(ENABLE);

	NVIC_EnableIRQ(SysTick_IRQn);
}

/* Si interrumpe es porque hay que hacer otra medida */
void SysTick_IRQHandler(){

	if(pin_flag) // significa que no senso nada dentro de los limites del sensor
		pin_flag = 0;

	SysTick->CTRL &= SysTick->CTRL; // limpiamos la flag
	sensor_flag = 1;
	// desactivamos el timer 1 (capture de echo)
	TIM_Cmd(LPC_TIM1,DISABLE); // dejamos de contar ()

}

