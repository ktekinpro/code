;TP21
;Chenillard par interruption

.equ DDRA =0x01
.equ DDRB = 0x04
.equ PORTA = 0x02
.equ PORTB = 0x05
.equ EIMSK = 0x1D
.equ EICRA = 0x69
.def cpt1 = r20
.def cpt2 = r21

.org 0x0000
    jmp debut
    
.org 0x0002
   jmp irqINT0
   
.org 0x0004
   jmp irqINT1
   
   
.org 0x0100

debut:
    DDRA@IO <- 0xFF
    DDRB@IO <- 0xFF
	PORTA@IO <- 0x01
	PORTB@IO <- 0
    EIMSK@IO <- 0b00000011
     ;STS
    EICRA <- 0b00001111
     LDI cpt1,0x01
     LDI cpt2,0
    SEI

boucle:
     sleep
     jmp boucle

     
;déplacement dans le sens horaire
irqINT0:
	si cpt2>0x01 alors saut bouger_g_r
	si cpt2=0 && cpt1!=0x80 alors saut bouger_d_l
	si cpt1=0x80 alors saut change1
	si cpt2=0x01 alors saut change2
    RETI

     
;déplacement dans le sens anti-horaire    
irqINT1:
	si cpt1=0x01 alors saut change3
	si cpt2=0 alors saut bouger_d_r
	si cpt2<0x80 alors saut bouger_g_l
	si cpt2=0x80 alors saut change4
	RETI
     
change1:
     LDI cpt1,0
     LDI cpt2,0x80
     PORTA@IO <- cpt1
     PORTB@IO <- cpt2
     RETI
     
change2:
     LDI cpt1,0x01
     LDI cpt2,0
     PORTA@IO <- cpt1
     PORTB@IO <- cpt2
     RETI
     
change3:
     LDI cpt1,0
     LDI cpt2,0X01
     PORTA@IO <- cpt1
     PORTB@IO <- cpt2
     RETI
     
change4:
     LDI cpt1,0x80
     LDI cpt2,0X00
     PORTA@IO <- cpt1
     PORTB@IO <- cpt2
     RETI
     
bouger_d_l:
     lsl cpt1
     PORTA@IO <- cpt1
     PORTB@IO <- cpt2
     RETI
     
bouger_d_r:
     lsr cpt1
     PORTA@IO <- cpt1
     PORTB@IO <- cpt2
     RETI
     
bouger_g_l:
     lsl cpt2
     PORTA@IO <- cpt1
     PORTB@IO <- cpt2
     RETI
     
bouger_g_r:
     lsr cpt2
     PORTA@IO <- cpt1
     PORTB@IO <- cpt2
     RETI
