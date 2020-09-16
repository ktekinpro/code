#include <avr/io.h>
#include <freeRTOS.h>
#include <FreeRTOSConfig.h>
#include <task.h>
#include <avr/interrupt.h>

#define configUSE_PREEMPTION 1

int code_Afficheur[10]={0x7E,0x0C,0x37,0x1F,0x4D,0x5B,0x7B,0x0E,0x7F,0x5F};
TaskHandle_t xHandle_Modulo;
int cpt=0;



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

void modulo()
{
    int i,s;
    s=0;
    for(i=0; i<30001;i++)
    {
        s=(s+i)%100;
    }
}


void TaskModulo(void *pvParameters)
{
    (void) pvParameters;
    const TickType_t xFrequency = 1000 / portTICK_PERIOD_MS;
    TickType_t xLastWakeTime3;
    int debut, fin;
    xLastWakeTime3 = xTaskGetTickCount();

    debut=xTaskGetTickCount();
    DDRA = 0xFF;
    DDRC = 0xF0;

    for (;;)
    {
        vTaskSuspend(xHandle_Modulo);
        debut=xTaskGetTickCount();
        PORTC=0x10;
        PORTA=0xFF;


        modulo();

        PORTC=0x10;
        PORTA=0;

        fin=xTaskGetTickCount();
        cpt=fin-debut;
        vTaskDelayUntil( &xLastWakeTime3, xFrequency);
    }
}

ISR(INT0_vect)
{
    vTaskResume(xHandle_Modulo);
}

int main(void)
{
    xTaskCreate(TaskDisplay, "Dsiplay", 128, NULL, 2, NULL);
    xTaskCreate(TaskModulo, "Modulo", 128, NULL, 3, &xHandle_Modulo);

    EICRA = 0x02;
    EIMSK = 0x01;
    vTaskStartScheduler();

    return 0;
}
