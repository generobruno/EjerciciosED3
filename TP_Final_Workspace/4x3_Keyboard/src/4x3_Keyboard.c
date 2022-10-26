/*
===============================================================================
 Name        : 4x3_Keyboard.c
 Description :Codigo del teclado 4x3
===============================================================================
*/


/*
 *  Librerias a Incluir para el
 *  proyecto.
 */
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

/*
 *  Macros utiles
 */
#define OUTPUT 			1
#define INPUT 			0
#define FALLING			1
#define RISING			0
#define UPPER			1
#define LOWER			0
#define PORT_ONE		1
#define PORT_TWO 		2
#define KEY_COL 		3
#define KEY_ROW 		4
#define KEYBOARD_SIZE 	(KEY_COL * KEY_ROW)

/*
 *  Definicion de funciones
 */
void configPin(void);
void bounceDelay(void);
uint8_t getKey(void);

/*
 *  Variable Globales
 */
uint32_t key_aux = 0;				// Variable auxiliar para guardar una tecla
uint8_t keysHex[KEYBOARD_SIZE] = 	// Array con las teclas en hexadecimal
{
	0x06, 0x5B, 0x4F,	//	1	2	3
	0x66, 0x6D, 0x7D,	//	4	5	6
	0x07, 0x7F, 0x67,	//	7	8	9
	0x77, 0x3F, 0x7C	//	A	0	B
};


int main(void) {

	configPin();

    while(1) {}

    return 0 ;
}

void configPin(void) {
	PINSEL_CFG_Type pin_config;

	/*
	 * PUERTO 1 : Salidas para Display 7-segmentos
	 */
	pin_config.Portnum = PINSEL_PORT_1;
	pin_config.Funcnum = PINSEL_FUNC_0;
	pin_config.OpenDrain = PINSEL_PINMODE_NORMAL;
	pin_config.Pinmode = PINSEL_PINMODE_TRISTATE;
	for(uint8_t i = 0; i < 8; i++){
		pin_config.Pinnum = i;
		PINSEL_ConfigPin(&pin_config);
	}
	GPIO_SetDir(PORT_ONE,0xFF,OUTPUT);

	/*
	 * PUERTO 2 : Teclado Matricial
	 */
	pin_config.Portnum = PINSEL_PORT_2;
	pin_config.Funcnum = PINSEL_FUNC_0;
	pin_config.OpenDrain = PINSEL_PINMODE_NORMAL;
	pin_config.Pinmode = PINSEL_PINMODE_TRISTATE;

	// Configuramos las filas = SALIDAS
	for(uint8_t i = 0; i < KEY_ROW; i++) {
		pin_config.Pinnum = i;
		PINSEL_ConfigPin(&pin_config);
	}
	// Configuramos las columnas = ENTRADAS
	pin_config.Pinmode = PINSEL_PINMODE_PULLUP;
	for(uint8_t i = KEY_ROW; i < (KEY_ROW + KEY_COL); i++) {
		pin_config.Pinnum = i;
		PINSEL_ConfigPin(&pin_config);
	}

	// Configuramos columnas y filas GPIO
	GPIO_SetDir(PORT_TWO, 0xF, OUTPUT);
	GPIO_SetDir(PORT_TWO, (0x7 << 4), INPUT);

	// Configuramos interrupciones por GPIO del puerto 2
	FIO_IntCmd(PORT_TWO,(0x7 << 4),FALLING);
	FIO_ClearInt(PORT_TWO,(0x7 << 4));


	// Habilitamos las interrupciones
	NVIC_EnableIRQ(EINT3_IRQn);
}

void EINT3_IRQHandler(void) {
	// Desactivamos la interrupcion por EINT3
	NVIC_DisableIRQ(EINT3_IRQn);

	// Guardamos el valor de tecla en un aux
	key_aux = (GPIO_ReadValue(PORT_TWO) & 0x70);

	// Hacemos un delay anti-rebote
	bounceDelay();

	// Chequeamos que el valor de la tecla sea el mismo despues del delay
	if(((GPIO_ReadValue(PORT_TWO) & 0x70) - key_aux) == 0) {
		// Obtenemos el valor de la tecla presionada
		uint8_t key = getKey();
		uint8_t keyHex = keysHex[key];

		// La mostramos en el display
		FIO_HalfWordClearValue(PORT_ONE,LOWER,0xFF);
		FIO_HalfWordClearValue(PORT_ONE,LOWER,keyHex);
	}

	// Limpiamos banderas
	FIO_ClearInt(PORT_TWO, (0x7 << 4));
	// Volvemos a habilitar las interrupciones
	NVIC_EnableIRQ(EINT3_IRQn);
}

void bounceDelay(void) {
	for(uint32_t tmp = 0; tmp < 600000; tmp++);
}

/*
 * Funcion getKey(void)
 * Escanea el teclado para ver que tecla
 * se presiono.
 */
uint8_t getKey(void) {
	uint8_t row = 0;
	uint8_t col = 0;

	/*
	 *  FILAS:
	 *	Se envia un 1 por fila, y se analiza el primer byte
	 *	del Puerto 2 (P2.0 a 7). Si se presiono una tecla en
	 *	la fila "i", todas las columnas deberian ser 1, por lo
	 *	que el nibble superior deberia ser 0x7.
	 */
	for(uint8_t i = 0; i < KEY_ROW; i++) {
		// Setea en 1 el pin P2.i
		GPIO_SetValue(PORT_TWO,((uint32_t)(1<<i)));

		// Analizo si estan en 1 las columnas
		if((FIO_ByteReadValue(PORT_TWO, 0) & 0x70) == 0x70) {
			// Guardo el valor de la fila
			row = i;
			// Setea en 0 el pin P2.i
			GPIO_ClearValue(PORT_TWO,((uint32_t)(1<<i)));
			// Sale del bucle
			break;
		}

		// Setea en 0 el pin P2.i
		GPIO_ClearValue(PORT_TWO,((uint32_t)(1<<i)));
	}

	/*
	 * COLUMNAS:
	 * Se analiza el auxiliar de tecla hasta encontrar un 0,
	 * en cuyo caso se guarda la columna.
	 */
	for(uint8_t i = 0; i < KEY_COL; i++) {
		if( !(key_aux & ((uint32_t)(1<<(4+i)))) ){
			// Guarda el valor de la columna
			col = i;
			// Sale del bucle
			break;
		}
	}

	// Retorno el valor de la tecla
	return ((4 * row) + col);
}


