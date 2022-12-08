function [x, y, data] = read_coc_map(filename)

    delimiter = ',';
    whitespace = setdiff(sprintf(' \b\t'), delimiter);

    ts_args = {'Delimiter',delimiter, 'Whitespace',whitespace, 'ReturnOnError',false, 'EmptyValue',NaN, 'CollectOutput',true, 'EndOfLine','\r\n'};

    % open the file read only
    file_id = fopen(filename, 'r');

    % read in the first row of the csv file.  This should be the x values
    x  = textscan(file_id, '', 1, ts_args{:});
    x = x{1};
    x = x(~isnan(x));

    % read in the second line of the csv file.  This should be the y values
    y  = textscan(file_id, '', 1, ts_args{:});
    y = y{1};
    y = y(~isnan(y));

    % close the file
    fclose(file_id);

    % read in the data starting at row 3
    data = csvread(filename, 2, 0);

end
