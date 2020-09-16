function[y1]=Runge_Kutta2(f,y0,ts)
    N=length(ts)-1;
    y1=ones(1,N);
    y1(1)=y0;
    for i=2:N+1
        y1(i)=y1(i-1)+(1/N)*f((i-2)/N+(1/N)/2,y1(i-1)+(1/N)/2*f((i-2)/N,y1(i-1)));
    end
end
