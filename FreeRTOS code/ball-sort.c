#include <avr/io.h>
#include <freeRTOS.h>
#include <FreeRTOSConfig.h>
#include <task.h>
#include <avr/interrupt.h>

int balle_rouge=0;
int balle_verte=0;
int balle_bleue=0;
int ouvert_rouge=0;
int ouvert_verte=0;
int ouvert_bleue=0;

void TaskLecture(void *pvParameters)
{
    (void) pvParameters;
    unsigned char capt;
    const TickType_t xFrequency = 10 / portTICK_PERIOD_MS;
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    while(1)
    {
        if((UCSR0A & 0x80) != 0)
        {
            capt = UDR0;
            if(capt=='R') balle_rouge=1;
            if(capt=='V') balle_verte=1;
            if(capt=='B') balle_bleue=1;
        }
         PORTA=capt;
        vTaskDelayUntil( &xLastWakeTime, xFrequency);
    }
}

void TaskTrappe(void *pvParameters)
{
    (void) pvParameters;
    const TickType_t xFrequency = 10 / portTICK_PERIOD_MS;
    TickType_t xLastWakeTime2;
    xLastWakeTime2 = xTaskGetTickCount();

    while(1)
    {
        if (balle_rouge && !ouvert_rouge)
        {
            vTaskDelay(250);
            while(!(UCSR0A & (1<<UDRE0)))
            {
                vTaskDelayUntil(&xLastWakeTime2, 2/portTICK_PERIOD_MS);
            }
            UDR0 = 'O';
            while(!(UCSR0A & (1<<UDRE0)))
            {
                vTaskDelayUntil(&xLastWakeTime2, (2/portTICK_PERIOD_MS));
            }
            UDR0 = '1';
            ouvert_rouge=1;
        }
        if (balle_verte && !ouvert_verte)
        {
            vTaskDelay(500);
            while(!(UCSR0A & (1<<UDRE0)))
            {
                vTaskDelayUntil(&xLastWakeTime2, 2/portTICK_PERIOD_MS);
            }
            UDR0 = 'O';
            while(!(UCSR0A & (1<<UDRE0)))
            {
                vTaskDelayUntil(&xLastWakeTime2, (2/portTICK_PERIOD_MS));
            }
            UDR0 = '2';
            ouvert_verte=1;
        }
        if (balle_bleue && !ouvert_bleue)
        {
            UDR0 = 'O';
            while(!(UCSR0A & (1<<UDRE0)))
            {
                vTaskDelayUntil(&xLastWakeTime2, 2/portTICK_PERIOD_MS);
            }
            vTaskDelay(800);
            while(!(UCSR0A & (1<<UDRE0)))
            {
                vTaskDelayUntil(&xLastWakeTime2, (2/portTICK_PERIOD_MS));
            }
            UDR0 = '3';
            ouvert_bleue=1;
        }
        vTaskDelayUntil( &xLastWakeTime2, xFrequency);
    }
}

void TaskFerme(void *pvParameters)
{
    (void) pvParameters;
    const TickType_t xFrequency = 10 / portTICK_PERIOD_MS;
    TickType_t xLastWakeTime3;
    xLastWakeTime3 = xTaskGetTickCount();

    while(1)
    {
        PORTA=0xFF;
        if(balle_rouge)
        {
            vTaskDelay(300);

            while(!(UCSR0A & (1<<UDRE0)))
            {
                vTaskDelayUntil(&xLastWakeTime3, (2/portTICK_PERIOD_MS));
            }
            UDR0 = 'F';
            while(!(UCSR0A & (1<<UDRE0)))
            {
                vTaskDelayUntil(&xLastWakeTime3, (2/portTICK_PERIOD_MS));
            }
            UDR0 = '1';
            balle_rouge=0;
            ouvert_rouge=0;
        }

        if(balle_verte)
        {
            vTaskDelay(600);
            while(!(UCSR0A & (1<<UDRE0)))
            {
                vTaskDelayUntil(&xLastWakeTime3, (2/portTICK_PERIOD_MS));
            }
            UDR0 = 'F';
            while(!(UCSR0A & (1<<UDRE0)))
            {
                vTaskDelayUntil(&xLastWakeTime3, (2/portTICK_PERIOD_MS));
            }
            UDR0 = '2';
            balle_verte=0;
            ouvert_verte=0;
        }

        if(balle_bleue)
        {
            vTaskDelay(900);
            while(!(UCSR0A & (1<<UDRE0)))
            {
                vTaskDelayUntil(&xLastWakeTime3, (2/portTICK_PERIOD_MS));
            }
            UDR0 = 'F';
            while(!(UCSR0A & (1<<UDRE0)))
            {
                vTaskDelayUntil(&xLastWakeTime3, (2/portTICK_PERIOD_MS));
            }
            UDR0 = '3';
            balle_bleue=0;
            ouvert_bleue=0;
        }



        vTaskDelayUntil( &xLastWakeTime3, xFrequency);
    }
}

int main(void)
{
    DDRA = 0xFF;
    DDRC = 0xF0;
    PORTC=0x10;
    UBRR0 = 103;
    UCSR0C = 6;
    UCSR0B = 0x18;
    UCSR0A = 0;


    xTaskCreate(TaskLecture, "Lecture", 128, NULL, 2, NULL);
    xTaskCreate(TaskTrappe, "Trappe", 128, NULL, 2, NULL);
      xTaskCreate(TaskFerme, "Ferme", 128, NULL, 2, NULL);
    vTaskStartScheduler();
    return 0;
}
