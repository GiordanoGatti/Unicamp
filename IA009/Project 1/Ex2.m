clc
clear all
close all
load('dados_sistema.mat')

avgx = 0;
varx = 1;

J = zeros(10,1);
Ef = zeros(10,1);
alpha = 0;
for M = 2:2:20
    [Rx, rx] = autocorrelacao(x, M);
    %correlacao cruzada
    Pxd = corr_cruzada(x, d, M, alpha);
    %filtro otimo - Wiener
    Wo = inv(Rx)*Pxd;
    %resposta ao impulso
    imp = zeros(50, 1);
    imp(1)=1;
    imp_res = conv(Wo,imp);
    figure()
    stem(imp_res, 'k')
    xlabel('samples')
    ylabel('IR magnitude')
    grid()
    %resultado da filtragem
    y = conv(x,Wo);
    figure()
    stem(y(1:50), 'b')
    hold on
    stem(d(1:50),'r')
    legend()
    xlabel('samples')
    ylabel('magnitude')
    grid()
    
    
    %erro quadratico medio
    J(M/2) = varx - Pxd.'*inv(Rx)*Pxd;
end

for M = 1:1:20    
    [Rd, rd] = autocorrelacao(d, M);
end
    %FEPPPU
    wf = inv(Rd)*rd;
    a = [1;(-1)*wf]
    %erro quadratico medio
    Ef(M) = a.'*d(T-M:T).';
    
figure()
plot(J, 'r')
xlabel('#coeficientes M')
ylabel('E\{e(n)^2\}')
grid()
figure()
plot(Ef(1:end-1), 'r')
xlabel('#coeficientes M')
ylabel('E\{e(n)^2\}')
grid()









