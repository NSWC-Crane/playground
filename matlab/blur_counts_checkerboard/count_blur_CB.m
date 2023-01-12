
clear
clc

rangeV = 600:50:1000;
%zoomV = [2000,2500,3000,3500,4000];
zoomV = 5000;
gradL = 110;

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
    %% Set up a table to collect results 
    numFiles = 20000;
    col_label = ["ImgPath","Filename","Range","Zoom","Focus","ImgHt","ImgWd","BlurCount"];
    vartypes = {'string','string','uint16','uint16','uint16','uint16','uint16','double'};
    Tb = table('Size', [numFiles, length(col_label)], 'VariableTypes', vartypes);
    Tb.Properties.VariableNames = col_label.';
    indT = 1;
    for rng = rangeV
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
            img = img(:,:,2);
            [img_h,img_w] = size(img);
            
            %% Find max gradent in pixel values
            % Find max change over intV pixels.
            % For each image, look at different intV sizes 5,6,7 and take
            % the mean blurcount of all gradients. Find their average.
            intVs = [5,6,7];
            blurPix = [];
            maxgradV = [];
            ib = 1;
            for intV = intVs
                
                %% For each column in image, find column with maximum gradient over intV pixels
                % The maximum gradient represents the change from
                % black/white or white/black in checkerboard image.  The
                % maximum may indicate the least turbulence, so that the
                % blur in this region is due only to focus and range.
                [img_maxgCol,maxgrad,maxgRw,maxgCol] = FindMaxGradient(img, intV);
                               
                searchVal = 20;
                if maxgRw + searchVal+1 > img_h
                    %finalmx = 256;
                    continue;
                elseif maxgRw-searchVal-1 < 1
                    continue
                else
                    [iMx,mx,iMn,mn] = FindMaxMin(maxgRw,intV,img_maxgCol, searchVal);
                end

                % Find blur count
                blurPix(ib) = CalculateBlur(img_maxgCol, iMn, iMx, mn, mx);
                maxgradV(ib) = maxgrad;
                ib=ib+1;

%                 figure()
%                 plot(1:img_h, img_maxgCol, '-b')
%                 hold on
%                 plot(maxgRw, img_maxgCol(maxgRw), 'r*')
%                 hold on
%                 plot(iMn, mn, 'g*')
%                 hold on
%                 plot(iMx, mx, 'c*')
%                 xlabel("Pixel Number for Column")
%                 ylabel("Pixel Value")
%                 grid on
%                 xticks(0:10:img_h) 
%                 xlim([1,img_h])
%                 ylim([0,255])
%                 
%                 hold off
%                 close all

              
            end
            if mean(blurPix) > 0
                if mean(maxgradV) > gradL
                    Tb(indT,["ImgPath","Filename","Range","Zoom","Focus","ImgHt","ImgWd"]) = ...
                        {img_path, listing(idx).name, rng, zoom, focus, img_h, img_w };
                    Tb.BlurCount(indT) = mean(blurPix);
                    Tb.MaxGrad(indT) = maxgrad;
                    indT = indT+1;
                end
            end
            
        end
    end

    %% Heatmap
    % Round focus values to closest multiple of 5.
    Tb.Focus5 = 5*floor(Tb.Focus/5);
    % Remove unused rows in table
    Tb = Tb(Tb.Range > 0,:);
    %writetable(Tb, data_root + "results_CB_focusRange\tb120_" + num2str(zoom) + ".csv");
    
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
    %writetable(TbHeatm, data_root + "results_CB_focusRange\tbHeatmap120_" + num2str(zoom) + ".csv");
    
    fig = figure();
    h = heatmap(TbHeatm, 'Range', 'Focus5', 'ColorVariable', 'BlurPix','Colormap', parula);
    xlabel("Range")
    ylabel("Focus Interval")
    title("Zoom " + num2str(zoom) + ": Blurred Pixels per Range and Focus Interval")
    set(gcf,'position',([100,100,1100,1500]),'color','w')
    fileOut = data_root + "results_CB_focusRange\hm120_" + num2str(zoom) + ".png";
    %exportgraphics(h,fileOut,'Resolution',300)
    fileFig = data_root + "results_CB_focusRange\hm120_" + num2str(zoom) + ".fig";
    %savefig(fig, fileFig)

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

% figure()
% plot(1:img_h, img_maxgCol, '-b')
% hold on
% plot(maxgRw, img_maxgCol(maxgRw), 'r*')
% hold on
% plot(iMn, mn, 'g*')
% hold on
% plot(iMx, mx, 'c*')
% xlabel("Pixel Number for Column")
% ylabel("Pixel Value")
% grid on
% xticks(0:10:img_h) 
% xlim([1,img_h])
% ylim([0,255])
% 
% hold off
%         
