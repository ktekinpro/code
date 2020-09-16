/*---------------------------------------------------
Send a message to the 1,2,3 widgets according to the Hall effect sensor
        with the bridge method

------      SOCLE

-----------------------------------------------------*/
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[]="*******************";
char ssid[]="*******************";
char pass[]="*******************";


#define PIN_capteur_hall_1 16
#define PIN_capteur_hall_2 05
#define PIN_capteur_hall_3 04

int capteur1, capteur2, capteur3;


WidgetBridge bridge1(V1);
WidgetBridge bridge2(V2);
WidgetBridge bridge3(V3);

BLYNK_CONNECTED() {
  bridge1.setAuthToken("*******************");
  bridge2.setAuthToken("*******************");
  bridge3.setAuthToken("*******************");
}


/*----------------------------------------------------------*/
//
//                          setup
//
/*----------------------------------------------------------*/
void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);

  pinMode(capteur1, INPUT);
  pinMode(capteur2, INPUT);
  pinMode(capteur3, INPUT);
}

/*----------------------------------------------------------*/
//
//                           loop
//
/*----------------------------------------------------------*/
void loop()
{
  Blynk.run();
  capteur1 = digitalRead(PIN_capteur_hall_1);
  capteur2 = digitalRead(PIN_capteur_hall_2);
  capteur3 = digitalRead(PIN_capteur_hall_3);

  /* ------------------*/
  //      sensor 1
  /* ------------------*/
  if(sensor1==LOW){
    //Blynk.virtualWrite(V1, 1);
    bridge1.virtualWrite(V1, HIGH);
    }
  else{
    //Blynk.virtualWrite(V1, 0);
    bridge1.virtualWrite(V1, LOW);
  }
  /* ------------------*/
  //      sensor 2
  /* ------------------*/
  if(capteur2==LOW){
    bridge2.virtualWrite(V2, HIGH);
    }
  else{
    bridge2.virtualWrite(V2, LOW);
    }
  /* ------------------*/
  //      capteur 3
  /* ------------------*/
  if(capteur3==LOW){
    bridge3.virtualWrite(V3, HIGH);
    }
  else{
    bridge3.virtualWrite(V3, LOW);
  }
}
