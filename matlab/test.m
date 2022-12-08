clc
clear
close all 

filename = 'data.mat';
image_ext = '*.png';
range = [];
focus = [];
zoom = [];
coc_map = [];

folder_path = uigetdir('../Passive Ranging', 'Select Folder with Images');
listing = dir(strcat(folder_path, '/', '*_cm'));

for idx=1:numel(listing)
    folder_name = listing(idx).name;
    
    split_str = strsplit(folder_name, '_');
    dist = str2double(split_str(1));
    range(end+1) = dist;
    
    sub_folder = dir(strcat(folder_path, '\', folder_name, '\20*'));
    [coc_map(:,end+1), focus(end+1, :), zoom(end+1, :)] = get_blurs(strcat(folder_path, '\', folder_name, '\', sub_folder(1).name));
end

focus = floor(sum(focus)/numel(listing));
zoom = floor(sum(zoom)/numel(listing));

zoom(end+1) = 100;
zoom = unique(zoom);

coc_map(:, :, end+1) = coc_map;

% save workspace variables
save(filename, 'range', 'focus', 'zoom', 'coc_map')

% plot the surface
figure
set(gcf,'position',([50,50,800,600]),'color','w')
surf(range, focus, coc_map(:, :, 1))

box on
set(gca,'fontweight','bold','FontSize', 13);

colorbar;
shading interp;

% X-Axis
xlabel('d_0 (cm)')

% Y-Axis
ylabel('focus')

% Z-Axis
zlabel('Blur radius (px)')

view(-60, 30);

