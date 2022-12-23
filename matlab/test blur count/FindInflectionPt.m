function [idxIP, valid, ymax, ymin] = FindInflectionPt( x, y, ymax, ymin, domain)

% Finds the inflection point of fitted curve within a specified domain
% Tests for validity of location 
% x: domain for y
% y: fitted curve
% domain: limited domain to search for inflection point, IP
% ymax: logical of y to indicate location of local maxima
% ymin:  logical of y to indicat location of local minima

% step size
h = x(2)-x(1);
% To find inflection point, find 2nd derivative
% Look only in domain
dfdy = diff(y(domain,:))/h;
df2dy2 = diff(dfdy)/h;
d2 = domain(3:end)-1;
df2dy2 = [d2, df2dy2];

% Find inflection point where 2nd deriv = 0
% Find value in df2dy2 that is closest to zero
df2val = min(abs(df2dy2(:,2)));
% Find location in df2dy2 of valIP
df2idx = find(abs(df2dy2(:,2))==df2val,1,'first');
% Find location in fitted curve of inflection point
idxIP = df2dy2(df2idx,1); 

% Test validity of IP
% Is there a change from negative to poitive?
% Test for first and last indices of df2idx (meaning there's no inflection
% point).
if df2idx == length(df2dy2) || df2idx == 1
    valid = false;
    % Set idxIP to middle of domain
    idxIP = df2dy2(floor(end/2),1);
else
    if (df2dy2(df2idx-1,2)) > 0 && (df2dy2(df2idx+1,2) < 0)
        valid = true;
    elseif (df2dy2(df2idx-1,2) < 0) && (df2dy2(df2idx+1,2) > 0)
        valid = true;
    else 
        valid = false;
        % Set idxIP to middle of domain
        idxIP = df2dy2(floor(end/2),1);
    end
end

% Test for location of max mins
% Are there any mins to the right of the IP?
if sum(ymin(idxIP:end)) == 0
    ymin = zeros(size(ymin));
end
% Are there any maxs to the left of the IP?
if sum(ymax(1:idxIP)) == 0
    ymax = zeros(size(ymax));
end
              
end