/*
===============================================================================
 Name        : TP_Final
 Description :
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
#define KEY_COL 		4
#define KEY_ROW 		4
#define KEYBOARD_SIZE 	(KEY_COL * KEY_ROW)
#define CODE_SIZE		4
#define LETTER_A		10
#define LETTER_B		11
#define LETTER_C		12
#define LETTER_D		13

/*
 *  Definicion de funciones
 */

// Funciones de configuracion
void configPin(void);
void configInts(void);
void configTimer(void);
void configADC(void);
void configDMA(void);
// Funciones para el teclado
void bounceDelay(void);
uint8_t getKey(void);
void configPassword(void);
void registerEmployee(void);
void checkArea(void);
void enterEmployee(void);
void getLog(void);
// Funciones para el motor paso a paso
void motorOpen(void);
void motorClose(void);
void clearMotorPins(void);
// Funciones para el ADC
void checkBat(void);

/*
 *  Variables Globales
 */

uint32_t key_aux = 0;							// Variable auxiliar para guardar una tecla
uint8_t keysHex[KEYBOARD_SIZE] = 				// Array con las teclas en hexadecimal
{
	0x06, 0x5B, 0x4F, 0x77,	//	1	2	3	A
	0x66, 0x6D, 0x7D, 0x7C,	//	4	5	6	B
	0x07, 0x7F, 0x67, 0x39,	//	7	8	9	C
	0x79, 0x3F, 0x71, 0x5E	//	E	0	F	D
};
uint8_t keysDec[KEYBOARD_SIZE] =				// Array con las teclas en decimal
{
	1, 2, 3, 10,	//	1	2	3	A
	4, 5, 6, 11,	//	4	5	6	B
	7, 8, 9, 12,	//	7	8	9	C
	0, 0, 0, 13		//	0	0	0	D
};
uint8_t admin_password[CODE_SIZE-1];		// Contraseña de administrador
uint8_t code_buffer[CODE_SIZE-1];			// Buffer para el codigo ingresado


/*
 * 	Estructura que representa a un empleado.
 * 	Sus campos:
 * 		1. codigo: Codigo de identificacion del empleado.
 * 		2. area: Area donde trabaja el empleado.
 * 		3. presentismo: Campo que representa el presentismo. Cuenta los dias de ausente.
 * 		4. ausente: Campo que representa si el empleado estuvo ausente en caso de ser TRUE.
 */
typedef struct Employee {
	uint8_t codigo[4];
	char *area;
	uint8_t presentismo;
	uint8_t ausente;
} Employee;

/*
 * 	Funcion Principal.
 */
int main(void) {

	configPin();

	// ALOCAR MEMORIA PARA LOS EMPLEADOS ACA??

    while(1) {}

    return 0 ;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//									CONFIGURACIONES
///////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * 	Funcion que configura todos los pines que se utilizan en el proyecto, incluidas sus
 * 	funciones alternas.
 */
void configPin(void) {
	PINSEL_CFG_Type pin_config;

	/*
	 * 	PUERTO 1 :
	 * 	Salidas para Display 7-segmentos
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
	 * 	PUERTO 2 :
	 * 	Teclado Matricial
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
	GPIO_SetDir(PORT_TWO, (0xF << 4), INPUT);

	// Configuramos interrupciones por GPIO del puerto 2
	FIO_IntCmd(PORT_TWO,(0xF << 4),FALLING);
	FIO_ClearInt(PORT_TWO,(0xF << 4));


	// Habilitamos las interrupciones
	NVIC_EnableIRQ(EINT3_IRQn);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//									INTERRUPCIONES
///////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * 	Configuracion de las interrupciones, incluyendo EXTI, GPIO y perifericos,
 * 	junto con las prioridades de cada uno.
 */
void configInts(void){

}

///////////////////////////////////////////////////////////////////////////////////////////////////
//										TECLADO
///////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * 	El teclado funciona por interrupciones GPIO. Esta funcion se encarga de analizar las teclas
 * 	presionadas y hacer algo dependiendo de las teclas que hayan sido presionadas.
 * 	Si la primer tecla presionada es una letra (A,B,C o D) sabemos que se quiere realizar una
 * 	accion de admin (configPassword, registerEmployee, getLog).
 * 	En caso contrario, es un empleado intentando ingresar (enterEmployee).
 */
void EINT3_IRQHandler(void) {
	// Desactivamos la interrupcion por EINT3
	NVIC_DisableIRQ(EINT3_IRQn);

	// Variable auxiliar para contar los digitos del codigo
	static uint8_t dig_aux = 0;
	// Variable auxiliar para la tecla de configuracion
	static uint8_t conf_key = 0;

	// Guardamos el valor de tecla en un aux
	key_aux = (GPIO_ReadValue(PORT_TWO) & 0xF0);

	// Hacemos un delay anti-rebote
	bounceDelay();

	/*
	 *		Analisis del teclado
	 */

	// Chequeamos que el valor de la tecla sea el mismo despues del delay
	if(((GPIO_ReadValue(PORT_TWO) & 0xF0) - key_aux) == 0) {
		// Obtenemos el valor de la tecla presionada
		uint8_t key = getKey();
		// Valor en hexadecimal para mostrarlo por display
		uint8_t keyHex = keysHex[key];
		// Valor decimal para realizar operaciones
		uint8_t keyDec = keysDec[key];

		// Si la tecla presionada es una letra:
		if(keyDec > 9){
			// Reiniciamos el dig_aux
			dig_aux = 0;
			// Guardamos el valor en la variable auxiliar para configuracion
			conf_key = keyDec;
		} else {	// En caso contrario:
			// Guardamos el valor de la tecla en el buffer
			code_buffer[dig_aux] = keyDec;
			// Aumentamos la variable auxiliar para digitos
			dig_aux++;
		}

		// La mostramos en el display
		FIO_HalfWordClearValue(PORT_ONE,LOWER,0xFF);
		FIO_HalfWordClearValue(PORT_ONE,LOWER,keyHex);
	}

	/*
	 * 		Analisis del codigo
	 */

	// Cuando tengamos el codigo completo
	if((dig_aux == CODE_SIZE)){
		// Si no se presiono tecla de configuracion, hay un empleado ingresando
		if(conf_key == 0){
			enterEmployee();
		} else if(conf_key == LETTER_A) {
			configPassword();
		} else if(conf_key == LETTER_B) {
			registerEmployee();
		} else if(conf_key == LETTER_C) {
			getLog();
		} else {
			// Mensaje de error
		}
		// Reiniciamos auxiliares
		dig_aux = 0;
		conf_key = 0;
	}


	// Limpiamos banderas
	FIO_ClearInt(PORT_TWO, (0xF << 4));
	// Volvemos a habilitar las interrupciones
	NVIC_EnableIRQ(EINT3_IRQn);
}

/*
 * 	Funcion que se encarga de realizar un retardo anti-rebote
 * 	para las teclas del teclado.
 */
void bounceDelay(void) {
	for(uint32_t tmp = 0; tmp < 600000; tmp++);
}

/*
 * 	Funcion que se encarga de escanear el teclado y obtener la teclad
 * 	que fue presionada.
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
		if((FIO_ByteReadValue(PORT_TWO, 0) & 0xF0) == 0xF0) {
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

///////////////////////////////////////////////////////////////////////////////////////////////////
//										  BARRERA
///////////////////////////////////////////////////////////////////////////////////////////////////

void motorOpens(void) {

}

void motorClose(void) {

}

void TIMER0_IRQHandler(void) {

}

void clearMotorPins(void) {

}

///////////////////////////////////////////////////////////////////////////////////////////////////
//											ADC
///////////////////////////////////////////////////////////////////////////////////////////////////

void checkBat(void) {

}

///////////////////////////////////////////////////////////////////////////////////////////////////
//										ADMINISTACION
///////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * 	Funcion que se encarga de configurar la contraseña de admin, si se presiono primeramente la
 * 	*tecla A*. La contraseña consta de 4 numeros y es guardada en la variable global admin_password.
 * 	Si admin_password es NULL, no se podra realizar ninguna configuracion.
 * 	Si se vuelve a presionar la *tecla A* luego de haber una contraseña configurada, se necesitara que
 * 	se ingrese la misma antes de reconfigurar la contraseña.
 */
void configPassword(void) {

}

void registerEmployee(void) {

}

void enterEmployee(void) {

}

void getLog(void) {

}
