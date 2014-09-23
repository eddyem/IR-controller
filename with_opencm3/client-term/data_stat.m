function data_stat(filename, msrd)
% prints some statistics
% also plot graphs

	MF = 0;
	if(nargin == 0) filename = 'tempout'; endif;
	if(nargin == 2) MF = 1; endif;
	D = dlmread(filename);
	if(isempty(D)) return; endif;
%	[r c] = ind2sub(size(D), find(D(:,[3:18]) == 0)); % find bad data
%	D(r,:) = []; % and delete it
%	[r c] = ind2sub(size(D), (D(:,[11:18]) < 2000000));
%	D(r,:) = [];
	printf("Some statistics:\n\n\trelative error for inner ADC:\n");

	Time = (D(:,1)*2^24+D(:,2))/1000;
	T = []; Rr = [];

	x = rel_error(D(:,[3 4 6:10]));
	printf("%f ", x);
	avrg = mean(x);
	printf("\t(aver: %f)\n", avrg);
	printf("\n\trelative error for outern ADC:\n\n");
	plot(Time, D(:,[3 4 6:10]));
	if(MF)
		R = dlmread(msrd);
		Rr = R(:,2);
		T = R(:,1)/1000;
		addlabels(Time, D(:,[3 4 6:10]), T, Rr);
	endif;
	Tit = sprintf("Internal 12-bit ADC, err=%f%%", avrg);
	xlabel("Time, s"); ylabel("R, ADU"); title(Tit);
	print -dpng -color int.png;
	close;

	x = rel_error(D(:,[11:18]));
	avrg = mean(x);
	printf("%f ", x);
	printf("\t(aver: %f)\n", avrg);
	printf("\n");
	plot(Time, D(:,[11:18]));
	if(MF)
		addlabels(Time, D(:,[11:18]), T, Rr);
	endif;
	Tit = sprintf("External 24-bit ADC, err=%f%%", avrg);
	xlabel("Time, s"); ylabel("R, ADU"); title(Tit);
	print -dpng -color ext.png;
	close

	meanline = mean(D(:,[11:18])')';
	diffs = mean(D(:,[11:18]) - meanline)  % average difference between mean line
	newD  = D(:,[11:18]) - diffs;
	plot(Time, newD);
	if(MF)
		addlabels(Time, newD, T, Rr);
	endif;
	Tit = sprintf("External 24-bit ADC, corrected");
	xlabel("Time, s"); ylabel("R, ADU"); title(Tit);
	print -dpng -color ext_corr.png;
	close
	if(MF)
		% R = K*ADU -> K = ADU \ R
		Kmul = []; Kadd = []; Kmul2 = [];
		for i = 11:18
		dat = interp1(Time, D(:,i), T);
		N = [ ones(size(dat)) dat dat.^2];
		K = N \ Rr;
		Kadd = [Kadd K(1)];
		Kmul = [Kmul K(2)];
		Kmul2= [Kmul2 K(3)];
		endfor
		%Kmul = median(Kmul);
		%Kadd = median(Rr - dat*Kmul);
		printf("coefficients:\n\tKadd = %s\n\tKmul = %s\n\tKmul2 = %s\n", num2str(Kadd), ...
			num2str(Kmul), num2str(Kmul2));
		printf("stat:\n\t<Kmul> = %g, sigma = %g\n\t<Kmul2> = %g, sigma = %g\n", ...
			median(Kmul), std(Kmul), median(Kmul2), std(Kmul2));
		dd = D(:,[11:18]);
		newR  = dd.^2 .* Kmul2 + dd .* Kmul + Kadd;
		plot(Time, newR);
		addlabels(Time, newR, T, Rr);
		Tit = sprintf("External 24-bit ADC, corrected");
		xlabel("Time, s"); ylabel("R, Ohm"); title(Tit);
		print -dpng -color ext_corr_Ohm.png;
	endif
endfunction

function addlabels(Time, data, T, Rr)
%
% plot circles with R label
% - Time - original time labels
% - data - original data
% - T    - time labels for Rr
% - Rr   - measured resistance
%
	meanline = mean(data')';
	newDr = interp1(Time, meanline, T);
	%N = [ ones(size(Rr)) Rr Rr.^2];
	N = [ ones(size(Rr)) Rr ];
	K = N \ newDr
	%Y =  Rr.^2 * K(3) + Rr * K(2) + K(1);
	Y =  Rr * K(2) + K(1);
	hold on; plot(T, Y, '.');
	text(T+10, Y, num2str(Rr));
	%plot(Time, meanline, 'o');
	hold off;
endfunction
