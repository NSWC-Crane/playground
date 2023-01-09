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

kernel = (1/intv) * ones(intv,1);

yT = conv(img_line, kernel, 'valid');
yT = cat(2, yT(1)*ones(1, floor(intv/2)), yT, yT(end)*ones(1, floor(intv/2)));
xT = 1:length(yT);
        

% Finds max and min on new curve
[~,indyTmx] = max(yT);
[~,indyTmn] = min(yT);

% Allows tolerance in pixel value for real max and min
deltaMax = 5;
deltaMin = 5;

startPix = SearchMinMax(img_line, yT, deltaMax, indyTmx, 'max');
stopPix = SearchMinMax(img_line, yT, deltaMin, indyTmn, 'min');

% Calculates number of blurred pixels
numBlurPix = stopPix - startPix;
end