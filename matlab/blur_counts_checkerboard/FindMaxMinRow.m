function [iMx,mx,iMn,mn] = FindMaxMinRow(maxgCol, intV, img_maxgRow, searchVal, posSlope)

% Find max and min on both the up slope and down slope.
% Scan after maxgCol and determine when the value starts to decrease/increase.
% For min, search at and before maxgCol and determine when the value
% starts to increase/decrease.

% intV: number of pixels evaluated for gradient
% img_maxgRow: row with max gradient
% maxgCol: column of start of max gradient (then it proceeds up or
% down, depending on direction of slope).
% searchVal: the pixel number used to search for max or min
% posSlope: Direction of max gradient 1:up or positivie, 0:down or negative

% Return
% iMx, mx: index of maximum, pixel value of maximum
% iMn, mn: index of minimum, pixel value of minimum

% Some slopes have humps prior to max or min - test to find real max,min
testPix = 5; % After finding max or min, the pixel number to test to see if
%           a max or min exists closeby.

img_w = length(img_maxgRow);

if posSlope == 1 % Up
    % Find max between start of gradient and some pixels beyond end of
    % gradient (searchVal)
    finalmx = maxgCol + searchVal+1; 
    mxVector = img_maxgRow(1,maxgCol+intV:finalmx)-img_maxgRow(1,maxgCol+intV-1:finalmx-1);
    % Find location where slope turns negative
    chgImx = find(mxVector < 0, 1,'first');
    iMx = maxgCol+chgImx-1+intV-1;
    mx = img_maxgRow(1,iMx);
    % Test max: check next 4 pixels (in case a hump in slope)
    if (iMx + 1 <= img_w) && (iMx + testPix <= img_w)
        [testmx, itestMx] = max(img_maxgRow(1,iMx+1:iMx+testPix));
        if img_maxgRow(1,itestMx + iMx) > img_maxgRow(1,iMx)
            iMx = itestMx + iMx;
            mx = testmx; 
        end
    end
    
    % Find min
    finalmn = maxgCol-searchVal-1;
    mnVector = img_maxgRow(1,maxgCol:-1:finalmn + 1) - img_maxgRow(1,maxgCol-1:-1:finalmn);
    chgImn = find(mnVector < 0, 1,'first');
    iMn = maxgCol-chgImn+1;
    mn = img_maxgRow(1,iMn);
    % Test min: check previous 4 pixels (in case a hump in slope)
    if (iMn - 1 >= 1) && (iMn-testPix >= 1)
        [testmn, itestMn] = min(img_maxgRow(1,iMn-1:-1:iMn-testPix));
        if img_maxgRow(1,iMn-itestMn) < img_maxgRow(1,iMn)
            iMn = iMn - itestMn; 
            mn = testmn; 
        end
    end
else % Down (posSlope == 0)
    % Find max
    finalmx = maxgCol-searchVal-1;
    mxVector = img_maxgRow(1,maxgCol:-1:finalmx + 1) - img_maxgRow(1,maxgCol-1:-1:finalmx);
    chgImx = find(mxVector > 0, 1,'first');
    iMx = maxgCol-chgImx+1;
    mx = img_maxgRow(1,iMx);
    % Test max: check previous 4 pixels (in case a hump in slope)
    if (iMx - 1 >= 1) && (iMx-testPix >= 1)
        [testmx, itestMx] = max(img_maxgRow(1,iMx-1:-1:iMx-testPix));
        if img_maxgRow(1,iMx - itestMx) > img_maxgRow(1,iMx)
            iMx = iMx - itestMx;
            mx = testmx;
        end
    end
   
    % Find min
    finalmn = maxgCol + searchVal+1;  
    mnVector = img_maxgRow(1,maxgCol+intV:finalmn)-img_maxgRow(1,maxgCol+intV-1:finalmn-1);
    chgImn = find(mnVector > 0, 1,'first');
    iMn = maxgCol+chgImn-1+intV-1;
    mn = img_maxgRow(1,iMn);
    % Test min: check next 4 pixels (in case a hump in slope)
    if (iMn + 1 <= img_w) && (iMn + testPix <= img_w)
        [testmn, itestMn] = min(img_maxgRow(1,iMn+1:iMn+testPix));
        if img_maxgRow(1,itestMn + iMn) < img_maxgRow(1,iMn)
            iMn = itestMn + iMn;
            mn = testmn;
        end
    end
end



end