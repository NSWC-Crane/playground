function [maxgradV] = FindMaxGradient(img, intV, highest )

% highest: the number of columns that will be averaged for blur number
% intV: the number of pixels to evaluate to see the greatest change in pixel value
% %             For example: 190 to 60 = 130 over intV pixels

[img_w,img_h] = size(img);
% Collect max grad of each column
maxgradV = zeros(img_w,4);
              
% Test each column in image for largest gradient over intV
% Find it with lowest pixel value (dark color) first
% img_vline(rw,1) will be darker color (lower pixel value like 60)
% img_vline(rw+intv,1) will be lighter color (higher pixel value like 200)

% Find max gradient in each column and collect information
for col = 1:img_w 
    img_vline = img(:,col);
    maxgrad = 0;
    for rw = 1:img_h-intV
        grad = abs(img_vline(rw+intV,1)-img_vline(rw,1));
        if grad > maxgrad
            maxgrad = grad;
            maxgRw = rw;
            maxgCol = col;
            if img_vline(rw) < img_vline(rw+intV)
                direction = 1;
            else
                direction = 0;
            end
        end
    end
    maxgradV(col,:) = [maxgCol, maxgRw, maxgrad, direction];  
end

% Return the columns with the highest grads
maxgradV = sortrows(maxgradV, 3, 'descend');
% Return maxgradV
% After sorting, return the top rows with the highest gradient.
% The columns are row index, columne index, change in pixel value, and
% direction.
maxgradV = maxgradV(1:highest,:);

end