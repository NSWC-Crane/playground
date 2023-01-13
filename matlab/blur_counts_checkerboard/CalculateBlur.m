function [blurpix,indMn, indMx] = CalculateBlur(img_maxgCol, iMn, iMx, mn, mx, direction)

% Find blur count
allow = 5; % Allowance for pixel value of max and min to deviate.

if direction == 1

    indMn = find(img_maxgCol(iMn:iMx) > mn + allow,1,"first");
    indMx = find(img_maxgCol(iMn:iMx) < mx - allow,1,"last");
    blurpix = abs(indMx - indMn + 1);
else
    indMn = find(img_maxgCol(iMn:-1:iMx) > mn + allow,1,"first");
    indMx = find(img_maxgCol(iMn:-1:iMx) < mx - allow,1,"last");
    blurpix = abs(indMx - indMn + 1);

end

end