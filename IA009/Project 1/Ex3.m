N = 1000000;

s = sign(randn(N,1));
h = [0.2; 0.7; 1.3];

x_ = conv(h,s);
x_ = x_(1:end-1);
x_ = x_ ;
figure, stem(x_(1:30));


std=0.01;              
meanValue=0;         
x = x_ + std*randn(size(x_)) + meanValue;
x = x_(1:end-1);

%% item a
rx_0 = (x.' *x)/length(x);
rx_1 = (x(1:end-1).'*x(2:end))/(length(x)-1);
rx_2 = (x(1:end-2).'*x(3:end))/(length(x)-2);
rx_3 = (x(1:end-3).'*x(4:end))/(length(x)-3);
rx_4 = (x(1:end-4).'*x(5:end))/(length(x)-4);
rx_5 = (x(1:end-5).'*x(6:end))/(length(x)-5);

%Rx = [rx_0 rx_1 rx_2 rx_3 rx_4; 
%      rx_1 rx_0 rx_1 rx_2 rx_3; 
%      rx_2 rx_1 rx_0 rx_1 rx_2;
%      rx_3 rx_2 rx_1 rx_0 rx_1;
%      rx_4 rx_3 rx_2 rx_1 rx_0];

Rx = [rx_0 rx_1 ; 
      rx_1 rx_0];
%% item b
%pxd_0 = [(x.'*s)/length(x); 
%         (x(1:end-1).'*s(2:end))/(length(x)-1);
%         (x(1:end-2).'*s(3:end))/(length(x)-2);
%         (x(1:end-3).'*s(4:end))/(length(x)-3);
%         (x(1:end-4).'*s(5:end))/(length(x)-4)];

pxd_0 = [(x.'*s)/length(x); 
         (x(1:end-1).'*s(2:end))/(length(x)-1)];

%pxd_1 = [(x(2:end).'*s(1:end-1)/(length(x)-1));
%         (x.'*s)/length(x); 
%        (x(1:end-1).'*s(2:end))/(length(x)-1);
%         (x(1:end-2).'*s(3:end))/(length(x)-2);
%         (x(1:end-3).'*s(4:end))/(length(x)-3)];

pxd_1 = [(x(2:end).'*s(1:end-1)/(length(x)-1));
         (x.'*s)/length(x)];

%pxd_2 = [(x(3:end).'*s(1:end-2)/(length(x)-2));
%         (x(2:end).'*s(1:end-1)/(length(x)-1));
%         (x.'*s)/length(x);
%         (x(1:end-1).'*s(2:end))/(length(x)-1);
%         (x(1:end-2).'*s(3:end))/(length(x)-2)];
     
pxd_2 = [(x(3:end).'*s(1:end-2)/(length(x)-2));
         (x(2:end).'*s(1:end-1)/(length(x)-1))];     

%pxd_3 = [(x(4:end).'*s(1:end-3)/(length(x)-3));
%         (x(3:end).'*s(1:end-2)/(length(x)-2));
%         (x(2:end).'*s(1:end-1)/(length(x)-1));
%         (x.'*s)/length(x);
%         (x(1:end-1).'*s(2:end))/(length(x)-1)];
     
pxd_3 = [(x(4:end).'*s(1:end-3)/(length(x)-3));
         (x(3:end).'*s(1:end-2)/(length(x)-2))];     
     
%pxd_4 = [(x(5:end).'*s(1:end-4)/(length(x)-4));
%         (x(4:end).'*s(1:end-3)/(length(x)-3));
%         (x(3:end).'*s(1:end-2)/(length(x)-2));
%         (x(2:end).'*s(1:end-1)/(length(x)-1));
%         (x.'*s)/length(x)];     
     
pxd_4 = [(x(5:end).'*s(1:end-4)/(length(x)-4));
         (x(4:end).'*s(1:end-3)/(length(x)-3))];       
     
%pxd_5 = [(x(6:end).'*s(1:end-5)/(length(x)-5));
%         (x(5:end).'*s(1:end-4)/(length(x)-4));
%         (x(4:end).'*s(1:end-3)/(length(x)-3));
%         (x(3:end).'*s(1:end-2)/(length(x)-2));
%         (x(2:end).'*s(1:end-1)/(length(x)-1))];     
     
pxd_5 = [(x(6:end).'*s(1:end-5)/(length(x)-5));
         (x(5:end).'*s(1:end-4)/(length(x)-4))];     

 %% item c
wo_0 = inv(Rx)*pxd_0;
wo_1 = inv(Rx)*pxd_1;
wo_2 = inv(Rx)*pxd_2;
wo_3 = inv(Rx)*pxd_3;
wo_4 = inv(Rx)*pxd_4;
wo_5 = inv(Rx)*pxd_5;


J0 = std^2 - pxd_0.'*inv(Rx)*pxd_0;
J1 = std^2 - pxd_1.'*inv(Rx)*pxd_1;
J2 = std^2 - pxd_2.'*inv(Rx)*pxd_2;
J3 = std^2 - pxd_3.'*inv(Rx)*pxd_3;
J4 = std^2 - pxd_4.'*inv(Rx)*pxd_4;
J5 = std^2 - pxd_5.'*inv(Rx)*pxd_5;

%% item d


%% item e
y0 = conv(h,wo_0)
figure, stem(y0);
hold on
y1 = conv(h,wo_1)
figure, stem(y1);
hold on
y2 = conv(h,wo_2)
figure, stem(y2);
hold on
y3 = conv(h,wo_3)
figure, stem(y3);
hold on

%% item f