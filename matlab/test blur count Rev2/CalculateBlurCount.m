function [xT, yT, numBlurPix,startPix, stopPix] = CalculateBlurCount(img_line, intv)

%% Smoothing curve
% average first 1/2 of intv for point one
% average next 1/2 and 1/2 beyond next point for point two

xT = 1:intv:length(img_line);
halfI = round(intv/2);

yT = zeros(round(length(img_line)/intv),1);

yT(1) = mean(img_line(1:halfI));
ind = halfI+1;
for i = 2:length(yT)-1
    yT(i) = mean(img_line(ind:ind+intv-1));
    ind = ind+intv;
end

yT(i+1) = mean(img_line(ind:end));

[~,indyTmx] = max(yT);
[~,indyTmn] = min(yT);

deltaMax = 7;
deltaMin = 5;

startPix = SearchMinMax(img_line, yT, deltaMax, indyTmx, 'max');
stopPix = SearchMinMax(img_line, yT, deltaMin, indyTmn, 'min');

numBlurPix = stopPix - startPix;
end