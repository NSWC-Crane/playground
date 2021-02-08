function [fp1_value , fp2_value ] = update_plot(app)

fp1_value = floor(app.Focus_point_1Slider.Value - app.Focus_point_1Slider.Limits(1) + 1);
fp2_value = floor(app.Focus_point_2Slider.Value - app.Focus_point_2Slider.Limits(1) + 1);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% 
fp1_row = app.data(fp1_value, :);
fp2_row = app.data(fp2_value, :);
%get max value of the two rows plotted
fp1_row_max = max(fp1_row);
fp2_row_max = max(fp2_row);
fprow_max = max(fp1_row_max,fp2_row_max);
% absolute difference between fp1_row and fp2_row store as fp_diff
fp_diff = abs( fp1_row - fp2_row ) ; 
% make the value of 10 variable 
var = app.DivisionsEditField.Value; 
slice = fp_diff ( 1:1 , 1:var:end);
logical_array = logical (cat (2 , 0, slice(1:end-1) == slice (2:end)));
slice_x = app.x (1:1 , 1:var:end); 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
hold(app.UIAxes , 'off');
plot(app.UIAxes, app.x, app.data(fp1_value, :), 'b');
hold(app.UIAxes , 'on');
plot(app.UIAxes, app.x, app.data(fp2_value, :), 'g');
stem(app.UIAxes, slice_x(logical_array), fprow_max*ones(1,nnz(logical_array)),'r *')
%stem(app.UIAxes, slice_x(logical_array), fprow_max*ones(1,sum (logical_array)),'r *')
end

