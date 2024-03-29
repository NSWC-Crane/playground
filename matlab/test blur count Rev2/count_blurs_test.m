
% This script is only for testing.  The final file will create a table for
% all ranges without any figures of the image line. At the end, a heatmap
% will be plotted.
% Images:  20220908_101308 (Only using Zoom 2000)
% Define img_line
% Smooth curve
% Find local max and min on smooth curve
% Find pixels within intensity value called deltaVal, moving toward center
% Note location/index
% Find number of blurred pixels between new max and min

% Plot 1
% Plot image
% Plot line of selected row where img_line is created

% Plot 2
% Plot img_line
% Plot vertical lines at x-axis indices where selected max and min occur
% Plot horizontal lines at pixel values for max and min

%%
format long g
format compact
clc
close all
clearvars

%% Set constants
%slice_rows = [20, 50, 80, 110, 140];
slice_rows = [40,45,50];
rng = 550;  % For this test file, user must enter the range
zoom = 4500; 

%% Get the directory for the images
% Setup data directories
platform = string(getenv("PLATFORM"));
if(platform == "Laptop")
    data_root = "D:\data\turbulence\";
elseif (platform == "LaptopN")
    data_root = "C:\Projects\data\turbulence\";
else   
    data_root = "C:\Data\JSSAP\20220908_101308\";
end

img_path = data_root + "20220908_101308\0" + num2str(rng) + "\z" + num2str(zoom);
image_ext = '*.png';
listing = dir(strcat(img_path, '/', image_ext));

fprintf('IMAGE PATH: %s\n', img_path);

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
vartypes = horzcat(vartypes,{'uint16'});
Tb = table('Size', [length(listing), length(col_label)], 'VariableTypes', vartypes);
Tb.Properties.VariableNames = col_label.';
indT = 1;

%% Iterate through images
figure(1)
for idx=220:240%:numel(listing)
    fprintf('Image Filename: %s\n', listing(idx).name);
    % Load in an image and get its size
    img_file = fullfile(img_path, '/', listing(idx).name);
    img = imread(img_file);
    
    img = fliplr(imrotate(img, 90));   
    
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

    [img,startX] = ReduceImg(img, slice_rows);
    [~, img_wR] = size(img);

    % Plot image and selected slice_row 
    figure(1)
    set(gcf,'position',([100,100,1200,500]),'color','w')
    subplot(1,2,1);
    image(uint8(img));
    colormap(jet(256));
    %xticklabels([startX:50:startX+350-1])
    hold on
    
    totalblurP = 0;
    numRows = 0;
    % Iterate through slice_rows
    for i = 1:length(slice_rows)    
        % Plot selected slice_row(i) line on image
       
        subplot(1,2,1);
        plot([1,img_w],[slice_rows(i),slice_rows(i)], '-k');
        hold on

        % Define line image along slice_row(i)
        img_line = img(slice_rows(i), :);
        % Define domain of img_line

        x = (startX:startX+img_wR-1).';
        % Plot image line
        figure(1)
        subplot(1,2,2);
        plot(x, img_line,'k')
        hold on
        legendL = "selected image line";

        intv = 6;
        [xC,yC, numBlurPix,startBlPix] = CalculateBlurCount(img_line, intv);

        % Plot curve
        figure(1)
        subplot(1,2,2);
        
        xC = xC + startX-1;
        plot(xC.', yC,'m')
        hold on
        legendL = [legendL; "smoothed curve"];
        
        % Plot locations of blurred pixels 
        stem(startX + startBlPix-1,255,'filled','r')
        stem((startX+startBlPix+numBlurPix-2),255,'filled','c')
        plot([1,512],[img_line(startBlPix),img_line(startBlPix)],'r')
        plot([1,512],[img_line(startBlPix+numBlurPix-1),img_line(startBlPix+numBlurPix-1)],'c')
        hold on

        % Add results to table
        Tb(indT,"StartR" + num2str(slice_rows(i))) = {startX+startBlPix-1};
        Tb(indT,"NumBlurR" + num2str(slice_rows(i))) = {numBlurPix};
        totalblurP = totalblurP + numBlurPix;
        numRows = numRows + 1;

        % Plot parameters
        title("Range " + num2str(rng) + " Zoom "+ num2str(zoom) + " Focus " + num2str(focus))
        xlabel("Pixel Number")
        ylabel("Pixel Value")
        ylim([0,255])
        xlim([x(1),x(end)])
        grid on
        legend(legendL);
        hold off
        pause(1)
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

%% Save Tb table
%writetable(Tb, "C:\Data\JSSAP\20220908_101308\Tb" + num2str(rng) + ".csv");

