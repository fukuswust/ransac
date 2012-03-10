%% Initial Constants
frameName = '000025';

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

%% Segment Mesh
totFloor = 0;
for i = 1:30
    for j = 1:40
        if ~isnan(fPointCloud(i,j,1))
            totFloor = totFloor + 1;
            floorPoints(totFloor,1:3) = permute(fPointCloud(i,j,1:3),[1 3 2]);
        end
    end
end
scatter3(floorPoints(:,1),floorPoints(:,3),floorPoints(:,2));
axis equal

%% Get First Plane Extimate
sFloorPoints = sort(floorPoints,2);
floorModeY = mode(floor(sFloorPoints(:,2)));
totMFloor = 0;
for i = 1:totFloor
    if abs(floorPoints(i,2)-floorModeY) < 5
        totMFloor = totMFloor + 1;
        mFloorPoints(totMFloor,1:3) = floorPoints(i,1:3);
    end
end
scatter3(mFloorPoints(:,1),mFloorPoints(:,2),mFloorPoints(:,3));
axis equal

%% Regress Plane
x = mFloorPoints(:,1);
z = mFloorPoints(:,2);
y = mFloorPoints(:,3);
M = [sum(x.^2) sum(x.*y) sum(x);
     sum(x.*y) sum(y.^2) sum(y);
     sum(x)    sum(y)    totMFloor];
A = [sum(x.*z); sum(y.*z); sum(z)];
V = M\A;
align = [-V(1)/V(3) 1/V(3) -V(2)/V(3)];
align = align/sqrt(sum(align.^2));
if align(2) < 0
    align = -align;
end
upVect = [0 1 0];
alignRotate = getRotationMatrix(align, upVect);
faPointCloud = transform3dMatrix(gPointCloud, alignRotate);
x = faPointCloud(:,:,1);
y = faPointCloud(:,:,2);
z = faPointCloud(:,:,3);
scatter(x(:),y(:));
%printImage(faPointCloud(:,:,2)); %All floor should be black
axis equal


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
%tdView{fileOn+1} = maxDisList;

%% Segment Walls
wPointCloud = segmentWalls(pPointCloud, maxDisListPolar);
tmp = printImage(wPointCloud(:,:,3));
axis equal
imwrite(tmp,[dataPath '/MATLAB_output/4_' frameName '.bmp']);

%% Segment Mesh
% Polar to Cartesian
wCPC = polarToCartesian(wPointCloud);
% Split
totWall = 0;
for i = 1:30
    for j = 1:40
        if ~isnan(wCPC(i,j,1))
            totWall = totWall + 1;
            wallPoints(totWall,1:3) = permute(wCPC(i,j,1:3),[1 3 2]);
        end
    end
end
% Draw Top-Down
scatter(wallPoints(:,1),-wallPoints(:,3));
axis equal

%% Determine Wall Normal
rep = 574;
norm = zeros(rep,3);
for i = 1:rep
    r1 = mod(i,totWall)+1;
    r2 = randi(totWall);
    while (r1 == r2)
        r2 = randi(totWall);
    end
    r3 = randi(totWall);
    while (r3 == r1 || r3 == r2)
        r3 = randi(totWall);
    end
    
    p1 = wallPoints(r1,:);
    p2 = wallPoints(r2,:);
    p3 = wallPoints(r3,:);
    norm(i,1:3) = cross(p1-p2,p1-p3);
    norm(i,1:3) = norm(i,1:3)/(sqrt(sum(norm(i,1:3).^2)));
end
temp = norm(:,1);
hist(temp(:));

%%
end

%% Extract Height Slices
[slices source] = extractHeightSlice(pPointCloud, 125, 80, 10);
tmp = printImage(source(:,:,3));
axis equal
imwrite(tmp,[dataPath '/MATLAB_output/5_' frameName '.bmp']);

%% Print XZ Plot
pointColor = zeros(size(slices,1),3);
for i = 1:size(slices,1)
    if isnan(slices(i,3))
        pointColor(i,1) = 0;
        pointColor(i,2) = 0;
        pointColor(i,3) = 0;
    else
        pointColor(i,1) = 0;
        pointColor(i,2) = 0;
        pointColor(i,3) = 0;
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
        inWallSlice = csvread(fileName);
        newIndex = 1;
        for i=1:40
            if inWallSlice(i,1) ~= -999999.0 ...
                    && inWallSlice(i,2) <= 400
                pTdView{indexOn}(newIndex,1) = -inWallSlice(i,1);
                pTdView{indexOn}(newIndex,2) = inWallSlice(i,2);
                newIndex = newIndex + 1;
            end
        end
        indexOn = indexOn + 1;
    end
end
numData = indexOn - 1;

%% Set Range
curMinRange = 1;
curMaxRange = numData;

%% Convert to Cartesian
for frame = curMinRange:curMaxRange
    cTdView{frame} = polarToCartesian2D(pTdView{frame});
    scatter(cTdView{frame}(:,1), cTdView{frame}(:,2), 30, 'filled', 'blue')
    hold on
    frameName = sprintf('%.6u',frame);
    title(['Frame: ' frameName])
    axis([-250 250 0 500])
    drawnow
    hold off
    %saveas(gcf,[dataPath '/MATLAB_output/9_' frameName '.bmp'])
end

%% 
frame = 123;
cTdView{frame} = polarToCartesian2D(pTdView{frame});
scatter(cTdView{frame}(:,1), cTdView{frame}(:,2), 30, 'filled', 'blue')
hold on
frameName = sprintf('%.6u',frame);
title(['Frame: ' frameName])
axis([-250 250 0 500])
drawnow
hold off

%% Compare 2 Frames
sep = 1;
inSep = 0;
for frame = 8
    scatter(cTdView{frame}(:,1), cTdView{frame}(:,2), 30, 'filled', 'blue')
    hold on
    scatter(cTdView{frame+sep}(:,1), cTdView{frame+sep}(:,2), 30, 'filled', 'red')
    for i = 1:size(cTdView{frame},1)
        if i+inSep > 1 && i+inSep < size(cTdView{frame+sep},1)
        line([cTdView{frame}(i,1) cTdView{frame+sep}(i+inSep,1)], ...
                    [cTdView{frame}(i,2) cTdView{frame+sep}(i+inSep,2)])
        end
    end
    axis([-300 300 0 600])
    hold off
end

%% Determine MSE
sep = 1;
for frame = 8
    inSep = 1;
    nextPos = pTdView{frame+sep};
    trans0 = pTdView{frame};
    trans1 = apply2dTransform(trans0,0,0,0);
    trans2 = apply2dTransform(trans1,0,0,0);
    
    
    determineError(trans2,nextPos,inSep)
    scatter(trans2(:,1), trans2(:,2), 30, 'filled', 'blue')
    hold on
    scatter(nextPos(:,1), nextPos(:,2), 30, 'filled', 'red')
    for i = 1:size(pTdView{frame},1)
        if i+inSep > 1 && i+inSep < size(nextPos,1)
        line([trans2(i,1) nextPos(i+inSep,1)], ...
                    [trans2(i,2) nextPos(i+inSep,2)])
        end
    end
    %axis([-300 300 0 600])
    %axis([-pi() pi() 0 600])
    hold off
end















%% Find Wall Corners
for frame = curMinRange:curMaxRange
[tc fc] = findWallCorners(cTdView{frame}, pTdView{frame});
scatter(cTdView{frame}(:,1), cTdView{frame}(:,2), 30, 'filled', 'black')
hold on
scatter(cTdView{frame}(tc,1), cTdView{frame}(tc,2), 30, 'filled', 'green')
scatter(cTdView{frame}(fc,1), cTdView{frame}(fc,2), 30, 'filled', 'red')
hold off
frameName = sprintf('%.6u',frame);
title(['Frame: ' frameName])
axis([-250 250 0 500])
drawnow
pause(.001)
%saveas(gcf,[dataPath '/MATLAB_output/9_' frameName '.bmp'])
end

%% Show Polar Graph
for frame = curMinRange:curMaxRange
curMaxDis{frame} = 0;
for i=1:size(pTdView{frame},1)
    if ~isnan(pTdView{frame}(i,1))
        if pTdView{frame}(i,2) > curMaxDis{frame}
            curMaxDis{frame} = pTdView{frame}(i,2);
            curMaxDir{frame} = pTdView{frame}(i,1);
        end
    end
end
scatter(pTdView{frame}(:,1), pTdView{frame}(:,2), 30, 'filled', 'blue')
hold on
scatter(curMaxDir{frame}, curMaxDis{frame}, 30, 'filled', 'red')
hold off
frameName = sprintf('%.6u',frame);
title(['Frame: ' frameName])
axis([0.5 2.5 0 500])
drawnow
pause(.001)
%saveas(gcf,[dataPath '/MATLAB_output/10_' frameName '.bmp'])
end

%% Align to Corner
for frame = curMinRange:curMaxRange
cornerX = curMaxDis{frame}*cos(curMaxDir{frame});
cornerZ = curMaxDis{frame}*sin(curMaxDir{frame});
alignedTd{frame}(1:40,1) = cTdView{frame}(:,1) - cornerX;
alignedTd{frame}(1:40,2) = cTdView{frame}(:,2) - cornerZ;
scatter(alignedTd{frame}(:,1), alignedTd{frame}(:,2), 30, 'filled', 'blue')
axis([-300 300 -300 300])
frameName = sprintf('%.6u',frame);
title(['Frame: ' frameName])
drawnow
pause(.001)
%saveas(gcf,[dataPath '/MATLAB_output/10_' frameName '.bmp'])
end

%% Find Polar Rotation from Corner Origin
for frame = curMinRange:curMaxRange
pAlignedTd{frame} = cartesianToPolar2D(alignedTd{frame});
curMaxDis{frame} = 0;
for i=1:40
    if ~isnan(pAlignedTd{frame}(i,1))
        if pAlignedTd{frame}(i,2) > curMaxDis{frame}
            curMaxDis{frame} = pAlignedTd{frame}(i,2);
            curMaxDir{frame} = pAlignedTd{frame}(i,1);
        end
    end
end
scatter(pAlignedTd{frame}(:,1), pAlignedTd{frame}(:,2), 30, 'filled', 'blue')
line([curMaxDir{frame} curMaxDir{frame}],[0 curMaxDis{frame}], 'color', 'red')
axis([-pi() pi() 0 600])
frameName = sprintf('%.6u',frame);
title(['Frame: ' frameName])
drawnow
pause(.001)
%saveas(gcf,[dataPath '/MATLAB_output/11_' frameName '.bmp'])
end

%% Find Polar Rotation
for frame = curMinRange:curMaxRange
finalRot{frame} = apply2dTransform(alignedTd{frame}, -curMaxDir{frame}, 0, 0);
scatter(finalRot{frame}(:,1), finalRot{frame}(:,2), 30, 'filled', 'blue')
axis([-300 300 -300 300])
frameName = sprintf('%.6u',frame);
title(['Frame: ' frameName])
drawnow
pause(.001)
%saveas(gcf,[dataPath '/MATLAB_output/10_' frameName '.bmp'])
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