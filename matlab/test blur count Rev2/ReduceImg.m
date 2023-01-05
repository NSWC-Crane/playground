function [img_reduced] = ReduceImg(rangeV,img)
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here

switch(rangeV)
%     case 500 
%         img_reduced = img(1:250,263:512);
    case {500,510,520,530,540,550,560,570,580,590,600}
        img_reduced = img(1:350,163:512);
    case {610,620,630,640,650,660,670,680,690,700}
        img_reduced = img(1:350,133:482);
    case {710,720,730,740,750}
        img_reduced = img(1:350,113:472);
end


end