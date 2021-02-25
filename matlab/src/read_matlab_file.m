function [x, y, z, data] = read_matlab_file(filename)

  
    % open the file read only
     %file_id = fopen(filename, 'r');
  %%% load mydata.mat a b c load('mydata.mat','a','b','c')
    % file_id =  matfile('filename', 'focus' ,'range','zoom','coc_map');
    %   load filename.mat;
    file_id = importdata(filename);
       
  %  file_id = matfile(filename);
    % read in the first row of the csv file.  This should be the x values
%     x  = textscan(file_id, '', 1, ts_args{:});
%     x = x{1};
%     x = x(~isnan(x));
   %%%%%  x = file_id.range(:,:);
        x = file_id.range;
    % read in the second line of the csv file.  This should be the y values
%     y  = textscan(file_id, '', 1, ts_args{:});
%     y = y{1};
%     y = y(~isnan(y));
     y = file_id.focus;
     z = file_id.zoom ;
    % close the file
%       fclose(file_id);

    % read in the data starting at row 3
  %  data = csvread(filename, 2, 0);
   data = file_id.coc_map;


end
