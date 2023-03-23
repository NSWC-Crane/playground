function [iMx,mx,iMn,mn] = FindMaxMinCol(maxgCol, img_maxgRow)
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
% Find last occurence of pixel intensity that is > maxAvg - 10 (intensty);
% Set this column as iMx with intensity of mx
iMx = find(img_maxgRow > maxAvg - 10, 1,"last" );
mx = img_maxgRow(1,iMx);

%% Min
% Find first occurence of pixel intensity that is < minAvg+3 (intensty);
% Set this column as iMn with intensity of mn
iMn = find(img_maxgRow < minAvg + 3, 1,"first" );
mn = img_maxgRow(1,iMn);

end




