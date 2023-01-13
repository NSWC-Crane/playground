
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
%close all
clearvars

%% Set constants
slice_rows = [46,48,50];
rangeV = 510:10:750;
zoomV = 3000;
numFilesPerDir = 301;

%% Setup data directories
platform = string(getenv("PLATFORM"));
if(platform == "Laptop")
    data_root = "D:\data\turbulence\";
elseif (platform == "LaptopN")
    data_root = "C:\Projects\data\turbulence\";
else   
    data_root = "C:\Data\JSSAP\20220908_101308\";
end

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
Tb = table('Size', [numFilesPerDir*length(rangeV)*length(zoomV), length(col_label)], 'VariableTypes', vartypes);
Tb.Properties.VariableNames = col_label.';
indT = 1;

%% Iterate through zoom values
for zoom = zoomV

    %% Iterate through ranges
    for rng = rangeV
    
        %% Get the directory info of the images
        img_path = data_root + "20220908_101308\0" + num2str(rng) + "\z" + num2str(zoom);
        image_ext = '*.png';
        listing = dir(strcat(img_path, '/', image_ext));
        fprintf('IMAGE PATH: %s\n', img_path);
        
        %% Iterate through images
        for idx=1:numel(listing)
            %fprintf('Image Filename: %s\n', listing(idx).name);
            % Load in an image and get its size
            img_file = fullfile(img_path, '/', listing(idx).name);
            img = double(imread(img_file));
            
            img = fliplr(imrotate(img, 90));   
            
            [img_h, img_w, img_c] = size(img);
            % Test for number of channels. Create gray image if more than 1.
            if(img_c > 1)
                img = img(:,:,2);
            end
            % Add data to Table
            focus = FindFocus(listing(idx).name);
            Tb(indT,["ImgPath","Filename","Range","Zoom","Focus","ImgHt","ImgWd"]) = ...
                {img_path, listing(idx).name, rng, zoom, focus, img_h, img_w };
        
            [img,startX] = ReduceImg(img, slice_rows);
            [~, img_wR] = size(img);
        
            totalblurP = 0;
            numRows = 0;
            % Iterate through slice_rows
            for i = 1:length(slice_rows)    
                        
                % Define line image along slice_row(i)
                img_line = img(slice_rows(i), :);
                                        
                intv = 6;
                [~,~, numBlurPix,startBlPix] = CalculateBlurCount(img_line, intv);
            
                % Add results to table
                Tb(indT,"StartR" + num2str(slice_rows(i))) = {startX+startBlPix-1};
                Tb(indT,"NumBlurR" + num2str(slice_rows(i))) = {numBlurPix};
                totalblurP = totalblurP + numBlurPix;
                numRows = numRows + 1;
        
            end
            
            % Calculate Mean
            if numRows > 0
                avgBlur = floor(totalblurP/numRows);
            else
                avgBlur = img_w;  % Indicates that all slice_rows were invalid
            end
            
            Tb(indT,"BlurMean") = {avgBlur};
            indT = indT + 1;  
         end
    end
end
% %% Save Tb table
filename = data_root + "Rework3\TbZ" + num2str(zoom) +  ".csv";
writetable(Tb,filename);

%% Create heatmap
% Group data by focus intervals
% startFocus = 46200;
% endFocus = 49200;
% intvF = 10;

intvF = 10;
startFocus = 47500;
endFocus = 48350;
zoom = zoomV;

focusI = startFocus:intvF:endFocus;
rowsH = (length(rangeV)-1) * length(focusI);
vartypesH = {'uint16', 'uint16', 'uint16', 'double'};
TbHeatm = table('Size', [rowsH,4], 'VariableTypes', vartypesH);
TbHeatm.Properties.VariableNames = ["Range", "Zoom", "FocusItv", "BlurPix"];
indH = 1;
for rng = rangeV
    for fc = 1:length(focusI)-1 % One less interval than number of focus points
        %indF = find(Tb.Range == rng & Tb.focusRnd10 == focusI(fc)); % & Tb.focusRnd10 < focusI(fc+1) );
        indF = find(Tb.Range == rng & Tb.Zoom == zoom & Tb.Focus >= focusI(fc) & Tb.Focus < focusI(fc+1) );
        TbHeatm(indH,:) = {rng, zoom, focusI(fc), mean(Tb.BlurMean(indF))};
        %TbHeatm(indH,:) = {rng, focusI(fc), mean(Tb.BlurMean(indF))};
        indH = indH + 1;
    end
end
writetable(TbHeatm, data_root + "Rework3\TbHeatmapZ" + num2str(zoom) + ".csv");

fig = figure();
h = heatmap(TbHeatm, 'Range', 'FocusItv', 'ColorVariable', 'BlurPix','Colormap', parula);
xlabel("Range")
ylabel("Focus Interval")
title("Zoom " + num2str(zoom) + ": Blurred Pixels per Range and Focus Interval")
set(gcf,'position',([100,100,1100,1500]),'color','w')
fileOut = data_root + "Rework3\Heatmap" + num2str(zoom) + ".png";
exportgraphics(h,fileOut,'Resolution',300)
fileFig = data_root + "Rework3\Heatmap" + num2str(zoom) + ".fig";
savefig(fig, fileFig)

% %% Create output matrix
% fileMat = data_root + "Rework3\Nsample_blur_radius_data2_z" + num2str(zoom) + ".mat";
% for rInd = 1:length(rangeV)
%     for fInd = 1:length(focusI)-1
%         indB = find(TbHeatm.Range == rangeV(rInd) & TbHeatm.FocusItv == focusI(fInd));
%         newMat(fInd, rInd) = TbHeatm.BlurPix(indB);
%     end
% end
% range = rangeV;
% coc_map = newMat;
% focus = focusI(1:end-1);
% 
% save(fileMat, 'coc_map', 'focus','range','zoom')
