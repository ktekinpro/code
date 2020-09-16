# -*- coding: utf-8 -*-

"""
Final work of our 18-month project.

The UAV does an uniform rectilinear motion.
So, he moves with a constant speed between a point A and a point B.

If it meets an obstacle, it contourns it, then return to its initial axis in order
to follow its path by contuining its uniform rectilinear motion until it reach the point B.

Authors : P21 group members - Polytech'Lille
"""

import logging
import sys
import time

import cflib.crtp
from cflib.crazyflie import Crazyflie
from cflib.crazyflie.syncCrazyflie import SyncCrazyflie
from cflib.positioning.motion_commander import MotionCommander
from cflib.utils.multiranger import Multiranger

URI = 'radio://0/80/250K'
DELAY_MVT = 0.1
MIN_DISTANCE = 0.2  # 20cm

if len(sys.argv) > 1:
    URI = sys.argv[1]

# Only output errors from the logging framework
logging.basicConfig(level=logging.ERROR)


def is_close(range):
    if range is None:
        return False
    else:
        return range < MIN_DISTANCE


def decollage(pas_de_boucle):
    print('Decollage')

    for y in range(10):
        capteur=multi_ranger.up
        if is_close(capteur):
            print("arret decollage")
            return 1000 # valeur pour indiquer un obstacle
        cf.commander.send_hover_setpoint(0, 0, 0, y / pas_boucle)
        time.sleep(DELAY_MVT)
    return capteur


def mvt_droite(vitesse,temps,altitude):
    y=0
    while (y<10*temps):
        capteur=multi_ranger.right
        print('Droite : ',capteur)
        if is_close(capteur):
            print("arret mvt droite")
            return 1000 # valeur pour indiquer un obstacle
        cf.commander.send_hover_setpoint(0, -vitesse, 0, altitude)
        time.sleep(DELAY_MVT)
        y+=1
    return capteur


def mvt_gauche(vitesse,temps,altitude):
    print('Gauche')
    y=0
    while (y<10*temps):
        capteur=multi_ranger.left
        if is_close(capteur):
            print("arret mvt gauche")
            return 1000 # valeur pour indiquer un obstacle
        cf.commander.send_hover_setpoint(0, vitesse, 0, altitude)
        time.sleep(DELAY_MVT)
        y+=1
    return capteur


def mvt_avant(vitesse,temps,altitude):
    print('Avant')

    y=0
    while (y<10*temps):
        capteur=multi_ranger.front
        if is_close(capteur):
            print("arret mvt avant")
            return 1000 # valeur pour indiquer un obstacle
        cf.commander.send_hover_setpoint(vitesse, 0, 0, altitude)
        time.sleep(0.1)
        y+=1

    return capteur


def mvt_arriere(vitesse,temps,altitude):
    print('Arriere')
    y=0
    while (y<10*temps):
        capteur=multi_ranger.back
        if is_close(capteur):
            print("arret mvt arriere")
            return 1000 # valeur pour indiquer un obstacle
        cf.commander.send_hover_setpoint(-vitesse, 0, 0, altitude)
        time.sleep(0.1)
        y+=1
    return capteur


def atterrir(pas_de_boucle):
    print('Atterrissage')

    for y in range(10):
        capteur=multi_ranger.down
        if is_close(capteur):
            print("arret atterrissage")
            return 1000 # valeur pour indiquer un obstacle
        cf.commander.send_hover_setpoint(0, 0, 0, (10 - y) / pas_de_boucle)
        time.sleep(0.1)

    cf.commander.send_stop_setpoint() # on arrete les moteurs
    return capteur



if __name__ == '__main__':

    # Initialize the low-level drivers (don't list the debug drivers)
    cflib.crtp.init_drivers(enable_debug_driver=False)

    cf = Crazyflie(rw_cache='./cache')
    with SyncCrazyflie(URI, cf=cf) as scf:
        with MotionCommander(scf) as motion_commander:
            with Multiranger(scf) as multi_ranger:

                keep_flying = True
                zdistance = 0.40
                pas_boucle = 25
                # pas de boucle = 1/zdistance * rang max de la boucle
                # soit ici : 1/0.4 * 10 = 25

                decollage(pas_boucle)

                while keep_flying:

                    cf.commander.send_hover_setpoint(0.2, 0, 0, zdistance)

                    if is_close(multi_ranger.front):
                        x_arriere=mvt_arriere(0.3,1,zdistance)
                        x_arriere_2=0
                        ecart=0
                        while(x_arriere_2 < x_arriere+0.2):
                            x_arriere_2=multi_ranger.front
                            ecart+=0.15
                            if (mvt_droite(0.3,0.5,zdistance)==1000):
                                break
                                print("premier break, premier while")

                        cf.commander.send_hover_setpoint(0, 0, 0, zdistance)
                        print("commence a contourne !")
                        x_gauche=multi_ranger.left
                        while(x_gauche > 1):
                            if(mvt_avant(0.3,0.5,zdistance)==1000):
                                break
                                print("deuxieme break, deuxieme while")
                            x_gauche=multi_ranger.left
                            print("capteur gauche",x_gauche)

                        cf.commander.send_hover_setpoint(0, 0, 0, zdistance)
                        print("obstacle depasse !")
                        while(x_gauche <= 1):
                            # on considère qu'on a dépassé l'obstacle
                            if(mvt_avant(0.3,0.5,zdistance)):
                                break
                                print("3eme break")
                            x_gauche=multi_ranger.left

                            mvt_avant(0.3,0.5,zdistance) #on avance de 15 cm avant de revenir à gauche

                        print("il faut revenir sur son chemin...",ecart)
                        y=0
                        while (y<ecart):
                            if(mvt_gauche(0.3,0.5,zdistance)==1000):
                                break
                                print("4eme break")
                            y+=0.15
                        print("retour a la normale")

                    if is_close(multi_ranger.back):
                        mvt_avant(0.3,1,zdistance)

                    if is_close(multi_ranger.left):
                        mvt_droite(0.3,1,zdistance)

                    if is_close(multi_ranger.right):
                        mvt_gauche(0.3,1,zdistance)

                    if is_close(multi_ranger.up):
                        keep_flying = False

                    time.sleep(0.05)

            atterrir(pas_boucle)

            print('Fin du programme')
