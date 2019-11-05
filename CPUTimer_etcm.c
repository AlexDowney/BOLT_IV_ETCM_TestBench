/*
 * CPUTimer_etcm.c
 *
 *  Created on: Nov 4, 2019
 *      Author: Brady Siegel
 */

#include "CPUTimer_etcm.h"

void initTimer(uint32_t base, uint32_t period, uint32_t prescalar)
{
    CPUTimer_setPeriod(base, period);
    CPUTimer_setPrescalar(base, prescalar);
    CPUTimer_enableInterrupt(base);
}
void startTimer(uint32_t base)
{
    CPUTimer_startTimer(base);
}
