function [y]=EULERPROG2(f,y0,ts)
    N=length(ts)-1;
    m=2;
    y=ones(m,N);
    y(:,1)=y0;
    for i=2:N+1
        y(:,i)=y(:,i-1)+(200/N)*f((i-2)/N,y(:,i-1));
    end 
end