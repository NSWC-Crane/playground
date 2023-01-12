function [blurpix] = CalculateBlur(img_maxgCol, iMn, iMx, mn, mx)

% Find blur count
allow = 3;
indMn = find(img_maxgCol(iMn:iMx) > mn + allow,1,"first");
indMx = find(img_maxgCol(iMn:iMx) < mx - allow,1,"last");
blurpix = abs(indMx - indMn + 1);

end