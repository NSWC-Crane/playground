function [iMx,mx,iMn,mn] = FindMaxMinCol(maxgCol, intV, img_maxgRow)
% intV: number of pixels evaluated for gradient
% img_maxgRow: row with max gradient
% maxgCol: column of start of max gradient (then it proceeds up or
% down, depending on direction of slope).

% Return
% iMx, mx: index of maximum, pixel value of maximum
% iMn, mn: index of minimum, pixel value of minimum

% Find max and min on down slope.
% Subtract 10 from maxgCol and find average of all previous pixels from
% 1:maxgCol-10
% Scan pixels 

%% Find average of pixel intensity prior to maxgCol
finalCol = length(img_maxgRow);
maxAvg = mean(img_maxgRow(1:maxgCol-10));
minAvg = mean(img_maxgRow(finalCol-30:finalCol));

%% Max
% Find last occurence of pixel intensity that is < maxAvg - 10 (intensty);
% Set this column as iMx with intensity of mx
iMx = find(img_maxgRow > maxAvg - 10, 1,"last" );
mx = img_maxgRow(1,iMx);

%% Min
% First point where gradient changes from negative to positive after the
% interval of maxgCol + intV
% At the minimum, there will be noise and the curve will rise and fall
% slightly

iMn = find(img_maxgRow < minAvg + 3, 1,"first" );
mn = img_maxgRow(1,iMn);

% finalmn = length(img_maxgRow);
% mnVector = img_maxgRow(1,maxgCol+intV:finalmn-1)-img_maxgRow(1,maxgCol+intV-1:finalmn-2);
% chgImn = find(mnVector < 0, 1,'first');
% iMn = maxgCol + intV + chgImn -1;
% mn = img_maxgRow(1,iMn);
% %     % Test min: check next 4 pixels (in case a hump in slope)
%     if (iMn + 1 <= img_w) && (iMn + testPix <= img_w)
%         [testmn, itestMn] = min(img_maxgRow(1,iMn+1:iMn+testPix));
%         if img_maxgRow(1,itestMn + iMn) < img_maxgRow(1,iMn)
%             iMn = itestMn + iMn;
%             mn = testmn;
%         end
%     end

end

% Scan after maxgCol and determine when the value starts to decrease/increase.
% For min, search at and before maxgCol and determine when the value
% starts to increase/decrease.



