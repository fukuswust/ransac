%% Initial Constants
firstFile = '000005';

%% Loop
for fileOn = 0:33
    firstFile = sprintf('%.6u',fileOn);

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
axis equal
%imwrite(colorData,[dataPath '/colorImages/' firstFile '.bmp']);

%% Shrink Color Data
sColorData = shrinkPointCloud(colorData,4);
image(sColorData);
axis equal

%% Shrink Depth Data and Replace NaN
% Shrink Matrix
sDepthData = shrinkMatrix(inDepthData, 16);
% Interchange 0's for NaN
nanDepthData = replaceValue(sDepthData,2047,NaN);
printImage(nanDepthData);
axis equal

%% Convert Depth to Point Cloud
% Convert Depth to Z
zData = depthToZ(nanDepthData);
% Convert Z Array to Point Cloud
pointCloud = zToPointCloud(zData);
printImage(pointCloud(:,:,3));
axis equal

%% Add Color to Point Cloud
cPointCloud = addGreyscaleToPointCloud(pointCloud, colorData);
printImage(cPointCloud(:,:,4)); %All floor should be black
axis equal

%% Determine Quality of Gravity Vector
gravityVectorQuality(inAccelData)
tmp = printImage(pointCloud(:,:,2));
axis equal
imwrite(tmp,[dataPath '/MATLAB_output/0_' firstFile '.bmp']);

%% Align to Gravity
upVect = [0 1 0];
gravityRotate = getRotationMatrix(inAccelData, upVect);
gPointCloud = transform3dMatrix(cPointCloud, gravityRotate);
tmp = printImage(gPointCloud(:,:,2)); %All floor should be black
axis equal
imwrite(tmp,[dataPath '/MATLAB_output/1_' firstFile '.bmp']);

%% Segment Floor (Implement RANSAC)
fPointCloud = segmentFloor(gPointCloud);
tmp = printImage(fPointCloud(:,:,2)); %All floor should be black
axis equal
imwrite(tmp,[dataPath '/MATLAB_output/2_' firstFile '.bmp']);

%% Convert to Polar
pPointCloud = cartesianToPolar(gPointCloud);
tmp = printImage(pPointCloud(:,:,3));
axis equal
imwrite(tmp,[dataPath '/MATLAB_output/3_' firstFile '.bmp']);

floorHeight = min(min(pPointCloud(:,:,1)));
fprintf('Height: %f cm\n', floorHeight);
minDir = min(min(pPointCloud(:,:,2)));
fprintf('Min Dir: %f rad\n', minDir);
maxDir = max(max(pPointCloud(:,:,2)));
fprintf('Max Dir: %f rad\n', maxDir);
dirSpan = maxDir - minDir;
fprintf('Dir Span: %f rad (%f deg)\n', dirSpan, (dirSpan/pi())*180);

%% Segment Walls
wPointCloud = segmentWalls(pPointCloud);
tmp = printImage(wPointCloud(:,:,3));
axis equal
imwrite(tmp,[dataPath '/MATLAB_output/4_' firstFile '.bmp']);

%% Extract Height Slices
[slices source] = extractHeightSlice(pPointCloud, 125, 80, 10);
tmp = printImage(source(:,:,3));
axis equal
imwrite(tmp,[dataPath '/MATLAB_output/5_' firstFile '.bmp']);

%% Print XZ Plot
pointColor = zeros(size(slices,1),3);
for i = 1:size(slices,1)
    if isnan(slices(i,3))
        pointColor(i,1) = 1;
        pointColor(i,2) = 0;
        pointColor(i,3) = 0;
    else
        pointColor(i,1) = slices(i,3);
        pointColor(i,2) = slices(i,3);
        pointColor(i,3) = slices(i,3);
    end
end

scatter(slices(:,1), slices(:,2), 30, pointColor , 'filled')
axis equal
saveas(gcf,[dataPath '/MATLAB_output/6_' firstFile '.bmp']) 

%% Convert to Cartesian
cwPointCloud = polarToCartesian(wPointCloud);
tmp = printImage(cwPointCloud(:,:,3));
axis equal

%% Export Point Cloud to Meshlab Format
exportMeshlab(cPointCloud, [dataPath '/pointClouds/' firstFile '_pp.ply']);

end
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