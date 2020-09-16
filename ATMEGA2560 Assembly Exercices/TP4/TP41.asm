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
	
	; initialisation du timer
	OCR0A@IO <- 80
	TCCR0A@IO <- 0x02
	TCCR0B@IO <- 0x05
	TIMSK0 <- 0x02

	;initialisation des variables
	pos <- 0
	centaine <- 0
	dizaine <- 0
	unite <- 0

	sei
	
boucle:
	SLEEP
	saut boucle

irqRX:
	car <- UDR0
	centaine <- car/100
	dizaine <- car/10-centaine*10
	unite <- car - centaine*100 - dizaine*10
	reti
	
	
int_timer0:
	si pos==0 alors saut itunite
	si pos==1 alors saut itdizaine
	si pos==2 alors saut itcentaine
	
itunite:
	PORTC@IO <- 0x20
	PORTA@IO <- afficheur@ROM[unite]
	pos <- 1
	
	; test touche #
	PORTC@IO <- 0x21 
		si PORTC@IO==PINC@IO alors UDR0 <- 35

	; test touche 9
	PORTC@IO <- 0x22
		si PORTC@IO==PINC@IO alors UDR0 <- '9'
	
	; test touche 6
	PORTC@IO <- 0x24
		si PORTC@IO==PINC@IO alors UDR0 <- '6'
	
	; test touche 3	
	PORTC@IO <- 0x28
		si PORTC@IO==PINC@IO alors UDR0 <-  '3'
	reti
	
itdizaine:
	PORTC@IO <- 0x40  ; on acitve le 2eme afficheur
	PORTA@IO <- afficheur@ROM[dizaine]
	pos <- 2
	
	; test touche 0
	PORTC@IO <- 0x41 
		si PORTC@IO==PINC@IO alors UDR0 <- '0'
	
	; test touche 8
	PORTC@IO <- 0x42
		si PORTC@IO==PINC@IO alors UDR0 <- '8'
		
	; test touche 5 
	PORTC@IO <- 0x44
		si PORTC@IO==PINC@IO alors UDR0 <- '5'
	
	; test touche 2
	PORTC@IO <- 0x48
		si PORTC@IO==PINC@IO alors UDR0 <- '2'
	reti
	
itcentaine: 
	PORTC@IO <- 0x80 ; on acitve le 3eme afficheur
	PORTA@IO <- afficheur@ROM[centaine]
	pos <- 0
	
	; test touche *
	PORTC@IO<-0x81 
		si PORTC@IO==PINC@IO alors UDR0 <- 42
		
	; test touche 7	
	PORTC@IO<-0x82
		si PORTC@IO==PINC@IO alors UDR0 <- '7'
	
	; test touche 4	
	PORTC@IO<-0x84
		si PORTC@IO==PINC@IO alors UDR0 <- '4'
	
	; test touche 1
	PORTC@IO<-0x88
		si PORTC@IO==PINC@IO alors UDR0 <- '1'
	reti
