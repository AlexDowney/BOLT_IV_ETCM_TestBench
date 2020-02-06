/*
 * adc_etcm.h
 *
 *  Created on: Nov 1, 2019
 *      Author: willc
 */

#ifndef ADC_ETCM_H_
#define ADC_ETCM_H_

#include "adc.h"
#include "device.h"

void initADC(void);
//void initEPWM(void);
void initADCSOC(void);
uint16_t getADCVal(void);
__interrupt void adcA0ISR(void);

#define ADC_BASE ADCA_BASE

#endif /* ADC_ETCM_H_ */
