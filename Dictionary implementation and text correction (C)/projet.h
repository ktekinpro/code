/*
Dictionary implementation and text correction in C language


The text is analyzed letter by letter (must be ASCII code)

The dictionary is implemented using a tree which is divided by the 26 letters in the alphabet
and the " ' " character



	Advanced Programming &
	Data structure

	5th May 2019
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define taille_alphabet 27 //taille de notre alphabet :  de ' à z sans accent et sans majuscule
#define taille_max 50 //taille maximal d'un mot


typedef struct node {
	char lettre;
  bool end_of_world ;
	struct node* children[taille_alphabet];
}Node, *PtNode, *Tree;


/*
Action initilisant notre pointeur de noeud à NULL

Paramètre :
un pointeur d'arbre : ptr_tree
*/
void mk_empty_tree(struct node ** ptr_tree);

/*
Action qui construit et initialise les valeurs d'un noeud

Paramètres :
un pointeur d'arbre : ptr_tree
une lettre de type char : lettre

Remarque : end_of_world vaut toujours false lors de la construction de notre noeud
c'est après, une fois que le noeud a été construit et que les conditions de fin de mot
ont été validées que l'ont marquera end_of_world à true, indépendamment de cons_tree
*/
void cons_tree(struct node ** ptr_tree, char lettre);


/*
Action réalisant l'ajout de la lettre x à sa bonne place dans l'arbre, en suivant
l'ordre d'un mot

Paramètres :
un pointeur d'arbre : ptr_tree
une lettre de type char : lettre
*/
void add(struct node **ptr_tree, char x);


/*
Action réalisant le chargement d'un dictionnaire

Paramètres :
un pointeur d'arbre : ptr_tree
un pointeur de fichier : fp
*/
void load_tree(FILE *fp, struct node **ptr_tree);

/*
Fonction de recherche
renvoie 0 si le mot n'est pas présent dans le dictionnaire, 1 si non
En cas d'erreur, renvoie -1

Paramètres :
un dictionnaire de type arbre : arbre
un mot de type char* : mot
la taille du mot de type int : taille_mot
*/
int recherche_dans_arbre(Tree arbre, char* mot, int taille_mot);

/*
Action parcourant un fichier texte et recherchant, pour chaque mot, s'il est
présent dans le dictionnaire passé en paramètre

Paramètres :
un dictionnaire de type arbre : dictionnaire
un pointeur de fichier : fp
*/
void corrige_texte(Tree dictionnaire, FILE* fp);

/*
Action désallouant en mémoire les cellules de l'arbre
Paramètre :
un pointeur d'arbre : ptr_tree
*/
void free_tree(struct node ** ptr_tree);
