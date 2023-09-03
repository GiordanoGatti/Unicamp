clc
clear all
close all

load('ex_2.mat')

M = 20;
alpha = 0;
N = length(fotopletismograma);


Rx = autocorrelacao(aceleracao, M);
Pxd  = corr_cruzada(aceleracao, fotopletismograma, M, alpha);

%WIENER
Wiener = inv(Rx)*Pxd;
Yw = conv(Wiener, aceleracao);
ew = -Yw(1:N)+fotopletismograma;

figure()
subplot(3,1,1)
plot(fotopletismograma)
subplot(3,1,2)
plot(aceleracao)
subplot(3, 1, 3)
plot(ew)



%LMS
mu = 0.001;
L = 5;

w = zeros(L,N);
X = zeros(L,1);
for n=1:N
    for l=0:L-1
        if n-l<1
            X(l+1)=0;
        else
            X(l+1)=aceleracao(n-l);
        end
    end
    if n<N
        e(n) = fotopletismograma(n) - w(:,n)'*X;
        w(:,n+1) = w(:,n) + mu*X*e(n); %erro quadratico medio
        %w(:,n+1) = w(:,n) + mu*X*sign(e(n)); %modulo do erro
    end
end

w




