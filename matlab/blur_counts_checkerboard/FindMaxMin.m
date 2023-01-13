function [iMx,mx,iMn,mn] = FindMaxMin(maxgRw,intV,img_maxgCol, searchVal, direction)

% Find max and min on both the up slope and down slope.
% Scan after maxgRw and determine when the value starts to decrease/increase.
% For min, search at and before maxgRx and determine when the value
% starts to increase/decrease.

if direction == 1
    % Find max
    finalmx = maxgRw + searchVal+1;
    mxVector = img_maxgCol(maxgRw+intV:finalmx)-img_maxgCol(maxgRw+intV-1:finalmx-1);
    chgImx = find(mxVector < 0, 1,'first');
    iMx = maxgRw+chgImx-1+intV-1;
    mx = img_maxgCol(iMx);
    
    % Find min
    finalmn = maxgRw-searchVal-1;
    mnVector = img_maxgCol(maxgRw:-1:finalmn + 1) - img_maxgCol(maxgRw-1:-1:finalmn);
    chgImn = find(mnVector < 0, 1,'first');
    iMn = maxgRw-chgImn+1;
    mn = img_maxgCol(iMn);
else
    % Find max
    finalmx = maxgRw-searchVal-1;
    mxVector = img_maxgCol(maxgRw:-1:finalmx + 1) - img_maxgCol(maxgRw-1:-1:finalmx);
    chgImx = find(mxVector > 0, 1,'first');
    iMx = maxgRw-chgImx+1;
    mx = img_maxgCol(iMx);
   
    % Find min
    finalmn = maxgRw + searchVal+1;  
    mnVector = img_maxgCol(maxgRw+intV:finalmn)-img_maxgCol(maxgRw+intV-1:finalmn-1);
    chgImn = find(mnVector > 0, 1,'first');
    iMn = maxgRw+chgImn-1+intV-1;
    mn = img_maxgCol(iMn);
end



end