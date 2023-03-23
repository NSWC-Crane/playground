% This program calcuates the blur count in the checkerboard images.
% In each image, it finds the columns with the maximum gradients
% (indicating the transition from black to white or white to black).
% Then it finds the max and min values associated with the gradients.
% Finally, it finds the blur count between the max and min values and
% averages the values among the highest gradients in the image.

clear
clc

rangeV = 500:25:900;
zoomV = 5500;
gradL = 100; % Required number between pixel values of max and min.

rangeV = 10:1:19;
zoomV = 0;
gradL = 80; % Required number between pixel values of max and min.

% Options
textureMatt = 1; % textureMatt:  matt is 1, glossy is 0
if textureMatt == 1
    texstr = 'Matt';
else
    texstr = 'Glossy';
end
plotcurves = 0; % Plots the blur count curves gradient selected in each image
saveHeatmap = 1;

%% Setup data directories
platform = string(getenv("PLATFORM"));
if(platform == "Laptop")
    data_root = "D:\data\dfd\20230317\processed\";
elseif (platform == "LaptopN")
    data_root = "C:\Projects\data\dfd\20230317\";
else   
    data_root = "C:\Data\JSSAP\";
end

dirOut = data_root + "Results_LrgCB\";

%% Loop through images by zoom and range
for zoom = zoomV
    %% Set up a table to collect results by zoom value
    numRows = 20000;
    col_label = ["ImgPath","Filename","Range","Zoom","Focus","ImgHt","ImgWd","BlurCount"];
    vartypes = {'string','string','uint16','uint16','uint16','uint16','uint16','double'};
    Tb = table('Size', [numRows, length(col_label)], 'VariableTypes', vartypes);
    Tb.Properties.VariableNames = col_label.';
    indT = 1;
    for rng = rangeV
        % Get filename based on zoom and range values
        img_path = data_root + num2str(zoom, 'z%04d') + "/" + num2str(rng, '%04d');
        image_ext = '*.png';
        listing = dir(strcat(img_path, '/', image_ext));
        fprintf('IMAGE PATH: %s\n', img_path);
        
        for idx=1:numel(listing)
            % Find focus
            focus = FindFocus(listing(idx).name);
                      
            %% Image pre-processing
            img_file = fullfile(img_path, '/', listing(idx).name);
            img = double(imread(img_file));
            img = img(:,:,2); % Green channel
            [~,img_w1] = size(img);
%             if textureMatt == 1
%                 endpix = floor(img_w1/2)-20;
%                 img = img(:,1:endpix);
%                 [img_h,img_w] = size(img);
%             else %texture is 'glossy'
%                 srtpix = ceil(img_w1/2)+20;
%                 img = img(:,srtpix:end);
%                 [img_h,img_w] = size(img);
%             end
            
            srtpix = floor(img_w1/2)-100;
            endpix = ceil(img_w1/2)+100;
            img = img(:,srtpix:endpix);
            [img_h,img_w] = size(img);
                        
            %% Find max gradent in pixel values by column of the image
            % Determine the columns with the highest gradients over intV
            % pixels.
            % The maximum gradient represents the change from
            % black/white or white/black in checkerboard image.  The
            % maximum gradient may indicate the least turbulence, so that the
            % blur in this region is due only to focus and range.
            highestNum = 3; % Number of rows that will be averaged for blur number
            intV = 100; % Interval size in pixels that will be evaluated for gradient
            indB = 1; % Index for blurpix matrix
            maxgradV = FindMaxGradient(img, intV, highestNum, "row");

            % Test gradient sizes.  If less than gradL, go to next image.
            if maxgradV(:,3) < gradL 
                continue;
            else
                blurPix = [];
                % For each gradient selected in image, find the blur count
                for i = 1:size(maxgradV, 1) 
                    maxgRw = maxgradV(i,1);  % Row where max gradient starts
                    maxgCol = maxgradV(i,2); % Column where max gradient starts
                    maxgrad = maxgradV(i,3); % Gradient
                    posSlope = maxgradV(i,4);% Slope: 1 positive, 0 negative
                    
                    % Looking at rows, not columns, in these images
                    %img_maxgCol = img(:,maxgCol); 
                    img_maxgRow = img(maxgRw,:);
    
                    searchVal = intV + 70;
                    if maxgCol + searchVal + 1 > img_w
                        continue;
                    elseif maxgCol-searchVal-1 < 1
                        continue
                    else
                        [iMx,mx,iMn,mn] = FindMaxMinRow(maxgCol, intV, img_maxgRow, searchVal, posSlope);
                    end
                
                    % Find blur count
                    [blurPix(indB), indMn, indMx] = CalculateBlur(img_maxgRow, iMn, iMx, mn, mx, posSlope);
                    indB=indB+1;
                    
                    %% Plot the column in the image with the gradient max, min,
                    % and blur count
                    if plotcurves == 1
                        figure()
                        plot(1:img_w, img_maxgRow, '-b')
                        hold on
                        plot(maxgRw, img_maxgRow(maxgRw), 'r*')
                        hold on
                        plot(iMn, mn, 'g*')
                        hold on
                        plot(iMx, mx, 'm*')
                        hold on
                        if posSlope == 1 % Slope of gradient is positive
                            plot([iMn+indMn-1,iMn+indMn-1],[1,img_w],'g')
                            hold on
                            plot([iMn+indMx-1,iMn+indMx-1],[1,img_w],'m')
                        else % Slope of gradient is negative
                            plot([iMn-indMn+1,iMn-indMn+1],[1,img_w],'g')
                            hold on
                            plot([iMn-indMx+1,iMn-indMx+1], [1,img_w],'m')
                        end
                        xlabel("Pixel Number for Column")
                        ylabel("Pixel Value")
                        grid on
                        xticks(0:10:img_w) 
                        xlim([1,img_w])
                        ylim([0,255])
                        
                        hold off
                        %pause(1)
                        close all
                    end
                end
            end
              
            % Calculate mean blur count of all gradients in image
            if mean(blurPix) > 0
                Tb(indT,["ImgPath","Filename","Range","Zoom","Focus","ImgHt","ImgWd"]) = ...
                    {img_path, listing(idx).name, rng, zoom, focus, img_h, img_w };
                Tb.BlurCount(indT) = mean(blurPix);
                Tb.MaxGrad(indT) = mean(maxgradV(:,3));
                indT = indT+1;
            end                     
        end
    end
    
    %% Create Heatmap by range and focus with color indicating blur count
    % Round focus values to closest multiple of 5.
    Tb.Focus5 = 5*floor(Tb.Focus/5);
    % Remove unused rows in table
    Tb = Tb(Tb.Range > 0,:);
    if saveHeatmap == 1
        writetable(Tb, dirOut + "tb" + texstr + "_g" + num2str(gradL)+ "_" + num2str(zoom) + ".csv");
    end
    
    % Create heatmap table since there are many images with same focus
    focusVals = unique(Tb.Focus5);
    startFocus = min(focusVals);
    endFocus = max(focusVals);
    focusI = startFocus:5:endFocus;
    
    %rowsH = (length(rangeV)) * length(focusI);
    rowsH = 20000;
    vartypesH = {'uint16', 'uint16', 'uint16', 'double'};
    TbHeatm = table('Size', [rowsH,4], 'VariableTypes', vartypesH);
    TbHeatm.Properties.VariableNames = ["Range", "Zoom", "Focus5", "BlurPix"];
    indH = 1;
    for rng = rangeV
        for fc = 1:length(focusI)
            indF = find(Tb.Range == rng & Tb.Zoom == zoom & Tb.Focus5 == focusI(fc));
            bc = mean(Tb.BlurCount(indF));
            if bc>=1
                bc = cast(bc,"uint8");
            end
            TbHeatm(indH,:) = {rng, zoom, focusI(fc), bc};
            indH = indH + 1;
        end
    end
    % Remove unused rows in table
    TbHeatm = TbHeatm(TbHeatm.Range > 0,:);
    if saveHeatmap == 1
        writetable(TbHeatm, dirOut + "tbHeatmap" + texstr + "_g" + num2str(gradL)+ "_" + num2str(zoom) + ".csv");
    end
    
    fig = figure();
    h = heatmap(TbHeatm, 'Range', 'Focus5', 'ColorVariable', 'BlurPix','Colormap', parula);
    xlabel("Range")
    ylabel("Focus Interval")
    title("Zoom " + num2str(zoom) + ": Blurred Pixels per Range and Focus Interval")
    set(gcf,'position',([100,100,1100,1500]),'color','w')
    if saveHeatmap == 1
        fileOut = dirOut + "hm" + texstr + "_g" + num2str(gradL)+ "_" + num2str(zoom) + ".png";
%         exportgraphics(h,fileOut,'Resolution',300)
        fileFig = dirOut + "hm" + texstr + "_g" + num2str(gradL)+ "_" + num2str(zoom) + ".fig";
        savefig(fig, fileFig)
    end

%     %% Create output matrix .mat from TbHeatm
% 
%     fileMat = data_root + "results_CB_focusRange\Nsample_blur_radius_data5_z" + num2str(zoom) + ".mat";
%     for rInd = 1:length(rangeV)
%         for fInd = 1:length(focusI)
%             indB = find(TbHeatm.Range == rangeV(rInd) & TbHeatm.Focus5 == focusI(fInd));
%             newMat(fInd, rInd) = TbHeatm.BlurPix(indB);
%         end
%     end
%     range = rangeV;
%     coc_map = newMat;
%     focus = focusI;
%     
%     save(fileMat, 'coc_map', 'focus','range','zoom')
    
end


