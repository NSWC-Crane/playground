
% This script creates a table for all ranges without creating figures of
% the image line. At the end, a heatmap will be plotted to show optimal
% focus for each rangeV.
% Images:  20220908_101308 (Only using Zoom 2000)
% For each rangeV:
% Define img_line
% Smooth curve
% Find local max and min on smoothed curve
% Find pixels within intensity value called deltaVal, moving toward center
% Note location/index
% Find number of blurred pixels between new max and min
% Handle the case when no max and/or no mins

%%
format long g
format compact
clc
close all
clearvars

%% Set constants
%slice_rows = [20, 50, 80, 110, 140];
slice_rows = [110];
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
        fprintf('Image Filename: %s\n', listing(idx).name);
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
    
        % Reduce size of image to find blur count
        [img,startX] = ReduceImg(rng,img);
        %img = img(1:350,113:472);
        [~, img_wR] = size(img);
          
        totalblurP = 0;
        numRows = 0;
        for i = 1:length(slice_rows)     
            % Define line image along slice_row(i)
            img_line = img(slice_rows(i), :);
            % Define domain of img_line
            x = (startX:startX+img_wR-1).';
            % Calculate pixel blur    
            intv = 16;
            [xC,yC, numBlurPix,startPix] = CalculateBlurCount(img_line, intv);           
            xC = xC + startX-1;
                
            % Add results to table
            Tb(indT,"StartR" + num2str(slice_rows(i))) = {startX+startPix-1};
            Tb(indT,"NumBlurR" + num2str(slice_rows(i))) = {numBlurPix};
            totalblurP = totalblurP + numBlurPix;
            numRows = numRows + 1;
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
%% Save Tb table
filename = "C:\Data\JSSAP\20220908_101308\Rework\NR100TbAll_1slice.csv";
writetable(Tb,filename);

%% Create heatmap
% Group data by focus intervals
% startFocus = 46200;
% endFocus = 49200;
% intvF = 10;

startFocus = 46500;
endFocus = 47340;
intvF = 20;

focusI = startFocus:intvF:endFocus;
rowsH = (length(rangeV)-1) * length(focusI);
vartypesH = {'uint16', 'uint16', 'double'};
TbHeatm = table('Size', [rowsH,3], 'VariableTypes', vartypesH);
TbHeatm.Properties.VariableNames = ["Range", "FocusItv", "BlurPix"];
indH = 1;
for rng = rangeV
    for fc = 1:length(focusI)-1 % One less interval than number of focus points
        %indF = find(Tb.Range == rng & Tb.focusRnd10 == focusI(fc)); % & Tb.focusRnd10 < focusI(fc+1) );
        indF = find(Tb.Range == rng & Tb.Focus >= focusI(fc) & Tb.Focus < focusI(fc+1) );
        TbHeatm(indH,:) = {rng, focusI(fc), mean(Tb.BlurMean(indF))};
        TbHeatm(indH,:) = {rng, focusI(fc), mean(Tb.BlurMean(indF))};
        indH = indH + 1;
    end
end
writetable(TbHeatm, "C:\Data\JSSAP\20220908_101308\Rework\NR100TbHeatmap10s_1slice.csv");

fig = figure(5);
h = heatmap(TbHeatm, 'Range', 'FocusItv', 'ColorVariable', 'BlurPix','Colormap', parula);
xlabel("Range")
ylabel("Focus Interval")
title("Mean Blurred Pixels per Range and Focus Interval")
set(gcf,'position',([100,100,1100,1500]),'color','w')
fileOut = "C:\Data\JSSAP\20220908_101308\Rework\NR100heatmap10s_1slice.png";
exportgraphics(h,fileOut,'Resolution',300)
fileFig = "C:\Data\JSSAP\20220908_101308\Rework\NR100heatmap10s_1slice.fig";
savefig(fig, fileFig)

%% Create output matrix
fileMat = "C:\Data\JSSAP\20220908_101308\Rework\Nsample_blur_radius_data2.mat";
for rInd = 1:length(rangeV)
    for fInd = 1:length(focusI)-1
        indB = find(TbHeatm.Range == rangeV(rInd) & TbHeatm.FocusItv == focusI(fInd));
        newMat(fInd, rInd) = TbHeatm.BlurPix(indB);
    end
end
range = rangeV;
coc_map = newMat;
zoom = [2000];
focus = focusI(1:end-1);

save(fileMat, 'coc_map', 'focus','range','zoom')
