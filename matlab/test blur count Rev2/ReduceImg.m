function [img_reduced, startX] = ReduceImg(rangeV, zoom, img)
% Reduces image size based on range to ease processing.
% Returns reduced image and the pixel number of the first x-axis pixel in
% reduced image.


switch(zoom)

    case 2000
        length = 80;
        switch(rangeV)
            case {500,510,520,530,540,550,560,570,580,590,600}
                startX = 220;
        %         img_reduced = img(1:350,163:512);
                img_reduced = img(1:250,startX:(startX+length));
            case {610,620,630,640,650,660,670,680,690,700}
                startX = 205;
        %         img_reduced = img(1:350,133:482);
                img_reduced = img(1:250,startX:(startX+length));
            case {710,720,730,740,750}
                startX = 186;
        %         img_reduced = img(1:350,123:472);
                img_reduced = img(1:250,startX:(startX+length));
        end
        
    case {4500}
        
        length = 50;
        switch(rangeV)
            case {500,510,520,530,540,550,560,570,580,590,600}
                startX = 30;
        %         img_reduced = img(1:350,163:512);
                img_reduced = img(1:250,startX:(startX+length));
            case {610,620,630,640,650,660,670,680,690,700}
                startX = 20;
        %         img_reduced = img(1:350,133:482);
                img_reduced = img(1:250,startX:(startX+length));
            case {710,720,730,740,750}
                startX = 20;
        %         img_reduced = img(1:350,123:472);
                img_reduced = img(1:250,startX:(startX+length));
        end
    
    case {5000}
        
        length = 50;
        switch(rangeV)
            case {500,510,520,530,540,550,560,570,580,590,600}
                startX = 30;
        %         img_reduced = img(1:350,163:512);
                img_reduced = img(1:250,startX:(startX+length));
            case {610,620,630,640,650,660,670,680,690,700}
                startX = 30;
        %         img_reduced = img(1:350,133:482);
                img_reduced = img(1:250,startX:(startX+length));
            case {710,720,730,740,750}
                startX = 30;
        %         img_reduced = img(1:350,123:472);
                img_reduced = img(1:250,startX:(startX+length));
        end
    end
end