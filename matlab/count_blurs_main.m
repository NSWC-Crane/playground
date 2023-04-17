format long g
format compact
clc
close all
clearvars

% get the location of the script file to save figures
full_path = mfilename('fullpath');
[startpath,  filename, ext] = fileparts(full_path);
plot_num = 1;

commandwindow;

%%
platform = string(getenv("PLATFORM"));
if(platform == "Laptop")
    data_root = "D:\data\dfd\20230407\processed\";
elseif (platform == "LaptopN")
    data_root = "C:\Projects\data\dfd\20230317\";
else   
    data_root = "C:\Data\JSSAP\Mar2023BlurCount\";
end


% this defines the expected maximum blur radius
% max_blur_radius = 300;
image_skip = 4;

focus_step = 5;

scan_step = 3;
scan_length = 10*scan_step;
scan_offset = 2.8;

high_std = 4.0;
low_std = 2.0;

img_off_low = 10;
img_off_high = 10;

slice_row = 0;

% sk = create_1D_gauss_kernel(3, 1.0);
sk = [1/3 1/3 1/3];
% 
% mf = [1 1 1];
% 
% dx = [-0.5 0 0.5];
% 
% fx = [1 -2 1];
% 
% % this is a value to provide a small buffer to exceed before being counted
% offset = 1/255;

verbose = false;

%% get the directory for the images
img_path = uigetdir(data_root, 'Select Folder with Images');
image_ext = '*.png';

if(img_path == 0)
    return;
end
save_dir = img_path;
fprintf('Image Path: %s\n\n', img_path);

img_listing = dir(strcat(img_path, '/', image_ext));

if(isempty(img_listing))
    folder_listing = dir(strcat(img_path, '/'));
    folder_listing = folder_listing(3:end);
    num_folders = numel(folder_listing);
else
    folder_listing = struct('name','', 'folder',img_path, 'date','', 'bytes',0, 'isdir',1, 'datenum',0);
    num_folders = 1;
end


%% Set up a table to collect results

numRows = 20000;
col_label = ["ImgPath", "Filename", "Range", "Zoom", "Focus", "BlurCount"];
vartypes = {'string','string','double','double','double','double'};
Tb = table('Size', [0, length(col_label)], 'VariableTypes', vartypes);
Tb.Properties.VariableNames = col_label.';


%% start running through images

fprintf('File Name\t\t\t\t\t\t\t\t# of Pixels Blurred\n')
fprintf('--------------------------------------------------------\n')
first = true;

coc_map = zeros(numel(img_listing),1);


for kdx=1:num_folders
    
    if(folder_listing(kdx).isdir == 0)
        continue;
    else
        img_listing = dir(strcat(folder_listing(kdx).folder, '/', folder_listing(kdx).name, '/', image_ext));
        
        num_images = numel(img_listing); 
        
        tmp_split = strsplit(folder_listing(kdx).folder, filesep);
        
        zoom_val = str2double(tmp_split{end}(2:end));
        range_val = str2double(folder_listing(kdx).name);
    end

%     focus_vals = zeros(num_images,1);
    for idx=1:num_images

        % load in an image and get its size
        % img_file = fullfile(listing(idx).folder, listing(idx).name);
        img_file = fullfile(img_listing(idx).folder, '/', img_listing(idx).name);
        
        split_str = strsplit(img_listing(idx).name, '_');
        dist = str2double(split_str(1));        
        focus_val = floor(str2double(split_str{3}(2:end))/focus_step + 0.5) * focus_step;
        
        img = imread(img_file);
        [img_h, img_w, img_c] = size(img);

        if(img_c > 1)
            img = double(img(:,:,2));
        else
            img = double(img);
        end

        % rotate the images
        %img = imrotate(img, 270);

        figure(1)
        set(gcf,'position',([50,100,1500,400]),'color','w')
        subplot(1,3,1);
        hold off
        image(uint8(img));
        colormap(jet(256));
        hold on

        if(first)
            [slice_col, slice_row, ~] = ginput(1);
            slice_row = floor(slice_row);
            first = false;

            if(slice_row - img_off_low < 1)
                img_off_low = slice_row - 1;
            end

            if(slice_row + img_off_high > img_h)
                img_off_high = img_h - slice_row;
            end
        end

        plot([1,img_w],[slice_row,slice_row], '-k');
    %     plot([1,img_w],floor([slice_row-img_off_low,slice_row-img_off_low]), '--k');
    %     plot([1,img_w],floor([slice_row+img_off_high,slice_row+img_off_high]), '--k');


        % find the rough center points assuming that the knife edge is vertical
        % get the subset of the image at the center of the image - note this 
        % can be shifted to anywhere
    %     img_s = img(floor([slice_row - img_off_low, slice_row, slice_row + img_off_high]),:);
        img_s = img(slice_row,:);
        img_s = mean(img, 1);

        % find the 'x' center of the image
        %img_cw = floor(img_w/2);

        subplot(1,3,2);
    %     mesh(img_s);
    %     view(10, 10);    
        plot(img_s);

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

            x1_line = img(x1);
            x2_line = img(x2);

            % find the high limit 
            count = 0;
            for jdx=mid_idx:-1:(scan_length + 1)
                high_limit = img_line(jdx);
                tmp_line = img_line(jdx:-scan_step:jdx-scan_length);
                mean_tmp_line = mean(tmp_line);
                std_tmp_line = std(tmp_line);
                tmp_line_diff = abs(tmp_line - mean_tmp_line);

    %             r = ((tmp_line) > (high_limit-scan_offset)) & (tmp_line < (high_limit+scan_offset));
    %             r = (tmp_line_diff < scan_offset);
    %             count = sum(r);

    %             if(count >= 4)
    %                 break;
    %             end

                if(std_tmp_line < high_std)
                    break;
                end

            end

            if(verbose)
                figure(201)
                subplot(1,2,1);
                plot(img_line(x1),'b');
                hold on;
                plot(img_line_s(x1),'g');
        %         stem(jdx:-scan_step:jdx-scan_length, r*max_line, 'r');
                plot([x1(1), x1(end)], mean_tmp_line*ones(2,1), '-k')
                plot([x1(1), x1(end)], std_tmp_line*[1,1]+mean_tmp_line, '--k')
                plot([x1(1), x1(end)], std_tmp_line*[-1,-1]+mean_tmp_line, '--k')
                stem(jdx, max_line, 'c');
                xlim([jdx-50, x1(end)]);
                ylim([mid_line, max_line]);
                hold off;
                drawnow;        
            end

            count = 0;
            for jdx=mid_idx:(numel(img_line) - scan_length - 1)
                low_limit = img_line(jdx);
                tmp_line = img_line(jdx:scan_step:jdx+scan_length);  
                mean_tmp_line = mean(tmp_line);
                std_tmp_line = std(tmp_line);
                tmp_line_diff = abs(tmp_line - mean_tmp_line);

                r = ((tmp_line) > (low_limit-scan_offset)) & (tmp_line < (low_limit+scan_offset));
                count = sum(r);

    %             if(count >= 4)
    %                 break;
    %             end

                if(std_tmp_line < low_std)
                    break;
                end

            end

            if(verbose)
                figure(201)
                subplot(1,2,2);
                plot(x2, img_line(x2),'b');
                hold on;
                plot(x2, img_line_s(x2),'g');        
                %stem((jdx:scan_step:jdx+scan_length)-mid_idx, r*max_line, 'r');
                plot([x2(1), x2(end)], mean_tmp_line*ones(2,1), '-k')
                plot([x2(1), x2(end)], std_tmp_line*[1,1]+mean_tmp_line, '--k')
                plot([x2(1), x2(end)], std_tmp_line*[-1,-1]+mean_tmp_line, '--k')
                stem(jdx, mid_line, 'c');
                xlim([x2(1), jdx+50]);
                ylim([min_line, mid_line]);
                hold off;
                drawnow; 
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

        coc_map(idx) = floor(mean(num2));
        
        Tb(end+1,col_label) = {img_listing(idx).folder, img_listing(idx).name, range_val, zoom_val, focus_val, coc_map(idx) };

        fprintf('%03d: %s, \t%03d,\t%03d\n', (idx-1), img_listing(idx).name, num, floor(mean(num2)));

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

        xlim([max(max_idx2-50,1), min(min_idx2+50, numel(img_line))]);
        hold off;

        drawnow
    %     pause(1)
    end
    
    fprintf('--------------------------------------------------------\n')

    [min_count, min_idx] = min(coc_map);
    min_blur_count = coc_map(1:min_idx);
    max_blur_count = coc_map(min_idx:end);    

end




%% heatmap

figure(301)
h = heatmap(Tb, 'Range', 'Focus', 'ColorVariable', 'BlurCount', 'Colormap', jet);

range = str2double(h.XData);
focus = str2double(h.YData);
coc_map = h.ColorData;
% blur_count2 = blur_count;
coc_map(isnan(coc_map)) = -1;

range = cat(1, range, range(end)+1);

coc_map = cat(2, coc_map, coc_map(:,end));

figure(302);
s1 = surf(range, focus, coc_map, 'edgecolor','none');
xlabel("Range")
ylabel("Focus")
zlabel("BlurCount")

zlim([0, max(coc_map(:))]);

%% save the data as a mat file

mat_savefile = save_dir + "/Nsample_blur_radius_data_z0.mat";

zoom = zoom_val;
save(mat_savefile, 'coc_map', 'focus','range','zoom')



%% plot the final blur count

% figure(303)
% plot(blur_count, 'b')
% hold on
% box on
% grid on

