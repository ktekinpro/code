############################################################################
# Sources :
#
# Quadrotor control: modeling, nonlinear control design, and simulation
# FRANCESCO SABATINO
# 2.36 p21,p12, 2.16 p15, 2.33 p20, p13
#
# Crazyflie physic model's values
# https://arxiv.org/pdf/1608.05786.pdf
# pages 12 and 13
#
# https://www.bitcraze.io/portals/research/
#
# Julian Fôrster, ETH Zürich, 2015
# System Identification of the Crazyflie 2.0 Nano Quadrocopter
# https://polybox.ethz.ch/index.php/s/20dde63ee00ffe7085964393a55a91c7
#
#
# https://www.wilselby.com/research/arducopter/simulation-environment/
############################################################################
extends KinematicBody

var vitesses_angulaires = Vector3(0,0,0)
var vitesse_lineaire = Vector3(0,0,0)
var timer = 0.0
var timer2 = 0.0

# temps delta T pour les intégrales
var deltaT = 0.0

# angles et positions dans le repère fixe
var phi = 0.0
var theta = 0.0
var psi = 0.0
var x = 0.0
var y = 0.0
var z = 0.0

# vitesses linéaires et angulaires dans le repère fixe
var phi_point = 0.0
var theta_point = 0.0
var psi_point = 0.0
var x_point = 0.0
var y_point = 0.0
var z_point = 0.0

# vitesses linéaires et angulaires dans le repère mobile
var p = 0.0
var q = 0.0
var r = 0.0
var u = 0.0
var v = 0.0
var w = 0.0

# accélérations linéaires et angulaires dans le repère mobile
var p_point = 0.0
var q_point = 0.0
var r_point = 0.0
var u_point = 0.0
var v_point = 0.0
var w_point = 0.0

# Perturbation du vent : nulle au départ
var f_wx = 0.0
var f_wy = 0.0
var f_wz = 0.0
var T_wx = 0.0
var T_wy = 0.0
var T_wz = 0.0

# Constantes pour les intégrations
var p0 = 0.0
var q0 = 0.0
var r0 = 0.0
var u0 = 0.0
var v0 = 0.0
var w0 = 0.0
var theta0 = 0.0
var phi0 = 0.0

# Entrée pour le contrôle du drone : 2.16 p15
var Vm1 = 0.0 # Vitesse moteur 1
var Vm2 = 0.0# Vitesse moteur 2
var Vm3 = 0.0 # Vitesse moteur 3
var Vm4 = 0.0 # Vitesse moteur 4

# Force et moment générés par les vitesses des 4 moteurs : 2.16 p15
var f_t = 0.0 # Force de poussée (verticale)
var T_x = 0.0 # Moment (torque) selon X
var T_y = 0.0 # Moment (torque) selon Y
var T_z = 0.0 # Moment (torque) selon Z


# Constantes
var g = 9.81 # m.s^-2  constante de gravité
var m = 1.4  # en kg, masse du drone
var l = 0.56 # en mètre  distance moteur - centre du drone
var b = 1.32 * pow(10,-5) # N.rad^-2 thrust coefficient/facteur de poussée
var d = 1.385* pow(10,-6) # Nm.rad^-2  drag coefficient/facteur de trainée
# élément matrice d'inertie, 2 axes de symétrie => matrice diagonale
var Ix =0.05 # kg.m²
var Iy =0.05 # kg.m²
var Iz =0.24# kg.m²


# Variables pour la communication UDP
var socketc
var socket
var data
var continu = false
var done = false
var compteur = 0
var tab = []
var message_recu = PoolRealArray(tab)

var timer3=0.0
############################################################################

func _init():
	socketc = PacketPeerUDP.new()
	socketc.set_dest_address("127.0.0.1",4245)

	socket = PacketPeerUDP.new()
	if(socket.listen(4242,"127.0.0.1") != OK):
		print("An error occurred listening on port 4242 on 127.0.0.1 (localhost)\n")
	else:
		print("Listening on port 4242 on 127.0.0.1 (localhost)\n")


# Called when the node enters the scene tree for the first time.
func _ready():
	pass

# Boucle tout les 'delta' = temps entre 2 fps
func _physics_process(delta):
	# Réception UDP Pour la simulation
	recoit_UDP()

	# Entrée ---- RECU PAR UDP : cf code de la fonction recoit_UDP()
	#Vm1 = 552.8694 # Vitesse moteur CW 1 	---- ENTREE (cw = clockwise)
	#Vm2 = 330.8612 # Vitesse moteur CCW 2 ---- ENTREE (counterclockwise = clockwise)
	#Vm3 = 552.9049 # Vitesse moteur CW 3 	---- ENTREE le sens des moteurs est déja géré dans les équations
	#Vm4 = 340.0000 # Vitesse moteur CCW 4 ---- ENTREE #toute façon c'est au carré donc bon les signes pas grave


	#perturbation : forces et moments du vent : cf 2.33
	T_wx = 0.0
	T_wy = 0.0
	T_wz = 0.5

	# équations 2.16 p15
	f_t = b*(pow(Vm1,2) + pow(Vm2,2) + pow(Vm3,2) + pow(Vm4,2))
	T_x = b*(pow(Vm3,2) - pow(Vm1,2))
	T_y = b*l*(pow(Vm4,2) - pow(Vm2,2))
	T_z = d*(pow(Vm2,2) + pow(Vm4,2) - pow(Vm1,2) - pow(Vm3,2))

	print("T_x : ",T_x," || T_y : ",T_y," || T_z : ",T_z)

	#arrondi pour compenser les imprécisions de matlab
	T_x = stepify(T_x,0.1)
	T_y = stepify(T_y,0.1)
	T_z = stepify(T_z,0.1)

	print("T_x : ",T_x," || T_y : ",T_y," || T_z : ",T_z,'\n')
	print("V1 : ",Vm1," || V2 : ",Vm2," || V3 : ",Vm3," || V4 : ",Vm4,'\n')

	# intégrales
	deltaT = delta
	theta = theta_point * deltaT + theta
	phi = phi_point * deltaT + phi


	# équations 2.36 - partie 1/2
	p_point = (T_x + T_wx) / Ix
	q_point = (T_y + T_wy) / Iy
	r_point = (T_z + T_wz) / Iz
	u_point = -g * theta + f_wx / m
	v_point = g * phi + f_wy / m
	w_point = (f_wz - f_t) / m

	# intégrales
	p = p_point * deltaT + p0
	q = q_point * deltaT + q0
	r = r_point * deltaT + r0
	u = u_point * deltaT + u0
	v = v_point * deltaT + v0
	w = w_point * deltaT + w0

	# équations 2.36 - partie 2/2
	phi_point = p
	theta_point = q
	psi_point = r
	x_point = u
	y_point = v
	z_point = w

	# positions
	x= translation.x + x_point
	y= translation.z - z_point #inversion axe Z-Y entre les axes de Godot et ceux du modèle du drone
	z= translation.y + y_point
	phi= rotation.x + phi_point
	psi= rotation.y + psi_point
	theta= rotation.z + theta_point
	#print("00 phi : ",phi," || theta : ",theta," || psi : ",psi)
	#print("00 x : ",x," || y : ",y," || z : ",z)


	#simule un échelon de vitesse, les vitesses en entrée sont imposées pendant 1 seconde au système
	#simulation_rotation_t_1s()

	#simule à t infini la réponse rotanionnelle du système
	simulation_rotation_t_infi()

	#simule un échelon de vitesse, les vitesses en entrée sont imposées pendant 1 seconde au système
	#simulation_lineaire_t_1s()

	#simule à t infini la réponse du système
	simulation_lineaire_t_infi()


	# Envoie UDP Pour la régulation
	envoie_UDP()

	pass


func simulation_rotation_t_infi():
	#print("phi_point : ",phi_point," || theta_point : ",theta_point," || psi_point : ",psi_point)

	# 'rotation' : valeurs des angles de rotation actuelles du drone
	vitesses_angulaires = rotation + Vector3(phi_point,psi_point,theta_point)
	set_rotation(vitesses_angulaires)
	#print("11 phi : ",rotation.x," || theta : ",rotation.z," || psi : ",rotation.y)


func simulation_rotation_t_1s():
	timer+=deltaT
	if(timer< 1.0):
		simulation_rotation_t_infi()


func simulation_lineaire_t_infi():
	#print("x_point : ",x_point," || y_point : ",y_point," || z_point : ",z_point)

	# 'translation' : valeurs des coordonnées actuelles du drone
	vitesse_lineaire = translation + Vector3(x_point,-z_point,y_point)
	if(vitesse_lineaire.y>20): #on ne veut par déborder la fenêtre
		vitesse_lineaire.y=20;

	set_translation(vitesse_lineaire)
	#print("11 x : ",vitesse_lineaire.x," || y : ",vitesse_lineaire.z," || z : ",vitesse_lineaire.y,'\n')


func simulation_lineaire_t_1s():
	timer2+=deltaT
	if(timer2< 1.0):
		simulation_lineaire_t_infi()


func envoie_UDP():
  socketc.put_packet("start".to_ascii())
  # 1ère simulation
  #socketc.put_packet(str(phi_point).to_ascii())
  #socketc.put_packet(str(psi_point).to_ascii())
  #socketc.put_packet(str(theta_point).to_ascii())
  #socketc.put_packet(str(w_point).to_ascii())

  #2ème simulation REGULATION MATLAB
  #socketc.put_packet(str(x).to_ascii())
  #socketc.put_packet(str(y).to_ascii())
  #socketc.put_packet(str(z).to_ascii())
  #socketc.put_packet(str(phi).to_ascii())
  #socketc.put_packet(str(psi).to_ascii())
  #socketc.put_packet(str(theta).to_ascii())

  #3ème regulation
  socketc.put_packet(str(-T_wx).to_ascii())
  socketc.put_packet(str(-T_wy).to_ascii())
  socketc.put_packet(str(-T_wz).to_ascii())


func recoit_UDP():
	if(socket.get_available_packet_count() > 0):
		data = socket.get_packet().get_string_from_ascii()
		print("data received : ",data)

	if(data == "quit\n"):
		#print("Data received: " + data)
		continu = false

	if(continu == true && compteur <4):
		#print("Data received: " + data)
		message_recu.append(float(data))
		compteur += 1

	if (compteur >=4):
		compteur = 0 # reinit compteur
		continu = false #reinit booleen
		print("\ndonees_a_simuler :",message_recu)
		Vm1=message_recu[0]
		Vm2=message_recu[1]
		Vm3=message_recu[2]
		Vm4=message_recu[3]
		message_recu = PoolRealArray(tab) #reinit buffer "mesage_recu"

	if(data == "start\n"):
		#print("Data received: " + data)
		continu = true
