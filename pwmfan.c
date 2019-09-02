/*
 * pwmfan.c: ECE 266 Final Project, spring 2019
 *
 * This file contains the initialization function for Timer/PWM hardware used
 * with the fan.
 *
 * Created by Joe Komosa
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include <inc/hw_memmap.h>
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/timer.h>
#include <pwmfan.h>

#define WTIMER0         WTIMER0_BASE
#define PORTC           GPIO_PORTC_BASE

void fanPwmInit() {

    // Enable Timer 0
       SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER0);
       SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

       // Connect pins to those timers
       GPIOPinTypeTimer(PORTC, GPIO_PIN_5);
       GPIOPinConfigure(GPIO_PC5_WT0CCP1);

       // Select PWM for Timer 0 sub-Timer B
       TimerConfigure(WTIMER0, (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_B_PWM));

       // Invert the PWM waveform, so that the Match register value is the pulse width.
       // Otherwise, the pulse width will be (Load value) - (Match value).
       // The inversion is done by enabling output inversion on the PWM pins.
       TimerControlLevel(WTIMER0, TIMER_B, true /* output inversion */);

       // Enable the Timer 0's TimerB
       TimerEnable(WTIMER0, TIMER_B);
   }

