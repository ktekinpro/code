function [thetaGS,iter]=GaussSeidel(A,b,x,prec,Itermax)
n=size(b,1);
iter=0;
norme=prec+1;
thetaGS=x;

while ((norme > prec) && (iter < Itermax))
    x_ancien=x;
    for i=1:n
        s=0;
        for j=1:i-1
            s=s+A(i,j)*x(j);
        end
        for j=i+1:n
             s=s+A(i,j)*x_ancien(j);
        end
    x(i)=(1/A(i,i))* (b(i)-s);
    end
  iter=iter+1;
  norme=norm(x-x_ancien);
end
thetaGS=x;
end



  
        
            
    
    