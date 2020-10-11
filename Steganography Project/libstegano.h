/*
Header files of the "libstegano" library
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define TAILLE_CHAINE_MAX 500

typedef struct Pixel
{
	unsigned char r,g,b;
} Pixel;

typedef struct Image
{
	int w,h;
	Pixel* dat;
} Image;

Image* NouvelleImage(int w,int h);
Image* Charger(const char* fichier);
int Sauver(Image* I,const char* fichier);
int hachage(char* key, int taille_image);
int is_here(int* tableau,int nombre_element, int element);
int get_new_radom_position(int* tableau,int nombre_element, int taille_image);
void DelImage(Image* I);
void GetPartChar(char* ma_chaine, unsigned char* tableau_chaine, int taille_tableau_chaine, int taille_chaine);
void SetPartChar(char* ma_chaine, unsigned char* tableau_chaine, int taille_tableau_chaine, int taille_chaine);
void stegano_v2(Image* I, char* ma_chaine, char* vigenerekey);
void decrypt_stegano_v2(Image* I, char* vigenerekey);
void vigenere_encryption(char* msg_to_encrypt, char* output, char* key);
void vigenere_decryption(char* encryptedMsg, char*  output, char* key);
