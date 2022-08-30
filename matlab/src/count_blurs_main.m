format long g
format compact
clc
close all
clearvars

% get the location of the script file to save figures
full_path = mfilename('fullpath');
[startpath,  filename, ext] = fileparts(full_path);
plot_num = 1;


%% get the directory for the images

img_path = uigetdir(startpath, 'Select Folder with Images');
image_ext = '*.png';

if(img_path == 0)
    return;
end

listing = dir(strcat(img_path, '/', image_ext));

save_dir = img_path;

fprintf('Image Path: %s\n\n', img_path);

%% run through each image in the list
commandwindow;

% this defines the expected maximum blur radius
max_blur_radius = 300;

img_offset = 10;

scan_length = 12;
scan_offset = 0.8;

slice_row = 0;
sk = create_1D_gauss_kernel(3, 1.0);
%sk = [1/3 1/3 1/3];
mf = [1 1 1];

dx = [-0.5 0 0.5];

fx = [1 -2 1];

% this is a value to provide a small buffer to exceed before being counted
offset = 1/255;

%% start running through images
fprintf('File Name\t\t\t\t\t\t\t\t# of Pixels Blurred\n')
fprintf('--------------------------------------------------------\n')
first = true;
for idx=30:numel(listing)

    % load in an image and get its size
    % img_file = fullfile(listing(idx).folder, listing(idx).name);
    img_file = fullfile(img_path, '/', listing(idx).name);
    img = imread(img_file);
    [img_h, img_w, img_c] = size(img);
    
    if(img_c > 1)
        img = double(rgb2gray(img));
    else
        img = double(img);
    end
    
    % rotate the images
    img = imrotate(img, 270);
    
    figure(1)
    set(gcf,'position',([100,100,1500,600]),'color','w')
    subplot(1,3,1);
    image(uint8(img));
    colormap(jet(256));
    
    if(first)
        [slice_col, slice_row, ~] = ginput(1);
        first = false;
    end

    % find the rough center points assuming that the knife edge is vertical
    % get the subset of the image at the center of the image - note this 
    % can be shifted to anywhere
    img_s = img(floor([slice_row - img_offset, slice_row, slice_row + img_offset]),:);
    
    % find the 'x' center of the image
    %img_cw = floor(img_w/2);
       
    subplot(1,3,2);
    mesh(img_s);
    view(10, 10);    
    
    % find the average of each column in the subset
    img_s = mean(img_s, 1);
    
    % run through each row and find the best pixel count
    num2 = zeros(1, size(img_s, 1));
    
    for row=1:size(img_s, 1)
    
        % grab a line and work with it
        img_line = img_s(row, :);
    
        % find minimum and maximum values in the line
        [min_line, min_idx] = min(img_line);
        [max_line, max_idx] = max(img_line);
        
        % get the orientation of the knife edge high->low = -1 / low->high = 1
        if(max_idx < min_idx)
            direction = -1;
        else
            direction = 1;
        end
        
        % find the rough midpoint using the min/max indexes
        %mid_idx = floor((max_idx + min_idx)/2);
        %mid_line = (max_line + min_line)/2;
        
        % find the rough end points
        % this can be the first and last element in the array or an 
        % inflection point if things curve up/down at the ends of the array
        min_start = numel(img_line);
        max_start = 1;
        
        if(direction == 1)
            
%             % check for upwards curve and cut at the min index
%             curve_check = sum(img_line(1:min_idx) > img_line(min_idx)) > 0;
%             if(curve_check == true)
%                 min_start = min_idx;
%             end
%             
%             % check for downwards curve and cut at the max index
%             curve_check = sum(img_line(max_idx:end) < img_line(max_idx)) > 0;
%             if(curve_check == true)
%                 max_start = max_idx;
%             end
            
            % crop the line based on what was previously found and make the
            % direction high -> low
            img_line = img_line(min_start:-1:max_start);
            
        else
            
%             % check for upwards curve and cut at the min index
%             curve_check = sum(img_line(min_idx:end) > img_line(min_idx)) > 0;
%             if(curve_check == true)
%                 min_start = min_idx;
%             end
%             
%             % check for downwards curve and cut at the max index
%             curve_check = sum(img_line(1:max_idx) < img_line(max_idx)) > 0;
%             if(curve_check == true)
%                 max_start = max_idx;
%             end           
            
            % crop the line based on what was previously found
            img_line = img_line(max_start:min_start);
            
        end
       
        % find the index of a point close to the mid_line value
%         [~, min_idx] = min(img_line);
%         [~, max_idx] = max(img_line);
%         mid_idx = floor((max_idx + min_idx)/2);
%         mid_line = img_line(mid_idx);
        mid_line = (max_line-min_line)/2 + min_line;
        mid_idx = find(img_line < mid_line, 1, 'first');
        
        %split the ranges in the middle
        x1 = 2:mid_idx;
        x2 = (mid_idx+1):numel(img_line)-1;
                    
        % smooth the img_line to get the rough values
        img_line_s = conv(img_line, sk, 'same');
        
        % find the high limit 
        figure(201)
        count = 0;
        for jdx=mid_idx:-1:(scan_length + 1)
            high_limit = img_line(jdx);
            
            plot(img_line(x1),'b');
            hold on;
            plot(img_line_s(x1),'g');
            tmp_line = img_line(jdx:-1:jdx-scan_length);
            r = ((tmp_line) > (high_limit-scan_offset)) & (tmp_line < (high_limit+scan_offset));
        
            count = sum(r);

            stem(jdx:-1:jdx-scan_length, r*max_line, 'r');
            hold off;
            drawnow;
            
            if(count >= 4)
                break;
            end
            %high_limit = high_limit + 1;
      
        end
        
        
%         figure(202)
        count = 0;
        for jdx=mid_idx:(numel(img_line) - scan_length - 1)
            low_limit = img_line(jdx);
%             plot(img_line(x2),'b');
%             hold on;
%             plot(img_line_s(x2),'g');
            tmp_line = img_line(jdx:jdx+scan_length);            
            r = ((tmp_line) > (low_limit-scan_offset)) & (tmp_line < (low_limit+scan_offset));
            
            count = sum(r);

%             stem((jdx:jdx+20)-mid_idx, r*max_line, 'r');
%             hold off;
%             drawnow;
            
            if(count >= 4)
                break;
            end
%             low_limit = low_limit - 1;            
        end
        
        % find the initial match
        [match, num, min_idx2, max_idx2] = find_match(img_line, low_limit, high_limit, mid_idx);        
       
        % find the mean of the lines 30 pixels from the match
        % need to make sure that we don't go out of bounds on the arrays
%         min_mean = floor(mean(img_line(max(1, min_idx2-10):max(1,min_idx2-1))) + 0.5);
%         max_mean = floor(mean(img_line(min(max_idx2+1, numel(img_line)):min(max_idx2+10, numel(img_line)))) + 0.5);       
        min_mean = ceil(mean(img_line(min_idx2:min(min_idx2+30, numel(img_line)))));
        max_mean = floor(mean(img_line(max_idx2:-1:max(1, max_idx2-30))));

        % run through the matches a second time and try to refine the match
        for jdx=min_idx2:-1:(min_idx2-30)
            if(img_line(jdx) > min_mean)
                break;
            end
        end
        min_ex2 = jdx;

        % find max_ex2
        for jdx=max_idx2:1:(max_idx2+30)
            if(img_line(jdx) < max_mean)
                break;
            end
        end
        max_ex2 = jdx;

        num2(row) = min_ex2 - max_ex2;        
               
    end
    
    
    fprintf('%03d: %s, \t%03d,\t%03d\n', (idx-1), listing(idx).name, num, floor(mean(num2)));
        
    figure(1)
    subplot(1,3,3);
    plot(img_line, '.-b');
    %stairs(floor(img_line), '.-b');
    
    hold on;
    plot(img_line_s, '--k');
    stem([max_idx2 min_idx2], [1 1]*max(img_line(:)), 'Color', 'r', 'Marker', '.');
    stem([max_ex2 min_ex2], [1 1]*max(img_line(:)), 'Color', 'g', 'Marker', '.');

    plot(low_limit*ones(size(img_line)), '--r');
    plot(high_limit*ones(size(img_line)), '--r');
    
    plot(min_mean*ones(size(img_line)), '--g');
    plot(max_mean*ones(size(img_line)), '--g');
    hold off;

    pause(1)
end    
    
fprintf('--------------------------------------------------------\n')

