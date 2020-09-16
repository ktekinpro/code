;TP1 : port parallèle
;TP10 : un bouton / une LED

.equ PINA = 0x00
.equ DDRA = 0x01
.equ PORTA = 0x02
 
.org 0x0000
	jmp debut

.org 0x0080

debut:
	ldi r16, 0x01
	out DDRA, r16

boucle:
	in r16, PINA
	lsr r16
	PORTA, r16
	jmp boucle
