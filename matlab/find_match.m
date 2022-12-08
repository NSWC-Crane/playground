%% matching function

function [match, num, min_idx, max_idx] = find_match(data, low_limit, high_limit, mid_idx)

    match = (data > low_limit) == (data < high_limit);
    
    % this is where we find the group of matches that contains mid_idx
    measurements = regionprops(logical(match), 'Extrema');
    
    % default values for the whole range
    max_idx = 1;
    min_idx = numel(match);
    num = min_idx - max_idx;
    
    for jdx=1:numel(measurements)
        
        max_idx = floor(min(measurements(jdx).Extrema(:,1)));
        min_idx = ceil(max(measurements(jdx).Extrema(:,1)));
        
        if((mid_idx <= min_idx) && (mid_idx >= max_idx))
            num = min_idx - max_idx;
            break;
        end
            
    end
    
    % make sure things are within the bounds of the array
    max_idx = max(1, max_idx);
    min_idx = min(numel(match), min_idx);
    
    match = zeros(size(match));
    match(max_idx:min_idx) = 1;

end