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


/*
Action initilisant notre pointeur de noeud à NULL

Paramètre :
un pointeur d'arbre : ptr_tree
*/
void mk_empty_tree(struct node ** ptr_tree)
{
	*ptr_tree = NULL;
}


/*
Action qui construit et initialise les valeurs d'un noeud

Paramètres :
un pointeur d'arbre : ptr_tree
une lettre de type char : lettre

Remarque : end_of_world vaut toujours false lors de la construction de notre noeud
c'est après, une fois que le noeud a été construit et que les conditions de fin de mot
ont été validées que l'ont marquera end_of_world à true, indépendamment de cons_tree
*/
void cons_tree(struct node ** ptr_tree, char lettre)
{
	*ptr_tree = malloc(sizeof(struct node));

    (*ptr_tree) -> lettre = lettre;
		(*ptr_tree) -> end_of_world = false;

    for (int i=0; i<taille_alphabet ; i++)
    {
        (*ptr_tree)->children[i]=NULL;
    }
}


/*
Action réalisant l'ajout de la lettre x à sa bonne place dans l'arbre, en suivant
l'ordre d'un mot

Paramètres :
un pointeur d'arbre : ptr_tree
une lettre de type char : lettre
*/
void add(struct node **ptr_tree, char x)
{
	int indice;

	if(*ptr_tree == NULL)
    {
        cons_tree(ptr_tree, x);
        return;
    }

  indice=x-96;
  if ((indice<0) || (indice>=taille_alphabet))
	{
      return;
  }

  if ((*ptr_tree)->children[indice] == NULL)
  {
      cons_tree(&((*ptr_tree) -> children[indice]), x);
  }
}


/*
Action réalisant le chargement d'un dictionnaire

Paramètres :
un pointeur d'arbre : ptr_tree
un pointeur de fichier : fp
*/
void load_tree(FILE *fp, struct node **ptr_tree)
{
    char valeur;
    Tree debut;
		Tree temp;
    debut=*ptr_tree;
		temp=*ptr_tree;

    while ( !feof(fp) && !ferror(fp))
    {
       if (fscanf(fp, "%c", &valeur) ==1)
       {
           if((valeur>= 96) && (valeur<=122))
           {
              add(&temp, valeur);
						  temp=temp->children[valeur-96];
           }
					 else if ((valeur>=65) && (valeur <=90))
					 {
						 valeur+=32;
						 add(&temp, valeur);
						 temp=temp->children[valeur-96];
					 }
					 else if (valeur==39)
					 {
						 valeur=96;
						 add(&temp, valeur);
						 temp=temp->children[valeur-96];
					 }

          if (valeur == 10)
          {
                temp->end_of_world=true;
                temp=debut;
				}
       }

    }
    if(feof(fp)) printf("\nFin du fichier dictionnaire\n");
		if(ferror(fp)) printf("\n\nErreur dans le fichier dictionnaire\n\n");
}


/*
Fonction de recherche
renvoie 0 si le mot n'est pas présent dans le dictionnaire, 1 si non
En cas d'erreur, renvoie -1

Paramètres :
un dictionnaire de type arbre : arbre
un mot de type char* : mot
la taille du mot de type int : taille_mot
*/
int recherche_dans_arbre(Tree arbre, char* mot, int taille_mot)
{
    int i;
    int indice;

    for (i=0; i<taille_mot; i++)
    {
        indice=mot[i]-96;

				if (arbre==NULL)
				{
					printf("\n\n\tErreur pointeur de noeud :  %p\n",arbre);
					return -1;
				}

        if ((indice<0) || (indice>taille_alphabet))
        {
	        return 0;
        }


        if(arbre->children[indice]==NULL) return 0;


        else
           arbre=arbre->children[indice];
    }
    if (arbre==NULL)
    {
        printf("\n\n\tErreur dans l'arbre...ou pas : arbre = %p\n",arbre);
        return -1;
    }

    if(arbre->end_of_world == true)
        return 1;
    else
			  return 0;
}


/*
Action parcourant un fichier texte et recherchant, pour chaque mot, s'il est
présent dans le dictionnaire passé en paramètre

Paramètres :
un dictionnaire de type arbre : dictionnaire
un pointeur de fichier : fp
*/
void corrige_texte(Tree dictionnaire, FILE* fp)
{
  char lettre[2];
 	int taille=0;
	int trouve;
	int nb=0;
	int nb_faute=0;
	bool pas_de_faute=true;

	char* chaine2;
	chaine2 = malloc (sizeof (*chaine2) * taille_max);
	strcpy(chaine2, "");


	while ( !feof(fp) && !ferror(fp) && (fscanf(fp, "%c", lettre) == 1))
	{
			if ((lettre[0]>=96) && (lettre[0] <=122))
			{
				taille++;
				lettre[1]='\0';
				strcat(chaine2, lettre);
			}
			else if ((lettre[0]>=65) && (lettre[0] <=90))
			{
				taille++;
				lettre[0]+=32;
				lettre[1]='\0';
				strcat(chaine2, lettre);
			}
			else if (lettre[0]==39)
			{
				taille++;
				lettre[0]=96;
				lettre[1]='\0';
				strcat(chaine2, lettre);
			}
			else
			{
				if (taille>0)
				{
					nb++;
					trouve = recherche_dans_arbre(dictionnaire, chaine2, taille);
					if (trouve==0)
					{
						printf("\nle mot n° %d '%s' est mal orthographie car inconnu du dictionnaire",nb,chaine2);
						nb_faute++;
						pas_de_faute=false;
					}
					taille=0;

					chaine2 = realloc (chaine2, sizeof (char) * taille_max);
					strcpy(chaine2, "");
				}
			}
	}

	if(!pas_de_faute) printf("\n\n%d mots mals orthographies sur %d mots lus \n",nb_faute,nb);
	if(pas_de_faute) printf("\n\nAucune erreur d'orthographe detectee\n");

	if(feof(fp)) printf("\n\nFin de l'analyse du fichier\n");
	if(ferror(fp)) printf("\n\nErreur dans le fichier a corriger\n\n");
	free(chaine2);
}

/*
Action désallouant en mémoire les cellules de l'arbre
Paramètre :
un pointeur d'arbre : ptr_tree
*/
void free_tree(struct node ** ptr_tree)
{

	if(*ptr_tree!=NULL)
    {
		for(int i=0 ; i<taille_alphabet ; i++)
			 {
				 free_tree(&(*ptr_tree)->children[i]);
			 }
     free(*ptr_tree);
    }
		if(*ptr_tree!=NULL) mk_empty_tree(ptr_tree);
}
