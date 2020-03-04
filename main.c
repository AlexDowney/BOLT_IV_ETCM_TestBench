//#############################################################################
//
// FILE:   main.c
//
// TITLE:  BOLT IV ETCM
//
//#############################################################################
// Contributors:
//  Will Campbell
//  Quinton Miller
//  Josh Collins
//
//#############################################################################
//Notes:
//  Follow the GNOME style guide
//
//  Anything related to individual peripherals should be in corresponding .c and .h files
//      that are then included below
//
//
//#############################################################################

// Included peripheral files
#include "device.h"
#include "adc_etcm.h"
#include "dac_etcm.h"
#include "device.h"
#include "CPUTimer_etcm.h"
#include "uart_etcm.h"
#include "gpio_etcm.h"
#include "interrupt_etcm.h"
#include "epwm_etcm.h"
#include <stdlib.h>
#include "string.h"

typedef enum {INPUT, OUTPUT} testState;

//Globals
int CPUTimer0Count = 0;
char outButtonStatus = 0x0000;
char resetButtonStatus = 0x0000;

//Function Prototypes.
void init(void);
void run(void);
void initLookup(void);
__interrupt void cpuTimer0ISR(void);
void getInput(char* prompt, char* in, unsigned int length);
bool Timer0Expired();

void main(void)
{
    init();
    run();

    return;
}

void run(void)
{
    char read[3];
    char write[10];
    while (1)
    {
        //Resetting Values
        uint16_t suspensionTravel[2] = {0,0};
        uint16_t pSwitches[3] = {0,0,0};
        uint16_t bSwitches[2] = {0,0};
        uint16_t wSensors[2] = {0,0};
        uint16_t tSwitch = 0;

        //Read in values from serial
        SCIread(SCI_DEBUG_BASE, (uint16_t *) read, 3); //Front Wheel Speed
        wSensors[0] = atoi(read);
        SCIread(SCI_DEBUG_BASE, (uint16_t *) read, 3); //Rear Wheel Speed
        wSensors[1] = atoi(read);
        SCIread(SCI_DEBUG_BASE, (uint16_t *) read, 3); //Front Susp Travel
        suspensionTravel[0] = atoi(read);
        SCIread(SCI_DEBUG_BASE, (uint16_t *) read, 3); //Rear Susp Travel
        suspensionTravel[1] = atoi(read);

        char oneBit[1];
        SCIread(SCI_DEBUG_BASE, (uint16_t *) oneBit, 1); //Front Brake
        bSwitches[0] = atoi(oneBit);
        SCIread(SCI_DEBUG_BASE, (uint16_t *) oneBit, 1); //Back Brake
        bSwitches[1] = atoi(oneBit);
        SCIread(SCI_DEBUG_BASE, (uint16_t *) oneBit, 1); //Profile 1
        pSwitches[0] = atoi(oneBit);
        SCIread(SCI_DEBUG_BASE, (uint16_t *) oneBit, 1); //Profile 2
        pSwitches[1] = atoi(oneBit);
        SCIread(SCI_DEBUG_BASE, (uint16_t *) oneBit, 1); //Profile 3
        pSwitches[2] = atoi(oneBit);
        SCIread(SCI_DEBUG_BASE, (uint16_t *) oneBit, 1); //Throttle
        tSwitch = atoi(oneBit);

        //Set Values on Pins
        GPIO_writePin(67, pSwitches[0]); //Profile Switches (J1 5,6,7)
        GPIO_writePin(111, pSwitches[1]);
        GPIO_writePin(60, pSwitches[2]);

        GPIO_writePin(61, bSwitches[0]); //Brake Switches (J2 19, 18)
        GPIO_writePin(123, bSwitches[1]);

        GPIO_writePin(122, tSwitch); //Throttle Closed Switch (J2 17)

        setDACOutputVoltage(DACA_BASE, (150 - suspensionTravel[0]) * (3.0/150)); //Suspension Travel (J3 30)
        setDACOutputVoltage(DACB_BASE, (150 - suspensionTravel[1]) * (3.0/150)); //Suspension Travel (J7 70)

        //Calculate PWM Signal
        initEPWM1(2*(wSensors[0]*4*40)); //Front Wheel Speed (J4 40)
        initEPWM2(2*(wSensors[1]*4*40)); //Rear Wheel Speed  (J4 38)

        unsigned int motor_request = getADCVal(); //(J3 29)

        /* convert ADC val to char */
        SCIwrite(SCI_DEBUG_BASE, (uint16_t *) write, 10);

    }
}
//Initialize, runs all initialization functions
void init(void)
{
    Device_init();
    Device_initGPIO();
    initInterrupt();
    initLookup();

    initGPIO();

    initADC();
    initADCSOC();

    initSCI();

    initDAC(DACA_BASE);
    initDAC(DACB_BASE);

    addInterrupt(&cpuTimer0ISR, INT_TIMER0);
    initTimer(CPUTIMER0_BASE, 5000, 0);
    enableInterrupts();
    startTimer(CPUTIMER0_BASE);
    EINT;
    ERTM;
}
//Initialize lookup tables
void initLookup(void)
{
    // Open file containing tables

    // Load tables into ROM
}

__interrupt void cpuTimer0ISR(void)
{
    CPUTimer0Count++;
    sampleGPIO(&outButtonStatus);
    sampleGPIO(&resetButtonStatus);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

void getInput(char* prompt, char* in, unsigned int length){
    char* newline = "\r\n";
    SCIwrite(SCI_DEBUG_BASE, (uint16_t *) prompt, strlen(prompt));
    SCIwrite(SCI_DEBUG_BASE, (uint16_t *) in, length);
    SCIwrite(SCI_DEBUG_BASE, (uint16_t *) newline, strlen(newline));
}


//
// End of File
//
