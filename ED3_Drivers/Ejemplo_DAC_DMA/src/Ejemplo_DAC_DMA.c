#include "lpc17xx_dac.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpdma.h"

#define DMA_SIZE 60
#define NUM_SINE_SAMPLE 60
#define SINE_FREQ_IN_HZ 60
#define PCLK_DAC_IN_MHZ 25 //CCLK divided by 4

void confPin(void);
void confDMA(void);
void confDac(void);

GPDMA_Channel_CFG_Type GPDMACfg;
uint32_t dac_sine_lut[NUM_SINE_SAMPLE];	// Muestras para formar la onda

int main(void)
{
	uint32_t i;
	// 1/4 de una onda senoidal
	uint32_t sin_0_to_90_16_samples[16]={\
			0,1045,2079,3090,4067,\
			5000,5877,6691,7431,8090,\
			8660,9135,9510,9781,9945,10000\
	};
	confPin();
	confDac();
	//Prepare DAC sine look up table
	for(i=0;i<NUM_SINE_SAMPLE;i++)
	{
		if(i<=15)	// 1/4 de onda
		{
			dac_sine_lut[i] = 360 + 360*sin_0_to_90_16_samples[i]/10000;	// Se normaliza a 1
			if(i==15) dac_sine_lut[i]= 720;	// Ya que en i=15 dac_sine_lut seria 1024
		}
		else if(i<=30) // 2/4 de onda
		{
			dac_sine_lut[i] = 360 + 360*sin_0_to_90_16_samples[30-i]/10000;
		}
		else if(i<=45) // 3/4 de onda
		{
			//dac_sine_lut[i] = 360 - 360*sin_0_to_90_16_samples[i-30]/10000;
			dac_sine_lut[i] = 360;
		}
		else 		   // 4/4 de onda
		{
			//dac_sine_lut[i] = 360 - 360*sin_0_to_90_16_samples[60-i]/10000;
			dac_sine_lut[i] = 360;
		}
		dac_sine_lut[i] = (dac_sine_lut[i]<<6);
	}
	confDMA();
	// Enable GPDMA channel 0
	GPDMA_ChannelCmd(0, ENABLE);
	while (1);
	return 0;
}

void confPin(void){
	PINSEL_CFG_Type PinCfg;
	/*
	 * Init DAC pin connect
	 * AOUT on P0.26
	 */
	PinCfg.Funcnum = 2;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 26;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	return;
}

void confDMA(void){
	GPDMA_LLI_Type DMA_LLI_Struct;
	//Prepare DMA link list item structure
	DMA_LLI_Struct.SrcAddr= (uint32_t)dac_sine_lut;			// Fuente de los datos (array de datos)
	DMA_LLI_Struct.DstAddr= (uint32_t)&(LPC_DAC->DACR);		// Origen de los datos (DAC)
	DMA_LLI_Struct.NextLLI= (uint32_t)&DMA_LLI_Struct;		// Siguiente item de lista (el mismo)
	DMA_LLI_Struct.Control= DMA_SIZE						// Tamaños de los campos
			| (2<<18) //source width 32 bit
			| (2<<21) //dest. width 32 bit
			| (1<<26) //source increment
			;
	/* GPDMA block section -------------------------------------------- */

	/* Initialize GPDMA controller */
	GPDMA_Init();

	// Setup GPDMA channel --------------------------------
	// channel 0
	GPDMACfg.ChannelNum = 0;
	// Source memory
	GPDMACfg.SrcMemAddr = (uint32_t)(dac_sine_lut);
	// Destination memory - unused
	GPDMACfg.DstMemAddr = 0;					// EL DESTINO NO ES UNA POSICION DE MEMORIA
	// Transfer size
	GPDMACfg.TransferSize = DMA_SIZE;
	// Transfer width - unused
	GPDMACfg.TransferWidth = 0;
	// Transfer type
	GPDMACfg.TransferType = GPDMA_TRANSFERTYPE_M2P;	// Memoria a Periferico
	// Source connection - unused
	GPDMACfg.SrcConn = 0;
	// Destination connection
	GPDMACfg.DstConn = GPDMA_CONN_DAC;
	// Linker List Item - unused
	GPDMACfg.DMALLI = (uint32_t)&DMA_LLI_Struct;	// Se asocia la lista anterior

	// Setup channel with given parameter
	GPDMA_Setup(&GPDMACfg);

	return;
}

void confDac(void){
	uint32_t tmp;

	DAC_CONVERTER_CFG_Type DAC_ConverterConfigStruct;
	DAC_ConverterConfigStruct.CNT_ENA =SET;			// Habilitamos el contador de DMA
	DAC_ConverterConfigStruct.DMA_ENA = SET;		// Habilitamos DMA para DAC

	DAC_Init(LPC_DAC);

	/* set time out for DAC*/
	// Por regla de 3, si 60 muestras [NUM_SINE_SAMPLE] se envian cada 20ms [SINE_FREC_HZ = 50Hz],
	// ¿Cada cuanto tiempo se enviara 1 muestra? t_m = 1*20ms/60 = 0,333ms
	// Entonces el DMA debe interrumpir cada t_m.
	tmp = (PCLK_DAC_IN_MHZ*1000000)/(SINE_FREQ_IN_HZ*NUM_SINE_SAMPLE);
	DAC_SetDMATimeOut(LPC_DAC,tmp);

	DAC_ConfigDAConverterControl(LPC_DAC, &DAC_ConverterConfigStruct);
	return;
}
