#include <avr/io.h>
#include <freeRTOS.h>
#include <FreeRTOSConfig.h>
#include <task.h>
#include <avr/interrupt.h>

int code_Afficheur[10]={0x7E,0x0C,0x37,0x1F,0x4D,0x5B,0x7B,0x0E,0x7F,0x5F};
int codeTouche[10]={0b01000001, 0b10001000, 0b01001000, 0b00101000, 0b10000100, 0b01000100, 0b00100100, 0b10000010, 0b01000010, 0b00100010};
int switches[16]={23,182,212,51,165,211,36,95,45,126,147,128,236,210,59,111};

TaskHandle_t xHandle_LED;
TaskHandle_t xHandle_SWITCHES;
TaskHandle_t xHandle_KEYBOARD;
TaskHandle_t xHandle_DISPLAY;
TaskHandle_t xHandle_TIMER;

int cpt=0;
int code=0;
int pos=-1;
int touche=0;


void TaskDisplay(void *pvParameters)
{
    (void) pvParameters;
    const TickType_t xFrequency = 5 / portTICK_PERIOD_MS;
    TickType_t xLastWakeTime2;

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

void TaskSwitches(void *pvParameters)
{
    (void) pvParameters;
    const TickType_t xFrequency = 100 / portTICK_PERIOD_MS;
    TickType_t xLastWakeTime3;
    xLastWakeTime3 = xTaskGetTickCount();
    for(;;)
    {
        PORTC=0b00010000;
        PORTA=0;
        code=switches[PINC&0b00001111];

        vTaskDelayUntil( &xLastWakeTime3, xFrequency);
    }
}


void TaskTimer500(void *pvParameters)
{
    (void) pvParameters;
    const TickType_t xFrequency = 500 / portTICK_PERIOD_MS;
    TickType_t xLastWakeTime7;
    xLastWakeTime7 = xTaskGetTickCount();
    int cpt_timer=0;
    vTaskSuspend(xHandle_TIMER);
    for(;;)
    {
        cpt_timer++;
        if(cpt_timer >= 6){
            cpt_timer=0;
            vTaskSuspend(xHandle_DISPLAY);
            vTaskSuspend(xHandle_KEYBOARD);
            vTaskSuspend(xHandle_SWITCHES);
            vTaskSuspend(xHandle_LED);
            PORTC=0x10;
            while(1)
            {
                PORTA=0xFF;
                vTaskDelay(100);
                PORTA=0;
                vTaskDelay(100);
            }
        }
        vTaskDelayUntil( &xLastWakeTime7, xFrequency);
    }
}

void TaskKeyboard(void *pvParameters)
{
    (void) pvParameters;
    const TickType_t xFrequency = 100 / portTICK_PERIOD_MS;
    TickType_t xLastWakeTime4;
    xLastWakeTime4 = xTaskGetTickCount();
    int PINC_avant1,PINC_avant2,PINC_avant3;

    for(;;)
    {
        PORTC = 0b10000000;
        if(PINC_avant1==PORTC && PINC==0b10000010)
        {
            touche = 7;
            pos++;
            if(pos==0) cpt+=touche*100;
            if(pos==1) cpt+=touche*10;
            if(pos==2) cpt+=touche;
        }
        else if(PINC_avant1==PORTC && PINC==0b10000100)
        {
            touche = 4;
            pos++;
            if(pos==0) cpt+=touche*100;
            if(pos==1) cpt+=touche*10;
            if(pos==2) cpt+=touche;
        }
        else if(PINC_avant1==PORTC && PINC==0b10001000)
        {
            touche = 1;
            pos++;
            if(pos==0) cpt+=touche*100;
            if(pos==1) cpt+=touche*10;
            if(pos==2) cpt+=touche;
        }
        PINC_avant1=PINC;


        PORTC=0b01000000;
        if(PINC_avant2==PORTC && PINC==0b01000001)
        {
            touche = 0;
            pos++;
            if(pos==0) cpt+=touche*100;
            if(pos==1) cpt+=touche*10;
            if(pos==2) cpt+=touche;
        }
        else if(PINC_avant2==PORTC && PINC==0b01000010)
        {
            touche = 8;
            pos++;
            if(pos==0) cpt+=touche*100;
            if(pos==1) cpt+=touche*10;
            if(pos==2) cpt+=touche;
        }
        else if(PINC_avant2==PORTC && PINC==0b01000100)
        {
            touche = 5;
            pos++;
            if(pos==0) cpt+=touche*100;
            if(pos==1) cpt+=touche*10;
            if(pos==2) cpt+=touche;
        }
        else if(PINC_avant2==PORTC && PINC==0b01001000)
        {
            touche = 2;
            pos++;
            if(pos==0) cpt+=touche*100;
            if(pos==1) cpt+=touche*10;
            if(pos==2) cpt+=touche;
        }
        PINC_avant2=PINC;


        PORTC=0b00100000;
        if(PINC_avant3==PORTC && PINC==0b00100010)
        {
            touche = 9;
            pos++;
            if(pos==0) cpt+=touche*100;
            if(pos==1) cpt+=touche*10;
            if(pos==2) cpt+=touche;
        }
        else if(PINC_avant3==PORTC && PINC==0b00100100)
        {
            touche = 6;
            pos++;
            if(pos==0) cpt+=touche*100;
            if(pos==1) cpt+=touche*10;
            if(pos==2) cpt+=touche;
        }
        else if(PINC_avant3==PORTC && PINC==0b00101000)
        {
            touche = 3;
            pos++;
            if(pos==0) cpt+=touche*100;
            if(pos==1) cpt+=touche*10;
            if(pos==2) cpt+=touche;
        }
        PINC_avant3=PINC;

        if(pos==0) vTaskResume(xHandle_TIMER);

        if(pos>2)
        {
            cpt=0;
            pos=-1;
        }

        vTaskDelayUntil( &xLastWakeTime4, xFrequency);
    }
}

void TaskLED(void *pvParameters)
{
    (void) pvParameters;
    const TickType_t xFrequency = 5 / portTICK_PERIOD_MS;
    TickType_t xLastWakeTime5;
    xLastWakeTime5 = xTaskGetTickCount();


    for(;;)
    {
        if(pos==2)
        {
            if(cpt==code)
            {
                vTaskSuspend(xHandle_DISPLAY);
                vTaskSuspend(xHandle_KEYBOARD);
                vTaskSuspend(xHandle_SWITCHES);
                vTaskSuspend(xHandle_TIMER);
                PORTC=0x10;
                PORTA=0xFF;
            }
            if(cpt!=code)
            {
                vTaskSuspend(xHandle_DISPLAY);
                vTaskSuspend(xHandle_KEYBOARD);
                vTaskSuspend(xHandle_SWITCHES);
                vTaskSuspend(xHandle_TIMER);
                PORTC=0x10;
                while(1)
                {
                    PORTA=0xFF;
                    vTaskDelay(100);
                    PORTA=0;
                    vTaskDelay(100);
                }
            }
        }
        vTaskDelayUntil( &xLastWakeTime5, xFrequency);
    }
}

int main(void)
{
    DDRA = 0xFF;
    DDRC = 0xF0;

    xTaskCreate(TaskDisplay, "Display", 128, NULL, 2, &xHandle_DISPLAY);
    xTaskCreate(TaskKeyboard, "Keyboard",128,NULL,2, &xHandle_KEYBOARD);
    xTaskCreate(TaskSwitches, "Switches",128,NULL,2, &xHandle_SWITCHES);
    xTaskCreate(TaskTimer500, "Timer500",128,NULL,2, &xHandle_TIMER);
    xTaskCreate(TaskLED, "LED",128,NULL,2, &xHandle_LED);
    vTaskSuspend(xHandle_TIMER);
    vTaskStartScheduler();
    return 0;
}
