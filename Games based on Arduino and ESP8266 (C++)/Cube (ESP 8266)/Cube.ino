/*---------------------------------------------------

Cube 1 : receive a message from the base and turn on the LED

------      CUBE 1

the power supply for the LED is charged by an induction module.
One part of this module is placed in the cube, the other in the socle.
-----------------------------------------------------*/
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[]="*******************";
char ssid[]="*******************";
char pass[]="*******************";


#define PIN_led 16

int valeur_capteur_hall=LOW;
int LED;

/*----------------------------------------------------------*/
//
//        widgets reading -> 0 : nothing happened / 1 : magnet detected
//
/*----------------------------------------------------------*/
BLYNK_WRITE(V1){
  valeur_capteur_hall = param.asInt();}


/*----------------------------------------------------------*/
//
//                          setup
//
/*----------------------------------------------------------*/
void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  pinMode(PIN_led, OUTPUT);

  valeur_capteur_hall=LOW;
}


/*----------------------------------------------------------*/
//
//                           loop
//
/*----------------------------------------------------------*/
void loop()
{
  Blynk.run();
  if(valeur_capteur_hall==HIGH){
    Serial.println("Capteur détecté");
    digitalWrite(PIN_led, HIGH); // turn on the LED
  }
  else{
    Serial.println("...");
      digitalWrite(PIN_led, LOW);
  }
}
