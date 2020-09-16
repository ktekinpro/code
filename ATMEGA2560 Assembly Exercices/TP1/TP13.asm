;TP13
;afficheur 7 segments : portA
;clavier 16 touches : portC (on en utilise que 10)
;affiche les chiffres tapés sur le clavier


.equ PINA = 0x00
.equ DDRA = 0x01
.equ PORTA = 0x02
.equ PINC = 0x06
.equ DDRC = 0x07
.equ PORTC = 0x08
.equ SPL = 0x3D
.equ SPH = 0x3E

;attention R20 ne doit pas être utilisé ailleurs
.def indicetouche = r23
.def resultat = r24
.def ZH = R31
.def ZL = R30

.ORG 0x0000
	jmp init
	
.ORG 0x100


afficheur:
	.DB 0x7E, 0x0C, 0x37, 0x1F, 0x4D, 0x5B, 0x7B, 0x0E, 0x7F, 0x5F, 0x00
	;	0	  1 	2	  3	    4	  5	    6	  7		8	  9		rien
clavier:
	.DB 0x41, 0x88, 0x48, 0x28, 0x84, 0x44, 0x24, 0x82, 0x42, 0x22 
	

	
init:
	DDRA@IO <- 0x7F
	DDRC@IO <- 0x0F 

	
boucle:
	call lectureclavier
	call afficher
	jmp boucle
	

	
lectureclavier:
	LDI ZL,low(clavier<<1)
	LDI ZH,high(clavier<<1)
	indicetouche <-0
	resultat <-0

	test:

		LPM R19,Z 
		PORTC@IO <- R19
		R18 <- PINC@IO

		si R18 = R19 alors saut trouve
	
		INC indicetouche
		INC ZL
		si indicetouche != 16 alors saut test
	RET
	
	
trouve:
	mov resultat, indicetouche
	RET		
	
	
afficher:
	LDI ZH, high(afficheur << 1)
	LDI ZL, low(afficheur << 1)
	ZL <- ZL + resultat
	LPM R18, Z 
	PORTA@IO <- R18
	RET
	
	
