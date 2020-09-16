%Variables fixées par l'utilisateur
%Theta = 1.07;
%Phi = 0;
%Psi = 0;
%w_point = -0.34;
Twx = 0;
Twy = 0;
Twz = 0;
Fwz = 0;

%Variables du crazyflie
Ix = 1.4 * 10^-5;
Iy = 1.43 * 10^-5;
Iz = 2.17 * 10^-5;
m = 0.033;
b = 3.2 * 10^-10;
d = 7.9 * 10^-12;
l = 0.04;



%Variable en sortie
Moteur1 = out.Moteur1;
Moteur2 = out.Moteur2;
Moteur3 = out.Moteur3;
Moteur4 = out.Moteur4;

% --------------------------------
% Serveur UDP
% --------------------------------
u = udp();
while(1)

u.LocalPort = 4245;
fopen(u);



premiermsg = fscanf(u,'%s');

if(premiermsg=="start")

    PHI = fscanf(u,'%s');
    phi_point_recupere = str2double(PHI);

    PSI = fscanf(u,'%s');
    psi_point_recupere = str2double(PSI);

    THETA = fscanf(u,'%s');
    theta_point_recupere = str2double(THETA);

    Wpoint = fscanf(u,'%s');
    w_point_recupere = str2double(Wpoint);

end




fclose(u); % on ferme la connexion

%-----------------------------------------------------------------------
%
%
%               SIMULATION
%
%
%
%-----------------------------------------------------------------------

% --------------------------------
% client UDP : envoie la sortie de la simulation en entrée de Godot
% --------------------------------

u = udp('127.0.0.1',4242); %envoie sur localhost (127.0.0.1) port 4242
fopen(u); % ouverture communication UDP

fprintf(u,'start'); %début transmission UDP, on envoie l'ID

fprintf(u,Moteur1); %Vm1
fprintf(u,Moteur2); %Vm2
fprintf(u,Moteur3); %Vm3
fprintf(u,Moteur4); %Vm4

echoudp('off');


fclose(u);

end
