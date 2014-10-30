function [Time T Tfxd] = get_T(filename, filter_treshold)
% convert ADU into T in Kelvins
% filename - name of file with temperature data
% filter_treshold - treshold level

	D = dlmread(filename);
	if(isempty(D)) return; endif;

	Time = (D(:,1)*2^24+D(:,2))/1000;

	% measured coefficients
	Kadd = [35.628     34.595     35.088     34.754     33.936     33.604     33.966     34.816];
	Kmul = [0.00014524 0.00014577  0.0001462 0.00014628 0.00014577 0.00014545 0.00014558 0.00014523];
	Kmul2 = [3.6789e-11  3.688e-11 3.6878e-11 3.6894e-11 3.6923e-11 3.6929e-11 3.6914e-11 3.6841e-11];
	Readed = D(:,[11:18]);
	nstr = [size(Readed, 1) 1];
	Kadd = repmat(Kadd, nstr);
	Kmul = repmat(Kmul, nstr);
	Kmul2 = repmat(Kmul2, nstr);
	R = Readed.^2 .* Kmul2 + Readed .* Kmul + Kadd;
	T = H705(R);

	% filtering
	if(nargin == 2)
		for i = 1:2
			DT = diff(T);
			[r c] = ind2sub(size(DT), find(DT < -filter_treshold));
			T(r+1, :) = [];
			R(r+1, :) = [];
			Time(r+1, :) = [];
		endfor
	endif

	Leg = ['1'; '2'; '3'; '4'; '5'; '6'; '7'; '8'];

	more_colors(plot(Time, R), 8);
	Tit = sprintf("Resistance, \\Omega");
	xlabel("Time, s"); ylabel("R, \\Omega"); title(Tit);
	print -dpng -color resistance.png;
	close
	more_colors(plot(Time, T), 8);
	Tit = sprintf("Temperature, ^\\circ{}C");
	xlabel("Time, s"); ylabel("T, ^\\circ{}C"); title(Tit);
	legend(Leg);
	print -dpng -color temperatures.png;
	close

	% calculate difference between thermometers
	Tavr = mean(T, 2);
	Tadd = mean(T - repmat(Tavr, [1 8]));
	Tfxd = T - repmat(Tadd,[size(T,1) 1]);
	more_colors(plot(Time, Tfxd), 8);
	Tit = sprintf("Temperature fixed to average value, ^\\circ{}C");
	xlabel("Time, s"); ylabel("T, ^\\circ{}C"); title(Tit);
	print -dpng -color temperatures_fixed.png;
	legend(Leg);
	close
	printf("differences:\n"); printf("%g\n", -Tadd);
endfunction

function more_colors(h, N)
	palette = jet (N);
	for i = 1:N
		set(h(i),"color",palette(i,:))
	endfor
endfunction
