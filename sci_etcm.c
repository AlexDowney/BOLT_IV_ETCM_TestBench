/*
 * sci_etcm.c
 *
 *  Created on: Nov 4, 2019
 *      Author: Brady Siegel
 */

#include "sci_etcm.h"

void initSCI(uint32_t base, uint32_t lspclkHz, uint32_t baud)
{
    SCI_setConfig(base, lspclkHz, baud, SCI_CONFIG_WLEN_8);
    GPIO_setDirectionMode(28, GPIO_DIR_MODE_IN);
    GPIO_setPinConfig(GPIO_28_SCIRXDA);
    GPIO_setPinConfig(GPIO_29_SCITXDA);
}
void sendData(uint32_t base, uint16_t* ch, int length)
{
    SCI_writeCharArray(base, ch, length);
}
char* getData(uint32_t base, uint16_t* ch, int length)
{
    return SCI_readCharArray(base, ch, length);
}

#endif /* SCI_ETCM_C_ */
