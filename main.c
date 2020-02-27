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
    //ADCINA0/DACOUTA - J3 30
    testState t = INPUT;
    buttonState outButton = UP;
    buttonState resetButton = UP;
    char read[5];
    while (1)
    {
        uint16_t suspensionTravel[2] = {0,0}; // {front, back}
        uint16_t pSwitches[3] = {0,0,0};
        uint16_t bSwitches[2] = {0,0};
        uint16_t wSensors[2] = {0,0};
        uint16_t tSwitch = 0;

        getInput("Please enter a wheel speed (3 Digits):\t", read, 3);
        wSensors[0] = atoi(read);
        getInput("Please enter a wheel speed (3 Digits):\t", read, 3);
        wSensors[1] = atoi(read);

        getInput("Please enter a front wheel suspension value (3 Digits):\t", read, 3);
        suspensionTravel[0] = atoi(read);
        getInput("Please enter a back wheel suspension value (3 Digits):\t", read, 3);
        suspensionTravel[1] = atoi(read);

        getInput("Please enter a break value (1 or 0):\t", read, 1);
        bSwitches[1] = atoi(read);
        getInput("Please enter a break value (1 or 0):\t", read, 1);
        bSwitches[1] = atoi(read);

        getInput("Please enter a throttle (1 or 0):\t", read, 1);
        tSwitch = atoi(read);

        //Should tactile buttons be used for Brake Switches, Profile Switches, and Throttle Closed Switch?
        //Wheel Speed sensor 2 pwm signals or just one?
        //I just need to send data that mimics the IMU not know how to use it right?
        //What measures suspension travel?
        //Whats the plan for a UI

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
        char resetCheck[5];
        char reset[5] = "reset";
        while (strcmp(resetCheck, reset) != 0)
        {
            getInput("Type reset to reset:\t", resetCheck, 5);
            int a = strcmp(resetCheck, reset);
            int b = 1;
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

void getInput(char* prompt, char* in, unsigned int length){
    char* newline = "\r\n";
    SCIwrite(SCI_DEBUG_BASE, (uint16_t *) prompt, strlen(prompt));
    SCIread(SCI_DEBUG_BASE, (uint16_t *) in, length);
    SCIwrite(SCI_DEBUG_BASE, (uint16_t *) in, length);
    SCIwrite(SCI_DEBUG_BASE, (uint16_t *) newline, strlen(newline));
}


//
// End of File
//
