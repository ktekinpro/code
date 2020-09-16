# -*- coding: utf-8 -*-
"""
Here, the UAV does a stationary fly indefinitely and move on the other side
as soon as it detects an obstacle.

These algorithms allow us to test Crazyflie's reactivty, and the values return
by the detectors.

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

if len(sys.argv) > 1:
    URI = sys.argv[1]

# Only output errors from the logging framework
logging.basicConfig(level=logging.ERROR)


def is_close(range):
    MIN_DISTANCE = 0.2  # 20cm

    if range is None:
        return False
    else:
        return range < MIN_DISTANCE


def ecrit_log(log_avant,log_arriere,log_droite,log_gauche,log_haut,log_bas):
    print('Ecriture des logs')

    # création et ouverture du fichier test.txt en mode write 'w' (écriture)
    # si le fichier test.txt existe déjà, il est écrasé
    Fichier = open('log_avant.txt','w')
    for element in log_avant:
        Fichier.write(str(element))
        Fichier.write('\n')
    Fichier.close()

    Fichier = open('log_arriere.txt','w')
    for element in log_arriere:
        Fichier.write(str(element))
        Fichier.write('\n')
    Fichier.close()

    Fichier = open('log_droite.txt','w')
    for element in log_droite:
        Fichier.write(str(element))
        Fichier.write('\n')
    Fichier.close()

    Fichier = open('log_gauche.txt','w')
    for element in log_gauche:
        Fichier.write(str(element))
        Fichier.write('\n')
    Fichier.close()

    Fichier = open('log_haut.txt','w')
    for element in log_haut:
        Fichier.write(str(element))
        Fichier.write('\n')
    Fichier.close()

    Fichier = open('log_altitude.txt','w')
    for element in log_bas:
        Fichier.write(str(element))
        Fichier.write('\n')
    Fichier.close()

    print('Fin ecritures des logs')
    return 0


def decollage(pas_de_boucle):
    print('Decollage')
    # on decolle en augmentant l'altitude progressivement
    for y in range(10):
        if is_close(multi_ranger.up):
            break
        cf.commander.send_hover_setpoint(0, 0, 0, y / pas_boucle)
        time.sleep(0.1)
    return 0


def mvt_droite(vitesse,temps,altitude):
    print('Droite')
    y=0
    while (y<10*temps):
        if is_close(multi_ranger.right):
            break
        cf.commander.send_hover_setpoint(0, -vitesse, 0, altitude)
        time.sleep(0.1)
        y+=1
    return 0

def mvt_gauche(vitesse,temps,altitude):
    print('Gauche')

    y=0
    while (y<10*temps):
        if is_close(multi_ranger.left):
            break
        cf.commander.send_hover_setpoint(0, vitesse, 0, altitude)
        time.sleep(0.1)
        y+=1
    return 0

def mvt_avant(vitesse,temps,altitude):
    print('Avant')

    y=0
    while (y<10*temps):
        if is_close(multi_ranger.front):
            break
        cf.commander.send_hover_setpoint(vitesse, 0, 0, altitude)
        time.sleep(0.1)
        y+=1
    return 0

def mvt_arriere(vitesse,temps,altitude):
    print('Arriere')

    y=0
    while (y<10*temps):
        if is_close(multi_ranger.back):
            break
        cf.commander.send_hover_setpoint(-vitesse, 0, 0, altitude)
        time.sleep(0.1)
        y+=1
    return 0

def atterrir(pas_de_boucle):
    print('Atterrissage')

    for y in range(10):
        if is_close(multi_ranger.down):
            break
        cf.commander.send_hover_setpoint(0, 0, 0, (10 - y) / pas_de_boucle)
        time.sleep(0.1)

    cf.commander.send_stop_setpoint() # on arrete les moteurs
    return 0

if __name__ == '__main__':
    log_avant=[]
    log_arriere=[]
    log_droite=[]
    log_gauche=[]
    log_haut=[]
    log_bas=[]

    # Initialize the low-level drivers (don't list the debug drivers)
    cflib.crtp.init_drivers(enable_debug_driver=False)

    cf = Crazyflie(rw_cache='./cache')
    with SyncCrazyflie(URI, cf=cf) as scf:
        with MotionCommander(scf) as motion_commander:
            with Multiranger(scf) as multi_ranger:

                keep_flying = True
                zdistance = 0.4
                pas_boucle = 25
                epsilon = 0.05 #0.05m = 5 centimetres

                # epsilon (en metre) : quand on atteint epsilon, on coupe les moteurs = atterissage directe
                # pas de boucle = 1/zdistance * rang max de la boucle
                # soit ici : 1/0.4 * 10 = 25
                # ATTENTION : mettre un ENTIER (integer)
                # valeurs testees 1 : zdistance = 0.4 ; pas_boucle = 25
                # valeurs testees 2 : zdistance = 1.5 ; pas_boucle = 7

                decollage(pas_boucle)

                while keep_flying:

                    if(len(log_avant)<5000): # on enregistre les 5000 premières valeurs des capteurs
                        log_avant.append(multi_ranger.front)
                        log_arriere.append(multi_ranger.back)
                        log_droite.append(multi_ranger.right)
                        log_gauche.append(multi_ranger.left)
                        log_haut.append(multi_ranger.up)
                        log_bas.append(multi_ranger.down)

                    cf.commander.send_hover_setpoint(0, 0, 0, zdistance)

                    if is_close(multi_ranger.front):
                        mvt_arriere(0.3,1,zdistance)

                    if is_close(multi_ranger.back):
                        mvt_avant(0.3,1,zdistance)

                    if is_close(multi_ranger.left):
                        mvt_droite(0.3,1,zdistance)

                    if is_close(multi_ranger.right):
                        mvt_gauche(0.3,1,zdistance)

                    if is_close(multi_ranger.up):
                        keep_flying = False

                    time.sleep(0.1)

            atterrir(pas_boucle)

            print('Demo terminated!')

            # écrit dans 6 fichiers textes les valeurs des 6 capteurs de proximité
            ecrit_log(log_avant,log_arriere,log_droite,log_gauche,log_haut,log_bas)

            print('Fin du programme')
