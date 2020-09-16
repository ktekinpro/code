#include <avr/io.h>
#include <freeRTOS.h>
#include <task.h>
#include <avr/interrupt.h>

int code_Afficheur[10]={0x7E,0x0C,0x37,0x1F,0x4D,0x5B,0x7B,0x0E,0x7F,0x5F};
TaskHandle_t xHandle_counter;

int cpt;
void TaskCounter(void *pvParameters);

ISR(INT1_vect)
{
    vTaskResume(xHandle_counter);
}

void TaskCounter(void *pvParameters)
{

    (void) pvParameters;
    const TickType_t xFrequency = 250 / portTICK_PERIOD_MS;
    TickType_t xLastWakeTime;

    xLastWakeTime = xTaskGetTickCount();

    for(;;)
    {
        cpt += 1;
        if(cpt%10==0){
            vTaskSuspend(xHandle_counter);
            xLastWakeTime = xTaskGetTickCount();
          }
        if (cpt>999)
            cpt=0;

        vTaskDelayUntil( &xLastWakeTime, xFrequency);
    }
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
    EICRA = 0x08;
    EIMSK = 0x02;
    cpt=0;
    sei();

    xTaskCreate(TaskCounter, "Count", 128, NULL, 2, &xHandle_counter);
    xTaskCreate(TaskDisplay, "Dsiplay", 128, NULL, 2, NULL);
    vTaskStartScheduler();
    return 0;
}
