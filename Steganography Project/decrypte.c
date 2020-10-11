#include "libstegano.h"

int main(int argc, char const *argv[]) {
    if(argc !=3){
        printf("\n\nFormat de l'image : ppm P3\tFormat de la clef : lettre minuscule ASCII\n");
        printf("\nTaille max de la clef : %d\n",TAILLE_CHAINE_MAX);

        printf("\n\nVeuillez entrer : \t<exec>\tnom_image.ppm\tvigenere_key\n");
        printf("\nExemple : \t./decrypte img-sortie.ppm masuperclef\n\n");
        return -1;
    }

    char* mon_image = malloc(sizeof(char) * strlen(argv[1]) +1);
	char* key = malloc(sizeof(char) * strlen(argv[2]) +1);
    strcpy(mon_image,argv[1]);
	strcpy(key,argv[2]);

	if(strlen(key)>=TAILLE_CHAINE_MAX){
		printf("Erreur taille de chaine trop grande, ne pas depasser %d caractères\n",TAILLE_CHAINE_MAX);
		exit(0);
	}

    Image* I = Charger(mon_image);
	decrypt_stegano_v2(I,key);

	/* free */
	DelImage(I);
	free(mon_image);
	free(key);
    return 0;
}
