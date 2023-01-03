function [startPix,numBlurPix] = CalcBlurredPix(ymin, ymax, idxIP, img_line)
% ymin: local minimum values found for y
% ymax: local maximum values found for y
% idxIP: the x value where the inflection point resides
% img_line: the line image selected to evaluate blurred pixels
% Returns:
% startPix: index where blur begins
% numBlurPix:  number of blurred pixels

% Calculate number of blurred pixels after fine tuning indices for max and
% min values (within deltaVal) 

deltaVal = 2;
% Find max pixel location
ymaxTest = ymax(1:idxIP,:);
countYMx = sum(ymaxTest);
switch(countYMx)
    case 0
        startPix = 1;
    case 1
        % Scan around ymax to find first val + 3
        ind = find(ymaxTest == 1);
        startPix = SearchMinMax(img_line, deltaVal, ind, 'max');
    otherwise
        % Find ymaxTest with highest index
        ind = find(ymaxTest == 1,1,'last');
        startPix = SearchMinMax(img_line, deltaVal, ind, 'max');
end

% Find min pixel location
yminTest = ymin(idxIP:end,:);
countYMn = sum(yminTest);
switch(countYMn)
    case 0
        stopPix = length(img_line);
    case 1
        % Scan around ymin to find first val + 3
        ind = find(yminTest == 1);
        stopPix = SearchMinMax(img_line, deltaVal, ind+idxIP-1, 'min');
    otherwise
        % Find ymaxTest with lowest index
        ind = find(yminTest == 1,1,'first');
        stopPix = SearchMinMax(img_line, deltaVal, ind+idxIP-1, 'min');
end

% Calculate number of blurred pixels
numBlurPix = stopPix - startPix;

end