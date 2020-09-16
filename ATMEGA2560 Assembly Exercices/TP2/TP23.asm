;TP23
;4 interruptions, 2 ports pour 2 affichages et une LED (affichage logo)

.equ DDRA =0x01
.equ DDRB = 0x04
.equ PORTA = 0x02
.equ PORTB = 0x05
.equ DDRD = 0x0A
.equ PORTD = 0x0B
.equ EIMSK = 0x1D
.equ EICRA = 0x69
.equ WDTCSR = 0x60

.def temps = r20
.def unite = r21
.def dizaine = r22


.org 0x0000
    jmp debut
.org 0x0002
   jmp plusDix
.org 0x0004
   jmp plusUn
.org 0x0006
   ;jmp marche
.org 0x0008
   jmp annuler
.org 0x0018
	jmp topHorloge
	
.org 0x0100

afficheur:
	.DB 0x7E, 0x0C, 0x37, 0x1F, 0x4D, 0x5B, 0x7B, 0x0E, 0x7F, 0x5F
	;	0	  1 	2	  3	    4	  5	    6	  7		8	  9

debut:   
	DDRA@IO <- 0xFF
	DDRB@IO <- 0xFF
	DDRD@IO <- 0b10000000
	EIMSK@IO <- 0b00011111
	EICRA <- 0xFF
	LDI temps,0
	LDI unite,0
	LDI dizaine,0
	LDI R26,0
	SEI 
    
boucle:
    sleep
    jmp boucle


plusDix:
    temps <- temps + 10
    RETI
    

plusUn:
    INC temps
    RETI
    
    
marche:
	WDTCSR <- 0x10
	WDTCSR <- 0b01000110
	PORTD@IO <- 0x80
	RETI
   

annuler:
	WDTCSR <- 0x10
	WDTCSR <- 0
	ldi temps,0
	PORTA@IO <- 0
	PORTB@IO <- 0
	PORTD@IO <- 0
	RETI
	
topHorloge:
	INC R26
	dizaine=temps/10
	unite=temps%10
	
	PORTA@IO <- afficheur@ROM[unite]
	PORTB@IO <- afficheur@ROM[dizaine]
	
	DEC temps
	si temps=0 alors saut annuler
	RETI