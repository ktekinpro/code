;TP3 : conversion analogique/numérique
; TP30 : affiche la valeur analogique lue  

.equ PORTA = 0x02
.equ DDRA = 0x01

;pour le pointeur de pile
.equ RAMEND = 0x21FF
.equ SPH = 0x3E
.equ SPL = 0x3D

.equ SREG = 0x3F

.equ ADMUX = 0x7C
.equ ADCSRB = 0x7B
.equ ADCSRA = 0x7A
.equ ADCL = 0x78
.equ ADCH = 0x79

.org 0x0000 
	jmp debut

.org 0x0080

CodeAff:
.db 0b1111110,0b0001100 ; 0,1
.db 0b0110111,0b0011111 ; 2,3
.db 0b1001101,0b1011011 ; 4,5
.db 0b1111011,0b0001110 ; 6,7
.db 0b1111111,0b1011111 ; 8,9

debut:
	;initialisation du pointeur de pile
	ldi r28, low(RAMEND)
	ldi r29, high(RAMEND)
	out SPL, r28
	out SPH, r29
	
	DDRA@IO <- 0xFF 
	
	ADMUX <- 0b01100000
	ADCSRB <- 0
	ADCSRA <- 0b11100111
boucle:
	PORTA@IO <- codeAff@ROM[ADCH/51]
	jmp boucle
