/*
===============================================================================
 Name        : eje1Genero.c
 Author      : Bruno A. Genero
 Description : Una famosa empresa de calzados a incorporado a sus
 zapatillas 10 luces leds comandadas por un microcontrolador LPC1769
 y ha pedido a su grupo de ingenieros que diseñen 2 secuencias de luces
 que cada cierto tiempo se vayan intercalando (secuencia A - secuencia B-
 secuencia A- ... ). Como todavía no se ha definido la frecuencia a la
 cual va a funcionar el CPU del microcontrolador, las funciones de
 retardos que se incorporen deben tener como parametros de entrada
 variables que permitan modificar el tiempo de retardo que se vaya a
 utilizar finalmente. Se pide escribir el código que resuelva este
 pedido, considerando que los leds se encuentran conectados en los
  puertos P0,0 al P0.9.
===============================================================================
*/

#include "LPC17xx.h"

void retardo(uint32_t limite);

int main(void){
	uint32_t relojCpu = SystemCoreClock;

	// Configuramos los pines P0.0 a P0.9 como salida
	LPC_GPIO0->FIODIRL = 0x02FF;
	LPC_GPIO0->FIOMASKL = 0xF000;

	// Definimos un limite
	uint32_t lim = 8000000;

	// Hacemos el Parpadeo
	while(1){
		LPC_GPIO0->FIOSETL = 0x02AA;
		retardo(lim);
		LPC_GPIO0->FIOCLRL = 0x02AA;
		LPC_GPIO0->FIOSETL = 0x0155;
		retardo(lim);
		LPC_GPIO0->FIOCLRL = 0x0155;
	}

	return 0;
}

/*
 * Funcion retardo
 * Genera un retardo cuya longitud depende del parametro
 * "limite" que le pasemos como argumento.
 */
void retardo(uint32_t limite){
	uint32_t contador;
	for(contador = 0; contador < limite; contador++){};
}



