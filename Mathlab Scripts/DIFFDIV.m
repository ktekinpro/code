function [dd]=DIFFDIV(xp,yp)
n=length(xp);
dd=yp;
for k=1:1:n-1
    for j=n:-1:k+1
        dd(j)=(dd(j)-dd(j-1))/(xp(j)-xp(j-k));
    end
end