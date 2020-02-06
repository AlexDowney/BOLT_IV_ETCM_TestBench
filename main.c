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
    testState t = INPUT;
    buttonState outButton = UP;
    buttonState resetButton = UP;
    while (1)
    {
        //Should tactile buttons be used for Brake Switches, Profile Switches, and Throttle Closed Switch?
        //Wheel Speed sensor 2 pwm signals or just one?
        //I just need to send data that mimics the IMU not know how to use it right?
        //What measures suspension travel?
        //Whats the plan for a UI
        char b = SCIreceive();

        if (b == 'b')
        {
            GPIO_writePin( , 1);
            GPIO_writePin( , 0);
        }
        else
        {
            GPIO_writePin( , 0);
            GPIO_writePin( , 1);
        }

        uint16_t suspensionTravel = getADCVal();
        uint16_t pSwitches[3];
        uint16_t bSwitches[2];
        uint16_t tSwitch = 0;
        buttonStateMachine(resetButtonStatus, &resetButton);
        buttonStateMachine(outButtonStatus, &outButton);
        switch(t)
        {
        case INPUT:
            if (outButton == DOWN)
            {
                /*GPIO_writePin(, pSwitches[0]); //Profile Switches
                GPIO_writePin(, pSwitches[1]);
                GPIO_writePin(, pSwitches[2]);

                GPIO_writePin(, bSwitches[0]); //Brake Switches
                GPIO_writePin(, bSwitches[1]);

                GPIO_writePin(, tSwitch); //Throttle Closed Switch*/
                requestTorque(suspensionTravel); //ACTING AS SUSPENSION TRAVEL SENSOR HERE
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
