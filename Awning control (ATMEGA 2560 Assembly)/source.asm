.equ DDRA = 0x01
.equ PORTA = 0x02
.equ PINA = 0x00
.equ DDRB = 0x01
.equ PINB = 0x02
.equ DDRC = 0x01
.equ PORTC = 0x02
.equ WDTCSR = 0x60

.def cpt = R20
.equ limite_vent = 255 
.equ consigne = 128


.org 0x0000	
	jmp init
.org 0x0018
	jmp topHorloge
.org 0x0100

init :
	DDRA ←0b 0000 0110 
	DDRB ←0x00
	DDRC ←0x0F
	SEI

main : 
	if (PINA & 0x01) = 0

		PORTA ← 0b00001010
		if (PINA = PORTA)
			call monte
			jmp main
		else 
			PORTA ← 0b00001100
			if (PINA = PORTA)
				call descend 
				jmp main
			else 
				jmp main

	else
		call lit_vent
		if (R22!=0 || R21 > limite_vent)
			call monte
			jmp main
		else
			call lit_lumière
			if (R22!=0 || R21 > consigne)
				call descend
				jmp main
			else
				call monte
				jmp main

monte : 
	cpt ← 0
	R19 ← 1
	WDTCSR ←0x10 
	WDTCSR ←0b0100 0110
	
	tant que  (PINB & 0x01) = 0 && R19 = 1 
		PORTC ← 0b00001001
	fin tant que
	PORTC ← 0
	WDTCSR ←0x10
	WDTCSR ←0x00 
	RET


descend : 
	cpt ← 0
	R19 ← 1
	WDTCSR ←0x10
	WDTCSR ←0b0100 0110
	tant que  (PINB & 0x02) = 0 && R19 = 1 
		PORTC ← 0b00000110 
	fin tant que
	PORTC ← 0
	WDTCSR ←0x10
	WDTCSR ←0x00
	RET


lit_vent : 
	ADMUX ← 01000001
	ADCSRB ← X0XX1000
	ADCSRA ← 11100110
encore : 
	if (ADCSRA & 0x10) = 1
		R21← ADCL
		R22 ← ADCH
	else 
		jmp encore
	RET
lit_lumière :
	ADMUX ← 01000000
	ADCSRB ← X0XX1000
	ADCSRA ← 11100110
encore : 
	if (ADCSRA & 0x10) = 1 
		R21← ADCL
		R22 ← ADCH
	else 
		jmp encore
	RET


topHorloge :
	cpt ← cpt +1
	if cpt=20 
		R19 ← 0
	RETI
