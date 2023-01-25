function [iMx,mx,iMn,mn] = FindMaxMin(maxgRw,intV,img_maxgCol, searchVal, posSlope)

% Find max and min on both the up slope and down slope.
% Scan after maxgRw and determine when the value starts to decrease/increase.
% For min, search at and before maxgRx and determine when the value
% starts to increase/decrease.

% intV: number of pixels evaluated for gradient
% img_maxgCol: column of max gradient
% maxgRw: row of start of max gradient (then it proceeds up or
% down, depending on direction of slope).
% searchVal: the pixel number used to search for max or min
% posSlope: Direction of max gradient 1:up or positivie, 0:down or negative

% Return
% iMx, mx: index of maximum, pixel value of maximum
% iMn, mn: index of minimum, pixel value of minimum

% Some slopes have humps prior to max or min - test to find real max,min
testPix = 5; % After finding max or min, the pixel number to test to see if
%           a max or min exists closeby.

img_h = length(img_maxgCol);

if posSlope == 1 % Up
    % Find max
    finalmx = maxgRw + searchVal+1; 
    mxVector = img_maxgCol(maxgRw+intV:finalmx)-img_maxgCol(maxgRw+intV-1:finalmx-1);
    chgImx = find(mxVector < 0, 1,'first');
    iMx = maxgRw+chgImx-1+intV-1;
    mx = img_maxgCol(iMx);
    % Test max: check next 4 pixels (in case a hump in slope)
    if (iMx + 1 <= img_h) && (iMx + testPix <= img_h)
        [testmx, itestMx] = max(img_maxgCol(iMx+1:iMx+testPix));
        if img_maxgCol(itestMx + iMx) > img_maxgCol(iMx)
            iMx = itestMx + iMx;
            mx = testmx; 
        end
    end
    
    % Find min
    finalmn = maxgRw-searchVal-1;
    mnVector = img_maxgCol(maxgRw:-1:finalmn + 1) - img_maxgCol(maxgRw-1:-1:finalmn);
    chgImn = find(mnVector < 0, 1,'first');
    iMn = maxgRw-chgImn+1;
    mn = img_maxgCol(iMn);
    % Test min: check previous 4 pixels (in case a hump in slope)
    if (iMn - 1 >= 1) && (iMn-testPix >= 1)
        [testmn, itestMn] = min(img_maxgCol(iMn-1:-1:iMn-testPix));
        if img_maxgCol(iMn-itestMn) < img_maxgCol(iMn)
            iMn = iMn - itestMn; 
            mn = testmn; 
        end
    end
else % Down (posSlope == 0)
    % Find max
    finalmx = maxgRw-searchVal-1;
    mxVector = img_maxgCol(maxgRw:-1:finalmx + 1) - img_maxgCol(maxgRw-1:-1:finalmx);
    chgImx = find(mxVector > 0, 1,'first');
    iMx = maxgRw-chgImx+1;
    mx = img_maxgCol(iMx);
    % Test max: check previous 4 pixels (in case a hump in slope)
    if (iMx - 1 >= 1) && (iMx-testPix >= 1)
        [testmx, itestMx] = max(img_maxgCol(iMx-1:-1:iMx-testPix));
        if img_maxgCol(iMx - itestMx) > img_maxgCol(iMx)
            iMx = iMx - itestMx;
            mx = testmx;
        end
    end
   
    % Find min
    finalmn = maxgRw + searchVal+1;  
    mnVector = img_maxgCol(maxgRw+intV:finalmn)-img_maxgCol(maxgRw+intV-1:finalmn-1);
    chgImn = find(mnVector > 0, 1,'first');
    iMn = maxgRw+chgImn-1+intV-1;
    mn = img_maxgCol(iMn);
    % Test min: check next 4 pixels (in case a hump in slope)
    if (iMn + 1 <= img_h) && (iMn + testPix <= img_h)
        [testmn, itestMn] = min(img_maxgCol(iMn+1:iMn+testPix));
        if img_maxgCol(itestMn + iMn) < img_maxgCol(iMn)
            iMn = itestMn + iMn;
            mn = testmn;
        end
    end
end



end