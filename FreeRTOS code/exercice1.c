#include <avr/io.h>
#include <freeRTOS.h>
#include <task.h>

void TaskBlink(void *pvParameters)
{
    (void) pvParameters;
    const TickType_t xFrequency = 500 / portTICK_PERIOD_MS;
    TickType_t xLastWakeTime;

    DDRA = 0xFF;
    DDRC = 0xF0;
    PORTC = 0x10;

    xLastWakeTime = xTaskGetTickCount();

    for(;;)
    {
        PORTA = PINA ^ 0x01;

        vTaskDelayUntil( &xLastWakeTime, xFrequency);
    }
}

void TaskBlink_2(void *pvParameters)
{
    (void) pvParameters;
    const TickType_t xFrequency = 1000 / portTICK_PERIOD_MS;

    TickType_t xLastWakeTime2;

    DDRA = 0xFF;
    DDRC = 0xF0;
    PORTC = 0x10;

    xLastWakeTime2 = xTaskGetTickCount();

    for(;;)
    {
        PORTA = PINA ^ 0x02;
        vTaskDelayUntil( &xLastWakeTime2, xFrequency);
    }
}

int main(void)
{
    xTaskCreate(TaskBlink, "Blink", 128, NULL, 2, NULL);
    xTaskCreate(TaskBlink_2, "Blink_2", 128, NULL, 2, NULL);
    vTaskStartScheduler();
    return 0;
}
