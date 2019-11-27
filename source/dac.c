#include "dac.h"

#include "board.h"
#include "fsl_dac.h"

#include "pin_mux.h"
#include "clock_config.h"




void init_dac()
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

// TODO error code
void write_dac(uint32_t inVal)
{
	DAC_SetBufferValue(DAC0, 0U, inVal);
}
