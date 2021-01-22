%% matching function

function [blurs] = get_blurs(folder_path)

    max_blur_radius = 180;
    image_ext = '*.png';
    listing = dir(strcat(folder_path, '\', image_ext));
    blurs = zeros(1, numel(listing));
    
    sigma = 2;
    
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
        img = imgaussfilt(img,sigma);
%         kernel = create_gauss_kernel(3, 2.0);
%         img = conv2(img, kernel, 'valid');

        % define kernels for dx and dy partial derivatives
        kernel_dx = 0.5.*[-1, 1];
        kernel_dy = 0.5.*[-1; 1];

        % partial derivatives using convolution
        img_dx = conv2(img, kernel_dx, 'same');
        img_dy = conv2(img, kernel_dy, 'same');

        % calculate magnitude of change 
        img_gradient = (img_dx.^2 + img_dy.^2);

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

        [grad_max, max_idx] = max(grad_line);
        
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

        upper_limit = mean(x1);  
        upper_limit_std = std(x1);
        lower_limit = mean(x2);
        lower_limit_std = std(x2);


        % update starting and stopping points based on new limits
        start_point_v2 = start_point;
        while((upper_limit-upper_limit_std)> img_line(start_point_v2))
            start_point_v2 = start_point_v2 - 1;
        end 

        stop_point_v2 = stop_point;
        while((lower_limit+lower_limit_std) < img_line(stop_point_v2 ))
            stop_point_v2 = stop_point_v2  + 1;
        end 
        
        radius_v2 = stop_point_v2-start_point_v2-(2*ceil(2*sigma)+1);
        blurs(idx) = radius_v2;
%         blurs(idx) = max(0, radius_v2 - 10);
        fprintf('%03d: %s, \t%02d\n', (idx-1), listing(idx).name, radius_v2);

    end


    fprintf('-----------------------------------------------------\n')

end