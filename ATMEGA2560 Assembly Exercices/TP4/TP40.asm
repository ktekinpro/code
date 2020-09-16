;TP4 : communication série
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

.org 0x0080
debut:
	;f=16MHz et 9600Bd	
	UBRRL0 <- 103
	UCSR0A <- 0b00000000
	UCSR0B <- 0b00011000
	UCSR0C <- 0b00000110
	
	car@REG <- 0
	
boucle:
	;0x80 : flag réception complète, on charge la donnée dans car
	si (UCSR0A & 0x80)!=0 alors car@REG<- UDR0
	
	;0x20 : flag transmission complète
	;ou pas de caractère : on revient au début
	si ((UCSR0A & 0x20)==0 ou car@REG==0) saut boucle
	
	;envoie de la donnée (on envoie le caractère lu)
	UDR0<-car@REG
	;si on est entre A et Z : on continue d'afficher, si non on arrête
	si car@REG<'A' ou car@REG>'Z' alors car@REG<-0
	
	saut boucle
	
	
