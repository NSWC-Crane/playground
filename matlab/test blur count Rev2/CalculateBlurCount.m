function [xT, yT, numBlurPix,startPix] = CalculateBlurCount(img_line, intv)
% Smooths the image line by taking points at each interval and then averages the
% points before and after the interval point.  Then finds max and min on
% the smoothed curve.  Next alters max and min by deltaMax or deltaMin to
% allow a tolerance of the pixel values in max and min.  Uses SearchMinMax
% to find new location of max and min. 

% img_line: line image created by slicing image
% intv: number of pixels in interval for creating a smoother curve.
% Returns:
% xT, yT: Defines points on smoothing curve.
% numBlurPix: number of blurred pixels
% startPix: location of start of blurred pixels in line image

%% Smoothing curve
% point 1: get mean of pixel values from pixel 1 to pixel at half of
% interval.
% point 2: get mean of pixel values for pixels that are at half the
% interval size before the point and for half the interval size after the
% point.
% point last:  get mean of pixel values from pixels at half of
% interval before this pixel to the last pixel in image.

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

% Finds max and min on new curve
[~,indyTmx] = max(yT);
[~,indyTmn] = min(yT);

% Allows tolerance in pixel value for real max and min
deltaMax = 7;
deltaMin = 5;
startPix = SearchMinMax(img_line, yT, deltaMax, indyTmx, 'max');
stopPix = SearchMinMax(img_line, yT, deltaMin, indyTmn, 'min');

% Calculates number of blurred pixels
numBlurPix = stopPix - startPix;
end