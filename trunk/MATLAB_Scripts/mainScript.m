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

%% Top Down Alignment Initial Conditions
frame1 = 19;

%% Top Down Alignment Loop
for frame1 = 1:lastFile

%% Convert to Polar
pTdView1 = cartesianToPolar2D(tdView{frame1});
pTdView2 = cartesianToPolar2D(tdView{frame1+1});
scatter(pTdView1(:,1), pTdView1(:,2), 30, 'filled', 'blue')
hold on
scatter(pTdView2(:,1), pTdView2(:,2), 30, 'filled', 'red')
hold off
axis([0.5 2.5 0 600])
saveas(gcf,[dataPath '/MATLAB_output/9_' frameName '.bmp'])

%% Translate Cartesian
curMaxDis1 = 0;
curMaxDis2 = 0;
for i=1:40
    if ~isnan(pTdView1(i,1))
        if pTdView1(i,2) > curMaxDis1
            curMaxDis1 = pTdView1(i,2);
            curMaxDir1 = pTdView1(i,1);
        end
    end
    if ~isnan(pTdView2(i,1))
        if pTdView2(i,2) > curMaxDis2
            curMaxDis2 = pTdView2(i,2);
            curMaxDir2 = pTdView2(i,1);
        end
    end
end
corner1X = curMaxDis1*cos(curMaxDir1);
corner1Z = curMaxDis1*sin(curMaxDir1);
corner2X = curMaxDis2*cos(curMaxDir2);
corner2Z = curMaxDis2*sin(curMaxDir2);
delX = corner2X - corner1X;
delZ = corner2Z - corner1Z;
alignedTd1(1:40,1) = tdView{frame1}(:,1) + delX - corner2X;
alignedTd1(1:40,2) = tdView{frame1}(:,2) + delZ - corner2Z;
alignedTd2(1:40,1) = tdView{frame1+1}(:,1) - corner2X;
alignedTd2(1:40,2) = tdView{frame1+1}(:,2) - corner2Z;
scatter(alignedTd1(:,1), alignedTd1(:,2), 30, 'filled', 'blue')
hold on
scatter(alignedTd2(:,1), alignedTd2(:,2), 30, 'filled', 'red')
hold off
axis([-300 300 -300 300])
saveas(gcf,[dataPath '/MATLAB_output/10_' frameName '.bmp'])

%% Find Rotation in Polar
pAlignedTd1 = cartesianToPolar2D(alignedTd1);
pAlignedTd2 = cartesianToPolar2D(alignedTd2);
scatter(pAlignedTd1(:,1), pAlignedTd1(:,2), 30, 'filled', 'blue')
hold on
scatter(pAlignedTd2(:,1), pAlignedTd2(:,2), 30, 'filled', 'red')
hold off
axis([-pi() pi() 0 600])
saveas(gcf,[dataPath '/MATLAB_output/11_' frameName '.bmp'])

%% Rotate
curMaxDis1 = 0;
curMaxDis2 = 0;
for i=1:40
    if ~isnan(pAlignedTd1(i,1))
        if pAlignedTd1(i,2) > curMaxDis1
            curMaxDis1 = pAlignedTd1(i,2);
            curMaxDir1 = pAlignedTd1(i,1);
        end
    end
    if ~isnan(pAlignedTd2(i,1))
        if pAlignedTd2(i,2) > curMaxDis2
            curMaxDis2 = pAlignedTd2(i,2);
            curMaxDir2 = pAlignedTd2(i,1);
        end
    end
end
delDir = curMaxDir2 - curMaxDir1;
finalRot = apply2dTransform(alignedTd1, delDir, 0, 0);
final = apply2dTransform(finalRot, 0, corner2X, corner2Z);
scatter(final(:,1), final(:,2), 30, 'filled', 'blue')
hold on
scatter(tdView{frame1+1}(:,1), tdView{frame1+1}(:,2), 30, 'filled', 'red')
axis([-300 300 0 600])
hold off
saveas(gcf,[dataPath '/MATLAB_output/12_' frameName '.bmp'])

end

%% Show Nearest Neighbors
frameName = sprintf('%.6u',frame1);
matches = nearestNeighbor(tdView{frame1}, tdView{frame1+1});
scatter(tdView{frame1}(:,1), tdView{frame1}(:,2), 30, 'filled', 'blue')
hold on
scatter(tdView{frame1+1}(:,1), tdView{frame1+1}(:,2), 30, 'filled', 'red')
for i = 1:40
    if ~isnan(matches(i))
        line([tdView{frame1}(i,1) tdView{frame1+1}(matches(i),1)], ...
            [tdView{frame1}(i,2) tdView{frame1+1}(matches(i),2)])
    end
end
axis([-300 300 0 600])
hold off
saveas(gcf,[dataPath '/MATLAB_output/7_' frameName '.bmp'])