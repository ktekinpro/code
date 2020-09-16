;TP22
;affichage sur 3 ports : |A|B|C|
;compteur 1 seconde, jusqu'à 255

.equ DDRA = 0x01
.equ PORTA = 0x02
.equ PINA = 0x00
.equ DDRB = 0x04
.equ PORTB = 0x05
.equ DDRC = 0x07
.equ PORTC = 0x08
.equ WDTCSR = 0x60


.def cpt = r20
.def valeur = r21
.def unite = r22
.def dizaine = r23
.def centaine = r24
.def deja_fait = r25

.org 0x0000
      jmp debut

.org 0x0018
	jmp topHorloge
      
.org 0x0080

afficheur:
	.DB 0x7E, 0x0C, 0x37, 0x1F, 0x4D, 0x5B, 0x7B, 0x0E, 0x7F, 0x5F
	;	0	  1 	2	  3	    4	  5	    6	  7		8	  9
	

debut:
	DDRA@IO <- 0xff    
	DDRB@IO <- 0xff 
	DDRC@IO <- 0xff 
	

   	WDTCSR <- 0x10
   	WDTCSR <- 0b01000000
   	cpt@REG <- 0
   	valeur@REG <- 0
    	unite@REG <- 0
	dizaine@REG <- 0
	centaine@REG <- 0
	deja_fait@REG <- 0
    sei

boucle:
	SLEEP
	JMP boucle

	 
topHorloge:
	si cpt<63 alors saut finSi
		; cpt = 63 * 16ms = 1.008s
		; incrément tout les 1 sec
		INC valeur
		cpt<-0
	finSi:	
	
	INC cpt
	centaine=valeur/100
	dizaine=valeur/10 - 10*centaine
	unite=valeur-dizaine*10-centaine*100
	
	si deja_fait=1 saut dizaine1
	si deja_fait=2 saut centaine1
	
	PORTC@IO <- afficheur@ROM[unite@REG]
	deja_fait=1
	RETI

dizaine1:
	PORTB@IO <- afficheur@ROM[dizaine@REG]
	deja_fait=2
	RETI


centaine1:
	PORTA@IO <- afficheur@ROM[centaine@REG]
	deja_fait=3
	RETI
