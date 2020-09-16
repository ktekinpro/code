;TP33 : régulateur pour chauffage central
;potantiomète (analogique) : de 10 à 26°C
;interrupteur : normal/hors-gel
;sortie digitale (1-0) pour commander le convecteur
;mode hors-gel = 6°C
;température mesurée affichée en permanence sur 2 afficheurs 7 segments (port A/B)

.DSEG
.ORG 0x200
temperature: .BYTE 1
dizaine: .BYTE 1
unite: .BYTE 1
consigne: .BYTE 1


.CSEG
.equ PORTA = 0x02
.equ DDRA = 0x01
.equ PINA = 0x00
.equ PORTB = 0x05 
.equ DDRB = 0x04
.equ PINB = 0x03
.equ ADMUX = 0x7C
.equ ADCSRB = 0x7B
.equ ADCSRA = 0x7A
.equ ADCL = 0x78
.equ ADCH = 0x79

.org 0x0000
	jmp init

.org 0x0080
	afficheur1:
	.DB 0b01111110,0b00001100,0b00110111,0b00011111,0b01001101,0b01011011,0b01111011,0b00001110,0b01111111,0b01011111

init:
	DDRA@IO <- 0x7F
	DDRB@IO <- 0xFF
	
	SEI

main:
	ADMUX<-0b01100000
	ADCSRA<-0b11000111
	ConvAnalogique1:
	si (ADCSRA & 0x10)== 0 saut ConvAnalogique1
	
	consigne<-(ADCH/16)+10
	
	ADMUX<-0b01100001
	ADCSRA<-0b11000111
	ConvAnalogique2:
	si (ADCSRA & 0x10)== 0 saut ConvAnalogique2
	
	temperature<-ADCH/8
	
	si (PINA@IO & 0x80)== 0 saut HorsGel
	
	si (consigne>temperature) saut afficheDecroissant
	si (consigne<temperature) saut afficheCroissant
	
	jmp main


afficheDecroissant:
	dizaine<-temperature/10
	unite<-temperature - dizaine*10
	PORTA@IO<- afficheur1@ROM[dizaine]
	PORTB@IO<- 0x80 | afficheur1@ROM[unite]
	jmp main
	
	
afficheCroissant:
	dizaine<-temperature/10
	unite<-temperature - dizaine*10
	PORTA@IO <- afficheur1@ROM[dizaine]
	PORTB@IO <- afficheur1@ROM[unite]
	jmp main

	
HorsGel:
	dizaine<-temperature/10
	unite<-temperature - dizaine*10
	PORTA@IO <- 0x80 | afficheur1@ROM[dizaine]
	si (temperature)>6 alors PORTB@IO <- afficheur1@ROM[unite]
	si (temperature)<6 alors PORTB@IO <- 0x80 | afficheur1@ROM[unite]
	jmp main
