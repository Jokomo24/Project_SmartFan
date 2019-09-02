/*
 * ras.c
 *
 *  Created on: Mar 5, 2019
 *      Author: Joseph Komosa
 */

#include <stdint.h>
#include <stdbool.h>
#include <inc/hw_memmap.h>
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>
#include "ras.h"
#include "driverlib/adc.h"

// Pin usage: Grove base port J5, Tiva C PD0 (Port D, Pin 0)
#define RAS_PERIPH   SYSCTL_PERIPH_GPIOD

void rasInit() {

    // Enable the ADC0 peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    // Enable the ADC1 peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);

    // Configure ADC0, sequencer #0
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
    // Configure ADC1, sequencer #1
    ADCSequenceConfigure(ADC1_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);

    // Configure RAS 1 sequencer, can have multiple steps
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH7);

    // Configure RAS 2 sequencer
    ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_IE | ADC_CTL_CH6 | ADC_CTL_END );

    // Configure Temperature Sensor sequencer
    ADCSequenceStepConfigure(ADC1_BASE, 0, 0, ADC_CTL_IE | ADC_CTL_TS | ADC_CTL_END );

    // Enable the ADC sequencer
    ADCSequenceEnable(ADC0_BASE, 0);
    ADCSequenceEnable(ADC1_BASE, 0);

    // Clear the ADC interrupt
    ADCIntClear(ADC0_BASE, 0);
    ADCIntClear(ADC1_BASE, 0);
}
