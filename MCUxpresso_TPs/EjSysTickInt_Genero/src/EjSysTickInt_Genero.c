/*
===============================================================================
 Name        : EjSysTickInt_Genero.c
 Author      : Bruno A. Genero
 Description : 1.- Configure el Systick Timer de modo que genere una forma
 de onda llamada PWM tal como la que se muestra en la figura adjunta. Esta
 señal debe ser sacada por el pin P0.22 para que controle la intensidad de
 brillo del led. El periodo de la señal debe ser de 10 mseg con un duty
 cycle de 10%. Configure la interrupción externa EINT0 de modo que cada vez
 que se entre en una rutina de interrupción externa el duty cycle
 incremente en un 10% (1 mseg). Esto se repite hasta llegar al 100%,
 luego, si se entra nuevamente a la interrupción externa, el duty cycle
 volverá al 10%.
	2.- Modificar los niveles de prioridad para que la interrupción por
 systick tenga mayor prioridad que la interrupción externa.
	3. Adjuntar el .C  del código generado.
===============================================================================
*/

#include "LPC17xx.h"

void configGPIO(void);
void configIntExt(void);

// Variable globales
uint8_t inte = 0;
uint8_t dc = 1;

int main(void) {

	configGPIO();
	configIntExt();

    while(1) {
    	if(inte <= dc) {
    		LPC_GPIO0->FIOSET |= (1<<22);
    	} else {
    		LPC_GPIO0->FIOCLR |= (1<<22);
    	}
    }

    return 0;
}

void configGPIO() {
	// Pin P0.22 como salida
	LPC_GPIO0->FIODIR2 |= (1<<6);
}

void configIntExt() {
	// Pin P2.10 como EINT0
	LPC_PINCON->PINSEL4 |= (1<<20);
	// EINT0 por flanco de subida
	LPC_SC->EXTMODE |= (1<<0);
	LPC_SC->EXTPOLAR |= (1<<0);
	LPC_SC->EXTINT |= (1<<0);

	// Configuro el Systick (1ms)
	SysTick_Config(SystemCoreClock/1000);

	// Prioridad Systick > Prioridad EINT0
	NVIC_SetPriority(SysTick_IRQn,1);
	NVIC_SetPriority(EINT0_IRQn,2); // No es necesario

	// Habilito las interrupciones
	NVIC_EnableIRQ(EINT0_IRQn);
	NVIC_EnableIRQ(SysTick_IRQn);
}

void EINT0_IRQHanlder() {
	// Reiniciamos dc si es necesario
	if(dc == 10) { // TODO == o > ??
		dc = 1;
	}
	// Incrementamos la variable del duty cycle
	dc++;
	// Limpiamos banderas
	LPC_SC->EXTINT |= (1<<0);
	return;
}

void SysTick_Handler() {
	// Reiniciamos inte si es necesario
	if(inte == 10) { // TODO == o >??
		inte = 0;
	}
	// Incrementamos la variable global
	inte++;
	// Limpiamos banderas
	SysTick->CTRL &= SysTick->CTRL;
	return;
}



