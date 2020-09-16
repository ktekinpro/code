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

.org 0x0080
debut:
	;f=16MHz et 9600Bd	
	UBRRL0 <- 103
	UCSR0A <- 0b00000000
	UCSR0B <- 0b00011000
	UCSR0C <- 0b00000110
	
	car@REG <- 0
	
boucle:
	;0x80 : flag r�ception compl�te, on charge la donn�e dans car
	si (UCSR0A & 0x80)!=0 alors car@REG<- UDR0
	
	;0x20 : flag transmission compl�te
	;ou pas de caract�re : on revient au d�but
	si ((UCSR0A & 0x20)==0 ou car@REG==0) saut boucle
	
	;envoie de la donn�e (on envoie le caract�re lu)
	UDR0<-car@REG
	;si on est entre A et Z : on continue d'afficher, si non on arr�te
	si car@REG<'A' ou car@REG>'Z' alors car@REG<-0
	
	saut boucle
	
	
