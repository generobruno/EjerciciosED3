/*
===============================================================================
 Name        : Ext_Int1_Genero.c
 Author      : Bruno A. Genero
 Description :
Configurar la interrupción externa EINT1 para que interrumpa por flanco
de bajada y la interrupción EINT2 para que interrumpa por flanco de
subida. En la interrupción por flanco de bajada configurar el systick
para desbordar cada 25 mseg, mientras que en la interrupción por flanco
de subida configurarlo para que desborde cada 60 mseg. Considerar que
EINT1 tiene mayor prioridad que EINT2
===============================================================================
*/

#include "LPC17xx.h"

void configGPIO(void);
void configIntExt(void);
void EINT1_IRQHandler(void);
void EINT2_IRQHandler(void);
void Config_Systick(uint32_t val);

int main(void) {

	configGPIO();
	configIntExt();

	while(1){
		// Prender LED de prueba
		LPC_GPIO0->FIOSET |= (1<<22);
	}

    return 0 ;
}

void configGPIO(void) {
	LPC_GPIO0->FIODIR |= (1<<22);
	return;
}

void configIntExt(void) {
	// Configuramos P2.11 y P2.12 como EINT1 y EINT2
	LPC_PINCON->PINSEL4 |= (5<<22);
	// Configuramos EINT1 y EINT2 para int por flancos
	LPC_SC->EXTMODE |= (6<<0);
	// Configuramos EINT1 = flanco bajada y EINT2 = flanco subida
	LPC_SC->EXTPOLAR |= (4<<0);
	// Limpiamos las banderas
	LPC_SC->EXTINT |= (6<<0);
	// Habilitamos las interrupciones
	NVIC_EnableIRQ(EINT1_IRQn);
	NVIC_EnableIRQ(EINT2_IRQn);
	return;
}

// Configuramos el SysTick para desbordar cada 25ms (100 MHz)
void EINT1_IRQHandler(void) {
	Config_Systick(0x26259F);
	return;
}

// Configuramos el SysTick para desbordar cada 60ms (100 MHz)
void EINT2_IRQHandler(void) {
	Config_Systick(0x5B8D7F);
	return;
}

void Config_Systick(uint32_t val) {
	// Deshabilitamos el SysTick
	SysTick->CTRL &= ~(1<<0);
	// Cargamos el SysTick
	SysTick->LOAD = val;
	// Limpiamos el SysTick
	SysTick->VAL = 0;
	// Habilitamos el SysTick
	SysTick->CTRL |= (1<<0);
	// La fuente de reloj es cclk por reset y no queremos que interrumpa
	return;
}


