/*
 * ECE 266, Spring 2019, Final Project
 *
 * main.c
 * Created by Joe Komosa
 */

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>
#include <pwmfan.h>
#include "launchpad.h"
#include "motion.h"
#include "ras.h"
#include "driverlib/timer.h"
#include "driverlib/adc.c"
#include "seg7.h"
#include "seg7digit.h"
                                                                            // The initial state of the 7-segment display
seg7Display_t seg7Display = {
    10, 10, 10, 10, 10                                                      // 10 = digit off
};

typedef enum {On, Off} OnOff_t;                                             // ON/Off state type

static OnOff_t motionState = Off;                                           // Motion state

static OnOff_t tempState = Off;                                             // Temperature state

static OnOff_t fanState = Off;                                              // Fan state

#define FAN_PWM_PERIOD          100                                         // fan-related constants
#define FAN_MAX_PULSE_WIDTH     100
                                                                            // Global Variables
uint32_t rasArray[2] = {0,0};
uint32_t tempVal[1] = {0};
uint32_t timeTrue = 0;
uint32_t tempF = 0;
uint32_t tempSet = 99;                                                      // User temperature setting
double   ras1Val = 0.0;                                                     // Fan Speed
uint32_t ras2Val = 0;                                                       // User selected temperature
uint8_t  i = 0;                                                             // Digits
uint8_t  j = 0;
uint8_t  k = 0;
uint8_t  l = 0;
uint8_t button1State = 1;                                                   // Switch 1 button select
uint8_t button2State = 0;                                                   // Switch 2 button select
uint8_t dispValue = 0;

typedef struct
{
    int pwmPeriod;                                                          // PWM period for fan
    int maxPulseWidth;                                                      // Maximum pulse width
} FanState_t;

static FanState_t fan =
    {FAN_PWM_PERIOD /* PWM Period */, FAN_MAX_PULSE_WIDTH/* Multiply factor */};

//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

void seg7(uint32_t time) {

    if(button2State == 0){                                                  // If Motion State is On

        tempRead(tempVal);
        tempF = (147.5 - ((75.0 * (3.3/*VREFP*/ - 0/*VREFN*/)
                                * tempVal[0]) / 4096.0)) * (9 / 5.0) + 32;  // Conversion temperature formula: Voltage -> Celsius -> Fahrenheit
        dispValue = tempF;
        uprintf("temperature in F: %d\n\r", tempF);
    }

    else if(tempState == On && button2State == 1){                          // If Temperature State is On and SW2 is pushed

        rasRead(rasArray);
        dispValue = ((double) abs(rasArray[1] - 4125) / 4095.0) * 99.0;     // Displays 0-99
        tempSet = dispValue;                                                // Saves user selected temperature
    }
                                                                            // Assigns calculated values of range to seg 7 digits 1-4
    i = 12;                                                                 // displays F for fahrenheit
    j = 11;                                                                 // displays degree symbol
    k = dispValue % 10;                                                     // Digit 3 value - Returns Remainder
    l = dispValue / 10;                                                     // Left most digit 4 value - Returns Quotient

    if(k == 0 && l == 0){                                                   // Turn off leading zeros
        k = 10;                                                             // 10 = digit off
    }
    if(l == 0){
        l = 10;
    }

    seg7Display_t seg7Display = {                                           // Update display
        i, j, k, l, 0
    };
    seg7DigitUpdate(&seg7Display);                                          // schedule the next callback
    schdCallback(seg7, time + 250);
}

//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

void ras1(uint32_t time) {                                                  // Call RAS assembly function and schedule callback

    if(fanState == On){
                                                                            // Calls ras assembly function
        rasRead(rasArray);

        ras1Val = ((double) (4095.0 - rasArray[0]) / 4096.0);
        double ratio = (2200 + (1895 * ras1Val)) / 4096.0;

        fan.maxPulseWidth = FAN_MAX_PULSE_WIDTH * ratio/* Multiply factor */;

                                                                            // Set the PWM parameters for the fan
        fanPwmSet(fan.pwmPeriod, fan.maxPulseWidth);
    }
    else {

        fan.maxPulseWidth = 0;
                                                                            // Set the PWM parameters for the fan
        fanPwmSet(fan.pwmPeriod, fan.maxPulseWidth);
    }                                                                       // schedule the next callback
    schdCallback(ras1, time + 20);
}

//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

void callbackCheckTemperature(uint32_t time) {

    uint32_t delay = 10;                                                    // the default delay for the next checking

    if (tempState == On) {

        if(tempF >= tempSet) {                                              // Activate temperature sensor fan
            fanState = On;
            ledTurnOnOff(false /* red */, true /* blue */, true /* green */);
        }
        else {
            fanState = Off;
            ledTurnOnOff(false /* red */, true /* blue */, false /* green */);
        }
    }
    delay = 250;                                                            // schedule the next callback
    schdCallback(callbackCheckTemperature, time + delay);
}

//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

void callbackCheckMotionSensor(uint32_t time) {

    uint32_t delay = 10;                                                    // the default delay for the next checking

    if (motionState == On) {                                                // If statement ensures that fan stays off until SW1 is pushed
                                                                            // Read motion sensor
        int motionDet = motionRead();

        switch (motionDet)                                                  // read the motion sensor
            {
            case 16:                                                        // Detection is true. Turn on the system and the fan
                uprintf("Motion Detected!\n\r");
                ledTurnOnOff(false /* red */, false /* blue */, true /* green */);
                fanState = On;
                timeTrue = sysTimeGet() + 3000;
                break;

            case 0:                                                         // Detection is false. Turn off the fan
                if(sysTimeGet() > timeTrue) {                               // Timetrue ensures constant activation if there is continual motion
                    uprintf("No Motion Detected.\n\r");
                    fanState = Off;
                    ledTurnOnOff(true /* red */, false /* blue */, false /* green */);
                }
                break;
            }
    }
    delay = 250;                                                            // schedule the next callback
    schdCallback(callbackCheckMotionSensor, time + delay);
}

//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

void callbackCheckPushButton(uint32_t time)                                 // Callback function for checking the push buttons
{
    uint32_t delay = 10;                                                    // the default delay for the next checking

    int code = pbRead();                                                    // read the pushbutton
    switch (code)
    {
    case 1:                                                                 // SW1: Turn on/off the system and the fan and select setting
       if(button1State == 1) {                                              // Activate motion sensor fan
           motionState = On;
           ledTurnOnOff(false /* red */, false /* blue */, true /* green */);
           button1State++;
       }
       else if(button1State == 2){                                          // Activate check temperature setting/state
           motionState = Off;
           tempState = On;
           callbackCheckTemperature(10);
           ledTurnOnOff(false /* red */, true /* blue */, false /* green */);
           button1State++;
       }
       else {                                                               // Turn off the system and the fan
           motionState = Off;
           tempState = Off;
           ledTurnOnOff(false /* red */, false /* blue */, false /* green */);
           button1State = 1;
           fanState = Off;
       }
        delay = 250;
        break;

    case 2:                                                                 // SW2: Turn on/off the user temperature select setting

        if(tempState == On && button2State == 0) {                          // Activate  temperature select
            button2State = 1;
        }
        else {
            button2State = 0;
        }
        delay = 250;
        break;
    }                                                                           // schedule the next callback
    schdCallback(callbackCheckPushButton, time + delay);
}

//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

/*
 * The main function
 */
int main(void){

    lpInit();
    motionInit();
    rasInit();
    fanPwmInit();
    seg7Init();

    // Print out a start message
    uprintf("%s\n\r", "ECE 266 Final Project by Joe Komosa");

    // Schedule the first callback events
    schdCallback(callbackCheckPushButton, 1002);
    schdCallback(callbackCheckMotionSensor, 250);
    schdCallback(ras1, 252);
    schdCallback(seg7, 254);
    // Turn on the green sub-LED--OFF
    ledTurnOnOff(false /* red */, false /* blue */, false /* green */);

    // Run the callback scheduler
    while (true)
    {
        schdExecute();
    }
}
