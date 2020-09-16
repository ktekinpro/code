# Project : Bijective Transformations of Images
# May 2017

# Pillow for Python 3.5.2 is needed for the use of this project
# https://pypi.org/project/Pillow/

# The pictures that are included with the repository
# are necessary to execute the test
##############################################


from PIL import Image
import math
import sys

# NOTE : Pour lancer le programme, appuyez sur F5.

##### Algorithme de base #####

def algo(permut,img_entree,it,img_sortie=None):
    '''
    algo(permut,img_entree,it) renvoie l'image img_entree transformée
    à l'aide de la permutation permut, it fois.

    Paramètres :
    -permut le nom d'une fonction de permutation
    -img_entree le nom de l'image sur laquelle on veut travailler
    -img_sortie le nom de l'image de sortie, par défaut None, parce que
    l'utilisateur n'est pas obligé d'enregistrer l'image transformée, dans ce
    cas il ne renseignera rien en 4ème paramètre (cf Exemples)
    -it un entier correspondant au nombre de permutation à effectuer
    /!\ il s'agit du premier algorithme travaillant sur l'image au rang n-1,
    peut prendre du temps si it >= 10^2

    CU : it un entier positif

    Exemples :
    >>> image=Image.open('galets.png')
    >>> image.size==algo(p_boulanger,'galets.png',1).size
    True
    >>> image.mode==algo(p_photomaton,'galets.png',1).mode
    True

    NOTE : la version plus rapide se situe à la fin du code (juste avant
    les fonctions pour l'interface utilisateur)
    '''
    assert it>=0, 'it doit être positif'
    image=Image.open(img_entree)
    for k in range (it) :
        img2=Image.new(image.mode, image.size)
        if image.mode=="P":
            img2.putpalette(image.getpalette())
        for i in range(image.size[0]):
            for n in range(image.size[1]):
                (x,y)=permut((i,n),image.size)
                img2.putpixel((x,y),image.getpixel((i,n)))
        image=img2
    if img_sortie!=None :
        img2.save(img_sortie)
    return img2


##### Fonctions de permutations #####

#Les fonctions de permutations sont nommées de la forme p_permutation.


#### permutation identité ####

def p_identite(coord,dim):
    '''
    p_identite(coord,dim) renvoie les nouvelles coordonnées du pixel positionné en
    coord, dans une image de dimension dim, transformées par la permutation "identité".

    Paramètres:
    -coord un couple de coordonnées compris entre (0,0) et dim.
    -dim un couple donnant les dimensions d'une image.

    CU : aucune

    Exemples :
    >>> p_identite((22,22),(256,256))
    (22, 22)
    >>> p_identite((253,145),(256,256))
    (253, 145)
    >>> d=p_identite((253,145),(256,256))
    >>> 0<=d[0]<=256 and 0<=d[1]<=256
    True
    '''
    x, y = coord
    larg, haut = dim
    assert 0 <= x < larg and 0 <= y < haut, 'coordonnées non valides'
    return coord



#### Permutation photomaton ####

def p_photomaton(coord,dim):
    '''
    p_photomaton(coord,dim) renvoie les nouvelles coordonnées du pixel positionné en
    coord, dans une image de dimension dim, transformées par la permutation photomaton.

    Paramètres:
    -coord un couple de coordonnées compris entre (0,0) et dim.
    -dim un couple donnant les dimensions d'une image.

    CU : aucune

    Exemples :
    >>> p_photomaton((253,145),(256,256))
    (254, 200)
    >>> d=p_photomaton((253,145),(256,256))
    >>> 0<=d[0]<=256 and 0<=d[1]<=256
    True
    '''
    x,y=coord
    larg,haut=dim
    assert 0 <= x < larg and 0 <= y < haut, 'coordonnées non valides'
    if x%2==0:
        if y%2==0:
            return (x//2,y//2)
        else:
            return (x//2,(haut+y)//2)
    else:
        if y%2==0:
            return((larg+x)//2,y//2)
        else:
            return((larg+x)//2,(haut+y)//2)



#### Permutation du boulanger ####

def p_boulanger(coord,dim):
    '''
    p_boulanger(coord,dim) renvoie les nouvelles coordonnées du pixel positionné en
    coord, dans une image de dimension dim, transformées par la permutation boulanger.

    Paramètres:
    -coord un couple de coordonnées compris entre (0,0) et dim.
    -dim un couple donnant les dimensions d'une image.

    CU : aucune

    Exemples :
    >>> p_boulanger((253,145),(256,256))
    (4, 183)
    >>> d=p_boulanger((253,145),(256,256))
    >>> 0<=d[0]<=256 and 0<=d[1]<=256
    True
    '''
    x,y=coord
    larg,haut=dim
    assert 0 <= x < larg and 0 <= y < haut, 'coordonnées non valides'
    if y%2==0:
        nc=(x*2,y//2)
    else:
        nc=(x*2+1,y//2)
    if nc[0]<larg:
        return nc
    else:
        return(2*larg-1-nc[0],(haut-1)-nc[1])



#### Permutation symétrie horizontale ####

def p_sym_h(coord,dim):
    '''
    p_sym_h(coord,dim) renvoie les nouvelles coordonnées du pixel positionné en
    coord, dans une image de dimension dim, transformées par la permutation symétrie horizontale.

    Paramètres:
    -coord un couple de coordonnées compris entre (0,0) et dim.
    -dim un couple donnant les dimensions d'une image.

    CU : aucune

    Exemples :
    >>> p_sym_h((253,145),(256,256))
    (253, 110)
    >>> d=p_sym_h((253,145),(256,256))
    >>> 0<=d[0]<=256 and 0<=d[1]<=256
    True
    '''
    x,y=coord
    larg,haut=dim
    assert 0 <= x < larg and 0 <= y < haut, 'coordonnées non valides'
    return (x,haut-1-y)



#### Permutation symétrie verticale ####

def p_sym_v(coord,dim):
    '''
    p_sym_v(coord,dim) renvoie les nouvelles coordonnées du pixel positionné en
    coord, dans une image de dimension dim, transformées par la permutation symétrie verticale.

    Paramètres:
    -coord un couple de coordonnées compris entre (0,0) et dim.
    -dim un couple donnant les dimensions d'une image.

    CU : aucune

    Exemples :
    >>> p_sym_v((253,145),(256,256))
    (2, 145)
    >>> d=p_sym_v((253,145),(256,256))
    >>> 0<=d[0]<=256 and 0<=d[1]<=256
    True
    '''
    x,y=coord
    larg,haut=dim
    assert 0 <= x < larg and 0 <= y < haut, 'coordonnées non valides'
    return (larg-1-x,y)



#### Permutation symétrie centrale ####

def p_sym_c(coord,dim):
    '''
    p_sym_c(coord,dim) renvoie les nouvelles coordonnées du pixel positionné en
    coord, dans une image de dimension dim, transformées par la permutation symétrie centrale.

    Paramètres:
    -coord un couple de coordonnées compris entre (0,0) et dim.
    -dim un couple donnant les dimensions d'une image.

    CU : aucune

    Exemples :
    >>> p_sym_c((253,145),(256,256))
    (2, 110)
    >>> d=p_sym_c((253,145),(256,256))
    >>> 0<=d[0]<=256 and 0<=d[1]<=256
    True
    '''
    x,y=coord
    larg,haut=dim
    assert 0 <= x < larg and 0 <= y < haut, 'coordonnées non valides'
    return (larg-1-x,haut-1-y)



#### Permutation défilement vertical ####

def p_defilement_v(coord,dim):
    '''
    p_defilement_v(coord,dim) renvoie les nouvelles coordonnées du pixel positionné en
    coord, dans une image de dimension dim, transformées par la permutation "défilement vertical".

    Paramètres:
    -coord un couple de coordonnées compris entre (0,0) et dim.
    -dim un couple donnant les dimensions d'une image.

    CU : aucune

    Exemples :
    >>> p_defilement_v((253,145),(256,256))
    (253, 146)
    >>> d=p_defilement_v((253,145),(256,256))
    >>> 0<=d[0]<=256 and 0<=d[1]<=256
    True
    '''
    x,y=coord
    larg,haut=dim
    assert 0 <= x < larg and 0 <= y < haut, 'coordonnées non valides'
    if y==haut-1:
        return (x,0)
    else:
        return (x,y+1)



#### Permutation défilement horizontal ####

def p_defilement_h(coord,dim):
    '''
    p_defilement_h(coord,dim) renvoie les nouvelles coordonnées du pixel positionné en
    coord, dans une image de dimension dim, transformées par la permutation "défilement horizontal".

    Paramètres:
    -coord un couple de coordonnées compris entre (0,0) et dim.
    -dim un couple donnant les dimensions d'une image.

    CU : aucune

    Exemples :
    >>> p_defilement_h((253,145),(256,256))
    (254, 145)
    >>> d=p_defilement_h((253,145),(256,256))
    >>> 0<=d[0]<=256 and 0<=d[1]<=256
    True
    '''
    x,y=coord
    larg,haut=dim
    assert 0 <= x < larg and 0 <= y < haut, 'coordonnées non valides'
    if x==larg-1:
        return (0,y)
    else:
        return (x+1,y)



#### Permutation concentrique ####

def p_concentrique(coord,dim):
    '''
    p_concentrique(coord,dim) renvoie les nouvelles coordonnées du pixel positionné en
    coord, dans une image de dimension dim, transformées par la permutation "concentrique".

    Paramètres:
    -coord un couple de coordonnées compris entre (0,0) et dim.
    -dim un couple donnant les dimensions d'une image.

    CU : Image de dimensions paires, dim doit être un doublet d'entier paire

    Exemples :
    >>> p_concentrique((253,145),(256,256))
    (253, 146)
    >>> d=p_concentrique((253,145),(256,256))
    >>> 0<=d[0]<=256 and 0<=d[1]<=256
    True
    '''
    x,y=coord
    larg,haut=dim
    assert 0 <= x < larg and 0 <= y < haut, 'coordonnées non valides'
    if y<haut//2:
        if y<=x<haut-1-y:
            return (x+1,y)
        elif x>=(haut-1-y):
            return(x,y+1)
        else:
            return(x,y-1)
    else:
        if y>=x>haut-1-y:
            return (x-1,y)
        elif x>(haut-1-y):
            return(x,y+1)
        else:
            return(x,y-1)


#### Permutation boustrophedon #####

def p_boustrophedon(coord,dim):
    '''
    p_boustrophedon(coord,dim) renvoie les nouvelles coordonnées du pixel positionné en
    coord, dans une image de dimension dim, transformées par la permutation "boustrophedon".

    Paramètres:
    -coord un couple de coordonnées compris entre (0,0) et dim.
    -dim un couple donnant les dimensions d'une image.

    CU : aucune

    Exemples :
    >>> p_boustrophedon((1,1),(256,256))
    (0, 1)
    >>> d=p_boustrophedon((1,1),(256,256))
    >>> 0<=d[0]<=256 and 0<=d[1]<=256
    True
    '''
    x,y=coord
    larg,haut=dim
    assert 0 <= x < larg and 0 <= y < haut, 'coordonnées non valides'
    if (y%2==0 and x==larg-1) or (y%2==1 and x==0):
        if y==haut-1:
            return (x,0)
        else:
            return (x,y+1)
    else:
        if y%2==0:
            return (x+1,y)
        else:
            return (x-1,y)



##### Cycles des permutations #####

def cycles(permut,dim):
    '''
    cycles(permut,dim) renvoie la liste des cycles de la permutation
    permut effectuée sur une image de dimension dim.

    Paramètres :
    -permut le nom d'une fonction de permutation
    -dim un couple donnant les dimensions d'une image.
    /!\ la fonction ne doit pas être utilisée directement, l'ordinateur
    prendra beaucoup de temps à afficher la liste des cycles.
    On préférera définir une variable égale au résultat retourné
    par la fonction pour des dimensions supérieures à 10^2

    CU : aucune

    Exemple :
    >>> cycles(p_concentrique,(4,4))
    [[(0, 0), (1, 0), (2, 0), (3, 0), (3, 1), (3, 2), (3, 3), (2, 3), (1, 3), (0, 3), (0, 2), (0, 1)], [(1, 1), (2, 1), (2, 2), (1, 2)]]
    '''
    lst_cycle=[]
    ens=set()
    for i in range(dim[0]):
        for e in range(dim[1]):
            if not (i,e) in ens:
                ens.add((i,e))
                p=permut((i,e),dim)
                ens.add(p)
                l=[(i,e)]
                while p!=(i,e):
                    l.append(p)
                    p=permut(p,dim)
                    ens.add(p)
                lst_cycle.append(l)
    return lst_cycle


def pgcd(m,n): #utilisée dans la fonction ppcm
    '''
    pgdc(m,n) retourne le pgcd (plus grand diviseur commun)
    des deux entiers m et n passés en paramètre.

    CU : aucune

    >>> pgcd(8,20)
    4
    '''
    while m%n:
        r=m%n
        m=n
        n=r
    return n

def ppcm(m,n): #utilisée dans la fonction ordre
    '''
    ppcm(m,n) retourne le ppcm (plus petit multiple commun)
    des deux entiers m et n passés en paramètre.

    CU : aucune

    Exemple :
    >>> ppcm(7,12)
    84
    '''
    return m*n//pgcd(m,n)


def ordre(permut,dim):
    '''
    ordre(permut,dim) retourne l'ordre d'une permutation de l'image, c'est-à-dire
    au bout de combien de coup l'image se régénère en fonction de la permutation
    et de sa dimension passées en paramètre.

    Paramètres:
    -permut le nom d'une fonction de permutation
    -dim un couple donnant les dimensions d'une image.

    CU : aucune

    Exemple :
    >>> ordre(p_sym_v,(16,16))
    2
    '''
    c=cycles(permut,dim)
    a=len(c[0])
    for i in c[1:]:
        a=ppcm(a,len(i))
    return int(a)


##### Calculer loin #####

def algo_loin(permut,img_entree,it):
    '''
    algo_loin(permut,img_entree,it) renvoie l'image img_entree transformée
    à l'aide de la permutation permut, it fois. Elle est (beaucoup) plus rapide
    que la fonction algo car elle applique directement la transformation de
    l'image à partir de son cycle de permutation au rang it.

    Paramètres :
    -permut le nom d'une fonction de permutation
    -img_entree le nom d'une image sur laquelle on veut travailler
     de la forme : 'galets.png'
    -it un entier correspondant au nombre de permutation à effectuer

    CU : it un entier positif

    Exemples :
    >>> image=Image.open('galets.png')
    >>> image.size==algo_loin(p_boulanger,'galets.png',65320).size
    True
    >>> image.mode==algo_loin(p_photomaton,'galets.png',1000000001).mode
    True
    '''
    assert it>=0 and it%1==0, 'it doit être un entier positif'
    image=Image.open(img_entree)
    c=cycles(permut,image.size)
    img2=Image.new(image.mode, image.size)
    if image.mode=="P":
            img2.putpalette(image.getpalette())
    for n in c:
        for i in range(len(n)):
            img2.putpixel(n[(n.index(n[i])+it)%len(n)],image.getpixel(n[i]))
    return img2



##### Message secret #####


def solution(img_entree='msg_secret.png',img_sortie='solution.png'):
    '''
    solution(nom_image,nom_sortie) affiche le message dissimulé dans mystere
    et l'enregistre dans le fichier solution.

    Paramètres:
    -mystere est le nom du fichier d'origine de l'image (image secrète).
    -solution est le nom du fichier où la solution est enregistrée.
    les noms de fichiers sont de la forme 'msg_secret.png'

    NOTE : dans notre dossier, l'image secrète se nomme 'msg_secret.png' et la
    solution 'solution.png' (valeurs par défaut de notre fonction).

    CU : aucune

    Exemples :
    >>> mystere=Image.open('msg_secret.png')
    >>> solution=Image.open('solution.png')
    >>> mystere.size==solution.size
    True
    >>> mystere.mode==solution.mode
    True
    '''
    img=Image.open(img_entree)
    algo_loin(p_concentrique,img_entree,ordre(p_concentrique,img.size)-765765764675587676,img_sortie)
    algo_loin(p_photomaton,img_sortie,ordre(p_photomaton,img.size)-5,img_sortie)
    algo_loin(p_concentrique,img_sortie,ordre(p_concentrique,img.size)-9868756755876,img_sortie)
    algo_loin(p_boustrophedon,img_sortie,ordre(p_boustrophedon,img.size)-10000,img_sortie)
    algo_loin(p_boulanger,img_sortie,ordre(p_boulanger,img.size)-1,img_sortie)
    image=Image.open(img_sortie)
    image.show()


##### Tests #####

if __name__ == '__main__':
    import doctest
    doctest.testmod()


########################################################################################
################################
# Interface pour l'utilisateur #
################################



def my_input(prompt='') :
    '''
    Paramètre optionnel : prompt (str) chaîne à imprimer avant lecture

    Valeur renvoyée : (str) chaîne lue depuis l'entrée standard

    Effet de bord : interrompt le flux d'exécution pour lire une chaîne
    de caractères depuis l'entrée standard.

    CU : aucune
    '''
    sys.stdout.write(prompt)
    return sys.stdin.readline().rstrip('\n')


def aide() :
    '''
    Procédure affichant la documentaton

    Paramètre : aucun

    CU : aucune
    '''
    liste_aide=["\n\tNotes à l'utilisateur\n", '\n',
                "Le programme Projet.py permet d'effectuer des transformations d'images grâce aux fonctions de permutation présentes dans le fichier.\n",
                '\n', 'Pour lancer le programme, appuyer tout simplement sur la touche F5.\n',
                "Ensuite, il ne vous reste plus qu'à répondre aux questions qui vous seront posées, en respectant les contraintes qui vous seront indiquées. ",
                "En cas de réponse non valide (ne respectant les contraintes d'utilisation), vous serez amené à saisir de nouveaux l'élément demandé.\n",
                'Si vous voulez lancer le programme depuis le Shell (pour une seconde utilisation ou bien après un arrêt de votre par), tapez : user()\n',
                '\n', '\n', "A tout moment vous pouvez interrompre l'exécution du script en appuyant simultanément sur la touche  \n",
                "CTRL et sur la touche C. Vous pourrez alors choisir entre fermer le programme (tout simplement) ",
                "ou  bien fermer le programme en retournant la documentation.\n",
                '\n', '\n', 'Fonctionnalités du programme :\n',
                "Vous pouvez, entre autre, transformer une image selon les 10 fonctions de permutation mises à votre disposition,",
                "choisir l'image d'entrée, le nombre de permutation que vous voulez effectuer et pour finir vous aurez le choix ",
                "entre juste visionner l'image ou bien l'enregistrer avec le nom de votre choix puis la visionner. \n", '\n',
                "Ensuite, vous pouvez déterminer le cycle de permutation d'une fonction donnée en fonction des dimensions de votre choix ",
                "(il est recommandé d'utiliser de petites dimensions si non le résultat sera beaucoup trop grand).\n",
                '\n', "En lien avec le cycle, vous pouvez déterminer au bout de combien de temps une image se régénère en fonction ",
                "de la permutation de votre choix. Vous aurez le choix entre fournir les dimensions de votre choix ou bien saisir ",
                "le nom d'une image présente dans le même dossier que le programme.\n", '\n', "Ensuite, vous avez la possibilité de ",
                "résoudre l'image mystère. Vous aurez le choix entre voir directement la réponse (plus rapide) ou bien résoudre cette ",
                "image puis la visionner (environ une minute selon les ressources de votre machine).\n",
                '\n', "Pour finir, vous avez la possibilité d'afficher la documentation et d'en savoir plus sur les auteurs de ce programme.\n",
                '\n', '\n', 'Bugs : \n', 'Aucun bug connu à ce jour.']
    aide=''.join(liste_aide)
    print(aide)



def user () :
    '''
    Fonction affichant le résutat souhaité par l'utilisateur :
    Au cours de l'exécution de la fonction, les paramatères de la fonction
    à utiliser sont demandés. Une fois les paramètres saisies, la
    fonction renvoie ce que l'utilisateur a choisi de faire.

    Paramètre : aucun

    CU : aucune
    '''

    fin='\nFin du programme'
    pas_valide=True
    pas_reponse_valide=True
    reponse=None

    while pas_valide:
        try : #Teste si on fait CTRL+C


            message_fin='\nPressez simultanément sur la touche CTRL et C pour mettre fin au programme à tout moment ou afficher la documentation'

            print(36*'#')
            print("# Transformation bijective d'image #")
            print(36*'#')
            print("\n// Veuillez choisir l'option voulue //")
            print("\nAstuce :\n Pour choisir l'option, appuyez sur la touche correspondante à votre choix puis appuyez sur la touche ENTRER")


            # première étape : on demande ce que l'utilisateur veut faire, quelle fonction il veut utiliser #
            proposition=("\nQue voulez-vous faire ?\n\na.Transformer une image\nb.Connaître le cycle de permutation d'une fonction\nc.Savoir au bout de combien de temps mon image se regénère\nd.Révéler l'image mystère\nh.Afficher l'aide\ne.En savoir plus")
            print(message_fin)
            print(proposition)
            liste_des_choix='abcdeh'
            choix=my_input('\n Votre choix :')
            while choix not in liste_des_choix : # pour donner une seconde chance à l'utilisateur
                print('Veuillez indiquer un choix valide\nATTENTION les majuscules ne sont pas prises en compte')
                print(message_fin)
                choix=my_input('\n Votre choix :')


            # deuxième étape : on demande les paramètres des fonctions
            ## on demande d'abord quelle fonction de permutation utiliser
            if choix in 'abc': # les fonctions qui utilisent la variable permut
                proposition=("\nSur qu'elle fonction voulez-vous travailler ?\n\na.Photomaton\nb.Boulanger\nc.Concentrique\nd.Boustrophédon\ne.Défilement horizontal\nf.Défilement vertical\ng.Symétrie centrale\nh.Symétrie verticale\ni.Symétrie horizontale\nj.Identité")
                print(proposition)
                dic_permut={'a':p_photomaton,'b':p_boulanger,'c':p_concentrique,
                            'd':p_boustrophedon,'e':p_defilement_h,'f':p_defilement_v,
                            'g':p_sym_c,'h':p_sym_v,'i':p_sym_h,'j':p_identite}

                choix2=my_input('\n Votre choix :')
                while choix2 not in dic_permut : # pour donner une seconde chance à l'utilisateur
                    print('Veuillez indiquer un choix valide\nATTENTION les majuscules ne sont pas prises en compte')
                    print(message_fin)
                    choix2=my_input('\n Votre choix :')
                permut=dic_permut[choix2]


            ## on demande sur qu'elle dimension travailler si on veut utiliser les fonctions cycles ou ordre
            choix9=None
            if choix=='c': # les fonctions qui utilisent la variable dim
                proposition=("\nSouhaitez-vous travailler sur une image?\na.Oui, je souhaite travailler sur une image\nb.Non, je souhaite travailler seulement avec des dimensions quelconques")
                print(proposition)
                choix9=my_input('\n Votre choix :')

            CU="\nPour la fonction concentrique, il ne faut pas utiliser des dimensions impaires (se référer aux contraintes d'utilisation de la fonction)"
            #dimension
            if choix9=='b' or choix=='b':
                proposition=("\nSur quelle dimension voulez-vous travailler ?\n\nNOTE : Si vous voulez connaître le cycle de permutation d'une fonction, utiliser des petites valeurs, si non le résultat sera trop long.\n\nPour renseigner une dimension, veuillez rentrer un tuple contenant 2 entiers positifs correspondant à la longeur de l'image voulue")
                print(proposition)
                print("\nPar exemple : si vous voulez connaître le cycle d'une image, tapez (4,4)")
                choix3=my_input('\n Votre choix :')

                pas_valide=True
                while pas_valide :
                    try :
                        l=choix3.split('(') #l=['', '15,15)']
                        l2=l[1].split(')') #l2=['15,15', '']
                        l3=l2[0].split(',') #l3=['15', '15']
                        a=int(l3[0])
                        b=int(l3[1])
                        dim=(a,b)
                        if choix2=='c' and a%2!=0 or b%2!=0 : #CU concentrique : dimension paire
                            print('Veuillez indiquer un choix valide\nATTENTION seul les entiers positifs sont valides\nDe plus, pensez à bien renseigner un doublet\nPar exemple : (4,4)')
                            print(message_fin)
                            print(CU)
                            choix3=my_input('\n Votre choix :')
                        pas_valide=False
                    except ValueError and IndexError :
                            print('Veuillez indiquer un choix valide\nATTENTION seul les entiers positifs sont valides\nDe plus, pensez à bien renseigner un doublet\nPar exemple : (4,4)')
                            print(message_fin)
                            choix3=my_input('\n Votre choix :')




            ## on demande le nombre de fois que l'utilisateur veut transformé son image et sur quelle image travailler pour la fonction algo_loin

            # On demande sur qu'elle image travailler
            if choix=='a' or choix9=='a':
                proposition=("\nSur qu'elle image voulez-vous travailler ? \nNOTE : le nom doit être renseigner SANS apostrophe et avoir une bonne extension.\nPar exemple : écureuil.jpg")
                print(proposition)
                choix7=my_input('\n Votre choix :')
                pas_valide=True
                while pas_valide : #petite modification pour redonner une 'chance' à l'utilisateur
                    try :
                        img_test=Image.open(choix7)
                        if choix2=='c' and img_test.size[0]%2!=0 or img_test.size[1]%2!=0 : #CU concentrique : dimension paire
                            print("Le nom doit être renseigner SANS apostrophe et avoir une bonne extension.\nPar exemple : écureuil.jpg\n/!\le fichier doit se trouver dans le même dossier que le programme Python")
                            print("\nATTENTION : renseigner le nom SANS apostrophe, ce que vous renseignez est déja de type str")
                            print(message_fin)
                            print(CU)
                            choix7=my_input('\n Votre choix :')
                        pas_valide=False
                    except FileNotFoundError and AttributeError and KeyError and OSError :
                        print("Le nom doit être renseigner SANS apostrophe et avoir une bonne extension.\nPar exemple : écureuil.jpg\n/!\le fichier doit se trouver dans le même dossier que le programme Python")
                        print("\nATTENTION : renseigner le nom SANS apostrophe, ce que vous renseignez est déja de type str")
                        print(message_fin)
                        print(CU)
                        choix7=my_input('\n Votre choix :')
                img_entree=choix7
                if choix9=='a' :
                    dim=Image.open(img_entree).size

            #Itération
            if choix in 'a': # la fonction qui utilise la variable it
                proposition=("\nCombien de fois voulez-vous appliquer la transformation?\n\nNOTE : il vous faut renseigner un entier positif")
                print(proposition)
                choix4=my_input('\n Votre choix :')
                pas_valide=True
                while pas_valide : #petite modification pour redonner une 'chance' à l'utilisateur
                    try :
                        it1=int(choix4) #test si choix4 est une chaine de caractère contenant un entier
                        if it1>=0 : #il faut un entier positif
                            it=it1
                            pas_valide=False
                        else:
                            print('Veuillez renseigner un entier positif')
                            print(message_fin)
                            choix4=my_input('\n Votre choix :')
                    except ValueError : # exception à l'erreur : on demande à l'utilisateur s'il veut continuer
                        print('Veuillez renseigner un entier positif')
                        print(message_fin)
                        choix4=my_input('\n Votre choix :')


            ## On demande si l'utilisateur souhaite enregistrer son image transformée pour la fonction algo_loin
            if choix=='a' :
                proposition=("\nSouhaitez-vous enregistrer l'image transformée ?\n\na.Oui,je souhaite enregistrer la nouvelle image puis la visionner\nb.Non, je veux juste visionner l'image")
                print(proposition)
                liste_des_choix5='ab'
                choix5=my_input('\n Votre choix :')
                while choix5 not in liste_des_choix5 : # pour donner une seconde chance à l'utilisateur
                    print('Veuillez indiquer un choix valide\nATTENTION les majuscules ne sont pas prises en compte')
                    print(message_fin)
                    choix5=my_input('\n Votre choix :')
            else :
                choix5=None # nécessaire si non la varibale n'est pas défini


            ## si l'utilisateur a demandé de l'aide :
            if choix=='h' :
                aide()

            ## si l'utilisateur souhaite en savoir plus :
            if choix=='e' :
                print("Projet d'AP1 réalisé en avril 2017 par le groupe image_04 issu de la PEIP11")


            ## si l'utilisateur souhaite résoudre l'image mystère :
            if choix=='d':
                proposition=("\nQue voulez-vous faire ?\n\na.Résoudre l'image puis la visionner /!\ Prends environ 1 minutes (selon les ressources de votre machine)\n\nb.Visionner directement l'image résolu (déja résolu au préalable par nos soin)")
                print(proposition)
                liste_des_choix10='ab'
                choix10=my_input('\n Votre choix :')
                while choix10 not in liste_des_choix10 : # pour donner une seconde chance à l'utilisateur
                    print('Veuillez indiquer un choix valide\nATTENTION les majuscules ne sont pas prises en compte')
                    print(message_fin)
                    choix10=my_input('\n Votre choix :')



            ################## Dernière étape ####################
            #présentation des résultats
            r=None


            if choix=='a' :
                if choix5=='a' : ## si l'utilisateur veut enregistrer son image transformé :
                    proposition=("\nVeuillez renseigner le nom du fichier sur lequelle enregistrer la photo transformée : \nNOTE : le nom doit être renseigner sans apostrophe et avoir une bonne extension.\nPar exemple : image_04.jpg")
                    print(proposition)
                    choix8=my_input('\n Votre choix :')
                    pas_valide=True
                    while pas_valide : #petite modification pour redonner une 'chance' à l'utilisateur
                        try :
                            algo_loin(permut,img_entree,it).save(choix8)
                            algo_loin(permut,img_entree,it).show()
                            pas_valide=False
                        except KeyError :
                            print("Le nom doit être renseigner sans apostrophe et avoir une bonne extension.\nPar exemple : image_04.jpg")
                            print("\nATTENTION : renseigner le nom SANS apostrophe, ce que vous renseignez est déja de type str")
                            print(message_fin)
                            choix8=my_input('\n Votre choix :')
                else :
                    img_sortie=None
                    return algo_loin(permut,img_entree,it).show()


            if choix=='b' :
                print(cycles(permut,dim))
            if choix=='c' :
                print(ordre(permut,dim))
            if choix=='d' :
                if choix10=='a' :
                    solution()
                else :
                    image=Image.open('solution.png')
                    image.show()

############################## retour au premier try, interception du CTRL+C ######################################

            pas_valide=False # si pas de CTRL+C la boucle s'arrête (donc le programme aussi)


        except KeyboardInterrupt : # si CTRL+C a été saisi, on demande comment arrêter le programme
            proposition=("\nComment voulez-vous arrêter le programme ?\n\na.Arrêter le programme en retournant la documentation\nb.Arrêter simplement le programme")
            print(proposition)

            #Si on refait CTRL+C, une deuxième, troisième fois...
            while pas_reponse_valide :
                try :
                    reponse=my_input('\n Votre choix :')
                    pas_reponse_valide=False
                    if reponse=='a' :
                        aide()
                        print(fin)
                        pas_valide=False
                        pas_reponse_valide=False
                    elif reponse !=None : # Dans tout les autres cas on arrête le programme sauf si on retape CTRL+C
                        print(fin)
                        pas_valide=False
                        pas_reponse_valide=False
                # on redemande comment arrêter le programme et on recommence au try
                except KeyboardInterrupt :
                    proposition=("\nComment voulez-vous arrêter le programme ?\n\na.Arrêter le programme en retournant la documentation\nb.Arrêter simplement le programme")
                    print(proposition)

############### Fin de l'interface utilisateur

user() # exécution automatique du programme lorsque l'on appuie sur F5
