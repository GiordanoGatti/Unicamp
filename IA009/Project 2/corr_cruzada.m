function Pxd = corr_cruzada(x, d, N_coef, alpha)
    Pxd = zeros(N_coef, 1);
    for j = -alpha:1:(N_coef-1)
        Pxd(abs(j)+1) = (x(1:end-abs(j))*d(abs(j)+1:end).')/(length(x)-abs(j));
    end
    Pxd;
end