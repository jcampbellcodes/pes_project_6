#include "dac_adc.h"
#include "board.h"
#include "fsl_dac.h"
#include "fsl_adc16.h"
#include "pin_mux.h"
#include "clock_config.h"

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
}

void adc_init()
{
	adc16_config_t sAdc16ConfigStruct;
    ADC16_GetDefaultConfig(&sAdc16ConfigStruct);
    ADC16_Init(ADC0, &sAdc16ConfigStruct);

    /* Make sure the software trigger is used. */
    ADC16_EnableHardwareTrigger(ADC0, false);
}

void write_dac(uint32_t inVal)
{
	DAC_SetBufferValue(DAC0, 0U, inVal);
}

uint32_t read_adc()
{
	adc16_channel_config_t sAdc16ChannelConfigStruct;
    /* Prepare ADC channel setting */
    sAdc16ChannelConfigStruct.channelNumber = 0U;
    sAdc16ChannelConfigStruct.enableInterruptOnConversionCompleted = false;

	ADC16_SetChannelConfig(ADC0, 0U, &sAdc16ChannelConfigStruct);

	while (0U == (kADC16_ChannelConversionDoneFlag &
				  ADC16_GetChannelStatusFlags(ADC0, 0U)))
	{
	}
	return ADC16_GetChannelConversionValue(ADC0, 0U);
}
