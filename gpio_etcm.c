/*
 * gpio_etcm.c
 *
 *  Created on: Feb 4, 2020
 *      Author: Brady Siegel
 */
#include "gpio_etcm.h"

void initGPIO()
{
//    GPIO_setDirectionMode( , GPIO_DIR_MODE_OUT);
//    GPIO_setDirectionMode( , GPIO_DIR_MODE_OUT);
//    GPIO_setDirectionMode( , GPIO_DIR_MODE_OUT);
//    GPIO_setDirectionMode( , GPIO_DIR_MODE_OUT);
//    GPIO_setDirectionMode( , GPIO_DIR_MODE_OUT);
//
//    GPIO_setDirectionMode( , GPIO_DIR_MODE_IN);
}
void sampleGPIO(char* status)
{
    *status <<= 1;
    *status += 1;
}
void buttonStateMachine(char status, buttonState* b)
{
    switch(*b)
    {
    case UP:
        if (status == 0xFFFF)
        {
            *b = DOWN;
        }
        break;
    case DOWN:
        if (status == 0x0000)
        {
            *b = UP;
        }
        break;
    }
}

