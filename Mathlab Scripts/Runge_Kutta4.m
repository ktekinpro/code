function[y2]=Runge_Kutta4(f,y0,ts)
    N=length(ts)-1;
    y2=ones(1,N);
    y2(1)=y0;
    for i=2:N+1
        K1=f((i-2)/N,y2(i-1));
        K2=f((i-2)/N+1/(N*2),y2(i-1)+1/(N*2)*K1);
        K3=f((i-2)/N+1/(N*2),y2(i-1)+1/(N*2)*K2);
        K4=f((i-2)/N+1/N,y2(i-1)+(1/N)*K3);
        y2(i)=y2(i-1)+(1/(N*6))*(K1+2*K2+2*K3+K4);
    end
end
