#include <avr/io.h>
#include <avr/interrupt.h>
#include <freeRTOS.h>
#include <task.h>

int code_Afficheur[10]={0x7E,0x0C,0x37,0x1F,0x4D,0x5B,0x7B,0x0E,0x7F,0x5F};

int cpt=0;
int valeur_reg;
int v2=0;

ISR(ADC_vect)
{
    valeur_reg=5*ADCH/255;
    if ((valeur_reg >= 1) && (v2<1))
    {
        cpt += 1;
        if (cpt>999) cpt=0;
    }
    v2 = valeur_reg;
}

void TaskDisplay(void *pvParameters)
{
    (void) pvParameters;
    const TickType_t xFrequency = 5 / portTICK_PERIOD_MS;
    TickType_t xLastWakeTime2;

    DDRA = 0xFF;
    DDRC = 0xF0;

    xLastWakeTime2 = xTaskGetTickCount();

    int i=1;

    for(;;)
    {
        if(i==1)
        {
            PORTC = 0x20;
            PORTA = code_Afficheur[cpt%10];
        }

        if(i==2)
        {
            PORTC = 0x40;
            PORTA = code_Afficheur[(cpt/10)%10];
        }

        if(i==3)
        {
            PORTC = 0x80;
            PORTA = code_Afficheur[cpt/100];
        }

        i+=1;
        if(i>3) i=1;
        vTaskDelayUntil( &xLastWakeTime2, xFrequency);
    }
}

int main(void)
{
    DDRF = 0;
    ADMUX =0b01100001 ;
    ADCSRB = 0x00 ;
    ADCSRA = 0b11101111;
    sei();

    xTaskCreate(TaskDisplay, "Dsiplay", 128, NULL, 2, NULL);
    vTaskStartScheduler();
    return 0;
}
