
% This script is only for testing.  The final file will create a table for
% all ranges without any figures of the image line. At the end, a heatmap
% will be plotted.
% Images:  20220908_101308 (Only using Zoom 2000)
% Define img_lineMean
% Fit curve
% Find local max and min on fitted curve
% Find pixels within intensity value called deltaVal, moving toward center
% Note location/index
% Find number of blurred pixels between new max and min
% Handle the case when no max and/or no mins

% Note:  Each range value will require a different slice row - the slice
% row affects the number of blurred pixels so taking 5 slices -
% report slice row and number of blurred pixels

% Plot 1
% Plot image
% Plot line of selected row where img_line is created

% Plot 2
% Plot img_line
% Plot vertical lines at x-axis indices where selected max and min occur
% Plot horizontal lines at pixel values for max and min

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
rng = 750;  % For this test file, user must enter the range
zoom = 2000; % Always use 2000

%% Get the directory for the images
img_path = uigetdir(startpath, 'Select Folder with Images');
%img_path = "C:\Data\JSSAP\20220908_101308\20220908_101308\0750\z2000";
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
vartypes = horzcat(vartypes,{'double'});
Tb = table('Size', [length(listing), length(col_label)], 'VariableTypes', vartypes);
Tb.Properties.VariableNames = col_label.';
indT = 1;

%% Iterate through images
for idx=111:113 %:numel(listing) 
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

    % Plot image and selected slice_row 
    figure(1)
    set(gcf,'position',([100,100,1200,500]),'color','w')
    subplot(1,2,1);
    image(uint8(img));
    colormap(jet(256));
    hold on
    
    totalblurP = 0;
    numRows = 0;
    for i = 1:length(slice_rows)
        % Define domain of img_line and fitted curve
        x = (1:img_w).';
        % Plot selected slice_row(i) line on image
        figure(1)
        subplot(1,2,1);
        plot([1,img_w],[slice_rows(i),slice_rows(i)], '-k');
        hold on

        % Define line image along slice_row(i)
        img_line = img(slice_rows(i), :);

        % Plot curve
        figure(1)
        subplot(1,2,2);
        plot(x, img_line,'k')
        hold on
        legendL = "selected image line";

        % Test validity of slice
        [validT, d] = TestValidity(img_line);

        if validT == true
            
            % Fit Curve y to img_line and get local max and mins on fitted curve
            [y, ymax, ymin] = FitCurveF(x, img_line, d);
    
            % Plot curve
            figure(1)
            subplot(1,2,2);
            plot(x, y,'m')
            hold on
            legendL = [legendL; "fitted curve"];
                
            % Find inflection point of y in applicable domain d
            [idxIP, validIP, ymax, ymin] = FindInflectionPt(x, y, ymax, ymin, d);
            if validIP == true
                % Plot inflection point
                plot(idxIP, img_line(idxIP), 'xg','MarkerSize', 10)
                legendL = [legendL;"inflection point"];
                hold on
            end
            
            % Plot maxs and mins
            if sum(ymax) > 0
                plot(x(ymax),y(ymax),'r*','MarkerSize', 10)
                legendL = [legendL;"maxima"];
                hold on 
            end
            if sum(ymin) > 0
                plot(x(ymin),y(ymin), 'b*','MarkerSize', 10)
                legendL = [legendL;"minima"];
                hold on
            end
    
            % Calculate number of blurred pixels
            [startPix, numBlurPix] = CalcBlurredPix(ymin, ymax, idxIP, img_line);
            
            % Plot locations of blurred pixels 
            stem(startPix,200,'filled','r')
            stem(startPix+numBlurPix,200,'filled','c')
            plot([1,512],[img_line(startPix),img_line(startPix)],'r')
            plot([1,512],[img_line(startPix+numBlurPix),img_line(startPix+numBlurPix)],'c')
            hold on
    
            % Add results to table
            Tb(indT,"StartR" + num2str(slice_rows(i))) = {startPix};
            Tb(indT,"NumBlurR" + num2str(slice_rows(i))) = {numBlurPix};
            totalblurP = totalblurP + numBlurPix;
            numRows = numRows + 1;
        end
        % Plot parameters
        title("Range " + num2str(rng) + " Zoom "+ num2str(zoom) + " Focus " + num2str(focus))
        xlabel("Pixel Number")
        ylabel("Pixel Value")
        ylim([0,255])
        xlim([1,img_w])
        grid on
        legend(legendL);
        hold off
        pause(1)
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

%% Save Tb table
%writetable(Tb, "C:\Data\JSSAP\20220908_101308\Tb" + num2str(rng) + ".csv");

% figure(2)
% plot(mean,focus)