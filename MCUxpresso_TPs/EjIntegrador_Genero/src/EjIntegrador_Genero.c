/*
===============================================================================
 Name        : EjIntegrador_Genero.c
 Author      : Bruno A. Genero
 Description : Algoritmo de antirrebote de un pulsador: Escribir un
 programa en C que ante la interrupción por flanco de subida del pin
 P0.1, configurado como entrada digital con pull-down interno, se
 incremente un contador de un dígito, se deshabilite esta interrupción
 y se permita la interrupción por systick cada 20 milisegundos. En cada
 interrupción del systick se testeará una vez el pin P0.1. Solo para el
 caso de haber testeado 3 estados altos seguidos se sacará por los pines
 del puerto P2.0 al P2.7 el equivalente en ascii del valor del contador,
 se desactivará las interrupción por systick y se habilitará nuevamente
 la interrupción por P0.1. Por especificación de diseño se pide que los
 pines del puerto 2 que no sean utilizados deben estar enmascarados por
 hardware. Considerar que el CPU se encuentra funcionando con el oscilador
 interno RC (4Mhz).
===============================================================================
*/

#include "LPC17xx.h"

void configGPIO(void);
void configInt(void);
uint8_t toASCII(uint8_t val);

// Variables globales
uint8_t cont = 0;

int main(void) {

	configGPIO();
	configInt();

    while(1) {}
    return 0 ;
}

void configGPIO() {
	// Pin P0.1 como entrada
	LPC_GPIO0->FIODIR0 &= ~(1<<1);
	// Pin P0.1 con resistencia de pull-down
	LPC_PINCON->PINMODE0 |= (3<<2);
	// Pines P2.0 a P2.7 como salida
	LPC_GPIO2->FIODIR0 = 0xFF;
}

void configInt() {
	// Pin P0.1 interrumpe por flanco de subida
	LPC_GPIOINT->IO0IntEnR |= (1<<1);
	LPC_GPIOINT->IO0IntClr |= (1<<1);	// Limpiamos bandera

	// Configuramos SysTick para interrumpir cada 20ms (cclk 4MHz)
	SysTick->CTRL &= ~(1<<0);	// Deshabilitamos el SysTick
	SysTick->LOAD = 0x1387F;	// Cargamos el valor deseado
	SysTick->VAL = 0;			// Limpiamos el SysTick
	SysTick->CTRL |= (1<<0);	// Habilitamos el SysTick

	// Habilitamos interrupcion para pin P0.1
	NVIC_EnableIRQ(EINT3_IRQn);
}

uint8_t toASCII(uint8_t val) {
	uint8_t ascii;

	if(val <= 9) {
		ascii = val + 48;
	} else {
		cont = 0;
		ascii = 48;
	}

	return ascii;
}

void EINT3_IRQHandler(void) {
	// Chequeamos si interrumpio por P0.1
	if((LPC_GPIOINT->IO0IntStatR) & (1<<1)) {
		cont++;								// Incrementamos el contador
		LPC_GPIOINT->IO0IntClr |= (1<<1);	// Limpiamos bandera
		NVIC_DisableIRQ(EINT3_IRQn);		// Deshabilitamos int EINT3
		NVIC_EnableIRQ(SysTick_IRQn);		// Habilitamos int por SysTick
	}
}

void SysTick_Handler(void) {
	static uint8_t inte = 0;	// Variable de control

	if(inte < 3) {
		if((LPC_GPIOINT->IO0IntStatR) & (1<<1)) {
			inte++;
		}
	} else {
		// Reiniciamos la variable
		inte = 0;
		// Sacamos por los pines P2.0 a P2.7 el valor de cont
		LPC_GPIO2->FIOMASK = 0xFFFFFF00;
		LPC_GPIO2->FIOPIN = toASCII(cont);
		// Deshabilitamos int por SysTick
		NVIC_DisableIRQ(SysTick_IRQn);
		// Habilitamos int por EINT3
		NVIC_EnableIRQ(EINT3_IRQn);
	}
}





