/*
"The rings" game

equipments : 1 arduino Uno, 7 light rings, 6  light bars


3 buttons allow users to change rings and rotate them clockwise and counterclockwise

the goal is to pass a bar through the rings from the left to the center, then from the center to the right.
*/

#include <Adafruit_NeoPixel.h>
#include <avr/wdt.h> // Required to use the watchdog

#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

/*----------------------------------------------------------*/
//
//               global variables and macros
//
/*----------------------------------------------------------*/

#define PINANNEAU1 6 //ring 1, 48 LEDs
#define PINANNEAU2 7
#define PINANNEAU3 8
#define PINANNEAU4 9
#define PINANNEAU5 10
#define PINANNEAU6 11
#define PINANNEAU7 12 //ring 7, the smallest

#define PINBarreGauche1 42
#define PINBarreGauche2 40
#define PINBarreGauche3 38
#define PINBarreDroite1 48
#define PINBarreDroite2 50
#define PINBarreDroite3 46

#define NUMPIXELS1 48 // our first light ring have 48 LEDs
#define NUMPIXELS2 40
#define NUMPIXELS3 32
#define NUMPIXELS4 24
#define NUMPIXELS5 16
#define NUMPIXELS6 12
#define NUMPIXELS7 8
#define NUMPIXELSBG1 3  // our fist light bar have 2 LEDs
#define NUMPIXELSBG2 3
#define NUMPIXELSBG3 3
#define NUMPIXELSBD1 3
#define NUMPIXELSBD2 3
#define NUMPIXELSBD3 3

#define PINint1 2 // button for changing ring
#define PINint2 3 // button for clockwise rotation
#define PINint3 4 // button for counterclockwise rotation

#define DELAYanneau_1 0 // ring one's speed
#define DELAYanneau_2 0
#define DELAYanneau_3 0
#define DELAYanneau_4 20
#define DELAYanneau_5 30
#define DELAYanneau_6 30
#define SENSIBILITE_BOUTON_UN 40 // speed for changing rings

#define DELAY_position_valide 80

int R=0; //color of the rings
int G=1;
int B=0;

int R_selection=2;
int G_selection=1;
int B_selection=0;

int R_position_valide=10;
int G_position_valide=0;
int B_position_valide=0;

int R_barre_rouge=30;
int G_barre_rouge=0;
int B_barre_rouge=0;

Adafruit_NeoPixel pixels1(NUMPIXELS1, PINANNEAU1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels2(NUMPIXELS2, PINANNEAU2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels3(NUMPIXELS3, PINANNEAU3, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels4(NUMPIXELS4, PINANNEAU4, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels5(NUMPIXELS5, PINANNEAU5, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels6(NUMPIXELS6, PINANNEAU6, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels7(NUMPIXELS7, PINANNEAU7, NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel barreG1(NUMPIXELSBG1, PINBarreGauche1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel barreG2(NUMPIXELSBG2, PINBarreGauche2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel barreG3(NUMPIXELSBG3, PINBarreGauche3, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel barreD1(NUMPIXELSBD1, PINBarreDroite1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel barreD2(NUMPIXELSBD2, PINBarreDroite2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel barreD3(NUMPIXELSBD3, PINBarreDroite3, NEO_GRB + NEO_KHZ800);


int POSITION_VALIDE_1=24;
int POSITION_VALIDE_2=20;
int POSITION_VALIDE_3=16;
int POSITION_VALIDE_4=12;
int POSITION_VALIDE_5=8;
int POSITION_VALIDE_6=6;

int int1=0, int2=0, int3=0;
int position_centrale_1=0, premier_pixel_1=0, dernier_pixel_1=0;
int position_centrale_2=0, premier_pixel_2=0, dernier_pixel_2=0;
int position_centrale_3=0, premier_pixel_3=0, dernier_pixel_3=0;
int position_centrale_4=0, premier_pixel_4=0, dernier_pixel_4=0;
int position_centrale_5=0, premier_pixel_5=0, dernier_pixel_5=0;
int position_centrale_6=0, premier_pixel_6=0, dernier_pixel_6=0;
int position_centrale_BG1=0, premier_pixel_BG1=0, dernier_pixel_BG1=0;
int position_centrale_BG2=0, premier_pixel_BG2=0, dernier_pixel_BG2=0;
int position_centrale_BG3=0, premier_pixel_BG3=0, dernier_pixel_BG3=0;
int position_centrale_BD1=0, premier_pixel_BD1=0, dernier_pixel_BD1=0;
int position_centrale_BD2=0, premier_pixel_BD2=0, dernier_pixel_BD2=0;
int position_centrale_BD3=0, premier_pixel_BD3=0, dernier_pixel_BD3=0;
int compteur=1, nombre_de_reussite=0;

boolean anneau_1_valide=false,anneau_2_valide=false,anneau_3_valide=false;
boolean anneau_4_valide=false,anneau_5_valide=false,anneau_6_valide=false;

/*----------------------------------------------------------*/
//
//                       functions
//
/*----------------------------------------------------------*/
void reset_software(){
  wdt_enable(WDTO_15MS); // watchdog to reset the program when it ended
  for(;;); // wait before the watchdog (here 15 ms)
}

int increment(int taille_anneau, int x){
  if (x!=taille_anneau-1) return x+1;
  else return 0;
}

int decrement(int taille_anneau, int x){
  if (x!=0) return x-1;
  else return taille_anneau-1;
}
/*----------------------------------------------------------*/
//
//              function to turn off the rings
//
/*----------------------------------------------------------*/
void eteint_anneau(int NUMPIXELS, int numero)
{
    for(int i=0; i<NUMPIXELS; i++)
    {
      if (numero==1){pixels1.setPixelColor(i, pixels1.Color(0, 0, 0)); pixels1.show();}
      else if (numero==2){pixels2.setPixelColor(i, pixels2.Color(0, 0, 0)); pixels2.show();}
      else if (numero==3){pixels3.setPixelColor(i, pixels3.Color(0, 0, 0)); pixels3.show();}
      else if (numero==4){pixels4.setPixelColor(i, pixels4.Color(0, 0, 0)); pixels4.show();}
      else if (numero==5){pixels5.setPixelColor(i, pixels5.Color(0, 0, 0)); pixels5.show();}
      else if (numero==6){pixels6.setPixelColor(i, pixels6.Color(0, 0, 0)); pixels6.show();}
      else if (numero==7){pixels7.setPixelColor(i, pixels7.Color(0, 0, 0)); pixels7.show();}
      else if (numero==11){barreG1.setPixelColor(i, barreG1.Color(0, 0, 0)); barreG1.show();}
      else if (numero==12){barreG2.setPixelColor(i, barreG2.Color(0, 0, 0)); barreG2.show();}
      else if (numero==13){barreG3.setPixelColor(i, barreG3.Color(0, 0, 0)); barreG3.show();}
      else if (numero==21){barreD1.setPixelColor(i, barreD1.Color(0, 0, 0)); barreD1.show();}
      else if (numero==22){barreD2.setPixelColor(i, barreD2.Color(0, 0, 0)); barreD2.show();}
      else if (numero==23){barreD3.setPixelColor(i, barreD3.Color(0, 0, 0)); barreD3.show();}
    }
}

/*----------------------------------------------------------*/
//
//                      display function
//
/*----------------------------------------------------------*/
void affiche(int debut, int fin, int taille, int red, int green, int blue, int numero)
{
  if (numero==1){
  if (debut<fin){
    for (int i=debut; i<=fin; i++){pixels1.setPixelColor(i, pixels1.Color(red, green, blue));pixels1.show();} }
  else{
    for (int i=debut; i<taille; i++){pixels1.setPixelColor(i, pixels1.Color(red, green, blue));pixels1.show();}
    for (int i=0; i<=fin; i++){pixels1.setPixelColor(i, pixels1.Color(red, green, blue)); pixels1.show();}}
  }
  else if (numero==2){
  if (debut<fin){
    for (int i=debut; i<=fin; i++){pixels2.setPixelColor(i, pixels2.Color(red, green, blue));pixels2.show();} }
  else{
    for (int i=debut; i<taille; i++){pixels2.setPixelColor(i, pixels2.Color(red, green, blue));pixels2.show();}
    for (int i=0; i<=fin; i++){pixels2.setPixelColor(i, pixels2.Color(red, green, blue)); pixels2.show();}}
  }
  else if (numero==3){
  if (debut<fin){
    for (int i=debut; i<=fin; i++){pixels3.setPixelColor(i, pixels3.Color(red, green, blue));pixels3.show();} }
  else{
    for (int i=debut; i<taille; i++){pixels3.setPixelColor(i, pixels3.Color(red, green, blue));pixels3.show();}
    for (int i=0; i<=fin; i++){pixels3.setPixelColor(i, pixels3.Color(red, green, blue)); pixels3.show();}}
  }
  else if (numero==4){
  if (debut<fin){
    for (int i=debut; i<=fin; i++){pixels4.setPixelColor(i, pixels4.Color(red, green, blue));pixels4.show();} }
  else{
    for (int i=debut; i<taille; i++){pixels4.setPixelColor(i, pixels4.Color(red, green, blue));pixels4.show();}
    for (int i=0; i<=fin; i++){pixels4.setPixelColor(i, pixels4.Color(red, green, blue)); pixels4.show();}}
  }
  else if (numero==5){
  if (debut<fin){
    for (int i=debut; i<=fin; i++){pixels5.setPixelColor(i, pixels5.Color(red, green, blue));pixels5.show();} }
  else{
    for (int i=debut; i<taille; i++){pixels5.setPixelColor(i, pixels5.Color(red, green, blue));pixels5.show();}
    for (int i=0; i<=fin; i++){pixels5.setPixelColor(i, pixels5.Color(red, green, blue)); pixels5.show();}}
  }
  else if (numero==6){
  if (debut<fin){
    for (int i=debut; i<=fin; i++){pixels6.setPixelColor(i, pixels6.Color(red, green, blue));pixels6.show();} }
  else{
    for (int i=debut; i<taille; i++){pixels6.setPixelColor(i, pixels6.Color(red, green, blue));pixels6.show();}
    for (int i=0; i<=fin; i++){pixels6.setPixelColor(i, pixels6.Color(red, green, blue)); pixels6.show();}}
  }
  else if (numero==7){
  if (debut<fin){
    for (int i=debut; i<=fin; i++){pixels7.setPixelColor(i, pixels7.Color(red, green, blue));pixels7.show();} }
  else{
    for (int i=debut; i<taille; i++){pixels7.setPixelColor(i, pixels7.Color(red, green, blue));pixels7.show();}
    for (int i=0; i<=fin; i++){pixels7.setPixelColor(i, pixels7.Color(red, green, blue)); pixels7.show();}}
  }


  else if (numero==11){
      for (int i=debut; i<=fin; i++){barreG1.setPixelColor(i, barreG1.Color(red, green,blue));barreG1.show();}
  }
  else if (numero==12){
    for (int i=debut; i<=fin; i++){barreG2.setPixelColor(i, barreG2.Color(red, green,blue));barreG2.show();}
  }
  else if (numero==13){
      for (int i=debut; i<=fin; i++){barreG3.setPixelColor(i, barreG3.Color(red, green,blue));barreG3.show();}
  }
  else   if (numero==21){
      for (int i=debut; i<=fin; i++){barreD1.setPixelColor(i, barreD1.Color(red, green,blue));barreD1.show();}
  }
  else if (numero==22){
  for (int i=debut; i<=fin; i++){barreD2.setPixelColor(i, barreD2.Color(red, green,blue));barreD2.show();}
  }
  else if (numero==23){
  for (int i=debut; i<=fin; i++){barreD3.setPixelColor(i, barreD3.Color(red, green,blue));barreD3.show();}
  }
}

/*----------------------------------------------------------*/
//
//                       SETUP
//
/*----------------------------------------------------------*/
void setup() {
  pinMode(int1, PINint1);
  pinMode(int2, PINint2);
  pinMode(int3, PINint3);

  pixels1.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels2.begin();
  pixels3.begin();
  pixels4.begin();
  pixels5.begin();
  pixels6.begin();
  pixels7.begin();
  barreG1.begin();
  barreG2.begin();
  barreG3.begin();
  barreD1.begin();
  barreD2.begin();
  barreD3.begin();

  pixels1.clear();
  pixels2.clear();
  pixels3.clear();
  pixels4.clear();
  pixels5.clear();
  pixels6.clear();
  pixels7.clear();
  barreG1.clear();
  barreG2.clear();
  barreG3.clear();
  barreD1.clear();
  barreD2.clear();
  barreD3.clear();

  eteint_anneau(NUMPIXELS1,1);
  eteint_anneau(NUMPIXELS2,2);
  eteint_anneau(NUMPIXELS3,3);
  eteint_anneau(NUMPIXELS4,4);
  eteint_anneau(NUMPIXELS5,5);
  eteint_anneau(NUMPIXELS6,6);
  eteint_anneau(NUMPIXELS7,7);

  eteint_anneau(NUMPIXELSBG1,11);
  eteint_anneau(NUMPIXELSBG2,12);
  eteint_anneau(NUMPIXELSBG3,13);
  eteint_anneau(NUMPIXELSBD1,21);
  eteint_anneau(NUMPIXELSBD2,22);
  eteint_anneau(NUMPIXELSBD3,23);

  premier_pixel_1=0; // 48
  dernier_pixel_1=44;
  position_centrale_1=46;
  affiche(premier_pixel_1, dernier_pixel_1, NUMPIXELS1, R, G, B, 1);

  premier_pixel_2=10; //40
  dernier_pixel_2=6;
  position_centrale_2=8;
  affiche(premier_pixel_2, dernier_pixel_2, NUMPIXELS2, R, G, B, 2);

  premier_pixel_3=31;//32
  dernier_pixel_3=27;
  position_centrale_3=29;
  affiche(premier_pixel_3, dernier_pixel_3, NUMPIXELS3, R, G, B, 3);

  premier_pixel_4=7;//24
  dernier_pixel_4=3;
  position_centrale_4=5;
  affiche(premier_pixel_4, dernier_pixel_4, NUMPIXELS4, R, G, B, 4);

  premier_pixel_5=13;//16
  dernier_pixel_5=9;
  position_centrale_5=11;
  affiche(premier_pixel_5, dernier_pixel_5, NUMPIXELS5, R, G, B, 5);

  premier_pixel_6=4;//12
  dernier_pixel_6=0;
  position_centrale_6=2;
  affiche(premier_pixel_6, dernier_pixel_6, NUMPIXELS6, R, G, B, 6);


  premier_pixel_BG2=0;
  dernier_pixel_BG2=2;
  affiche(premier_pixel_BG2, dernier_pixel_BG2, NUMPIXELSBG2, R_barre_rouge,G_barre_rouge,B_barre_rouge, 12);
}

/*----------------------------------------------------------*/
//
//                       LOOP
//
/*----------------------------------------------------------*/
void loop() {

  /*----------------------------------------------------------*/
  //               selection of the ring
  /*----------------------------------------------------------*/

  int1 = digitalRead(PINint1);
  if ((int1 == LOW) && (int2 == HIGH) && (int3 == HIGH))
  {
    //while (int1=LOW) {int1 = digitalRead(PINint1);}
    delay(SENSIBILITE_BOUTON_UN);              //wait for button 1
    if (nombre_de_reussite==0) compteur+=1;
    if (nombre_de_reussite==1) compteur-=1;
    if (compteur>=7) compteur=1;
    if (compteur<1) compteur=6;
  }

  /*----------------------------------------------------------*/
  //             red ring's displacememnt
  /*----------------------------------------------------------*/

if (nombre_de_reussite == 0){
/*--------------- ring 1 - step 1 ---------------*/
  if ((position_centrale_1==POSITION_VALIDE_1) ||
  (increment(NUMPIXELS1, position_centrale_1) == POSITION_VALIDE_1) ||
  (decrement(NUMPIXELS1, position_centrale_1) == POSITION_VALIDE_1))
  {
    pixels1.setPixelColor(POSITION_VALIDE_1, pixels1.Color(R_position_valide, G_position_valide, B_position_valide));
    pixels1.show();
    anneau_1_valide=true;
  }

  else anneau_1_valide=false;

/*--------------- ring 2 - step 1 ---------------*/
  if ((position_centrale_2==POSITION_VALIDE_2) ||
  (increment(NUMPIXELS2, position_centrale_2) == POSITION_VALIDE_2) ||
  (decrement(NUMPIXELS2, position_centrale_2) == POSITION_VALIDE_2))
  {
    if (anneau_1_valide==true) pixels2.setPixelColor(POSITION_VALIDE_2, pixels2.Color(R_position_valide, G_position_valide, B_position_valide));
    pixels2.show();
    anneau_2_valide=true;
  }

  else anneau_2_valide = false;

  if ((anneau_2_valide==true) && (anneau_1_valide==false))
  {
    pixels2.setPixelColor(POSITION_VALIDE_2, pixels2.Color(0, 0, 0));
    pixels2.show();
  }


/*--------------- ring 3 - step 1 ---------------*/
  if ((position_centrale_3==POSITION_VALIDE_3) ||
  (increment(NUMPIXELS3, position_centrale_3) == POSITION_VALIDE_3) ||
  (decrement(NUMPIXELS3, position_centrale_3) == POSITION_VALIDE_3))
  {
    if ((anneau_1_valide) && (anneau_2_valide))
    pixels3.setPixelColor(POSITION_VALIDE_3, pixels3.Color(R_position_valide, G_position_valide, B_position_valide));
    pixels3.show();
    anneau_3_valide=true;
  }

  else anneau_3_valide = false;

  if ((anneau_3_valide==true) &&
  ((anneau_1_valide==false) ||
  (anneau_2_valide==false)))
  {
    pixels3.setPixelColor(POSITION_VALIDE_3, pixels3.Color(0, 0, 0));
    pixels3.show();
  }

  /*--------------- ring 4 - step 1 ---------------*/
    if ((position_centrale_4==POSITION_VALIDE_4) ||
    (increment(NUMPIXELS4, position_centrale_4) == POSITION_VALIDE_4) ||
    (decrement(NUMPIXELS4, position_centrale_4) == POSITION_VALIDE_4))
    {
      if ((anneau_1_valide) && (anneau_2_valide) && (anneau_3_valide))
      pixels4.setPixelColor(POSITION_VALIDE_4, pixels4.Color(R_position_valide, G_position_valide, B_position_valide));
      pixels4.show();
      anneau_4_valide=true;
    }

    else anneau_4_valide = false;

    if ((anneau_4_valide==true) &&
    ((anneau_1_valide==false) ||
    (anneau_2_valide==false) ||
    (anneau_3_valide==false)))
    {
      pixels4.setPixelColor(POSITION_VALIDE_4, pixels4.Color(0, 0, 0));
      pixels4.show();
    }

  /*--------------- ring 5 - step 1 ---------------*/
    if ((position_centrale_5==POSITION_VALIDE_5) ||
    (increment(NUMPIXELS5, position_centrale_5) == POSITION_VALIDE_5) ||
    (decrement(NUMPIXELS5, position_centrale_5) == POSITION_VALIDE_5))
    {
      if ((anneau_1_valide) && (anneau_2_valide) && (anneau_3_valide) && (anneau_4_valide))
      pixels5.setPixelColor(POSITION_VALIDE_5, pixels5.Color(R_position_valide, G_position_valide, B_position_valide));
      pixels5.show();
      anneau_5_valide=true;
    }

    else anneau_5_valide = false;

    if ((anneau_5_valide==true) &&
    ((anneau_1_valide==false) ||
    (anneau_2_valide==false) ||
    (anneau_3_valide==false) ||
    (anneau_4_valide==false)))
    {
      pixels5.setPixelColor(POSITION_VALIDE_5, pixels5.Color(0, 0, 0));
      pixels5.show();
    }

    /*--------------- ring 6 - step 1 ---------------*/
    if ((position_centrale_6==POSITION_VALIDE_6) ||
    (increment(NUMPIXELS6, position_centrale_6) == POSITION_VALIDE_6) ||
    (decrement(NUMPIXELS6, position_centrale_6) == POSITION_VALIDE_6))
    {
      if ((anneau_1_valide) && (anneau_2_valide) && (anneau_3_valide) && (anneau_4_valide) && (anneau_5_valide))
      pixels6.setPixelColor(POSITION_VALIDE_6, pixels6.Color(R_position_valide, G_position_valide, B_position_valide));
      pixels6.show();
      anneau_6_valide=true;
    }

    else anneau_6_valide=false;

    if ((anneau_6_valide==true) &&
    ((anneau_1_valide==false) ||
    (anneau_2_valide==false) ||
    (anneau_3_valide==false) ||
    (anneau_4_valide==false) ||
    (anneau_5_valide==false)))
    {
      pixels6.setPixelColor(POSITION_VALIDE_6, pixels6.Color(0, 0, 0));
      pixels6.show();
    }
  }

  if (nombre_de_reussite == 1){ //step 2 : centre --> droite
  /*--------------- ring 6 - step 2 ---------------*/
    if ((position_centrale_6==POSITION_VALIDE_6) ||
    (increment(NUMPIXELS6, position_centrale_6) == POSITION_VALIDE_6) ||
    (decrement(NUMPIXELS6, position_centrale_6) == POSITION_VALIDE_6))
    {
      pixels6.setPixelColor(POSITION_VALIDE_6, pixels6.Color(R_position_valide, G_position_valide, B_position_valide));
      pixels6.show();
      anneau_6_valide=true;
    }

    else anneau_6_valide=false;


  /*--------------- ring 5 - step 2 ---------------*/
    if ((position_centrale_5==POSITION_VALIDE_5) ||
    (increment(NUMPIXELS5, position_centrale_5) == POSITION_VALIDE_5) ||
    (decrement(NUMPIXELS5, position_centrale_5) == POSITION_VALIDE_5))
    {
      if (anneau_6_valide==true)
      pixels5.setPixelColor(POSITION_VALIDE_5, pixels5.Color(R_position_valide, G_position_valide, B_position_valide));
      pixels5.show();
      anneau_5_valide=true;
    }

    else anneau_5_valide = false;

    if ((anneau_5_valide==true) && (anneau_6_valide==false))
    {
      pixels5.setPixelColor(POSITION_VALIDE_5, pixels5.Color(0, 0, 0));
      pixels5.show();
    }

    /*--------------- ring 4 - step 2 ---------------*/
      if ((position_centrale_4==POSITION_VALIDE_4) ||
      (increment(NUMPIXELS4, position_centrale_4) == POSITION_VALIDE_4) ||
      (decrement(NUMPIXELS4, position_centrale_4) == POSITION_VALIDE_4))
      {
        if ((anneau_6_valide==true) && (anneau_5_valide==true))
        pixels4.setPixelColor(POSITION_VALIDE_4, pixels4.Color(R_position_valide, G_position_valide, B_position_valide));
        pixels4.show();
        anneau_4_valide=true;
      }

      else anneau_4_valide = false;

      if ((anneau_4_valide==true) &&
        ((anneau_6_valide==false) ||
        (anneau_5_valide==false)))
      {
        pixels4.setPixelColor(POSITION_VALIDE_4, pixels4.Color(0, 0, 0));
        pixels4.show();
      }
    /*--------------- ring 3 - step 2 ---------------*/
      if ((position_centrale_3==POSITION_VALIDE_3) ||
      (increment(NUMPIXELS3, position_centrale_3) == POSITION_VALIDE_3) ||
      (decrement(NUMPIXELS3, position_centrale_3) == POSITION_VALIDE_3))
      {
        if ((anneau_6_valide==true) && (anneau_5_valide==true) && (anneau_4_valide==true))
        pixels3.setPixelColor(POSITION_VALIDE_3, pixels3.Color(R_position_valide, G_position_valide, B_position_valide));
        pixels3.show();
        anneau_3_valide=true;
      }

      else anneau_3_valide = false;

      if ((anneau_3_valide==true) &&
        ((anneau_6_valide==false) ||
        (anneau_5_valide==false) ||
        (anneau_4_valide==false)))
      {
        pixels3.setPixelColor(POSITION_VALIDE_3, pixels3.Color(0, 0, 0));
        pixels3.show();
      }
      /*--------------- ring 2 - step 2 ---------------*/
        if ((position_centrale_2==POSITION_VALIDE_2) ||
        (increment(NUMPIXELS2, position_centrale_2) == POSITION_VALIDE_2) ||
        (decrement(NUMPIXELS2, position_centrale_2) == POSITION_VALIDE_2))
        {
          if ((anneau_6_valide==true) && (anneau_5_valide==true) &&
              (anneau_4_valide==true) && (anneau_3_valide))
          pixels2.setPixelColor(POSITION_VALIDE_2, pixels2.Color(R_position_valide, G_position_valide, B_position_valide));
          pixels2.show();
          anneau_2_valide=true;
        }

        else anneau_2_valide = false;

        if ((anneau_2_valide==true) &&
          ((anneau_6_valide==false) ||
          (anneau_5_valide==false) ||
          (anneau_4_valide==false) ||
          (anneau_3_valide==false)))
        {
          pixels2.setPixelColor(POSITION_VALIDE_2, pixels2.Color(0, 0, 0));
          pixels2.show();
        }
        /*--------------- ring 1 - step 1 ---------------*/
          if ((position_centrale_1==POSITION_VALIDE_1) ||
          (increment(NUMPIXELS1, position_centrale_1) == POSITION_VALIDE_1) ||
          (decrement(NUMPIXELS1, position_centrale_1) == POSITION_VALIDE_1))
          {
            if ((anneau_6_valide==true) && (anneau_5_valide==true) &&
                (anneau_4_valide==true) && (anneau_3_valide) && (anneau_2_valide==true))
            pixels1.setPixelColor(POSITION_VALIDE_1, pixels1.Color(R_position_valide, G_position_valide, B_position_valide));
            pixels1.show();
            anneau_1_valide=true;
          }

          else anneau_1_valide = false;

          if ((anneau_1_valide==true) &&
            ((anneau_6_valide==false) ||
            (anneau_5_valide==false) ||
            (anneau_4_valide==false) ||
            (anneau_3_valide==false) ||
            (anneau_2_valide==false)))
          {
            pixels1.setPixelColor(POSITION_VALIDE_1, pixels1.Color(0, 0, 0));
            pixels1.show();
          }
  }
  /*----------------------------------------------------------*/
  //                  ring 1
  /*----------------------------------------------------------*/

  if(compteur==1) {
    affiche(premier_pixel_1,dernier_pixel_1,NUMPIXELS1,R_selection,G_selection,B_selection,1);
    if (nombre_de_reussite==0) affiche(premier_pixel_6,dernier_pixel_6,NUMPIXELS6,R,G,B,6);
    if (nombre_de_reussite==1) affiche(premier_pixel_2,dernier_pixel_2,NUMPIXELS2,R,G,B,2);

    int2 = digitalRead(PINint2);
    int3 = digitalRead(PINint3);

    if ((int2 == LOW) && (int1 == HIGH) && (int3 == HIGH))  {
      pixels1.setPixelColor(premier_pixel_1, pixels1.Color(0, 0,0));
      premier_pixel_1=increment(NUMPIXELS1,premier_pixel_1);
      dernier_pixel_1=increment(NUMPIXELS1,dernier_pixel_1);
      position_centrale_1=increment(NUMPIXELS1,position_centrale_1);
      pixels1.setPixelColor(dernier_pixel_1, pixels1.Color(R_selection,G_selection,B_selection));
      pixels1.show();
      delay(DELAYanneau_1);
     }

    if ((int3 == LOW) && (int1 == HIGH) && (int2 == HIGH))
     {
      pixels1.setPixelColor(dernier_pixel_1, pixels1.Color(0, 0,0));
      premier_pixel_1=decrement(NUMPIXELS1,premier_pixel_1);
      dernier_pixel_1=decrement(NUMPIXELS1,dernier_pixel_1);
      position_centrale_1=decrement(NUMPIXELS1,position_centrale_1);
      pixels1.setPixelColor(premier_pixel_1, pixels1.Color(R_selection,G_selection,B_selection));
      pixels1.show();
      delay(DELAYanneau_1);
     }
   }

  /*----------------------------------------------------------*/
  //                  ring 2
  /*----------------------------------------------------------*/

  if(compteur==2){
    affiche(premier_pixel_2,dernier_pixel_2,NUMPIXELS2,R_selection,G_selection,B_selection,2);
    if (nombre_de_reussite==0) affiche(premier_pixel_1,dernier_pixel_1,NUMPIXELS1,R,G,B,1);
    if (nombre_de_reussite==1) affiche(premier_pixel_3,dernier_pixel_3,NUMPIXELS3,R,G,B,3);

    int2 = digitalRead(PINint2);
    int3 = digitalRead(PINint3);

    if ((int2 == LOW) && (int1 == HIGH) && (int3 == HIGH))  {
      pixels2.setPixelColor(premier_pixel_2, pixels2.Color(0, 0,0));
      premier_pixel_2=increment(NUMPIXELS2,premier_pixel_2);
      dernier_pixel_2=increment(NUMPIXELS2,dernier_pixel_2);
      position_centrale_2=increment(NUMPIXELS2,position_centrale_2);
      pixels2.setPixelColor(dernier_pixel_2, pixels2.Color(R_selection,G_selection,B_selection));
      pixels2.show();
      delay(DELAYanneau_2);
     }

    if ((int3 == LOW) && (int1 == HIGH) && (int2 == HIGH))
     {
      pixels2.setPixelColor(dernier_pixel_2, pixels2.Color(0, 0,0));
      premier_pixel_2=decrement(NUMPIXELS2,premier_pixel_2);
      dernier_pixel_2=decrement(NUMPIXELS2,dernier_pixel_2);
      position_centrale_2=decrement(NUMPIXELS2,position_centrale_2);
      pixels2.setPixelColor(premier_pixel_2, pixels2.Color(R_selection,G_selection,B_selection));
      pixels2.show();
      delay(DELAYanneau_2);
     }
   }

  /*----------------------------------------------------------*/
  //                   ring 3
  /*----------------------------------------------------------*/

  if(compteur==3) {
    affiche(premier_pixel_3,dernier_pixel_3,NUMPIXELS3,R_selection,G_selection,B_selection,3);
    if (nombre_de_reussite==0) affiche(premier_pixel_2,dernier_pixel_2,NUMPIXELS2,R,G,B,2);
    if (nombre_de_reussite==1) affiche(premier_pixel_4,dernier_pixel_4,NUMPIXELS4,R,G,B,4);

    int2 = digitalRead(PINint2);
    int3 = digitalRead(PINint3);

    if ((int2 == LOW) && (int1 == HIGH) && (int3 == HIGH))  {
      pixels3.setPixelColor(premier_pixel_3, pixels3.Color(0, 0,0));
      premier_pixel_3=increment(NUMPIXELS3,premier_pixel_3);
      dernier_pixel_3=increment(NUMPIXELS3,dernier_pixel_3);
      position_centrale_3=increment(NUMPIXELS3,position_centrale_3);
      pixels3.setPixelColor(dernier_pixel_3, pixels3.Color(R_selection,G_selection,B_selection));
      pixels3.show();
      delay(DELAYanneau_3);
     }

    if ((int3 == LOW) && (int1 == HIGH) && (int2 == HIGH))
     {
      pixels3.setPixelColor(dernier_pixel_3, pixels3.Color(0, 0,0));
      premier_pixel_3=decrement(NUMPIXELS3,premier_pixel_3);
      dernier_pixel_3=decrement(NUMPIXELS3,dernier_pixel_3);
      position_centrale_3=decrement(NUMPIXELS3,position_centrale_3);
      pixels3.setPixelColor(premier_pixel_3, pixels3.Color(R_selection,G_selection,B_selection));
      pixels3.show();
      delay(DELAYanneau_3);
     }
   }

  /*----------------------------------------------------------*/
  //                  ring 4
  /*----------------------------------------------------------*/

  if(compteur==4) {
    affiche(premier_pixel_4,dernier_pixel_4,NUMPIXELS4,R_selection,G_selection,B_selection,4);
    if (nombre_de_reussite==0) affiche(premier_pixel_3,dernier_pixel_3,NUMPIXELS3,R,G,B,3);
    if (nombre_de_reussite==1) affiche(premier_pixel_5,dernier_pixel_5,NUMPIXELS5,R,G,B,5);

    int2 = digitalRead(PINint2);
    int3 = digitalRead(PINint3);

    if ((int2 == LOW) && (int1 == HIGH) && (int3 == HIGH))  {
      pixels4.setPixelColor(premier_pixel_4, pixels4.Color(0, 0,0));
      premier_pixel_4=increment(NUMPIXELS4,premier_pixel_4);
      dernier_pixel_4=increment(NUMPIXELS4,dernier_pixel_4);
      position_centrale_4=increment(NUMPIXELS4,position_centrale_4);
      pixels4.setPixelColor(dernier_pixel_4, pixels4.Color(R_selection,G_selection,B_selection));
      pixels4.show();
      delay(DELAYanneau_4);
     }

    if ((int3 == LOW) && (int1 == HIGH) && (int2 == HIGH))
     {
      pixels4.setPixelColor(dernier_pixel_4, pixels4.Color(0, 0,0));
      premier_pixel_4=decrement(NUMPIXELS4,premier_pixel_4);
      dernier_pixel_4=decrement(NUMPIXELS4,dernier_pixel_4);
      position_centrale_4=decrement(NUMPIXELS4,position_centrale_4);
      pixels4.setPixelColor(premier_pixel_4, pixels4.Color(R_selection,G_selection,B_selection));
      pixels4.show();
      delay(DELAYanneau_4);
     }
   }

  /*----------------------------------------------------------*/
  //                 ring 5
  /*----------------------------------------------------------*/

  if(compteur==5) {
    affiche(premier_pixel_5,dernier_pixel_5,NUMPIXELS5,R_selection,G_selection,B_selection,5);
    if (nombre_de_reussite==0) affiche(premier_pixel_4,dernier_pixel_4,NUMPIXELS4,R,G,B,4);
    if (nombre_de_reussite==1) affiche(premier_pixel_6,dernier_pixel_6,NUMPIXELS6,R,G,B,6);

    int2 = digitalRead(PINint2);
    int3 = digitalRead(PINint3);

    if ((int2 == LOW) && (int1 == HIGH) && (int3 == HIGH))  {
      pixels5.setPixelColor(premier_pixel_5, pixels5.Color(0, 0,0));
      premier_pixel_5=increment(NUMPIXELS5,premier_pixel_5);
      dernier_pixel_5=increment(NUMPIXELS5,dernier_pixel_5);
      position_centrale_5=increment(NUMPIXELS5,position_centrale_5);
      pixels5.setPixelColor(dernier_pixel_5, pixels5.Color(R_selection,G_selection,B_selection));
      pixels5.show();
      delay(DELAYanneau_5);
     }
   if ((int3 == LOW) && (int1 == HIGH) && (int2 == HIGH))
    {
     pixels5.setPixelColor(dernier_pixel_5, pixels5.Color(0, 0,0));
     premier_pixel_5=decrement(NUMPIXELS5,premier_pixel_5);
     dernier_pixel_5=decrement(NUMPIXELS5,dernier_pixel_5);
     position_centrale_5=decrement(NUMPIXELS5,position_centrale_5);
     pixels5.setPixelColor(premier_pixel_5, pixels5.Color(R_selection,G_selection,B_selection));
     pixels5.show();
     delay(DELAYanneau_5);
    }
   }

  /*----------------------------------------------------------*/
  //                  ring 6
  /*----------------------------------------------------------*/

  if(compteur==6){
    affiche(premier_pixel_6,dernier_pixel_6,NUMPIXELS6,R_selection,G_selection,B_selection,6);
    if (nombre_de_reussite==0) affiche(premier_pixel_5,dernier_pixel_5,NUMPIXELS5,R,G,B,5);
    if (nombre_de_reussite==1) affiche(premier_pixel_1,dernier_pixel_1,NUMPIXELS1,R,G,B,1);

    int2 = digitalRead(PINint2);
    int3 = digitalRead(PINint3);

    if ((int2 == LOW) && (int1 == HIGH) && (int3 == HIGH))  {
      pixels6.setPixelColor(premier_pixel_6, pixels6.Color(0, 0,0));
      premier_pixel_6=increment(NUMPIXELS6,premier_pixel_6);
      dernier_pixel_6=increment(NUMPIXELS6,dernier_pixel_6);
      position_centrale_6=increment(NUMPIXELS6,position_centrale_6);
      pixels6.setPixelColor(dernier_pixel_6, pixels6.Color(R_selection,G_selection,B_selection));
      pixels6.show();
      delay(DELAYanneau_6);
     }

    if ((int3 == LOW) && (int1 == HIGH) && (int2 == HIGH))
     {
      pixels6.setPixelColor(dernier_pixel_6, pixels6.Color(0, 0,0));
      premier_pixel_6=decrement(NUMPIXELS6,premier_pixel_6);
      dernier_pixel_6=decrement(NUMPIXELS6,dernier_pixel_6);
      position_centrale_6=decrement(NUMPIXELS6,position_centrale_6);
      pixels6.setPixelColor(premier_pixel_6, pixels6.Color(R_selection,G_selection,B_selection));
      pixels6.show();
      delay(DELAYanneau_6);
     }
   }

  /*----------------------------------------------------------*/
  //              central ring and animation
  /*----------------------------------------------------------*/

  if((anneau_1_valide) && (anneau_2_valide) && (anneau_3_valide) &&
     (anneau_4_valide) && (anneau_5_valide) && (anneau_6_valide))
  {
    nombre_de_reussite+=1;
    if(nombre_de_reussite>2) nombre_de_reussite=1;

    if (nombre_de_reussite<2){
    affiche(0,NUMPIXELS7,NUMPIXELS7,R_position_valide, G_position_valide, B_position_valide,7);

    eteint_anneau(NUMPIXELS1,1);
    eteint_anneau(NUMPIXELS2,2);
    eteint_anneau(NUMPIXELS3,3);
    eteint_anneau(NUMPIXELS4,4);
    eteint_anneau(NUMPIXELS5,5);
    eteint_anneau(NUMPIXELS6,6);

    premier_pixel_1=0; //48
    dernier_pixel_1=44;
    position_centrale_1=46;
    affiche(premier_pixel_1, dernier_pixel_1, NUMPIXELS1, R, G, B, 1);

    premier_pixel_2=10; //40
    dernier_pixel_2=6;
    position_centrale_2=8;
    affiche(premier_pixel_2, dernier_pixel_2, NUMPIXELS2, R, G, B, 2);

    premier_pixel_3=31;//32
    dernier_pixel_3=27;
    position_centrale_3=29;
    affiche(premier_pixel_3, dernier_pixel_3, NUMPIXELS3, R, G, B, 3);

    premier_pixel_4=7;//24
    dernier_pixel_4=3;
    position_centrale_4=5;
    affiche(premier_pixel_4, dernier_pixel_4, NUMPIXELS4, R, G, B, 4);

    premier_pixel_5=13;//16
    dernier_pixel_5=9;
    position_centrale_5=11;
    affiche(premier_pixel_5, dernier_pixel_5, NUMPIXELS5, R, G, B, 5);

    premier_pixel_6=4;//12
    dernier_pixel_6=0;
    position_centrale_6=2;
    affiche(premier_pixel_6, dernier_pixel_6, NUMPIXELS6, R, G, B, 6);
  }


    if(nombre_de_reussite==1){
        POSITION_VALIDE_1=0;
        POSITION_VALIDE_2=0;
        POSITION_VALIDE_3=0;
        POSITION_VALIDE_4=0;
        POSITION_VALIDE_5=0;
        POSITION_VALIDE_6=0;}

    if(nombre_de_reussite==2){
        POSITION_VALIDE_1=24;
        POSITION_VALIDE_2=20;
        POSITION_VALIDE_3=16;
        POSITION_VALIDE_4=12;
        POSITION_VALIDE_5=8;
        POSITION_VALIDE_6=6;}

  anneau_1_valide=false,anneau_2_valide=false,anneau_3_valide=false;
  anneau_4_valide=false,anneau_5_valide=false,anneau_6_valide=false;
  }

  if(nombre_de_reussite==0) {
    eteint_anneau(NUMPIXELS7,7);
    eteint_anneau(NUMPIXELSBD1,21);
    eteint_anneau(NUMPIXELSBD2,22);
    eteint_anneau(NUMPIXELSBD3,23);
    premier_pixel_BG2=0;
    dernier_pixel_BG2=2;
    affiche(premier_pixel_BG2, dernier_pixel_BG2, NUMPIXELSBG2, R_barre_rouge,G_barre_rouge,B_barre_rouge, 12);
  }

  if (nombre_de_reussite==1){
    affiche(0,NUMPIXELS7,NUMPIXELS7,R_position_valide, G_position_valide, B_position_valide,7);
    eteint_anneau(NUMPIXELSBG2,12);
  }

  if (nombre_de_reussite==2){ // end animation
    eteint_anneau(NUMPIXELS1,1);
    eteint_anneau(NUMPIXELS2,2);
    eteint_anneau(NUMPIXELS3,3);
    eteint_anneau(NUMPIXELS4,4);
    eteint_anneau(NUMPIXELS5,5);
    eteint_anneau(NUMPIXELS6,6);
    eteint_anneau(NUMPIXELS7,7);

    affiche(0, 2, NUMPIXELSBG2, R_barre_rouge,G_barre_rouge,B_barre_rouge, 12); delay(100);

    pixels1.setPixelColor(24, pixels1.Color(R_position_valide,G_position_valide,B_position_valide)); pixels1.show();delay(100);
    pixels2.setPixelColor(20, pixels2.Color(R_position_valide,G_position_valide,B_position_valide)); pixels2.show();delay(100);
    pixels3.setPixelColor(16, pixels3.Color(R_position_valide,G_position_valide,B_position_valide)); pixels3.show();delay(100);
    pixels4.setPixelColor(12, pixels4.Color(R_position_valide,G_position_valide,B_position_valide)); pixels4.show();delay(100);
    pixels5.setPixelColor( 8, pixels5.Color(R_position_valide,G_position_valide,B_position_valide)); pixels5.show();delay(100);
    pixels6.setPixelColor( 6, pixels6.Color(R_position_valide,G_position_valide,B_position_valide)); pixels6.show();delay(100);

    affiche(0,NUMPIXELS7,NUMPIXELS7,R_position_valide, G_position_valide, B_position_valide,7);delay(100);

    pixels6.setPixelColor(0, pixels6.Color(R_position_valide,G_position_valide,B_position_valide)); pixels6.show();delay(100);
    pixels5.setPixelColor(0, pixels5.Color(R_position_valide,G_position_valide,B_position_valide)); pixels5.show();delay(100);
    pixels4.setPixelColor(0, pixels4.Color(R_position_valide,G_position_valide,B_position_valide)); pixels4.show();delay(100);
    pixels3.setPixelColor(0, pixels3.Color(R_position_valide,G_position_valide,B_position_valide)); pixels3.show();delay(100);
    pixels2.setPixelColor(0, pixels2.Color(R_position_valide,G_position_valide,B_position_valide)); pixels2.show();delay(100);
    pixels1.setPixelColor(0, pixels1.Color(R_position_valide,G_position_valide,B_position_valide)); pixels1.show();delay(100);

    affiche(0, 2, NUMPIXELSBD2, R_barre_rouge,G_barre_rouge,B_barre_rouge, 22); delay(100);

    delay(5000); // reset after 5 seconds
    reset_software();
  }
}
