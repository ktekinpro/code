.DSEG ; mémoire de donnée

.org 0x200
centaine: .byte 1
dizaine: .byte 1
unite: .byte 1
pos: .byte 1
car: .byte 1




.CSEG ; mémoire de programme

.equ RAMEND=0x21FF
.equ SPH=0x3E
.equ SPL=0x3D

.equ DDRC=0x07
.equ PORTC=0x08
.equ PINC=0x06
.equ DDRA=0x01
.equ PORTA=0x02

.equ UBRRL0=0x00C4
.equ UCSR0A=0x00C0
.equ UCSR0B=0x00C1
.equ UCSR0C=0x00C2
.equ UDR0=0x00C6

.equ OCR0A=0x27
.equ TCCR0A=0x24
.equ TCCR0B=0x25
.equ TIMSK0=0x6E

.equ ADMUX = 0x7C
.equ ADCSRB = 0x7B
.equ ADCSRA = 0x7A
.equ ADCL = 0x78
.equ ADCH = 0x79





.org 0x0000
jmp debut

.org 0x002A
jmp int_timer0

.org 0x0032
jmp irqRX


.org 0x0080

afficheur: .DB 0x7E,0x0C,0x37,0x1F,0x4D,0x5B,0x7B,0x0E,0x7F,0x5F

debut:
	;initialisation pointeur de pile
	ldi r28, low(RAMEND)
	ldi r29, high(RAMEND)
	out SPL, r28
	out SPH, r29

	;initialisation des ports
	DDRA@IO <- 0xFF
	DDRC@IO <- 0xF0
	
	;initialisation de la lisaon série
	UBRRL0 <- 103
	UCSR0A <- 0b00000000
	UCSR0B <- 0b10011000
	UCSR0C <- 0b00100110
	
	;initialisation des variables
	pos <- 0
	centaine <- 0
	dizaine <- 0
	unite <- 0
	
	
	;initialisation de la conversion numérique analogique
	ADMUX <- 0b01100000
	ADCSRB <- 0b00000000
	ADCSRA <- 0b11110111

	sei
	
boucle:
	SLEEP
	saut boucle

	
iqadc: 
	car <- ADCH
	centaine <- car/100
	dizaine <- car/10-centaine*10
	unite <- car - centaine*100 - dizaine*10
	
	; initialisation du timer
	OCR0A@IO <- 80
	TCCR0A@IO <- 0x02
	TCCR0B@IO <- 0x05
	TIMSK0 <- 0x02
	cpt1<-0
	cpt2<-0
	
	reti
	
int_timer0:
	si cpt1 <200 alors cpt1 = cpt1 + 1
	si cpt1>=200 alors jmp compteur2
	compteur2 : 
		cpt2 = cp2+1
		si cpt2 <20 alors jmp compteur2
		si cpt2>=20 alors jmp suite
	
	suite
	si cpt2 ==20 alors si pos==0 alors saut itunite
	si cpt2 ==20 alors si pos==1 alors saut itdizaine
	si cpt2 ==20 alors si pos==2 alors saut itcentaine
	
itunite:
	PORTC@IO <- 0x20
	PORTA@IO <- 0b01110010
	pos <- 1
	reti
	
itdizaine:
	PORTC@IO <- 0x40 
	PORTA@IO <- afficheur@ROM[unite]
	pos <- 2
	reti
	
itcentaine: 
	PORTC@IO <- 0x80
	PORTA@IO <- afficheur@ROM[dizaine]
	pos <- 0
	reti
