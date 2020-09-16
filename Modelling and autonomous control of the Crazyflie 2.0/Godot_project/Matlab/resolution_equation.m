u = udp();
while(1)
    %On utilise la communication UDP:

    u.LocalPort = 4245; 
    fopen(u); % ouverture communication UDP
    % --------------------------------
    % Serveur UDP
    % --------------------------------

    premiermsg = fscanf(u,'%s');

    if(premiermsg=="start")
        data = fscanf(u,'%s');
        vx = str2double(data);
        disp(['je recoit vx' vx]);
        data1 = fscanf(u,'%s');
        vy = str2double(data1);
        disp(['je recoit vy' vy]);
        data2 = fscanf(u,'%s');
        vz = str2double(data2);
        disp(['je recoit vz' vz]);
    end

    fclose(u); % on ferme la connexion


    %-------------------------


    v4=max(340.0000001,378.0000001*sqrt(vy));

    v1=0.003953*(-2.42375*10^9*(vx+1.78571*(vy+5.33718*(vz-0.000003*v4^2))))^(1/2);
    v2=0.065795*(-3.125*10^7*(vy-0.000007*v4^2))^(1/2);
    v3=194.625*(vx-1.78571*(vy+5.33718*(vz-0.000003*v4^2)))^(1/2);

    % Conditions de validité
    % −2.9*10^−5*(v4^2-62455*(vy+5.4*vz))≤ vx ≤ 2.9*10^−5*(v4^2-62455*(vy+5.4*vz))
    % vz≤min(3*10^−6*(v4^2+34975*(vx-1.78571*vy)),3*10^−6*(v4^2-34975*(vx+1.78571*vy))) and v4-max(0.,378*√(vy))≥0. and vy≥0
    % vy < 0.000007*v4^2
    % v4 ≥ 378*√(vy) avec V4max=925, alors vymax=5.9

    % --------------------------------
    % client UDP
    % --------------------------------

    u = udp('127.0.0.1',4242); %envoie sur localhost (127.0.0.1) port 4242
    fopen(u); % ouverture communication UDP

    fprintf(u,'start'); %début transmission UDP, on envoie l'ID

    fprintf(u,v1); %Vm1
    disp(["j'envoie v1" v1]);
    fprintf(u,v2); %Vm2
    disp(["j'envoie v2" v2]);
    fprintf(u,v3); %Vm3
    disp(["j'envoie v3" v3]);

    fprintf(u,v4); %Vm4
    disp(["j'envoie v4" v4]);

    echoudp('off');

    fclose(u);
end
