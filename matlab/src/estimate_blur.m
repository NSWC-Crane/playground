clc
clear
close all 

folder_path = uigetdir('../Passive Ranging', 'Select Folder with Images');
image_ext = '*.png';

if(folder_path == 0)
    fprintf('folder path was not chosen\n')
    return;
end

listing = dir(strcat(folder_path, '/', image_ext));

%% variables

show_imgs = false;

max_blur_radius = 180;
sk = create_1D_gauss_kernel(3, 2.0);
offset = 0.25;
delta = 0.95;

%% 
fprintf('File Name\t\t\t\t\t\t\t# of Pixels Blurred\n')
fprintf('-----------------------------------------------------\n')

for idx=1:numel(listing)
    
    % load image
    img_path = strcat(folder_path, '/',  listing(idx).name);
    img = imread(img_path);
    
    % image dimensions
    [img_h, img_w, img_c] = size(img);
    
    % convert to gray scale image 
    if(img_c > 1)
        img = double(rgb2gray(img));
    else
        img = double(img);
    end
    
    % apply a guassian filter
    img = imgaussfilt(img, 2);
    
    % define kernels for dx and dy partial derivatives
    kernel_3dx = .5.*[0, 0, 0; 
                     -1, 0, 1;
                      0, 0, 0];
    % look at how 1st and 2nd order derivatives change as blur radius
    % increases and decreases. 
    kernel_dx = 0.5.*[-1, 1];
    kernel_dy = 0.5.*[-1; 1];
    
    % partial derivatives using convolution
    img_dx = conv2(img, kernel_dx, 'same');
    img_dy = conv2(img, kernel_dy, 'same');
    
    % calculate magnitude of change 
    img_gradient_sqrt = ((img_dx.^2 + img_dy.^2).^(1/2));
    img_gradient = (img_dx.^2 + img_dy.^2);
    
    % display image if show_img is true
    if(show_imgs)
        figure(1)
        imshow(img_gradient)
        figure(2)
        imshow(img_gradient_sqrt)
    end
    
    % slice images
    grad_hc = floor(img_h/2);
    grad_wc = floor(img_w/2);
    width_range = max(2, grad_wc-max_blur_radius):1:min(img_w, grad_wc+max_blur_radius);
    
    grad_line = img_gradient(grad_hc-5:grad_hc+5, :); % horizontal slice
    grad_line = mean(grad_line, 1);
    grad_line = grad_line(width_range);
    
    img_line = img(floor(img_h/2-5:img_h/2+5),:);  % a horizontal slice centered around img_h/2
    img_line = mean(img_line, 1); % mean of each column in img slice    
    img_line = img_line(width_range);
    
    if(show_imgs)
        figure(3)
        plot(grad_line); 
        img_line = conv(grad_line, sk);
        figure(4)
        plot(img_line); 
    end
    
    [grad_max, max_idx] = max(grad_line);
    [img_min, min_idx] = min(grad_line);
    
    % estimate starting and stopping points    
    threshold = grad_max*(1-0.97);
    
    start_point = max_idx;
    while(grad_line(start_point) > threshold)
        start_point = start_point - 1;
    end
    
    stop_point = max_idx;
    while(grad_line(stop_point) > threshold)
        stop_point = stop_point + 1;
    end
    
    
    % approx the upper and lower limits 
    x1 = img_line(start_point-30:start_point);
    x2 = img_line(stop_point:stop_point+30);
    
    upper_limit = mean(x1);  %% NEW upper and lower limits 
    lower_limit = mean(x2);
    
    % update starting and stopping points based on new limits
    start_point_v2 = start_point;
    while(upper_limit > img_line(start_point_v2))
        start_point_v2 = start_point_v2 - 1;
    end 
    
    stop_point_v2 = stop_point;
    while(lower_limit < img_line(stop_point_v2 ))
        stop_point_v2 = stop_point_v2  + 1;
    end 
    
    
%     [match, num, min_ex, max_ex] = find_match(img_line, start_point_v2, stop_point_v2, 250);
    radius = stop_point-start_point;
    fprintf('%03d: %s, \t%02d\t%02d\n', (idx-1), listing(idx).name, radius, (stop_point_v2-start_point_v2));
    
    
    %https://stackoverflow.com/questions/28389997/plot-vertical-lines-at-a-certain-time
    figure(1);  
    plot(grad_line, '.-b') 
    ylabel('Magnitude')
    hold on;
    
    limits = [start_point stop_point, start_point_v2, stop_point_v2];
    for i=1:numel(limits)
        if(i<3)
            plot([limits(i) limits(i)], [0, max(grad_line)], 'r')
        else
            plot([limits(i) limits(i)], [0, max(grad_line)], 'm')
        end
    end
    hold off;


    figure(2)
    plot(img_line, '.-b');   
    ylabel('Pixel Value')
    hold on;
    
    limits = [start_point, stop_point, start_point_v2, stop_point_v2];
    for i=1:numel(limits)
        if(i<3)
            plot([limits(i) limits(i)], [0, max(img_line)], 'r')
        else
            plot([limits(i) limits(i)], [0, max(img_line)], 'm')
        end
    end
    
    plot(img_line(start_point)*ones(size(img_line)), 'g');
    plot(img_line(stop_point)*ones(size(img_line)), 'g');
    
    % new computed lower and upper limits
    plot(upper_limit*ones(size(img_line)), 'c');
    plot(lower_limit*ones(size(img_line)), 'c');
    hold off;
    
end


fprintf('-----------------------------------------------------\n')



