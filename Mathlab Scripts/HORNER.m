function [P]=HORNER(dd,xp,T)
n=length(xp);
i=length(T);
for j=i:-1:1    
    P(j)=dd(n);
    for k=n-1:-1:1
        P(j)=P(j)*(T(j)-xp(k))+dd(k);
    end
end
