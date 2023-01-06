function [img_reduced, startX] = ReduceImg(rangeV,img)
% Reduces image size based on range to ease processing.
% Returns reduced image and the pixel number of the first x-axis pixel in
% reduced image.

switch(rangeV)
    case {500,510,520,530,540,550,560,570,580,590,600}
        img_reduced = img(1:350,163:512);
        startX = 163;
    case {610,620,630,640,650,660,670,680,690,700}
        img_reduced = img(1:350,133:482);
        startX = 133;
    case {710,720,730,740,750}
        img_reduced = img(1:350,123:472);
        startX = 123;
end

end