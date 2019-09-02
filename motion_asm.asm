;
; Lab 4, ECE 266, Spring 2019
;
; Assembly functions related to motion sensor operations.
;

; To include names declared in C
                .cdecls "stdint.h","stdbool.h","inc/hw_memmap.h","driverlib/pin_map.h","driverlib/sysctl.h","driverlib/gpio.h","motion.h"

                .text
                .global motionRead

; GPIO peripheral, port base and pin mask value for the motion sensor
; The motion sensor is assumed to be connected to J16, and then the motion sensor's
; output pin is connected to PC4.
MOTION_PORT     .field  GPIO_PORTC_BASE
MOTION_PIN      .equ    GPIO_PIN_4

;
; void motionRead(): Recieve data from motion sensor. It calls GPIOPinWrite() to write 1 or 0 to the signal pin.
;
motionRead      PUSH    {LR}            ; save the return address

                ; Write value of motion sensor to GPIO Pin:
                ; GPIOPinWrite(MOTION_PORT, MOTION_PIN)
                LDR     r0, MOTION_PORT
                MOV     r1, #MOTION_PIN
                BL      GPIOPinRead
                POP     {PC}            ; return
