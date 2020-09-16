/*
11 - "Pixel screen" game

equipments : Blynk application and an ESP 8266


8 sliders slowly deflate a mystery image when a slider is placed in the right position.

Sliders are widgets set up on the Blynk application.
The algorithm is coded on the ESP 8266 and the communication is done by LAN.
*/

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[]="*******************";
char ssid[]="*******************";
char pass[]="*******************";


#define POSITION_VALIDE1 5
#define POSITION_VALIDE2 5
#define POSITION_VALIDE3 5
#define POSITION_VALIDE4 5
#define POSITION_VALIDE5 5
#define POSITION_VALIDE6 5
#define POSITION_VALIDE7 5
#define POSITION_VALIDE8 5

 #define DELAIS_AVANT_REMISE_A_ZERO 10000 // reset after 10 seconds


int valeur_slider1=0, valeur_slider2=0, valeur_slider3=0, valeur_slider4=0;
int valeur_slider5=0, valeur_slider6=0, valeur_slider7=0, valeur_slider8=0;
int numero_image=1;

boolean deja_passe_1=false, deja_passe_2=false, deja_passe_3=false, deja_passe_4=false;
boolean deja_passe_5=false, deja_passe_6=false, deja_passe_7=false, deja_passe_8=false;

/*----------------------------------------------------------*/
//
//                  reset function
//
/*----------------------------------------------------------*/
void remise_a_zero(){
  Blynk.virtualWrite(V1, 0);
  Blynk.virtualWrite(V2, 0);
  Blynk.virtualWrite(V3, 0);
  Blynk.virtualWrite(V4, 1); // 1 valeur par défaut des images
  valeur_slider1=0; valeur_slider2=0; valeur_slider3=0; valeur_slider4=0;
  valeur_slider5=0; valeur_slider6=0; valeur_slider7=0; valeur_slider8=0;
  numero_image=1;
  deja_passe_1=false; deja_passe_2=false; deja_passe_3=false; deja_passe_4=false;
  deja_passe_5=false; deja_passe_6=false; deja_passe_7=false; deja_passe_8=false;
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
  remise_a_zero();
}

/*----------------------------------------------------------*/
//
//                     sliders reading
//
/*----------------------------------------------------------*/

// This function will be called every time Slider Widget
// in Blynk app writes values to the Virtual Pin 1
BLYNK_WRITE(V1)
{
  valeur_slider1 = param.asInt(); // assigning incoming value from pin V1 to a variable
}

BLYNK_WRITE(V2){
  valeur_slider2 = param.asInt();}

BLYNK_WRITE(V3){
  valeur_slider3 = param.asInt();}

BLYNK_WRITE(V4){
  valeur_slider4 = param.asInt();}

BLYNK_WRITE(V5){
  valeur_slider5 = param.asInt();}

BLYNK_WRITE(V6){
  valeur_slider6 = param.asInt();}

BLYNK_WRITE(V7){
  valeur_slider7 = param.asInt();}

BLYNK_WRITE(V8){
  valeur_slider8 = param.asInt();}

/*----------------------------------------------------------*/
//
//                           loop
//
/*----------------------------------------------------------*/
void loop()
{
  Blynk.run();

  /* ------------------*/
  //      slider 1
  /* ------------------*/
  if ((valeur_slider1==POSITION_VALIDE1) && (deja_passe_1==false)){
     numero_image += 1;
     deja_passe_1=true;
   }
 if ((valeur_slider1!=POSITION_VALIDE1) && (deja_passe_1==true)){
    numero_image -= 1;
    deja_passe_1=false;
  }

  /* ------------------*/
  //      slider 2
  /* ------------------*/
  if ((valeur_slider2==POSITION_VALIDE2) && (deja_passe_2==false)){
     numero_image += 1;
     deja_passe_2=true;
   }
 if ((valeur_slider2!=POSITION_VALIDE2) && (deja_passe_2==true)){
    numero_image -= 1;
    deja_passe_2=false;
  }

  /* ------------------*/
  //      slider 3
  /* ------------------*/
  if ((valeur_slider3==POSITION_VALIDE3) && (deja_passe_3==false)){
     numero_image += 1;
     deja_passe_3=true;
   }
 if ((valeur_slider3!=POSITION_VALIDE3) && (deja_passe_3==true)){
    numero_image -= 1;
    deja_passe_3=false;
  }

  /* ------------------*/
  //      slider 4
  /* ------------------*/
  if ((valeur_slider4==POSITION_VALIDE4) && (deja_passe_4==false)){
     numero_image += 1;
     deja_passe_4=true;
   }
 if ((valeur_slider4!=POSITION_VALIDE4) && (deja_passe_4==true)){
    numero_image -= 1;
    deja_passe_4=false;
  }

  /* ------------------*/
  //      slider 5
  /* ------------------*/
  if ((valeur_slider5==POSITION_VALIDE5) && (deja_passe_5==false)){
     numero_image += 1;
     deja_passe_5=true;
   }
 if ((valeur_slider5!=POSITION_VALIDE5) && (deja_passe_5==true)){
    numero_image -= 1;
    deja_passe_5=false;
  }

  /* ------------------*/
  //      slider 6
  /* ------------------*/
  if ((valeur_slider6==POSITION_VALIDE6) && (deja_passe_6==false)){
     numero_image += 1;
     deja_passe_6=true;
   }
 if ((valeur_slider6!=POSITION_VALIDE6) && (deja_passe_6==true)){
    numero_image -= 1;
    deja_passe_6=false;
  }

  /* ------------------*/
  //      slider 7
  /* ------------------*/
  if ((valeur_slider7==POSITION_VALIDE7) && (deja_passe_7==false)){
     numero_image += 1;
     deja_passe_7=true;
   }
 if ((valeur_slider7!=POSITION_VALIDE7) && (deja_passe_7==true)){
    numero_image -= 1;
    deja_passe_7=false;
  }

  /* ------------------*/
  //      slider 8
  /* ------------------*/
  if ((valeur_slider8==POSITION_VALIDE8) && (deja_passe_8==false)){
     numero_image += 1;
     deja_passe_8=true;
   }
 if ((valeur_slider8!=POSITION_VALIDE8) && (deja_passe_8==true)){
    numero_image -= 1;
    deja_passe_8=false;
  }

  /* ------------------*/
  //      success
  /* ------------------*/

  if((valeur_slider1==POSITION_VALIDE1) &&
    (valeur_slider2==POSITION_VALIDE2) &&
    (valeur_slider3==POSITION_VALIDE3) &&
    (valeur_slider4==POSITION_VALIDE4) &&
    (valeur_slider5==POSITION_VALIDE5) &&
    (valeur_slider6==POSITION_VALIDE6) &&
    (valeur_slider7==POSITION_VALIDE7) &&
    (valeur_slider8==POSITION_VALIDE8))
  {
    delay(DELAIS_AVANT_REMISE_A_ZERO);
    remise_a_zero();
  }

  /* ------------------*/
  // image display
  /* ------------------*/
  if (numero_image>9) numero_image = 9;
  if (numero_image<1) numero_image = 1;
  Blynk.virtualWrite(V0, numero_image);
}
