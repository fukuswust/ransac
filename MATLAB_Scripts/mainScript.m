%% Initial Constants
firstFile = '000005';

%for fileOn = 0:32
%    firstFile = sprintf('%.6u',fileOn);

globalPlanes(3).normal = [0 7 5];
globalPlanes(3).vertices(4,:) = [1 2 3];
globalPlanes(3).adjacentPlanes(2) = 0;
globalPlanes(3).features(2) = 0;

prevTransform = [1 2 3 4 5 6];

%% Import Data
inDepthData = csvread([dataPath '/depthData/' firstFile '.csv']);
inRColorData = csvread([dataPath '/colorData/' firstFile '_R.csv']);
inGColorData = csvread([dataPath '/colorData/' firstFile '_G.csv']);
inBColorData = csvread([dataPath '/colorData/' firstFile '_B.csv']);
inAccelData = transpose(csvread([dataPath '/accelData/' firstFile '.csv']));

%% Combine Color Values
colorData(:,:,1) = inRColorData(:,:)./256;
colorData(:,:,2) = inGColorData(:,:)./256;
colorData(:,:,3) = inBColorData(:,:)./256;
image(colorData);

%% Shrink Color Data
sColorData = shrinkPointCloud(colorData,4);
image(sColorData);

%% Convert Depth to Point Cloud
% Interchange 0's for NaN
depthData = replaceValue(inDepthData,2047,NaN);
% Convert Depth to Z
zData = depthToZ(depthData);
% Convert Z Array to Point Cloud
pointCloud = zToPointCloud(zData);
printImage(pointCloud(:,:,3));

%% Shrink Point Cloud
sPointCloud = shrinkPointCloud(pointCloud, 16);
printImage(sPointCloud(:,:,3));

%% Determine Quality of Gravity Vector
gravityVectorQuality(inAccelData)
tmp = printImage(sPointCloud(:,:,2));
imwrite(tmp,[dataPath '/MATLAB_output/0_' firstFile '.bmp']);

%% Align to Gravity
upVect = [0 1 0];
gravityRotate = getRotationMatrix(inAccelData, upVect);
gPointCloud = rotate3dMatrix(sPointCloud, gravityRotate);
tmp = printImage(gPointCloud(:,:,2)); %All floor should be black
imwrite(tmp,[dataPath '/MATLAB_output/1_' firstFile '.bmp']);

%% Segment Floor (Implement RANSAC)
fPointCloud = segmentFloor(gPointCloud);
tmp = printImage(fPointCloud(:,:,2)); %All floor should be black
imwrite(tmp,[dataPath '/MATLAB_output/2_' firstFile '.bmp']);

%% Convert to Polar
pPointCloud = convertToPolar(gPointCloud);
tmp = printImage(pPointCloud(:,:,3));
imwrite(tmp,[dataPath '/MATLAB_output/3_' firstFile '.bmp']);

%% Segment Walls
wPointCloud = segmentWalls(pPointCloud);
tmp = printImage(wPointCloud(:,:,3));
imwrite(tmp,[dataPath '/MATLAB_output/4_' firstFile '.bmp']);

%% Convert to Cartesian
cwPointCloud = polarToCartesian(wPointCloud);
tmp = printImage(cwPointCloud(:,:,3));

%% Export Point Cloud to Meshlab Format
exportMeshlab(gPointCloud, [dataPath '/pointClouds/' firstFile '_g.ply']);

% 
% %% Determine Normals for Point Cloud
% normals = determineNormals(pointCloud);
% 
% %% Print Image
% printImage(log(abs(normals(:,:,3))+0.5));
% 
% %% Segment Point Cloud
% segments = segmentPointCloud(pointCloud, normals);
% 
% %% Extract Planes
% planes = extractPlanes(segments);
%   
% %% Compare to Global Model
% transform = comparePlanes(planes, globalPlanes, prevTransform);
% 
% %% Combine with Global Model for Given Specs