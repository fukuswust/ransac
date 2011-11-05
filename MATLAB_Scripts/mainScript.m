%% Import Data
inRawData = csvread([dataPath '/depthData/000015.csv']);

%% Interchange 0's for NaN
tic
depthData = replaceValue(inRawData,2047,NaN);
toc

%% Convert Depth to Z
convertedDepthData = depthToZ(depthData);