/*
===============================================================================
 Name        : EjemploCMSIS.c
 Author      : Bruno A. Genero
===============================================================================
*/

#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

#define PORT_ZERO 	(uint8_t) 0
#define PIN_22		(uint32_t) (1<<22)
#define OUTPUT		(uint8_t) 1

void confGPIO(void) {
	PINSEL_CFG_Type pin_configuration;

	pin_configuration.Portnum = PINSEL_PORT_0; 			// PINSEL_PORT_0 = 0 (Puerto 0)
	pin_configuration.Pinnum = PINSEL_PIN_22;			// PINSEL_PIN_22 = 22 (Pin 22)
	pin_configuration.Pinmode = PINSEL_PINMODE_PULLUP;	// Habilitamos pull-up

	PINSEL_ConfigPin(&pin_configuration);
	GPIO_SetDir(PORT_ZERO,PIN_22,OUTPUT);				// Pin P0.22 como salida
}


