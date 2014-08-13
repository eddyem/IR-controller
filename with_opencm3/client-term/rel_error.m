function e = rel_error(x)
%
% returns relative error for data x in percents!
% e = std(x) ./ mean(x) * 100.

e = std(x) ./ mean(x) * 100.;

endfunction
