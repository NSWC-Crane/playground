function [y, ymax, ymin] = FitCurveF(x, img_line, domain)
% Fits curve of line image values with pixel number as x value
% x:  x-axis values of curve
% img_line: row image selected from main image
% domain:
% Returns:
% y: fitted curve
% ymax: logical of local maximas
% ymin: logical of local minimas

imgL_FF = fit(x,img_line.','fourier3');
y = imgL_FF(x);

% Get local max and min on fitted curve
ymax = islocalmax(y);
ymin = islocalmin(y);

% Test for location of max mins
% Are there any mins to the right of the start of domain?
if sum(ymin(domain(1):end)) == 0
    ymin = zeros(size(ymin));
end
% Are there any maxs to the left of the end of domain?
if sum(ymax(1:domain(end))) == 0
    ymax = zeros(size(ymax));
end

end