#include "dac_adc.h"

#include "board.h"
#include "fsl_dac.h"
#include "fsl_adc16.h"

#include "pin_mux.h"
#include "clock_config.h"

volatile bool g_Adc16ConversionDoneFlag = false;
volatile uint32_t g_Adc16ConversionValue = 0;
void ADC0_IRQHandler(void)
{
    g_Adc16ConversionDoneFlag = true;
    /* Read conversion result to clear the conversion completed flag. */
    g_Adc16ConversionValue = ADC16_GetChannelConversionValue(ADC0, 0U);
}

static adc16_config_t sAdc16ConfigStruct;
static adc16_channel_config_t sAdc16ChannelConfigStruct;

void dac_init()
{
	// TAKEN FROM SDK
    dac_config_t dacConfigStruct;

    /* Configure the DAC. */
    /*
     * dacConfigStruct.referenceVoltageSource = kDAC_ReferenceVoltageSourceVref2;
     * dacConfigStruct.enableLowPowerMode = false;
     */
    DAC_GetDefaultConfig(&dacConfigStruct);
    DAC_Init(DAC0, &dacConfigStruct);
    DAC_Enable(DAC0, true);             /* Enable output. */
    DAC_SetBufferReadPointer(DAC0, 0U); /* Make sure the read pointer to the start. */
                                                     /*
                                                     * The buffer is not enabled, so the read pointer can not move automatically. However, the buffer's read pointer
                                                     * and itemss can be written manually by user.
                                                     */

}

void adc_init()
{
	//EnableIRQ(ADC0_IRQn);
    ADC16_GetDefaultConfig(&sAdc16ConfigStruct);
    ADC16_Init(ADC0, &sAdc16ConfigStruct);

    /* Make sure the software trigger is used. */
    ADC16_EnableHardwareTrigger(ADC0, false);

    /* Prepare ADC channel setting */
    sAdc16ChannelConfigStruct.channelNumber = 0U;
    sAdc16ChannelConfigStruct.enableInterruptOnConversionCompleted = false;
}

// TODO error code
void write_dac(uint32_t inVal)
{
	DAC_SetBufferValue(DAC0, 0U, inVal);
}

uint32_t read_adc()
{
	ADC16_SetChannelConfig(ADC0, 0U, &sAdc16ChannelConfigStruct);

	// TODO: Don't block here maybe?
	while (0U == (kADC16_ChannelConversionDoneFlag &
				  ADC16_GetChannelStatusFlags(ADC0, 0U)))
	{
	}
	return ADC16_GetChannelConversionValue(ADC0, 0U);

//	while (!g_Adc16ConversionDoneFlag)
//	        {
//	        }
//	return g_Adc16ConversionValue;
}
