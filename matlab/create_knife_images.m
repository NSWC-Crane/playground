clc
clear
close all 


%% variables
img_h = 268;
img_w = 268;
num_channels = 1;
direction = -1; % high->low = -1 / low->high = 1

% sigma = [0.0005,0.1725,0.3450,0.5175,0.6900,0.8625,1.0350,1.2075,1.3800,1.5525,1.7250,1.8975,2.0700,2.2425,2.4150,2.5875,2.7600,2.9325,3.1050,3.2775,3.4500,3.6225,3.7950,3.9675,4.1400,4.3125,4.4850,4.6575,4.8300,5.0025,5.1750,5.3475,5.5200,5.6925,5.8650,6.0375,6.2100,6.3825,6.5550,6.7275,6.9000,7.0725,7.2450,7.4175,7.5900,7.7625,7.9350,8.1075,8.2800,8.4525,8.6250,8.7975,8.9700,9.1425,9.3150,9.4875,9.6600,9.8325,10.0050,10.1775,10.3500,10.5225,10.6950,10.8675,11.0400,11.2125,11.3850,11.5575,11.7300,11.9025,12.0750,12.2475];
sigma = [0.0005,0.0005,6.0375,12.0750];
kernel_size = 60;
mean = 3;

save_dir = 'C:\Projects\playground\images\sample_v2';
image_ext = '.png';
% tag = create_tag(7);
% num_imgs = 30;

[status, msg, msgID] = mkdir(save_dir);
if(status == 0)
    fprintf('Failed to create save directory')
    return;
end

%% 

fprintf('-----------------------------------------------------\n')
fprintf('File Name\t\tkernel size\t\tsigma\n')

for idx=1:numel(sigma)
    
%     create img 
    black_img = 1.0*ones(img_h, floor(img_w/2), 'double');
    white_img = 255.0*ones(img_h, floor(img_w/2), 'double');
    
    if(direction == 1)
        img = [black_img white_img]; 
    else
        img = [white_img black_img];
    end
    
    img = im2double(img); 

    kernel = create_gauss_kernel(kernel_size, sigma(idx));  
    img = conv2(img, kernel, 'valid');

%     add noise
    % noise = poissrnd(mean, size(img)) - mean;
    % img = img + noise;
    
    img = uint8(img);
    % if(num_channels == 3)
    %     img = cat(3, img, img, img);
    % end
    
%     save img
    file_name = sprintf('img%03i%s', idx, image_ext);
    file_name = fullfile(save_dir, file_name);
    % 
    
    fprintf('%s:\t\t%i\t\t\t\t%.6f\n', file_name, kernel_size, sigma(idx));
    
    fig = figure(1);
    set(gcf,'position',([100,100,500,500]),'color','w')
    imshow(img)
    hold on
    plot(170-0.5*img(100,:), 'r', 'LineWidth', 1.5);

    % frame = getframe(fig);
    % imwrite(frame2im(frame), file_name);
    % ax = gca;
    % ax.Position = [-0.1, 0.0, 0.8, 1.0];
    print(1, '-dpng', file_name);
    
end



fprintf('-----------------------------------------------------\n')
