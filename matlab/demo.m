clear all; close all; clc

org_img  = im2double(imread('tb23a_00_image_f1_0000.png'));
%img_in = imresize(org_img,[256,256]);
img_in = org_img;
% img_in = org_img([1:256], [1:256], [1:3]);

params.t_params.Cn2 = 5e-17;
params.t_params.L = 1000;
opts.frames = 1;

params.t_params.D = 0.095;
%params.t_params.L = 1000;
params.t_params.d = 1.38;
%params.t_params.Cn2 = 5e-13;

%opts.frames = 1;


params.t_params.lambda = 639.5e-9;
temp_r = sim_fun(img_in(:,:,1),params, opts);

params.t_params.lambda = 525e-9;
temp_g = sim_fun(img_in(:,:,2),params, opts);

params.t_params.lambda = 470e-9;
temp_b = sim_fun(img_in(:,:,3),params, opts);

out_stack_r = uint8(256*temp_r);
out_stack_g = uint8(256*temp_g);
out_stack_b = uint8(256*temp_b);

figure
axis tight manual % this ensures that getframe() returns a consistent size
filename = 'test_v12.gif';

img = zeros(256, 256, 3);

for n = 1:opts.frames
    % create image
    img = cat(3, out_stack_r(:,:,n), out_stack_g(:,:,n), out_stack_b(:,:,n));    
    imshow(uint8(img))
    
    frame = getframe(1);
    im = frame2im(frame);
    [imind,cm] = rgb2ind(im,256);
    
    if n == 1;
        imwrite(imind,cm,filename,'gif', 'Loopcount',inf);
    else
        imwrite(imind,cm,filename,'gif','WriteMode','append');
    end
end



figure
subplot(1, 2, 1)
imshow(org_img)
title('Original Image')
subplot(1, 2, 2)
imshow(img)
title('Added Turbulence')


% figure
% subplot(1, 3, 1)
% imshow(org_img)
% title('Original Image')
% subplot(1, 3, 2)
% imshow(img_in)
% title('Original Image (crop)')
% subplot(1, 3, 3)
% imshow(img)
% title('Added Turbulence Image')
% 
% 
% multi_r = cat(4, out_stack_r(:,:,1),out_stack_r(:,:,2),out_stack_r(:,:,3),out_stack_r(:,:,4),out_stack_r(:,:,5),out_stack_r(:,:,6),out_stack_r(:,:,7),out_stack_r(:,:,8),out_stack_r(:,:,9));
% multi_g = cat(4, out_stack_g(:,:,1),out_stack_b(:,:,2),out_stack_g(:,:,3),out_stack_g(:,:,4),out_stack_g(:,:,5),out_stack_g(:,:,6),out_stack_g(:,:,7),out_stack_g(:,:,8),out_stack_g(:,:,9));
% multi_b = cat(4, out_stack_b(:,:,1),out_stack_b(:,:,2),out_stack_b(:,:,3),out_stack_b(:,:,4),out_stack_b(:,:,5),out_stack_b(:,:,6),out_stack_b(:,:,7),out_stack_b(:,:,8),out_stack_b(:,:,9));
% 
% figure
% montage(multi_r)
% title('Red Channel')
% figure
% montage(multi_g)
% title('Green Channel')
% figure
% montage(multi_b)
% title('Blue Channel')


