format long g
format compact
clc
close all
clearvars

% get the location of the script file to save figures
full_path = mfilename('fullpath');
[startpath,  filename, ext] = fileparts(full_path);
plot_num = 1;

%% input parameters

% range limits (m)
r_min = 500;
r_max = 1020;
r_step = 1;

% focus point limits (m)
fp_min = 590;
fp_max = 1020;
fp_step = 1;

% focal length (mm)
f = 1380;

% f-number
f_num = 14.5;

% pixel size (mm)
px_size = 0.00345;

%% for through the ranges

range = (r_min:r_step:r_max)*1e3;

coc_map = [];

for d_o=(fp_min:fp_step:fp_max)*1e3
    
    dn = (range <= d_o);
    
    d_near = range(dn);
    d_far = range(~dn);
    
    tl = (d_o*f*f)/(f_num*(d_o-f));
    
    coc_far = (tl*((1/d_o)-(1./d_far)));
    coc_near = (tl*((1./d_near) - (1/d_o)));
    
    coc_map(end+1, :) = ceil([coc_near coc_far]/px_size); 
     
end

%% plot the surface

x = floor(range/1e3);
y = fp_min:fp_step:fp_max;

figure(plot_num)
set(gcf,'position',([50,50,800,600]),'color','w')
surf(x, y, coc_map)

box on
set(gca,'fontweight','bold','FontSize', 13);

colormap(parula(100));
shading interp;

% X-Axis
xlim([x(1) x(end)]);
%xticks(linspace(X(1), X(end), 11));
%xticklabels([(-fs/2):1e6:fs/2]/1e6);
xlabel('Range (m)', 'fontweight', 'bold', 'FontSize', 13);

% Y-Axis
ylim([y(1) y(end)]);
%yticks(linspace(Y(1), Y(end), 11));
%yticklabels([0:0.005:0.1]);
%ytickformat('%1.3f')
ylabel('d_o (m)', 'fontweight', 'bold', 'FontSize', 13);

% Z-Axis
zlim([0 40]);
zlabel('Blur Radius (px)', 'fontweight', 'bold', 'FontSize', 13);

view(-15, 60);

ax = gca;
%ax.Position = [0.07 0.175 0.92 0.23];

plot_num = plot_num + 1; 

%% save the x, y, and coc_map

file_filter = {'*.csv','csv Files';'*.*','All Files' };

[save_file, save_path] = uiputfile(file_filter, 'Enter Base File Name', startpath);
if(save_path == 0)
    return;
end

dlmwrite(fullfile(save_path, save_file), x, 'delimiter', ',');
dlmwrite(fullfile(save_path, save_file), y, '-append', 'delimiter', ',');
dlmwrite(fullfile(save_path, save_file), coc_map, '-append', 'delimiter', ',');



