/* --------------------------------------------
3 physical buttons are linked to an ESP 8266

Each button differently increments the values of 3 widgets
present on the Blynk app

The goal of the game is to find the right combination to fill up 3 widgets on an Android application

The ESP 8266 is connected to the Blynk application via LAN, and it is
installed on a tablet acessible by the players.
---------------------------------------------*/
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[]="*******************";
char ssid[]="*******************";
char pass[]="*******************";

#define BLYNK_GREEN     "#23C48E" // hexadecimal values for the widgets' colors
#define BLYNK_BLUE      "#04C0F8"
#define BLYNK_YELLOW    "#ED9D00"
#define BLYNK_RED       "#D3435C"
#define BLYNK_DARK_BLUE "#5F7CD8"
#define Vert_clair      "#00FF00"
#define Noir            "#000000"
#define Blanc           "#FFFFFF"

#define PINint1 16
#define PINint2 05
#define PINint3 04

#define DELAIS_ANIMATION_REUSSITE 50

#define ValBouton_UN_BLEU 3
#define ValBouton_UN_VERT 4
#define ValBouton_UN_ROUGE -2

#define ValBouton_DEUX_BLEU -1
#define ValBouton_DEUX_VERT -2
#define ValBouton_DEUX_ROUGE 4

#define ValBouton_TROIS_BLEU 2
#define ValBouton_TROIS_VERT 1
#define ValBouton_TROIS_ROUGE 3


#define taille_barre_telechargement 100
#define SENSIBILITE_BOUTON 500

int value=0;
int valeurBbleue=0, valeurBverte=0, valeurBrouge=0;
int valeurBtelechargement=0;
int int1, int2,int3;
int i;

/*----------------------------------------------------------*/
//
//                  reset function
//
/*----------------------------------------------------------*/
void remise_a_zero(){
  Blynk.virtualWrite(V1, 0);
  Blynk.virtualWrite(V2, 0);
  Blynk.virtualWrite(V3, 0);
  Blynk.virtualWrite(V4, 0);
  valeurBrouge=0; valeurBverte=0; valeurBbleue=0;
}

/*----------------------------------------------------------*/
//
//                          setup
//
/*----------------------------------------------------------*/
void setup()
{
  // Debug console
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  Blynk.setProperty(V1, "color", BLYNK_BLUE);
  Blynk.setProperty(V2, "color", BLYNK_GREEN);
  Blynk.setProperty(V3, "color", BLYNK_RED);

  pinMode(PINint1, INPUT);
  pinMode(PINint2, INPUT);
  pinMode(PINint3, INPUT);

  remise_a_zero();
}


/*----------------------------------------------------------*/
//
//                           loop
//
/*----------------------------------------------------------*/
void loop()
{
  Blynk.run();
  int1 = digitalRead(PINint1);
  int2 = digitalRead(PINint2);
  int3 = digitalRead(PINint3);

/* ------------------*/
//      button 1
/* ------------------*/

  if ((int1==LOW) && (int2==HIGH) && (int3==HIGH)){
  while (int1=LOW){
    int1 = digitalRead(PINint1);
    }

  delay(SENSIBILITE_BOUTON);

  valeurBbleue+= ValBouton_UN_BLEU;
  valeurBverte+= ValBouton_UN_VERT;
  valeurBrouge+= ValBouton_UN_ROUGE;

  if ((valeurBbleue>9) || (valeurBverte>9) || (valeurBrouge>9)){ // widget get off
    valeurBbleue=0;
    valeurBverte=0;
    valeurBrouge=0;
    }

  if(valeurBbleue<0) valeurBbleue=0; // we stay at zero if negative value
  if(valeurBverte<0) valeurBverte=0;
  if(valeurBrouge<0) valeurBrouge=0;

  Blynk.virtualWrite(V1, valeurBbleue); // we send values on widgets
  Blynk.virtualWrite(V2, valeurBverte);
  Blynk.virtualWrite(V3, valeurBrouge);
  }
/* ------------------*/
//      button 2
/* ------------------*/

  if ((int2==LOW) && (int1==HIGH) && (int3==HIGH)){
  while (int2=LOW){
    int2 = digitalRead(PINint2);
    }

  delay(SENSIBILITE_BOUTON);

  valeurBbleue+= ValBouton_DEUX_BLEU;
  valeurBverte+= ValBouton_DEUX_VERT;
  valeurBrouge+= ValBouton_DEUX_ROUGE;

  if ((valeurBbleue>9) || (valeurBverte>9) || (valeurBrouge>9)){
    valeurBbleue=0;
    valeurBverte=0;
    valeurBrouge=0;
    }

  if(valeurBbleue<0) valeurBbleue=0;
  if(valeurBverte<0) valeurBverte=0;
  if(valeurBrouge<0) valeurBrouge=0;

  Blynk.virtualWrite(V1, valeurBbleue);
  Blynk.virtualWrite(V2, valeurBverte);
  Blynk.virtualWrite(V3, valeurBrouge);
  }


/* ------------------*/
//      button 3
/* ------------------*/

  if ((int3==LOW) && (int1==HIGH) && (int2==HIGH)){
  while (int3=LOW){
    int3 = digitalRead(PINint3);
    }

  delay(SENSIBILITE_BOUTON);

  valeurBbleue+= ValBouton_TROIS_BLEU;
  valeurBverte+= ValBouton_TROIS_VERT;
  valeurBrouge+= ValBouton_TROIS_ROUGE;

  if ((valeurBbleue>9) || (valeurBverte>9) || (valeurBrouge>9)){
    valeurBbleue=0;
    valeurBverte=0;
    valeurBrouge=0;
    }

  if(valeurBbleue<0) valeurBbleue=0;
  if(valeurBverte<0) valeurBverte=0;
  if(valeurBrouge<0) valeurBrouge=0;

  Blynk.virtualWrite(V1, valeurBbleue);
  Blynk.virtualWrite(V2, valeurBverte);
  Blynk.virtualWrite(V3, valeurBrouge);
  }

/* ------------------*/
//  one widget is at 7
/* ------------------*/
if (((valeurBverte==7) && (valeurBrouge!=7) && (valeurBbleue!=7)) ||
   ((valeurBverte!=7) && (valeurBrouge==7) && (valeurBbleue!=7)) ||
   ((valeurBverte!=7) && (valeurBrouge!=7) && (valeurBbleue==7)))
   {
     Blynk.virtualWrite(V4, 33);
   }

/* ------------------*/
// 2 widgets are at 7
/* ------------------*/
if (((valeurBverte==7) && (valeurBrouge==7) && (valeurBbleue!=7)) ||
   ((valeurBverte!=7) && (valeurBrouge==7) && (valeurBbleue==7)) ||
   ((valeurBverte==7) && (valeurBrouge!=7) && (valeurBbleue==7)))
   {
     Blynk.virtualWrite(V4, 67);
   }

/* ------------------*/
//  None of them are at 7
/* ------------------*/
if ((valeurBbleue!=7) && (valeurBverte!=7) && (valeurBrouge!=7))
  {
    Blynk.virtualWrite(V4, 0);
  }

/* ------------------*/
//      success
/* ------------------*/
  if ((valeurBbleue==7) && (valeurBverte==7) && (valeurBrouge==7)){
    for(i=0; i<=taille_barre_telechargement; i++){
      Blynk.virtualWrite(V4, i);
      delay(DELAIS_ANIMATION_REUSSITE);
      }
    delay(5000);
    remise_a_zero();
    }
}
