;TP4 : communication s�rie
;TP40 : introduction

.equ RAMEND=0x21FF
.equ SPH=0x3E
.equ SPL=0x3D

.equ UBRRL0=0x00C4
.equ UCSR0A=0x00C0
.equ UCSR0B=0x00C1
.equ UCSR0C=0x00C2
.equ UDR0=0x00C6

.def car=r2

.org 0x0000
jmp debut

.org 0x0032
jmp irqRX

.org 0x0034
jmp irqUDRE

.org 0x0080
debut:
	; f=16MHz et 9600Bd
	UBRRL0 <- 103
	UCSR0A <- 0b00000000
	;enable interrupt en 0x0032
	UCSR0B <- 0b10011000
	UCSR0C <- 0b00000110
	
	car@REG <- 0
	
boucle:
	sleep
	saut boucle

;une donn�e a �t� re�ue
irqRX:
	car@REG<-UDR0
	;active interrupt : donn�e pleine (irqUDRE)
	UCSR0B <- 0b10111000
	RETI

;une donn�e est pr�te � �tre envoy�e
irqUDRE:
	UDR0 <- car@REG
	;on r�active la lecture de donn�e si la condition est valid�e
	si car@REG <'A' ou car@REG > 'Z' alors UCSR0B <- 0b10011000
	
	
