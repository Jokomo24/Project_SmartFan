; To include names declared in C
                .cdecls "stdint.h","stdbool.h","inc/hw_memmap.h","driverlib/pin_map.h","driverlib/sysctl.h","inc/hw_gpio.h","driverlib/gpio.h","driverlib/adc.h","ras.h"


                .text
                .global tempRead

; GPIO peripheral, D port base and pin mask value for RAS 1 & 2
; Signal pin is connected to PD0.
TEMP_PORT	.field  ADC1_BASE

tempRead
			PUSH	{r0, LR}			; Pushes LR and rasArray address parameter to stack

			LDR		r0, TEMP_PORT		; Funtion parameter 1
			MOV		r1, #0				; Funtion parameter 2
			BL		ADCIntClear			; Calls Function

			LDR		r0, TEMP_PORT		; Funtion parameter 1
			MOV		r1, #0				; Funtion parameter 2
			BL		ADCProcessorTrigger ; Calls Function

while_loop	LDR		r0, TEMP_PORT		; Funtion parameter 1
			MOV		r1, #0				; Funtion parameter 2
			MOV		r2, #0				; Funtion parameter 3
			BL		ADCIntStatus		; Calls Function: r0 holds the return value
			CMP		r0, #0				; compare r0 and 0
			BEQ		while_loop			; repeat loop if r0 == 0


			POP		{r0}				; Pops rasArray address into r0
			MOV		r3, r0				; Moves rasArray address into r3

			LDR		r0, TEMP_PORT		; Funtion parameter 1
			MOV		r1, #0				; Funtion parameter 2
			MOV		r2, r3				; Funtion parameter 3: Moves rasArray address into r2 and sends as destination address for function
			BL		ADCSequenceDataGet	; Calls Function

			POP		{PC}
