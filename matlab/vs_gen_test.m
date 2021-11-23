format long g
format compact
clc
close all
clearvars

% get the location of the script file to save figures
full_path = mfilename('fullpath');
[scriptpath,  filename, ext] = fileparts(full_path);
plot_count = 1;
line_width = 1.0;

commandwindow;

%% load the dll/so file

lib_path = 'E:\Projects\vs_gen\build\Release\';
lib_name = 'vs_gen';
hfile = 'E:\Projects\vs_gen\include\vs_gen_lib.h';

if(~libisloaded(lib_name))
    [notfound, warnings] = loadlibrary(fullfile(lib_path, strcat(lib_name,'.dll')), hfile);
end

if(~libisloaded(lib_name))
   fprintf('\nThe %s library did not load correctly!',  lib_name);    
end

% show all the available functions and 
%libfunctionsview(lib_name);

% initialize the generator using the file
calllib(lib_name,'init_vs_gen_from_file','E:\Projects\playground\blur_params_v23a.txt');

% set the seed for the random generator
%calllib(lib_name,'set_vs_seed', 3851);

% void generate_vs_scene(double scale, unsigned int img_w, unsigned int img_h, unsigned char* img_f1_t,  unsigned char* img_f2_t, unsigned char* dm_t);
img_w = 512;
img_h = 512;
img_f1 = uint8(zeros(img_h * img_w *3, 1));
img_f2 = uint8(zeros(img_h * img_w * 3, 1));
dm = uint8(zeros(img_h * img_w, 1));

% create the correct matlab pointers to pass into the function
img_f1_t = libpointer('uint8Ptr', img_f1);
img_f2_t = libpointer('uint8Ptr', img_f2);
dm_t = libpointer('uint8Ptr', dm);

% generate the scene
calllib(lib_name,'generate_vs_scene', 0.1, img_w, img_h, img_f1_t, img_f2_t, dm_t);

% deinterleave the pointers and stack to create the images
img_f1 = cat(3, reshape(img_f1_t.Value(3:3:end), [img_h, img_w])', reshape(img_f1_t.Value(2:3:end), [img_h, img_w])', reshape(img_f1_t.Value(1:3:end), [img_h, img_w])');
img_f2 = cat(3, reshape(img_f2_t.Value(3:3:end), [img_h, img_w])', reshape(img_f2_t.Value(2:3:end), [img_h, img_w])', reshape(img_f2_t.Value(1:3:end), [img_h, img_w])');

dm = reshape(dm_t.Value, [img_h, img_w])';

figure; image(img_f1);
figure; image(img_f2);
figure; imagesc(dm); colormap(gray(23));

bp = 1;

unloadlibrary(lib_name);

