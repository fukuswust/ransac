%% Initial Constants
firstFile = '000015';
globalPlanes(3).normal = [0 7 5];
globalPlanes(3).num = 5;
prevTransform = [1 2 3 4 5 6];

%% Import Data
inRawData = csvread([dataPath '/depthData/' firstFile '.csv']);

%% Interchange 0's for NaN
depthData = replaceValue(inRawData,2047,NaN);

%% Convert Depth to Z
zData = depthToZ(depthData);

%% Convert Z Array to Point Cloud
pointCloud = zToPointCloud(zData);

%% Export Point Cloud to Meshlab Format
exportMeshlab(pointCloud, [dataPath '/individualPointClouds/' firstFile '.ply']);

%% Determine Normals for Point Cloud
normals = determineNormals(pointCloud);

%% Segment Point Cloud
segments = segmentPointCloud(pointCloud, normals);

%% Extract Planes
planes = extractPlanes(segments);

%% Compare to Global Model
transform = comparePlanes(planes, globalPlanes, prevTransform);

%% Combine with Global Model for Given Specs