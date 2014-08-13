function Tout = H705(Rin)
% Converts resistance of TRD into T (degrC)

_alpha = 0.00375;
_beta = 0.16;
_delta = 1.605;
T = [-200:0.1:50];
_A = _alpha + _alpha*_delta/100.;
_B = -_alpha*_delta/1e4;
_C = zeros(size(T));
_C(find(T<0.)) = -_alpha*_beta/1e8;
rT = 1000.*(1 + _A*T + _B*T.^2 - _C.*T.^3*100. + _C.*T.^4);
%plot(T, rT);
Tout = interp1(rT, T, Rin, 'spline')
endfunction
