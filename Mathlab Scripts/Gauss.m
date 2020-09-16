function [thetaG]=Gauss(A,b)
n=size(b,1); % ou length(b)

for i=1:1:n-1
    for k=i+1:1:n
        coeff=A(k,i)/A(i,i);
        for j=i:1:n
            A(k,j)=A(k,j)-coeff*A(i,j);
        end
        b(k)=b(k)-coeff*b(i);
    end
end
b(n)=b(n)/A(n,n);

for i=n-1:-1:1
    for k=i+1:1:n
        b(i)=b(i)-A(i,k)*b(k);
    end
    b(i)=b(i)/A(i,i);
end

thetaG=b;

end