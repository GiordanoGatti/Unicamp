clear all
close all
clc
load('sismograma.mat')
load('refletividade.mat')

M =100;

[Rt, rt] = autocorrelacao(sismograma.', M);
[Rr, rr] = autocorrelacao(refletividade.', M);

%FEPPPU
wf = inv(Rt)*rt;
a = [1;(-1)*wf];

%erro quadratico medio
Ef = a.'*refletividade(length(end-M:end)).';

%estimativa
y = conv(a, sismograma);
[Ry, ry] = autocorrelacao(y.', M);

%plots
figure()
plot(y, 'b')
hold on
plot(refletividade, 'r')
xlabel('samples')
ylabel('magnitude')
legend({'estimado', 'funcao refletividade'})
grid()

figure()
subplot(3,1,1)
stem([Rt(1,1);rt])
ylabel('sismograma', 'Fontsize', 15)
title('sequencia de autocorrelacao', 'Fontsize', 15)
grid()
subplot(3,1,2)
stem([Rr(1,1);rr])
ylabel('funcao refletividade', 'Fontsize', 15)
grid()
subplot(3,1,3)
stem([Ry(1,1);ry])
ylabel('estimativa', 'Fontsize', 15)
xlabel('atraso', 'Fontsize', 15)
grid()






