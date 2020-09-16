;TP12
;incrémentation automatique d'un afficheur 7 segments

.equ PINA = 0x00
.equ DDRA = 0x01
.equ PORTA = 0x02

; attention : le registre R21 ne doit pas être utilisé ailleur !
.def i = R21
.def ZH = R31
.def ZL = R30


.ORG 0x0000
	jmp debut

		
.ORG 0x0080

afficheur:
	.DB 0x7E, 0x0C, 0x37, 0x1F, 0x4D, 0x5B, 0x7B, 0x0E, 0x7F, 0x5F
	;	0	  1 	2	  3	    4	  5	    6	  7		8	  9
	
		
;initialisation
debut: 
	DDRA@IO <- 0x7F
	i <- 0
	call boucle

boucle:
	call afficher
	call pause
	
	i <- i+1
	si i=10 alors i<-0
	
	; loop
	jmp boucle

	
afficher:
	LDI ZH, high(afficheur << 1)
	LDI ZL, low(afficheur << 1)
	
	ZL <- ZL + i
	
	LPM r17, Z
	
	PORTA@IO <- r17
	RET

	
pause:
	LDI R24, 10
tempo:
	SUBI R22, 1
	SBCI R23, 0
	SBCI R24, 0
	BRCC tempo
	RET 
	

		
	
	
