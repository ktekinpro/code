/*
Dictionary implementation and text correction in C language


The text is analyzed letter by letter (must be ASCII code)

The dictionary is implemented using a tree which is divided by the 26 letters in the alphabet
and the " ' " character



	Advanced Programming &
	Data structure

	5th May 2019
*/

#include "projet.h"

int main(int argc, char* argv[])
{
    FILE* fp;
		FILE* fp2;
    Tree mon_arbre;
		char nom_dico[50];
		char nom_texte_a_corrige[50];
    int choix=1;

		if (argc==3)
		{
      strcpy(nom_dico, argv[1]);
      strcpy(nom_texte_a_corrige, argv[2]);
      choix=0; // on ne lance pas le mode interface
		}

		if (argc==2 && strcmp("-h", argv[1]) == 0)
		{ //affichage de l'aide

    printf("\n####################################################");
    printf("\n############              Aide            ##########");
    printf("\n####################################################");


    printf("\n\nBienvenue et merci d'utiliser notre projet");
    printf("\nCe programme a pour but d'implémenter un dictionnaire et d'afficher sur la sortie standard les mots");
    printf("\ndu texte à corriger qui ne sont pas dans le dictionnaire");
    printf("\n\nPour cela, vous aurez besoin d'un dictionnaire avec chaque mots séparés par des '\\n'");
    printf("\n\nEt d'un fichier à corriger");
    printf("\n\nA noter que seul les mots en codage ASCII sont pris en compte, typiquement du texte en anglais.");
    printf("\nVous trouverez un exemple de texte et de dictionnaire dans : 'mon_texte.txt' et 'dico.txt'");
    printf("\n\nVous pouvez quitter le programme puis le relancer avec une seule ligne de commande : ");
    printf("\n\n~$./main dico.txt mon_texte.texte");
    printf("\n\nOu bien continuer et suivre le guide");


    printf("\n\n Continuer ?\n1. Oui\n2. Non\n\nVotre choix [1/2] ? : ");
    scanf("%d",&choix);

    if (choix!=1) return 0; // on s'arrête

		}

    if (choix==1)  //mode interface
    {
      printf("\n####################################################");
      printf("\n############         Dictionnaire         ##########");
      printf("\n####################################################");


      printf("\n\nConstruction d'un dictionnaire et correction d'un fichier");
      printf("\nVeuillez entrer un nom de dictionnaire valide");
      printf("\nle dictionnaire doit être code en ASCII et être de la forme : 'un mot par ligne'");
      printf("\n\n Nom du dictionnaire (exemple : dico.txt) : ");
      scanf("%s",nom_dico);

      printf("\n\n\nCorrection d'un fichier");
      printf("\nVeuillez entrer un nom de fichier valide");
      printf("\nle fichier doit être code en ASCII");
      printf("\n\n Nom du fichier à corriger (exemple : mon_texte.txt) : ");
      scanf("%s",nom_texte_a_corrige);
    }

		/* construction du dictionnaire à partir du fichier */
		fp=fopen(nom_dico,"r");
		if(fp==NULL) // est-ce que le fichier 'nom_dico' s'est bien ouvert ?
		{
			printf("\nErreur ! \n\n fichier '%s' inexistant ou impossible à ouvrir\n\n",nom_dico);
      printf("Remarque : les noms de fichier avec un espace ne fonctionnent pas\n\n");
			return 0;
		}

		/* lecture du texte à corriger */
		fp2=fopen(nom_texte_a_corrige,"r");
		if(fp2==NULL) // est-ce que le fichier 'nom_texte_a_corrige' s'est bien ouvert ?
		{
			printf("\nErreur ! \n\n fichier '%s' inexistant ou impossible à ouvrir\n\n",nom_texte_a_corrige);
      printf("Remarque : les noms de fichier avec un espace ne fonctionnent pas\n\n");
      fclose(fp); // on ferme fp car il a été ouvert juste avant mais on s'en est pas servi
			return 0;
		}

    /* -------------- Tout s'est bien passé, on commence l'analyse --------- */

    /* initialisation */
    mk_empty_tree(&mon_arbre); //init arbre
    cons_tree(&mon_arbre, '#'); //construit racine, caractère  :  #

    /* construit dictionnaire */
    load_tree(fp, &mon_arbre);
    fclose(fp);


    printf("\n####################################################");
    printf("\n############          Correction          ##########");
    printf("\n####################################################\n\n");

		/* correction : affiche les mots non présents dans le dictionnaire */
		corrige_texte(mon_arbre, fp2);

    /* free */
		fclose(fp2);
		free_tree(&mon_arbre);
    return 0;
}
