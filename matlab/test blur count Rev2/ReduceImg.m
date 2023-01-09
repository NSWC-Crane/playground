function [img_reduced, startW] = ReduceImg(img, slice_rows)
% Returns reduced image and the pixel number of the first x-axis pixel in
% reduced image.

% Finds max and min of image that includes image lines of interest
% Includes pixAllow pixels before max and pixAllow pixels after max
% Use above to find length and startX
% No longer dependent on zoom,range of image

pixAllow = 10;

% Define height of image based on location of slices (doesn't really affect
% processing speed).
heightR = max(slice_rows) + 50;

% Create matrix of slices to find min and max values and locations
for i = 1:length(slice_rows)
    test_imgline(i,:) = img(slice_rows(i), :);
end
maxPixVal = max(test_imgline,[],'all');
minPixVal = min(test_imgline,[],'all');
[~, cMx] = find(test_imgline == maxPixVal, 1, "last");
[~, cMn] = find(test_imgline == minPixVal, 1, "first");

% Using location of max and mins, determine pixel number where reduced
% image starts and the width of the reduced image.
widthL = cMn - cMx + 2*(pixAllow);
startW = cMx - pixAllow;
if startW < 1
    startW = 1;
end

% Reduce image size
[~, widthI] = size(img);
if startW + widthL > widthI
    img_reduced = img(1:heightR,startW:end);
else
    img_reduced = img(1:heightR,startW:startW+widthL);
end

end
