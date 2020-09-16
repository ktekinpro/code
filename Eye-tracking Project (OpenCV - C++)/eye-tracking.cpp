/*
                Eye-tracking Project (OpenCV - C++)

Program interface is in French

    Aim of the project

The goal of this project is to develop an application, which control the mouse
of the computer according to the positions of user's eyes. To control the mouse
we have implemented 2 algorithms : one called "joystick" and the other one
"touchscreen". Each of these algorithms has been developed to respond to
human-computer interaction problematic.

    How the program works

First of all, open the Makefile and write the path to your opencv directory.
For example, if the "opencvfile" directory is in "/home/my_path/opencvfiles"
write : PATH_TO_DIR =  /home/my_path

To compile the program, type : ~$ make
To begin, run : ~$ ./tpIHM

Then, choose what you want to do.
Finally, follow the instructions, which will be printed on the terminal.

*/
#include <X11/Xlib.h>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "opencv2/opencv.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#define X0_f 0 //début de la fenêtre en X
#define Y0_f 0 //début de la fenêtre en Y
#define LARGEUR 500 //largeur (widht) de la fenêtre
#define HAUTEUR 500//taille / hauteur (height) de la fenêtre
#define E 2 //precision pour l'orientation de la tête
#define KERNEL_J 20 //position centrale du joystick
#define INCREMENT_JOYSTICK 20

using namespace std;
using namespace cv;

int position_jostick[2]={LARGEUR/2,HAUTEUR/2};


/*--------------------------------------------------------------------------------*/
//                      affiche_orientation_tete
/*---------------------------------------------------------------------------------*/
int affiche_orientation_tete(int deltaX,int deltaY,int x_centre_avant,int y_centre_avant){
    if(((deltaX <= 0 + E) && (deltaX >= 0 - E) && (deltaY <= 0 + E) && (deltaY >= 0 - E))
    || ((deltaX == x_centre_avant) && (deltaY == y_centre_avant))){
        printf("\tNEUTRE\n");
        return 0;
    }
    if((deltaX >= 0 + E) && (deltaY >= 0 - E) && (deltaY <= 0 + E)){
        printf("\tGAUCHE\n");
        return 1;
    }
    if((deltaX <= 0 - E) && (deltaY >= 0 - E) && (deltaY <= 0 + E)){
        printf("\tDROITE\n");
        return 2;
    }
    if((deltaY >= 0 + E) && (deltaX >= 0 - E) && (deltaX <= 0 + E)){
        printf("\tBAS\n");
        return 3;
    }
    if((deltaY <= 0 - E) && (deltaX >= 0 - E) && (deltaX <= 0 + E)){
        printf("\tHAUT\n");
        return 4;
    }
    if((deltaX >= 0 + E) && (deltaY <= 0 - E)){
        printf("\tHAUT GAUCHE\n");
        return 5;
    }
    if((deltaX <= 0 - E) && (deltaY <= 0 - E)){
        printf("\tHAUT DROITE\n");
        return 6;
    }
    if((deltaX >= 0 + E) && (deltaY >= 0 + E)){
        printf("\tBAS GAUCHE\n");
        return 7;
    }
    if((deltaX <= 0 - E) && (deltaY >= 0 + E)){
        printf("\tBAS DROITE\n");
        return 8;
    }
    printf("\tposition inconnue\n");
    return -1;
}
/*--------------------------------------------------------------------------------*/
//                      calcul_position_joystick
/*---------------------------------------------------------------------------------*/
void calcul_position_joystick(int valeur_retour){
    switch (valeur_retour) {
        case 1://gauche x--
            position_jostick[0]-=INCREMENT_JOYSTICK;
            break;
        case 2://droite x++
            position_jostick[0]+=INCREMENT_JOYSTICK;
            break;
        case 3://bas y++
            position_jostick[1]+=INCREMENT_JOYSTICK;
            break;
        case 4://haut y--
            position_jostick[1]-=INCREMENT_JOYSTICK;
            break;
        case 5://haut gauche x--, y--
            position_jostick[0]-=INCREMENT_JOYSTICK;
            position_jostick[1]-=INCREMENT_JOYSTICK;
            break;
        case 6://haut droite x++, y--
            position_jostick[0]+=INCREMENT_JOYSTICK;
            position_jostick[1]-=INCREMENT_JOYSTICK;
            break;
        case 7://bas gauche x--, y++
            position_jostick[0]-=INCREMENT_JOYSTICK;
            position_jostick[1]+=INCREMENT_JOYSTICK;
            break;
        case 8://bas droite x++, y++
            position_jostick[0]+=INCREMENT_JOYSTICK;
            position_jostick[1]+=INCREMENT_JOYSTICK;
            break;
    }
}

/*--------------------------------------------------------------------------------*/
//                          gestion des clics Xlib
/*---------------------------------------------------------------------------------*/
void click (Display *display, int button)
{
    // Create and setting up the event
    XEvent event;
    memset (&event, 0, sizeof (event));
    event.xbutton.button = button;
    event.xbutton.same_screen = True;
    event.xbutton.subwindow = DefaultRootWindow (display);

    while (event.xbutton.subwindow)
    {
        cout << "hello event.xbutton.subwindow" << endl;
        event.xbutton.window = event.xbutton.subwindow;
        XQueryPointer (display, event.xbutton.window,
             &event.xbutton.root, &event.xbutton.subwindow,
             &event.xbutton.x_root, &event.xbutton.y_root,
             &event.xbutton.x, &event.xbutton.y,
             &event.xbutton.state);
    }

    // Press
    event.type = ButtonPress;
    if (XSendEvent (display, PointerWindow, True, ButtonPressMask, &event) == 0)
        fprintf (stderr, "Error to send the event!\n");
    XFlush (display);
    usleep (1);

    // Release
    event.type = ButtonRelease;
    if (XSendEvent (display, PointerWindow, True, ButtonReleaseMask, &event) == 0)
        fprintf (stderr, "Error to send the event!\n");
    XFlush (display);
    usleep (1);
}

/*--------------------------------------------------------------------------------*/
//                                  main
/*---------------------------------------------------------------------------------*/
int main(int, char**)
{
    int choix=1;
    printf("Que voulez-vous faire ?\n");
    printf("1. Voir l'orientation du visage\n");
    printf("2. Tester l'implementation \"joystick\"\n");
    printf("3. Test l'implementation \"touchscreen\"\n");
    printf("choix [1-3] : \n");
    scanf("%d",&choix);

/*--------------------------------------------------------------------------------*/
//                              déclaration variables
/*---------------------------------------------------------------------------------*/
    int nb_visage = 0, nb_yeux=0;
    int i=0;
    int x_ref=0, y_ref=0;
    int nb_deplacement=0;
    int deltaX=0, deltaY=0;
    int x0_avant=0, x0_actuel=0, y0_avant=0, y0_actuel=0;
    int x1_avant=0, x1_actuel=0, y1_avant=0, y1_actuel=0;
    int x_centre_avant=0, x_centre_actuel=0, y_centre_avant=0, y_centre_actuel=0;
    int val_retour_avant = -1, val_retour_actuelle = -1;
    int radius =5, radius2=5;
    int x1_ref_rect=0, y1_ref_rect=0, x2_ref_rect=0, y2_ref_rect=0;
    int nb_touches=0;
    int cpt_g=0, cpt_d=0;

    bool continu=false, deja_fait=false;
    bool est_dans_carre_avant = false, est_dans_carre_maintenant = false;

/*--------------------------------------------------------------------------------*/
//                              opencv
/*---------------------------------------------------------------------------------*/
    vector<Mat> images;
    vector<int> labels;

    CascadeClassifier haar_cascade;
    haar_cascade.load("./haarcascade_frontalface_default.xml");
    CascadeClassifier eyes_cascade;
    eyes_cascade.load("./haarcascade_eye_tree_eyeglasses.xml");

    VideoCapture cap(0);
    if(!cap.isOpened())
        return -1;

    Mat grImage;
    namedWindow("face",1);
    moveWindow("face",500,0);

/*--------------------------------------------------------------------------------*/
//                              souris Xlib
/*---------------------------------------------------------------------------------*/
    GC gc;
    Display* dpy;
    int src;
    Window win, root;
    unsigned long white_pixel, black_pixel;

    dpy = XOpenDisplay(0);
    src = DefaultScreen (dpy);
    gc = DefaultGC (dpy, src);
    root = RootWindow (dpy, src);
    white_pixel = WhitePixel (dpy, src);
    black_pixel = BlackPixel (dpy, src);
    XSetBackground (dpy, gc,black_pixel);
    XSetForeground (dpy, gc,white_pixel);
    win = XCreateSimpleWindow (dpy,root,X0_f,X0_f,LARGEUR,HAUTEUR,5,white_pixel,black_pixel);
    XSelectInput (dpy, win, ExposureMask | KeyPressMask);
    XStoreName (dpy, win, "IHM"); // titre de la fenetre
    XMapWindow (dpy, win);

    //initialise la position du curseur au milieu de l'écran
    int posX = LARGEUR/2;
    int posY = HAUTEUR/2;

/*--------------------------------------------------------------------------------*/
//                instructions avant de rentrer dans la boucle infinie
/*---------------------------------------------------------------------------------*/
printf("################################################################\n");
printf("\t\t\nINFO\n\n\n");

    switch (choix) {
        case 1:
            printf("Orientation du visage : effectuez des mouvements et regardez ce qu'il s'affiche sur le terminal\n");
            printf("\nAppuyez 1 fois sur une touche pour commencer, 2 fois pour quitter la boucle infinie, une troisieme fois pour quitter le programme\n");
            break;
        case 2:
            printf("Joystick : placez vous confortablement puis appuyez sur une touche pour verouiller le centre du joystick ( = carre noir)\n");
            printf("Appuyez 1 fois pour verouiller le carre noir, 2 fois pour quitter la boucle, 3 fois pour quitter le programme\n");
            printf("Il faut revenir au centre, dans le carre noir pour effectuer un 2eme mouvement\n");
            break;
        case 3:
            printf("Touchscreen : placez vous confortablement puis appuyez sur une touche pour commencer\n");
            printf("Appuyez 1 fois pour commencer, 2 fois pour quitter la boucle, 3 fois pour quitter le programme\n");
            printf("Pour refaire un autre mouvement, fermer les yeux, replacez votre tete en situation confortable, puis rouvrez vos yeux\n");
            break;
        default:
            printf("Choix invalide... affichage de l'orientaion du visage (mode par defaut)\n\n");
            printf("Orientation du visage : effectuez des mouvements et regardez ce qu'il s'affiche sur le terminal\n");
            choix=1;
    }


printf("\n################################################################\n");
/*--------------------------------------------------------------------------------*/
//                               loop
/*---------------------------------------------------------------------------------*/
    for(;;)
    {
        Mat frame;
        cap >> frame;
        cvtColor(frame, grImage, CV_BGR2GRAY);
        vector< Rect_<int> > faces;
        haar_cascade.detectMultiScale(grImage, faces);
        nb_visage = faces.size();

        if(nb_visage==1){//1 seul visage détecté
            i=0;
            Rect face_i = faces[i];
            rectangle(frame, face_i, CV_RGB(0, 255,0), 1);

            Mat face_frame = frame(faces[i]);
            Mat faceROI;
            cvtColor(face_frame, faceROI, CV_BGR2GRAY);
            std::vector<Rect> eyes;
            eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(30, 30) );

            nb_yeux=eyes.size();
/*--------------------------------------------------------------------------------*/
//                      1 visage, 2 yeux
/*---------------------------------------------------------------------------------*/
            if(nb_yeux==2){
                cpt_g=0;
                cpt_d=0;

                Point yeux1(faces[i].x + eyes[0].x + eyes[0].width*0.5, faces[i].y + eyes[0].y + eyes[0].height*0.5);
                Point yeux2(faces[i].x + eyes[1].x + eyes[1].width*0.5, faces[i].y + eyes[1].y + eyes[1].height*0.5);

                /* calcul du milieu et des deltas */
                x0_actuel = faces[i].x + eyes[0].x + eyes[0].width*0.5;
                y0_actuel = faces[i].y + eyes[0].y + eyes[0].height*0.5;
                x1_actuel = faces[i].x + eyes[1].x + eyes[1].width*0.5;
                y1_actuel = faces[i].y + eyes[1].y + eyes[1].height*0.5;
                x_centre_actuel = (x0_actuel+x1_actuel)/2;
                y_centre_actuel = (y0_actuel+y1_actuel)/2;
                deltaX = x_centre_actuel - x_centre_avant;
                deltaY = y_centre_actuel - y_centre_avant;
                Point centre_c(x_centre_actuel,y_centre_actuel);

                /* affichage des 2 yeux en rouge et du centre en bleu */
                radius = cvRound(5);
                circle(frame, yeux1, radius, Scalar( 0, 0, 255 ), -1, 8, 0 );
                circle(frame, yeux2, radius, Scalar( 0, 0, 255 ), -1, 8, 0 );
                circle(frame, centre_c, radius, Scalar( 255, 0, 0), -1, 8, 0 );
/*--------------------------------------------------------------------------------*/
//                    orientation de la tête
/*---------------------------------------------------------------------------------*/
                if(choix==1){
                    if(deja_fait==true){
                        affiche_orientation_tete(deltaX, deltaY, x_centre_avant, y_centre_avant);
                    }
                }

/*--------------------------------------------------------------------------------*/
//                    centre du joystick
/*---------------------------------------------------------------------------------*/

                if(deja_fait==false){
                    x_ref=x_centre_actuel;
                    y_ref=y_centre_actuel;
                    x1_ref_rect = faces[i].x;
                    x2_ref_rect = faces[i].x + faces[i].width;
                    y1_ref_rect = faces[i].y;
                    y2_ref_rect = faces[i].y + faces[i].height;
                }

/*--------------------------------------------------------------------------------*/
//                                   joystick
/*---------------------------------------------------------------------------------*/

                if(choix==2){ /* ---------------------------- CHOIX utilisateur  -----------------------------------*/

                    if(deja_fait==true){
                        val_retour_actuelle = affiche_orientation_tete(deltaX, deltaY, x_centre_avant, y_centre_avant);
                    }

                    /* on est dans le carre noir ou pas */
                    if((x_centre_actuel <= x_ref + KERNEL_J) && (x_centre_actuel >= x_ref - KERNEL_J)
                    && (y_centre_actuel <= y_ref + KERNEL_J) && (y_centre_actuel >= y_ref - KERNEL_J)
                    && (deja_fait==true)){
                        est_dans_carre_maintenant = true;
                        nb_deplacement = 0;
                    }
                    else{
                        est_dans_carre_maintenant = false;
                    }

                    /* bouger le joystick ou pas */
                    if((est_dans_carre_maintenant == false)
                    && (est_dans_carre_avant == true)
                    && (nb_deplacement == 0)){
                        continu=true;
                    }

                    if(continu==true){
                        calcul_position_joystick(val_retour_actuelle);
                        if(val_retour_actuelle > 0){
                            nb_deplacement ++;
                            continu=false;
                            printf("\n\t[joystick] Calcul nouvelle position\n\n");
                        }
                    }

                    posX=position_jostick[0];
                    posY=position_jostick[1];
                }

/*--------------------------------------------------------------------------------*/
//                                   touchscreen
/*---------------------------------------------------------------------------------*/
                if(choix==3){
                    int limite = 20;
                    if(abs(deltaX) > 2 && abs(deltaX) < limite){
                        posX+=deltaX * 5;
                    }
                    if(abs(deltaY) > 2 && abs(deltaY) < limite){
                        posY+=deltaY * 5;
                    }
                    if(deja_fait==true){
                        affiche_orientation_tete(deltaX, deltaY, x_centre_avant, y_centre_avant);
                    }
                }

/*--------------------------------------------------------------------------------*/
//                     mouvement de la souris avec Xlib
/*---------------------------------------------------------------------------------*/
                if(choix>1 && deja_fait==true){ /* ---------------------------- CHOIX utilisateur  -----------------------------------*/

                    if(posX>LARGEUR) posX = LARGEUR;
                    if(posY>HAUTEUR) posY = HAUTEUR;

                    if(posX<0) posX = 0;
                    if(posY<0) posY = 0;

                    XWarpPointer(dpy, None, win, 0, 0, 0, 0,posX,posY);
                    XFillRectangle (dpy,win,gc,posX,posY,5,5);

                    XFlush(dpy);
                    usleep(50);
                }
            }
/*--------------------------------------------------------------------------------*/
//                     fin algo 1 visage, 2 yeux
/*---------------------------------------------------------------------------------*/
            else if(nb_yeux==1){ //clignement
                if(abs(eyes[0].x - x0_avant) < abs(eyes[0].x - x1_avant)){
                    cpt_g++;
                    if(choix>1 && cpt_g==5){ //un "vrai" clic est effectué en mode manette et en mode joystick
                        printf("OEIL GAUCHE CLIGNE... %d\n",cpt_g);
                        //click (dpy, Button1); // clique gauche
                    }
                }
                else{
                    cpt_d++;
                    if(choix>1 && cpt_d==5){
                        printf("OEIL DROIT CLIGNE... attente de clic : %d\n",cpt_d);
                    }
                }
            }
            else{//1 visage et 0 ou plus de 2 yeux
                printf("[%dv %dy]\n",nb_visage,nb_yeux);
            }
/*--------------------------------------------------------------------------------*/
//                     fin algo 1 visage
/*---------------------------------------------------------------------------------*/
        }//fin du if nb_visage == 1
        else{ //0 ou plus d'un visage
            printf("[%d v]\n",nb_visage);
        }

/*--------------------------------------------------------------------------------*/
//                              changement avant - actuel
/*---------------------------------------------------------------------------------*/
        x0_avant=x0_actuel;
        y0_avant=y0_actuel;
        x1_avant=x1_actuel;
        y1_avant=y1_actuel;
        x_centre_avant=x_centre_actuel;
        y_centre_avant=y_centre_actuel;
        val_retour_avant = val_retour_actuelle;
        est_dans_carre_avant = est_dans_carre_maintenant;

/*--------------------------------------------------------------------------------*/
//             affichage des repères visuels pour le centre du joystick
/*---------------------------------------------------------------------------------*/

        if(choix==2){ /* ---------------------------- CHOIX utilisateur  -----------------------------------*/
            Point coin1(x1_ref_rect, y1_ref_rect);
            Point coin2(x2_ref_rect, y1_ref_rect);
            Point coin3(x1_ref_rect, y2_ref_rect);
            Point coin4(x2_ref_rect, y2_ref_rect);
            Point p1(x_ref - KERNEL_J, y_ref - KERNEL_J);
            Point p2(x_ref + KERNEL_J, y_ref + KERNEL_J);
            radius2 = cvRound(5);
            circle(frame, coin1, radius2, Scalar(0,0,0), -1, 8, 0 );
            circle(frame, coin2, radius2, Scalar(0,0,0), -1, 8, 0 );
            circle(frame, coin3, radius2, Scalar(0,0,0), -1, 8, 0 );
            circle(frame, coin4, radius2, Scalar(0,0,0), -1, 8, 0 );
            rectangle(frame,p1,p2, Scalar(0,0,0), 3);
        }

        imshow("face", frame);
        if(waitKey(1) >= 0){
            nb_touches++;
        }
        if(nb_touches==1){
            deja_fait=true;
        }
        else if(nb_touches>1){
            break;
        }
    }
/*--------------------------------------------------------------------------------*/
//                             fin boucle infinie
/*---------------------------------------------------------------------------------*/
    printf("fin de la boucle... appuyez sur une touche pour quitter le programme\n");
    while(waitKey(1)<0) usleep(500000);
    return 0;
}
