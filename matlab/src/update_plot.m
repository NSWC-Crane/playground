function [fp1_value , fp2_value ] = update_plot(app)

fp1_value = floor(app.Focus_point_1Slider.Value - app.Focus_point_1Slider.Limits(1) + 1);
fp2_value = floor(app.Focus_point_2Slider.Value - app.Focus_point_2Slider.Limits(1) + 1);
 
hold(app.UIAxes , 'off');
plot(app.UIAxes, app.x, app.data(fp1_value, :), 'b');
hold(app.UIAxes , 'on');
plot(app.UIAxes, app.x, app.data(fp2_value, :), 'g');

end
