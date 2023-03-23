function [blurpix,indMn, indMx] = CalculateBlur2(iMn, iMx, mn, mx)

% Find blur count for defined gradient with max and min

blurpix = iMn-iMx;
indMn = iMn;
indMx = iMx;

end