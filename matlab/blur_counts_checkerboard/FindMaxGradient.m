function [img_maxgCol,maxgrad,maxgRw,maxgCol] = FindGradient(img, intV )

[img_w,img_h] = size(img);
maxgrad = 0;
                
% Test each column in image for largest gradient over intV
% Find it with lowest pixel value (dark color) first
% img_vline(rw,1) will be darker color (lower pixel value like 60)
% img_vline(rw+intv,1) will be lighter color (higher pixel value like 200)
for col = 1:img_w 
    img_vline = img(:,col);
    for rw = 1:img_h-intV
        grad = img_vline(rw+intV,1)-img_vline(rw,1);
        if grad > maxgrad
            maxgrad = grad;
            maxgRw = rw;
            maxgCol = col;
            img_maxgCol = img(:,maxgCol); % maxgRw will be darker (low pix val)
        end
    end
end

end