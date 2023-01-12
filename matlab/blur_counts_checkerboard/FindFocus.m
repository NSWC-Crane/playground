function [focus] = FindFocus(filename)
% Parse filename to get focus value
% Return uint16

fstr = strsplit(filename,'_f');
fstr = strsplit(fstr{2},'_');
fstr = fstr{1};

focus = floor(str2double(fstr));

end