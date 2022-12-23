
% This script creates a table for all ranges without creating figures of
% the image line. At the end, a heatmap will be plotted to show optimal
% focus for each rangeV.
% Images:  20220908_101308 (Only using Zoom 2000)
% For each rangeV:
% Define img_line
% Fit curve
% Find local max and min on fitted curve
% Find pixels within intensity value called deltaVal, moving toward center
% Note location/index
% Find number of blurred pixels between new max and min
% Handle the case when no max and/or no mins

% Functions:
% 1.  Fit curve: return curve, maxima, and minima
% 2.  Find point near inflection point of curve.
% 3.  Calculate blurred pixels after fine tuning indices for max and min 
%     values (within deltaVal) - need inflection point to do this.
% 4.  Fine tune max and min values
% 5.  Find focus value from filename
% 6.  Test validity of finding blurred pixels on img_line

%%
format long g
format compact
clc
close all
clearvars

%% Set constants
slice_rows = [20, 50, 80, 110, 140];
rangeV = 500:10:750;
zoom = 2000; % Always use 2000
numFilesPerDir = 301;

%% Set up a table to collect results 
col_label = ["ImgPath","Filename","Range","Zoom","Focus","ImgHt","ImgWd"];
vartypes = {'string','string','uint16','uint16','uint16','uint16','uint16'};
colS1 = [];
colS2 = [];
for slr = slice_rows
    colS1 = [colS1, "StartR"+ num2str(slr)];
    colS2 = [colS2, "NumBlurR" + num2str(slr)];
    vartypes = horzcat(vartypes,{'uint16','uint16'});
end
col_label = [col_label, colS1, colS2, "BlurMean"];
vartypes = horzcat(vartypes,{'double'});
Tb = table('Size', [numFilesPerDir* length(rangeV)+100, length(col_label)], 'VariableTypes', vartypes);
Tb.Properties.VariableNames = col_label.';
indT = 1;

%% Iterate through ranges
for rng = rangeV

    %% Get the directory info of the images
    img_path = "C:\Data\JSSAP\20220908_101308\20220908_101308\0" + num2str(rng) + "\z2000";
    image_ext = '*.png';
    listing = dir(strcat(img_path, '/', image_ext));
    fprintf('IMAGE PATH: %s\n', img_path);
    
    %% Iterate through images
    for idx=1:numel(listing) 
        %fprintf('Image Filename: %s\n', listing(idx).name);
        % Load in an image and get its size
        img_file = fullfile(img_path, '/', listing(idx).name);
        img = imread(img_file);
        [img_h, img_w, img_c] = size(img);
        % Test for number of channels. Create gray image if more than 1.
        if(img_c > 1)
            img = double(rgb2gray(img));
        else
            img = double(img);
        end
        % Add data to Table
        focus = FindFocus(listing(idx).name);
        Tb(indT,["ImgPath","Filename","Range","Zoom","Focus","ImgHt","ImgWd"]) = ...
            {img_path, listing(idx).name, rng, zoom, focus, img_h, img_w };
    
        totalblurP = 0;
        numRows = 0;
        for i = 1:length(slice_rows)
            % Define domain of img_line and fitted curve
            x = (1:img_w).';
            % Define line image along slice_row(i)
            img_line = img(slice_rows(i), :);
            % Test validity of slice
            [validT, d] = TestValidity(img_line);
    
            if validT == true
                % Fit Curve y to img_line and get local max and mins on fitted curve
                [y, ymax, ymin] = FitCurveF(x, img_line, d);
                    
                % Find inflection point of y in applicable domain d
                [idxIP, validIP, ymax, ymin] = FindInflectionPt(x, y, ymax, ymin, d);
        
                % Calculate number of blurred pixels
                [startPix, numBlurPix] = CalcBlurredPix(ymin, ymax, idxIP, img_line);
        
                % Add results to table
                Tb(indT,"StartR" + num2str(slice_rows(i))) = {startPix};
                Tb(indT,"NumBlurR" + num2str(slice_rows(i))) = {numBlurPix};
                totalblurP = totalblurP + numBlurPix;
                numRows = numRows + 1;
            end
        end
        
        % Calculate Mean
        if numRows > 0
            avgBlur = totalblurP/numRows;
        else
            avgBlur = img_w;  % Indicates that all slice_rows were invalid
        end
        
        Tb(indT,"BlurMean") = {avgBlur};
        indT = indT + 1;  
    end
end
% Save Tb table
writetable(Tb, "C:\Data\JSSAP\20220908_101308\TbAll.csv");

%% Create heatmap
% Group data by focus intervals

focusI = 46200:50:49200;
rangeH = 500:10:750;
rowsH = (length(rangeH)-1) * length(focusI);
vartypesH = {'uint16', 'uint16', 'double'};
TbHeatm = table('Size', [rowsH,3], 'VariableTypes', vartypesH);
TbHeatm.Properties.VariableNames = ["Range", "FocusInt", "BlurPix"];
indH = 1;
for rng = rangeH
    for fc = 1:length(focusI)-1
        indF = find(Tb.Range == rng & Tb.Focus >= focusI(fc) & Tb.Focus < focusI(fc+1) );
        TbHeatm(indH,:) = {rng, focusI(fc), mean(Tb.BlurMean(indF))};
        indH = indH + 1;
    end
end
writetable(TbHeatm, "C:\Data\JSSAP\20220908_101308\TbHeatmap500s.csv");

figure(5)
h = heatmap(TbHeatm, 'Range', 'FocusInt', 'ColorVariable', 'BlurPix','Colormap', parula);
xlabel("Range")
ylabel("Focus Interval")
title("Mean Blurred Pixels per Range and Focus Interval")
set(gcf,'position',([100,100,1000,1100]),'color','w')
fileOut = "C:\Data\JSSAP\20220908_101308\plots\heatmapAll.png";
exportgraphics(h,fileOut,'Resolution',300)

