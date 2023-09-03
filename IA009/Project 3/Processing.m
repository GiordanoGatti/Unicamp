clear all
close all
clc

load('ECG_database.mat');

f = 60; %Hz
fs = 360; %samples/s

ecg_clean = Data1/max(Data1); %normalizando dataset
pl = (rand/2)*sin(2*pi*f*(0:LENGTH-1)/fs) + rand(LENGTH,1)'/10; %criando o ruido da rede
ref = sin(2*pi*f*(0:length(Data1)-1)/fs); %criando um sinal de referencia (correlacionado a pl)
ecg_noisy = ecg_clean+pl; %

figure()
hold on
plot(ecg_noisy(1:1000), 'b')
plot(ecg_clean(1:1000), 'r')
grid()
xlabel('samples','fontsize', 15)
ylabel('Amplitude','fontsize', 15)
legend('Noisy ECG', 'clean ECG')

figure()
hold on
plot(pl(1:100))
plot(ref(1:100), 'r')
grid()
xlabel('samples','fontsize', 15)
ylabel('Amplitude','fontsize', 15)
legend('power line noise', 'reference signal')

M = 2; %coeficientes
mu = 0.02; %passo
lambda = 1; %fator de esquecimento

[e_LMS, y_LMS, w_LMS] = myLMS(ecg_noisy, ref, mu, M);            %LMS
[e_NLMS, y_NLMS, w_NLMS] = myNLMS(ecg_noisy, ref, mu, M, 0.01);   %NLMS
[e_RLS, y_RLS, w_RLS] = myRLS(ecg_noisy, ref, lambda, M);        %RLS

figure()
subplot(3,1,1)
hold on
plot(e_LMS(1:1000), 'b')
plot(ecg_clean(1:1000), 'r')
legend('LMS filtered', 'clean ECG')
ylabel('Amplitude','fontsize', 15)
grid()
subplot(3,1,2)
hold on
plot(e_NLMS(1:1000), 'b')
plot(ecg_clean(1:1000), 'r')
legend('NLMS filtered', 'clean ECG')
ylabel('Amplitude','fontsize', 15)
grid()
subplot(3, 1, 3)
hold on
plot(e_RLS(1:1000), 'b')
plot(ecg_clean(1:1000), 'r')
legend('RLS filtered', 'clean ECG')
ylabel('Amplitude','fontsize', 15)
xlabel('samples','fontsize', 15)
grid()

[e_LMS_pl, y_LMS_pl, w_LMS_pl] = myLMS(ecg_noisy, pl, mu, M);            %LMS
[e_NLMS_pl, y_NLMS_pl, w_NLMS_pl] = myNLMS(ecg_noisy, pl, mu, M, 0.01);   %NLMS
[e_RLS_pl, y_RLS_pl, w_RLS_pl] = myRLS(ecg_noisy, pl, lambda, M);        %RLS

figure()
subplot(3,1,1)
hold on
plot(e_LMS_pl(1:1000), 'b')
plot(ecg_clean(1:1000), 'r')
legend('LMS filtered', 'clean ECG')
ylabel('Amplitude','fontsize', 15)
grid()
subplot(3,1,2)
hold on
plot(e_NLMS_pl(1:1000), 'b')
plot(ecg_clean(1:1000), 'r')
legend('NLMS filtered', 'clean ECG')
ylabel('Amplitude','fontsize', 15)
grid()
subplot(3, 1, 3)
hold on
plot(e_RLS_pl(1:1000), 'b')
plot(ecg_clean(1:1000), 'r')
legend('RLS filtered', 'clean ECG')
ylabel('Amplitude','fontsize', 15)
xlabel('samples','fontsize', 15)
grid()
