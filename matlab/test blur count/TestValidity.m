function [valid, domain] = TestValidity(img_line)
% Test if start and stop of blurred pixels can be found on img_line (or 
% this slice of image) 

valid = true;
domain = (1:length(img_line)).';

if max(img_line)-min(img_line) < 100
    valid = false;
end

intv = 50;
% Find change in pixel value over long intervals
testR = 1:intv:round(length(img_line),-2);
testM = zeros(length(testR),3);
testM(:,1) = testR;
testM(:,2) = img_line(testM(1:end,1));
testM(2:end,3) = testM(2:end,2) - testM(1:end-1,2);
[maxT, indT] = max(abs(testM(:,3)));
if maxT < 30
    valid = false;
else
    domain = (testM(indT,1)-(intv+floor(intv/2)):testM(indT,1)+(intv)).';
end

