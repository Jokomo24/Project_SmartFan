; pwmfan.asm: Assembly PWM-FAN functions
; Final Project, ECE 266, Spring 2019
; Created by Joe Komosa

; Include C header files
                .cdecls "stdint.h","stdbool.h","inc/hw_memmap.h","driverlib/sysctl.h","driverlib/gpio.h","driverlib/pin_map.h","driverlib/timer.h","pwmfan.h"

                .text
                .global fanPwmSet

; Field and any other declarations
WTIMER0         .field  WTIMER0_BASE

;
; void fanPwmSet(int pulsePeriod, int pulseWidth)
;

fanPwmSet    	PUSH    {lr, r1, r0}        ; Save r0, r1 and lr

                ; 	Set the period and duty cycle length for the fan
                ;   call TimerLoadSet(TIMER0, TIMER_B, pulsePeriod)
                ;   call TimerMatchSet(TIMER0, TIMER_B, pulsePeriodRed)
                ;   Note that r0, r1, r2 each holds a pwm_t type, with
                LDR     r0, WTIMER0                  ; r0 = TIMER2
                MOV     r1, #TIMER_B                ; r1 = TIMER_B
                LDR     r2, [sp, #0]                ; r2 = pulsePeriod (pushed r0 in stack)
                BL      TimerLoadSet                ; call TimerLoadSet

                LDR     r0, WTIMER0                  ; r0 = TIMER2
                MOV     r1, #TIMER_B                ; r1 = TIMER_B
                LDR     r2, [sp, #4]                ; r2 = pulseWidth (pushed r1 in stack)
                BL      TimerMatchSet

                ADD     sp, #8                     ; release the stack space for r3, r2, r1, r0
                POP     {pc}
