function [maxgradV] = FindMaxGradient(img, intV, highest, colORrow )

% highest: the number of columns that will be averaged for blur number
% intV: the number of pixels to evaluate to see the greatest change in pixel value
% %             For example: 190 to 60 = 130 over intV pixels

[rowsMx,colsMx] = size(img);
if colORrow == "col"

    % Collect max grad of each column
    maxgradV = zeros(colsMx,4);
                  
    % Test each column in image for largest gradient over intV
    % Find max gradient in each column and collect information
    for col = 1:colsMx 
        img_vline = img(:,col);
        maxgrad = 0;
        for rw = 1:rowsMx-intV
            grad = abs(img_vline(rw+intV,1)-img_vline(rw,1));
            if grad > maxgrad
                maxgrad = grad;
                maxgRw = rw;
                maxgCol = col;
                if img_vline(rw,1) < img_vline(rw+intV,1)
                    posSlope = 1; % Slope of gradient is positive
                else
                    posSlope = 0; % Slope of gradient is negative
                end
            end
        end
        maxgradV(col,:) = [maxgRw, maxgCol, maxgrad, posSlope];  
    end
    
    % Return the columns with the highest gradients
    maxgradV = sortrows(maxgradV, 3, 'descend');
    % Return maxgradV
    % After sorting, return the top rows with the highest gradient.
    % The columns of maxgradV are row index, column index, change in pixel value, and
    % whether or not slope of gradient is positive.
    maxgradV = maxgradV(1:highest,:);

elseif colORrow == "row"
    % Collect max grad of each row
    maxgradV = zeros(rowsMx,4);

    % Test each row in image for largest gradient over intV
    % Find max gradient in each row and collect information
    for row = 1:rowsMx 
        img_hline = img(row,:);
        maxgrad = 0;
        for col = 1:colsMx-intV
            grad = abs(img_hline(1,col+ intV)-img_hline(1,col));
            if grad > maxgrad
                maxgrad = grad;
                maxgRw = row;
                maxgCol = col;
                if img_hline(1,col) < img_hline(1,col+intV)
                    posSlope = 1; % Slope of gradient is positive
                else
                    posSlope = 0; % Slope of gradient is negative
                end
            end
        end
        maxgradV(row,:) = [maxgRw, maxgCol, maxgrad, posSlope];  
    end
    % Return the columns with the highest gradients
    maxgradV = sortrows(maxgradV, 3, 'descend');
    % Return maxgradV
    % After sorting, return the top rows with the highest gradient.
    % The columns of maxgradV are row index, column index, change in pixel value, and
    % whether or not slope of gradient is positive.
    maxgradV = maxgradV(1:highest,:);
end

end