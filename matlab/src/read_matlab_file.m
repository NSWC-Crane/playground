function [x, y, z, data] = read_matlab_file(filename)
    
     file_id = importdata(filename);
     x = file_id.focus;
     y = file_id.range;
     z = file_id.zoom ;
     data = file_id.coc_map; 
     
%      size_x = size(x);
%      size_y = size(y);
%      size_z = size(z);
     
%      x_max_index  = size_x(2);
%      y_max_index  = size_y(2);
%      z_max_index  = size_z(2);
     
     x_max_index  = numel(x);
     y_max_index  = numel(y);
     z_max_index  = numel(z);

     [range, focus, zoom] = size(data);
     if range ==  x_max_index 
         x = file_id.range ;
     else
         disp( 'range does not match with data')
     end
     if focus == y_max_index
          y = file_id.focus;
     else
         disp( 'focus does not match with data')
     end
      if zoom == z_max_index
          z = file_id.zoom;
     else
         disp( 'zoom does not match with data')
     end
     
     
    % close the file
%       fclose(file_id);

    % read in the data starting at row 3
  %  data = csvread(filename, 2, 0);
  


end
