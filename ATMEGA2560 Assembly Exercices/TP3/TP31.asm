; TP31 : afficher sur 2 afficheurs 7 segments la valeur de la température (analogique)
; une sonde analogique en ADC0 et 2 afficheurs sur les port A (dizaine) et B (unité)

.equ PORTA = 0x02
.equ DDRA = 0x01

.equ PORTB = 0x05 
.equ DDRB = 0x04

.equ ADMUX = 0x7C
.equ ADCSRB = 0x7B
.equ ADCSRA = 0x7A
.equ ADCL = 0x78
.equ ADCH = 0x79

.org 0x0000 ; Vecteur RESET
jmp debut

.org 0x100

.def valeur=R21
.def unite=R22
.def dizaine=R23

CodeAff:
.db 0b1111110,0b0001100 ; 0,1 
.db 0b0110111,0b0011111 ; 2,3
.db 0b1001101,0b1011011 ; 4,5
.db 0b1111011,0b0001110 ; 6,7
.db 0b1111111,0b1011111 ; 8,9

debut:
	DDRA@IO <- 0x7F 
	DDRB@IO <- 0x7F 
	
	ADMUX <- 0b01100000
	ADCSRB <- 0
	ADCSRA <- 0b11100111

boucle:
	;32 ->255
	;P(val à afficher) -> X (ADC)
	;P = ADC*255/32 = ADC/8
	valeur=ADCH/8;
	dizaine=valeur/10;
	unite=valeur-dizaine*10;
	
	PORTA@IO <- codeAff@ROM[dizaine];
	PORTB@IO <- codeAff@ROM[unite];

jmp boucle



