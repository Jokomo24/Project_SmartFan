/*
 * motion.c
 *
 *  Created on: Feb 19, 2019
 *      Author: Joseph Komosa
 */

#include <stdint.h>
#include <stdbool.h>
#include <inc/hw_memmap.h>
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>
#include "motion.h"

// Pin usage: Grove base port J16, Tiva C PC4 (Port C, Pin 4)
#define MOTION_PERIPH   SYSCTL_PERIPH_GPIOC
#define MOTION_PORT     GPIO_PORTC_BASE
#define MOTION_PIN      GPIO_PIN_4

// Initialize the motion sensor
void motionInit()
{
    // Enable the port peripheral used by the motion sensor
    SysCtlPeripheralEnable(MOTION_PERIPH);

    // Configure the pin as input
    GPIOPinTypeGPIOInput(MOTION_PORT, MOTION_PIN);

}
