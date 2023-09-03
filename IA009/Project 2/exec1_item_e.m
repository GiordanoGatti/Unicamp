close all
clear all
clc

load('dados_identificacao_ruido_impulsivo.mat')
x = x';
d = d';


%%
N = length(x);

%s = sign(randn(N,1));
%figure, stem(s(1:30))


h = [1; 0.2; 0.5];
r = conv(h,x);

r = r(1:end-2); 
%figure, stem(x(1:30))
%primeiro e ultimo elementos possuem 
%valores invalidos por falta de amostra 
%em instantes anteriores ou posteriores 
%no calculo da conv

%adicao de ruido wgn
%r = r + 1*randn(length(r),1);
%%
%autocorrelacao
rx_0 = (x.' *x)/length(x);
rx_1 = (x(1:end-1).'*x(2:end))/(length(x)-1);
Rx = [rx_0 rx_1; rx_1 rx_0]


%%
%correlacao cruzada
Pxd_0  = [(x.'*d)/length(x); (x(1:end-1).'*d(2:end))/(length(x)-1)] %alfa=0
%Pxd_1 = [(x(2:end).'*d(1:end-1)/(length(x)-1)); (x.'*d)/length(x)] %alfa=1

% pxd[i] = np.sum(np.dot(x[0 : (-i - 1)], d[i:-1])) / len(x[i:-1])

%%
%aproximacoes dos filtro otimos
wo_0 = inv(Rx)*Pxd_0
%wo_1 = inv(Rx)*Pxd_1
%wo_0 = [1.015 0.228]'

%resposta combinada
u_0 = conv(h, wo_0);
%u_1 = conv(h, wo_1);


%%
%Filtragem
y_0 = conv(r, wo_0);
figure, stem(y_0(1:30))
hold on
stem(x(1:30),'k')

%y_1 = conv(r, wo_1);
%figure, stem(y_1(2:31))
%hold on
%stem(x(1:30),'k')


%%
%erro quadratico medio

JJ_0 = 1 - Pxd_0.'*inv(Rx)*Pxd_0

%JJ_1 = 1 - Pxd_1.'*inv(Rx)*Pxd_1

%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%LMS
alpha = 0.15;
mu = 0.005;

err1 = []
w = zeros(2,N);
for n=1:N
    if n==1
        R = [x(1) 0]';
    else
        R = [x(n) x(n-1)]';
    end
    
    if n<N
        e(n) = d(n) - w(:,n)'*R;
        err1 = [e(n) err1];
        w(:,n+1) = w(:,n) + mu*R*e(n);
    end
end

figure, plot(wo_0(1)*ones(N,1),'r')
hold on
plot(wo_0(2)*ones(N,1),'r')
plot(w(1,:), 'k')
plot(w(2,:), 'k')
title('Evolução dos coeficientes do filtro em função do número de iterações LMS \mu = 0.005 (sinal impulsivo)')

%% item b - curvas de nível
w0 = linspace(-2,2,1000);
w1 = linspace(-2,2,1000);

var_d = var(d);
J = zeros(1000,1000);

for i = 1:1000
    for j = 1:1000
        ww = [w0(i) w1(j)];
        J(i,j) = var_d - 2*(ww*Pxd_0) + (ww)*Rx*(ww');
        
    end
        
end

levels = logspace(log10(min(J,[],"all")), log10(max(J,[],"all")), 20);

contour(w0,w1, J, levels)
hold on
plot(wo_0(2),wo_0(1),'r*')
plot(w(2,:),w(1,:),'k')


%%

plot(err1.^2,'k')
hold on 
title('Evolução erro quadrático médio LMS')
ylabel('e^2(n)')
ylim([0 20])
%% LMS sinal de erro

alpha = 0.15;
mu = 0.005;

err2 = [];
w = zeros(2,N);
for n=1:N
    if n==1
        R = [x(1) 0]';
    else
        R = [x(n) x(n-1)]';
    end
    
    if n<N
        e(n) = d(n) - w(:,n)'*R;
        err2 = [e(n) err2];
        w(:,n+1) = w(:,n) + mu*R*sign(e(n));
    end
end

figure, plot(wo_0(1)*ones(N,1),'r')
hold on
plot(wo_0(2)*ones(N,1),'r')
plot(w(1,:), 'k')
plot(w(2,:), 'k')
title('Evolução dos coeficientes do filtro em função do número de iterações LMS sinal de erro \mu = 0.005 (sinal impulsivo)')
%%
w0 = linspace(-2,2,1000);
w1 = linspace(-2,2,1000);

var_d = var(d);
J = zeros(1000,1000);

for i = 1:1000
    for j = 1:1000
        ww = [w0(i) w1(j)];
        J(i,j) = var_d - 2*(ww*Pxd_0) + (ww)*Rx*(ww');
        
    end
        
end

levels = logspace(log10(min(J,[],"all")), log10(max(J,[],"all")), 20);

contour(w0,w1, J, levels)
hold on
plot(wo_0(2),wo_0(1),'r*')
plot(w(2,:),w(1,:),'k')

%%
plot(err2.^2,'k')
hold on 
title('Evolução erro quadrático médio LMS sinal de erro')
ylabel('e^2(n)')
ylim([0 20])
%% RLS

M = 2;
alpha = 0.15;
mu = 0.004;
lambda = 0.999;
w = zeros(M,N);
delta = 10^(-4);
err3 = [];
P = (inv(delta))*eye(M);

for n=1:N
    if n==1
        R = [x(1) 0]';
        
    else
        R = [x(n) x(n-1)]';
        
    end
    
    if n<N
        err_3(n) = d(n) - R'*w(:,n);
        err3 = [err_3(n) err3];
        g = (P*R)*(inv(lambda + ((R')*(P*R))));
        P = inv(lambda)*P - g*(R')*inv(lambda)*P;
        w(:,n+1) = w(:,n) + g*err_3(n);
    end
end

figure, plot(wo_0(1)*ones(N,1),'r')
hold on
plot(wo_0(2)*ones(N,1),'r')
plot(w(1,:), 'k')
plot(w(2,:), 'k')
title('Evolução dos coeficientes do filtro em função do número de iterações RLS \lambda = 0.999 (sinal impulsivo)')
%%
w0 = linspace(-2,2,1000);
w1 = linspace(-2,2,1000);

var_d = var(d);
J = zeros(1000,1000);

for i = 1:1000
    for j = 1:1000
        ww = [w0(i) w1(j)];
        J(i,j) = var_d - 2*(ww*Pxd_0) + (ww)*Rx*(ww');
        
    end
        
end

levels = logspace(log10(min(J,[],"all")), log10(max(J,[],"all")), 20);

contour(w0,w1, J, levels)
hold on
plot(wo_0(2),wo_0(1),'r*')
plot(w(2,:),w(1,:),'k')

%%
plot(err3.^2,'k')
hold on 
title('Evolução erro quadrático médio RLS')
ylabel('e^2(n)')
ylim([0 20])
