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
bool Timer0Expired();

void main(void)
{
    init();
    run();

    return;
}

void run(void)
{
    //ADCINA0/DACOUTA - J3 30
    testState t = INPUT;
    buttonState outButton = UP;
    buttonState resetButton = UP;
    char read[100];
    char* newline = "\r\n";
    while (1)
    {
        char* input = "Please enter a wheel speed (3 Digits):\t";
        SCIwrite(SCI_DEBUG_BASE, (uint16_t *) input, strlen(input));
        SCIread(SCI_DEBUG_BASE, (uint16_t *) read, 3);
        SCIwrite(SCI_DEBUG_BASE, (uint16_t *) read, 3);
        SCIwrite(SCI_DEBUG_BASE, (uint16_t *) newline, strlen(newline));
        int bSwitch = atoi(read);
        //Should tactile buttons be used for Brake Switches, Profile Switches, and Throttle Closed Switch?
        //Wheel Speed sensor 2 pwm signals or just one?
        //I just need to send data that mimics the IMU not know how to use it right?
        //What measures suspension travel?
        //Whats the plan for a UI

        uint16_t suspensionTravel[2] = {0,0};
        uint16_t pSwitches[3] = {0,0,0};
        uint16_t bSwitches[2] = {0,0};
        uint16_t wSensors[2] = {0,0};
        uint16_t tSwitch = 0;

        buttonStateMachine(resetButtonStatus, &resetButton);
        buttonStateMachine(outButtonStatus, &outButton);

        switch(t)
        {
        case INPUT:
            if (outButton == UP)
            {
                GPIO_writePin(67, pSwitches[0]); //Profile Switches (J1 5,6,7)
                GPIO_writePin(111, pSwitches[1]);
                GPIO_writePin(60, pSwitches[2]);

                GPIO_writePin(61, bSwitches[0]); //Brake Switches (J2 19, 18)
                GPIO_writePin(123, bSwitches[1]);

                GPIO_writePin(122, tSwitch); //Throttle Closed Switch (J2 17)

                float request = (150 - suspensionTravel[0]) * (3.0/150);

                setDACOutputVoltage(request); //ACTING AS SUSPENSION TRAVEL SENSOR HERE
                //setDACOutputVoltage((150 - suspensionTravel[1]) * (5/150)); //ACTING AS SUSPENSION TRAVEL SENSOR HERE

                //Calculate PWM Signal
                //setCounterCompareAValue1();
                //setCounterCompareAValue2();

                t = OUTPUT;
            }
            break;
        case OUTPUT:
            //Pull dac value from bike and verify with the inputs
            //If it's right do something, if it's wrong do something
            if (resetButton == DOWN)
            {
                //Clear outputs from test
                t = INPUT;
            }
            break;
        }
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

    initEPWM1();
    initEPWM2();

    initSCI();

    initDAC();

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
//
// End of File
//
