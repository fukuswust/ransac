%% Initial Constants
frameName = '000012';

%% Loop
lastFile = 33;
tdView = cell(lastFile+1,1);
for fileOn = 0:lastFile
    frameName = sprintf('%.6u',fileOn);

prevTransform = [1 2 3 4 5 6];

%% Import Data
inDepthData = csvread([dataPath '/depthData/' frameName '.csv']);
inRColorData = csvread([dataPath '/colorData/' frameName '_R.csv']);
inGColorData = csvread([dataPath '/colorData/' frameName '_G.csv']);
inBColorData = csvread([dataPath '/colorData/' frameName '_B.csv']);
inAccelData = transpose(csvread([dataPath '/accelData/' frameName '.csv']));

%% Combine Color Values
colorData(:,:,1) = inRColorData(:,:)./256;
colorData(:,:,2) = inGColorData(:,:)./256;
colorData(:,:,3) = inBColorData(:,:)./256;
image(colorData);
axis equal
%imwrite(colorData,[dataPath '/colorImages/' frameName '.bmp']);

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
imwrite(tmp,[dataPath '/MATLAB_output/0_' frameName '.bmp']);

%% Align to Gravity
upVect = [0 1 0];
gravityRotate = getRotationMatrix(inAccelData, upVect);
gPointCloud = transform3dMatrix(cPointCloud, gravityRotate);
tmp = printImage(gPointCloud(:,:,2)); %All floor should be black
axis equal
imwrite(tmp,[dataPath '/MATLAB_output/1_' frameName '.bmp']);

%% Segment Floor
fPointCloud = segmentFloor(gPointCloud);
tmp = printImage(fPointCloud(:,:,2)); %All floor should be black
axis equal
imwrite(tmp,[dataPath '/MATLAB_output/2_' frameName '.bmp']);
floorHeight = min(min(gPointCloud(:,:,1)));
fprintf('Height: %f cm\n', floorHeight);

%% Convert to Polar
pPointCloud = cartesianToPolar(gPointCloud);
tmp = printImage(pPointCloud(:,:,3));
axis equal
imwrite(tmp,[dataPath '/MATLAB_output/3_' frameName '.bmp']);

%% Get Max Distances for Each Direction Slice
maxDisListPolar = getMaxDisPerDir(pPointCloud,40);
minDir = min(min(pPointCloud(:,:,2)));
fprintf('Min Dir: %f rad\n', minDir);
maxDir = max(max(pPointCloud(:,:,2)));
fprintf('Max Dir: %f rad\n', maxDir);
dirSpan = maxDir - minDir;
fprintf('Dir Span: %f rad (%f deg)\n', dirSpan, (dirSpan/pi())*180);

%% Change MaxDisPerDir List to Cartesian
maxDisList = polarToCartesian2D(maxDisListPolar);
maxDisList(:,2) = -maxDisList(:,2);
scatter(maxDisList(:,1), maxDisList(:,2), 30, 'filled')
axis equal
%saveas(gcf,[dataPath '/MATLAB_output/32_' frameName '.bmp']) 
tdView{fileOn+1} = maxDisList;

end

%% Fit Lines to Walls
lines = fitWallsToLines(maxDisList);
scatter(maxDisList(:,1), maxDisList(:,2), 30, 'filled')
t = 2;
line([-20 20], [((lines(t,1)*-20)+lines(t,2)) ...
    ((lines(t,1)*20)+lines(t,2))])
axis equal
saveas(gcf,[dataPath '/MATLAB_output/34_' frameName '.bmp'])

%% Segment Walls
wPointCloud = segmentWalls(pPointCloud, maxDisListPolar);
tmp = printImage(wPointCloud(:,:,3));
axis equal
imwrite(tmp,[dataPath '/MATLAB_output/4_' frameName '.bmp']);

%% Extract Height Slices
[slices source] = extractHeightSlice(pPointCloud, 125, 80, 10);
tmp = printImage(source(:,:,3));
axis equal
imwrite(tmp,[dataPath '/MATLAB_output/5_' frameName '.bmp']);

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
saveas(gcf,[dataPath '/MATLAB_output/6_' frameName '.bmp']) 

%% Convert to Cartesian
cwPointCloud = polarToCartesian(wPointCloud);
tmp = printImage(cwPointCloud(:,:,3));
axis equal

%% Export Point Cloud to Meshlab Format
exportMeshlab(fPointCloud, [dataPath '/pointClouds/' frameName '_fa.ply']);



%% Load Slice Data
firstSliceFile = 0;
lastSliceFile = 2500;
indexOn = 1;
for fileOn=firstSliceFile:3:lastSliceFile
    frameName = sprintf('%.6u',fileOn);
    fileName = [dataPath '/../sliceData/' frameName '.csv'];
    if exist(fileName, 'file')
        inWallSlice{indexOn} = csvread(fileName);
        for i=1:40
            if inWallSlice{indexOn}(i,1) == -999999.0
                wallSlice{indexOn}(i,1) = NaN;
                wallSlice{indexOn}(i,2) = NaN;
            end
        end
        indexOn = indexOn + 1;
    end
end
numData = indexOn - 1;

%% Convert to Cartesian
for frame = 1:numData
pTdView{frame} = wallSlice{frame};
pTdView{frame}(:,1) = -pTdView{frame}(:,1);
cTdView{frame} = polarToCartesian2D(pTdView{frame});
scatter(cTdView{frame}(:,1), cTdView{frame}(:,2), 30, 'filled', 'blue')
axis([-300 300 0 600])
drawnow
pause(.001)
%saveas(gcf,[dataPath '/MATLAB_output/9_' frameName '.bmp'])
end

%% Show Polar Graph
for frame = 1:numData
scatter(pTdView{frame}(:,1), pTdView{frame}(:,2), 30, 'filled', 'blue')
axis([0.5 2.5 0 600])
drawnow
pause(.001)
%saveas(gcf,[dataPath '/MATLAB_output/10_' frameName '.bmp'])
end

%% Find Corner
for frame = 1:numData
curMaxDis = 0;
for i=1:40
    if ~isnan(pTdView{frame}(i,1))
        if pTdView{frame}(i,2) > curMaxDis
            curMaxDis = pTdView{frame}(i,2);
            curMaxDir = pTdView{frame}(i,1);
        end
    end
end
cornerX = curMaxDis*cos(curMaxDir);
cornerZ = curMaxDis*sin(curMaxDir);
alignedTd{frame}(1:40,1) = cTdView{frame}(:,1) - cornerX;
alignedTd{frame}(1:40,2) = cTdView{frame}(:,2) - cornerZ;
scatter(alignedTd{frame}(:,1), alignedTd{frame}(:,2), 30, 'filled', 'blue')
axis([-300 300 -300 300])
drawnow
pause(.001)
%saveas(gcf,[dataPath '/MATLAB_output/10_' frameName '.bmp'])
end

%% Find Rotation in Polar
for frame = 1:numData
pAlignedTd{frame} = cartesianToPolar2D(alignedTd{frame});
scatter(pAlignedTd{frame}(:,1), pAlignedTd{frame}(:,2), 30, 'filled', 'blue')
axis([-pi() pi() 0 600])
drawnow
pause(.001)
%saveas(gcf,[dataPath '/MATLAB_output/11_' frameName '.bmp'])
end

%% Show Nearest Neighbors
frameName = sprintf('%.6u',frame1);
matches = nearestNeighbor(cTdView1, cTdView2);
scatter(cTdView1(:,1), cTdView1(:,2), 30, 'filled', 'blue')
hold on
scatter(cTdView2(:,1), cTdView2(:,2), 30, 'filled', 'red')
for i = 1:40
    if ~isnan(matches(i))
        line([cTdView1(i,1) cTdView2(matches(i),1)], ...
            [cTdView1(i,2) cTdView2(matches(i),2)])
    end
end
axis([-300 300 0 600])
hold off
saveas(gcf,[dataPath '/MATLAB_output/7_' frameName '.bmp'])