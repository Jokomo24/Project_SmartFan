/*
 * pwdfan.h: Final Project, Spring 2019
 *
 *  Created by Joe Komosa
 */

#ifndef PWMFAN_H_
#define PWMFAN_H_

#include <stdint.h>

/*
 * Initialize the timer PWM functions connected to the fan
 */
void fanPwmInit();

/*
 * Set the parameters for the fan
 */
void fanPwmSet(int pulsePeriod, int maxPulseWidth);

#endif /* PWDFAN_H_ */
