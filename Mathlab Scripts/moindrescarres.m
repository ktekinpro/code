function [coeff]=moindrescarres(xp,yp,m)
    n=length(yp);
    A=ones(n,m+1);
    for i=1:n
        for j=1:m+1
            A(i,j)=xp(i).^(m+1-j);
        end
    end
 %xp en ligne, yp en colonne
 coeff=(A'*A)\(A'*yp');
 
 
            

    