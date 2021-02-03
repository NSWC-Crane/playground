clc
clear
close all 

folder_path = uigetdir('../Passive Ranging', 'Select Folder with Images');

fileID = fopen('data.txt','w');
distances = [];
position = zeros(1, 41);
image_ext = '*.png';

listing = dir(strcat(folder_path, '/', '*_cm'));
coc_map = zeros(41, numel(listing));

for idx=1:numel(listing)
    folder_name = listing(idx).name;
    
    split_str = strsplit(folder_name, '_');
    dist = str2double(split_str(1));
    distances(end+1) = dist;
    
    sub_folder = dir(strcat(folder_path, '\', folder_name, '\2020*'));
    [coc_map(:,idx), temp] = get_blurs(strcat(folder_path, '\', folder_name, '\', sub_folder(1).name));
    position = position + temp;
end

position = floor(position/7);

% write data to file 
for idx=2:numel(distances)
    fprintf(fileID,'%d,',distances(idx-1));
end

fprintf(fileID,'%d',distances(idx));
fprintf(fileID,'\n');

for idx=2:numel(position)
    fprintf(fileID,'%d,',position(idx-1));
end

fprintf(fileID,'%d',position(idx));
fprintf(fileID,'\n');

[rows, cols] = size(coc_map);
for r=1:rows
    for c=1:(cols-1)
        fprintf(fileID,'%d,',coc_map(r,c));
    end
    fprintf(fileID,'%d',coc_map(r, cols));
    fprintf(fileID,'\n');
end

fclose(fileID);

% plot the surface
figure
set(gcf,'position',([50,50,800,600]),'color','w')
surf(distances, position, coc_map)

box on
set(gca,'fontweight','bold','FontSize', 13);

colorbar;
shading interp;

% X-Axis
xlabel('d_0 (cm)')

% Y-Axis
ylabel('Motor Position')

% Z-Axis
zlabel('Blur radius (px)')

view(-60, 30);

