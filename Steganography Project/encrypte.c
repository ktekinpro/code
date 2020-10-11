#include "libstegano.h"

int main(int argc, char const *argv[]) {
    if(argc !=5){
        printf("\n\nFormat de l'image : ppm P3\tFormat de la clef et du message : lettre minuscule ASCII\n");
        printf("\nTaille max de la clef : %d\t Taille max du message : %d\n",TAILLE_CHAINE_MAX,TAILLE_CHAINE_MAX);

        printf("\n\nVeuillez entrer : \t<exec>\tnom_image_entree.ppm\tvigenere_key\tchaine_de_caractere_ascii\timage_de_sortie.ppm\n");
        printf("\nExemple : \t./encrypte img.ppm masuperclef jetestemonprogramme img-sortie.ppm\n\n");
        return -1;
    }

    char* mon_image = malloc(sizeof(char) * strlen(argv[1]) +1);
	char* key = malloc(sizeof(char) * strlen(argv[2]) +1);
	char* ma_chaine = malloc(sizeof(char) * strlen(argv[3]) +1);
    char* mon_image_de_sortie = malloc(sizeof(char) * strlen(argv[4]) +1);

    strcpy(mon_image,argv[1]);
	strcpy(key,argv[2]);
	strcpy(ma_chaine,argv[3]);
    strcpy(mon_image_de_sortie,argv[4]);

	if((strlen(key)>=TAILLE_CHAINE_MAX) || (strlen(ma_chaine)>=TAILLE_CHAINE_MAX)){
		printf("Erreur taille de chaine trop grande, ne pas depasser %d caractères\n",TAILLE_CHAINE_MAX);
		exit(0);
	}

    Image* I = Charger(mon_image);
    stegano_v2(I,ma_chaine,key);
	Sauver(I,mon_image_de_sortie);

	/* free */
	DelImage(I);
	free(mon_image);
	free(key);
	free(ma_chaine);
    free(mon_image_de_sortie);
    return 0;
}
