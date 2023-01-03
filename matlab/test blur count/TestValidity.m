function [valid, domain] = TestValidity(img_line)
% Test if start and stop of blurred pixels can be found on img_line (or 
% this slice of image) 
% img_line: horizontal slice of pixels on image (vector)
% valid: determines if this img_line can be used to find section of blurred
% pixels
% domain: set of values on img_line to look for inflection point
% Validity Test:
% 1. Is the difference between the max and min intensity values at least
% 100?
% 2. Can the location of the inflection point be estimated?

valid = true;
domain = (1:length(img_line)).';

if max(img_line)-min(img_line) < 100
    valid = false;
end

intv = 50;
% Find change in pixel value over long intervals
testR = 1:intv:round(length(img_line),-2);
% Create matrix testM to find location of changes in pixel values and
% determine the maximum differences over small intervals, intv
testM = zeros(length(testR),3);
testM(:,1) = testR;
testM(:,2) = img_line(testM(1:end,1));
testM(2:end,3) = testM(2:end,2) - testM(1:end-1,2); % Change in step values
[maxT, indT] = max(abs(testM(:,3)));
if maxT < 30
    valid = false;
else
    domain = (testM(indT,1)-(intv+floor(intv/2)):testM(indT,1)+(intv)).';
end

