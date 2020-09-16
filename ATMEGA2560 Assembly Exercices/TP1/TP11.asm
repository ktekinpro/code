; TP11
; chenillard automatique - sens horaire - mode auto


.equ DDRA = 0x01
.equ PORTA = 0x02
.equ PINB = 0x03
.equ DDRB = 0x04
.equ PORTB = 0x05

.def cpt = R20
.def null = R21


.ORG 0x0000
	jmp debut
	
.ORG 0x0080

debut: 
	DDRA@IO <- 0xFF ; PORTA en sortie
	DDRB@IO <- 0xFF ; PORTB en sortie
	
chenillard:
	cpt <- 0x01
	call cheminA
	
	cpt <- 0x80
	call cheminB
	
	jmp chenillard	


cheminA:
	PORTA@IO <- cpt
	
	call pause
	
	lsl cpt
	si cpt != 0 alors saut cheminA

	PORTA@IO <- 0
	RET
	
	
cheminB:
	PORTB@IO <- cpt
	
	call pause
	
	lsr cpt
	si cpt != 0 alors saut cheminB

	PORTB@IO <- 0
	RET
	

;500k cycles pour temporiser (énoncé)
pause:
	LDI R24, 8
tempo:
	SUBI R22, 1
	SBCI R23, 0
	SBCI R24, 0
	BRCC tempo
	RET 
