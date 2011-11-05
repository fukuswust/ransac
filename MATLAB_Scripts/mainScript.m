%% Initial Constants
firstFile = '000015';

%% Import Data
inRawData = csvread([dataPath '/depthData/' firstFile '.csv']);

%% Interchange 0's for NaN
depthData = replaceValue(inRawData,2047,NaN);

%% Convert Depth to Z
zData = depthToZ(depthData);

%% Convert Z Array to Point Cloud
pointCloud = zToPointCloud(zData);

%% Export Point Cloud to Meshlab Format
exportMeshlab(pointCloud, [dataPath '/individualPointClouds/' firstFile '.csv']);