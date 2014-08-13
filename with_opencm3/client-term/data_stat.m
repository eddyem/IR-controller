function data_stat(filename)
% prints some statistics
% also plot graphs

	if(nargin == 0) filename = 'tempout'; endif
	D = dlmread(filename);
	if(isempty(D)) return; endif
	[r c] = ind2sub(size(D), find(D(:,[3:18]) == 0)); % find bad data
	D(r,:) = []; % and delete it
%	[r c] = ind2sub(size(D), (D(:,[11:18]) < 2000000));
%	D(r,:) = [];
	printf("Some statistics:\n\n\trelative error for inner ADC:\n");

	x = rel_error(D(:,[3 4 6:10]));
	printf("%f ", x);
	printf("\t(aver: %f)\n", mean(x));
	printf("\n\trelative error for outern ADC:\n\n");
	plot((D(:,1)*2^24+D(:,2))/1000, D(:,[3 4 6:10]));
	T = sprintf("Internal 12-bit ADC, err=%f%%", mean(x));
	xlabel("Time, s"); ylabel("R, ADU"); title(T);
	print -dpng -color int.png

	x = rel_error(D(:,[11:18]));
	printf("%f ", x);
	printf("\t(aver: %f)\n", mean(x));
	printf("\n");
	plot((D(:,1)*2^24+D(:,2))/1000, D(:,[11:18]));
	T = sprintf("External 24-bit ADC, err=%f%%", mean(x));
	xlabel("Time, s"); ylabel("R, ADU"); title(T);
	print -dpng -color ext.png


	%D = D(1:200, :);

	close
endfunction
