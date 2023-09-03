function [Rx, rx] = autocorrelacao(x, N_coef)
    Rx = zeros(N_coef);
    rx = zeros(N_coef, 1);
    for i = 0:1:(N_coef-1)
        r = (x(1:end-i)*x(i+1:end).')/(length(x)-i);
        dia = r(ones(N_coef-i, 1));
        if i==0
            Rx = Rx+diag(dia,i);                        
        else  
            Rx = Rx+diag(dia,i);
            Rx = Rx+diag(dia,-i);
            rx(i) = r;            
        end
        clear dia
        clear r
    end
    rx(N_coef) = (x(1:end-N_coef)*x(N_coef+1:end).')/(length(x)-N_coef);
    Rx;
    rx;
end