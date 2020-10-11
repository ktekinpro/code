/*
The library allows us to hide an ASCII message into a PPM P3 iamge and to recover it
by using steganographic methods and the Vigenere cipher.
*/

#include "libstegano.h"

Image* NouvelleImage(int w,int h)
{
	Image* I = malloc(sizeof(Image));
	I->w = w;
	I->h = h;
	I->dat = calloc(1,w*h*sizeof(Pixel*));
	return I;
}

Image* Charger(const char* fichier)
{
	int i,j,max;
	char buf[10];
	Image* I;
	FILE* F = fopen(fichier,"r");
    int r,g,b;

	if (!F){
		printf("\nFichier '%s' non valide : ",fichier);fflush(stdout);
		perror("");
		exit(0);
    }

	int valeur_retour = fscanf(F,"%s %d %d %d",buf,&i,&j,&max);
	if(valeur_retour != 4){
		perror("\nvaleur de retour fscanf Image* Charger(const char* fichier)");
		exit(0);
	}

    printf("\n[info] Format de l'image '%s' : %s %d %d %d \n",fichier,buf,i,j,max);

    I = NouvelleImage(i,j);

    for(i=0;i<I->w*I->h;i++)
	{
		int valeur_retour2 = fscanf(F,"%d %d %d",&r,&g,&b);
		if(valeur_retour2 != 3){
			perror("\nvaleur de retour fscanf Image* Charger(const char* fichier) :");
			exit(0);
		}

		I->dat[i].r = (unsigned char)r;
		I->dat[i].g = (unsigned char)g;
		I->dat[i].b = (unsigned char)b;
	}

	fclose(F);
	return I;
}

int Sauver(Image* I,const char* fichier)
{
	int i;
	FILE* F = fopen(fichier,"w");
	if (!F){
		perror("\nProbleme lors de l'enregistrement");
		exit(0);
    }

    fprintf(F,"P3\n%d %d\n255\n",I->w,I->h);

    for(i=0; i<I->w*I->h; i++){
		fprintf(F,"%d %d %d ",I->dat[i].r,I->dat[i].g,I->dat[i].b);
    }

    fclose(F);
	return 0;
}

void DelImage(Image* I)
{
	if (I)
	{
		free(I->dat);
		free(I);
	}
}


void GetPartChar(char* ma_chaine, unsigned char* tableau_chaine, int taille_tableau_chaine, int taille_chaine){
	for(int i=0, j=0; i<taille_tableau_chaine  && j<taille_chaine; i+=4, j++){
		tableau_chaine[i]=(ma_chaine[j] & 0xC0) >> 6;
		tableau_chaine[i+1]=(ma_chaine[j] & 0x30) >> 4;
		tableau_chaine[i+2]=(ma_chaine[j] & 0x0C) >> 2;
		tableau_chaine[i+3]=(ma_chaine[j] & 0x03);
	}
}


void SetPartChar(char* ma_chaine, unsigned char* tableau_chaine, int taille_tableau_chaine, int taille_chaine){
	for(int i=0, j=0; i<taille_tableau_chaine  && j<taille_chaine; i+=4, j++){
		ma_chaine[j]=(tableau_chaine[i]<<6)+(tableau_chaine[i+1]<<4)+(tableau_chaine[i+2]<<2)+(tableau_chaine[i+3]);
	}
}

void stegano_v2(Image* I, char* ma_chaine, char* vigenerekey){
	int j=0;
	int new_radom_position = 0;
	int nombre_element = 0;
	int taille_image=I->w*I->h;
	int taille_chaine = strlen(ma_chaine) +1;
	int taille_tableau_chaine = (strlen(ma_chaine) +1) * 4;
	unsigned char pixel_b_modif;
	unsigned char pixel_b = 0;
	unsigned char* tableau_chaine = malloc(taille_tableau_chaine);
	int* tableau_position = malloc(taille_tableau_chaine);

	srand(hachage(vigenerekey,I->w*I->h));
	char encryptedOut[strlen(ma_chaine)+1];
	vigenere_encryption(ma_chaine,encryptedOut,vigenerekey);
	printf("\n[cryptage] Steganographie \n[cryptage] ma chaine :      %s\n",ma_chaine);
	printf("[cryptage] vigenere key :   %s\n[cryptage] chaine cryptee : %s\n",vigenerekey,encryptedOut);

	GetPartChar(encryptedOut, tableau_chaine, taille_tableau_chaine, taille_chaine);

	for(j=0; j<taille_tableau_chaine; j++){
		new_radom_position = get_new_radom_position(tableau_position,nombre_element,taille_image);
		nombre_element++;

		pixel_b = I->dat[new_radom_position].b;
		pixel_b_modif = pixel_b & 0x000000FC;
		pixel_b_modif |= tableau_chaine[j];
		I->dat[new_radom_position].b = pixel_b_modif;

	}
	free(tableau_chaine);
	free(tableau_position);
}

void decrypt_stegano_v2(Image* I, char* vigenerekey){
	char ma_chaine_reconstruite[TAILLE_CHAINE_MAX];
	int tableau_position[TAILLE_CHAINE_MAX * 4];
	unsigned char tableau_chaine[TAILLE_CHAINE_MAX * 4];
	unsigned char pixel_b = 0;
	unsigned char pixel_n_0=1;
	unsigned char pixel_n_1=1;
	unsigned char pixel_n_2=1;
	unsigned char pixel_n_3=1;
	int new_radom_position=0;
	int nombre_element=0;
	int taille_image=I->w*I->h;
	int taille_tableau_chaine=0, taille_chaine=0;
	int j=0;
	int cpt=0;

	srand(hachage(vigenerekey,I->w*I->h));

	for(int i=0; i<TAILLE_CHAINE_MAX * 4; i++){
		new_radom_position = get_new_radom_position(tableau_position,nombre_element,taille_image);
		nombre_element++;

		pixel_b = I->dat[new_radom_position].b;
		pixel_n_0 = pixel_b & 0x3;

		tableau_chaine[j]=pixel_n_0;
		j++;

		if(j>=TAILLE_CHAINE_MAX * 4){
			printf("Erreur indice tableau chaine trop grand, > %d\n",j);
			break;
		}

		if(pixel_n_0== 0 && pixel_n_1==0 && pixel_n_2==0 && pixel_n_3==0){
			cpt++;
		}
		if(cpt==4){
			printf("\n[decryptage] Chaine chiffree retrouvee dans l'image :      ");
			break;
		}

		pixel_n_1 = pixel_n_0;
		pixel_n_2 = pixel_n_1;
		pixel_n_3 = pixel_n_2;
	}

	taille_tableau_chaine=j+1;
	taille_chaine=taille_tableau_chaine/4;
	SetPartChar(ma_chaine_reconstruite, tableau_chaine, taille_tableau_chaine, taille_chaine);
	printf("%s\n",ma_chaine_reconstruite);

	char decryptedOut[strlen(ma_chaine_reconstruite)+1];
	vigenere_decryption(ma_chaine_reconstruite,decryptedOut,vigenerekey);
	printf("[decryptage] Le message apres decryptage de Vigenere est : %s\n",decryptedOut);
}

void vigenere_encryption(char* msg_to_encrypt, char* output, char* key){
	int msgLen = strlen(msg_to_encrypt);
	int keyLen = strlen(key);
	int i, j;

	char newKey[msgLen+1];
	char encryptedMsg[msgLen+1];

	for(i = 0, j = 0; i < msgLen; ++i, ++j){
		if(j == keyLen) j = 0;
		newKey[i] = key[j];
	}
	newKey[msgLen] = '\0';

	for(i = 0; i < msgLen; ++i){
		encryptedMsg[i] = ((msg_to_encrypt[i]-'a' + newKey[i]-'a') % 26) +'a';
	}
	encryptedMsg[msgLen] = '\0';

	strcpy(output,encryptedMsg);
}


void vigenere_decryption(char* encryptedMsg, char*  output, char* key){
	int msgLen = strlen(encryptedMsg);
	int keyLen = strlen(key);
	int i, j;

	char newKey[msgLen+1];
	char decryptedMsg[msgLen+1];

	for(i = 0, j = 0; i < msgLen; ++i, ++j){
		if(j == keyLen)
			j = 0;
		newKey[i] = key[j];
	}
	newKey[msgLen] = '\0';

	for(i = 0; i < msgLen; ++i)
		decryptedMsg[i] = (((encryptedMsg[i] - newKey[i]) + 26) % 26) + 'a';
	decryptedMsg[msgLen] = '\0';

	strcpy(output,decryptedMsg);
}


int hachage(char* key, int taille_image){
	unsigned long somme=1;
	int modulo=taille_image/4;
	int valeur_hachage;

	for(unsigned int i=0;i<strlen(key);i++){
		somme*=(key[i]-'a')+1;
		somme = somme % INT_MAX;
	}

	valeur_hachage = somme % modulo;

	return valeur_hachage;
}

int is_here(int* tableau,int nombre_element, int element){
	for(int i=0; i<=nombre_element; i++){
		if(element==tableau[i]){
			return 1;
		}
	}
	return 0;
}

int get_new_radom_position(int* tableau,int nombre_element, int taille_image){
	int i=-1;
	int random_position=0;

	do {
		random_position=rand()%(taille_image);
		i++;
		if(i>=taille_image){
			printf("\nErreur... tous les pixels ont ete remplis\n");
			exit(0);
		}
	} while(is_here(tableau,nombre_element,random_position)!=0);

	return random_position;
}
