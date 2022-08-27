%% matching function

function [match, num, min_ex, max_ex] = find_match(data, low_limit, high_limit, mid_idx)

    match = (data > low_limit) == (data < high_limit);
    
    % this is where we find the group of matches that contains mid_idx
    measurements = regionprops(logical(match), 'Extrema');
    
    % default values for the whole range
    min_ex = 1;
    max_ex = numel(match);
    num = max_ex - min_ex;
    
    for jdx=1:numel(measurements)
        
        min_ex = floor(min(measurements(jdx).Extrema(:,1)));
        max_ex = ceil(max(measurements(jdx).Extrema(:,1)));
        
        if((mid_idx >= min_ex) && (mid_idx <= max_ex))
            num = max_ex - min_ex;
            break;
        end
            
    end
    
    % make sure things are within the bounds of the array
    min_ex = max(1, min_ex);
    max_ex = min(numel(match), max_ex);
    
    match = zeros(size(match));
    match(min_ex:max_ex) = 1;

end