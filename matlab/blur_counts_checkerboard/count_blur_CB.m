
clear
clc

rangeV = 600:50:1000;
%zoomV = [2000,2500,3000,3500,4000];
zoomV = 5000;
gradL = 110; % Required number between pixel values of max and min.

%% Setup data directories
platform = string(getenv("PLATFORM"));
if(platform == "Laptop")
    data_root = "D:\data\turbulence\";
elseif (platform == "LaptopN")
    data_root = "C:\Projects\data\turbulence\";
else   
    data_root = "C:\Data\JSSAP\";
end

%% Loop through images by zoom and range
for zoom = zoomV
    %% Set up a table to collect results by zoom value
    numFiles = 20000;
    col_label = ["ImgPath","Filename","Range","Zoom","Focus","ImgHt","ImgWd","BlurCount"];
    vartypes = {'string','string','uint16','uint16','uint16','uint16','uint16','double'};
    Tb = table('Size', [numFiles, length(col_label)], 'VariableTypes', vartypes);
    Tb.Properties.VariableNames = col_label.';
    indT = 1;
    for rng = rangeV
        % Get filename based on zoom and range values
        img_path = data_root + "z" + num2str(zoom) + "\" + num2str(rng);
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
            [img_h,img_w] = size(img);
            
            %% Find max gradent in pixel values by column of the image
            % Determine the columns with the highest gradients over intV
            % pixels.
            % The maximum gradient represents the change from
            % black/white or white/black in checkerboard image.  The
            % maximum gradient may indicate the least turbulence, so that the
            % blur in this region is due only to focus and range.
            highestNum = 3; % Indicates number of columns that will be averaged for blur number
            intV = 7; % Consider 2 image sizes 256x256 and 200x200
            indB = 1;
            maxgradV = FindMaxGradient(img, intV, highestNum);

            % Test gradient size.  If less than gradL, go to next image.
            if maxgradV(:,3) < gradL 
                continue;
            else
                blurPix = [];
                for i = 1:height(maxgradV)
                    maxgCol = maxgradV(i,1);
                    maxgRw = maxgradV(i,2);
                    maxgrad = maxgradV(i,3);
                    direction = maxgradV(i,4);
    
                    img_maxgCol = img(:,maxgCol); 
    
                    searchVal = 20;
                    if maxgRw + searchVal + 1 > img_h
                        continue;
                    elseif maxgRw-searchVal-1 < 1
                        continue
                    else
                        [iMx,mx,iMn,mn] = FindMaxMin(maxgRw, intV, img_maxgCol, searchVal, direction);
                    end
                
                    % Find blur count
                    [blurPix(indB), indMn, indMx] = CalculateBlur(img_maxgCol, iMn, iMx, mn, mx, direction);
                    indB=indB+1;
    
%                     figure()
%                     plot(1:img_h, img_maxgCol, '-b')
%                     hold on
%                     plot(maxgRw, img_maxgCol(maxgRw), 'r*')
%                     hold on
%                     plot(iMn, mn, 'g*')
%                     hold on
%                     plot(iMx, mx, 'm*')
%                     hold on
%                     if direction == 1
%                         plot([1,img_h], [img_maxgCol(iMn+indMn-1),img_maxgCol(iMn+indMn-1)],'g')
%                         hold on
%                         plot([1,img_h], [img_maxgCol(iMn+indMx-1),img_maxgCol(iMn+indMx-1)],'m')
%                     else
%                         plot([1,img_h], [img_maxgCol(iMn-indMn+1),img_maxgCol(iMn-indMn+1)],'g')
%                         hold on
%                         plot([1,img_h], [img_maxgCol(iMn-indMx+1),img_maxgCol(iMn-indMx+1)],'m')
%                     end
%                     xlabel("Pixel Number for Column")
%                     ylabel("Pixel Value")
%                     grid on
%                     xticks(0:10:img_h) 
%                     xlim([1,img_h])
%                     ylim([0,255])
%                     
%                     hold off
%                     close all
                end
            end
               
            if mean(blurPix) > 0
                Tb(indT,["ImgPath","Filename","Range","Zoom","Focus","ImgHt","ImgWd"]) = ...
                    {img_path, listing(idx).name, rng, zoom, focus, img_h, img_w };
                Tb.BlurCount(indT) = mean(blurPix);
                Tb.MaxGrad(indT) = mean(maxgradV(:,3));
                indT = indT+1;
            end
        end            
    end

    %% Create Heatmap
    % Round focus values to closest multiple of 5.
    Tb.Focus5 = 5*floor(Tb.Focus/5);
    % Remove unused rows in table
    Tb = Tb(Tb.Range > 0,:);
    writetable(Tb, data_root + "Results_new_CB\tb110_" + num2str(zoom) + ".csv");
    
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
            TbHeatm(indH,:) = {rng, zoom, focusI(fc), mean(Tb.BlurCount(indF))};
            indH = indH + 1;
        end
    end
    % Remove unused rows in table
    TbHeatm = TbHeatm(TbHeatm.Range > 0,:);
    writetable(TbHeatm, data_root + "Results_new_CB\tbHeatmap110_" + num2str(zoom) + ".csv");
    
    fig = figure();
    h = heatmap(TbHeatm, 'Range', 'Focus5', 'ColorVariable', 'BlurPix','Colormap', parula);
    xlabel("Range")
    ylabel("Focus Interval")
    title("Zoom " + num2str(zoom) + ": Blurred Pixels per Range and Focus Interval")
    set(gcf,'position',([100,100,1100,1500]),'color','w')
    fileOut = data_root + "Results_new_CB\hm110_" + num2str(zoom) + ".png";
    exportgraphics(h,fileOut,'Resolution',300)
    fileFig = data_root + "Results_new_CB\hm110_" + num2str(zoom) + ".fig";
    savefig(fig, fileFig)

%     %% Create output matrix from TbHeatm
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

