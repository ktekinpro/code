;TP32 : Capteur de distance (sur maquette)

.equ DDRA = 0x01
.equ DDRC = 0x07
.equ PINA = 0x00
.equ PINC = 0x06
.equ PORTA = 0x02
.equ PORTC = 0x08

.equ ADMUX = 0x7C
.equ ADCSRB = 0x7B
.equ ADCSRA = 0x7A
.equ ADCL = 0x78
.equ ADCH = 0x79

.def deja_fait=R23
.def unite=R22
.def dizaine=R21
.def centaine=R20
.def valeur=R24

.equ TIMSK0 = 0x6E
.equ OCR0A = 0x27
.equ TCCR0A = 0x24
.equ TCCR0B = 0x25


.org 0x0000
	jmp init

.org 0x002A
	jmp topHorloge

.org 0x003A
	jmp charge_valeur

.org 0x0080
	afficheur: .DB 0x7E,0x0C,0x37,0x1F,0x4D,0x5B,0x7B,0x0E,0x7F,0x5F

init:
	DDRA@IO <- 0x7F
	DDRB@IO <- 0x7F
	DDRC@IO <- 0xFF
	ADMUX <- 0b01100001
	ADCSRB <- 0b00000000
	ADCSRA <- 0b11101111
	deja_fait=0
	TIMSK0 <- 0x02
	OCR0A@IO <- 50
	TCCR0A@IO <- 0x02
	TCCR0B@IO <- 0x03
	valeur<-0
	SEI

main:
	SLEEP
	jmp main

	
charge_valeur:
	 valeur <- ADCH
	 
topHorloge:
	centaine=valeur/100
	dizaine=valeur/10 - 10*centaine
	unite=valeur-dizaine*10-centaine*100
	
	si deja_fait=1 saut dizaine1
	si deja_fait=2 saut centaine1
	
	PORTC@IO <- 0b00100000
	PORTA@IO <- afficheur@ROM[unite]
	deja_fait=1
	RETI

dizaine1:
	PORTC@IO <- 0b01000000
	PORTA@IO <- afficheur@ROM[dizaine]
	deja_fait=2
	RETI


centaine1:
	PORTC@IO <- 0b10000000
	PORTA@IO <- afficheur@ROM[centaine]
	deja_fait=3
	RETI
	
