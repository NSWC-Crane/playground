function [blurpix,indMn, indMx] = CalculateBlur(img_maxgCol, iMn, iMx, mn, mx, posSlope)

% Find blur count for defined gradient with max and min
% img_maxgCol: the column of the image with the max gradient
% iMn: the row of img_maxgCol where the minimum pixel value of the gradient
% resides
% IMx: the row of img_maxgCol where the maximum pixel value of the gradient
% resides
% mn: the minimum pixel value
% mx: the maximum pixel value
% posSlope: binary indicating if slope is positive (1:true, 0:false)

% Allowance for pixel value of max and min to deviate.
% For example, if max value is 190, then neighbor values between 185 to 190
% are eliminated from blur count.
allow = 3; 

if posSlope == 1 % Slope of gradient is positive

    indMn = find(img_maxgCol(iMn:iMx) > mn + allow,1,"first");
    indMx = find(img_maxgCol(iMn:iMx) < mx - allow,1,"last");
    blurpix = abs(indMx - indMn + 1);
else % Slope of gradient is negative
    indMn = find(img_maxgCol(iMn:-1:iMx) > mn + allow,1,"first");
    indMx = find(img_maxgCol(iMn:-1:iMx) < mx - allow,1,"last");
    blurpix = abs(indMx - indMn + 1);

end

end