function [maxgradV] = FindMaxGradient(img, intV, highest )

[img_w,img_h] = size(img);
% Collect max grad of each column
maxgradV = zeros(img_w,4);
              
% Test each column in image for largest gradient over intV
% Find it with lowest pixel value (dark color) first
% img_vline(rw,1) will be darker color (lower pixel value like 60)
% img_vline(rw+intv,1) will be lighter color (higher pixel value like 200)
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
maxgradV = maxgradV(1:highest,:);

end