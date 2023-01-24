function [iMx,mx,iMn,mn] = FindMaxMin(maxgRw,intV,img_maxgCol, searchVal, direction)

% Find max and min on both the up slope and down slope.
% Scan after maxgRw and determine when the value starts to decrease/increase.
% For min, search at and before maxgRx and determine when the value
% starts to increase/decrease.

% intV: number of pixels evaluated for gradient
% img_maxgCol: column of max gradient
% maxgRw: row of start of max gradient (then it proceeds up or
% down, depending on direction).
% searchVal: the pixel number used to search for max or min
% direction: Direction of max gradient 1:up, 0:down

% Return
% iMx, mx: index of maximum, pixel value of maximum
% iMn, mn: index of minimum, pixel value of minimum
testPix = 5; % After finding max or min, the pixel number to test to see if
%           a max or min exists closeby.

if direction == 1 % Up
    % Find max
    finalmx = maxgRw + searchVal+1; 
    mxVector = img_maxgCol(maxgRw+intV:finalmx)-img_maxgCol(maxgRw+intV-1:finalmx-1);
    chgImx = find(mxVector < 0, 1,'first');
    iMx = maxgRw+chgImx-1+intV-1;
    mx = img_maxgCol(iMx);
    % Test max: check next 4 pixels (in case a hump in slope)
    [testmx, itestMx] = max(img_maxgCol(iMx+1:iMx+testPix));
    if img_maxgCol(itestMx + iMx) > img_maxgCol(iMx)
        iMx = itestMx + iMx;
        mx = testmx; 
    end
    
    % Find min
    finalmn = maxgRw-searchVal-1;
    mnVector = img_maxgCol(maxgRw:-1:finalmn + 1) - img_maxgCol(maxgRw-1:-1:finalmn);
    chgImn = find(mnVector < 0, 1,'first');
    iMn = maxgRw-chgImn+1;
    mn = img_maxgCol(iMn);
    % Test min: check previous 4 pixels (in case a hump in slope)
    [testmn, itestMn] = min(img_maxgCol(iMn-1:-1:iMn-testPix));
    if img_maxgCol(iMn-itestMn) < img_maxgCol(iMn)
        iMn = iMn - itestMn; 
        mn = testmn; 
    end

else % Down (direction == 0)
    % Find max
    finalmx = maxgRw-searchVal-1;
    mxVector = img_maxgCol(maxgRw:-1:finalmx + 1) - img_maxgCol(maxgRw-1:-1:finalmx);
    chgImx = find(mxVector > 0, 1,'first');
    iMx = maxgRw-chgImx+1;
    mx = img_maxgCol(iMx);
    % Test max: check previous 4 pixels (in case a hump in slope)
    [testmx, itestMx] = max(img_maxgCol(iMx-1:-1:iMx-testPix));
    if img_maxgCol(iMx - itestMx) > img_maxgCol(iMx)
        iMx = iMx - itestMx;
        mx = testmx;
    end
   
    % Find min
    finalmn = maxgRw + searchVal+1;  
    mnVector = img_maxgCol(maxgRw+intV:finalmn)-img_maxgCol(maxgRw+intV-1:finalmn-1);
    chgImn = find(mnVector > 0, 1,'first');
    iMn = maxgRw+chgImn-1+intV-1;
    mn = img_maxgCol(iMn);
    % Test min: check next 4 pixels (in case a hump in slope)
    [testmn, itestMn] = min(img_maxgCol(iMn+1:iMn+testPix));
    if img_maxgCol(itestMn + iMn) < img_maxgCol(iMn)
        iMn = itestMn + iMn;
        mn = testmn;
    end
end



end