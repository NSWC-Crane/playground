function [locPix] = SearchMinMax(img_line, deltaVal, indx, stype)
%Fine tune max and min values

switch(stype)
    case 'max'
        % Scans forward along x val to find values within range rng
        testVal = img_line(indx)-deltaVal;
        locPix = find(img_line > testVal,1,'last');
    case 'min'
        testVal = img_line(indx)+deltaVal;
        locPix = find(img_line < testVal,1,'first');
end

end