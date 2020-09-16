; TP20
;1 bouton, 1 LED, et un chien de garde (watchdog)

.equ PINA = 0x00
.equ DDRA = 0x01
.equ PORTA = 0x02

.equ WDTCSR = 0x60

.org 0x0000
      jmp debut

.org 0x0080

debut:
     DDRA@IO <- 0x01
     PORTA@IO <- 0x00 

      ;RESET toutes les secondes
     WDTCSR <- 0x10
     WDTCSR <- 0b00001110
     sei

boucle:
     si (PINA@IO & 0x02) == 0 alors saut boucle

     PORTA@IO <- 0x01
     
     ; Reset watchdog timer
     WDR
     JMP boucle
