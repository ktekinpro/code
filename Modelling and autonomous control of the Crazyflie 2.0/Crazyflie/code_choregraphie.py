# -*- coding: utf-8 -*-

"""
This code make the Crazyflie move around 6 degrees of liberty.

Authors : P21 group members - Polytech'Lille
"""


import logging
import time

import cflib.crtp
from cflib.crazyflie import Crazyflie
from cflib.crazyflie.log import LogConfig
from cflib.crazyflie.syncCrazyflie import SyncCrazyflie
from cflib.crazyflie.syncLogger import SyncLogger

# Only output errors from the logging framework
logging.basicConfig(level=logging.ERROR)



if __name__ == '__main__':


    # Initialize the low-level drivers (don't list the debug drivers)
    cflib.crtp.init_drivers(enable_debug_driver=False)
    # Scan for Crazyflies and use the first one found
    print('Scanning interfaces for Crazyflies...')
    available = cflib.crtp.scan_interfaces()
    print('Crazyflies found:')


    for i in available:
        print(i[0])

    if len(available) == 0:
        print('No Crazyflies found, cannot run the code')
    else:
        # print('\n\nLog the Stabilizer but does not print it to the console.\nAfter 10s the application will go on to the next steps\n')

        lg_stab = LogConfig(name='Stabilizer', period_in_ms=10)
        lg_stab.add_variable('stabilizer.roll', 'float')
        lg_stab.add_variable('stabilizer.pitch', 'float')
        lg_stab.add_variable('stabilizer.yaw', 'float')


        cf = Crazyflie(rw_cache='./cache')


        with SyncCrazyflie(available[0][0], cf=cf) as scf:
            with SyncLogger(scf, lg_stab) as logger:
                endTime = time.time() + 5

                for log_entry in logger:
                    timestamp = log_entry[0]
                    data = log_entry[1]
                    logconf_name = log_entry[2]

                    # print('[%d][%s]: %s' % (timestamp, logconf_name, data))

                    if time.time() > endTime:
                        break
        URI=""
        print('frequence sur laquelle on est connecte : x/xx/xxxK')
        URI=input()
        URI="radio://"+URI

        cflib.crtp.init_drivers(enable_debug_driver=False)

        with SyncCrazyflie(URI, cf=Crazyflie(rw_cache='./cache')) as scf:
                cf = scf.cf

                cf.param.set_value('kalman.resetEstimation', '1')
                time.sleep(0.1)
                cf.param.set_value('kalman.resetEstimation', '0')
                time.sleep(2)


                arret=False
                zdistance = 0.4
                pas_boucle = 25
                epsilon = 0.05 #0.05m

                #-----------------------------------------------------------------#
                # take off
                # 1rst degree of liberty
                #-----------------------------------------------------------------#

                print('Decollage')
                # take off
                for y in range(10):
                        cf.commander.send_hover_setpoint(0, 0, 0, y / pas_boucle)
                        time.sleep(0.1)

                # 2 secondd stationary fly
                for _ in range(20):
                        cf.commander.send_hover_setpoint(0, 0, 0, zdistance)
                        time.sleep(0.1)

                #-----------------------------------------------------------------#
                # move foreward / backward
                # 2nd degree of liberty
                #-----------------------------------------------------------------#

                print('Avant - Arriere')
                # move foreward
                for y in range(10):
                        cf.commander.send_hover_setpoint(0.5, 0, 0, zdistance)
                        time.sleep(0.1)

                # 1 second stationary fly
                for _ in range(10):
                        cf.commander.send_hover_setpoint(0, 0, 0, zdistance)
                        time.sleep(0.1)

                # move backward
                for y in range(10):
                        cf.commander.send_hover_setpoint(-0.5, 0, 0, zdistance)
                        time.sleep(0.1)

                # 1 second stationary fly
                for _ in range(10):
                        cf.commander.send_hover_setpoint(0, 0, 0, zdistance)
                        time.sleep(0.1)

                #-----------------------------------------------------------------#
                # move left / right
                # 3rd  degree of liberty
                #-----------------------------------------------------------------#

                print('Gauche - Droite')
                for y in range(10):
                        cf.commander.send_hover_setpoint(0, 0.5, 0, zdistance)
                        time.sleep(0.1)

                # 1 second stationary fly
                for _ in range(10):
                        cf.commander.send_hover_setpoint(0, 0, 0, zdistance)
                        time.sleep(0.1)

                for y in range(10):
                        cf.commander.send_hover_setpoint(0, -0.5, 0, zdistance)
                        time.sleep(0.1)

                # 1 second stationary fly
                for _ in range(10):
                        cf.commander.send_hover_setpoint(0, 0, 0, zdistance)
                        time.sleep(0.1)

                #-----------------------------------------------------------------#
                # 360° yaw : clockwise then counterclockwise
                # 4th degree of liberty
                #-----------------------------------------------------------------#

                print('Lacet')
                # 2 seconds stationary fly
                for _ in range(20):
                        cf.commander.send_hover_setpoint(0, 0, 0, zdistance)
                        time.sleep(0.1)

                # clockwise
                for y in range(20):
                        cf.commander.send_hover_setpoint(0, 0, 180, zdistance)
                        time.sleep(0.1)

                # 3 seconds stationary fly
                for _ in range(30):
                        cf.commander.send_hover_setpoint(0, 0, 0, zdistance)
                        time.sleep(0.1)

                # counterclockwise
                for y in range(20):
                        cf.commander.send_hover_setpoint(0, 0, -180, zdistance)
                        time.sleep(0.1)

                # 1 second stationary fly
                for _ in range(10):
                        cf.commander.send_hover_setpoint(0, 0, 0, zdistance)
                        time.sleep(0.1)

                #-----------------------------------------------------------------#
                # roll
                # 5th degree of liberty
                #-----------------------------------------------------------------#

                print('Roulis')
                for y in range(10):
                        cf.commander.send_hover_setpoint(0, 0.7, 0, zdistance)
                        time.sleep(0.5)
                        cf.commander.send_hover_setpoint(0, -0.7, 0, zdistance)
                        time.sleep(0.5)

                # 1 second stationary fly
                for _ in range(10):
                        cf.commander.send_hover_setpoint(0, 0, 0, zdistance)
                        time.sleep(0.1)

                #-----------------------------------------------------------------#
                # pitch
                # in one sens and another every 0.5 seconds
                # 6th degree of liberty
                #-----------------------------------------------------------------#

                print('Tangage')
                for y in range(10):
                        cf.commander.send_hover_setpoint(0.7, 0, 0, zdistance)
                        time.sleep(0.5)
                        cf.commander.send_hover_setpoint(-0.7, 0, 0, zdistance)
                        time.sleep(0.5)

                # 1 second stationary fly
                for _ in range(10):
                        cf.commander.send_hover_setpoint(0, 0, 0, zdistance)
                        time.sleep(0.1)

                #-----------------------------------------------------------------#
                # landing
                # 1rst degree of liberty
                #-----------------------------------------------------------------#

                print('Atterrissage')
                pas_boucle *= 10
                for y in range(100):
                        if ((100 - y) / pas_boucle > epsilon):
                            cf.commander.send_hover_setpoint(0, 0, 0, (100 - y) / pas_boucle)
                        else:
                            cf.commander.send_stop_setpoint()
                            arret=True
                            break
                        time.sleep(0.1)

                if (arret==False):
                        cf.commander.send_stop_setpoint()

        print("FIN DU MAIN")
