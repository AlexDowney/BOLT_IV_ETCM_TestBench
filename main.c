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
#include "CPUTimer_etcm.h"
#include "uart_etcm.h"
#include "gpio.h"
#include "interrupt_etcm.h"
#include "epwm_etcm.h"

typedef enum {UP, DOWN} udState;
typedef enum {INPUT, OUTPUT} testState;

//Globals
int CPUTimer0Count = 0;
unsigned int period1 = 0;
unsigned int period2 = 0xEEEE;
udState u = UP;

//Function Prototypes.
void init(void);
void run(void);
void initLookup(void);
void requestTorque(int torque);
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
    while (1)
    {
        //Should tactile buttons be used for Brake Switches, Profile Switches, and Throttle Closed Switch?
        //Wheel Speed sensor 2 pwm signals or just one?
        //I just need to send data that mimics the IMU not know how to use it right?
        //What measures suspension travel?
        //Whats the plan for a UI

        char a = 'a';
        unsigned int c = 0;
        for (c = 0; c < 26; c++)
        {
            SCIsend(a + c);
            char b = SCIreceive();

            if (b == a + c)
            {
                GPIO_writePin(0, 0);
                GPIO_writePin(2, 1);
            }
            else
            {
                GPIO_writePin(0, 1);
                GPIO_writePin(2, 0);
                c = 0;
            }
        }

        switch(t)
        {
        case INPUT:
            /*If tactile button that controls testing is pressed then go to output
             *
             */
            break;
        case OUTPUT:
            //Pull dac value from bike and verify with the inputs
            //If it's right do something, if it's wrong do something
            t = INPUT;
            break;
        }
    }
}
//Initialize, runs all initialization functions
void init(void)
{
    Device_init();
    Device_initGPIO();
    //    GPIO_setPinConfig(GPIO_0_EPWM1A);
    //    GPIO_setPinConfig(GPIO_2_EPWM2A);
    GPIO_setDirectionMode(0, GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(2, GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(61, GPIO_DIR_MODE_OUT);
    GPIO_writePin(61, 0);
    initInterrupt();
    initLookup();
    initADC();
    initEPWM1(); //Sets it to a 50/50 duty cycle for now
    initEPWM2();
    initADCSOC();
    initSCI();

    addInterrupt(&cpuTimer0ISR, INT_TIMER0);
    initTimer(CPUTIMER0_BASE, 5000, 0);
    enableInterrupts();
    startTimer(CPUTIMER0_BASE);
    EINT;
    ERTM;
    GPIO_writePin(61,1);
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
    GPIO_togglePin(61);
    switch (u)
    {
    case UP:
        period1 += 1;
        period2 -= 1;
        if (period1 == 0xEEEE)
        {
            u = DOWN;
        }
        break;
    case DOWN:
        period1 -= 1;
        period2 += 1;
        if (period1 == 0)
        {
            u = UP;
        }
        break;
    }
    setCounterCompareAValue1(period1);
    setCounterCompareAValue2(period2);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

//
// End of File
//
