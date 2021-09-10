function [Table,Info] = importExperiment(filename, dataLines)
%IMPORTFILE Import data from a text file
%  MSHOME = IMPORTFILE(FILENAME) reads data from text file FILENAME for
%  the default selection.  Returns the data as a table.
%
%  MSHOME = IMPORTFILE(FILE, DATALINES) reads data for the specified row
%  interval(s) of text file FILENAME. Specify DATALINES as a positive
%  scalar integer or a N-by-2 array of positive scalar integers for
%  dis-contiguous row intervals.
%
%  Example:
%  msHome = importfile("/home/alfy/Documents/FusionLab/02_ArduinoVersion/03_Code/MatlabCode/dataPlot/experiment/01-1ms-Home.dat", [2, Inf]);
%
%  See also READTABLE.
%
% Auto-generated by MATLAB on 09-Sep-2021 19:27:04

%% Input handling

% If dataLines is not specified, define defaults
if nargin < 2
    dataLines = [4, Inf];
end

%% Set up the Import Options and import the data
opts = delimitedTextImportOptions("NumVariables", 3);

% Specify range and delimiter
opts.DataLines = dataLines;
opts.Delimiter = "\t";

% Specify column names and types
opts.VariableNames = ["PWM", "V2_read", "Isense_read"];
opts.VariableTypes = ["double", "double", "double"];

% Specify file level properties
opts.ExtraColumnsRule = "ignore";
opts.EmptyLineRule = "read";

% Import the data
Table = readtable(filename, opts);

%% Mean Import
opts = delimitedTextImportOptions("NumVariables", 3);

% Specify range and delimiter
opts.DataLines = [2, 2];
opts.Delimiter = "\t";

% Specify column names and types
opts.VariableNames = ["V2_mean", "Isense_mean", "dt"];
opts.SelectedVariableNames = ["V2_mean", "Isense_mean", "dt"];
opts.VariableTypes = ["double", "double", "double"];

% Specify file level properties
opts.ExtraColumnsRule = "ignore";
opts.EmptyLineRule = "read";


% Import the data
Info = readtable(filename, opts);
Info.dt = Info.dt * 1E-6; % dt shuld be in second
[filepath,name,ext] = fileparts(filename);
Info.Properties.Description = name;

end