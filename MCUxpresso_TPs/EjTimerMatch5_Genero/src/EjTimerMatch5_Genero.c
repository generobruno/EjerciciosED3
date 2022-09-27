/*
===============================================================================
 Name        : EjTimerMatch5_Genero.c
 Author      : Bruno A. Genero
 Description : Utilizando los 4 registros match del Timer 0 y las salidas
 P0.0, P0.1, P0.2 y P0.3, realizar un programa en C que permita obtener
 las formas de ondas adjuntas, donde los pulsos en alto tienen una
 duración de 5 mseg. Un pulsador conectado a la entrada EINT3, permitirá
 elegir entre las dos secuencias mediante una rutina de servicio a la
 interrupción. La prioridad de la interrupción del Timer tiene que ser
 mayor que la del pulsador. Estas formas de ondas son muy útiles para
 controlar un motor paso a paso. Adjuntar el código en C.
===============================================================================
*/

#include "LPC17xx.h"

void confTimer(void);
void confGPIO(void);
void confInt(void);
void parMenor(void);
void parMax(void);

uint8_t control = 0;

int main(void) {

	confGPIO();
	confTimer();
	confInt();

    while(1) {
    }
    return 0 ;
}

/**
 * Configura el timer para realizar la secuencia del
 * par menor por el puerto 0.
 */
void parMenor(void) {
	// Desactivamos el timer para poder reconfigurarlo
	if((LPC_TIM0->TCR) & (1<<0)) {
		LPC_TIM0->TCR = 3;
	}

	// Configuramos los Matches
	LPC_TIM0->MR0 = 0x7A11F;	// 5 ms
	LPC_TIM0->MR1 = 0xF423F;	// 10 ms
	LPC_TIM0->MR2 = 0x16E35F;	// 15 ms
	LPC_TIM0->MR3 = 0x1E847F;	// 20 ms
	// Hacemos que se reinicie con MR3
	LPC_TIM0->MCR |= (1<<10);

	// Volvemos a habilitar el timer
	LPC_TIM0->TCR &= ~(1<<1);
	return;
}

/**
 * Configura el timer para realizar la secuencia del
 * par maximo por el puerto 0.
 */
void parMax(void) {
	// Desactivamos el timer para poder reconfigurarlo
	if((LPC_TIM0->TCR) & (1<<0)) {
		LPC_TIM0->TCR = 3;
	}

	// Configuramos los Matches
	LPC_TIM0->MR0 = 0xF423F;	// 10 ms
	LPC_TIM0->MR1 = 0x16E35F;	// 15 ms
	LPC_TIM0->MR2 = 0x1E847F;	// 20 ms
	LPC_TIM0->MR3 = 0x26259F;	// 25 ms
	// Hacemos que se reinicie con MR3
	LPC_TIM0->MCR |= (1<<10);

	// Volvemos a habilitar el timer
	LPC_TIM0->TCR &= ~(1<<1);
	return;
}

void confGPIO(void) {
	// Configuramos los pines P0.0 a P0.3 como salida
	LPC_GPIO0->FIODIR0 |= (15<<0);
	return;
}

void confTimer(void) {
	// El timer 0 ya esta siendo alimentado por reset
	// pclk = cclk
	LPC_SC->PCLKSEL0 |= (1<<2);
	// Cargamos los valores de match (par menor predet, pckl = 100MHz)
	parMenor();
	// Habilitamos las interrupciones por los matches
	LPC_TIM0->MCR |= (1<<0);
	LPC_TIM0->MCR |= (1<<3);
	LPC_TIM0->MCR |= (1<<6);
	LPC_TIM0->MCR |= (1<<9);
	// Habilitamos el timer
	LPC_TIM0->TCR = 3;
	LPC_TIM0->TCR &= ~(1<<1);
	return;
}

void confInt(void) {
	// Configuramos pin P2.13 como EINT3
	LPC_PINCON->PINSEL4 |= (1<<26);
	// EINT3 por flanco de bajada
	LPC_SC->EXTMODE |= (1<<3);
	LPC_SC->EXTPOLAR &= ~(1<<3);
	LPC_SC->EXTINT |= (1<<3);

	// Configuramos prioridades
	NVIC_SetPriority(EINT3_IRQn, 6);
	NVIC_SetPriority(TIMER0_IRQn, 5);

	// Habilitamos las interrupciones
	NVIC_EnableIRQ(EINT3_IRQn);
	NVIC_EnableIRQ(TIMER0_IRQn);
	return;
}

void EINT3_IRQHandler(void) {
	// Cambiamos el modo de contar del timer
	if(control){
		parMax();
		control = 1;
	} else {
		parMenor();
		control = 0;
	}
	return;
}

/**
 * Saca por el puerto 0 las secuencias determinadas
 * por el timer 0.
 */
void TIMER0_IRQHandler(void) {
	// Enmascaramos los pines
	LPC_GPIO0->FIOMASK0 = 0x0F;

	// Interrumpe MR0
	if((LPC_TIM0->IR) & (1<<0)) {
		LPC_GPIO0->FIOSET0 |= (1<<0);
		//Limpiamos bandera
		LPC_TIM0->IR |= (1<<0);
	} else {
		LPC_GPIO0->FIOCLR0 |= (1<<0);
	}

	// Interrumpe MR1
	if((LPC_TIM0->IR) & (1<<1)) {
		LPC_GPIO0->FIOSET0 |= (1<<1);
		//Limpiamos bandera
		LPC_TIM0->IR |= (1<<1);
	} else {
		LPC_GPIO0->FIOCLR0 |= (1<<1);
	}

	// Interrumpe MR2
	if((LPC_TIM0->IR) & (1<<2)) {
		LPC_GPIO0->FIOSET0 |= (1<<2);
		//Limpiamos bandera
		LPC_TIM0->IR |= (1<<2);
	} else {
		LPC_GPIO0->FIOCLR0 |= (1<<2);
	}

	// Interrumpe MR3
	if((LPC_TIM0->IR) & (1<<3)) {
		LPC_GPIO0->FIOSET0 |= (1<<3);
		//Limpiamos bandera
		LPC_TIM0->IR |= (1<<3);
	} else {
		LPC_GPIO0->FIOCLR0 |= (1<<3);
	}

	return;
}
