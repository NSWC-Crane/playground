function [locPix] = SearchMinMax(img_line, y, deltaVal, indx, stype)
%Fine tunes max and min values
% img_line: horizontal slice of pixels on image (vector)
% deltaVal: to find end of max or beginning of min on img_line, this value
% is added to max or min value
% indx: location of max or min
% stype: max or min
% Returns:
% locPix: index of new max or min for use in identifying blurred pixels

switch(stype)
    case 'max'
        % Scans forward along x val to find values within range rng
        testVal = y(indx)-deltaVal;
        locPix = find(img_line > testVal,1,'last');
    case 'min'
        testVal = y(indx)+deltaVal;
        locPix = find(img_line < testVal,1,'first');
end

end