function [y]=EULERPROG(f,y0,ts)
    N=length(ts)-1;
    y=ones(1,N);
    y(1)=y0;
    for i=2:N+1
        y(i)=y(i-1)+(1/N)*f((i-2)/N,y(i-1));
    end 
end