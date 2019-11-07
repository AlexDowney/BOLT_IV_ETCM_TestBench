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
#include "sci_etcm.h"
#include "gpio.h"
#include "interrupt_etcm.h"

//Globals
int CPUTimer0Count = 0;

typedef enum {LIGHTON, LIGHTOFF} lightState;

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
    int torque_request = 0; // likely to change type
    lightState l = LIGHTON;
    while (1)
    {
        uint16_t a = getADCVal();
        switch(l)
        {
        case LIGHTOFF:
            if (a > 3000)
            {
                l = LIGHTON;
                GPIO_writePin(29, 1);
            }
        case LIGHTON:
            if (a < 1500)
            {
                l = LIGHTOFF;
                GPIO_writePin(29, 0);
            }
        }


        // Pull in sensor data to local variables
        // This will use getters inside the peripheral .h/.c files

        // Follow lookup table logic
        // Specific logic TBD
        // Carry out any calculations

        // Send torque request to motor
        requestTorque(torque_request);
    }
}
//Initialize, runs all initialization functions
void init(void)
{
    Device_init();
    Device_initGPIO();
    GPIO_setDirectionMode(29, GPIO_DIR_MODE_OUT);
    GPIO_writePin(29, 1);
    initInterrupt();
    initLookup();
    initADC();
    initEPWM();
    initADCSOC();

    addInterrupt(&cpuTimer0ISR, INT_TIMER0);
    initTimer(CPUTIMER0_BASE, 10000000, 0);
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
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

//
// End of File
//
