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
lib_ext = '.dll';
header_file = 'vs_gen_lib.h';

if(~libisloaded(lib_name))
    [notfound, warnings] = loadlibrary(fullfile(lib_path, strcat(lib_name, lib_ext)), header_file);
end

if(~libisloaded(lib_name))
   fprintf('\nThe %s library did not load correctly!',  lib_name);    
end

% show all the available functions and input/ouputs - Only need this if you
% want to see what MATLAB is seeing for the library - Comment out if you
% don't need it
% libfunctionsview(lib_name);
% drawnow;

% initialize the generator using the file
calllib(lib_name,'init_vs_gen_from_file', 'E:\Projects\playground\blur_params_v23a.txt');

% set the seed for the random generator - if you don't set the seed then it is based on time(NULL) in C++ 
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

%% generate the first scene
fprintf('Generating Scene 1 ...\n');
calllib(lib_name, 'generate_vs_scene', 0.1, img_w, img_h, img_f1_t, img_f2_t, dm_t);

% deinterleave the pointers and stack to create the images that are in BGR pixel format
img_f1 = cat(3, reshape(img_f1_t.Value(3:3:end), [img_h, img_w])', reshape(img_f1_t.Value(2:3:end), [img_h, img_w])', reshape(img_f1_t.Value(1:3:end), [img_h, img_w])');
img_f2 = cat(3, reshape(img_f2_t.Value(3:3:end), [img_h, img_w])', reshape(img_f2_t.Value(2:3:end), [img_h, img_w])', reshape(img_f2_t.Value(1:3:end), [img_h, img_w])');

dm = reshape(dm_t.Value, [img_h, img_w])';

figure; image(img_f1); axis off;
figure; image(img_f2); axis off;
figure; imagesc(dm); colormap(gray(23)); axis off;
drawnow;

%% generate the second scene
fprintf('Generating Scene 2 ...\n');
calllib(lib_name, 'generate_vs_scene', 0.1, img_w, img_h, img_f1_t, img_f2_t, dm_t);

% deinterleave the pointers and stack to create the images that are in BGR pixel format
img_f1_2 = cat(3, reshape(img_f1_t.Value(3:3:end), [img_h, img_w])', reshape(img_f1_t.Value(2:3:end), [img_h, img_w])', reshape(img_f1_t.Value(1:3:end), [img_h, img_w])');
img_f2_2 = cat(3, reshape(img_f2_t.Value(3:3:end), [img_h, img_w])', reshape(img_f2_t.Value(2:3:end), [img_h, img_w])', reshape(img_f2_t.Value(1:3:end), [img_h, img_w])');

dm_2 = reshape(dm_t.Value, [img_h, img_w])';

figure; image(img_f1_2); axis off;
figure; image(img_f2_2); axis off;
figure; imagesc(dm_2); colormap(gray(23)); axis off;
drawnow;

%% unload the library from memory after we're done with it
fprintf('Unloading %s\n', lib_name);
unloadlibrary(lib_name);

