# Crazyflie

Python codes are written and interpreted on the computer, which it's considered as
our ground station. As long as Python code are interpreted, cflib (Crazflie Python libraries)
send commands to the UAVs through an USB CrazyRadio PA antenna.

- code_choregraphie.py : the Crazyflie move around 6 degrees of liberty by doing
a "choreography".

- evite_obstacle.py : the Crazyflie does a stationary fly indefinitely and move
on the other side as soon as it detects an obstacle.

- contourne_obstacle.py : final work of our 18-month project.
The UAV does an uniform rectilinear motion. If it meets an obstacle, it contourns it,
then return to its initial axis in order to follow its path by contuining its uniform
rectilinear motion.


- autonomous_flight.c : FreeRTOS task that run on Crazyflie's firmware in real-time
by doing a preemptive round robin (see FreeRTOS documentation for more informations).
Actually, it's one of the examples given by M. Barret in his master thesis,
that we have tried to implement.
