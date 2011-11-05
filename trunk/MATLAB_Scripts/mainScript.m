%% Import Data
inRawData = csvread([dataPath '/depthData/000015.csv']);

%% Interchange 0's for NaN
depthData = replaceValue(inRawData,2047,NaN);

%% Convert Depth to Z
zData = depthToZ(depthData);

%% Convert Z Array to Point Cloud
pointCloud = zToPointCloud(zData);