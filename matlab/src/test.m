clc
% clear
close all 

folder_path = uigetdir('../Passive Ranging', 'Select Folder with Images');

listing = dir(strcat(folder_path, '/', '*_cm'));
coc_map = zeros(41, numel(listing));

for idx=1:numel(listing)
    folder_name = listing(idx).name;
    sub_folder = dir(strcat(folder_path, '\', folder_name, '\2020*'));
    coc_map(:,idx) = get_blurs(strcat(folder_path, '\', folder_name, '\', sub_folder(1).name));
end


surf(coc_map)
colorbar;
shading interp;

xlabel('Focus Range')
ylabel('d_0 (cm)')
zlabel('Blur radius (px)')

