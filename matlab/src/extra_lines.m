
%scan through fp_diff at every division and find neighboring divisions that are the same value 
%fp_diff1 = fp_diff;
%fp_diff2 = fp_diff;
%combine_fp_diff = [fp_diff1;fp_diff2];

%find all repeated max values for both row fp1 
fp1_max_repeated = ( fp1_row_max == fp1_row);
fp1_max_repeated_values = ( fp1_row .* ( fp1_max_repeated));
%find all repeated max values for both row fp2 
fp2_max_repeated = ( fp2_row_max == fp2_row);
fp2_max_repeated_values = ( fp2_row .* ( fp2_max_repeated));
%find all repeated max values for both row fp1 logical matrix 
fp_diff_repeated =   ( fp1_max_repeated ==  fp2_max_repeated);
%fp_diff_value = 
compared = ( fp1_row == fp2_row);
%compared_values = ( fp1_row .* ( compared));

%%%%%%%%%%%%%%%%%
%hold(app.UIAxes , 'off');
%figure (3)
%stem( app.x , app.data(fp_diff, :));
%hold(app.UIAxes , 'on');

cat (2 , 0, slice(1:end-1) == slice (2:end))
 stem([600 900] , 25 *ones (1,2) )
