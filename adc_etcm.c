/*
 * adc_etcm.c
 *
 *  Created on: Nov 1, 2019
 *      Author: willc
 */

#include "adc_etcm.h"

//Initialize and power up the ADC
void initADC(void)
{
    //Set ADC divider to /4.
    ADC_setPrescaler(ADC_BASE, ADC_CLK_DIV_4_0);

    //Set resolution and signal mode and load the right trims.
    ADC_setMode(ADC_BASE, ADC_RESOLUTION_12BIT, ADC_MODE_SINGLE_ENDED);

    //Set pulse position to late.
    ADC_setInterruptPulseMode(ADC_BASE, ADC_PULSE_END_OF_CONV);

    //Power up ADC and delay for one ms.
    ADC_enableConverter(ADC_BASE);
    DEVICE_DELAY_US(1000);
}

////Initialize EPWM
//void initEPWM(void)
//{
//
//}

//Initialize ADC SOC
void initADCSOC(void)
{
//    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_CPU1_TINT0,
//                 ADC_CH_ADCIN0, 15);
    ADC_setupSOC(ADC_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY,
                 ADC_CH_ADCIN0, 15);
    ADC_setInterruptSource(ADC_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER0);
    ADC_enableInterrupt(ADC_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(ADC_BASE, ADC_INT_NUMBER1);
}

uint16_t getADCVal(void)
{
    ADC_forceSOC(ADC_BASE, ADC_SOC_NUMBER0);
    ADC_forceSOC(ADC_BASE, ADC_SOC_NUMBER1);

    while(ADC_getInterruptStatus(ADC_BASE, ADC_INT_NUMBER1) == false)
    {
    }
    ADC_clearInterruptStatus(ADC_BASE, ADC_INT_NUMBER1);

    return ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER1);
}
