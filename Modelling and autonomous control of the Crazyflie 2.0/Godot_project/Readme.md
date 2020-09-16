# Godot Project

In this subproject the goal is to simulate a quadcopter in Godot, by implementing its dynamic model system.
On the other side, we use Matlab to regulate the UAV according to the data given by the ouput of Godot.
At the end, Matlab output will be given to the input of Godot.

So we have a Godot-Matlab loop that simulates and controls a quadcopter. The 2 softwares communicate each other by UDP.


Here are the principales files for the simulation and regulation :

- Godot Simulation/drone_physique.gd : Godot script that allows us to simulate
a quacopter as a physic engine (dynamic model system equations are implemented
in there), and to communicate with Matlab through UDP.

- Matlab/VariableDrone.m, Matlab/Regulation_pid.mdl : Matlab and Simulink files that calculate
the speeds of the 4 motors according to the angular speeds of the quacopter.

- Matlab/resolution_equation.m : resolution of the dynamic system model for
the regulation of the UAV.
